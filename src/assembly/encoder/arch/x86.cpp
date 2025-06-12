#include "x86.hpp"

namespace x86 {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section)
    {
        (void)instr;
        (void)section;
        //TODO
        section.buffer.push_back(0);
        return 1;
    }
}