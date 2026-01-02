#include "string.hpp"
#include <algorithm>
#include <cctype>

// trim strings
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

// turn strings lowercase
std::string toLower(const std::string& input)
{
    std::string result = input;  // Kopiere den Input
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}