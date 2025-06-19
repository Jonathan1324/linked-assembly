#include "util.hpp"

namespace ELF {
    SectionType getSectionType(std::string name)
    {
        if (name.compare(".text") == 0 || name.compare(".data") == 0)
            return SectionType::ProgBits;
        else if (name.compare(".bss") == 0)
            return SectionType::NoBits;
        else if (name.compare(".symtab") == 0)
            return SectionType::SymTab;
        else if (name.compare(".strtab") == 0 || name.compare(".shstrtab") == 0)
            return SectionType::StrTab;
        else if (name.find(".rela"))
            return SectionType::Rela;
        else if (name.find(".rel"))
            return SectionType::Rel;

        //TODO: current default: ProgBits
        return SectionType::ProgBits;
    }

    namespace bits64 {
        enum SectionFlags : uint64_t {
            WRITE            = 0x1,         // SHF_WRITE
            ALLOC            = 0x2,         // SHF_ALLOC
            EXECUTE          = 0x4,         // SHF_EXECINSTR
            MERGE            = 0x10,        // SHF_MERGE
            STRINGS          = 0x20,        // SHF_STRINGS
            INFO_LINK        = 0x40,        // SHF_INFO_LINK
            LINK_ORDER       = 0x80,        // SHF_LINK_ORDER
            OS_NONCONFORMING = 0x100,       // SHF_OS_NONCONFORMING
            GROUP            = 0x200,       // SHF_GROUP
            TLS              = 0x400,       // SHF_TLS
            COMPRESSED       = 0x800,       // SHF_COMPRESSED
            GNU_RETAIN       = 0x200000,    // SHF_GNU_RETAIN (GNU extension)
            EXCLUDE          = 0x80000000,  // SHF_EXCLUDE (non-standard, used by GNU)

            MASKOS           = 0x0ff00000,  // OS-specific mask
            MASKPROC         = 0xf0000000,  // Processor-specific mask
        };
    }

    namespace bits32 {
        enum SectionFlags : uint32_t {
            WRITE            = 0x1,         // SHF_WRITE
            ALLOC            = 0x2,         // SHF_ALLOC
            EXECUTE          = 0x4,         // SHF_EXECINSTR
            MERGE            = 0x10,        // SHF_MERGE
            STRINGS          = 0x20,        // SHF_STRINGS
            INFO_LINK        = 0x40,        // SHF_INFO_LINK
            LINK_ORDER       = 0x80,        // SHF_LINK_ORDER
            OS_NONCONFORMING = 0x100,       // SHF_OS_NONCONFORMING
            GROUP            = 0x200,       // SHF_GROUP
            TLS              = 0x400,       // SHF_TLS
            COMPRESSED       = 0x800,       // SHF_COMPRESSED
            GNU_RETAIN       = 0x200000,    // SHF_GNU_RETAIN (GNU extension)
            EXCLUDE          = 0x80000000,  // SHF_EXCLUDE (non-standard, used by GNU)

            MASKOS           = 0x0ff00000,  // OS-specific mask
            MASKPROC         = 0xf0000000,  // Processor-specific mask
        };
    }

    uint64_t getFlags64(std::string name)
    {
        using namespace bits64;

        if (name.compare(".text") == 0)
            return ALLOC | EXECUTE;

        if (name.compare(".data") == 0)
            return ALLOC | WRITE;

        if (name.compare(".bss") == 0)
            return ALLOC | WRITE;

        if (name.compare(".rodata") == 0)
            return ALLOC;

        if (name.compare(".tdata") == 0 || name.compare(".tbss") == 0)
            return ALLOC | WRITE | TLS;

        if (name.compare(".strtab") == 0 || name.compare(".shstrtab") == 0)
            return STRINGS;

        if (name.compare(".symtab") == 0)
            return 0;

        if (name.find(".rela.") == 0 || name.find(".rel.") == 0)
            return INFO_LINK;

        if (name.find(".debug") == 0)
            return 0;

        // Default
        return ALLOC;
    }

    uint32_t getFlags32(std::string name)
    {
        using namespace bits32;

        if (name.compare(".text") == 0)
            return ALLOC | EXECUTE;

        if (name.compare(".data") == 0)
            return ALLOC | WRITE;

        if (name.compare(".bss") == 0)
            return ALLOC | WRITE;

        if (name.compare(".rodata") == 0)
            return ALLOC;

        if (name.compare(".tdata") == 0 || name.compare(".tbss") == 0)
            return ALLOC | WRITE | TLS;

        if (name.compare(".strtab") == 0 || name.compare(".shstrtab") == 0)
            return STRINGS;

        if (name.compare(".symtab") == 0)
            return 0;

        if (name.find(".rela.") == 0 || name.find(".rel.") == 0)
            return INFO_LINK;

        if (name.find(".debug") == 0)
            return 0;

        // Default
        return ALLOC;
    }

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

    uint32_t getAlignment32(std::string name)
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