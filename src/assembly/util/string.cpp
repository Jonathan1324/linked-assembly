#include "string.hpp"
#include <algorithm>
#include <cctype>

// Hilfsfunktion zum Trimmen
std::string trim(const std::string& str)
{
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}