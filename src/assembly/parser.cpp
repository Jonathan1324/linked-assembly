#include "parser.hpp"
#include "util/string.hpp"

Parsed parseAssembly(std::istream& input)
{
    Parsed parsed;

    parsed.sections.push_back(Section{".text", {}, {}});
    Section* currentSection = &parsed.sections.back();

    std::string line;
    int lineNumber = 0;

    BitMode currentBitMode = BitMode::Bits32; //TODO

    while (std::getline(input, line))
    {
        lineNumber++;
        std::string trimmed = trim(line);

        // cut off comments
        size_t commentPos = trimmed.find(';');
        if (commentPos != std::string::npos)
        {
            trimmed = trimmed.substr(0, commentPos);
            trimmed = trim(trimmed);
        }
        if (trimmed.empty()) continue;

        // Macros and constants
        if (trimmed[0] == '%')
        {
            //TODO
            continue;
        }

        //TODO: equ

        // Bit mode
        if (trimmed.find("bits ") == 0)
        {
            std::string bits = trimmed.substr(5);
            if (bits.compare("16") == 0)
                currentBitMode = BitMode::Bits16;
            else if (bits.compare("32") == 0)
                currentBitMode = BitMode::Bits32;
            else if (bits.compare("64") == 0)
                currentBitMode = BitMode::Bits64;
            else
            {
                //TODO: error
            }

            trimmed = trim(trimmed.substr(7));

            if (trimmed.empty()) continue;
        }

        // Directives
        if (trimmed.find("section ") == 0)
        {
            std::string sectionName = trimmed.substr(8);
            sectionName = trim(sectionName);

            auto it = std::find_if(parsed.sections.begin(), parsed.sections.end(),
                [&](const Section& s) { return s.name == sectionName; });

            if (it == parsed.sections.end()) {
                // Neue Section anlegen und Pointer setzen
                parsed.sections.push_back(Section{sectionName, {}, {}});
                currentSection = &parsed.sections.back();
            } else {
                currentSection = &(*it);
            }
            continue;
        }

        // Label
        size_t colonPos = trimmed.find(':');
        if (colonPos != std::string::npos) {
            std::string labelName = trim(trimmed.substr(0, colonPos));
            currentSection->labels.push_back(Label{labelName, currentSection->entries.size()});
        
            trimmed = trim(trimmed.substr(colonPos + 1)); // Rest nach dem Label
        
            if (trimmed.empty()) continue; // Nur Label auf der Zeile
            // Ansonsten ist nach dem Label noch eine Instruktion => weiter unten behandeln
        }

        // global or extern
        if (trimmed.find("global ") == 0) {
            parsed.globals.push_back(trimmed.substr(7));
            continue;
        }
        if (trimmed.find("extern ") == 0) {
            parsed.externs.push_back(trimmed.substr(7));
            continue;
        }

        // Data
        if (trimmed.find("db ") == 0 || trimmed.find("dw ") == 0
         || trimmed.find("dd ") == 0 || trimmed.find("dq ") == 0
         || trimmed.find("dt ") == 0
         || trimmed.find("resb ") == 0 || trimmed.find("resw ") == 0
         || trimmed.find("resd ") == 0 || trimmed.find("resq ") == 0
         || trimmed.find("rest ") == 0)
        {
            // get type (db, dw, dd)
            size_t spacePos = trimmed.find(' ');
            std::string type = trimmed.substr(0, spacePos);
            std::string valuesStr = trim(trimmed.substr(spacePos + 1));

            // Split values to vector
            std::vector<std::string> values;
            size_t start = 0;
            while (true) {
                size_t commaPos = valuesStr.find(',', start);
                if (commaPos == std::string::npos) {
                    std::string val = trim(valuesStr.substr(start));
                    if (!val.empty()) values.push_back(val);
                    break;
                }
                std::string val = trim(valuesStr.substr(start, commaPos - start));
                if (!val.empty()) values.push_back(val);
                start = commaPos + 1;
            }

            std::string labelName = ""; // TODO: falls vorher Label gesetzt wurde, übergib hier

            bool isReserved = type.rfind("res", 0) == 0;

            currentSection->entries.push_back(DataDefinition{labelName, type, values, isReserved, lineNumber});
            continue;
        }

        // Instructions
        size_t spacePos = trimmed.find(' ');
        std::string mnemonic;
        std::vector<std::string> operands;

        if (spacePos == std::string::npos)
            mnemonic = trimmed;
        else
        {
            mnemonic = trimmed.substr(0, spacePos);
            std::string operandsStr = trim(trimmed.substr(spacePos + 1));

            size_t start = 0;
            //TODO: add string support
            while (true) {
                size_t commaPos = operandsStr.find(',', start);
                std::string operand = trim(operandsStr.substr(start, commaPos - start));
                if (!operand.empty()) operands.push_back(operand);
                if (commaPos == std::string::npos) break;
                start = commaPos + 1;
            }
        }

        BitMode mode = currentBitMode;

        currentSection->entries.push_back(Instruction{
            mnemonic,
            operands,
            mode,
            lineNumber
        });
    }

    return parsed;
}