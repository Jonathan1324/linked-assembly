#include "Encoder.hpp"

x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : ::Encoder::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> x86::Encoder::EncodeInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    instrUse16BitPrefix = false;
    
    std::vector<uint8_t> instr;
    switch (instruction.mnemonic)
    {
        // CONTROL
        case Instructions::NOP:
        case Instructions::HLT:
            instr = EncodeControlInstruction(instruction, ignoreUnresolved, optimize);
            break;

        // INTERRUPT
        case Instructions::INT: case Instructions::IRET:
        case Instructions::IRETQ: case Instructions::IRETD:
        case Instructions::SYSCALL: case x86::Instructions::SYSRET:
        case x86::Instructions::SYSENTER: case x86::Instructions::SYSEXIT:
            instr = EncodeInterruptInstruction(instruction, ignoreUnresolved, optimize);
            break;

        // FLAGS
        case Instructions::CLC: case Instructions::STC: case Instructions::CMC:
        case Instructions::CLD: case Instructions::STD:
        case Instructions::CLI: case Instructions::STI:
        case Instructions::LAHF: case Instructions::SAHF:
            instr = EncodeFlagInstruction(instruction, ignoreUnresolved, optimize);
            break;

        // STACK
        case Instructions::PUSHA: case Instructions::POPA:
        case Instructions::PUSHAD: case Instructions::POPAD:
        case Instructions::PUSHF: case Instructions::POPF:
        case Instructions::PUSHFD: case Instructions::POPFD:
        case Instructions::PUSHFQ: case Instructions::POPFQ:
            instr = EncodeStackInstruction(instruction, ignoreUnresolved, optimize);
            break;

        // DATA
        case Instructions::MOV:
            instr = EncodeDataInstruction(instruction, ignoreUnresolved, optimize);
            break;

        default: throw Exception::InternalError("Unknown instruction", instruction.lineNumber, instruction.column);
    }

    std::vector<uint8_t> buf;

    if (instrUse16BitPrefix) buf.push_back(0x66);

    buf.insert(buf.end(), instr.begin(), instr.end());
    return buf;
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
