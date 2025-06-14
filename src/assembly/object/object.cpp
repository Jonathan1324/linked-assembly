#include "object.hpp"

#include "elf/elf.hpp"
#include "mach-o/mach-o.hpp"
#include "coff/coff.hpp"

#include "../debug.hpp"

#include <variant>

void createFile(Format& format, std::ofstream& out, BitMode& bitMode,
                Architecture& architecture, Encoded& encoded, Parsed& parsed,
                Endianness endianness, bool debug)
{
    std::variant<ELF::Data, MACHO::Data, COFF::Data> data;
    switch (format)
    {
        case Format::ELF:
            data = ELF::create(bitMode, architecture, endianness, encoded, parsed);
            ELF::write(out, endianness, std::get<ELF::Data>(data));
            if (debug)
                ELF::print(std::get<ELF::Data>(data));
            break;
        case Format::MACHO:
            data = MACHO::create(bitMode, architecture, endianness, encoded, parsed);
            MACHO::write(out, endianness, std::get<MACHO::Data>(data));
            if (debug)
                MACHO::print(std::get<MACHO::Data>(data));
            break;
        case Format::COFF:
            data = COFF::create(bitMode, architecture, endianness, encoded, parsed);
            COFF::write(out, endianness, std::get<COFF::Data>(data));
            if (debug)
                COFF::print(std::get<COFF::Data>(data));
            break;
        default:
            std::cerr << "Unknown format" << std::endl;
            break;
    }
}