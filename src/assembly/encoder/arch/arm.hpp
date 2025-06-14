#pragma once

#include "../encoder.hpp"

namespace ARM {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness);
}