#include "jump.hpp"

#include "../evaluate.hpp"
#include <util/string.hpp>
#include "registers.hpp"
#include "memory.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeJump(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            //TODO
            return 0;
        }
    }
}