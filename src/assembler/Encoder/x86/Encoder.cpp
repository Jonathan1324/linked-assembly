#include "Encoder.hpp"

Encoder::x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : ::Encoder::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> Encoder::x86::Encoder::EncodeInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved)
{
    switch (instruction.mnemonic)
    {
        // CONTROL
        case ::x86::Instructions::NOP:
        case ::x86::Instructions::HLT:
            return EncodeControlInstruction(instruction, ignoreUnresolved);

        // INTERRUPT
        case ::x86::Instructions::INT:
            return EncodeInterruptInstruction(instruction, ignoreUnresolved);

        // FLAGS
        case ::x86::Instructions::CLC: case ::x86::Instructions::STC: case ::x86::Instructions::CMC:
        case ::x86::Instructions::CLD: case ::x86::Instructions::STD:
        case ::x86::Instructions::CLI: case ::x86::Instructions::STI:
        case ::x86::Instructions::LAHF: case ::x86::Instructions::SAHF:
            return EncodeFlagInstruction(instruction, ignoreUnresolved);

        // STACK
        case ::x86::Instructions::PUSHA: case ::x86::Instructions::POPA:
        case ::x86::Instructions::PUSHAD: case ::x86::Instructions::POPAD:
        case ::x86::Instructions::PUSHF: case ::x86::Instructions::POPF:
        case ::x86::Instructions::PUSHFD: case ::x86::Instructions::POPFD:
        case ::x86::Instructions::PUSHFQ: case ::x86::Instructions::POPFQ:
            return EncodeStackInstruction(instruction, ignoreUnresolved);

        // DATA
        case ::x86::Instructions::MOV:
            break;

        default: throw Exception::InternalError("Unknown instruction", instruction.lineNumber, instruction.column);
    }
}

uint64_t Encoder::x86::Encoder::GetSize(const Parser::Instruction::Instruction& instruction)
{
    const std::vector<uint8_t> instr = EncodeInstruction(instruction, true);
    return static_cast<uint64_t>(instr.size());
}

std::vector<uint8_t> Encoder::x86::Encoder::EncodePadding(size_t length)
{
    std::vector<uint8_t> buffer(length, 0x90);  // TODO: not cool
    return buffer;
}
