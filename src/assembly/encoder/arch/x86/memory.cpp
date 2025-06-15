#include "memory.hpp"

namespace x86 {
    namespace bits32 {
        MemoryOperand parseMem(const std::string& input)
        {
            MemoryOperand memOp;

            //TODO
            memOp.label = "data2";

            return memOp;
        }

        bool isMemoryOperand(const std::string& op)
        {
            return !op.empty() && op.front() == '[' && op.back() == ']';
        }
    }
}