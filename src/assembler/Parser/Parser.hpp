#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <Architecture.hpp>
#include "../Context.hpp"
#include "Tokenizer.hpp"

class Parser
{
public:
    Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);
    virtual ~Parser() = default;

    virtual void Parse(const std::vector<Token::Token>& tokens) = 0;
    void Print();

    struct Instruction
    {
        
    };

    struct DataDefinition
    {

    };

    struct Label
    {

    };

    using SectionEntry = std::variant<Instruction, DataDefinition, Label>;

    struct Section
    {
        std::string name;
        std::vector<SectionEntry> entries;
    };

protected:
    Context context;
    Architecture arch;
    BitMode bits;
    Endianness endianness;

    uint64_t org = 0;
    std::vector<Section> sections;
};

// FIXME: only temporary solution
Parser* getParser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);