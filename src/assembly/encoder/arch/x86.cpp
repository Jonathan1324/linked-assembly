#include "x86.hpp"
#include "x86/registers.hpp"
#include "x86/no_operands.hpp"
#include "x86/datatransfer.hpp"
#include "x86/stack.hpp"

namespace x86 {
    namespace bits16 {
        size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            sectionBuffer& buffer = section.buffer;
            size_t offset = 0;

            (void)instr;
            (void)constants;
            (void)buffer;
            (void)endianness;
            (void)context;
            //TODO
            return offset;
        }
    }

    namespace bits32 {
        size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            offset = encodeNoOperands(instr, section, constants, endianness, context);
            if (offset > 0)
                return offset;
            
            offset = encodeDataTransfer(instr, section, constants, endianness, context);
            if (offset > 0)
                return offset;

            offset = encodeStack(instr, section, constants, endianness, context);
            if (offset > 0)
                return offset;

            return offset;
        }
    }

    namespace bits64 {
        size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            sectionBuffer& buffer = section.buffer;
            size_t offset = 0;

            (void)instr;
            (void)constants;
            (void)buffer;
            (void)endianness;
            (void)context;
            //TODO
            return offset;
        }
    }

    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
    {
        size_t offset;
        switch(instr.mode)
        {
            case BitMode::Bits16:
                offset = bits16::encodeInstruction(instr, section, constants, endianness, context);
                break;
            case BitMode::Bits32:
                offset = bits32::encodeInstruction(instr, section, constants, endianness, context);
                break;
            case BitMode::Bits64:
                offset = bits64::encodeInstruction(instr, section, constants, endianness, context);
                break;
            default:
                throw Exception::InternalError("Unknown bitmode");
                break;
        }

        if (offset <= 0)
        {
            throw Exception::InternalError("Error on line " + std::to_string(instr.lineNumber));
            return 0;
        }

        return offset;
    }
}