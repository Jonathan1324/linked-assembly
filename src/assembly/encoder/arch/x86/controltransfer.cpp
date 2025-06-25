#include "datatransfer.hpp"

#include "../evaluate.hpp"
#include "jump.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeInterrupt(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            if (instr.operands.size() < 1)
                throw Exception::SyntaxError("Not enough operands for an interrupt", instr.lineNumber);
            else if (instr.operands.size() > 1)
                throw Exception::SyntaxError("Too many operands for an interrupt", instr.lineNumber);

            std::string num = instr.operands[0];

            unsigned long long val = evaluate(num, constants, instr.lineNumber, false);
            if (val > 0xFF)
                throw Exception::OverflowError(num + " too large for an interrupt", instr.lineNumber);
            uint8_t interrupt = static_cast<uint8_t>(val);

            uint8_t opcode = 0xCD;

            section.buffer.push_back(opcode);
            section.buffer.push_back(interrupt);

            return 2;
        }

        size_t encodeControlTransfer(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.mnemonic.compare("int") == 0)
                offset = encodeInterrupt(instr, section, constants, endianness, context);
            else if (instr.mnemonic.compare("jmp") == 0)
                offset = encodeJump(instr, section, constants, endianness, context);

            return offset;
        }
    }
}