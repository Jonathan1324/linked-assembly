#include "Preprocessor.hpp"
#include <Exception.hpp>
#include <util/string.hpp>

PreProcessor::PreProcessor(const Context& _context, std::ofstream& _output, std::ifstream& _input)
    : context(_context), output(_output), input(_input)
{

}

void PreProcessor::Process()
{
    if (!input.is_open())
        throw Exception::InternalError("Input file isn't open");
    if (!output.is_open())
        throw Exception::InternalError("Output file isn't open");

    std::string line;
    while (std::getline(input, line))
    {
        std::string trimmed = trim(line);

        // TODO: I know, ugly
        if (!trimmed.empty() && trimmed.find("%") == 0)
        {
            if (trimmed.find("%define") == 0)
            {
                continue;
            }
        }

        output << line << "\n";
    }
}