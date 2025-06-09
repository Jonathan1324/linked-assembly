#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <variant>

enum class BitMode { Bits16, Bits32, Bits64 };

struct Instruction {
    std::string mnemonic;
    std::vector<std::string> operands;
    BitMode mode;

    int lineNumber;
};

struct DataDefinition {
    std::string name;
    std::string type;
    std::vector<std::string> values;
    bool reserved;

    int lineNumber;
};

struct Label {
    std::string name;
    size_t instructionIndex;
};

using SectionEntry = std::variant<Instruction, DataDefinition>;

struct Section {
    std::string name;
    std::vector<SectionEntry> entries;
    std::vector<Label> labels;
};

struct Parsed {
    std::vector<Section> sections;
    std::vector<std::string> globals;
    std::vector<std::string> externs;
};

Parsed parseAssembly(std::istream& input);