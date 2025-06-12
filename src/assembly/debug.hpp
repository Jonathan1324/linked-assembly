#pragma once

#include "parser.hpp"
#include "encoder/encoder.hpp"
#include "object/elf/elf.hpp"

void printParsed(Parsed parsed);
void printEncoded(const Encoded& encoded, int indent = 0);

namespace ELF {
    void print(const ELF::Data& data);
}