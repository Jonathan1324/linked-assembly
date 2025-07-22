#include "Encoder.hpp"

using namespace Encoder;

size_t Encoder::Section::size() const
{
    return isInitialized ? buffer.size() : reservedSize;
}


Encoder::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : context(_context), arch(_arch), bits(_bits), parser(_parser)
{

}

void Encoder::Encoder::Encode()
{
    const std::vector<Parser::Section>& parsedSections = parser->getSections();

    size_t bytesWritten = 0;

    for (const auto& section : parsedSections)
    {
        Section sec;
        sec.name = section.name;
        sec.isInitialized = true;
        if (section.name.compare(".bss") == 0)
        {
            sec.isInitialized = false;
        }

        size_t offset = 0;
        for (size_t i = 0; i < section.entries.size(); i++)
        {
            const Parser::SectionEntry& entry = section.entries[i];
            
            if (std::holds_alternative<Parser::Instruction::Instruction>(entry))
            {
                const Parser::Instruction::Instruction& instruction = std::get<Parser::Instruction::Instruction>(entry);
                const std::vector<uint8_t> encoded = EncodeInstruction(instruction);
                const size_t size = encoded.size();

                size_t alignment = instruction.alignment;
                size_t padding = (alignment - (offset % alignment)) % alignment;

                if (padding > 0)
                {
                    if (sec.isInitialized)
                    {
                        const std::vector<uint8_t> pad = EncodePadding(padding);
                        sec.buffer.insert(sec.buffer.end(), pad.begin(), pad.end());
                    }
                    else
                        sec.reservedSize += padding;

                    offset += padding;
                    bytesWritten += padding;
                }
                
                if (sec.isInitialized)
                    sec.buffer.insert(sec.buffer.end(), encoded.begin(), encoded.end());
                else
                    sec.reservedSize += size;
                
                offset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::DataDefinition>(entry))
            {
                const Parser::DataDefinition& dataDefinition = std::get<Parser::DataDefinition>(entry);
                
            }
            else if (std::holds_alternative<Parser::Label>(entry))
            {
                const Parser::Label& label = std::get<Parser::Label>(entry);
                //std::cout << "Label" << std::endl;
            }
            else if (std::holds_alternative<Parser::Constant>(entry))
            {
                const Parser::Constant& constant = std::get<Parser::Constant>(entry);
                //std::cout << "Constant" << std::endl;
            }
            else if (std::holds_alternative<Parser::Repetition>(entry))
            {
                const Parser::Repetition& repetition = std::get<Parser::Repetition>(entry);
                //std::cout << "Repetition" << std::endl;
            }
        }

        sections.push_back(sec);
    }
}

void Encoder::Encoder::Print() const
{
    for (const auto& section : sections)
    {
        std::cout << section.name << ":" << std::endl;
        std::cout << "  Size: " << section.size() << std::endl;
    }
}

std::vector<Section> Encoder::Encoder::getSections() const
{
    return sections;
}

#include "x86/Encoder.hpp"

// FIXME: only temporary solution
Encoder::Encoder* Encoder::getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser)
{
    return new x86::Encoder(context, arch, bits, parser);
}
