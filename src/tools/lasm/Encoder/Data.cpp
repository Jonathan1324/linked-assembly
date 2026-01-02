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

            Evaluation evaluated = Evaluate(value, bytesWritten, sectionOffset, currentSection);

            if (evaluated.useOffset)
            {
                for (size_t i = 0; i < dataDefinition.size; i++)
                {
                    uint8_t byte = static_cast<uint8_t>((evaluated.offset >> (i * 8)) & 0xFF);
                    buffer.push_back(byte);
                }
                Relocation reloc;
                reloc.offsetInSection = sectionOffset;
                reloc.addend = evaluated.offset;
                reloc.addendInCode = true;
                reloc.section = *currentSection;
                reloc.usedSection = evaluated.usedSection;
                reloc.type = RelocationType::Absolute;
                switch (dataDefinition.size)
                {
                    case 1: reloc.size = RelocationSize::Bit8; break;
                    case 2: reloc.size = RelocationSize::Bit16; break;
                    case 3: reloc.size = RelocationSize::Bit24; break;
                    case 4: reloc.size = RelocationSize::Bit32; break;
                    case 8: reloc.size = RelocationSize::Bit64; break;
                }
                relocations.push_back(std::move(reloc));
            }
            else
            {
                for (size_t i = 0; i < dataDefinition.size; i++)
                {
                    uint8_t byte = static_cast<uint8_t>((evaluated.result >> (i * 8)) & 0xFF);
                    buffer.push_back(byte);
                }
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