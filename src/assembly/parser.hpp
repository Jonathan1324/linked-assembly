#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <variant>
#include "architecture/architecture.hpp"

struct Instruction {
    std::string mnemonic;
    std::vector<std::string> operands;
    BitMode mode;
    int alignment;

    int lineNumber;
};

struct DataDefinition {
    std::string name;
    std::string type;
    std::vector<std::string> values;
    int alignment;
    bool reserved;

    int lineNumber;
};

struct LocalLabel {
    std::string name;
    size_t instructionIndex;
};

struct Label {
    std::string name;
    size_t instructionIndex;
    std::vector<LocalLabel> localLabels;
    bool isGlobal = false;
};

using SectionEntry = std::variant<Instruction, DataDefinition>;

struct Section {
    std::string name;
    std::vector<SectionEntry> entries;
    std::unordered_map<std::string, Label> labels;
};

struct ConstantDefinition {
    std::string name;
    std::string value;
    int lineNumber;
};

struct Parsed {
    std::vector<Section> sections;
    std::vector<std::string> globals;
    std::vector<std::string> externs;
    std::unordered_map<std::string, ConstantDefinition> constants;
};

Parsed parseAssembly(std::istream& input, BitMode bits);