#include "Tokenizer.hpp"

#include <Exception.hpp>

using namespace Token;

Tokenizer::Tokenizer() {
	
}

void Tokenizer::clear() {
    tokens.clear();
}

void Tokenizer::tokenize(std::istream& input)
{
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(input, line))
    {
        lineNumber++;
        size_t pos = 0;
        size_t length = line.size();

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
                    pos + 1
                );
                pos++;
            }
            // ; or :
            else if (line[pos] == ';' || line[pos] == ':')
            {
                tokens.emplace_back(Type::Punctuation, std::string() + line[pos], lineNumber, pos);
                pos++;
            }
            // %
            else if (line[pos] == '%')
            {
                tokens.emplace_back(Type::Macro, "", lineNumber, pos);
                pos++;
            }

            // +,-,*,/
            else if (line[pos] == '+' || line[pos] == '-' || line[pos] == '*' || line[pos] == '/')
            {
                tokens.emplace_back(Type::Operator, std::string() + line[pos], lineNumber, pos);
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
                    pos + 1
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

                tokens.emplace_back(Type::String, value, lineNumber, startPos);

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

                tokens.emplace_back(Type::Character, std::string() + value, lineNumber, startPos);

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
                       line[pos] != '+' && line[pos] != '-' && line[pos] != '*' && line[pos] != '/')
                    pos++;
                
                tokens.emplace_back(
                    Type::Token,
                    line.substr(startPos, pos - startPos),
                    lineNumber,
                    startPos + 1
                );
            }
        }

        tokens.emplace_back(
            Type::EOL,
            "",
            lineNumber,
            length + 1
        );
    }
    tokens.emplace_back(
        Type::_EOF,
        "",
        lineNumber + 1,
        0
    );
}

std::vector<Token::Token> Tokenizer::getTokens()
{
    return tokens;
}

void Tokenizer::print()
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
        case Type::Macro:
        case Type::EOL:
            result += " in line " + std::to_string(line) + " at column " + std::to_string(column);
            break;

        case Type::_EOF:
            result += " in line " + std::to_string(line);
            break;

        case Type::Character:
        {
            char buf[80];
            unsigned char c = static_cast<unsigned char>(value[0]);
            snprintf(buf, sizeof(buf), " 0x%02x ('%c') in line %zu at column %zu", c, value[0], line, column);
            result += buf;
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