#include <iostream>
#include <fstream>

#include "parser.hpp"
#include "symbol.hpp"
#include "encoder/encoder.hpp"

#include "debug.hpp"

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: assembly <input.asm> [-o output.o]" << std::endl;
        return 1;
    }
    
    std::string input_path = "";
    std::string output_path = "out.o";

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            output_path = argv[++i];
        }

        else
        {
            input_path = argv[i];
        }
        // TODO: weitere Optionen: --dump, --verbose, ...
    }

    if (input_path.empty())
    {
        std::cerr << "Enter input file" << std::endl;
        return 1;
    }

    std::ifstream file(input_path);
    if (!file.is_open())
    {
        std::cerr << "Couldn't open file " << input_path << std::endl;
        return 1;
    }

    Parsed parsed = parseAssembly(file);

    file.close();

    resolveParsed(parsed);

    //TODO
    printParsed(parsed);

    std::ofstream out(output_path, std::ios::out | std::ios::trunc);
    if (!out)
    {
        std::cerr << "Couldn't open file " << output_path << std::endl;
        return 1;
    }

    Encoded encoded = encode(parsed);

    out.close();

    return 0;
}