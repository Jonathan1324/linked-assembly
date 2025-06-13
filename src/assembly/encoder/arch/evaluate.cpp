#include "evaluate.hpp"

#include <iostream>

unsigned long long evaluate(std::string str, std::unordered_map<std::string, std::string> constants, int lineNumber)
{
    std::string value = str;
    if (constants.find(str) != constants.end())
        value = constants[str];

    size_t pos = 0;
    try
    {
        unsigned long long val = std::stoull(value, &pos, 0);

        if (pos != value.size()) {
            std::cout << value << " contains invalid characters (line " << lineNumber << ")" << std::endl;
            return 0;
        }

        return val;
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << value << " not a number (line " << lineNumber << ")" << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << value << " number out of range (line " << lineNumber << ")" << std::endl;
    }
    return 0;
}