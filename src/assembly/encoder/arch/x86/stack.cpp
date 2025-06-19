#include "stack.hpp"

#include "pushpop.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeStack(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.mnemonic.compare("push") == 0)
                offset = encodePush(instr, section, constants, endianness, context);
            else if (instr.mnemonic.compare("pop") == 0)
                offset = encodePop(instr, section, constants, endianness, context);

            return offset;
        }
    }
}