#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <Architecture.hpp>
#include "../Context.hpp"
#include "Tokenizer.hpp"

class Parser
{
public:
    Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);
    virtual ~Parser() = default;

    virtual void Parse(std::vector<Token::Token> tokens) = 0;
    void Print();

protected:
    Context context;
    Architecture arch;
    BitMode bits;
    Endianness endianness;
};