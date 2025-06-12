#pragma once

#include "../encoder.hpp"

namespace x86 {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section);
}