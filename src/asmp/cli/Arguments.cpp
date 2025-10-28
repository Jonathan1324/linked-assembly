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
        throw Exception::ArgumentError("No input file specified", -1, -1, "command-line");
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
                throw Exception::ArgumentError("Missing output file after '-o'", -1, -1, "command-line");
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
                throw Exception::ArgumentError("Too many input streams specified", -1, -1, "command-line");
            input = argv[i];
            inputSet = true;
        }
    }

    if (input.empty())
        throw Exception::ArgumentError("No input file entered", -1, -1, "command-line");

    if (output.empty())
    {
        output = "-";
    }

    return false;
}