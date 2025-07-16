#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <io/file.hpp>
#include <Architecture.hpp>
#include <Exception.hpp>
#include "cli/Arguments.hpp"
#include "Context.hpp"

#include "Parser/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Encoder/Encoder.hpp"
#include "ObjectWriter/ObjectWriter.hpp"

int handleError(const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}

int main(int argc, const char *argv[])
{
    WarningManager warningManager;
    Context context;
    context.warningManager = &warningManager;

    std::vector<std::string> inputFiles;
    std::string outputFile;
    BitMode bitMode;
    Architecture arch;
    Format format;
    bool debug;

    // Parse arguments
    try
    {
        bool stop = parseArguments(argc, argv, inputFiles, outputFile, bitMode, arch, format, debug, context);
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
    catch(const std::exception& e)
    {
        return handleError(e);
    }

    context.filename = std::filesystem::path(inputFiles.at(0)).filename().string();

    // Create file handles and tokenize
    std::ofstream objectFile;
    Token::Tokenizer tokenizer;
    try
    {
        objectFile = openOfstream(outputFile, std::ios::out | std::ios::trunc | std::ios::binary);

        tokenizer.clear();
        for (size_t i = 0; i < inputFiles.size(); i++)
        {
            std::ifstream file = openIfstream(inputFiles.at(i));
            tokenizer.tokenize(file);
            file.close();
        }
        if (debug)
            tokenizer.print();
        
        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);

        // delete outputFile
        objectFile.close();
        std::remove(outputFile.c_str());

        return 1;
    }
    catch(const std::exception& e)
    {
        // delete outputFile
        objectFile.close();
        std::remove(outputFile.c_str());

        return handleError(e);
    }

    // Parse
    Parser* parser = getParser(context, arch, bitMode);
    try
    {
        parser->Parse(tokenizer.getTokens());
        if (debug)
            parser->Print();

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        
        // delete outputFile
        objectFile.close();
        std::remove(outputFile.c_str());

        return 1;
    }
    catch(const std::exception& e)
    {
        // delete outputFile
        objectFile.close();
        std::remove(outputFile.c_str());
        
        return handleError(e);
    }
    

    // Encode

    // Create .o/.bin file

    if (parser)
        delete parser;

    return 0;
}