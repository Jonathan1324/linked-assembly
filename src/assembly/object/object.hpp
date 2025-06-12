#pragma once

#include "../architecture/architecture.hpp"
#include "../encoder/encoder.hpp"
#include "../parser.hpp"
#include <fstream>

void createFile(Format& format, std::ofstream& out, BitMode& bitMode,
                Architecture& architecture, Encoded& encoded, Parsed& parsed,
                bool debug = false);