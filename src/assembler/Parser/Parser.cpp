#include "Parser.hpp"

#include <util/string.hpp>
#include <algorithm>

Parser::Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : context(_context), arch(_arch), bits(_bits), endianness(_endianness)
{

}

void Parser::Print()
{
    for (const auto& section : sections)
    {
        std::cout << section.name << ": " << std::endl;
        for (const auto& entry : section.entries)
        {
            if (std::holds_alternative<Instruction::Instruction>(entry))
            {
                const Instruction::Instruction& instruction = std::get<Instruction::Instruction>(entry);
                // TODO
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
                    std::cout << "0x" << std::hex << value << std::dec << std::endl;
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

Parser* getParser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
{
    return new x86::Parser(_context, _arch, _bits, _endianness);
}