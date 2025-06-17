#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>
#include "../parser/parser.hpp"
#include "../Context.hpp"

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
    None,
    Absolute,
    Relative,
    PCRelative,
    GOT,
    PLT,
    Copy,
    GlobDat,
    JmpSlot,
    GOTPC,
    Size,
    //...
};

struct Relocation {
    size_t offsetInSection;
    std::string labelName;
    Type type;
    uint8_t size;
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

Encoded encode(Parsed& parsed, Architecture arch, Endianness endianness, Context& context);