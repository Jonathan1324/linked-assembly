#pragma once

#include <cinttypes>
#include <string>

#include "../architecture/architecture.hpp"
#include "../encoder/encoder.hpp"

namespace ELF {
    enum class Bitness : uint8_t {
        None    = 0,
        Bits32  = 1,
        Bits64  = 2
    };

    enum class Endianness : uint8_t {
        LittleEndian    = 1,
        BigEndian       = 2
    };

    enum class Type : uint16_t {
        Relocatable = 1,
        Executable  = 2,
        Shared      = 3,
        Core        = 4
    };

    enum class InstructionSet : uint16_t {
        None    = 0,
        x86     = 3,
        x64     = 0x3E,
        Arm     = 0x28,
        Arm64   = 0xB7,
        riscv   = 0xF3
    };

    struct Header {
        char Magic[4] = {0x7F, 'E', 'L', 'F'};

        Bitness Bitness;
        Endianness Endianness;
        uint8_t HeaderVersion;
        uint8_t ABI;
        uint8_t ABIVersion;
        uint8_t _Padding[7]  = {};

        Type Type;
        InstructionSet InstructionSet;
        uint32_t Version;

        union {
            struct {
                uint32_t ProgramEntryPosition;
                uint32_t ProgramHeaderTablePosition;
                uint32_t SectionHeaderTablePosition;
            } bits32;

            struct {
                uint64_t ProgramEntryPosition;
                uint64_t ProgramHeaderTablePosition;
                uint64_t SectionHeaderTablePosition;
            } bits64;
        };

        uint32_t Flags;
        uint16_t HeaderSize;
        uint16_t ProgramHeaderTableEntrySize;
        uint16_t ProgramHeaderTableEntryCount;
        uint16_t SectionHeaderTableEntrySize;
        uint16_t SectionHeaderTableEntryCount;
        uint16_t SectionNamesIndex;

        bool is64Bit() const {
            return Bitness == Bitness::Bits64;
        }

        uint64_t getProgramEntryPosition() const {
            return is64Bit() ? bits64.ProgramEntryPosition : bits32.ProgramEntryPosition;
        }

        uint64_t getProgramHeaderTablePosition() const {
            return is64Bit() ? bits64.ProgramHeaderTablePosition : bits32.ProgramHeaderTablePosition;
        }

        uint64_t getSectionHeaderTablePosition() const {
            return is64Bit() ? bits64.SectionHeaderTablePosition : bits32.SectionHeaderTablePosition;
        }

        void setProgramEntryPosition(uint64_t val) {
            if (is64Bit()) bits64.ProgramEntryPosition = val;
            else bits32.ProgramEntryPosition = static_cast<uint32_t>(val);
        }

        void setProgramHeaderTablePosition(uint64_t val) {
            if (is64Bit()) bits64.ProgramHeaderTablePosition = val;
            else bits32.ProgramHeaderTablePosition = static_cast<uint32_t>(val);
        }

        void setSectionHeaderTablePosition(uint64_t val) {
            if (is64Bit()) bits64.SectionHeaderTablePosition = val;
            else bits32.SectionHeaderTablePosition = static_cast<uint32_t>(val);
        }
    } __attribute__((packed));

    enum class SectionType : uint32_t {
        Null            = 0,        // Unbenutzte Section
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

    struct SectionHeader64 {
        uint32_t offsetInSectionNameStringTable;
        SectionType Type;

        uint64_t Flags;
        uint64_t virtualAddress;
        uint64_t fileOffset;
        uint64_t sectionSize;

        uint32_t linkIndex;
        uint32_t info;

        uint64_t addressAlignment;
        uint64_t entrySize;
    } __attribute__((packed));

    struct SectionHeader32 {
        uint32_t offsetInSectionNameStringTable;
        SectionType Type;

        uint32_t Flags;
        uint32_t virtualAddress;
        uint32_t fileOffset;
        uint32_t sectionSize;

        uint32_t linkIndex;
        uint32_t info;

        uint32_t addressAlignment;
        uint32_t entrySize;
    } __attribute__((packed));

    struct ELFSection {
        std::string name;
        sectionBuffer buffer;
        std::variant<SectionHeader32, SectionHeader64> header;
    };

    struct Data {
        Header header;
        std::vector<ELFSection> sections;
    };

    Data createELF(BitMode bits, Architecture arch, Encoded encoded, Parsed parsed);
}