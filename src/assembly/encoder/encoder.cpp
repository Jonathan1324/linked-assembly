#include "encoder.hpp"
#include "data.hpp"

#include "arch/x86.hpp"
#include "arch/arm.hpp"
#include "arch/riscv.hpp"

Encoded encode(Parsed& parsed, Architecture arch, Endianness endianness)
{
    Encoded encoded;

    for (const auto& s : parsed.sections)
    {
        EncodedSection& section = encoded.sections[s.name];

        Relocation relocation;
        relocation.labelName = "data2";
        relocation.addend = 0x00;
        relocation.offsetInSection = 0x12;
        relocation.type = Type::Absolute;
        relocation.size = 32;

        section.relocations.push_back(relocation);

        section.name = s.name;

        std::vector<size_t> entryOffsets(s.entries.size());
        size_t currentOffset = 0;

        for (size_t i = 0; i < s.entries.size(); ++i)
        {
            entryOffsets[i] = currentOffset;
            
            if (std::holds_alternative<Instruction>(s.entries[i]))
            {
                Instruction instruction = std::get<Instruction>(s.entries[i]);
                size_t padding = (instruction.alignment > 0) ? 
                    ( (instruction.alignment - (currentOffset % instruction.alignment)) % instruction.alignment ) : 0;

                if (padding != 0)
                {
                    section.buffer.insert(section.buffer.end(), padding, 0x00);
                    currentOffset += padding;
                }

                switch (arch)
                {
                    case Architecture::x86:
                        currentOffset += x86::encodeInstruction(instruction, section, parsed.constants, endianness);
                        break;
                    case Architecture::ARM:
                        currentOffset += ARM::encodeInstruction(instruction, section, parsed.constants, endianness);
                        break;
                    case Architecture::RISC_V:
                        currentOffset += RISC_V::encodeInstruction(instruction, section, parsed.constants, endianness);
                        break;
                    default:
                        std::cerr << "Unknown architecture (encoder)" << std::endl;
                        break;
                }
            }
            else if (std::holds_alternative<DataDefinition>(s.entries[i]))
            {
                DataDefinition data = std::get<DataDefinition>(s.entries[i]);
                size_t padding = (data.alignment > 0) ? 
                    ( (data.alignment - (currentOffset % data.alignment)) % data.alignment ) : 0;

                if (padding != 0)
                {
                    section.buffer.insert(section.buffer.end(), padding, 0x00);
                    currentOffset += padding;
                }

                currentOffset += encodeData(data, section, encoded, endianness);
            }
            else
            {
                //TODO: error - unknown type
            }
        }

        for (const auto& [labelName, label] : s.labels)
        {
            size_t offset = entryOffsets[label.instructionIndex];
            section.labels[label.name].offset = offset;
            
            section.labels[label.name].name = label.name;
            section.labels[label.name].isGlobal = label.isGlobal;

            for (const auto& localLabel : label.localLabels)
            {
                section.labels[label.name].localLabels[localLabel.name].name = localLabel.name;
                section.labels[label.name].localLabels[localLabel.name].offset = entryOffsets[localLabel.instructionIndex];
            }
        }
    }

    return encoded;
}