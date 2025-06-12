#pragma once

#include <cinttypes>

namespace ELF {
    struct Sym32 {
        uint32_t nameOffset;     // Index in .strtab für den Symbolnamen
        uint32_t value;          // Adresse oder Wert des Symbols
        uint32_t size;           // Größe des Symbols (z. B. für Funktionen oder Variablen)
        uint8_t info;            // Bindung und Typ des Symbols (kombiniert)
        uint8_t other;           // Sichtbarkeit des Symbols
        uint16_t sectionIndex;   // Index der zugehörigen Section (SHN_*)
    };

    struct Sym64 {
        uint32_t nameOffset;     // Index in .strtab für den Symbolnamen
        uint8_t info;            // Bindung und Typ des Symbols (kombiniert)
        uint8_t other;           // Sichtbarkeit
        uint16_t sectionIndex;   // Index der zugehörigen Section (SHN_*)
        uint64_t value;          // Adresse oder Wert
        uint64_t size;           // Größe des Symbols
    };
}