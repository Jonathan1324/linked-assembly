#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

uint64_t evalInteger(std::string str, size_t size, std::unordered_map<std::string, std::string> constants, int lineNumber);