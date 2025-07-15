#include "Parser.hpp"

#include <util/string.hpp>
#include <unordered_set>
#include <array>
#include <algorithm>
#include "../evaluate.hpp"

x86::Parser::Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness)
    : ::Parser(_context, _arch, _bits, _endianness)
{

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

    Section text;
    text.name = ".text";
    sections.push_back(text);
    Section* currentSection = &sections.at(0);

    for (size_t i = 0; i < filteredTokens.size(); i++)
    {
        const Token::Token& token = filteredTokens[i];
        const std::string lowerVal = toLower(token.value);

        // Macros and constants
        // ('%')
        if (token.type == Token::Type::Macro)
        {
            //TODO
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL && filteredTokens[i].type != Token::Type::_EOF)
                i++;
            continue;
        }

        // Directives
        if (lowerVal.compare("section") == 0)
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
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
                i++;
            continue;
        }

        // Labels
        // TODO

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
                default: throw Exception::InternalError("Unknown size suffix");
            }

            i++;
            while (i < filteredTokens.size() && filteredTokens[i].type != Token::Type::EOL)
            {
                // TODO: strings
                if (filteredTokens[i].type == Token::Type::Token)
                {
                    // TODO: constants
                    // TODO: not just integers
                    uint64_t val = evalInteger(filteredTokens[i].value, std::unordered_map<std::string, std::string>{}, filteredTokens[i].line);
                    data.values.push_back(val);
                }
                else if (filteredTokens[i].type == Token::Type::Character)
                {
                    uint64_t val = filteredTokens[i].value[0];
                    data.values.push_back(val);
                }
                else
                    throw Exception::SyntaxError("Expected definition after data definition", filteredTokens[i].line);

                i++;
                if (i >= filteredTokens.size()) break;

                if (filteredTokens[i].type == Token::Type::Comma)
                {
                    i++;
                    if (i >= filteredTokens.size())
                        throw Exception::SyntaxError("Unexpected end after comma", filteredTokens.back().line);
                }
                else if (filteredTokens[i].type != Token::Type::EOL)
                {
                    throw Exception::SyntaxError("Expected comma or end of line after data definition", filteredTokens[i].line);
                }
            }

            // TODO
            data.alignment = 0;

            currentSection->entries.push_back(data);

            continue;
        }

        context.warningManager->add(Warning::GeneralWarning("Unhandled token: " + token.what()));
    }
}