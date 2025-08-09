#include "ELFWriter.hpp"
#include "Headers.hpp"

uint64_t ELF::Writer::getSectionFlags(const std::string& name)
{
    if (name.compare(".text") == 0)
        return SectionFlags::S_ALLOC | SectionFlags::S_EXECINSTR;
    else if (name.compare(".data") == 0)
        return SectionFlags::S_ALLOC | SectionFlags::S_WRITE;
    // TODO: BSS
    else if (name.compare(".rodata") == 0)
        return SectionFlags::S_ALLOC;

    else if (name.compare(".tdata") == 0)
        return SectionFlags::S_ALLOC | SectionFlags::S_WRITE | SectionFlags::S_TLS;

    else if (name.compare(".comment") == 0)
        return 0;

    return SectionFlags::S_ALLOC;
}

uint32_t ELF::Writer::getSectionType(const std::string& name)
{


    return SectionType::ProgBits; // TODO
}
