#include "Encoder.hpp"

std::vector<uint8_t> Encoder::Encoder::EncodeData(const Parser::DataDefinition& dataDefinition)
{
    // TODO: placeholder implementation
    if(!dataDefinition.reserved)
    {
        size_t size = dataDefinition.size * dataDefinition.values.size();
        std::vector<uint8_t> buffer;
        buffer.reserve(size);

        for (const auto& value : dataDefinition.values)
        {
            if (value.operands.empty())
                throw Exception::SemanticError("Data definition cannot be empty", dataDefinition.lineNumber, dataDefinition.column);

            Int128 evaluatedValue = Evaluate(value, bytesWritten, sectionOffset);

            for (size_t i = 0; i < dataDefinition.size; i++)
            {
                uint8_t byte = static_cast<uint8_t>((evaluatedValue >> (i * 8)) & 0xFF);
                buffer.push_back(byte);
            }
        }
        return buffer;
    }
    else
    {
        throw Exception::InternalError("Reserved data encoding is not implemented yet", dataDefinition.lineNumber, dataDefinition.column);
    }
}

uint64_t Encoder::Encoder::GetSize(const Parser::DataDefinition& dataDefinition)
{
    if(!dataDefinition.reserved)
    {
        size_t size = dataDefinition.size * dataDefinition.values.size();
        return size;
    }
    else
    {
        throw Exception::InternalError("Reserved data encoding is not implemented yet", dataDefinition.lineNumber, dataDefinition.column);
    }
}