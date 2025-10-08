#include "Parser.hpp"

#include <util/string.hpp>
#include <unordered_set>
#include <array>
#include <algorithm>
#include <x86/Registers.hpp>
#include <x86/Instructions.hpp>
#include "../evaluate.hpp"

x86::Parser::Parser(const Context& _context, Architecture _arch, BitMode _bits)
    : ::Parser::Parser(_context, _arch, _bits)
{

}

Parser::ImmediateOperand getOperand(const Token::Token& token)
{
    if (token.type == Token::Type::Operator || token.type == Token::Type::Bracket)
    {
        Parser::Operator op;
        op.op = token.value;
        return op;
    }
    else if (std::isdigit(static_cast<unsigned char>(token.value[0])) != 0)
    {
        Parser::Integer integer;
        // TODO: currently only integer
        integer.value = evalInteger(token.value, 8, token.line, token.column);
        return integer;
    }
    else if (token.type == Token::Type::Character)
    {
        Parser::Integer integer;
        integer.value = static_cast<uint64_t>(static_cast<unsigned char>(token.value[0]));
        return integer;
    }
    else if (token.type == Token::Type::Token && (token.value == "$" ||token.value == "$$"))
    {
        Parser::CurrentPosition curPos;
        curPos.sectionPos = (token.value == "$") ? false : true;
        return curPos;
    }
    else
    {
        Parser::String str;
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

    static constexpr std::array<std::string_view, 5> directives = {
        "section", "segment", "bits", "org", "align"
    };

    ::Parser::Section text;
    text.name = ".text";
    sections.push_back(text);

    ::Parser::Section* currentSection = &sections.at(0);
    BitMode currentBitMode = bits;

    for (size_t i = 0; i < filteredTokens.size(); i++)
    {
        const Token::Token& token = filteredTokens[i];
        if (token.type == Token::Type::EOL || token.type == Token::Type::_EOF)
            continue;
        
        const std::string lowerVal = toLower(token.value);

        // Constants
        if (filteredTokens[i + 1].type == Token::Type::Token && filteredTokens[i + 1].value.compare("equ") == 0)
        {
            ::Parser::Constant constant;
            constant.lineNumber = token.line;
            constant.column = token.column;
            // TODO: case sensitive
            constant.name = token.value;
            constant.hasPos = false;
            i += 2;

            if (std::find(globals.begin(), globals.end(), token.value) != globals.end())
                constant.isGlobal = true;
            else
                constant.isGlobal = false;

            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character
                 || filteredTokens[i].type == Token::Type::Bracket)
                {
                    while (i < filteredTokens.size() &&
                           !(filteredTokens[i].type == Token::Type::Comma || filteredTokens[i].type == Token::Type::EOL))
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i]);
                        if (std::holds_alternative<::Parser::CurrentPosition>(op) && !constant.hasPos)
                            constant.hasPos = true;
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

        // times
        if (token.type == Token::Type::Token && lowerVal.compare("times") == 0)
        {
            ::Parser::Repetition repetition;
            repetition.lineNumber = token.line;
            repetition.column = token.column;

            i++;
            
            // TODO: strange way
            while (i < filteredTokens.size())
            {
                if (filteredTokens[i].type == Token::Type::Token
                 || filteredTokens[i].type == Token::Type::Operator
                 || filteredTokens[i].type == Token::Type::Character
                 || filteredTokens[i].type == Token::Type::Bracket)
                {
                    ::Parser::ImmediateOperand op = getOperand(filteredTokens[i]);
                    repetition.count.operands.push_back(op);
                }
                else
                    throw Exception::SyntaxError("Unknown value type after 'times'", filteredTokens[i].line, filteredTokens[i].column);
                
                i++;
                if ( i < filteredTokens.size() && filteredTokens[i].type == Token::Type::EOL)
                    break;
            }
            i--;

            currentSection->entries.push_back(repetition);
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
            
            if (lowerDir.compare("section") == 0 || lowerDir.compare("segment") == 0)
            {
                // TODO: currently case insensitive
                std::string name = toLower(filteredTokens[++i].value);

                auto it = std::find_if(sections.begin(), sections.end(), [&](const ::Parser::Section& s) {return s.name == name; });

                if (it == sections.end())
                {
                    // Create new section
                    ::Parser::Section section;
                    section.name = name;
                    sections.emplace_back(section);
                    currentSection = &sections.back();
                }
                else
                {
                    currentSection = &(*it);
                }

                i++;

                if (toLower(filteredTokens[i].value).find("align") == 0)
                {
                    size_t pos = filteredTokens[i].value.find("=");
                    if (pos != 5)
                        context.warningManager->add(Warning::GeneralWarning("Unset section attribute 'align'", filteredTokens[i].line, filteredTokens[i].column));
                    
                    // TODO
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
            else if (lowerDir.compare("align") == 0)
            {
                ::Parser::Alignment align;
                align.lineNumber = directive.line;
                align.column = directive.column;
                i++;
                // TODO: strange way
                while (i < filteredTokens.size())
                {
                    if (filteredTokens[i].type == Token::Type::Token
                    || filteredTokens[i].type == Token::Type::Operator
                    || filteredTokens[i].type == Token::Type::Character
                    || filteredTokens[i].type == Token::Type::Bracket)
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i]);
                        align.align.operands.push_back(op);
                    }
                    else
                        throw Exception::SyntaxError("Unknown value type after 'align'", filteredTokens[i].line, filteredTokens[i].column);
                    
                    i++;
                    if ( i < filteredTokens.size() && filteredTokens[i].type == Token::Type::EOL)
                        break;
                }
                i--;

                currentSection->entries.push_back(align);
            }

            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                i++;
            continue;
        }

        // Labels
        if (token.type == Token::Type::Token &&
           ((filteredTokens[i + 1].type == Token::Type::Punctuation && filteredTokens[i + 1].value == ":" && /*TODO: not segment:offset*/ ::x86::registers.find(token.value) == ::x86::registers.end())
         || (filteredTokens[i + 1].type == Token::Type::Token && std::find(dataDefinitions.begin(), dataDefinitions.end(), toLower(filteredTokens[i + 1].value)) != dataDefinitions.end())))
        {
            ::Parser::Label label;
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
            ::Parser::DataDefinition data;
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
                 || filteredTokens[i].type == Token::Type::Character
                 || filteredTokens[i].type == Token::Type::Bracket)
                {
                    ::Parser::Immediate val;
                    
                    while (i < filteredTokens.size() &&
                           !(filteredTokens[i].type == Token::Type::Comma || filteredTokens[i].type == Token::Type::EOL))
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i]);
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

                        ::Parser::Immediate value;

                        ::Parser::Integer integer;
                        integer.value = combined;
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

            currentSection->entries.push_back(data);

            continue;
        }

        // Instructions

        // CONTROL
        static const std::unordered_map<std::string_view, uint64_t> controlInstructions = {
            {"nop", ::x86::Instructions::NOP},
            {"hlt", ::x86::Instructions::HLT}
        };

        auto it = controlInstructions.find(lowerVal);
        if (it != controlInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::NOP:
                case ::x86::Instructions::HLT:
                    break;

                default:
                    throw Exception::InternalError("Unknown control instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // INTERRUPT
        static const std::unordered_map<std::string_view, uint64_t> interruptInstructions = {
            {"int", ::x86::Instructions::INT}
        };

        it = interruptInstructions.find(lowerVal);
        if (it != interruptInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::INT:
                {
                    // TODO: immediate?
                    ::Parser::Immediate imm;
                    while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                    {
                        ::Parser::ImmediateOperand op = getOperand(filteredTokens[i]);
                        imm.operands.push_back(op);
                        i++;
                    }
                    instruction.operands.push_back(imm);
                } break;

                default:
                    throw Exception::InternalError("Unknown interrupt instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // FLAGS
        static const std::unordered_map<std::string_view, uint64_t> flagInstructions = {
            {"clc", ::x86::Instructions::CLC},
            {"stc", ::x86::Instructions::STC},
            {"cmc", ::x86::Instructions::CMC},
            {"cld", ::x86::Instructions::CLD},
            {"std", ::x86::Instructions::STD},
            {"cli", ::x86::Instructions::CLI},
            {"sti", ::x86::Instructions::STI},
            {"lahf", ::x86::Instructions::LAHF},
            {"sahf", ::x86::Instructions::SAHF}
        };

        it = flagInstructions.find(lowerVal);
        if (it != flagInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::CLC:
                case ::x86::Instructions::STC:
                case ::x86::Instructions::CMC:
                case ::x86::Instructions::CLD:
                case ::x86::Instructions::STD:
                case ::x86::Instructions::CLI:
                case ::x86::Instructions::STI:
                case ::x86::Instructions::LAHF:
                case ::x86::Instructions::SAHF:
                    break;

                default:
                    throw Exception::InternalError("Unknown flag instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        // STACK
        static const std::unordered_map<std::string_view, uint64_t> stackInstructions = {
            {"pusha", ::x86::Instructions::PUSHA},
            {"popa", ::x86::Instructions::POPA},
            {"pushad", ::x86::Instructions::PUSHAD},
            {"popad", ::x86::Instructions::POPAD},
            {"pushf", ::x86::Instructions::PUSHF},
            {"popf", ::x86::Instructions::POPF},
            {"pushfd", ::x86::Instructions::PUSHFD},
            {"popfd", ::x86::Instructions::POPFD},
            {"pushfq", ::x86::Instructions::PUSHFQ},
            {"popfq", ::x86::Instructions::POPFQ}
        };

        it = stackInstructions.find(lowerVal);
        if (it != stackInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::PUSHA:
                case ::x86::Instructions::POPA:
                case ::x86::Instructions::PUSHAD:
                case ::x86::Instructions::POPAD:
                case ::x86::Instructions::PUSHF:
                case ::x86::Instructions::POPF:
                case ::x86::Instructions::PUSHFD:
                case ::x86::Instructions::POPFD:
                case ::x86::Instructions::PUSHFQ:
                case ::x86::Instructions::POPFQ:
                    break;
                
                default:
                    throw Exception::InternalError("Unknown stack instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);

            currentSection->entries.push_back(instruction);
            continue;
        }

        // Data
        static const std::unordered_map<std::string_view, uint64_t> dataInstructions = {
            {"mov", ::x86::Instructions::MOV}
        };

        it = dataInstructions.find(lowerVal);
        if (it != dataInstructions.end())
        {
            ::Parser::Instruction::Instruction instruction(it->second, currentBitMode, token.line, token.column);
            i++;
            switch (instruction.mnemonic)
            {
                case ::x86::Instructions::MOV:
                {
                    const Token::Token& operand1 = filteredTokens[i];
                    auto regIt = ::x86::registers.find(operand1.value);
                    if (regIt != ::x86::registers.end()
                     && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if ((operand1.type == Token::Type::Bracket && operand1.value == "[")
                        || (regIt != ::x86::registers.end()
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
                    regIt = ::x86::registers.find(operand2.value);
                    if (regIt != ::x86::registers.end()
                    && filteredTokens[i + 1].type != Token::Type::Punctuation)
                    {
                        // reg
                        ::Parser::Instruction::Register reg;
                        reg.reg = regIt->second;
                        instruction.operands.push_back(reg);
                        i++;
                    }
                    else if ((operand1.type == Token::Type::Bracket && operand1.value == "[")
                        || (regIt != ::x86::registers.end()
                        && filteredTokens[i + 1].type == Token::Type::Punctuation))
                    {
                        // TODO: memory
                    }
                    else
                    {
                        // TODO: immediate?
                        ::Parser::Immediate imm;

                        while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                        {
                            ::Parser::ImmediateOperand op = getOperand(filteredTokens[i]);
                            imm.operands.push_back(op);
                            i++;
                        }
                        instruction.operands.push_back(imm);
                    }
                } break;

                default:
                    throw Exception::InternalError("Unknown data instruction", token.line, token.column);
            }
            if (i >= filteredTokens.size() || filteredTokens[i].type != Token::Type::EOL)
                throw Exception::SyntaxError("Expected end of line after second argument for '" + lowerVal + "'", token.line, token.column);
            
            currentSection->entries.push_back(instruction);
            continue;
        }

        context.warningManager->add(Warning::GeneralWarning("Unhandled token: " + token.what()));
    }

    // TODO: probably better way to handle this
    if (sections.at(0).entries.empty())
    {
        sections.erase(sections.begin());
    }
}