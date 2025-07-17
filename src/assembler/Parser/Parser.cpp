#include "Parser.hpp"

#include <util/string.hpp>
#include <algorithm>

Parser::Parser(const Context& _context, Architecture _arch, BitMode _bits)
    : context(_context), arch(_arch), bits(_bits)
{

}

void Parser::Print()
{
    std::cout << "org: " << org << std::endl;

    if (!externs.empty())
    {
        std::cout << "externs: " << std::endl;
        for (const auto& label : externs)
        {
            // '  '
            std::cout << "  " << label << std::endl;
        }
    }

    for (const auto& section : sections)
    {
        std::cout << section.name << ": " << std::endl;
        for (const auto& entry : section.entries)
        {
            if (std::holds_alternative<Instruction::Instruction>(entry))
            {
                const Instruction::Instruction& instruction = std::get<Instruction::Instruction>(entry);
                std::cout << "  "; // '  '
                switch (instruction.bits)
                {
                    case BitMode::Bits16: std::cout << "16-bit "; break;
                    case BitMode::Bits32: std::cout << "32-bit "; break;
                    case BitMode::Bits64: std::cout << "64-bit "; break;
                    default: std::cout << "Unknown bits "; break;
                }
                std::cout << "instruction 0x" << std::hex << instruction.mnemonic << std::dec;
                std::cout << " aligned to " << instruction.alignment << " on line " << instruction.lineNumber << " in column " << instruction.column;
                std::cout << ": " << std::endl;
                for (const auto& operand : instruction.operands)
                {
                    if (std::holds_alternative<Instruction::Register>(operand))
                    {
                        const Instruction::Register& reg = std::get<Instruction::Register>(operand);
                        std::cout << "    "; // 2x '  '
                        std::cout << "Register '" << reg.reg << "'" << std::endl;
                    }
                    else if (std::holds_alternative<Instruction::Immediate>(operand))
                    {
                        const Instruction::Immediate& imm = std::get<Instruction::Immediate>(operand);
                        std::cout << "    "; // 2x '  '
                        std::cout << "Immediate: " << std::endl;
                        
                        for (const auto& op : imm.operands)
                        {
                            std::cout << "      "; // 3x '  '
                            if (std::holds_alternative<Instruction::Integer>(op))
                            {
                                const Instruction::Integer& integer = std::get<Instruction::Integer>(op);
                                if (integer.isString)
                                    std::cout << "'" << integer.value << "'" << std::endl;
                                else
                                    std::cout << "0x" << std::hex << integer.val << std::dec << std::endl;
                            }
                            else if (std::holds_alternative<Instruction::Operator>(op))
                            {
                                const Instruction::Operator& Op = std::get<Instruction::Operator>(op);
                                std::cout << "'" << Op.op << "'" << std::endl;;
                            }
                            else if (std::holds_alternative<Instruction::String>(op))
                            {
                                const Instruction::String& str = std::get<Instruction::String>(op);
                                std::cout << "'" << str.value << "'" << std::endl;;
                            }
                        }
                    }
                }
            }
            else if (std::holds_alternative<DataDefinition>(entry))
            {
                const DataDefinition& dataDefinition = std::get<DataDefinition>(entry);
                std::cout << "  ";  // '  '
                if (dataDefinition.reserved)
                    std::cout << "Reserved";
                else
                    std::cout << "Data";

                std::cout << " of size " << dataDefinition.size;
                
                if (dataDefinition.alignment != 0)
                    std::cout << " aligned to " << dataDefinition.alignment;

                std::cout << " in line " << dataDefinition.lineNumber << " at column " << dataDefinition.column << std::endl;

                for (const auto& value : dataDefinition.values)
                {
                    std::cout << "    ";    // 2x '  '
                    std::cout << "Immediate: " << std::endl;
                        
                    for (const auto& op : value.operands)
                    {
                        std::cout << "      ";  // 3x '  '
                        if (std::holds_alternative<Instruction::Integer>(op))
                        {
                            const Instruction::Integer& integer = std::get<Instruction::Integer>(op);
                            if (integer.isString)
                                std::cout << "'" << integer.value << "'" << std::endl;
                            else
                                std::cout << "0x" << std::hex << integer.val << std::dec << std::endl;
                        }
                        else if (std::holds_alternative<Instruction::Operator>(op))
                        {
                            const Instruction::Operator& Op = std::get<Instruction::Operator>(op);
                            std::cout << "'" << Op.op << "'" << std::endl;;
                        }
                        else if (std::holds_alternative<Instruction::String>(op))
                        {
                            const Instruction::String& str = std::get<Instruction::String>(op);
                            std::cout << "'" << str.value << "'" << std::endl;;
                        }
                    }
                }
            }
            else if (std::holds_alternative<Label>(entry))
            {
                const Label& label = std::get<Label>(entry);
                std::cout << "  ";  // '  '
                if (label.isGlobal)
                    std::cout << "Global label '";
                else
                    std::cout << "Label '";
                std::cout << label.name << "' in line " << label.lineNumber << " at column " << label.column << std::endl;
            }
        }
    }
}

#include "x86/Parser.hpp"

// FIXME: only temporary solution
Parser* getParser(const Context& _context, Architecture _arch, BitMode _bits)
{
    return new x86::Parser(_context, _arch, _bits);
}
