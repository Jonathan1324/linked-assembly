#include "Preprocessor.hpp"
#include <Exception.hpp>
#include <util/string.hpp>
#include <cstdint>
#include <sstream>
#include <limits>
#include <io/file.hpp>

PreProcessor::PreProcessor(const Context& _context)
    : context(_context)
{

}

void PreProcessor::Process(std::ostream* output, std::istream* input, const std::string& filename)
{
    if (!input || !(*input))
        throw Exception::InternalError("Input stream isn't open or is in a bad state");
    if (!output || !(*output))
        throw Exception::InternalError("Output stream isn't open or is in a bad state");
    
    int64_t inputLine = 0;
    int64_t outputLine = std::numeric_limits<int64_t>::min();

    std::string line;
    while (std::getline(*input, line))
    {
        inputLine++;
        std::string trimmed = trim(line);
        

        if (outputLine != inputLine && trimmed[0] != '%')
        {
            if (trimmed.empty()) continue;
            if ((inputLine - outputLine) == 1)
            {
                (*output) << "\n";
                outputLine++;
            }
            else
            {
                (*output) << "%line " << inputLine << "+1 " << filename << "\n";
                outputLine = inputLine;
            }
        }

        // Multiline support
        while (!trimmed.empty() && trimmed.back() == '\\')
        {
            trimmed.pop_back();
            line.pop_back();
            std::string next;
            if (!std::getline(*input, next)) break;
            inputLine++;
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

            else if (trimmed.find("%include") == 0)
            {
                std::string rest = trim(trimmed.substr(8));

                char openChar = 0;
                char closeChar = 0;

                if (!rest.empty() && rest.front() == '"')
                {
                    openChar = '"';
                    closeChar = '"';
                }
                else if (!rest.empty() && rest.front() == '<')
                {
                    openChar = '<';
                    closeChar = '>';
                }
                else throw Exception::SyntaxError("Missing opening quote or angle bracket in %include");

                size_t firstPos = 0;
                if (rest[firstPos] != openChar)
                    throw Exception::SyntaxError("Opening character not found where expected");

                size_t secondPos = rest.find(closeChar, firstPos + 1);
                if (secondPos == std::string::npos)
                {
                    throw Exception::SyntaxError("Missing closing character in %include");
                }

                std::string filename = rest.substr(firstPos + 1, secondPos - firstPos - 1);

                std::ostringstream buffer;
                std::ifstream input(filename);
                if (!input) throw Exception::IOError("Could not open include file: " + filename);

                Process(&buffer, &input, filename);

                (*output) << buffer.str();

                outputLine = std::numeric_limits<int64_t>::min();
                continue;
            }
        }

        if (line[0] == ' ') (*output) << " ";
        (*output) << ProcessLine(trimmed) << "\n";
        outputLine++;
    }
}

size_t countTrailingBackslashes(const std::ostringstream& oss)
{
    std::string s = oss.str();
    size_t count = 0;
    for (auto it = s.rbegin(); it != s.rend(); ++it)
    {
        if (*it == '\\') count++;
        else break;
    }
    return count;
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
            size_t bsCount = countTrailingBackslashes(result);
            bool escaped = (bsCount % 2 == 1);

            if (!escaped) inString = !inString;
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