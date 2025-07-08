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

    void Parse(std::istream& input);

    void Print();

private:
    Context context;
    Architecture arch;
    BitMode bits;
    Endianness endianness;

    Token::Tokenizer tokenizer;
    std::vector<Token::Token> tokens;
};