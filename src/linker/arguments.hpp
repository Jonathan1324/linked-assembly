#pragma once

#include <string>
#include <Architecture.hpp>
#include "Context.hpp"

bool parseArguments(int argc, const char *argv[],
                    std::vector<std::string>& inputs, std::string& output,
                    BitMode& bits, Architecture& arch, Format& format,
                    Endianness& endianness, bool& debug,
                    Context& context);