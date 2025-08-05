#pragma once

#include <fstream>
#include <unordered_map>
#include "Context.hpp"

struct Definition
{
    std::string name;
    std::string value;
};

class PreProcessor
{
public:
    PreProcessor(const Context& _context, std::ostream* _output, std::istream* _input);
    ~PreProcessor() = default;

    void Process();
    void Print();

private:
    const Context& context;
    std::ostream* output;
    std::istream* input;

    std::unordered_map<std::string, Definition> definitions;

    std::string ProcessLine(const std::string& line);
};