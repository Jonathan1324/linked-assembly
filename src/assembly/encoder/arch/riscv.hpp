#pragma once

#include "../encoder.hpp"

namespace riscv {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section);
}