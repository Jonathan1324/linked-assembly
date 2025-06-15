#include "mov.hpp"

#include "../evaluate.hpp"
#include "../../../util/string.hpp"
#include "registers.hpp"
#include "memory.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeMovRegReg(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x89;
            uint8_t mod = 0b11 << 6;
            uint8_t reg = registers.at(toLower(src)) << 3;
            uint8_t rm = registers.at(toLower(dst));

            buffer.push_back(opcode);
            buffer.push_back(mod | reg | rm);

            return 2;
        }

        size_t encodeMovRegImm(uint32_t imm, std::string dst, sectionBuffer& buffer, Endianness endianness)
        {
            uint8_t reg = registers.at(toLower(dst));
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

        size_t encodeMovRegMem(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            MemoryOperand mem = parseMem(src);
            uint8_t opcode = 0x8B;
            uint8_t modrm = 0;
            uint8_t sib = 0;
            bool hasSIB = false;
            size_t size = 1; //opcode

            uint8_t reg = registers.at(toLower(dst));

            // Defaults for SIB
            uint8_t scaleBits = 0;
            uint8_t indexBits = 0xb100; // default: no index
            uint8_t baseBits = 0xb101;  // default: no base (disp32)

            // Determine addressing mode
            bool hasBase = mem.base.has_value();
            bool hasIndex = mem.index.has_value();

            // Starts with ModRM
            if (hasBase)
            {
                std::string base = toLower(mem.base.value());
                baseBits = registers.at(base);

                if (hasIndex)
                {
                    std::string index = toLower(mem.index.value());
                    indexBits = registers.at(index);

                    switch (mem.scale)
                    {
                        case 1: scaleBits = 0b00; break;
                        case 2: scaleBits = 0b01; break;
                        case 4: scaleBits = 0b10; break;
                        case 8: scaleBits = 0b11; break;
                        default:
                            std::cerr << "Invalid scale in memory operand: " << (int)mem.scale << std::endl;
                            return 0;
                    }

                    hasSIB = true;
                }
                else if (baseBits == 0b100)
                {
                    // ESP as base needs SIB even if no index
                    hasSIB = true;
                    indexBits = 0b100; // no index
                    scaleBits = 0b00;
                }

                // Determine Mod field
                if (mem.displacement == 0 && baseBits != 0b101)
                {
                    modrm = (0b00 << 6) | (reg << 3) | 0b100; // Use SIB
                    if (!hasSIB) modrm = (0b00 << 6) | (reg << 3) | baseBits;
                }
                else if (mem.displacement >= -128 && mem.displacement <= 127)
                {
                    modrm = (0b01 << 6) | (reg << 3) | (hasSIB ? 0b100 : baseBits);
                }
                else
                {
                    modrm = (0b10 << 6) | (reg << 3) | (hasSIB ? 0b100 : baseBits);
                }
            }
            else if (mem.label.has_value())
            {
                // [label] — no base or index
                modrm = (0b00 << 6) | (reg << 3) | 0b101; // disp32
            }
            else
            {
                std::cerr << "Invalid memory operand: " << src << std::endl;
                return 0;
            }

            // Emit opcode
            buffer.push_back(opcode);
            buffer.push_back(modrm);
            size += 2;

            // Emit SIB if required
            if (hasSIB)
            {
                sib = (scaleBits << 6) | (indexBits << 3) | baseBits;
                buffer.push_back(sib);
                size += 1;
            }

            // Emit displacement if needed
            if (mem.label.has_value())
            {
                Relocation rel;
                rel.offsetInSection = buffer.size();
                rel.labelName = mem.label.value();
                rel.type = Type::Absolute; // TODO: Could change based on context
                rel.size = 4;
                rel.addend = mem.displacement; // Add any constant offset

                relocations.push_back(rel);

                // Reserve 4 bytes in buffer for the address (to be relocated later)
                for (int i = 0; i < 4; i++)
                    buffer.push_back(0);
                size += 4;
            }
            else if (modrm >> 6 == 0b01) // disp8
            {
                buffer.push_back(static_cast<uint8_t>(mem.displacement & 0xFF));
                size += 1;
            }
            else if (modrm >> 6 == 0b10 || (modrm >> 6 == 0b00 && baseBits == 0b101)) // disp32
            {
                if (endianness == Endianness::Little)
                {
                    for (int i = 0; i < 4; i++)
                        buffer.push_back(static_cast<uint8_t>((mem.displacement >> (8 * i)) & 0xFF));
                }
                else
                {
                    for (int i = 3; i >= 0; i--)
                        buffer.push_back(static_cast<uint8_t>((mem.displacement >> (8 * i)) & 0xFF));
                }
                size += 4;
            }

            return size;
        }

        size_t encodeMovMemReg(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            std::cout << "Warning: mov mem, reg not implemented yet" << std::endl;
            return 0;
        }

        size_t encodeMovMemImm(uint32_t imm, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {
            std::cout << "Warning: mov mem, imm not implemented yet" << std::endl;
            return 0;
        }

        size_t encodeMov(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness)
        {
            size_t offset = 0;

            if (instr.operands.size() < 2)
            {
                std::cerr << "Not enough operands for mov in line " << instr.lineNumber << std::endl;
                return 0;
            }
            else if (instr.operands.size() > 2)
            {
                std::cerr << "Too many operands for mov in line " << instr.lineNumber << std::endl;
                return 0;
            }

            std::string dst = instr.operands[0];
            std::string src = instr.operands[1];

            if (registers.find(dst) != registers.end())
            {
                if (registers.find(src) != registers.end())
                {
                    //src: reg, dst: reg
                    offset = encodeMovRegReg(src, dst, section.buffer);
                }
                else if (isMemoryOperand(src))
                {
                    //src: mem, dst: reg
                    offset = encodeMovRegMem(src, dst, section.buffer, section.relocations, endianness);
                }
                else
                {
                    //src: imm, dst: reg
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);
                    if (val > 0xFFFFFFFF)
                    {
                        std::cerr << src << " too big for " << dst << " in line " << instr.lineNumber << std::endl;
                        return 0;
                    }
                    uint32_t imm32 = static_cast<uint32_t>(val);

                    offset = encodeMovRegImm(imm32, dst, section.buffer, endianness);
                }
            }
            else if (isMemoryOperand(dst))
            {
                if (registers.find(src) != registers.end())
                {
                    //src: reg, dst: mem
                    offset = encodeMovMemReg(src, dst, section.buffer, section.relocations, endianness);
                }
                else if (isMemoryOperand(dst))
                {
                    //src: mem, dst: mem
                    std::cerr << "mov mem, mem in line " << instr.lineNumber << std::endl;
                    return 0;
                }
                else
                {
                    //src: imm, dst: mem
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);
                    if (val > 0xFFFFFFFF)
                    {
                        std::cerr << src << " too big for " << dst << " in line " << instr.lineNumber << std::endl;
                        return 0;
                    }
                    uint32_t imm32 = static_cast<uint32_t>(val);

                    offset = encodeMovMemImm(imm32, dst, section.buffer, section.relocations, endianness);
                }
            }
            else
            {
                std::cerr << "mov with imm as destination in line " << instr.lineNumber << std::endl;
                return 0;
            }

            return offset;
        }
    }
}