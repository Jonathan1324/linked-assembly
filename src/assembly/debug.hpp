#pragma once

#include "parser.hpp"
#include "encoder/encoder.hpp"
#include "elf/elf.hpp"

void printParsed(Parsed parsed);
void printEncoded(const Encoded& encoded, int indent = 0);
void printElf(const ELF::Data& data);