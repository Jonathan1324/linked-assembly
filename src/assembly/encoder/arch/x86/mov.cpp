#include "mov.hpp"

#include "../evaluate.hpp"
#include <util/string.hpp>
#include "registers.hpp"
#include "memory.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeMovRegReg8(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x88;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits8::registers.at(src);
            uint8_t rm = bits8::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 2;
        }

        size_t encodeMovRegMem8(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovRegImm8(uint8_t imm, std::string dst, sectionBuffer& buffer, Endianness endianness)
        {
            uint8_t reg = bits8::registers.at(toLower(dst));
            uint8_t opcode = 0xB0 + reg;

            buffer.push_back(opcode);
            buffer.push_back(imm);

            return 2;
        }

        size_t encodeMovRegReg16(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x89;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits16::registers.at(src);
            uint8_t rm = bits16::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(prefix16);
            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 3;
        }

        size_t encodeMovRegSegReg16(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x8C;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits16::segmentRegisters.at(src);
            uint8_t rm = bits16::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(prefix16);
            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 3;
        }

        size_t encodeMovRegMem16(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovRegImm16(uint16_t imm, std::string dst, sectionBuffer& buffer, Endianness endianness)
        {
            uint8_t prefix = 0x66;

            uint8_t reg = bits16::registers.at(toLower(dst));
            uint8_t opcode = 0xB8 + reg;

            buffer.push_back(prefix);
            buffer.push_back(opcode);

            if (endianness == Endianness::Little)
            {
                buffer.push_back(static_cast<uint8_t>(imm & 0xFF));
                buffer.push_back(static_cast<uint8_t>((imm >> 8) & 0xFF));
            }
            else
            {
                buffer.push_back(static_cast<uint8_t>((imm >> 8) & 0xFF));
                buffer.push_back(static_cast<uint8_t>(imm & 0xFF));
            }

            return 4;
        }

        size_t encodeMovSegRegReg16(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x8E;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits16::segmentRegisters.at(dst);
            uint8_t rm = bits16::registers.at(src);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 2;
        }

        size_t encodeMovSegRegMem16(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovSegRegReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x8E;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits16::segmentRegisters.at(dst);
            uint8_t rm = bits32::registers.at(src);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 2;
        }

        size_t encodeMovRegReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x89;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits32::registers.at(src);
            uint8_t rm = bits32::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 2;
        }

        size_t encodeMovRegSegReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x8C;
            uint8_t mod = 0b11;   // reg to reg
            uint8_t reg = bits16::segmentRegisters.at(src);
            uint8_t rm = bits32::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 2;
        }

        size_t encodeMovRegCrReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t prefix = 0x0F;
            uint8_t opcode = 0x20;
            uint8_t mod = 0b11;
            uint8_t reg = bits32::controlRegisters.at(src);
            uint8_t rm = bits32::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(prefix);
            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 3;
        }

        size_t encodeMovRegDrReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t prefix = 0x0F;
            uint8_t opcode = 0x21;
            uint8_t mod = 0b11;
            uint8_t reg = bits32::debugRegisters.at(src);
            uint8_t rm = bits32::registers.at(dst);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(prefix);
            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 3;
        }

        size_t encodeMovReg32Mem(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovRegImm32(uint32_t imm, std::string dst, sectionBuffer& buffer, Endianness endianness)
        {
            uint8_t reg = bits32::registers.at(toLower(dst));
            uint8_t opcode = 0xB8 + reg;
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

        size_t encodeMovCrRegReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t prefix = 0x0F;
            uint8_t opcode = 0x22;
            uint8_t mod = 0b11;
            uint8_t reg = bits32::controlRegisters.at(dst);
            uint8_t rm = bits32::registers.at(src);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(prefix);
            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 3;
        }

        size_t encodeMovDrRegReg32(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t prefix = 0x0F;
            uint8_t opcode = 0x23;
            uint8_t mod = 0b11;
            uint8_t reg = bits32::debugRegisters.at(dst);
            uint8_t rm = bits32::registers.at(src);
            uint8_t modrm = (mod << 6) | (reg << 3) | rm;

            buffer.push_back(prefix);
            buffer.push_back(opcode);
            buffer.push_back(modrm);

            return 3;
        }

        size_t encodeMovMemReg8(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovMemReg16(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovMemSegReg16(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovMemReg32(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovMemImm8(uint8_t imm, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovMemImm16(uint16_t imm, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMovMemImm32(uint32_t imm, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            // TODO
            return 0;
        }

        size_t encodeMov(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            size_t offset = 0;

            if (instr.operands.size() < 2)
                throw Exception::SyntaxError("Not enough operands for mov", instr.lineNumber);
            else if (instr.operands.size() > 2)
                throw Exception::SyntaxError("Too many operands for mov", instr.lineNumber);

            std::string dst = toLower(instr.operands[0]);
            std::string src = toLower(instr.operands[1]);

            if (bits8::registers.find(dst) != bits8::registers.end())
            {
                if (bits8::registers.find(src) != bits8::registers.end())
                {
                    offset = encodeMovRegReg8(src, dst, section.buffer);
                }
                else if (isMemoryOperand(src) != Memory::None)
                {
                    Memory type = isMemoryOperand(src);
                    if (type == Memory::Default || type == Memory::Byte)
                        // default or specific 8-bit
                        offset = encodeMovRegMem8(src, dst, section.buffer, section.relocations, endianness);
                    else
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                }
                else
                {
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);
                    if (val > 0xFF)
                    {
                        throw Exception::OverflowError(src + " too large for " + dst, instr.lineNumber);
                        return 0;
                    }
                    uint8_t imm8 = static_cast<uint8_t>(val);

                    offset = encodeMovRegImm8(imm8, dst, section.buffer, endianness);
                }
            }
            else if (bits16::registers.find(dst) != bits16::registers.end())
            {
                if (bits16::registers.find(src) != bits16::registers.end())
                {
                    offset = encodeMovRegReg16(src, dst, section.buffer);
                }
                else if (bits16::segmentRegisters.find(src) != bits16::segmentRegisters.end())
                {
                    offset = encodeMovRegSegReg16(src, dst, section.buffer);
                }
                else if (isMemoryOperand(src) != Memory::None)
                {
                    Memory type = isMemoryOperand(src);
                    if (type == Memory::Default || type == Memory::Word)
                        // default or specific 16-bit
                        offset = encodeMovRegMem16(src, dst, section.buffer, section.relocations, endianness);
                    else
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                }
                else
                {
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);
                    if (val > 0xFFFF)
                    {
                        throw Exception::OverflowError(src + " too large for " + dst, instr.lineNumber);
                        return 0;
                    }
                    uint16_t imm16 = static_cast<uint16_t>(val);

                    offset = encodeMovRegImm16(imm16, dst, section.buffer, endianness);
                }
            }
            else if (bits16::segmentRegisters.find(dst) != bits16::segmentRegisters.end())
            {
                if (bits16::registers.find(src) != bits16::registers.end())
                {
                    offset = encodeMovSegRegReg16(src, dst, section.buffer);
                }
                else if (bits32::registers.find(src) != bits32::registers.end())
                {
                    offset = encodeMovSegRegReg32(src, dst, section.buffer);
                }
                else if (isMemoryOperand(src) != Memory::None)
                {
                    Memory type = isMemoryOperand(src);
                    if (type == Memory::Default || type == Memory::Word)
                        // default or specific 16-bit
                        encodeMovSegRegMem16(src, dst, section.buffer, section.relocations, endianness);
                    else
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                }
            }
            else if (bits32::registers.find(dst) != bits32::registers.end())
            {
                if (bits32::registers.find(src) != bits32::registers.end())
                {
                    offset = encodeMovRegReg32(src, dst, section.buffer);
                }
                else if (bits16::segmentRegisters.find(src) != bits16::segmentRegisters.end())
                {
                    offset = encodeMovRegSegReg32(src, dst, section.buffer);
                }
                else if (bits32::controlRegisters.find(src) != bits32::controlRegisters.end())
                {
                    offset = encodeMovRegCrReg32(src, dst, section.buffer);
                }
                else if (bits32::debugRegisters.find(src) != bits32::debugRegisters.end())
                {
                    offset = encodeMovRegDrReg32(src, dst, section.buffer);
                }
                else if (isMemoryOperand(src) != Memory::None)
                {
                    Memory type = isMemoryOperand(src);
                    if (type == Memory::Default || type == Memory::Dword)
                        // default or specific 32-bit
                        offset = encodeMovReg32Mem(src, dst, section.buffer, section.relocations, endianness);
                    else
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                }
                else
                {
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);
                    if (val > 0xFFFFFFFF)
                    {
                        throw Exception::OverflowError(src + " too big for " + dst, instr.lineNumber);
                    }
                    uint32_t imm32 = static_cast<uint32_t>(val);

                    offset = encodeMovRegImm32(imm32, dst, section.buffer, endianness);
                }
            }
            else if (bits32::controlRegisters.find(dst) != bits32::controlRegisters.end())
            {
                if (bits32::registers.find(src) != bits32::registers.end())
                {
                    offset = encodeMovCrRegReg32(src, dst, section.buffer);
                }
            }
            else if (bits32::debugRegisters.find(dst) != bits32::debugRegisters.end())
            {
                if (bits32::registers.find(src) != bits32::registers.end())
                {
                    offset = encodeMovDrRegReg32(src, dst, section.buffer);
                }
            }
            else if (isMemoryOperand(dst) != Memory::None)
            {
                if (bits8::registers.find(src) != bits8::registers.end())
                {
                    Memory type = isMemoryOperand(dst);
                    if (type != Memory::Default && type != Memory::Byte)
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                    offset = encodeMovMemReg8(src, dst, section.buffer, section.relocations, endianness);
                }
                else if (bits16::registers.find(src) != bits16::registers.end())
                {
                    Memory type = isMemoryOperand(dst);
                    if (type != Memory::Default && type != Memory::Word)
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                    offset = encodeMovMemReg16(src, dst, section.buffer, section.relocations, endianness);
                }
                else if (bits16::segmentRegisters.find(src) != bits16::segmentRegisters.end())
                {
                    Memory type = isMemoryOperand(dst);
                    if (type != Memory::Default && type != Memory::Word)
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                    offset = encodeMovMemSegReg16(src, dst, section.buffer, section.relocations, endianness);
                }
                else if (bits32::registers.find(src) != bits32::registers.end())
                {
                    Memory type = isMemoryOperand(dst);
                    if (type != Memory::Default && type != Memory::Dword)
                    {
                        //wrong type
                        throw Exception::SemanticError("Memory size and register mismatch", instr.lineNumber);
                    }
                    offset = encodeMovMemReg32(src, dst, section.buffer, section.relocations, endianness);
                }
                else
                {
                    Memory type = isMemoryOperand(dst);
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);

                    uint8_t imm8;
                    uint16_t imm16;
                    uint32_t imm32;
                    switch (type)
                    {
                    case Memory::Byte:
                        if (val > 0xFF)
                            throw Exception::OverflowError(src + " too big for a byte", instr.lineNumber);
                        imm8 = static_cast<uint8_t>(val);
                        offset = encodeMovMemImm8(imm8, dst, section.buffer, section.relocations, endianness);
                        break;
                    
                    case Memory::Word:
                        if (val > 0xFFFF)
                            throw Exception::OverflowError(src + " too big for a word", instr.lineNumber);
                        imm16 = static_cast<uint16_t>(val);
                        offset = encodeMovMemImm16(imm16, dst, section.buffer, section.relocations, endianness);
                        break;
                    
                    case Memory::Dword:
                    case Memory::Default:
                        if (val > 0xFFFFFFFF)
                            throw Exception::OverflowError(src + " too big for a dword", instr.lineNumber);
                        imm32 = static_cast<uint32_t>(val);
                        offset = encodeMovMemImm32(imm32, dst, section.buffer, section.relocations, endianness);
                        break;
                    
                    default:
                        throw Exception::SyntaxError(dst + " not supported using mov", instr.lineNumber);
                    }
                }
            }

            // error
            else if (bits16::flagsRegister.compare(dst) == 0 || bits32::flagsRegister.compare(dst) == 0)
                throw Exception::SyntaxError("mov used eflags", instr.lineNumber);
            else
                throw Exception::SyntaxError("mov used imm as destination", instr.lineNumber);

            return offset;
        }
    }
}