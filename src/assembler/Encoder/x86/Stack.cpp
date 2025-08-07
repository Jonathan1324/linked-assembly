#include "Encoder.hpp"

std::vector<uint8_t> Encoder::x86::Encoder::EncodeStackInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::PUSHA:
            if (instruction.bits == BitMode::Bits16 || instruction.bits == BitMode::Bits32) return {0x60};
            if (instruction.bits == BitMode::Bits64) throw Exception::SyntaxError("'pusha' not supported in 64-bit mode", instruction.lineNumber, instruction.column);
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);
        case ::x86::Instructions::POPA:
            if (instruction.bits == BitMode::Bits16 || instruction.bits == BitMode::Bits32) return {0x61};
            if (instruction.bits == BitMode::Bits64) throw Exception::SyntaxError("'pusha' not supported in 64-bit mode", instruction.lineNumber, instruction.column);
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);

        case ::x86::Instructions::PUSHAD: 
            if (instruction.bits == BitMode::Bits16) return {0x66, 0x60};   // 0x66 prefix: switch from 16-bit to 32-bit
            if (instruction.bits == BitMode::Bits32) return {0x60};
            if (instruction.bits == BitMode::Bits64) throw Exception::SyntaxError("'pushad' not supported in 64-bit mode", instruction.lineNumber, instruction.column);
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);
        case ::x86::Instructions::POPAD:
            if (instruction.bits == BitMode::Bits16) return {0x66, 0x61};   // 0x66 prefix: switch from 16-bit to 32-bit
            if (instruction.bits == BitMode::Bits32) return {0x61};
            if (instruction.bits == BitMode::Bits64) throw Exception::SyntaxError("'popad' not supported in 64-bit mode", instruction.lineNumber, instruction.column);
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);

        case ::x86::Instructions::PUSHF: return {0x9C};
        case ::x86::Instructions::POPF: return {0x9D};

        case ::x86::Instructions::PUSHFD: 
            if (instruction.bits == BitMode::Bits16) return {0x66, 0x9C};   // 0x66 prefix: switch from 16-bit to 32-bit
            if (instruction.bits == BitMode::Bits32) return {0x9C};
            if (instruction.bits == BitMode::Bits64) throw Exception::SyntaxError("'pushfd' not supported in 64-bit mode", instruction.lineNumber, instruction.column);
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);
        case ::x86::Instructions::POPFD:
            if (instruction.bits == BitMode::Bits16) return {0x66, 0x9D};   // 0x66 prefix: switch from 16-bit to 32-bit
            if (instruction.bits == BitMode::Bits32) return {0x9D};
            if (instruction.bits == BitMode::Bits64) throw Exception::SyntaxError("'popfd' not supported in 64-bit mode", instruction.lineNumber, instruction.column);
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);
        
        case ::x86::Instructions::PUSHFQ: 
            if (instruction.bits == BitMode::Bits16) throw Exception::SyntaxError("'pushfq' not supported in 16-bit mode", instruction.lineNumber, instruction.column);
            if (instruction.bits == BitMode::Bits32) throw Exception::SyntaxError("'pushfq' not supported in 32-bit mode", instruction.lineNumber, instruction.column);
            if (instruction.bits == BitMode::Bits64) return {0x9C};
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);
        case ::x86::Instructions::POPFQ:
            if (instruction.bits == BitMode::Bits16) throw Exception::SyntaxError("'popfq' not supported in 16-bit mode", instruction.lineNumber, instruction.column);
            if (instruction.bits == BitMode::Bits32) throw Exception::SyntaxError("'popfq' not supported in 32-bit mode", instruction.lineNumber, instruction.column);
            if (instruction.bits == BitMode::Bits64) return {0x9D};
            throw Exception::InternalError("Unknown bit mode", instruction.lineNumber, instruction.column);

        default: throw Exception::InternalError("Unknown stack instruction", instruction.lineNumber, instruction.column);
    }
}