#include "Encoder.hpp"

#include "ShuntingYard.hpp"

Encoder::Evaluation Encoder::Encoder::Evaluate(const Parser::Immediate& immediate, uint64_t bytesWritten, uint64_t sectionOffset) const
{
    // substitute position with two different values:
    // if both are equal:                               position doesn't matter
    // if both are equal when subtracting position:     can be written using offset + position (relocation)
    // else:                                            not even relocation is possible
    std::vector<ShuntingYard::Token> tokens = ShuntingYard::prepareTokens(immediate.operands, labels, constants, bytesWritten, sectionOffset);
    Int128 result = ShuntingYard::evaluate(tokens);

    Evaluation evaluation;
    evaluation.result = result;
    evaluation.relocationPossible = false;
    evaluation.offset = 0;
    return evaluation;
}