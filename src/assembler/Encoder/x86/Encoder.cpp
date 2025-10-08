#include "Encoder.hpp"

x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : ::Encoder::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> x86::Encoder::EncodeInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        // CONTROL
        case Instructions::NOP:
        case Instructions::HLT:
            return EncodeControlInstruction(instruction, ignoreUnresolved, optimize);

        // INTERRUPT
        case Instructions::INT:
            return EncodeInterruptInstruction(instruction, ignoreUnresolved, optimize);

        // FLAGS
        case Instructions::CLC: case Instructions::STC: case Instructions::CMC:
        case Instructions::CLD: case Instructions::STD:
        case Instructions::CLI: case Instructions::STI:
        case Instructions::LAHF: case Instructions::SAHF:
            return EncodeFlagInstruction(instruction, ignoreUnresolved, optimize);

        // STACK
        case Instructions::PUSHA: case Instructions::POPA:
        case Instructions::PUSHAD: case Instructions::POPAD:
        case Instructions::PUSHF: case Instructions::POPF:
        case Instructions::PUSHFD: case Instructions::POPFD:
        case Instructions::PUSHFQ: case Instructions::POPFQ:
            return EncodeStackInstruction(instruction, ignoreUnresolved, optimize);

        // DATA
        case Instructions::MOV:
            return EncodeDataInstruction(instruction, ignoreUnresolved, optimize);

        default: throw Exception::InternalError("Unknown instruction", instruction.lineNumber, instruction.column);
    }
}

bool x86::Encoder::OptimizeOffsets(std::vector<Parser::Section>& parsedSections)
{
    bool changed = true;
    // TODO
    return true;
}

uint64_t x86::Encoder::GetSize(Parser::Instruction::Instruction& instruction)
{
    const std::vector<uint8_t> instr = EncodeInstruction(instruction, true, false);
    return static_cast<uint64_t>(instr.size());
}

std::vector<uint8_t> x86::Encoder::EncodePadding(size_t length)
{
    std::vector<uint8_t> buffer(length, 0x90);  // TODO: not cool
    return buffer;
}
