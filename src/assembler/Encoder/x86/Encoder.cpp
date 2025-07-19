#include "Encoder.hpp"

#include "../evaluate.hpp"

x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser* _parser)
    : ::Encoder(_context, _arch, _bits, _parser)
{
    
}

void x86::Encoder::Encode()
{
    const std::vector<Section>& sections = parser->getSections();

    size_t bytesWritten = 0;

    for (const auto& section : sections)
    {
        size_t offset = 0;
        for (size_t i = 0; i < section.entries.size(); i++)
        {
            const SectionEntry& entry = section.entries[i];
            
            if (std::holds_alternative<Instruction::Instruction>(entry))
            {
                const Instruction::Instruction& instruction = std::get<Instruction::Instruction>(entry);
                std::cout << "Instruction" << std::endl;
            }
            else if (std::holds_alternative<DataDefinition>(entry))
            {
                const DataDefinition& dataDefinition = std::get<DataDefinition>(entry);
                std::cout << "DataDefinition" << std::endl;
            }
            else if (std::holds_alternative<Label>(entry))
            {
                const Label& label = std::get<Label>(entry);
                std::cout << "Label" << std::endl;
            }
            else if (std::holds_alternative<Constant>(entry))
            {
                const Constant& constant = std::get<Constant>(entry);
                std::cout << "Constant" << std::endl;
            }
            else if (std::holds_alternative<Repetition>(entry))
            {
                const Repetition& repetition = std::get<Repetition>(entry);
                std::cout << "Repetition" << std::endl;
            }
        }
    }
}