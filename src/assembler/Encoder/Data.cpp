#include "Encoder.hpp"

std::vector<uint8_t> Encoder::Encoder::_EncodeData(const Parser::DataDefinition& dataDefinition)
{
    // TODO: placeholder implementation
    if(!dataDefinition.reserved)
    {
        size_t size = dataDefinition.size * dataDefinition.values.size();
        std::vector<uint8_t> buffer(size, 0x00);
        return buffer;
    }
    else
    {
        throw Exception::InternalError("Reserved data encoding is not implemented yet", dataDefinition.lineNumber, dataDefinition.column);
    }
}