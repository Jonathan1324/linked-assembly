#pragma once

#include "../Parser.hpp"
#include <x86/Registers.hpp>

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
        
        protected:
            Instruction::Register getReg(const Token::Token& token);
        };

        inline Instruction::Register Parser::getReg(const Token::Token& token)
        {
            Instruction::Register reg;
            auto it = ::x86::registers.find(token.value);
            if (it == ::x86::registers.end()) throw Exception::InternalError("Unknown register: " + token.value, token.line, token.column);
            reg.reg = it->second;
            return reg;
        }
    }
}