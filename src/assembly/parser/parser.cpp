#include "parser.hpp"
#include <util/string.hpp>

#include <algorithm>
#include <Exception.hpp>

Parsed parseAssembly(std::istream& input, BitMode bits, Context& context)
{
    //TODO
    (void)context;
    
    Parsed parsed;

    parsed.sections.push_back(Section{".text", {}, {}});
    Section* currentSection = &parsed.sections.back();

    std::string line;
    int lineNumber = 0;

    BitMode currentBitMode = bits;
    std::string currentLabel;

    while (std::getline(input, line))
    {
        lineNumber++;
        std::string trimmed = trim(line);

        // cut off comments
        bool inString = false;
        for (size_t i = 0; i < trimmed.size(); ++i)
        {
            if (trimmed[i] == '"')
            {
                inString = !inString;
            }
            else if (trimmed[i] == ';' && !inString)
            {
                trimmed = trimmed.substr(0, i);
                break;
            }
        }
        trimmed = trim(trimmed);
        if (trimmed.empty()) continue;

        // Macros and constants
        if (trimmed[0] == '%')
        {
            //TODO
            continue;
        }

        //TODO: equ & times

        // assembler directives
        if (toLower(trimmed).find("bits ") == 0)
        {
            std::string bits = trimmed.substr(5);
            if (bits.compare(0, 2, "16") == 0)
                currentBitMode = BitMode::Bits16;
            else if (bits.compare(0, 2, "32") == 0)
                currentBitMode = BitMode::Bits32;
            else if (bits.compare(0, 2, "64") == 0)
                currentBitMode = BitMode::Bits64;
            else
                throw Exception::SyntaxError("Undefined bits mode", lineNumber);

            trimmed = trim(trimmed.substr(5 + bits.size()));

            if (trimmed.empty()) continue;
        }
        else if (trimmed.find("[") == 0)
        {
            size_t bitPos = toLower(trimmed).find("bits ");
            if (bitPos == std::string::npos)
            {
                throw Exception::SyntaxError("Didn't find 'bits' after '['", lineNumber);
            }
            else
            {
                std::string bits = trim(trimmed.substr(bitPos + 5));
                if (bits.compare(0, 2, "16") == 0)
                    currentBitMode = BitMode::Bits16;
                else if (bits.compare(0, 2, "32") == 0)
                    currentBitMode = BitMode::Bits32;
                else if (bits.compare(0, 2, "64") == 0)
                    currentBitMode = BitMode::Bits64;
                else
                {
                    throw Exception::SyntaxError("Undefined bits mode", lineNumber);
                }
            }

            size_t closed = trimmed.find("]");
            if (closed == std::string::npos)
            {
                throw Exception::SyntaxError("Didn't find ']' after '['", lineNumber);
            }
            else
            {
                trimmed = trim(trimmed.substr(closed + 1));
                if (trimmed.empty()) continue;
            }
        }

        if (toLower(trimmed).find("org ") == 0)
        {
            try
            {
                parsed.org = std::stoull(trimmed.substr(4), nullptr, 0);
            }
            catch(const Exception& e) { throw e; }
            catch(const std::exception& e) { throw e; }
        }

        // Directives
        if (toLower(trimmed).find("section ") == 0)
        {
            //TODO: currently case insensitive
            std::string sectionName = toLower(trimmed).substr(8);
            sectionName = trim(sectionName);

            auto it = std::find_if(parsed.sections.begin(), parsed.sections.end(),
                [&](const Section& s) { return s.name == sectionName; });

            if (it == parsed.sections.end()) {
                // Create new section
                parsed.sections.push_back(Section{sectionName, {}, {}});
                currentSection = &parsed.sections.back();
            } else {
                currentSection = &(*it);
            }
            continue;
        }

        // Label
        size_t colonPos = trimmed.find(':');
        if (colonPos != std::string::npos)
        {
            std::string left = trim(trimmed.substr(0, colonPos));
            std::string right = trim(trimmed.substr(colonPos + 1));
    
            if (left.find(" ") != std::string::npos)
            {
                //segment offset
            }
            else
            {
                std::string labelName = left;

                if (labelName.find(".") == 0)
                {
                    if (currentLabel.empty())
                    {
                        throw Exception::SemanticError("Local label '" + labelName + "' doesn't have any parent label", lineNumber);
                    }
                    else
                    {
                        LocalLabel label{labelName, currentSection->entries.size()};
                        
                        auto it = currentSection->labels.find(currentLabel);
                        if (it != currentSection->labels.end())
                            it->second.localLabels.push_back(label);
                        else
                            throw Exception::SemanticError("Local label '" + labelName + "' doesn't have any parent label", lineNumber);
                    }
                }
                else
                {
                    auto result = currentSection->labels.emplace(labelName, Label{labelName, currentSection->entries.size(), {}});
                    if (result.second)
                        currentLabel = labelName;
                    else
                        throw Exception::SemanticError("Label '" + labelName + "' already exists", lineNumber);
                }
            
                trimmed = trim(trimmed.substr(colonPos + 1));
            }
        
            if (trimmed.empty()) continue;
        }
        else
        {
            static const std::vector<std::string> dataDirectives = {
                "db", "dw", "dd", "dq", "dt", "do", "dy", "dz", "resb", "resw", "resd", "resq", "rest", "reso", "resy", "resz"
            };

            for (const std::string& directive : dataDirectives) {
                size_t dirPos = trimmed.find(" " + directive + " ");
                if (dirPos != std::string::npos && dirPos > 0) {
                    std::string before = trim(trimmed.substr(0, dirPos));
                    if (!before.empty()) {
                        std::string labelName = before;
                        if (labelName.find(".") == 0)
                        {
                            if (currentLabel.empty())
                            {
                                throw Exception::SemanticError("Local label '" + labelName + "' doesn't have any parent label", lineNumber);
                            }
                            else
                            {
                                LocalLabel label{labelName, currentSection->entries.size()};
                        
                                auto it = currentSection->labels.find(currentLabel);
                                if (it != currentSection->labels.end()) {
                                    it->second.localLabels.push_back(label);
                                } else {
                                    throw Exception::SemanticError("Local label '" + labelName + "' doesn't have any parent label", lineNumber);
                                }
                            }
                        }
                        else
                        {
                            auto result = currentSection->labels.emplace(labelName, Label{labelName, currentSection->entries.size(), {}});
                            if (result.second) {
                                currentLabel = labelName;
                            } else {
                                throw Exception::SemanticError("Label '" + labelName + "' already exists", lineNumber);
                            }
                        }
            
                        trimmed = trim(trimmed.substr(dirPos));
                    }
                    break;
                }
            }
        }

        // global or extern
        if (toLower(trimmed).find("global ") == 0) {
            parsed.globals.push_back(trimmed.substr(7));
            continue;
        }
        if (toLower(trimmed).find("extern ") == 0) {
            parsed.externs.push_back(trimmed.substr(7));
            continue;
        }

        // Data
        if (toLower(trimmed).find("db ") == 0 || toLower(trimmed).find("dw ") == 0
         || toLower(trimmed).find("dd ") == 0 || toLower(trimmed).find("dq ") == 0
         || toLower(trimmed).find("dt ") == 0 || toLower(trimmed).find("do ") == 0
         || toLower(trimmed).find("dy ") == 0 || toLower(trimmed).find("dz ") == 0
         || toLower(trimmed).find("resb ") == 0 || toLower(trimmed).find("resw ") == 0
         || toLower(trimmed).find("resd ") == 0 || toLower(trimmed).find("resq ") == 0
         || toLower(trimmed).find("rest ") == 0 || toLower(trimmed).find("reso ") == 0
         || toLower(trimmed).find("resy ") == 0 || toLower(trimmed).find("resz ") == 0)
        {
            // get type (db, dw, dd)
            size_t spacePos = trimmed.find(' ');
            std::string type = toLower(trimmed).substr(0, spacePos);
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

            std::string labelName = ""; // TODO: if label was defined before, set it here

            bool isReserved = type.rfind("res", 0) == 0;

            //TODO: alignment
            currentSection->entries.push_back(DataDefinition{labelName, type, values, 1, isReserved, lineNumber});
            continue;
        }

        // Instructions
        size_t spacePos = trimmed.find(' ');
        std::string mnemonic;
        std::vector<std::string> operands;

        if (spacePos == std::string::npos)
            mnemonic = toLower(trimmed);
        else
        {
            mnemonic = toLower(trimmed).substr(0, spacePos);
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

        //TODO: alignment
        currentSection->entries.push_back(Instruction{
            mnemonic,
            operands,
            mode,
            1,
            lineNumber
        });
    }

    return parsed;
}