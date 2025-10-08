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

std::vector<uint8_t> x86::Encoder::EncodeDataInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved, bool optimize)
{
    switch (instruction.mnemonic)
    {
        case Instructions::MOV:
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

                    switch (destReg.reg)
                    {
                        case SPL: case BPL:
                        case SIL: case DIL:
                        case R8B: case R9B:
                        case R10B: case R11B:
                        case R12B: case R13B:
                        case R14B: case R15B:
                        case R8W: case R9W:
                        case R10W: case R11W:
                        case R12W: case R13W:
                        case R14W: case R15W:
                        case R8D: case R9D:
                        case R10D: case R11D:
                        case R12D: case R13D:
                        case R14D: case R15D:
                        case RAX: case RCX:
                        case RDX: case RBX:
                        case RSP: case RBP:
                        case RSI: case RDI:
                        case R8: case R9:
                        case R10: case R11:
                        case R12: case R13:
                        case R14: case R15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                    }

                    switch (srcReg.reg)
                    {
                        case SPL: case BPL:
                        case SIL: case DIL:
                        case R8B: case R9B:
                        case R10B: case R11B:
                        case R12B: case R13B:
                        case R14B: case R15B:
                        case R8W: case R9W:
                        case R10W: case R11W:
                        case R12W: case R13W:
                        case R14W: case R15W:
                        case R8D: case R9D:
                        case R10D: case R11D:
                        case R12D: case R13D:
                        case R14D: case R15D:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                    }

                    bool useOpcodeEscape = false;
                    uint8_t opcode;
                    Mod mod = Mod::REGISTER;

                    bool rexW = false;
                    bool rexR = false;
                    bool rexX = false;
                    bool rexB = false;
                    bool useREX = false;

                    bool use16Bit = false;

                    uint8_t modrm;

                    auto [dest, dstRex, dstSetRex] = getReg(destReg.reg);
                    auto [src, srcRex, srcSetRex] = getReg(srcReg.reg);

                    if (dstRex || srcRex) useREX = true;
                    if (dstSetRex) rexB = true;
                    if (srcSetRex) rexR = true;

                    uint8_t destRegSize = getRegSize(destReg.reg, instruction.bits);
                    uint8_t srcRegSize = getRegSize(srcReg.reg, instruction.bits);

                    if (destRegSize != srcRegSize) throw Exception::SemanticError("Can't use 'mov' with registers of different size");

                    bool usingSpecialReg = false;

                    switch (destReg.reg)
                    {
                        case ES: case CS:
                        case SS: case DS:
                        case FS: case GS:
                            opcode = 0x8E;
                            modrm = getModRM(mod, dest, src);
                            usingSpecialReg = true;
                            break;

                        case CR0: case CR2:
                        case CR3: case CR4:
                        case CR5: case CR6:
                        case CR7:
                            useOpcodeEscape = true;
                            opcode = 0x22;
                            modrm = getModRM(mod, dest, src);
                            usingSpecialReg = true;
                            break;
                        case CR8: case CR9:
                        case CR10: case CR11:
                        case CR12: case CR13:
                        case CR14: case CR15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            useREX = true;
                            rexR = true;
                            useOpcodeEscape = true;
                            opcode = 0x22;
                            modrm = getModRM(mod, dest, src);
                            usingSpecialReg = true;
                            break;

                        case DR0: case DR1:
                        case DR2: case DR3:
                        case DR6: case DR7:
                            useOpcodeEscape = true;
                            opcode = 0x23;
                            modrm = getModRM(mod, dest, src);
                            usingSpecialReg = true;
                            break;
                        case DR8: case DR9:
                        case DR10: case DR11:
                        case DR12: case DR13:
                        case DR14: case DR15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            useREX = true;
                            rexR = true;
                            useOpcodeEscape = true;
                            opcode = 0x23;
                            modrm = getModRM(mod, dest, src);
                            usingSpecialReg = true;
                            break;

                        case TR0: case TR1:
                        case TR2: case TR3:
                        case TR4: case TR5:
                        case TR6: case TR7:
                            useOpcodeEscape = true;
                            opcode = 0x26;
                            modrm = getModRM(mod, dest, src);
                            usingSpecialReg = true;
                            break;
                    }

                    switch (srcReg.reg)
                    {
                        case ES: case CS:
                        case SS: case DS:
                        case FS: case GS:
                            if (usingSpecialReg) throw Exception::SemanticError("Can't set special register using special register");
                            if (instruction.bits != BitMode::Bits16)
                                use16Bit = true;
                            opcode = 0x8C;
                            modrm = getModRM(mod, src, dest);
                            usingSpecialReg = true;
                            break;

                        case CR0: case CR2:
                        case CR3: case CR4:
                        case CR5: case CR6:
                        case CR7:
                            if (usingSpecialReg) throw Exception::SemanticError("Can't set special register using special register");
                            useOpcodeEscape = true;
                            opcode = 0x20;
                            modrm = getModRM(mod, src, dest);
                            usingSpecialReg = true;
                            break;
                        case CR8: case CR9:
                        case CR10: case CR11:
                        case CR12: case CR13:
                        case CR14: case CR15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            if (usingSpecialReg) throw Exception::SemanticError("Can't set special register using special register");
                            useREX = true;
                            rexR = true;
                            useOpcodeEscape = true;
                            opcode = 0x20;
                            modrm = getModRM(mod, src, dest);
                            usingSpecialReg = true;
                            break;

                        case DR0: case DR1:
                        case DR2: case DR3:
                        case DR6: case DR7:
                            if (usingSpecialReg) throw Exception::SemanticError("Can't set special register using special register");
                            useOpcodeEscape = true;
                            opcode = 0x21;
                            modrm = getModRM(mod, src, dest);
                            usingSpecialReg = true;
                            break;
                        case DR8: case DR9:
                        case DR10: case DR11:
                        case DR12: case DR13:
                        case DR14: case DR15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("register only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            if (usingSpecialReg) throw Exception::SemanticError("Can't set special register using special register");
                            useREX = true;
                            rexR = true;
                            useOpcodeEscape = true;
                            opcode = 0x21;
                            modrm = getModRM(mod, src, dest);
                            usingSpecialReg = true;
                            break;

                        case TR0: case TR1:
                        case TR2: case TR3:
                        case TR4: case TR5:
                        case TR6: case TR7:
                            if (usingSpecialReg) throw Exception::SemanticError("Can't set special register using special register");
                            useOpcodeEscape = true;
                            opcode = 0x24;
                            modrm = getModRM(mod, src, dest);
                            usingSpecialReg = true;
                            break;
                    }
                    
                    if (!usingSpecialReg)
                    {
                        switch (destReg.reg)
                        {
                            case AL: case CL:
                            case DL: case BL:
                            case AH: case CH:
                            case DH: case BH:
                            case SPL: case BPL:
                            case SIL: case DIL:
                            case R8B: case R9B:
                            case R10B: case R11B:
                            case R12B: case R13B:
                            case R14B: case R15B:
                                opcode = 0x88; // mov r/m8, r8
                                modrm = getModRM(mod, src, dest);
                                break;

                            case AX: case CX:
                            case DX: case BX:
                            case SP: case BP:
                            case SI: case DI:
                            case R8W: case R9W:
                            case R10W: case R11W:
                            case R12W: case R13W:
                            case R14W: case R15W:
                                if (instruction.bits != BitMode::Bits16)
                                    use16Bit = true;
                                opcode = 0x89; // mov r/m16, r16
                                modrm = getModRM(mod, src, dest);
                                break;

                            case EAX: case ECX:
                            case EDX: case EBX:
                            case ESP: case EBP:
                            case ESI: case EDI:
                            case R8D: case R9D:
                            case R10D: case R11D:
                            case R12D: case R13D:
                            case R14D: case R15D:
                                if (instruction.bits == BitMode::Bits16)
                                    use16Bit = true;
                                opcode = 0x89; // mov r/m32, r32
                                modrm = getModRM(mod, src, dest);
                                break;

                            case RAX: case RCX:
                            case RDX: case RBX:
                            case RSP: case RBP:
                            case RSI: case RDI:
                            case R8: case R9:
                            case R10: case R11:
                            case R12: case R13:
                            case R14: case R15:
                                opcode = 0x89; // mov r/m64, r64
                                modrm = getModRM(mod, src, dest);
                                useREX = true;
                                rexW = true;
                                break;
                            
                            default:
                                throw Exception::SemanticError("instruction doesn't support this register");
                        }
                    }

                    if (useREX && (
                        destReg.reg == AH || destReg.reg == CH ||
                        destReg.reg == DH || destReg.reg == BH ||
                        srcReg.reg == AH || srcReg.reg == CH ||
                        srcReg.reg == DH || srcReg.reg == BH
                    )) throw Exception::SemanticError("Can't use high 8-bit regs using new registers");

                    if (use16Bit) instr.push_back(0x66);
                    if (useREX) instr.push_back(getRex(rexW, rexR, rexX, rexB));
                    if (useOpcodeEscape) instr.push_back(opcodeEscape);
                    instr.push_back(opcode);
                    instr.push_back(modrm);
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

                    bool useOpcodeEscape = false;
                    uint8_t opcode;
                    uint64_t max = std::numeric_limits<uint64_t>::max();
                    uint16_t sizeInBits = 64;

                    switch (destReg.reg)
                    {
                        case AL: case CL:
                        case DL: case BL:
                        case AH: case CH:
                        case DH: case BH:
                            max = std::numeric_limits<uint8_t>::max();
                            sizeInBits = 8;
                            break;

                        case SPL: case BPL:
                        case SIL: case DIL:
                        case R8B: case R9B:
                        case R10B: case R11B:
                        case R12B: case R13B:
                        case R14B: case R15B:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint8_t>::max();
                            sizeInBits = 8;
                            break;

                        case AX: case CX:
                        case DX: case BX:
                        case SP: case BP:
                        case SI: case DI:
                            max = std::numeric_limits<uint16_t>::max();
                            sizeInBits = 16;
                            break;

                        case R8W: case R9W:
                        case R10W: case R11W:
                        case R12W: case R13W:
                        case R14W: case R15W:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint16_t>::max();
                            sizeInBits = 16;
                            break;

                        case EAX: case ECX:
                        case EDX: case EBX:
                        case ESP: case EBP:
                        case ESI: case EDI:
                            max = std::numeric_limits<uint32_t>::max();
                            sizeInBits = 32;
                            break;

                        case R8D: case R9D:
                        case R10D: case R11D:
                        case R12D: case R13D:
                        case R14D: case R15D:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint32_t>::max();
                            sizeInBits = 32;
                            break;

                        case RAX: case RCX:
                        case RDX: case RBX:
                        case RSP: case RBP:
                        case RSI: case RDI:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint64_t>::max();
                            sizeInBits = 64;
                            break;

                        case R8: case R9:
                        case R10: case R11:
                        case R12: case R13:
                        case R14: case R15:
                            if (instruction.bits != BitMode::Bits64)
                                throw Exception::SyntaxError("instruction only supported in 64-bit mode", instruction.lineNumber, instruction.column);
                            max = std::numeric_limits<uint64_t>::max();
                            sizeInBits = 64;
                            break;

                        default:
                            throw Exception::SemanticError("instruction doesn't support this register");
                    }

                    // TODO: set rex dynamically
                    switch (destReg.reg)
                    {
                        case AL: case CL:
                        case DL: case BL:
                        case AH: case CH:
                        case DH: case BH:
                            break;

                        case AX: case CX:
                        case DX: case BX:
                        case SP: case BP:
                        case SI: case DI:
                            if (instruction.bits != BitMode::Bits16)
                                use16Bit = true;
                            break;

                        case EAX: case ECX:
                        case EDX: case EBX:
                        case ESP: case EBP:
                        case ESI: case EDI:
                            if (instruction.bits == BitMode::Bits16)
                                use16Bit = true;
                            break;

                        case SPL: case BPL:
                        case SIL: case DIL:
                            useREX = true;
                            break;

                        case R8B: case R9B:
                        case R10B: case R11B:
                        case R12B: case R13B:
                        case R14B: case R15B:
                        case R8D: case R9D:
                        case R10D: case R11D:
                        case R12D: case R13D:
                        case R14D: case R15D:
                            useREX = true;
                            rexB = true;
                            break;

                        case R8W: case R9W:
                        case R10W: case R11W:
                        case R12W: case R13W:
                        case R14W: case R15W:
                            use16Bit = true;
                            useREX = true;
                            rexB = true;
                            break;

                        case R8: case R9:
                        case R10: case R11:
                        case R12: case R13:
                        case R14: case R15:
                            useREX = true;
                            rexB = true;
                            rexW = true;
                            break;

                        case RAX: case RCX:
                        case RDX: case RBX:
                        case RSP: case RBP:
                        case RSI: case RDI:
                            useREX = true;
                            rexW = true;
                            break;

                        default: throw Exception::InternalError("Unknown register");
                    }

                    switch (destReg.reg)
                    {
                        case AL: opcode = 0xB0; break;
                        case CL: opcode = 0xB1; break;
                        case DL: opcode = 0xB2; break;
                        case BL: opcode = 0xB3; break;
                        case AH: opcode = 0xB4; break;
                        case CH: opcode = 0xB5; break;
                        case DH: opcode = 0xB6; break;
                        case BH: opcode = 0xB7; break;
                        case SPL: opcode = 0xB4; break;
                        case BPL: opcode = 0xB5; break;
                        case SIL: opcode = 0xB6; break;
                        case DIL: opcode = 0xB7; break;
                        case R8B: opcode = 0xB0; break;
                        case R9B: opcode = 0xB1; break;
                        case R10B: opcode = 0xB2; break;
                        case R11B: opcode = 0xB3; break;
                        case R12B: opcode = 0xB4; break;
                        case R13B: opcode = 0xB5; break;
                        case R14B: opcode = 0xB6; break;
                        case R15B: opcode = 0xB7; break;

                        case AX: opcode = 0xB8; break;
                        case CX: opcode = 0xB9; break;
                        case DX: opcode = 0xBA; break;
                        case BX: opcode = 0xBB; break;
                        case SP: opcode = 0xBC; break;
                        case BP: opcode = 0xBD; break;
                        case SI: opcode = 0xBE; break;
                        case DI: opcode = 0xBF; break;
                        case R8W: opcode = 0xB8; break;
                        case R9W: opcode = 0xB9; break;
                        case R10W: opcode = 0xBA; break;
                        case R11W: opcode = 0xBB; break;
                        case R12W: opcode = 0xBC; break;
                        case R13W: opcode = 0xBD; break;
                        case R14W: opcode = 0xBE; break;
                        case R15W: opcode = 0xBF; break;

                        case EAX: opcode = 0xB8; break;
                        case ECX: opcode = 0xB9; break;
                        case EDX: opcode = 0xBA; break;
                        case EBX: opcode = 0xBB; break;
                        case ESP: opcode = 0xBC; break;
                        case EBP: opcode = 0xBD; break;
                        case ESI: opcode = 0xBE; break;
                        case EDI: opcode = 0xBF; break;
                        case R8D: opcode = 0xB8; break;
                        case R9D: opcode = 0xB9; break;
                        case R10D: opcode = 0xBA; break;
                        case R11D: opcode = 0xBB; break;
                        case R12D: opcode = 0xBC; break;
                        case R13D: opcode = 0xBD; break;
                        case R14D: opcode = 0xBE; break;
                        case R15D: opcode = 0xBF; break;

                        case RAX: opcode = 0xB8; break;
                        case RCX: opcode = 0xB9; break;
                        case RDX: opcode = 0xBA; break;
                        case RBX: opcode = 0xBB; break;
                        case RSP: opcode = 0xBC; break;
                        case RBP: opcode = 0xBD; break;
                        case RSI: opcode = 0xBE; break;
                        case RDI: opcode = 0xBF; break;
                        case R8: opcode = 0xB8; break;
                        case R9: opcode = 0xB9; break;
                        case R10: opcode = 0xBA; break;
                        case R11: opcode = 0xBB; break;
                        case R12: opcode = 0xBC; break;
                        case R13: opcode = 0xBD; break;
                        case R14: opcode = 0xBE; break;
                        case R15: opcode = 0xBF; break;

                        default: throw Exception::InternalError("Unknown register");
                    }

                    // TODO: optimize (e.g. rax -> eax on 64 bit mode when imm is smaller than 2^32)
                    if (use16Bit) instr.push_back(0x66);
                    if (useREX) instr.push_back(getRex(rexW, rexR, rexX, rexB));
                    if (useOpcodeEscape) instr.push_back(opcodeEscape);
                    instr.push_back(opcode);
                    
                    uint64_t value = 0;
                    
                    if (!ignoreUnresolved)
                    {
                        ::Encoder::Evaluation eval = Evaluate(srcImm, bytesWritten, sectionOffset, currentSection);
                        if (eval.useOffset)
                        {
                            value = eval.offset; // TODO overflow
                            ::Encoder::Relocation reloc;
                            reloc.offsetInSection = sectionOffset + instr.size();
                            reloc.addend = eval.offset;
                            reloc.addendInCode = true;
                            reloc.section = *currentSection;
                            reloc.usedSection = eval.usedSection;
                            reloc.type = ::Encoder::RelocationType::Absolute;
                            switch (sizeInBits)
                            {
                                case 8: reloc.size = ::Encoder::RelocationSize::Bit8; break;
                                case 16: reloc.size = ::Encoder::RelocationSize::Bit16; break;
                                case 32: reloc.size = ::Encoder::RelocationSize::Bit32; break;
                                case 64: reloc.size = ::Encoder::RelocationSize::Bit64; break;
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