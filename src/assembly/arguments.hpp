#pragma once

#include <string>
#include "Architecture.hpp"

void parseArguments(int argc, const char *argv[],
                    std::string& input, std::string& output,
                    BitMode& bits, Architecture& arch, Format& format,
                    Endianness& endianness, bool& debug);