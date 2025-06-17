#pragma once

#include "../encoder.hpp"

namespace RISC_V {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context);
}