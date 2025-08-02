#include "Parser.hpp"

#include <util/string.hpp>
#include <algorithm>

using namespace Parser;

Parser::Parser::Parser(const Context& _context, Architecture _arch, BitMode _bits)
    : context(_context), arch(_arch), bits(_bits)
{

}

void Parser::Parser::Print() const
{
    if (!org.empty())
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
                std::cout << " on line " << instruction.lineNumber << " in column " << instruction.column;
                std::cout << ": " << std::endl;
                for (const auto& operand : instruction.operands)
                {
                    if (std::holds_alternative<Instruction::Register>(operand))
                    {
                        const Instruction::Register& reg = std::get<Instruction::Register>(operand);
                        std::cout << "    "; // 2x '  '
                        std::cout << "Register '" << reg.reg << "'" << std::endl;
                    }
                    else if (std::holds_alternative<Immediate>(operand))
                    {
                        const Immediate& imm = std::get<Immediate>(operand);
                        std::cout << "    "; // 2x '  '
                        std::cout << "Immediate: " << std::endl;
                        
                        for (const auto& op : imm.operands)
                        {
                            std::cout << "      "; // 3x '  '
                            if (std::holds_alternative<Integer>(op))
                            {
                                const Integer& integer = std::get<Integer>(op);
                                std::cout << "0x" << std::hex << integer.value << std::dec << std::endl;
                            }
                            else if (std::holds_alternative<Operator>(op))
                            {
                                const Operator& Op = std::get<Operator>(op);
                                std::cout << "'" << Op.op << "'" << std::endl;
                            }
                            else if (std::holds_alternative<String>(op))
                            {
                                const String& str = std::get<String>(op);
                                std::cout << "'" << str.value << "'" << std::endl;
                            }
                            else if (std::holds_alternative<CurrentPosition>(op))
                            {
                                const CurrentPosition& curPos = std::get<CurrentPosition>(op);
                                if (curPos.sectionPos)
                                    std::cout << "current position in section" << std::endl;
                                else
                                    std::cout << "current position" << std::endl;
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

                std::cout << " in line " << dataDefinition.lineNumber << " at column " << dataDefinition.column << std::endl;

                for (const auto& value : dataDefinition.values)
                {
                    std::cout << "    ";    // 2x '  '
                    std::cout << "Immediate: " << std::endl;
                        
                    for (const auto& op : value.operands)
                    {
                        std::cout << "      ";  // 3x '  '
                        if (std::holds_alternative<Integer>(op))
                        {
                            const Integer& integer = std::get<Integer>(op);
                            std::cout << "0x" << std::hex << integer.value << std::dec << std::endl;
                        }
                        else if (std::holds_alternative<Operator>(op))
                        {
                            const Operator& Op = std::get<Operator>(op);
                            std::cout << "'" << Op.op << "'" << std::endl;;
                        }
                        else if (std::holds_alternative<String>(op))
                        {
                            const String& str = std::get<String>(op);
                            std::cout << "'" << str.value << "'" << std::endl;;
                        }
                        else if (std::holds_alternative<CurrentPosition>(op))
                        {
                            const CurrentPosition& curPos = std::get<CurrentPosition>(op);
                            if (curPos.sectionPos)
                                std::cout << "current position in section" << std::endl;
                            else
                                std::cout << "current position" << std::endl;
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
                std::cout << label.name << "' on line " << label.lineNumber << " in column " << label.column << std::endl;
            }
            else if (std::holds_alternative<Constant>(entry))
            {
                const Constant& constant = std::get<Constant>(entry);
                std::cout << "  ";  // '  '
                std::cout << "Constant '" << constant.name << "' on line " << constant.lineNumber << " in column " << constant.column << std::endl;

                for (const auto& op : constant.value.operands)
                {
                    std::cout << "      ";  // 3x '  '
                    if (std::holds_alternative<Integer>(op))
                    {
                        const Integer& integer = std::get<Integer>(op);
                        std::cout << "0x" << std::hex << integer.value << std::dec << std::endl;
                    }
                    else if (std::holds_alternative<Operator>(op))
                    {
                        const Operator& Op = std::get<Operator>(op);
                        std::cout << "'" << Op.op << "'" << std::endl;;
                    }
                    else if (std::holds_alternative<String>(op))
                    {
                        const String& str = std::get<String>(op);
                        std::cout << "'" << str.value << "'" << std::endl;;
                    }
                    else if (std::holds_alternative<CurrentPosition>(op))
                    {
                        const CurrentPosition& curPos = std::get<CurrentPosition>(op);
                        if (curPos.sectionPos)
                            std::cout << "current position in section" << std::endl;
                        else
                            std::cout << "current position" << std::endl;
                    }
                }
            }
            else if (std::holds_alternative<Repetition>(entry))
            {
                const Repetition& repeat = std::get<Repetition>(entry);
                std::cout << "  ";  // '  '
                std::cout << "Repetition on line " << repeat.lineNumber << " in column " << repeat.column << std::endl;

                for (const auto& op : repeat.count.operands)
                {
                    std::cout << "      ";  // 3x '  '
                    if (std::holds_alternative<Integer>(op))
                    {
                        const Integer& integer = std::get<Integer>(op);
                        std::cout << "0x" << std::hex << integer.value << std::dec << std::endl;
                    }
                    else if (std::holds_alternative<Operator>(op))
                    {
                        const Operator& Op = std::get<Operator>(op);
                        std::cout << "'" << Op.op << "'" << std::endl;;
                    }
                    else if (std::holds_alternative<String>(op))
                    {
                        const String& str = std::get<String>(op);
                        std::cout << "'" << str.value << "'" << std::endl;;
                    }
                    else if (std::holds_alternative<CurrentPosition>(op))
                    {
                        const CurrentPosition& curPos = std::get<CurrentPosition>(op);
                        if (curPos.sectionPos)
                            std::cout << "current position in section" << std::endl;
                        else
                            std::cout << "current position" << std::endl;
                    }
                }
            }
            else if (std::holds_alternative<Alignment>(entry))
            {
                const Alignment& alignment = std::get<Alignment>(entry);
                std::cout << "  ";  // '  '
                std::cout << "Alignment on line " << alignment.lineNumber << " in column " << alignment.column << std::endl;

                for (const auto& op : alignment.align.operands)
                {
                    std::cout << "      ";  // 3x '  '
                    if (std::holds_alternative<Integer>(op))
                    {
                        const Integer& integer = std::get<Integer>(op);
                        std::cout << "0x" << std::hex << integer.value << std::dec << std::endl;
                    }
                    else if (std::holds_alternative<Operator>(op))
                    {
                        const Operator& Op = std::get<Operator>(op);
                        std::cout << "'" << Op.op << "'" << std::endl;;
                    }
                    else if (std::holds_alternative<String>(op))
                    {
                        const String& str = std::get<String>(op);
                        std::cout << "'" << str.value << "'" << std::endl;;
                    }
                    else if (std::holds_alternative<CurrentPosition>(op))
                    {
                        const CurrentPosition& curPos = std::get<CurrentPosition>(op);
                        if (curPos.sectionPos)
                            std::cout << "current position in section" << std::endl;
                        else
                            std::cout << "current position" << std::endl;
                    }
                }
            }
        }
    }
}

const std::string& Parser::Parser::getOrg() const
{
    return org;
}

const std::vector<Parser::Section>& Parser::Parser::getSections() const
{
    return sections;
}

const std::vector<std::string>& Parser::Parser::getExterns() const
{
    return externs;
}

#include "x86/Parser.hpp"

// FIXME: only temporary solution
Parser::Parser* Parser::getParser(const Context& context, Architecture arch, BitMode bits)
{
    return new x86::Parser(context, arch, bits);
}
