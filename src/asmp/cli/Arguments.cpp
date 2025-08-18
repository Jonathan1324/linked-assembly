#include "Arguments.hpp"

#include <cstring>
#include <Exception.hpp>
#include <version.h>
#include <util/string.hpp>

void printHelp()
{
    //TODO
    fflush(stdout);
}

bool parseArguments(int argc, const char *argv[], std::string& input, std::string& output, bool& debug, const Context& context)
{
    if (argc < 2)
    {
        throw Exception::ArgumentError("No input file specified");
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
        printVersion();
        return true;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printHelp();
        return true;
    }

    debug = false;

    bool inputSet = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]).compare("-o") == 0)
        {
            if (i + 1 < argc)
                output = argv[++i];
            else
                throw Exception::ArgumentError("Missing output file after '-o'");
        }
        else if (std::string(argv[i]).compare("--debug") == 0 || std::string(argv[i]).compare("-d") == 0)
        {
            debug = true;
        }
        else if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            context.warningManager->add(Warning::ArgumentWarning("Unknown option: " + std::string(argv[i])));
        }
        else
        {
            if (inputSet)
                throw Exception::ArgumentError("Too many input streams specified");
            input = argv[i];
            inputSet = true;
        }
    }

    if (input.empty())
        throw Exception::ArgumentError("No input file entered");

    if (output.empty())
    {
        size_t pos = input.rfind('.');
        if (pos == std::string::npos)
            output = input + ".i";
        else
            output = input.substr(0, pos) + ".i" + input.substr(pos);
    }

    return false;
}