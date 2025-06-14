#pragma once

#include <cinttypes>
#include <cstddef>
#include <unordered_map>
#include <string>
#include "../../encoder.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeMov(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness);
    }
}