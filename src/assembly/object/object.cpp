#include "object.hpp"

#include "elf/elf.hpp"
#include "mach-o/mach-o.hpp"
#include "coff/coff.hpp"

#include "../debug.hpp"

#include <variant>
#include <Exception.hpp>

void createFile(Format& format, std::ofstream& out, BitMode& bitMode,
                Architecture& architecture, Encoded& encoded, Parsed& parsed,
                Endianness endianness, Context& context, bool debug)
{
    std::variant<ELF::Data, MACHO::Data, COFF::Data> data;
    switch (format)
    {
        case Format::ELF:
            data = ELF::create(bitMode, architecture, endianness, encoded, parsed, context);
            ELF::write(out, endianness, std::get<ELF::Data>(data), context);
            if (debug)
                ELF::print(std::get<ELF::Data>(data));
            break;
        case Format::MACHO:
            data = MACHO::create(bitMode, architecture, endianness, encoded, parsed, context);
            MACHO::write(out, endianness, std::get<MACHO::Data>(data), context);
            if (debug)
                MACHO::print(std::get<MACHO::Data>(data));
            break;
        case Format::COFF:
            data = COFF::create(bitMode, architecture, endianness, encoded, parsed, context);
            COFF::write(out, endianness, std::get<COFF::Data>(data), context);
            if (debug)
                COFF::print(std::get<COFF::Data>(data));
            break;
        default:
            throw Exception::InternalError("Unknown format");
            break;
    }
}