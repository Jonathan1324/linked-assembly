#include "Encoder.hpp"

#include <x86/Instructions.hpp>

Encoder::x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : ::Encoder::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> Encoder::x86::Encoder::_EncodeInstruction(const Parser::Instruction::Instruction& instruction)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::NOP: return {0x90};

        default: return {0xFF, 0xFF};
    }
}

uint64_t Encoder::x86::Encoder::_GetSize(const Parser::Instruction::Instruction& instruction)
{
    const std::vector<uint8_t> instr = _EncodeInstruction(instruction);
    return instr.size();
}

std::vector<uint8_t> Encoder::x86::Encoder::_EncodePadding(size_t length)
{
    std::vector<uint8_t> buffer(length, 0x90);  // TODO: not cool
    return buffer;
}
