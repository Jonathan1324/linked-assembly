#include "Encoder.hpp"

std::vector<uint8_t> x86::Encoder::EncodeControlInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case Instructions::NOP: return {0x90};
        case Instructions::HLT: return {0xF4};

        default: throw Exception::InternalError("Unknown control instruction", instruction.lineNumber, instruction.column);
    }
}