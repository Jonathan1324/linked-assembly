#include "symbols.hpp"

#include "../../Architecture.hpp"

namespace ELF {
    void writeSymbol(sectionBuffer buffer, Sym32 sym, Endianness endianness)
    {
        Endian::write(buffer, sym.nameOffset, endianness);
        Endian::write(buffer, sym.value, endianness);
        Endian::write(buffer, sym.size, endianness);
        Endian::write(buffer, sym.info, endianness);
        Endian::write(buffer, sym.other, endianness);
        Endian::write(buffer, sym.sectionIndex, endianness);
    }

    void writeSymbol(sectionBuffer buffer, Sym64 sym, Endianness endianness)
    {
        Endian::write(buffer, sym.nameOffset, endianness);
        Endian::write(buffer, sym.info, endianness);
        Endian::write(buffer, sym.other, endianness);
        Endian::write(buffer, sym.sectionIndex, endianness);
        Endian::write(buffer, sym.value, endianness);
        Endian::write(buffer, sym.size, endianness);
    }
}