#pragma once

#include "../encoder.hpp"

namespace x86 {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context);
}