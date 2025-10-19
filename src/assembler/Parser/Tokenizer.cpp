#include "Tokenizer.hpp"

#include <Exception.hpp>
#include <util/string.hpp>
#include <cstdint>

Token::Tokenizer::Tokenizer(const Context& _context)
    : context(&_context)
{
	
}

void Token::Tokenizer::clear() {
    tokens.clear();
}

void Token::Tokenizer::tokenize(std::istream* input)
{
    uint64_t file;
    std::string line;
    size_t lineNumber = 0;
    size_t lineIncrease = 1;

    while (std::getline(*input, line))
    {
        lineNumber += lineIncrease;
        size_t pos = 0;
        size_t length = line.size();
        std::string trimmed = trim(line);

        if (trimmed.find("%line") == 0)
        {
            std::istringstream iss(trimmed.substr(5));
            std::string token;
            
            if (std::getline(iss, token, '+')) {
                lineNumber = std::stoul(trim(token)) - 1;
            }
            if (std::getline(iss, token, ' ')) {
                lineIncrease = std::stoul(trim(token));
            }
            std::string filename;
            if (iss >> filename) {
                if (filename == "-") {
                    // TODO: mainfile
                    filename = "";
                }
            }
            // TODO: stringPool

            // TODO: parse (including when seeing '-' as filename to put the main file there)
            continue;
        }

        while (pos < length)
        {
            // Skip whitespace
            while (pos < length && std::isspace(static_cast<unsigned char>(line[pos])))
                pos++;
            if (pos >= length) break;

            size_t startPos = pos;

            // ,
            if (line[pos] == ',')
            {
                tokens.emplace_back(
                    Type::Comma,
                    ",",
                    lineNumber,
                    pos + 1,
                    file
                );
                pos++;
            }
            // ; or :
            else if (line[pos] == ';' || line[pos] == ':')
            {
                tokens.emplace_back(Type::Punctuation, std::string() + line[pos], lineNumber, pos, file);
                pos++;
            }

            // +,-,*,/
            else if (line[pos] == '+' || line[pos] == '-' || line[pos] == '*' || line[pos] == '/' || line[pos] == '%')
            {
                tokens.emplace_back(Type::Operator, std::string() + line[pos], lineNumber, pos, file);
                pos++;
            }

            // Bracket
            else if (line[pos] == '(' || line[pos] == ')' ||
                     line[pos] == '[' || line[pos] == ']' ||
                     line[pos] == '{' || line[pos] == '}')
            {
                tokens.emplace_back(
                    Type::Bracket,
                    std::string(1, line[pos]),
                    lineNumber,
                    pos + 1,
                    file
                );
                pos++;
            }

            // Strings
            else if (line[pos] == '"')
            {
                pos++;  // skip opening "
                startPos = pos;
                std::string value;
                while (pos < length)
                {
                    if (line[pos] == '\\')
                    {
                        pos++;
                        switch(line[pos])
                        {
                            case '\\': value.push_back('\\'); pos++; break;
                            case '"': value.push_back('"'); pos++; break;
                            case '\'': value = '\''; pos++; break;
                            case 'n': value = '\n'; pos++; break;
                            // TODO: add more

                            default: throw Exception::SyntaxError("Unknown escape character", lineNumber, pos);
                        }
                    }
                    else if (line[pos] == '"')
                    {
                        break;
                    }
                    else
                    {
                        value.push_back(line[pos]);
                        pos++;
                    }
                }

                tokens.emplace_back(Type::String, value, lineNumber, startPos, file);

                if (pos < length && line[pos] == '"')
                    pos++; // skip closing "
            }
            // Characters
            else if (line[pos] == '\'')
            {
                pos++;  // skip opening '
                startPos = pos;
                char value;
                if (line[pos] == '\\')
                {
                    pos++;
                    if (pos >= line.length())
                        throw Exception::SyntaxError("Unexpected end of line after escape character", lineNumber, pos);

                    // TODO: one function only
                    switch(line[pos])
                    {
                        case '\\': value = '\\'; break;
                        case '"': value = '"'; break;
                        case '\'': value = '\''; break;
                        case 'n': value = '\n';break;
                        // TODO: add more

                        default: throw Exception::SyntaxError("Unknown escape character", lineNumber, pos);
                    }
                }
                else
                {
                    value = line[pos];
                }

                pos++;

                if (pos >= line.length() || line[pos] != '\'')
                {
                    throw Exception::SyntaxError("Expected closing '", lineNumber, pos);
                }

                tokens.emplace_back(Type::Character, std::string() + value, lineNumber, startPos, file);

                pos++; // skip closing '
            }

            // Everything else
            else
            {
                while (pos < length &&
                       !std::isspace(static_cast<unsigned char>(line[pos])) &&
                       line[pos] != ',' && line[pos] != ';' && line[pos] != ':' &&
                       line[pos] != '(' && line[pos] != ')' && line[pos] != '[' &&
                       line[pos] != ']' && line[pos] != '{' && line[pos] != '}' &&
                       line[pos] != '"' && line[pos] != '\'' &&
                       line[pos] != '+' && line[pos] != '-' && line[pos] != '*' && line[pos] != '/' && line[pos] != '%')
                    pos++;
                
                tokens.emplace_back(
                    Type::Token,
                    line.substr(startPos, pos - startPos),
                    lineNumber,
                    startPos + 1,
                    file
                );
            }
        }

        tokens.emplace_back(
            Type::EOL,
            "",
            lineNumber,
            length + 1,
            file
        );
    }
    tokens.emplace_back(
        Type::_EOF,
        "",
        lineNumber + 1,
        0,
        file
    );
}

std::vector<Token::Token> Token::Tokenizer::getTokens()
{
    return tokens;
}

void Token::Tokenizer::print()
{
    std::cout << "Tokens: " << std::endl;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token& token = tokens[i];
        std::cout << "  " << token.what() << std::endl;
    }
}

std::string Token::Token::what() const
{
    std::string result = std::string("Token (Type=") + to_string(type) + ")";

    switch (type)
    {
        case Type::Comma:
        case Type::EOL:
            result += " in line " + std::to_string(line) + " at column " + std::to_string(column);
            break;

        case Type::_EOF:
            result += " in line " + std::to_string(line);
            break;

        case Type::Character:
        {
            unsigned char c = static_cast<unsigned char>(value[0]);
            result += std::to_string(c) + "in line " + std::to_string(line) + " at column " + std::to_string(column);
            break;
        }

        case Type::Token:
        case Type::String:
        case Type::Bracket:
        case Type::Punctuation:
        default:
            result += " '" + value + "' in line " + std::to_string(line) + " at column " + std::to_string(column);
            break;
    }

    return result;
}