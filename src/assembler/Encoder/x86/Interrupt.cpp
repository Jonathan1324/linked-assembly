#include "Encoder.hpp"

std::vector<uint8_t> x86::Encoder::EncodeInterruptInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case Instructions::INT:
        {
            uint8_t opcode = 0xCD;

            if (instruction.operands.empty())
                throw Exception::InternalError("INT has no operands", instruction.lineNumber, instruction.column);
            if (!std::holds_alternative<Parser::Immediate>(instruction.operands[0]))
                throw Exception::InternalError("INT has wrong operand type", instruction.lineNumber, instruction.column);

            const Parser::Immediate& immediate = std::get<Parser::Immediate>(instruction.operands[0]);

            if (!Resolvable(immediate) && ignoreUnresolved)
                return {opcode, 0};

            uint8_t interrupt;

            ::Encoder::Evaluation interruptEval = Evaluate(immediate, bytesWritten, sectionOffset, currentSection);
            if (interruptEval.useOffset)
            {
                interrupt = interruptEval.offset;
                ::Encoder::Relocation reloc;
                reloc.offsetInSection = sectionOffset + 1; // opcode
                reloc.addend = interruptEval.offset;
                reloc.addendInCode = true;
                reloc.section = *currentSection;
                reloc.usedSection = interruptEval.usedSection;
                reloc.type = ::Encoder::RelocationType::Absolute;
                reloc.size = ::Encoder::RelocationSize::Bit8;
                relocations.push_back(std::move(reloc));
            }
            else
            {
                Int128& interrupt128 = interruptEval.result;

                if (interrupt128 < 0) throw Exception::SemanticError("'int' can't have a negative operand", instruction.lineNumber, instruction.column);
                if (interrupt128 > 255) throw Exception::SemanticError("Operand too large for 'int'", instruction.lineNumber, instruction.column);

                interrupt = static_cast<uint8_t>(interrupt128);
            }
            return {opcode, interrupt};
        }

        default: throw Exception::InternalError("Unknown interrupt instruction", instruction.lineNumber, instruction.column);
    }
}