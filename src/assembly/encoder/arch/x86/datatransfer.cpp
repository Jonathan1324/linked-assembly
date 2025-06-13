#include "datatransfer.hpp"

#include "mov.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeDataTransfer(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants)
        {
            size_t offset = 0;

            if (instr.mnemonic.compare("mov") == 0)
                offset = encodeMov(instr, section, constants);

            return offset;
        }
    }
}