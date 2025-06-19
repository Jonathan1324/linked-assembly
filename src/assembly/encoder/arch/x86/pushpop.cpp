#include "mov.hpp"

#include "../evaluate.hpp"
#include <util/string.hpp>
#include "registers.hpp"
#include "memory.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodePushReg16(std::string reg, Endianness endianness, sectionBuffer& buffer)
        {
            uint8_t regNum = bits16::registers.at(reg);
            uint8_t opcode = 0x50 + regNum;

            buffer.push_back(prefix16);
            buffer.push_back(opcode);

            return 2;
        }

        size_t encodePushSegReg16(std::string reg, Endianness endianness, sectionBuffer& buffer)
        {
            if (reg.compare("es") == 0)
            {
                buffer.push_back(0x06);
                return 1;
            }
            else if (reg.compare("cs") == 0)
            {
                buffer.push_back(0x0E);
                return 1;
            }
            else if (reg.compare("ss") == 0)
            {
                buffer.push_back(0x16);
                return 1;
            }
            else if (reg.compare("ds") == 0)
            {
                buffer.push_back(0x1E);
                return 1;
            }
            else if (reg.compare("fs") == 0)
            {
                buffer.push_back(0x0F);
                buffer.push_back(0xA0);
                return 2;
            }
            else if (reg.compare("gs") == 0)
            {
                buffer.push_back(0x0F);
                buffer.push_back(0xA8);
                return 2;
            }
            //else
            return 0;
        }

        size_t encodePushReg32(std::string reg, Endianness endianness, sectionBuffer& buffer)
        {
            uint8_t regNum = bits32::registers.at(reg);
            uint8_t opcode = 0x50 + regNum;

            buffer.push_back(opcode);

            return 1;
        }

        size_t encodePushImm32(uint32_t imm, Endianness endianness, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x68;
            buffer.push_back(opcode);

            if (endianness == Endianness::Little)
            {
                for (int i = 0; i < 4; i++)
                    buffer.push_back(static_cast<uint8_t>((imm >> (8 * i)) & 0xFF));
            }
            else
            {
                for (int i = 3; i >= 0; i--)
                    buffer.push_back(static_cast<uint8_t>((imm >> (8 * i)) & 0xFF));
            }

            return 5;
        }

        size_t encodePush(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.operands.size() < 1)
                throw Exception::SyntaxError("Not enough operands for push", instr.lineNumber);
            else if (instr.operands.size() > 1)
                throw Exception::SyntaxError("Too many operands for push", instr.lineNumber);

            std::string src = instr.operands[0];

            if (bits16::registers.find(src) != bits16::registers.end())
            {
                offset = encodePushReg16(src, endianness, section.buffer);
            }
            else if (bits16::segmentRegisters.find(src) != bits16::segmentRegisters.end())
            {
                offset = encodePushSegReg16(src, endianness, section.buffer);
            }
            else if (bits32::registers.find(src) != bits32::registers.end())
            {
                offset = encodePushReg32(src, endianness, section.buffer);
            }
            else if (isMemoryOperand(src) != Memory::None)
            {
                // TODO
            }
            else
            {
                unsigned long long val = evaluate(src, constants, instr.lineNumber);
                if (val > 0xFFFFFFFF)
                {
                    throw Exception::OverflowError(src + " too big for push", instr.lineNumber);
                }
                uint32_t imm32 = static_cast<uint32_t>(val);

                //TODO
                offset = encodePushImm32(imm32, endianness, section.buffer);
            }

            return offset;
        }


        size_t encodePopReg16(std::string reg, Endianness endianness, sectionBuffer& buffer)
        {
            uint8_t regNum = bits16::registers.at(reg);
            uint8_t opcode = 0x58 + regNum;

            buffer.push_back(prefix16);
            buffer.push_back(opcode);

            return 2;
        }

        size_t encodePopSegReg16(std::string reg, Endianness endianness, sectionBuffer& buffer)
        {
            if (reg.compare("es") == 0)
            {
                buffer.push_back(0x07);
                return 1;
            }
            else if (reg.compare("cs") == 0)
            {
                throw Exception::SyntaxError("can't pop 'cs'", -1);
            }
            else if (reg.compare("ss") == 0)
            {
                buffer.push_back(0x17);
                return 1;
            }
            else if (reg.compare("ds") == 0)
            {
                buffer.push_back(0x1F);
                return 1;
            }
            else if (reg.compare("fs") == 0)
            {
                buffer.push_back(0x0F);
                buffer.push_back(0xA1);
                return 2;
            }
            else if (reg.compare("gs") == 0)
            {
                buffer.push_back(0x0F);
                buffer.push_back(0xA9);
                return 2;
            }
            //else
            return 0;
        }

        size_t encodePopReg32(std::string reg, Endianness endianness, sectionBuffer& buffer)
        {
            uint8_t regNum = bits32::registers.at(reg);
            uint8_t opcode = 0x58 + regNum;

            buffer.push_back(opcode);

            return 1;
        }

        size_t encodePop(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.operands.size() < 1)
                throw Exception::SyntaxError("Not enough operands for push", instr.lineNumber);
            else if (instr.operands.size() > 1)
                throw Exception::SyntaxError("Too many operands for push", instr.lineNumber);

            std::string src = instr.operands[0];

            if (bits16::registers.find(src) != bits16::registers.end())
            {
                offset = encodePopReg16(src, endianness, section.buffer);
            }
            else if (bits16::segmentRegisters.find(src) != bits16::segmentRegisters.end())
            {
                offset = encodePopSegReg16(src, endianness, section.buffer);
            }
            else if (bits32::registers.find(src) != bits32::registers.end())
            {
                offset = encodePopReg32(src, endianness, section.buffer);
            }
            else if (isMemoryOperand(src) != Memory::None)
            {
                // TODO
            }
            else
            {
                throw Exception::SyntaxError("used pop with imm32", instr.lineNumber);
            }

            return offset;
        }
    }
}