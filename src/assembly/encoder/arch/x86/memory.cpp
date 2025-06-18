#include "memory.hpp"

#include <iostream>
#include <util/string.hpp>

namespace x86 {
    namespace bits32 {
        MemoryOperand parseMem(const std::string& input)
        {
            MemoryOperand memOp;

            (void)input;
            //TODO
            memOp.label = input.substr(1, input.size() - 2);

            return memOp;
        }

        Memory isMemoryOperand(const std::string& op)
        {
            // segments
            // Quick check if operand ends with ']'
            if (op.empty() || op.back() != ']')
                return Memory::None;

            // Lowercase copy of op to do case-insensitive prefix matching
            std::string lowerOp = toLower(op);

            // Look for size prefixes before the first '['
            size_t bracketPos = lowerOp.find('[');
            if (bracketPos == std::string::npos)
                return Memory::None; // malformed, no '[' but ends with ']'

            // Extract prefix text before '[' (trim spaces)
            std::string prefix = lowerOp.substr(0, bracketPos);
            prefix = trim(prefix);

            // Match known size prefixes
            if (prefix == "byte ptr")    return Memory::Byte;
            if (prefix == "word ptr")    return Memory::Word;
            if (prefix == "dword ptr")   return Memory::Dword;
            if (prefix == "fword ptr")   return Memory::Fword;
            if (prefix == "qword ptr")   return Memory::Qword;
            if (prefix == "mmword ptr")  return Memory::MMword;
            if (prefix == "tbyte ptr")   return Memory::Tbyte;
            if (prefix == "xword ptr")   return Memory::Xword;
            if (prefix == "oword ptr")   return Memory::Oword;
            if (prefix == "xmmword ptr") return Memory::XMMword;
            if (prefix == "yword ptr")   return Memory::Yword;
            if (prefix == "zword ptr")   return Memory::Zword;

            // No prefix but is memory operand
            return Memory::Default;
        }
    }
}