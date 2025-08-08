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
        CORE = 4,

        // Reserved inclusive range. Operating system specific.
        T_LOOS = 0xfe00,
        T_HIOS = 0xfeff,

        // Reserved inclusive range. Processor specific.
        T_LOPROC = 0xff00,
        T_HIPROC = 0xffff
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



    enum ProgramType : uint32_t
    {
        P_NONE  = 0, // Unused
        LOAD    = 1, // Loadable
        DYNAMIC = 2, // Dynamic linking information;
        INTERP  = 3, // Interpreter information
        NOTE    = 4, // Auxiliary information
        SHLIB   = 5, // Reserved
        PHDR    = 6, // Program header table
        TLS     = 7, // Thread-Local Storage

        // Reserved inclusive range. Operating system specific.
        P_LOOS = 0x60000000,
        P_HIOS = 0x6FFFFFFF,

        // Reserved inclusive range. Processor specific.
        P_LOPROC = 0x70000000,
        P_HIPROC = 0x7FFFFFFF
    };

    enum ProgramFlags : uint32_t
    {
        EXECUTE = 1,
        WRITE   = 2,
        READ    = 4,

        P_MASKOS   = 0x0ff00000,  // Operating system specific
        P_MASKPROC = 0xf0000000   // Processor specific
    };

    struct ProgramHeader32
    {
        uint32_t Type;
        uint32_t Offset;
        uint32_t VirtualAddress;
        uint32_t PhysicalAddress;
        uint32_t FileSize;
        uint32_t MemorySize;
        uint32_t Flags;
        uint32_t Align;
    };

    struct ProgramHeader64
    {
        uint32_t Type;
        uint32_t Flags;
        uint64_t Offset;
        uint64_t VirtualAddress;
        uint64_t PhysicalAddress;
        uint64_t FileSize;
        uint64_t MemorySize;
        uint64_t Align;
    };



    enum SectionType : uint32_t
    {
        S_None = 0,
        ProgBits        = 1,        // Programmdaten (.text, .data, etc.)
        SymTab          = 2,        // Symboltabelle
        StrTab          = 3,        // Stringtabelle
        Rela            = 4,        // Relocation mit Addend
        Hash            = 5,        // Hash-Tabelle
        Dynamic         = 6,        // Dynamische Linking-Information
        Note            = 7,        // Notiz-Sektion
        NoBits          = 8,        // Section ohne Daten, nur Größe (.bss)
        Rel             = 9,        // Relocation ohne Addend
        ShLib           = 10,       // Reserviert
        DynSym          = 11,       // Dynamische Symboltabelle
        InitArray       = 14,       // Array von Funktionen für Initialisierung
        FiniArray       = 15,       // Array von Funktionen für Destruktion
        PreInitArray    = 16,       // Array von Funktionen für Pre-Init
        Group           = 17,       // Gruppierung von Sections
        SymTabShndx     = 18,       // Erweiterte Symboltabellen-Indizes
        //...
    };
    
    struct SectionHeader32
    {
        uint32_t OffsetInSectionNameStringTable;
        uint32_t Type;
        uint32_t Flags;
        uint32_t VirtualAddress;
        uint32_t Offset;
        uint32_t SectionSize;
        uint32_t LinkIndex;
        uint32_t Info;
        uint32_t AddressAlignment;
        uint32_t EntrySize;
    };

    struct SectionHeader64
    {
        uint32_t OffsetInSectionNameStringTable;
        uint32_t Type;
        uint64_t Flags;
        uint64_t VirtualAddress;
        uint64_t Offset;
        uint64_t SectionSize;
        uint32_t LinkIndex;
        uint32_t Info;
        uint64_t AddressAlignment;
        uint64_t EntrySize;
    };
}