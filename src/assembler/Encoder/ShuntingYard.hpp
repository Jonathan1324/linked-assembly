#pragma once

#include <IntTypes.h>
#include <string>
#include "Encoder.hpp"

namespace ShuntingYard
{
    struct Token
    {
        enum class Type { Number, Operator, Position };
        Type type;

        Int128 number;
        std::string op;
        uint64_t offset;
        bool negative = false;

        Token(Int128 n) : type(Type::Number), number(n) {}
        Token(const std::string& o) : type(Type::Operator), op(o) {}
        Token() {}
    };

    struct PreparedTokens {
        std::vector<Token> tokens;
        bool relocationPossible;
        std::string usedSection;
        bool isExtern = false;
    };

    PreparedTokens prepareTokens(
        const std::vector<Parser::ImmediateOperand>& operands,
        std::unordered_map<std::string, Encoder::Label>& labels,
        const std::unordered_map<std::string, Encoder::Constant>& constants,
        uint64_t bytesWritten,
        uint64_t sectionOffset,
        const std::string* currentSection
    );

    Int128 evaluate(const std::vector<Token>& tokens, uint64_t offset);
}