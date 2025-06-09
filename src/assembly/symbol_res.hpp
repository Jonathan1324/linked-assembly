#pragma once

#include <iostream>
#include <unordered_map>
#include "parser.hpp"

struct Symbol {
    std::string name;
    std::string section;
    size_t offset;
    bool isGlobal;
    bool isExtern;
};

using SymbolTable = std::unordered_map<std::string, Symbol>;

SymbolTable resolveSymbols(const Parsed& parsed);