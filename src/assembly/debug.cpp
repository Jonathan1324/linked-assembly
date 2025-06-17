#include "debug.hpp"

void printParsed(Parsed parsed)
{
    std::cout << "Globals" << std::endl;
    for (size_t i = 0; i < parsed.globals.size(); i++)
    {
        std::cout << "\t" << parsed.globals[i] << std::endl;
    }

    std::cout << "Externs" << std::endl;
    for (size_t i = 0; i < parsed.externs.size(); i++)
    {
        std::cout << "\t" << parsed.externs[i] << std::endl;
    }

    for (size_t i = 0; i < parsed.sections.size(); i++)
    {
        Section* section = &parsed.sections[i];
        std::cout << "Section " << section->name << std::endl;

        for (const auto& [labelName, label] : section->labels) {
            std::cout << '\t';
            if (label.isGlobal)
                std::cout << "Global ";
            std::cout << (label.isGlobal ? "label " : "Label ") << label.name << " pointing to entry " << label.instructionIndex << std::endl;
            for (const auto& localLabel : label.localLabels) {
                std::cout << "\t\tLocal label " << localLabel.name << " pointing to entry " << localLabel.instructionIndex << std::endl;
            }
        }


        for (size_t entryIndex = 0; entryIndex < section->entries.size(); entryIndex++)
        {
            SectionEntry entry = section->entries[entryIndex];
            if (std::holds_alternative<Instruction>(entry))
            {
                Instruction instr = std::get<Instruction>(entry);
                std::cout << "\t";
                switch(instr.mode)
                {
                    case BitMode::Bits16:
                        std::cout << "16-bit ";
                        break;
                    case BitMode::Bits32:
                        std::cout << "32-bit ";
                        break;
                    case BitMode::Bits64:
                        std::cout << "64-bit ";
                        break;
                    default:
                        std::cout << "Unknown" ;
                        break;
                }
                std::cout << "instruction " << instr.mnemonic << " at line " << instr.lineNumber << std::endl;
                for (size_t a = 0; a < instr.operands.size(); a++)
                {
                    std::cout << "\t\t" << instr.operands[a] << std::endl;
                }
            }
            else if (std::holds_alternative<DataDefinition>(entry))
            {
                DataDefinition data = std::get<DataDefinition>(entry);
                std::cout << (data.reserved ? "\tReserved" : "\tData") << data.name << " of type " << data.type << " at line " << data.lineNumber << std::endl;
                for (size_t a = 0; a < data.values.size(); a++)
                {
                    std::cout << "\t\t" << data.values[a] << std::endl;
                }
            }
        }
    }
}

void printEncoded(const Encoded& encoded, int indent)
{
    std::string tab(indent, '\t');

    for (const auto& [sectionName, section] : encoded.sections) {
        std::cout << tab << "Section: " << sectionName << "\n";

        std::cout << tab << "\tBuffer size: " << section.buffer.size() << " bytes\n";

        if (!section.labels.empty()) {
            std::cout << tab << "\tLabels:\n";
            for (const auto& [labelName, label] : section.labels) {
                std::cout << tab << "\t\tName: " << label.name
                          << ", Offset: " << label.offset
                          << ", Global: " << (label.isGlobal ? "yes" : "no") << "\n";

                if (!label.localLabels.empty()) {
                    std::cout << tab << "\t\tLocal Labels:\n";
                    for (const auto& [localName, localLabel] : label.localLabels) {
                        std::cout << tab << "\t\t\tName: " << localLabel.name
                                  << ", Offset: " << localLabel.offset << "\n";
                    }
                }
            }
        } else {
            std::cout << tab << "\t(No labels)\n";
        }
    }
}

namespace ELF {
    void print(const Data& data)
    {
        for (const auto& section : data.sections)
        {
            std::cout << "Section: " << section.name << std::endl;
            if (std::holds_alternative<SectionHeader32>(section.header))
            {
                const SectionHeader32& hdr32 = std::get<SectionHeader32>(section.header);
                std::cout << "\tOffset in shstrtab: " << hdr32.offsetInSectionNameStringTable << std::endl;
                std::cout << "\tSection size: " << hdr32.sectionSize << std::endl;
                std::cout << "\tOffset in file: " << hdr32.fileOffset << std::endl;
                std::cout << "\tFlags: " << hdr32.Flags << std::endl;
                std::cout << "\tType: " << (uint32_t)hdr32.Type << std::endl;
                std::cout << "\tInfo: " << hdr32.info << std::endl;
            }
            else if (std::holds_alternative<SectionHeader64>(section.header))
            {
                const SectionHeader64& hdr64 = std::get<SectionHeader64>(section.header);
                std::cout << "\tOffset in shstrtab: " << hdr64.offsetInSectionNameStringTable << std::endl;
                std::cout << "\tSection size: " << hdr64.sectionSize << std::endl;
                std::cout << "\tOffset in file: " << hdr64.fileOffset << std::endl;
                std::cout << "\tFlags: " << hdr64.Flags << std::endl;
                std::cout << "\tType: " << (uint32_t)hdr64.Type << std::endl;
                std::cout << "\tInfo: " << hdr64.info << std::endl;
            }
        }
        if (data.header.Bitness == HBitness::Bits64)
            std::cout << "Bitness: 64 bits" << std::endl;
        else
            std::cout << "Bitness: 32 bits" << std::endl;
        if (data.header.Endianness == HEndianness::LittleEndian)
            std::cout << "Endianness: Little endian" << std::endl;
        else
            std::cout << "Endianness: Big endian" << std::endl;
        std::cout << "HeaderVersion: " << (uint32_t)data.header.HeaderVersion << std::endl;
        std::cout << "Type: " << (uint32_t)data.header.Type << std::endl;
        std::cout << "InstructionSet: " << (uint32_t)data.header.InstructionSet << std::endl;
        std::cout << "Version: " << (uint32_t)data.header.Version << std::endl;
        if (data.header.Bitness == HBitness::Bits64)
            std::cout << "SectionHeaderTablePos: " << data.header.bits64.SectionHeaderTablePosition << std::endl;
        else
            std::cout << "SectionHeaderTablePos: " << data.header.bits32.SectionHeaderTablePosition << std::endl;
        std::cout << "SectionHeaderTableEntrySize: " << data.header.SectionHeaderTableEntrySize << std::endl;
        std::cout << "SectionHeaderTableEntryCount: " << data.header.SectionHeaderTableEntryCount << std::endl;
        std::cout << "SectionNamesIndex: " << data.header.SectionNamesIndex << std::endl;
    }
}

namespace MACHO {
    void print(const Data& data)
    {
        (void)data;
        //TODO
    }
}

namespace COFF {
    void print(const Data& data)
    {
        (void)data;
        //TODO
    }
}