#pragma once

#include <Architecture.hpp>
#include "../encoder/encoder.hpp"
#include "../parser/parser.hpp"
#include <fstream>

#include "../Context.hpp"

void createFile(Format& format, std::ofstream& out, BitMode& bitMode,
                Architecture& architecture, Encoded& encoded, Parsed& parsed,
                Endianness endianness, Context& context, bool debug = false);