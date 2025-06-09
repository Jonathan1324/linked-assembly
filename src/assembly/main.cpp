#include <iostream>
#include <fstream>

#include "parser.hpp"

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: assembly <input.asm> [-o output.o]" << std::endl;
        return 1;
    }
    
    const char* input_path = argv[1];
    const char* output_path = "out.o";

    for (int i = 2; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            output_path = argv[++i];
        }
        // TODO: weitere Optionen: --dump, --verbose, ...
    }

    std::cout << "Input: " << input_path << std::endl;
    std::cout << "Output: " << output_path << std::endl;

    std::ifstream file(input_path);
    if (!file.is_open())
    {
        std::cerr << "Couldn't open file " << input_path << std::endl;
        return 1;
    }

    Parsed parsed = parseAssembly(file);

    std::cout << "Globals" << std::endl;
    for (size_t i = 0; i < parsed.globals.size(); i++)
    {
        std::cout << "\t" << parsed.globals.at(i) << std::endl;
    }

    std::cout << "Externs" << std::endl;
    for (size_t i = 0; i < parsed.externs.size(); i++)
    {
        std::cout << "\t" << parsed.externs.at(i) << std::endl;
    }

    for (size_t i = 0; i < parsed.sections.size(); i++)
    {
        Section* section = &parsed.sections.at(i);
        std::cout << "Section " << section->name << std::endl;

        for (size_t labelIndex = 0; labelIndex < section->labels.size(); labelIndex++)
        {
            std::cout << "\tLabel " << section->labels.at(labelIndex).name << " pointing to entry " << section->labels.at(labelIndex).instructionIndex << std::endl;
        }

        for (size_t entryIndex = 0; entryIndex < section->entries.size(); entryIndex++)
        {
            SectionEntry entry = section->entries.at(entryIndex);
            if (std::holds_alternative<Instruction>(entry))
            {
                Instruction instr = std::get<Instruction>(entry);
                std::cout << "\t";
                switch(instr.mode)
                {
                    case BitMode::Bits16:
                        std::cout << "16-bit ";
                        break;
                    case BitMode::Bits32:
                        std::cout << "32-bit ";
                        break;
                    case BitMode::Bits64:
                        std::cout << "64-bit ";
                        break;
                    default:
                        std::cout << "Unknown" ;
                        break;
                }
                std::cout << "instruction " << instr.mnemonic << " at line " << instr.lineNumber << std::endl;
                for (size_t a = 0; a < instr.operands.size(); a++)
                {
                    std::cout << "\t\t" << instr.operands.at(a) << std::endl;
                }
            }
            else if (std::holds_alternative<DataDefinition>(entry))
            {
                DataDefinition data = std::get<DataDefinition>(entry);
                std::cout << (data.reserved ? "\tReserved" : "\tData") << data.name << " of type " << data.type << std::endl;
                for (size_t a = 0; a < data.values.size(); a++)
                {
                    std::cout << "\t\t" << data.values.at(a) << std::endl;
                }
            }
        }
    }

    return 0;
}