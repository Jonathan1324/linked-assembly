#pragma once

#include <cinttypes>

namespace ELF {
    enum class SectionIndex : uint16_t {
        UNDEFINED       = 0
    };

    namespace SymbolBind {
        constexpr uint8_t local = 0;
        constexpr uint8_t global = 1;
        constexpr uint8_t weak = 2;
    }

    namespace SymbolType {
        constexpr uint8_t notype = 0;
        constexpr uint8_t object = 1;
        constexpr uint8_t function = 2;
        constexpr uint8_t section = 3;
        constexpr uint8_t file = 4;
        constexpr uint8_t common = 5;
        constexpr uint8_t tls = 6;
    }

    constexpr uint8_t makeSymbolInfo(uint8_t bind, uint8_t type)
    {
        return (bind << 4) | (type & 0x0F);
    }

    
    struct Sym32 {
        uint32_t nameOffset;        // Index in .strtab für den Symbolnamen
        uint32_t value;             // Adresse oder Wert des Symbols
        uint32_t size;              // Größe des Symbols (z. B. für Funktionen oder Variablen)
        uint8_t info;               // Bindung und Typ des Symbols (kombiniert)
        uint8_t other;              // Sichtbarkeit des Symbols
        uint16_t sectionIndex;      // Index der zugehörigen Section (SHN_*)
    } __attribute__((packed));

    struct Sym64 {
        uint32_t nameOffset;        // Index in .strtab für den Symbolnamen
        uint8_t info;               // Bindung und Typ des Symbols (kombiniert)
        uint8_t other;              // Sichtbarkeit
        uint16_t sectionIndex;      // Index der zugehörigen Section (SHN_*)
        uint64_t value;             // Adresse oder Wert
        uint64_t size;              // Größe des Symbols
    } __attribute__((packed));
}