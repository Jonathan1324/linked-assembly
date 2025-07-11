#pragma once

#include "parser/parser.hpp"
#include "encoder/encoder.hpp"
#include "object/binary/binary.hpp"
#include "object/elf/elf.hpp"
#include "object/mach-o/mach-o.hpp"
#include "object/coff/coff.hpp"

void printParsed(Parsed parsed);
void printEncoded(const Encoded& encoded, int indent = 0);

namespace Binary {
    void print(const Data& data);
}

namespace ELF {
    void print(const Data& data);
}

namespace MACHO {
    void print(const Data& data);
}

namespace COFF {
    void print(const Data& data);
}