#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <Architecture.hpp>
#include "../Context.hpp"
#include "Tokenizer.hpp"

namespace Instruction
{
    struct Register
    {
        std::string reg;
    };

    struct Integer
    {
        std::string value;
        uint64_t val;

        bool isString;
    };

    struct Operator
    {
        std::string op;
    };

    struct String
    {
        std::string value;
    };

    using ImmediateOperand = std::variant<Integer, Operator, String>;

    struct Immediate
    {
        std::vector<ImmediateOperand> operands;
    };

    struct Memory
    {
        // TODO
    };

    using Operand = std::variant<Register, Immediate, Memory>;

    struct Instruction
    {
        uint64_t mnemonic;
        std::vector<Operand> operands;
        BitMode bits;
        int alignment;

        size_t lineNumber;
        size_t column;
    };
}

struct DataValue
{
    std::vector<Instruction::ImmediateOperand> operands;
};

struct DataDefinition
{
    size_t size;
    bool reserved;
    std::vector<DataValue> values;
    int alignment;

    size_t lineNumber;
    size_t column;
};

struct Label
{
    std::string name;
    bool isGlobal;

    size_t lineNumber;
    size_t column;
};

using SectionEntry = std::variant<Instruction::Instruction, DataDefinition, Label>;

struct Section
{
    std::string name;
    std::vector<SectionEntry> entries;
};

class Parser
{
public:
    Parser(const Context& _context, Architecture _arch, BitMode _bits);
    virtual ~Parser() = default;

    virtual void Parse(const std::vector<Token::Token>& tokens) = 0;
    void Print();

protected:
    Context context;
    Architecture arch;
    BitMode bits;

    std::string org;
    std::vector<Section> sections;
    std::vector<std::string> externs;
    std::unordered_map<std::string, std::string> constants;
};

Parser* getParser(const Context& _context, Architecture _arch, BitMode _bits);
