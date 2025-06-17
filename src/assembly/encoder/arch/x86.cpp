#include "x86.hpp"
#include "x86/registers.hpp"
#include "x86/no_operands.hpp"
#include "x86/datatransfer.hpp"

namespace x86 {
    size_t encode16(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
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

    size_t encode32(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
    {
        size_t offset = 0;

        offset = bits32::encodeNoOperands(instr, section, constants, endianness, context);
        if (offset > 0)
            return offset;
        
        offset = bits32::encodeDataTransfer(instr, section, constants, endianness, context);
        if (offset > 0)
            return offset;

        return offset;
    }

    size_t encode64(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
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

    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
    {
        size_t offset;
        switch(instr.mode)
        {
            case BitMode::Bits16:
                offset = encode16(instr, section, constants, endianness, context);
                break;
            case BitMode::Bits32:
                offset = encode32(instr, section, constants, endianness, context);
                break;
            case BitMode::Bits64:
                offset = encode64(instr, section, constants, endianness, context);
                break;
            default:
                throw Exception::InternalError("Unknown bitmode");
                break;
        }

        if (offset <= 0)
        {
            throw Exception::InternalError("Error on line " + instr.lineNumber);
            return 0;
        }

        return offset;
    }
}