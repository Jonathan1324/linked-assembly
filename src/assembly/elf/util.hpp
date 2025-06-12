#pragma once

#include "elf.hpp"

namespace ELF {
    SectionType getSectionType(std::string name);

    uint64_t getFlags64(std::string name);
    uint32_t getFlags32(std::string name);

    uint64_t getAlignment64(std::string name);
    uint32_t getAlignment32(std::string name);
}