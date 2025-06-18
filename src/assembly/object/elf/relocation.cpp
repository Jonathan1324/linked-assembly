#include "relocation.hpp"
#include <util/string.hpp>

namespace ELF {
    sectionBuffer encodeRelocations(std::vector<Relocation> relocations, HBitness bits, Endianness endianness, std::unordered_map<std::string, size_t> labelIndexes)
    {
        sectionBuffer buffer;
        
        for (const auto& relocation : relocations)
        {
            if (labelIndexes.find(relocation.labelName) == labelIndexes.end())
            {
                std::cout << relocation.labelName << " not found." << std::endl;
                return {};
            }

            uint64_t symbolIndex = labelIndexes[relocation.labelName];
            uint64_t symbolType = 0;
            
            if (bits == HBitness::Bits64)
            {
                switch (relocation.type)
                {
                    case Type::None: symbolType = 0; break; // R_X86_64_NONE
                    case Type::Absolute:
                        if (relocation.size == 64) symbolType = 1; // R_X86_64_64
                        else if (relocation.size == 32) symbolType = 10; // R_X86_64_32
                        else if (relocation.size == 16) symbolType = 12; // R_X86_64_16
                        else if (relocation.size == 8)  symbolType = 14; // R_X86_64_8
                        break;
                    case Type::PCRelative:
                        if (relocation.size == 64) symbolType = 24; // R_X86_64_PC64
                        else if (relocation.size == 32) symbolType = 2; // R_X86_64_PC32
                        else if (relocation.size == 16) symbolType = 13; // R_X86_64_PC16
                        else if (relocation.size == 8)  symbolType = 15; // R_X86_64_PC8
                        break;
                    case Type::GOT:        symbolType = 3; break; // R_X86_64_GOT32
                    case Type::PLT:        symbolType = 4; break; // R_X86_64_PLT32
                    case Type::Copy:       symbolType = 5; break; // R_X86_64_COPY
                    case Type::GlobDat:    symbolType = 6; break; // R_X86_64_GLOB_DAT
                    case Type::JmpSlot:    symbolType = 7; break; // R_X86_64_JUMP_SLOT
                    case Type::Relative:   symbolType = 8; break; // R_X86_64_RELATIVE
                    case Type::GOTPC:      symbolType = 26; break; // R_X86_64_GOTPC32
                    case Type::Size:
                        if (relocation.size == 32) symbolType = 32; // R_X86_64_SIZE32
                        else if (relocation.size == 64) symbolType = 33; // R_X86_64_SIZE64
                        break;
                    default: symbolType = 0; break;
                }
            }
            else // 32-Bit (i386)
            {
                switch (relocation.type)
                {
                    case Type::None: symbolType = 0; break; // R_386_NONE
                    case Type::Absolute:
                        if (relocation.size == 16) symbolType = 20; // R_386_16
                        else if (relocation.size == 8) symbolType = 22; // R_386_8
                        else symbolType = 1; // R_386_32 (default)
                        break;
                    case Type::PCRelative:
                        if (relocation.size == 16) symbolType = 21; // R_386_PC16
                        else if (relocation.size == 8) symbolType = 23; // R_386_PC8
                        else symbolType = 2; // R_386_PC32 (default)
                        break;
                    case Type::GOT:        symbolType = 3; break; // R_386_GOT32
                    case Type::PLT:        symbolType = 4; break; // R_386_PLT32
                    case Type::Copy:       symbolType = 5; break; // R_386_COPY
                    case Type::GlobDat:    symbolType = 6; break; // R_386_GLOB_DAT
                    case Type::JmpSlot:    symbolType = 7; break; // R_386_JMP_SLOT
                    case Type::Relative:   symbolType = 8; break; // R_386_RELATIVE
                    case Type::GOTPC:      symbolType = 10; break; // R_386_GOTPC
                    case Type::Size:       symbolType = 38; break; // R_386_SIZE32
                    default:               symbolType = 0; break;
                }
            }


            if (bits == HBitness::Bits64)
            {
                Rela64 rela;
                rela.offset = relocation.offsetInSection;
                rela.info = ((symbolIndex << 32) | (symbolType & 0xFFFFFFFF));;
                rela.addend = relocation.addend;

                Endian::write(buffer, rela.offset, endianness);
                Endian::write(buffer, rela.info, endianness);
                Endian::write(buffer, rela.addend, endianness);
            }
            else
            {
                Rela32 rela;
                rela.offset = relocation.offsetInSection;
                rela.info = ((symbolIndex << 8) | (symbolType & 0xFF));;
                rela.addend = relocation.addend;

                Endian::write(buffer, rela.offset, endianness);
                Endian::write(buffer, rela.info, endianness);
                Endian::write(buffer, rela.addend, endianness);
            }
        }

        return buffer;
    }
}