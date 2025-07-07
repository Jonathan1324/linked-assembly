#include "Parser.hpp"

Parser::Parser(std::istream& _input, Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : input(_input), context(_context), arch(_arch), bits(_bits), endianness(_endianness)
{

}