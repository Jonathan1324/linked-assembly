#include "Parser.hpp"

#include <util/string.hpp>
#include <algorithm>

Parser::Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : context(_context), arch(_arch), bits(_bits), endianness(_endianness)
{

}

void Parser::Print()
{
    // TODO
}
