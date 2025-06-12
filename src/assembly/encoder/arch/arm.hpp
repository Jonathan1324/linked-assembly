#pragma once

#include "../encoder.hpp"

namespace arm {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section);
}