#include "Encoder.hpp"

enum class Operator
{
    NONE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    AND,
    OR,
    XOR,
    SHL,
    SHR
};

void calculate(uint64_t& result, uint64_t value, Operator op)
{
    switch (op)
    {
        case Operator::ADD:
            result += value;
            break;
        case Operator::SUB:
            result -= value;
            break;
        case Operator::MUL:
            result *= value;
            break;
        case Operator::DIV:
            if (value == 0)
                throw Exception::SemanticError("Division by zero", -1, -1);
            result /= value;
            break;
        case Operator::MOD:
            if (value == 0)
                throw Exception::SemanticError("Modulo by zero", -1, -1);
            result %= value;
            break;
        case Operator::AND:
            result &= value;
            break;
        case Operator::OR:
            result |= value;
            break;
        case Operator::XOR:
            result ^= value;
            break;
        case Operator::SHL:
            result <<= value;
            break;
        case Operator::SHR:
            result >>= value;
            break;
        case Operator::NONE:
            result = value;
            break;
        
        
        default:
            throw Exception::SemanticError("Unknown operator", -1, -1);
    }
}

uint64_t Encoder::Encoder::Evaluate(const Parser::Immediate& immediate) const
{
    uint64_t result = 0;
    Operator currentOp = Operator::NONE;

    for (const auto& operand : immediate.operands)
    {
        if (std::holds_alternative<Parser::Integer>(operand))
        {
            const Parser::Integer& integer = std::get<Parser::Integer>(operand);
            if (integer.isString)
            {
                // TODO
                uint64_t value = std::stoull(integer.value, nullptr, 0);
                calculate(result, value, currentOp);
                currentOp = Operator::NONE;
            }
            else
            {
                calculate(result, integer.val, currentOp);
                currentOp = Operator::NONE;
            }
        }
        else if (std::holds_alternative<Parser::Operator>(operand))
        {
            const Parser::Operator& op = std::get<Parser::Operator>(operand);
            // TODO
        }
        else if (std::holds_alternative<Parser::String>(operand))
        {
            const Parser::String& str = std::get<Parser::String>(operand);
            if (constants.find(str.value) != constants.end())
            {
                calculate(result, constants.at(str.value), currentOp);
                currentOp = Operator::NONE;
            }
            else if (labels.find(str.value) != labels.end())
            {
                const Label& label = labels.at(str.value);
                calculate(result, label.offset, currentOp);
                currentOp = Operator::NONE;
            }
            else
                throw Exception::UndefinedSymbol("Undefined constant or label: " + str.value, -1, -1);
        }
        else if (std::holds_alternative<Parser::CurrentPosition>(operand))
        {
            const Parser::CurrentPosition& pos = std::get<Parser::CurrentPosition>(operand);
            if (pos.sectionPos)
                return sectionOffset;
            else
                return bytesWritten;
        }
    }
    return result;
}