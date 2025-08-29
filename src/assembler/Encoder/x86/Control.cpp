#include "Encoder.hpp"

std::vector<uint8_t> Encoder::x86::Encoder::EncodeControlInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::NOP: return {0x90};
        case ::x86::Instructions::HLT: return {0xF4};

        default: throw Exception::InternalError("Unknown control instruction", instruction.lineNumber, instruction.column);
    }
}