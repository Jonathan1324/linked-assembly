#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

#include "architecture/architecture.hpp"

#include "cli/version.h"
#include "cli/help.h"

#include "../version.h"

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: linker <input files> [-o output file]" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
        printVersion(VERSION);
        return 0;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printHelp();
        return 0;
    }

    std::vector<std::string> inputFiles;
    std::string outputPath;

    bool debug = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc)
        {
            if (i + 1 < argc)
            {
                outputPath = argv[++i];
            }
            else
            {
                std::cerr << "Error: Missing output file after -o" << std::endl;
                return 1;
            }
        }

        else if (std::string(argv[i]).find("--debug") == 0)
        {
            debug = true;
        }

        else if (!std::string(argv[i]).empty() && argv[i][0] == '-')
        {
            std::cerr << "Warning: Unknown option " << argv[i] << std::endl;
        }
        else
        {
            inputFiles.push_back(argv[i]);
        }
    }

    if (inputFiles.empty())
    {
        std::cerr << "Enter input file" << std::endl;
        return 1;
    }

    if (outputPath.empty())
    {
        std::filesystem::path inputPath(inputFiles[0]);
        
#ifdef _WIN32
        // Windows
        outputPath = inputPath.stem().string() + ".exe";
#else
        // Anything else
        outputPath = inputPath.stem().string();
#endif
    }

    for (const auto& inputFile : inputFiles)
    {
        std::ifstream file(inputFile, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error opening file: " << inputFile << std::endl;
            return 1;
        }

        char buffer[4];

        file.read(buffer, 4);

        Format format = getFormat(buffer);

        file.close();
    }

    std::ofstream outputFile(outputPath, std::ios::out | std::ios::trunc);
    if (!outputFile)
    {
        std::cerr << "Couldn't open file " << outputPath << std::endl;
        return 1;
    }

    outputFile.close();

    return 0;
}