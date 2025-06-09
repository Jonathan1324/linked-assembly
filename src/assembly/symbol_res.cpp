#include "symbol_res.hpp"

#include <unordered_set>

int getInstructionSize(Instruction instr)
{
    return 1;
}

int getDataSize(DataDefinition data)
{
    int unitSize = 0;

    if (data.type == "db")      unitSize =      1;
    else if (data.type == "dw") unitSize =      2;
    else if (data.type == "dd") unitSize =      4;
    else if (data.type == "dq") unitSize =      8;
    else if (data.type == "dt") unitSize =      10;
    else if (data.type == "resb") unitSize =    1;
    else if (data.type == "resw") unitSize =    2;
    else if (data.type == "resd") unitSize =    4;
    else if (data.type == "resq") unitSize =    8;
    else if (data.type == "rest") unitSize =    10;
    else {
        std::cerr << "Unknown data type: " << data.type << std::endl;
        return 0;
    }

    return unitSize * data.values.size();
}

SymbolTable resolveSymbols(const Parsed& parsed)
{
    SymbolTable symbolTable;
    for (size_t sectionIndex = 0; sectionIndex < parsed.sections.size(); sectionIndex++)
    {
        Section section = parsed.sections.at(sectionIndex);

        size_t currentOffset = 0;
        size_t labelIndex = 0;

        for (size_t entryIndex = 0; entryIndex < section.entries.size(); entryIndex++)
        {
            // Check if there's a label pointing here
            while (labelIndex < section.labels.size() &&
                section.labels[labelIndex].instructionIndex == entryIndex)
            {
                Label label = section.labels[labelIndex];
                Symbol symbol{label.name, section.name, currentOffset, false, false};
                symbolTable[label.name] = symbol;
                std::cout << label.name << " at offset " << symbol.offset << std::endl;
                ++labelIndex;
            }

            SectionEntry entry = section.entries.at(entryIndex);

            if (std::holds_alternative<Instruction>(entry))
            {
                Instruction instr = std::get<Instruction>(entry);
                int size = getInstructionSize(instr);
                currentOffset += size;
            }
            else if (std::holds_alternative<DataDefinition>(entry))
            {
                DataDefinition data = std::get<DataDefinition>(entry);
                int size = getDataSize(data);
                currentOffset += size;
            }
        }

        // Labels without entry
        while (labelIndex < section.labels.size())
        {
            Label label = section.labels[labelIndex];
            Symbol symbol = Symbol{label.name, section.name, currentOffset, false, false};
            symbolTable[label.name] = symbol;
            ++labelIndex;
        }
    }

    for (size_t i = 0; i < parsed.globals.size(); i++)
    {
        const std::string& name = parsed.globals.at(i);
        if (symbolTable.find(name) == symbolTable.end()) {
            std::cerr << "Warning: global symbol '" << name << "' not defined in any section.\n";
        } else {
            symbolTable[name].isGlobal = true;
        }
    }

    for (size_t i = 0; i < parsed.externs.size(); i++)
    {
        const std::string& name = parsed.externs.at(i);
        if (symbolTable.find(name) == symbolTable.end()) {
            Symbol sym{name, "", 0, false, true};
            symbolTable[name] = sym;
        } else {
            symbolTable[name].isExtern = true;
        }
    }

    return symbolTable;
}