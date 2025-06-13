#pragma once

#include <cinttypes>
#include "elf.hpp"

namespace ELF {
    struct Rela64 {
        uint64_t offset;   // Offset der zu relocatierenden Stelle in der Section
        uint64_t info;     // Symbol- und Typ-Information (Symbol index + relocation type)
        int64_t  addend;   // Addend (zusätzlicher Wert, der zum Symbol addiert wird)
    } __attribute__((packed));

    // 32-bit Rela
    struct Rela32 {
        uint32_t offset;   // Offset der zu relocatierenden Stelle
        uint32_t info;     // Symbol- und Typ-Information
        int32_t  addend;   // Addend
    } __attribute__((packed));

    sectionBuffer encodeRelocations(std::vector<Relocation> relocations, HBitness bits);
}