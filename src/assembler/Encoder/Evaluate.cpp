#include "Encoder.hpp"

#include "ShuntingYard.hpp"

std::string int128_to_string(__int128 value) {
    if (value == 0) return "0";
    bool negative = value < 0;
    if (negative) value = -value;

    std::string result;
    while (value > 0) {
        result += '0' + (value % 10);
        value /= 10;
    }
    if (negative) result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

Encoder::Evaluation Encoder::Encoder::Evaluate(const Parser::Immediate& immediate, uint64_t bytesWritten, uint64_t sectionOffset, const std::string* curSection) const
{
    // substitute position with two different values:
    // if both are equal:                               position doesn't matter
    // if both are equal when subtracting position:     can be written using offset + position (relocation)
    // else:                                            not even relocation is possible
    ShuntingYard::PreparedTokens tokens = ShuntingYard::prepareTokens(immediate.operands, labels, constants, bytesWritten, sectionOffset, curSection);

    if (tokens.relocationPossible)
    {
        uint64_t off1 = bytesWritten - sectionOffset; //FIXME: should also work when usedSection is different
        uint64_t off2 = off1 + 0x12345678;

        Int128 res1 = ShuntingYard::evaluate(tokens.tokens, off1);
        Int128 res2 = ShuntingYard::evaluate(tokens.tokens, off2);

        Evaluation evaluation;
        evaluation.result = res1;
        evaluation.usedSection = tokens.usedSection;
        if (res1 == res2)
        {
            evaluation.useOffset = false;
            evaluation.relocationPossible = true;
            evaluation.offset = 0;
        }
        else if ((res1 - off1) == (res2 - off2))
        {
            evaluation.useOffset = true;
            evaluation.relocationPossible = true;
            evaluation.offset = res1 - off1;
        }
        else
        {
            evaluation.useOffset = false;
            evaluation.relocationPossible = false;
            evaluation.offset = 0;
        }
        return evaluation;
    }
    else
    {
        Int128 result = ShuntingYard::evaluate(tokens.tokens, bytesWritten - sectionOffset);

        Evaluation evaluation;
        evaluation.result = result;
        evaluation.usedSection = tokens.usedSection;
        evaluation.useOffset = false;
        evaluation.relocationPossible = false;
        evaluation.offset = 0;
        return evaluation;
    }
}