#include "Tokenizer.hpp"

using namespace Token;

Tokenizer::Tokenizer() {
	
}

std::vector<Token::Token> Tokenizer::tokenize(std::istream& input)
{
    std::vector<Token> tokens;
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(input, line))
    {
        lineNumber++;
        size_t pos = 0;
        size_t length = line.size();

        while (pos < length)
        {
            while (pos < length && std::isspace(static_cast<unsigned char>(line[pos])))
            {
                ++pos;
            }
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
                ++pos;
            }
            else
            {
                while (pos < length &&
                    !std::isspace(static_cast<unsigned char>(line[pos])) &&
                    line[pos] != ',')
                {
                    ++pos;
                }
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

    return tokens;
}

void Tokenizer::print(std::vector<Token> tokens, const char* indent)
{
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token& token = tokens[i];
        std::cout << indent;
        token.print();
        std::cout << '\n';
    }
}