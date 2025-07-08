#include "Parser.hpp"

#include <util/string.hpp>
#include <algorithm>

Parser::Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : context(_context), arch(_arch), bits(_bits), endianness(_endianness)
{

}

void Parser::Parse(std::istream& input)
{
    tokens = tokenizer.tokenize(input);
}

void Parser::Print()
{
    std::cout << "Tokens: " << std::endl;
    tokenizer.print(tokens, "\t");
}
