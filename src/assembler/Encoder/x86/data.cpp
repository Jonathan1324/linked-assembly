#include "Encoder.hpp"

#include <x86/Registers.hpp>
#include <limits>
#include <cstring>

void appendImmediate(std::vector<uint8_t> &buf, uint64_t value, uint32_t sizeInBits)
{
    uint32_t sizeInBytes = sizeInBits / 8;
    size_t oldSize = buf.size();
    buf.resize(oldSize + sizeInBytes);
    std::memcpy(buf.data() + oldSize, &value, sizeInBytes);
}

std::vector<uint8_t> Encoder::x86::Encoder::EncodeDataInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case ::x86::Instructions::MOV:
        {
            if (instruction.operands.size() != 2)
                throw Exception::InternalError("Wrong argument count for 'mov'");

            Parser::Instruction::Operand destinationOperand = instruction.operands[0];
            Parser::Instruction::Operand sourceOperand = instruction.operands[1];

            std::vector<uint8_t> instr;

            if (std::holds_alternative<Parser::Instruction::Register>(destinationOperand))
            {
                Parser::Instruction::Register destReg = std::get<Parser::Instruction::Register>(destinationOperand);
                
                if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                {
                    Parser::Instruction::Register srcReg = std::get<Parser::Instruction::Register>(sourceOperand);
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                {
                    Parser::Instruction::Memory srcMem = std::get<Parser::Instruction::Memory>(sourceOperand);
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                {
                    Parser::Immediate srcImm = std::get<Parser::Immediate>(sourceOperand);

                    bool rexW = false;
                    bool rexR = false;
                    bool rexX = false;
                    bool rexB = false;
                    bool useREX = false;

                    bool use16Bit = false;

                    uint8_t opcode;
                    uint64_t max = std::numeric_limits<uint64_t>::max();
                    uint16_t sizeInBits = 64;

                    switch (destReg.reg)
                    {
                        case ::x86::AL: case ::x86::CL:
                        case ::x86::DL: case ::x86::BL:
                        case ::x86::AH: case ::x86::CH:
                        case ::x86::DH: case ::x86::BH:
                            max = std::numeric_limits<uint8_t>::max();
                            sizeInBits = 8;
                            break;

                        case ::x86::SPL: case ::x86::BPL:
                        case ::x86::SIL: case ::x86::DIL:
                        case ::x86::R8B: case ::x86::R9B:
                        case ::x86::R10B: case ::x86::R11B:
                        case ::x86::R12B: case ::x86::R13B:
                        case ::x86::R14B: case ::x86::R15B:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint8_t>::max();
                            sizeInBits = 8;
                            break;

                        case ::x86::AX: case ::x86::CX:
                        case ::x86::DX: case ::x86::BX:
                        case ::x86::SP: case ::x86::BP:
                        case ::x86::SI: case ::x86::DI:
                            max = std::numeric_limits<uint16_t>::max();
                            sizeInBits = 16;
                            break;

                        case ::x86::R8W: case ::x86::R9W:
                        case ::x86::R10W: case ::x86::R11W:
                        case ::x86::R12W: case ::x86::R13W:
                        case ::x86::R14W: case ::x86::R15W:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint16_t>::max();
                            sizeInBits = 16;
                            break;

                        case ::x86::EAX: case ::x86::ECX:
                        case ::x86::EDX: case ::x86::EBX:
                        case ::x86::ESP: case ::x86::EBP:
                        case ::x86::ESI: case ::x86::EDI:
                            max = std::numeric_limits<uint32_t>::max();
                            sizeInBits = 32;
                            break;

                        case ::x86::R8D: case ::x86::R9D:
                        case ::x86::R10D: case ::x86::R11D:
                        case ::x86::R12D: case ::x86::R13D:
                        case ::x86::R14D: case ::x86::R15D:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint32_t>::max();
                            sizeInBits = 32;
                            break;

                        case ::x86::RAX: case ::x86::RCX:
                        case ::x86::RDX: case ::x86::RBX:
                        case ::x86::RSP: case ::x86::RBP:
                        case ::x86::RSI: case ::x86::RDI:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint64_t>::max();
                            sizeInBits = 64;
                            break;

                        case ::x86::R8: case ::x86::R9:
                        case ::x86::R10: case ::x86::R11:
                        case ::x86::R12: case ::x86::R13:
                        case ::x86::R14: case ::x86::R15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint64_t>::max();
                            sizeInBits = 64;
                            break;

                        case ::x86::IP: case ::x86::EIP:
                        case ::x86::RIP:
                        case ::x86::ES: case ::x86::CS:
                        case ::x86::SS: case ::x86::DS:
                        case ::x86::FS: case ::x86::GS:
                        case ::x86::FLAGS:
                        case ::x86::CR0: case ::x86::CR2:
                        case ::x86::CR3: case ::x86::CR4:
                        case ::x86::DR0: case ::x86::DR1:
                        case ::x86::DR2: case ::x86::DR3:
                        case ::x86::DR6: case ::x86::DR7:
                        case ::x86::DR8: case ::x86::DR9:
                        case ::x86::DR10: case ::x86::DR11:
                        case ::x86::DR12: case ::x86::DR13:
                        case ::x86::DR14: case ::x86::DR15:
                        case ::x86::EFLAGS:
                        case ::x86::CR8:
                        case ::x86::RFLAGS:
                        case ::x86::TR0: case ::x86::TR1:
                        case ::x86::TR2: case ::x86::TR3:
                        case ::x86::TR4: case ::x86::TR5:
                        case ::x86::TR6: case ::x86::TR7:
                        case ::x86::ST0: case ::x86::ST1:
                        case ::x86::ST2: case ::x86::ST3:
                        case ::x86::ST4: case ::x86::ST5:
                        case ::x86::ST6: case ::x86::ST7:
                        case ::x86::MM0: case ::x86::MM1:
                        case ::x86::MM2: case ::x86::MM3:
                        case ::x86::MM4: case ::x86::MM5:
                        case ::x86::MM6: case ::x86::MM7:
                        case ::x86::XMM0: case ::x86::XMM1:
                        case ::x86::XMM2: case ::x86::XMM3:
                        case ::x86::XMM4: case ::x86::XMM5:
                        case ::x86::XMM6: case ::x86::XMM7:
                        case ::x86::XMM8: case ::x86::XMM9:
                        case ::x86::XMM10: case ::x86::XMM11:
                        case ::x86::XMM12: case ::x86::XMM13:
                        case ::x86::XMM14: case ::x86::XMM15:
                        case ::x86::YMM0: case ::x86::YMM1:
                        case ::x86::YMM2: case ::x86::YMM3:
                        case ::x86::YMM4: case ::x86::YMM5:
                        case ::x86::YMM6: case ::x86::YMM7:
                        case ::x86::YMM8: case ::x86::YMM9:
                        case ::x86::YMM10: case ::x86::YMM11:
                        case ::x86::YMM12: case ::x86::YMM13:
                        case ::x86::YMM14: case ::x86::YMM15:
                        case ::x86::MXCSR: case ::x86::XCR0: // TODO: check if existing
                            throw Exception::SemanticError("instruction doesn't support this register");

                        default: throw Exception::InternalError("Unknown register");
                    }

                    // TODO: set rex dynamically
                    switch (destReg.reg)
                    {
                        case ::x86::AL: case ::x86::CL:
                        case ::x86::DL: case ::x86::BL:
                        case ::x86::AH: case ::x86::CH:
                        case ::x86::DH: case ::x86::BH:
                            break;

                        case ::x86::AX: case ::x86::CX:
                        case ::x86::DX: case ::x86::BX:
                        case ::x86::SP: case ::x86::BP:
                        case ::x86::SI: case ::x86::DI:
                            if (instruction.bits != BitMode::Bits16)
                                use16Bit = true;
                            break;

                        case ::x86::EAX: case ::x86::ECX:
                        case ::x86::EDX: case ::x86::EBX:
                        case ::x86::ESP: case ::x86::EBP:
                        case ::x86::ESI: case ::x86::EDI:
                            if (instruction.bits == BitMode::Bits16)
                                use16Bit = true;
                            break;

                        case ::x86::SPL: case ::x86::BPL:
                        case ::x86::SIL: case ::x86::DIL:
                            useREX = true;
                            break;

                        case ::x86::R8B: case ::x86::R9B:
                        case ::x86::R10B: case ::x86::R11B:
                        case ::x86::R12B: case ::x86::R13B:
                        case ::x86::R14B: case ::x86::R15B:
                        case ::x86::R8D: case ::x86::R9D:
                        case ::x86::R10D: case ::x86::R11D:
                        case ::x86::R12D: case ::x86::R13D:
                        case ::x86::R14D: case ::x86::R15D:
                            useREX = true;
                            rexB = true;
                            break;

                        case ::x86::R8W: case ::x86::R9W:
                        case ::x86::R10W: case ::x86::R11W:
                        case ::x86::R12W: case ::x86::R13W:
                        case ::x86::R14W: case ::x86::R15W:
                            use16Bit = true;
                            useREX = true;
                            rexB = true;
                            break;

                        case ::x86::R8: case ::x86::R9:
                        case ::x86::R10: case ::x86::R11:
                        case ::x86::R12: case ::x86::R13:
                        case ::x86::R14: case ::x86::R15:
                            useREX = true;
                            rexB = true;
                            rexW = true;
                            break;

                        case ::x86::RAX: case ::x86::RCX:
                        case ::x86::RDX: case ::x86::RBX:
                        case ::x86::RSP: case ::x86::RBP:
                        case ::x86::RSI: case ::x86::RDI:
                            useREX = true;
                            rexW = true;
                            break;

                        default: throw Exception::InternalError("Unknown register");
                    }

                    switch (destReg.reg)
                    {
                        case ::x86::AL: opcode = 0xB0; break;
                        case ::x86::CL: opcode = 0xB1; break;
                        case ::x86::DL: opcode = 0xB2; break;
                        case ::x86::BL: opcode = 0xB3; break;
                        case ::x86::AH: opcode = 0xB4; break;
                        case ::x86::CH: opcode = 0xB5; break;
                        case ::x86::DH: opcode = 0xB6; break;
                        case ::x86::BH: opcode = 0xB7; break;
                        case ::x86::SPL: opcode = 0xB4; break;
                        case ::x86::BPL: opcode = 0xB5; break;
                        case ::x86::SIL: opcode = 0xB6; break;
                        case ::x86::DIL: opcode = 0xB7; break;
                        case ::x86::R8B: opcode = 0xB0; break;
                        case ::x86::R9B: opcode = 0xB1; break;
                        case ::x86::R10B: opcode = 0xB2; break;
                        case ::x86::R11B: opcode = 0xB3; break;
                        case ::x86::R12B: opcode = 0xB4; break;
                        case ::x86::R13B: opcode = 0xB5; break;
                        case ::x86::R14B: opcode = 0xB6; break;
                        case ::x86::R15B: opcode = 0xB7; break;

                        case ::x86::AX: opcode = 0xB8; break;
                        case ::x86::CX: opcode = 0xB9; break;
                        case ::x86::DX: opcode = 0xBA; break;
                        case ::x86::BX: opcode = 0xBB; break;
                        case ::x86::SP: opcode = 0xBC; break;
                        case ::x86::BP: opcode = 0xBD; break;
                        case ::x86::SI: opcode = 0xBE; break;
                        case ::x86::DI: opcode = 0xBF; break;
                        case ::x86::R8W: opcode = 0xB8; break;
                        case ::x86::R9W: opcode = 0xB9; break;
                        case ::x86::R10W: opcode = 0xBA; break;
                        case ::x86::R11W: opcode = 0xBB; break;
                        case ::x86::R12W: opcode = 0xBC; break;
                        case ::x86::R13W: opcode = 0xBD; break;
                        case ::x86::R14W: opcode = 0xBE; break;
                        case ::x86::R15W: opcode = 0xBF; break;

                        case ::x86::EAX: opcode = 0xB8; break;
                        case ::x86::ECX: opcode = 0xB9; break;
                        case ::x86::EDX: opcode = 0xBA; break;
                        case ::x86::EBX: opcode = 0xBB; break;
                        case ::x86::ESP: opcode = 0xBC; break;
                        case ::x86::EBP: opcode = 0xBD; break;
                        case ::x86::ESI: opcode = 0xBE; break;
                        case ::x86::EDI: opcode = 0xBF; break;
                        case ::x86::R8D: opcode = 0xB8; break;
                        case ::x86::R9D: opcode = 0xB9; break;
                        case ::x86::R10D: opcode = 0xBA; break;
                        case ::x86::R11D: opcode = 0xBB; break;
                        case ::x86::R12D: opcode = 0xBC; break;
                        case ::x86::R13D: opcode = 0xBD; break;
                        case ::x86::R14D: opcode = 0xBE; break;
                        case ::x86::R15D: opcode = 0xBF; break;

                        case ::x86::RAX: opcode = 0xB8; break;
                        case ::x86::RCX: opcode = 0xB9; break;
                        case ::x86::RDX: opcode = 0xBA; break;
                        case ::x86::RBX: opcode = 0xBB; break;
                        case ::x86::RSP: opcode = 0xBC; break;
                        case ::x86::RBP: opcode = 0xBD; break;
                        case ::x86::RSI: opcode = 0xBE; break;
                        case ::x86::RDI: opcode = 0xBF; break;
                        case ::x86::R8: opcode = 0xB8; break;
                        case ::x86::R9: opcode = 0xB9; break;
                        case ::x86::R10: opcode = 0xBA; break;
                        case ::x86::R11: opcode = 0xBB; break;
                        case ::x86::R12: opcode = 0xBC; break;
                        case ::x86::R13: opcode = 0xBD; break;
                        case ::x86::R14: opcode = 0xBE; break;
                        case ::x86::R15: opcode = 0xBF; break;

                        // TODO

                        default: throw Exception::InternalError("Unknown register");
                    }

                    // TODO: optimize (e.g. rax -> eax on 64 bit mode when imm is smaller than 2^32)
                    if (use16Bit) instr.push_back(0x66);
                    if (useREX) instr.push_back(getRex(rexW, rexR, rexX, rexB));
                    instr.push_back(opcode);
                    
                    uint64_t value = 0;
                    
                    if (!ignoreUnresolved)
                    {
                        Evaluation eval = Evaluate(srcImm, bytesWritten, sectionOffset, currentSection);
                        if (eval.useOffset)
                        {
                            value = eval.offset; // TODO overflow
                            Relocation reloc;
                            reloc.offsetInSection = sectionOffset + instr.size();
                            reloc.addend = eval.offset;
                            reloc.addendInCode = true;
                            reloc.section = *currentSection;
                            reloc.usedSection = eval.usedSection;
                            reloc.type = RelocationType::Absolute;
                            switch (sizeInBits)
                            {
                                case 8: reloc.size = RelocationSize::Bit8; break;
                                case 16: reloc.size = RelocationSize::Bit16; break;
                                case 32: reloc.size = RelocationSize::Bit32; break;
                                case 64: reloc.size = RelocationSize::Bit64; break;
                                default: throw Exception::InternalError("Unknown size in bits " + std::to_string(sizeInBits));
                            }
                            relocations.push_back(std::move(reloc));
                        }
                        else
                        {
                            Int128& interrupt128 = eval.result;

                            // FIXME
                            if (interrupt128 > max) throw Exception::SemanticError("Operand too large for instruction", instruction.lineNumber, instruction.column);

                            value = static_cast<uint64_t>(interrupt128);
                        }
                    }

                    appendImmediate(instr, value, sizeInBits);
                }
            }
            else if (std::holds_alternative<Parser::Instruction::Memory>(destinationOperand))
            {
                Parser::Instruction::Memory destMem = std::get<Parser::Instruction::Memory>(destinationOperand);

                if (std::holds_alternative<Parser::Instruction::Register>(sourceOperand))
                {
                    Parser::Instruction::Register srcReg = std::get<Parser::Instruction::Register>(sourceOperand);
                    // TODO
                }
                else if (std::holds_alternative<Parser::Instruction::Memory>(sourceOperand))
                {
                    Parser::Instruction::Memory srcMem = std::get<Parser::Instruction::Memory>(sourceOperand);
                    // TODO
                }
                else if (std::holds_alternative<Parser::Immediate>(sourceOperand))
                {
                    Parser::Immediate srcImm = std::get<Parser::Immediate>(sourceOperand);
                    // TODO
                }
            }
            else
                throw Exception::InternalError("Wrong argument type for 'mov'");

            return instr;
        }

        default: throw Exception::InternalError("Unknown data instruction", instruction.lineNumber, instruction.column);
    }
}