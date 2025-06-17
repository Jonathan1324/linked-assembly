#include "no_operands.hpp"

#include "../evaluate.hpp"

namespace x86 {
    namespace bits32 {
        size_t encodeNoOperands(Instruction& instr, EncodedSection& section, std::unordered_map<std::string, std::string> constants, Endianness endianness, Context& context)
        {
            sectionBuffer& buffer = section.buffer;
            size_t offset = 0;

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

                    std::string operand = instr.operands[0];

                    uint16_t imm16 = 0;
                    try
                    {
                        unsigned long long val = evaluate(operand, constants, instr.lineNumber);

                        if (val > 0xFFFF)
                            throw Exception::OverflowError(operand + " too large for 16 bit", instr.lineNumber);

                        imm16 = static_cast<uint16_t>(val);

                    }
                    catch (const std::invalid_argument& e)
                    {
                        throw Exception::SyntaxError(operand + " not a number", instr.lineNumber);
                    }
                    catch (const std::out_of_range& e)
                    {
                        throw Exception::OverflowError(operand + " too large", instr.lineNumber);
                    }

                    if (endianness == Endianness::Little)
                    {
                        buffer.push_back(static_cast<uint8_t>(imm16 & 0xFF));       // low byte
                        buffer.push_back(static_cast<uint8_t>((imm16 >> 8) & 0xFF)); // high byte
                    }
                    else
                    {
                        buffer.push_back(static_cast<uint8_t>((imm16 >> 8) & 0xFF)); // high byte
                        buffer.push_back(static_cast<uint8_t>(imm16 & 0xFF));       // low byte
                    }
                    offset += 2;
                }
            }
            else if (instr.mnemonic.compare("int3") == 0)
            {
                buffer.push_back(0xCC);
                offset++;
            }
            else if (instr.mnemonic.compare("int1") == 0)
            {
                buffer.push_back(0xF1);
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
            else if (instr.mnemonic.compare("pause") == 0)
            {
                buffer.push_back(0xF3);
                buffer.push_back(0x90);
                offset += 2;
            }
            else if (instr.mnemonic.compare("rdtsc") == 0)
            {
                buffer.push_back(0x0F);
                buffer.push_back(0x31);
                offset += 2;
            }
            else if (instr.mnemonic.compare("cpuid") == 0)
            {
                buffer.push_back(0x0F);
                buffer.push_back(0xA2);
                offset += 2;
            }

            return offset;
        }
    }
}