#pragma once

#include <unordered_map>
#include <string>

unsigned long long evaluate(std::string str, std::unordered_map<std::string, std::string> constants, int lineNumber, bool floatingPoint = 0);