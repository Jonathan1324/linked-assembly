#pragma once

#include <cinttypes>
#include "header.hpp"

namespace ELF {
    struct Rela64 {
        uint64_t r_offset;   // Offset der zu relocatierenden Stelle in der Section
        uint64_t r_info;     // Symbol- und Typ-Information (Symbol index + relocation type)
        int64_t  r_addend;   // Addend (zusätzlicher Wert, der zum Symbol addiert wird)
    };

    // 32-bit Rela
    struct Rela32 {
        uint32_t r_offset;   // Offset der zu relocatierenden Stelle
        uint32_t r_info;     // Symbol- und Typ-Information
        int32_t  r_addend;   // Addend
    };

    sectionBuffer encodeRelocations(std::vector<Relocation> relocations, Bitness bits);
}