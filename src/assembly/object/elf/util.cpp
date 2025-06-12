#include "util.hpp"

namespace ELF {
    SectionType getSectionType(std::string name)
    {
        (void)name;
        return SectionType::ProgBits;
    }

    uint64_t getFlags64(std::string name)
    {
        (void)name;
        return 0;
    }

    uint32_t getFlags32(std::string name)
    {
        (void)name;
        return 0;
    }

    uint64_t getAlignment64(std::string name)
    {
        (void)name;
        return 0;
    }

    uint32_t getAlignment32(std::string name)
    {
        (void)name;
        return 0;
    }
}