#pragma once

#include <inttypes.h>

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
        uint8_t _Padding[8] = {0,0,0,0,0,0,0,0};

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
        P_EXECUTE = 1,
        P_WRITE   = 2,
        P_READ    = 4,

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
    } __attribute__((packed));

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
    } __attribute__((packed));



    enum SectionType : uint32_t
    {
        S_None          = 0,
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

    enum SectionFlags : uint64_t
    {
        S_WRITE = 1,
        S_ALLOC = 2,
        S_EXECINSTR = 4,
        S_MERGE = 0x10,
        S_STRINGS = 0x20,
        S_INFO_LINK = 0x40,
        S_LINK_ORDER = 0x80,
        S_OS_NONCONFORMING = 0x100,
        S_GROUP = 0x200,
        S_TLS = 0x400
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
    } __attribute__((packed));

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
    } __attribute__((packed));

    inline uint32_t SetRelocationInfo32(uint32_t symbol, uint8_t type)
    {
        return (symbol << 8) | (type & 0xff);
    }

    inline uint64_t SetRelocationInfo64(uint32_t symbol, uint32_t type)
    {
        return (static_cast<uint64_t>(symbol) << 32) | (type & 0xffffffff);
    }

    enum RelocationType32 : uint8_t
    {
        R386_None   = 0,
        R386_ABS32  = 1,
        R386_ABS16  = 20,
        R386_ABS8   = 22,
    };

    enum RelocationType64 : uint32_t
    {
        RX64_None   = 0,
        RX64_ABS64  = 1,
        RX64_ABS32  = 10,
        RX64_ABS16  = 12,
        RX64_ABS8   = 14,
    };

    struct RelEntry32
    {
        uint32_t offset;
        uint32_t info;
    } __attribute__((packed));

    struct RelEntry64
    {
        uint64_t offset;
        uint64_t info;
    } __attribute__((packed));

    struct RelaEntry32
    {
        uint32_t offset;
        uint32_t info;
        int32_t addend;
    } __attribute__((packed));

    struct RelaEntry64
    {
        uint64_t offset;
        uint64_t info;
        int64_t addend;
    } __attribute__((packed));

    namespace Symbol
    {
        constexpr uint16_t XINDEX = 0xFFF1;
        constexpr uint16_t UNDEFINDEX = 0;

        enum Bind : uint8_t
        {
            LOCAL = 0,
            GLOBAL = 1,
            WEAK = 2,

            // Reserved inclusive range. Operating system specific.
            B_LOOS = 10,
            B_HIOS = 12,

            // Reserved inclusive range. Processor specific.
            B_LOPROC = 13,
            B_HIPROC = 15
        };

        enum Type : uint8_t
        {
            NONE = 0,
            OBJECT = 1,
            FUNC = 2,
            SECTION = 3,
            FILE = 4,
            COMMON = 5,
            TLS = 6,

            // Reserved inclusive range. Operating system specific.
            T_LOOS = 10,
            T_HIOS = 12,

            // Reserved inclusive range. Processor specific.
            T_LOPROC = 13,
            T_HIPROC = 15
        };

        inline uint8_t SetInfo(uint8_t bind, uint8_t type)
        {
            return (bind << 4) | (type & 0xf);
        }

        struct Entry32
        {
            uint32_t OffsetInNameStringTable;
            uint32_t Value;
            uint32_t Size;
            uint8_t Info;
            uint8_t Other;
            uint16_t IndexInSectionHeaderTable;
        } __attribute__((packed));

        struct Entry64
        {
            uint32_t OffsetInNameStringTable;
            uint8_t Info;
            uint8_t Other;
            uint16_t IndexInSectionHeaderTable;
            uint64_t Value;
            uint64_t Size;
        } __attribute__((packed));
    }
}