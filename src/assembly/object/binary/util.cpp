#include "util.hpp"

namespace Binary {
    uint64_t getAlignment64(std::string name)
    {
        if (name.compare(".text") == 0)
            return 16;

        if (name.compare(".data") == 0)
            return 8;

        if (name.compare(".bss") == 0)
            return 8;

        if (name.compare(".rodata") == 0)
            return 8;

        if (name.compare(".tdata") == 0 || name.compare(".tbss") == 0)
            return 8;

        if (name.compare(".symtab") == 0)
            return 8;
        
        if (name.compare(".strtab") == 0 || name.compare(".shstrtab") == 0)
            return 1;

        if (name.find(".rela.") == 0 || name.find(".rel.") == 0)
            return 8;

        if (name.find(".debug") == 0)
            return 1;

        // Default
        return 8;
    }

    uint64_t getAlignment32(std::string name)
    {
        if (name.compare(".text") == 0)
            return 16;

        if (name.compare(".data") == 0)
            return 4;

        if (name.compare(".bss") == 0)
            return 4;

        if (name.compare(".rodata") == 0)
            return 4;

        if (name.compare(".tdata") == 0 || name.compare(".tbss") == 0)
            return 4;

        if (name.compare(".symtab") == 0)
            return 4;

        if (name.compare(".strtab") == 0 || name.compare(".shstrtab") == 0)
            return 1;

        if (name.find(".rela.") == 0 || name.find(".rel.") == 0)
            return 4;

        if (name.find(".debug") == 0)
            return 1;

        // Default
        return 4;
    }
}