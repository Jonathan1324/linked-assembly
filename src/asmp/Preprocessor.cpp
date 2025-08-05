#include "Preprocessor.hpp"
#include <Exception.hpp>
#include <util/string.hpp>
#include <cstdint>
#include <sstream>

PreProcessor::PreProcessor(const Context& _context, std::ostream* _output, std::istream* _input)
    : context(_context), output(_output), input(_input)
{

}

void PreProcessor::Process()
{
    if (!input || !(*input))
        throw Exception::InternalError("Input stream isn't open or is in a bad state");
    if (!output || !(*output))
        throw Exception::InternalError("Output stream isn't open or is in a bad state");

    std::string line;
    while (std::getline(*input, line))
    {
        std::string trimmed = trim(line);

        // Multiline support
        while (!trimmed.empty() && trimmed.back() == '\\')
        {
            trimmed.pop_back();
            std::string next;
            if (!std::getline(*input, next)) break;
            trimmed += trim(next);
        }

        // TODO: I know, ugly
        if (!trimmed.empty() && trimmed.find("%") == 0)
        {
            if (trimmed.find("%define") == 0)
            {
                std::string rest = trim(trimmed.substr(7));

                uint64_t space_pos = rest.find(' ');

                Definition def;
                if (space_pos == std::string::npos)
                {
                    def.name = rest;
                    def.value = "";
                }
                else
                {
                    def.name = rest.substr(0, space_pos);
                    def.value = trim(rest.substr(space_pos + 1));
                }

                definitions[def.name] = def;
                continue;
            }
            else if (trimmed.find("%undef") == 0)
            {
                std::string rest = trim(trimmed.substr(6));
                if (!rest.empty())
                    definitions.erase(rest);
                continue;
            }
        }
        
        (*output) << ProcessLine(line) << "\n";
    }
}

std::string PreProcessor::ProcessLine(const std::string& line)
{
    std::ostringstream result;
    std::istringstream stream(line);
    std::string word;
    bool inString = false;
    char ch;
    std::string currentToken;

    while (stream.get(ch))
    {
        if (ch == '"')
        {
            inString = !inString;
            result << ch;
            continue;
        }

        if (inString)
        {
            result << ch;
            continue;
        }

        if (std::isspace(ch) || ch == '%' || ch == ',' || ch == ';')
        {
            if (!currentToken.empty())
            {
                auto it = definitions.find(currentToken);
                if (it != definitions.end())
                    result << it->second.value;
                else
                    result << currentToken;
                currentToken.clear();
            }

            result << ch;
        }
        else
            currentToken += ch;
    }

    if (!currentToken.empty())
    {
        auto it = definitions.find(currentToken);
        if (it != definitions.end())
            result << it->second.value;
        else
            result << currentToken;
    }

    return result.str();
}

void PreProcessor::Print()
{
    for (const auto& [first, second] : definitions)
    {
        std::cout << "[DEF] " << first << " = " << second.value << std::endl;
    }
}