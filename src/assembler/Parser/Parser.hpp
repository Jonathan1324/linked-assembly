#pragma once

#include <fstream>
#include <Architecture.hpp>
#include "../Context.hpp"

class Parser
{
public:
    Parser(std::istream& _input, Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);

private:
    std::istream& input;
    Context context;
    
    Architecture arch;
    BitMode bits;
    Endianness endianness;
};