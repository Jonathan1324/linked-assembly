#include "riscv.hpp"

namespace RISC_V {
    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
    {
        (void)instr;
        (void)section;
        (void)constants;
        (void)endianness;
        (void)context;
        //TODO
        return 0;
    }
}