#pragma once

#include "../Parser.hpp"

namespace Parser
{
    namespace x86
    {
        class Parser : public ::Parser::Parser
        {
        public:
            Parser(const Context& _context, Architecture _arch, BitMode _bits);
            ~Parser() = default;
            
            void Parse(const std::vector<Token::Token>& tokens) override;
        };
    }
}