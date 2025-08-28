#include "Encoder.hpp"

std::vector<uint8_t> Encoder::x86::Encoder::EncodeDataInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::MOV:
        {
            // TODO: add
            throw Exception::InternalError("mov not implemented yet", instruction.lineNumber, instruction.column);
        }

        default: throw Exception::InternalError("Unknown data instruction", instruction.lineNumber, instruction.column);
    }
}