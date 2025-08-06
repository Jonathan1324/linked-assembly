#pragma once

#include <IntTypes.h>
#include <string>
#include "Encoder.hpp"

namespace ShuntingYard
{
    struct Token
    {
        enum class Type { Number, Operator };
        Type type;

        Int128 number;
        std::string op;

        Token(Int128 n) : type(Type::Number), number(n) {}
        Token(const std::string& o) : type(Type::Operator), op(o) {}
    };

    std::vector<Token> prepareTokens(
        const std::vector<Parser::ImmediateOperand>& operands,
        const std::unordered_map<std::string, Encoder::Label>& labels,
        const std::unordered_map<std::string, Encoder::Constant>& constants,
        uint64_t bytesWritten,
        uint64_t sectionOffset
    );

    Int128 evaluate(const std::vector<Token>& tokens);
}