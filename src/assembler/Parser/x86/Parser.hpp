#pragma once

#include "../Parser.hpp"

namespace x86
{
    class Parser : public ::Parser
    {
    public:
        Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);
        
        void Parse(const std::vector<Token::Token>& tokens) override;
    };
}