#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <io/file.hpp>
#include <Architecture.hpp>
#include <Exception.hpp>
#include <StringPool.hpp>
#include "cli/Arguments.hpp"
#include "Context.hpp"

#include "Parser/Tokenizer.hpp"
#include "Parser/Parser.hpp"
#include "Encoder/Encoder.hpp"
#include "OutputWriter/OutputWriter.hpp"
#include <rust.h>

#define CLEANUP                             \
    do {                                    \
    if (parser) delete parser;              \
    if (encoder) delete encoder;            \
    if (outputWriter) delete outputWriter;  \
    if (outputFile != "-" && objectFile) delete objectFile;\
    } while(0)                              \

#define ERROR_HANDLER                       \
    do {                                    \
    /* delete outputFile */                 \
    if (outputFile != "-" && objectFile)    \
    {                                       \
        delete objectFile;                  \
        objectFile = nullptr;               \
    }                                       \
    std::remove(outputFile.c_str());        \
    CLEANUP;                                \
    } while(0)                              \

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
    bool doPreprocess;

    Parser::Parser* parser = nullptr;
    Encoder::Encoder* encoder = nullptr;
    Output::Writer* outputWriter = nullptr;

    StringPool filenamePool;

    // Parse arguments
    try
    {
        bool stop = parseArguments(argc, argv, inputFiles, outputFile, bitMode, arch, format, debug, doPreprocess, context);
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

    context.filename = std::filesystem::path(inputFiles.at(0)).string();

    // Create file handles and tokenize
    std::ostream* objectFile = nullptr;
    Token::Tokenizer tokenizer(context);
    try
    {
        objectFile = openOstream(outputFile, std::ios::out | std::ios::trunc | std::ios::binary);

        tokenizer.clear();
        for (size_t i = 0; i < inputFiles.size(); i++)
        {
            std::istream* file = openIstream(inputFiles.at(i));
            std::istream* input = file;

            if (doPreprocess)
            {
                std::stringstream* preprocessed = new std::stringstream();

                std::string in_buf((std::istreambuf_iterator<char>(*file)),
                      std::istreambuf_iterator<char>());

                char* out_buf = nullptr;
                char* err_buf = nullptr;

                std::string cmd = getExecutableDir() + "/asmp";
                cmd += " - -o -";
                int32_t code = run_program(cmd.c_str(), in_buf.c_str(), &out_buf, &err_buf);

                if (code != 0)
                {
                    if (err_buf)
                    {
                        std::cerr << err_buf;
                        free_c_string(err_buf);
                    }
                    throw Exception::InternalError("Preprocessor failed");
                }

                if (out_buf)
                {
                    *preprocessed << out_buf;
                    free_c_string(out_buf);
                }

                if (err_buf)
                {
                    std::cerr << err_buf;;
                    free_c_string(err_buf);
                }

                if (inputFiles.at(i) != "-")
                    delete file;

                preprocessed->seekg(0);
                input = preprocessed;
            }

            tokenizer.tokenize(input);

            if (input != file)
                delete input;
            else if (inputFiles.at(i) != "-")
                delete file;
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
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // Parse
    try
    {
        parser = Parser::getParser(context, arch, bitMode);
        if (!parser)
            throw Exception::InternalError("Couldn't get parser");
        
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
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // Encode
    try
    {
        encoder = Encoder::getEncoder(context, arch, bitMode, parser);
        if (!encoder)
            throw Exception::InternalError("Couldn't get encoder");

        encoder->Encode();
        encoder->Optimize();
        if (debug)
            encoder->Print();

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // Create .o/.bin file
    try
    {
        outputWriter = Output::getWriter(context, arch, bitMode, format, objectFile, parser, encoder);
        if (!outputWriter)
            throw Exception::InternalError("Couldn't get outputWriter");

        outputWriter->Write();
        if (debug)
            outputWriter->Print();

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        ERROR_HANDLER;
        return 1;
    }
    catch(const std::exception& e)
    {
        ERROR_HANDLER;
        return handleError(e);
    }

    // cleanup
    CLEANUP;

    return 0;
}