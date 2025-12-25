#include "Preprocessor.hpp"
#include <Exception.hpp>
#include <util/string.hpp>
#include <cstdint>
#include <sstream>
#include <limits>
#include <io/file.hpp>
#include <unordered_set>

PreProcessor::PreProcessor(const Context& _context)
    : context(_context)
{

}

void PreProcessor::Process(std::ostream* output, std::istream* input, const std::string& filename)
{
    if (!input || !(*input))
        throw Exception::InternalError("Input stream isn't open or is in a bad state", -1, -1);
    if (!output || !(*output))
        throw Exception::InternalError("Output stream isn't open or is in a bad state", -1, -1);
    
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
                else throw Exception::SyntaxError("Missing opening quote or angle bracket in %include", -1, -1);

                size_t firstPos = 0;
                if (rest[firstPos] != openChar)
                    throw Exception::SyntaxError("Opening character not found where expected", -1, -1);

                size_t secondPos = rest.find(closeChar, firstPos + 1);
                if (secondPos == std::string::npos)
                {
                    throw Exception::SyntaxError("Missing closing character in %include", -1, -1);
                }

                std::string filename = rest.substr(firstPos + 1, secondPos - firstPos - 1);
                std::ifstream input;
                std::filesystem::path resolvedPath;
                std::filesystem::path requested(filename);

                if (requested.is_absolute())
                {
                    if (std::filesystem::exists(requested) &&
                        std::filesystem::is_regular_file(requested))
                    {
                        input.open(requested);
                        if (input)
                            resolvedPath = requested;
                    }
                }
                else
                {
                    for (const std::filesystem::path& base : context.include_paths)
                    {
                        std::filesystem::path candidate = base / filename;

                        if (std::filesystem::exists(candidate) && std::filesystem::is_regular_file(candidate))
                        {
                            input.open(candidate);
                            if (input)
                            {
                                resolvedPath = candidate;
                                break;
                            }
                        }
                    }
                }

                std::ostringstream buffer;
                if (!input.is_open())
                {
                    throw Exception::IOError(
                        "Could not open include file: " + filename,
                        -1,
                        -1
                    );
                }

                Process(&buffer, &input, resolvedPath.string());

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
    std::string current = line;
    std::unordered_set<std::string> seen;

    while (true)
    {
        if (!seen.insert(current).second)
            throw Exception::InternalError("Cyclic %define expansion detected", -1, -1);

        std::ostringstream result;
        std::istringstream stream(current);
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
                    result << (it != definitions.end() ? it->second.value : currentToken);
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
            result << (it != definitions.end() ? it->second.value : currentToken);
        }

        std::string expanded = result.str();

        if (expanded == current)
            return expanded;

        current = expanded;
    }
}

void PreProcessor::Print()
{
    for (const auto& [first, second] : definitions)
    {
        std::cout << "[DEF] " << first << " = " << second.value << std::endl;
    }
}