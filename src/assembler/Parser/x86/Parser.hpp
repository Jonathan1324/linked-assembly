#pragma once

#include "../Parser.hpp"

namespace x86
{
    class Parser : public ::Parser
    {
    public:
        void Parse(std::vector<Token::Token> tokens) override;
    };
}