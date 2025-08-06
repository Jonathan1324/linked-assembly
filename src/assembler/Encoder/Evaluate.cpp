#include "Encoder.hpp"

#include "ShuntingYard.hpp"

Int128 Encoder::Encoder::Evaluate(const Parser::Immediate& immediate, uint64_t bytesWritten, uint64_t sectionOffset) const
{
    std::vector<ShuntingYard::Token> tokens = ShuntingYard::prepareTokens(immediate.operands, labels, constants, bytesWritten, sectionOffset);
    Int128 result = ShuntingYard::evaluate(tokens);
    return result;
}