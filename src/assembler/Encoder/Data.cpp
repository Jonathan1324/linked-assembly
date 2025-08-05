#include "Encoder.hpp"

std::vector<uint8_t> Encoder::Encoder::_EncodeData(const Parser::DataDefinition& dataDefinition)
{
    // TODO: placeholder implementation
    if(!dataDefinition.reserved)
    {
        for (const auto& value : dataDefinition.values)
        {
            if (value.operands.empty())
                throw Exception::SemanticError("Data definition cannot be empty", dataDefinition.lineNumber, dataDefinition.column);

            Int128 evaluatedValue = Evaluate(value);
        }
        size_t size = dataDefinition.size * dataDefinition.values.size();
        std::vector<uint8_t> buffer(size, 0x00);
        return buffer;
    }
    else
    {
        throw Exception::InternalError("Reserved data encoding is not implemented yet", dataDefinition.lineNumber, dataDefinition.column);
    }
}

uint64_t Encoder::Encoder::_GetSize(const Parser::DataDefinition& dataDefinition)
{
    // TODO: placeholder implementation
    if(!dataDefinition.reserved)
    {
        for (const auto& value : dataDefinition.values)
        {
            if (value.operands.empty())
                throw Exception::SemanticError("Data definition cannot be empty", dataDefinition.lineNumber, dataDefinition.column);

            Int128 evaluatedValue = Evaluate(value);
        }
        size_t size = dataDefinition.size * dataDefinition.values.size();
        return size;
    }
    else
    {
        throw Exception::InternalError("Reserved data encoding is not implemented yet", dataDefinition.lineNumber, dataDefinition.column);
    }
}