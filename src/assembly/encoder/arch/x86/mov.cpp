#include "mov.hpp"

#include "../evaluate.hpp"

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

        size_t encodeMovRegImm(uint32_t imm, std::string dst, sectionBuffer& buffer)
        {
            uint8_t reg = registers[dst];
            uint8_t opcode = 0xB8 + reg;
            buffer.push_back(opcode);

            for (int i = 0; i < 4; i++)
                //little endian
                buffer.push_back((imm >> (8 * i)) & 0xFF);

            return 5;
        }

        size_t encodeMov(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants)
        {
            size_t offset = 0;

            if (instr.operands.size() < 2)
            {
                std::cerr << "Not enough operands for mov in line " << instr.lineNumber << std::endl;
                return 0;
            }

            std::string dst = instr.operands[0];
            std::string src = instr.operands[1];

            if (registers.find(dst) != registers.end())
            {
                if (registers.find(src) != registers.end())
                {
                    offset = encodeMovRegReg(src, dst, section.buffer);
                }
                else
                {
                    unsigned long long val = evaluate(src, constants, instr.lineNumber);
                    if (val > 0xFFFFFFFF)
                    {
                        std::cerr << src << " too big for " << dst << " in line " << instr.lineNumber << std::endl;
                        return 0;
                    }
                    uint32_t imm32 = static_cast<uint32_t>(val);

                    offset = encodeMovRegImm(imm32, dst, section.buffer);
                }
            }
            else
            {
                //TODO
                std::cerr << "We don't support mov with memory yet: line " << instr.lineNumber << std::endl;
            }

            return offset;
        }
    }
}