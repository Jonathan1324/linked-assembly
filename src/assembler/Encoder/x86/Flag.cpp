#include "Encoder.hpp"

std::vector<uint8_t> Encoder::x86::Encoder::EncodeFlagInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::CLC: return {0xF8};
        case ::x86::Instructions::STC: return {0xF9};
        case ::x86::Instructions::CMC: return {0xF5};
        case ::x86::Instructions::CLD: return {0xFC};
        case ::x86::Instructions::STD: return {0xFD};
        case ::x86::Instructions::CLI: return {0xFA};
        case ::x86::Instructions::STI: return {0xFB};
        case ::x86::Instructions::LAHF: return {0x9F};
        case ::x86::Instructions::SAHF: return {0x9E};

        default: throw Exception::InternalError("Unknown flag instruction", instruction.lineNumber, instruction.column);
    }
}