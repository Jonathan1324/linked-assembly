#pragma once

#include <cinttypes>

namespace ELF
{
    enum Bitness : uint8_t
    {
        BITS32 = 1,
        BITS64 = 2
    };

    enum Endianness : uint8_t
    {
        LITTLE = 1,
        BIG = 2,
    };

    enum InstructionSet : uint16_t
    {
        NONE    = 0,
        X86     = 3,
        ARM     = 0x28,
        X64     = 0x3E,
        ARM64   = 0xB7,
        RISCV   = 0xF3
    };

    enum Type : uint16_t
    {
        RELOCATABLE = 1,
        EXECUTABLE = 2,
        SHARED = 3,
        CORE = 4
    };

    struct Header32
    {
        // e_ident[16]
        uint8_t Magic[4] = {0x7F, 'E', 'L', 'F'};
        uint8_t Bitness;
        uint8_t Endianness;
        uint8_t HeaderVersion = 1;
        uint8_t ABI;
        uint8_t _Padding[8] = {0,0,0,0,0,0,0,0};

        uint16_t Type;
        uint16_t InstructionSet;
        uint32_t Version;

        uint32_t ProgramEntryPosition;
        uint32_t ProgramHeaderTablePosition;
        uint32_t SectionHeaderTablePosition;

        uint32_t Flags;
        uint16_t HeaderSize;

        uint16_t ProgramHeaderTableEntrySize;
        uint16_t ProgramHeaderTableEntryCount;

        uint16_t SectionHeaderTableEntrySize;
        uint16_t SectionHeaderTableEntryCount;
        uint16_t SectionNamesIndex;
    } __attribute__((packed));

    struct Header64
    {
        // e_ident[16]
        uint8_t Magic[4] = {0x7F, 'E', 'L', 'F'};
        uint8_t Bitness;
        uint8_t Endianness;
        uint8_t HeaderVersion = 1;
        uint8_t ABI;
        uint8_t _Padding[8];

        uint16_t Type;
        uint16_t InstructionSet;
        uint32_t Version;

        uint64_t ProgramEntryPosition;
        uint64_t ProgramHeaderTablePosition;
        uint64_t SectionHeaderTablePosition;

        uint32_t Flags;
        uint16_t HeaderSize;

        uint16_t ProgramHeaderTableEntrySize;
        uint16_t ProgramHeaderTableEntryCount;
        
        uint16_t SectionHeaderTableEntrySize;
        uint16_t SectionHeaderTableEntryCount;
        uint16_t SectionNamesIndex;
    } __attribute__((packed));
}