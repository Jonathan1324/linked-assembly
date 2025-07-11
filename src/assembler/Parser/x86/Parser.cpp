#include "Parser.hpp"

x86::Parser::Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : ::Parser(_context, _arch, _bits, _endianness)
{

}

void x86::Parser::Parse(const std::vector<Token::Token>& tokens)
{
    std::vector<Token::Token> filteredTokens;
    Token::Type before = Token::Type::_EOF;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token::Token& token = tokens[i];

        if (token.type == Token::Type::EOL && before == Token::Type::EOL)
            continue;
        
        if (token.type == Token::Type::Punctuation && token.value.at(0) == ';')
        {
            while (i < tokens.size() && tokens[i].type != Token::Type::EOL && tokens[i].type != Token::Type::_EOF)
                i++;
            if (i < tokens.size())
                filteredTokens.push_back(tokens[i]);
            continue;
        }

        before = token.type;
        filteredTokens.push_back(token);
    }

    std::cout << "filteredTokens: " << std::endl;

    for (size_t i = 0; i < filteredTokens.size(); i++)
    {
        const Token::Token& token = filteredTokens[i];

        // Macros and constants
        // ('%')
        if (token.type == Token::Type::Macro)
        {
            //TODO
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL && filteredTokens[i].type != Token::Type::_EOF)
                i++;
            continue;
        }

        std::cout << "  ";
        token.print();
        std::cout << std::endl;
    }
}