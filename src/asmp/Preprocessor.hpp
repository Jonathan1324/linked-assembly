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
    PreProcessor(const Context& _context);
    ~PreProcessor() = default;

    void Process(std::ostream* output, std::istream* input, const std::string& filename);
    void Print();

private:
    const Context& context;

    std::unordered_map<std::string, Definition> definitions;

    std::string ProcessLine(const std::string& line);
};