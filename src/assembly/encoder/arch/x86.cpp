#include "x86.hpp"
#include "x86/registers.hpp"

namespace x86 {
    size_t encode16(Instruction& instr, EncodedSection& section)
    {
        sectionBuffer& buffer = section.buffer;
        size_t offset = -1;

        //TODO
        return offset;
    }

    size_t encode32(Instruction& instr, EncodedSection& section)
    {
        sectionBuffer& buffer = section.buffer;
        size_t offset = -1;

        if (instr.mnemonic.compare("nop") == 0)
        {
            buffer.push_back(0x90);
            offset++;
        }
        else if (instr.mnemonic.compare("ret") == 0)
        {
            if (instr.operands.empty())
            {
                buffer.push_back(0xC3);
                offset++;
            }
            else
            {
                buffer.push_back(0xC2);
                offset++;

                uint16_t imm16 = 0;

                //TODO
                imm16 = std::stoul(instr.operands[0], nullptr, 0);

                // low byte
                buffer.push_back(static_cast<uint8_t>(imm16 & 0xFF));
                offset++;

                // high byte
                buffer.push_back(static_cast<uint8_t>((imm16 >> 8) & 0xFF));
                offset++;
            }
        }
        else if (instr.mnemonic.compare("int3") == 0)
        {
            buffer.push_back(0xCC);
            offset++;
        }
        else if (instr.mnemonic.compare("cli") == 0)
        {
            buffer.push_back(0xFA);
            offset++;
        }
        else if (instr.mnemonic.compare("sti") == 0)
        {
            buffer.push_back(0xFB);
            offset++;
        }
        else if (instr.mnemonic.compare("hlt") == 0)
        {
            buffer.push_back(0xF4);
            offset++;
        }
        else if (instr.mnemonic.compare("cmc") == 0)
        {
            buffer.push_back(0xF5);
            offset++;
        }
        else if (instr.mnemonic.compare("clc") == 0)
        {
            buffer.push_back(0xF8);
            offset++;
        }
        else if (instr.mnemonic.compare("stc") == 0)
        {
            buffer.push_back(0xF9);
            offset++;
        }
        else if (instr.mnemonic.compare("cld") == 0)
        {
            buffer.push_back(0xFC);
            offset++;
        }
        else if (instr.mnemonic.compare("std") == 0)
        {
            buffer.push_back(0xFD);
            offset++;
        }
        else if (instr.mnemonic.compare("wait") == 0 || instr.mnemonic.compare("fwait") == 0)
        {
            buffer.push_back(0x9B);
            offset++;
        }

        return offset;
    }

    size_t encode64(Instruction& instr, EncodedSection& section)
    {
        sectionBuffer& buffer = section.buffer;
        size_t offset = -1;

        //TODO
        return offset;
    }

    size_t encodeInstruction(Instruction& instr, EncodedSection& section)
    {
        size_t offset = 0;

        switch(instr.mode)
        {
            case BitMode::Bits16:
                offset = encode16(instr, section);
                break;
            case BitMode::Bits32:
                offset = encode32(instr, section);
                break;
            case BitMode::Bits64:
                offset = encode64(instr, section);
                break;
            default:
                offset = -1;
                std::cerr << "Unknown bitmode on line " << instr.lineNumber << std::endl;
                break;
        }

        if (offset < 0)
        {
            std::cerr << "Error in line " << instr.lineNumber << std::endl;
            return 0;
        }
    }
}