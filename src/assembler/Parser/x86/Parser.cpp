#include "Parser.hpp"

x86::Parser::Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : ::Parser(_context, _arch, _bits, _endianness)
{

}

void x86::Parser::Parse(const std::vector<Token::Token>& tokens)
{
    std::vector<Token::Token> filteredTokens;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token::Token& token = tokens[i];
        
        if (token.type == Token::Type::Punctuation && token.value.at(0) == ';')
        {
            while (i < tokens.size() && tokens[i].type != Token::Type::EOL && tokens[i].type != Token::Type::_EOF)
                i++;
            continue;
        }

        filteredTokens.push_back(token);
    }

    // TODO: IDK
}