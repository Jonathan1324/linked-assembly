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

    for (const auto& section : parsedSections)
    {
        Section sec;
        sec.name = section.name;
        sec.isInitialized = true;
        if (section.name.compare(".bss") == 0)
        {
            sec.isInitialized = false;
        }

        sectionOffset = 0;

        for (size_t i = 0; i < section.entries.size(); i++)
        {
            const Parser::SectionEntry& entry = section.entries[i];
            
            if (std::holds_alternative<Parser::Instruction::Instruction>(entry))
            {
                const Parser::Instruction::Instruction& instruction = std::get<Parser::Instruction::Instruction>(entry);
                const std::vector<uint8_t> encoded = _EncodeInstruction(instruction);
                const size_t size = encoded.size();
                
                if (sec.isInitialized)
                    sec.buffer.insert(sec.buffer.end(), encoded.begin(), encoded.end());
                else
                    sec.reservedSize += size;
                
                sectionOffset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::DataDefinition>(entry))
            {
                const Parser::DataDefinition& dataDefinition = std::get<Parser::DataDefinition>(entry);
                const std::vector<uint8_t> encoded = _EncodeData(dataDefinition);
                const size_t size = encoded.size();

                if (sec.isInitialized)
                    sec.buffer.insert(sec.buffer.end(), encoded.begin(), encoded.end());
                else
                    sec.reservedSize += size;

                sectionOffset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::Label>(entry))
            {
                const Parser::Label& label = std::get<Parser::Label>(entry);
                Label lbl;
                lbl.name = label.name;
                lbl.section = sec.name;
                lbl.offset = sectionOffset;
                if (labels.find(lbl.name) == labels.end())
                {
                    labels[lbl.name] = lbl;
                }
                else
                {
                    throw Exception::SemanticError("Label '" + lbl.name + "' already defined", label.lineNumber, label.column);
                }
            }
            else if (std::holds_alternative<Parser::Constant>(entry))
            {
                const Parser::Constant& constant = std::get<Parser::Constant>(entry);
                if (constants.find(constant.name) == constants.end())
                {
                    constants[constant.name] = 0;
                }
                else
                    throw Exception::SemanticError("Constant '" + constant.name + "' already defined", constant.lineNumber, constant.column);
            }
            else if (std::holds_alternative<Parser::Repetition>(entry))
            {
                const Parser::Repetition& repetition = std::get<Parser::Repetition>(entry);
                //std::cout << "Repetition" << std::endl;
            }
            else if (std::holds_alternative<Parser::Alignment>(entry))
            {
                const Parser::Alignment& alignment = std::get<Parser::Alignment>(entry);
                const uint64_t align = Evaluate(alignment.align);
                if (align == 0)
                    throw Exception::SemanticError("Alignment cannot be zero", alignment.lineNumber, alignment.column);
                
                const size_t padding = (align - (sectionOffset % align)) % align;
                if (padding > 0)
                {
                    // TODO: really ugly, but works for now
                    std::vector<uint8_t> paddingBytes;
                    if (sec.name.compare(".text") == 0)
                        paddingBytes = _EncodePadding(padding);
                    else
                        paddingBytes.resize(padding, 0);

                    if (sec.isInitialized)
                        sec.buffer.insert(sec.buffer.end(), paddingBytes.begin(), paddingBytes.end());
                    else
                        sec.reservedSize += padding;

                    sectionOffset += padding;
                    bytesWritten += padding;
                }
            }  
        }

        sections.push_back(sec);
    }

    // TODO: debug output
    for (const auto& constant : constants)
    {
        std::cout << "Constant: '" << constant.first << "' = " << constant.second << std::endl;
    }

    for (const auto& label : labels)
    {
        std::cout << "Label: '" << label.first << "' in section '" << label.second.section << "' at offset " << label.second.offset << std::endl;
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
