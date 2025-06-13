#include "x86.hpp"
#include "x86/registers.hpp"
#include "x86/no_operands.hpp"
#include "x86/datatransfer.hpp"

namespace x86 {
    size_t encode16(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants)
    {
        sectionBuffer& buffer = section.buffer;
        size_t offset = 0;

        (void)instr;
        (void)constants;
        (void)buffer;
        //TODO
        return offset;
    }

    size_t encode32(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants)
    {
        size_t offset = 0;

        offset = bits32::encodeNoOperands(instr, section, constants);
        if (offset > 0)
            return offset;
        
        offset = bits32::encodeDataTransfer(instr, section, constants);
        if (offset > 0)
            return offset;

        return offset;
    }

    size_t encode64(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants)
    {
        sectionBuffer& buffer = section.buffer;
        size_t offset = 0;

        (void)instr;
        (void)constants;
        (void)buffer;
        //TODO
        return offset;
    }

    size_t encodeInstruction(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants)
    {
        size_t offset;
        switch(instr.mode)
        {
            case BitMode::Bits16:
                offset = encode16(instr, section, constants);
                break;
            case BitMode::Bits32:
                offset = encode32(instr, section, constants);
                break;
            case BitMode::Bits64:
                offset = encode64(instr, section, constants);
                break;
            default:
                offset = 0;
                std::cerr << "Unknown bitmode on line " << instr.lineNumber << std::endl;
                break;
        }

        if (offset <= 0)
        {
            std::cerr << "Error in line " << instr.lineNumber << std::endl;
            return 0;
        }

        return offset;
    }
}