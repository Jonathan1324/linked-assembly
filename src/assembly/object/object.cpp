#include "object.hpp"

#include "elf/elf.hpp"
#include "../debug.hpp"

void createFile(Format& format, std::ofstream& out, BitMode& bitMode,
                Architecture& architecture, Encoded& encoded, Parsed& parsed,
                bool debug)
{
    ELF::Data elfData;
    switch (format)
    {
        case Format::ELF:
            elfData = ELF::createELF(bitMode, architecture, encoded, parsed);
            ELF::writeElf(out, elfData);
            if (debug)
                ELF::print(elfData);
            break;
        case Format::MACHO:
            break;
        case Format::COFF:
            break;
        default:
            break;
    }
}