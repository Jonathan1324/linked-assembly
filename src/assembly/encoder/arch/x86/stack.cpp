#include "stack.hpp"

#include "pushpop.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodePushX(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.operands.size() > 0)
                throw Exception::SyntaxError("Too many operands for pusha/pushf", instr.lineNumber);

            if (instr.mnemonic.compare("pusha") == 0
             || instr.mnemonic.compare("pushad") == 0)
            {
                uint8_t opcode = 0x60;
                section.buffer.push_back(opcode);
                offset = 1;
            }
            else if (instr.mnemonic.compare("pushf") == 0)
            {
                uint8_t opcode = 0x9C;
                section.buffer.push_back(opcode);
                offset = 1;
            }

            return offset;
        }

        size_t encodePopX(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.operands.size() > 0)
                throw Exception::SyntaxError("Too many operands for pusha/pushf", instr.lineNumber);

            if (instr.mnemonic.compare("popa") == 0
             || instr.mnemonic.compare("popad") == 0)
            {
                uint8_t opcode = 0x61;
                section.buffer.push_back(opcode);
                offset = 1;
            }
            else if (instr.mnemonic.compare("popf") == 0)
            {
                uint8_t opcode = 0x9D;
                section.buffer.push_back(opcode);
                offset = 1;
            }

            return offset;
        }

        size_t encodeStack(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.mnemonic.compare("push") == 0)
                offset = encodePush(instr, section, constants, endianness, context);
            else if (instr.mnemonic.compare("pop") == 0)
                offset = encodePop(instr, section, constants, endianness, context);

            else if (instr.mnemonic.find("push") == 0)
                offset = encodePushX(instr, section, constants, endianness, context);
            else if (instr.mnemonic.find("pop") == 0)
                offset = encodePopX(instr, section, constants, endianness, context);

            return offset;
        }
    }
}