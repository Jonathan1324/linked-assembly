#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "../parser.hpp"

using sectionBuffer = std::vector<unsigned char>;

struct EncodedLocalLabel {
    std::string name;
    size_t offset;
};

struct EncodedLabel {
    std::string name;
    size_t offset;
    std::unordered_map<std::string, EncodedLocalLabel> localLabels;
    bool isGlobal = false;
};

enum class Type {
    Absolute,
    Relative,
    PCRelative,
    GOT,
    PLT,
};

struct Relocation {
    size_t offsetInSection;
    std::string labelName;
    Type type;
    int64_t addend = 0;
};

struct EncodedSection {
    std::string name;
    sectionBuffer buffer;
    std::unordered_map<std::string, EncodedLabel> labels;
    std::vector<Relocation> relocations;
};

struct Encoded {
    std::unordered_map<std::string, EncodedSection> sections;
};

Encoded encode(Parsed& parsed, Architecture arch);