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

#include "x86/Parser.hpp"

Parser* getParser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
{
    return new x86::Parser(_context, _arch, _bits, _endianness);
}