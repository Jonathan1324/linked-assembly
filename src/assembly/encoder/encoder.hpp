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

struct EncodedSection {
    std::string name;
    sectionBuffer buffer;
    std::unordered_map<std::string, EncodedLabel> labels;
};

enum class Type {
    Absolute,
    Relative,
    PCRelative,
    GOT,
    PLT,
};

struct Relocation {
    std::string sectionName;
    size_t offsetInSection;
    uint8_t size;
    std::string labelName;
    Type type;
    int64_t addend = 0;
};

struct Encoded {
    std::unordered_map<std::string, EncodedSection> sections;
    std::vector<Relocation> relocations;
};

Encoded encode(Parsed& parsed, Architecture arch);