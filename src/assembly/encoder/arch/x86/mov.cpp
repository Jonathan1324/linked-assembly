#include "mov.hpp"

#include "../evaluate.hpp"

bool isMemoryOperand(const std::string& op) {
    return !op.empty() && op.front() == '[' && op.back() == ']';
}

namespace x86 {
    namespace bits32 {
        std::unordered_map<std::string, uint32_t> registers = 
        {
            {"eax", 0x0},
            {"ecx", 0x1},
            {"edx", 0x2},
            {"ebx", 0x3},
            {"esp", 0x4},
            {"ebp", 0x5},
            {"esi", 0x6},
            {"edi", 0x7},
        };

        size_t encodeMovRegReg(std::string src, std::string dst, sectionBuffer& buffer)
        {
            uint8_t opcode = 0x89;
            uint8_t mod = 0b11 << 6;
            uint8_t reg = registers[src] << 3;
            uint8_t rm = registers[dst];

            buffer.push_back(opcode);
            buffer.push_back(mod | reg | rm);

            return 2;
        }

        size_t encodeMovRegImm(uint32_t imm, std::string dst, sectionBuffer& buffer, Endianness endianness)
        {
            uint8_t reg = registers[dst];
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

        }

        size_t encodeMovMemReg(std::string src, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {

        }

        size_t encodeMovMemImm(uint32_t imm, std::string dst, sectionBuffer& buffer, std::vector<Relocation>& relocations, Endianness endianness)
        {

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