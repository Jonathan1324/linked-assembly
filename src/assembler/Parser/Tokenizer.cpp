#include "Tokenizer.hpp"

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
            else if (line[pos] == '"')
            {
                // String literal
                pos++; // skip opening "
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

                            default: pos++;
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
            // TODO: comments ';' or '#'
            else
            {
                while (pos < length &&
                       !std::isspace(static_cast<unsigned char>(line[pos])) &&
                       line[pos] != ',')
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
        std::cout << "  ";
        token.print();
        std::cout << std::endl;
    }
}