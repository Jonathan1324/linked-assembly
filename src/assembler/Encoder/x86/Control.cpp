#include "Encoder.hpp"

std::vector<uint8_t> Encoder::x86::Encoder::EncodeControlInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::NOP: return {0x90};
        case ::x86::Instructions::HLT: return {0xF4};

        default: throw Exception::InternalError("Unknown control instruction", instruction.lineNumber, instruction.column);
    }
}