#include <iostream>
#include <fstream>
#include <string>

#include "debug.hpp"
#include "arguments.hpp"
#include "parser/parser.hpp"
#include "parser/symbol.hpp"
#include "encoder/encoder.hpp"
#include "object/object.hpp"
#include "io/file.hpp"
#include "cli/version.h"
#include "cli/help.h"
#include "Architecture.hpp"
#include "Exception.hpp"
#include "Context.hpp"

int handleError(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
}

int main(int argc, const char *argv[])
{
    WarningManager warningManager;
    Context context;
    context.warningManager = &warningManager;

    std::string input_path;
    std::string output_path;
    BitMode bitMode;
    Architecture arch;
    Format format;
    Endianness endianness;
    bool debug;

    // Parse arguments
    try
    {
        bool stop = parseArguments(argc, argv, input_path, output_path, bitMode, arch, format, endianness, debug, context);
        if (stop)
            return 0;
        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { handleError(e); }
    
    // create file handles
    std::ifstream file;
    std::ofstream objectFile;
    try
    {
        file = openIfstream(input_path);
        objectFile = openOfstream(output_path, std::ios::out | std::ios::trunc);
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { handleError(e); }

    // Parse
    Parsed parsed;
    try
    {
        parsed = parseAssembly(file, bitMode, context);
        resolveParsed(parsed, context);
        if (debug)
            printParsed(parsed);
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { handleError(e); }

    // Encode
    Encoded encoded;
    try
    {
        encoded = encode(parsed, arch, endianness, context);

        if (debug)
            printEncoded(encoded);
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { handleError(e); }

    // Create .o file
    try
    {
        createFile(format, objectFile, bitMode, arch, encoded, parsed, endianness, context, debug);
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { handleError(e); } 

    return 0;
}