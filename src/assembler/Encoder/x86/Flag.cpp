#include "Encoder.hpp"

std::vector<uint8_t> x86::Encoder::EncodeFlagInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case Instructions::CLC: return {0xF8};
        case Instructions::STC: return {0xF9};
        case Instructions::CMC: return {0xF5};
        case Instructions::CLD: return {0xFC};
        case Instructions::STD: return {0xFD};
        case Instructions::CLI: return {0xFA};
        case Instructions::STI: return {0xFB};
        case Instructions::LAHF: return {0x9F};
        case Instructions::SAHF: return {0x9E};

        default: throw Exception::InternalError("Unknown flag instruction", instruction.lineNumber, instruction.column);
    }
}