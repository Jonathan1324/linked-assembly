#include "Parser.hpp"

#include <util/string.hpp>
#include <unordered_set>
#include <array>
#include <algorithm>
#include "registers.hpp"
#include "Instructions.hpp"

x86::Parser::Parser(const Context& _context, Architecture _arch, BitMode _bits)
    : ::Parser(_context, _arch, _bits)
{

}

ImmediateOperand getOperand(const Token::Token& token)
{
    if (token.type == Token::Type::Operator || token.type == Token::Type::Bracket)
    {
        Operator op;
        op.op = token.value;
        return op;
    }
    else if (std::isdigit(static_cast<unsigned char>(token.value[0])) != 0)
    {
        Integer integer;
        integer.value = token.value;
        integer.isString = true;
        return integer;
    }
    else if (token.type == Token::Type::Character)
    {
        Integer integer;
        integer.val = static_cast<uint64_t>(static_cast<unsigned char>(token.value[0]));
        integer.isString = false;
        return integer;
    }
    else
    {
        String str;
        str.value = token.value;
        return str;
    }
}

void x86::Parser::Parse(const std::vector<Token::Token>& tokens)
{
    std::vector<Token::Token> filteredTokens;
    Token::Type before = Token::Type::_EOF;
    for (size_t i = 0; i < tokens.size(); i++)
    {
        const Token::Token& token = tokens[i];

        if (token.type == Token::Type::EOL && before == Token::Type::EOL)
            continue;
        
        if (token.type == Token::Type::Punctuation && token.value.at(0) == ';')
        {
            while (i < tokens.size() && tokens[i].type != Token::Type::EOL && tokens[i].type != Token::Type::_EOF)
                i++;
            if (i < tokens.size())
                filteredTokens.push_back(tokens[i]);
            continue;
        }

        before = token.type;
        filteredTokens.push_back(token);
    }

    std::vector<std::string> globals;

    for (auto it = filteredTokens.begin(); it != filteredTokens.end(); /* manual increment */)
    {
        bool hasOpeningBracket = false;
        bool hasClosingBracket = false;

        if (it->type == Token::Type::Bracket && it->value == "[")
        {
            auto next = std::next(it);
            if (next != filteredTokens.end())
            {
                const std::string val = toLower(next->value);
                if (val == "global" || val == "extern")
                {
                    hasOpeningBracket = true;
                    it = filteredTokens.erase(it);
                }
            }
        }

        const std::string lowerVal = toLower(it->value);

        if (lowerVal == "global" || lowerVal == "extern")
        {
            // Get the next token (the symbol name)
            auto next = std::next(it);
            if (next != filteredTokens.end())
            {
                if (lowerVal == "global")
                    globals.push_back(next->value);
                else
                    externs.push_back(next->value);
            }

            // remove "global"/"extern"
            it = filteredTokens.erase(it);

            // remove symbol
            if (it != filteredTokens.end())
                it = filteredTokens.erase(it);

            // remove ']' if it started with '['
            if (hasOpeningBracket)
            {
                if (it == filteredTokens.end() || it->type != Token::Type::Bracket || it->value != "]")
                    throw Exception::SyntaxError("Missing closing ']' after '[global ...' or '[extern ...'", it->line, it->column);
                it = filteredTokens.erase(it);
            }
            else
            {
                /* TODO: think about it
                if (it != filteredTokens.end() && it->type == Token::Type::Bracket && it->value == "]")
                    throw Exception::SyntaxError("Unexpected closing ']' after directive", it->line, it->column);
                */
            }

            // Erase tokens until end-of-line
            while (it != filteredTokens.end() && it->type != Token::Type::EOL)
            {
                it = filteredTokens.erase(it);  // erase returns the next valid iterator
            }

            // Erase the EOL token as well, if present
            if (it != filteredTokens.end())
                it = filteredTokens.erase(it);

            continue; // Don't increment, already done via erase
        }

        ++it; // only increment if nothing was erased
    }

    static constexpr std::array<std::string_view, 16> dataDefinitions = {
        "db", "dw", "dd", "dq", "dt", "do", "dy", "dz",
        "resb", "resw", "resd", "resq", "rest", "reso", "resy", "resz"
    };

    static constexpr std::array<std::string_view, 3> directives = {
        "section", "bits", "org"
    };

    Section text;
    text.name = ".text";
    sections.push_back(text);

    Section* currentSection = &sections.at(0);
    BitMode currentBitMode = bits;

    for (size_t i = 0; i < filteredTokens.size(); i++)
    {
        const Token::Token& token = filteredTokens[i];
        if (token.type == Token::Type::EOL || token.type == Token::Type::_EOF)
            continue;
        
        const std::string lowerVal = toLower(token.value);

        // Macros
        // ('%')
        if (token.type == Token::Type::Macro)
        {
            //TODO
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL && filteredTokens[i].type != Token::Type::_EOF)
                i++;
            continue;
        }

        // Constants
        if (filteredTokens[i + 1].type == Token::Type::Token && filteredTokens[i + 1].value == "equ")
        {
            Constant constant;
            constant.lineNumber = token.line;
            constant.column = token.column;
            // TODO: case sensitive
            constant.name = token.value;
            i += 2;

            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character)
                {
                    while (i < filteredTokens.size() &&
                           !(filteredTokens[i].type == Token::Type::Comma || filteredTokens[i].type == Token::Type::EOL))
                    {
                        ImmediateOperand op = getOperand(filteredTokens[i]);
                        constant.value.operands.push_back(op);
                        i++;
                    }
                    i--;
                }
                else if (filteredTokens[i].type == Token::Type::String)
                {
                    // TODO
                }
                else
                    throw Exception::SyntaxError("Expected definition after 'equ'", filteredTokens[i].line, filteredTokens[i].column);

                i++;
            }

            currentSection->entries.push_back(constant);

            continue;
        }

        // Directives
        if ((token.type == Token::Type::Bracket && token.value == "[" && std::find(directives.begin(), directives.end(), toLower(filteredTokens[i + 1].value)) != directives.end())
         || std::find(directives.begin(), directives.end(), lowerVal) != directives.end())
        {
            if (token.type == Token::Type::Bracket)
                i++;
            const Token::Token& directive = filteredTokens[i];
            const std::string& lowerDir = toLower(directive.value);
            
            if (lowerDir.compare("section") == 0)
            {
                // TODO: currently case insensitive
                std::string name = toLower(filteredTokens[++i].value);

                auto it = std::find_if(sections.begin(), sections.end(), [&](const Section& s) {return s.name == name; });

                if (it == sections.end())
                {
                    // Create new section
                    Section section;
                    section.name = name;
                    sections.emplace_back(section);
                    currentSection = &sections.back();
                }
                else
                {
                    currentSection = &(*it);
                }
            }
            else if (lowerDir.compare("bits") == 0)
            {
                std::string bits = filteredTokens[i + 1].value;

                if (bits.compare(0, 2, "16") == 0)
                    currentBitMode = BitMode::Bits16;
                else if (bits.compare(0, 2, "32") == 0)
                    currentBitMode = BitMode::Bits32;
                else if (bits.compare(0, 2, "64") == 0)
                    currentBitMode = BitMode::Bits64;
                else
                    throw Exception::SyntaxError("Undefined bits", token.line, token.column);
            }
            else if (lowerDir.compare("org") == 0)
            {
                org = filteredTokens[i + 1].value;
            }

            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                i++;
            continue;
        }

        // Labels
        if (token.type == Token::Type::Token &&
           ((filteredTokens[i + 1].type == Token::Type::Punctuation && filteredTokens[i + 1].value == ":" && /*TODO: not segment:offset*/ std::find(registers.begin(), registers.end(), token.value) == registers.end())
         || (filteredTokens[i + 1].type == Token::Type::Token && std::find(dataDefinitions.begin(), dataDefinitions.end(), toLower(filteredTokens[i + 1].value)) != dataDefinitions.end())))
        {
            Label label;
            label.name = token.value;
            label.lineNumber = token.line;
            label.column = token.column;

            if (std::find(globals.begin(), globals.end(), token.value) != globals.end())
                label.isGlobal = true;
            else
                label.isGlobal = false;

            currentSection->entries.push_back(label);

            if (filteredTokens[i + 1].type == Token::Type::Punctuation && filteredTokens[i + 1].value == ":")
                i++;
            continue;
        }

        // Data
        if (token.type == Token::Type::Token && std::find(dataDefinitions.begin(), dataDefinitions.end(), lowerVal) != dataDefinitions.end())
        {
            DataDefinition data;
            char cSize = '\0';
            if (lowerVal.compare(0, 3, "res") == 0)
            {
                data.reserved = true;
                cSize = lowerVal[3];
            }
            else if (lowerVal[0] == 'd')
            {
                data.reserved = false;
                cSize = lowerVal[1];
            }

            data.lineNumber = token.line;
            data.column = token.column;

            switch (cSize)
            {
                case 'b': data.size = 1; break;
                case 'w': data.size = 2; break;
                case 'd': data.size = 4; break;
                case 'q': data.size = 8; break;
                case 't': data.size = 10; break;
                case 'o': data.size = 16; break;
                case 'y': data.size = 32; break;
                case 'z': data.size = 64; break;
                default: throw Exception::InternalError("Unknown size suffix", token.line, token.column);
            }

            i++;
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character)
                {
                    Immediate val;
                    
                    while (i < filteredTokens.size() &&
                           !(filteredTokens[i].type == Token::Type::Comma || filteredTokens[i].type == Token::Type::EOL))
                    {
                        ImmediateOperand op = getOperand(filteredTokens[i]);
                        val.operands.push_back(op);
                        i++;
                    }
                    i--;

                    data.values.push_back(val);
                }
                else if (filteredTokens[i].type == Token::Type::String)
                {
                    const std::string& val = filteredTokens[i].value;
                    size_t len = val.size();

                    for (size_t pos = 0; pos < len; pos += data.size)
                    {
                        uint64_t combined = 0;

                        for (size_t offset = 0; offset < data.size; ++offset)
                        {
                            if (pos + offset < len)
                            {
                                combined |= static_cast<uint64_t>(static_cast<unsigned char>(val[pos + offset])) << (8 * offset);
                            }
                            else
                            {
                                //add padding 0s
                                combined |= 0ULL << (8 * offset);
                            }
                        }

                        Immediate value;

                        Integer integer;
                        integer.val = combined;
                        integer.isString = false;
                        value.operands.push_back(integer);

                        data.values.push_back(value);
                    }
                }
                else
                    throw Exception::SyntaxError("Expected definition after data definition", filteredTokens[i].line, filteredTokens[i].column);

                i++;
                if (i >= filteredTokens.size()) break;

                if (filteredTokens[i].type == Token::Type::Comma)
                {
                    i++;
                    if (i >= filteredTokens.size())
                        throw Exception::InternalError("Unexpected end after comma", filteredTokens.back().line, filteredTokens.back().column);
                }
                else if (filteredTokens[i].type != Token::Type::EOL)
                {
                    throw Exception::SyntaxError("Expected comma or end of line after data definition", filteredTokens[i].line, filteredTokens[i].column);
                }
            }

            // TODO
            data.alignment = 0;

            currentSection->entries.push_back(data);

            continue;
        }

        // Instructions
        if (lowerVal.compare("mov") == 0)
        {
            Instruction::Instruction instruction;
            instruction.bits = currentBitMode;
            instruction.lineNumber = token.line;
            instruction.column = token.column;
            instruction.mnemonic = Instructions::MOV;

            const Token::Token& operand1 = filteredTokens[++i];
            if (registers.find(toLower(operand1.value)) != registers.end()
             && filteredTokens[i + 1].type != Token::Type::Punctuation)
            {
                // reg
                Instruction::Register reg;
                reg.reg = operand1.value;
                instruction.operands.push_back(reg);
                i++;
            }
            else if ((operand1.type == Token::Type::Bracket && operand1.value == "[")
                  || (registers.find(toLower(operand1.value)) != registers.end()
                   && filteredTokens[i + 1].type != Token::Type::Punctuation))
            {
                // TODO: memory
            }
            else
            {
                // TODO: Error
            }

            if (filteredTokens[i].type != Token::Type::Comma)
                throw Exception::SyntaxError("Expected ',' after first argument for 'mov'", operand1.line, operand1.column);
            i++;

            const Token::Token& operand2 = filteredTokens[i];
            if (registers.find(toLower(operand2.value)) != registers.end()
             && filteredTokens[i + 1].type != Token::Type::Punctuation)
            {
                // reg
                Instruction::Register reg;
                reg.reg = operand2.value;
                instruction.operands.push_back(reg);
                i++;
            }
            else if ((operand1.type == Token::Type::Bracket && operand1.value == "[")
                  || (registers.find(toLower(operand1.value)) != registers.end()
                   && filteredTokens[i + 1].type == Token::Type::Punctuation))
            {
                // TODO: memory
            }
            else
            {
                // TODO: immediate?
                Immediate imm;

                while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                {
                    ImmediateOperand op = getOperand(filteredTokens[i]);
                    imm.operands.push_back(op);
                    i++;
                }
                instruction.operands.push_back(imm);
            }

            if (filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for 'mov'", operand1.line, operand1.column);

            // TODO
            instruction.alignment = 0;

            currentSection->entries.push_back(instruction);

            continue;
        }

        context.warningManager->add(Warning::GeneralWarning("Unhandled token: " + token.what()));
    }
}