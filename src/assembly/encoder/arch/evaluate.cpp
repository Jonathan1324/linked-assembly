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
        // Try signed first
        long long sval = std::stoll(value, &pos, 0);
        if (pos != value.size()) {
            std::cout << value << " contains invalid characters (line " << lineNumber << ")" << std::endl;
            return 0;
        }
        // Convert negative signed to unsigned two's complement equivalent
        return static_cast<unsigned long long>(sval);
    }
    catch (const std::invalid_argument&)
    {
        // Failed signed parse, try unsigned
        pos = 0;
        try
        {
            unsigned long long uval = std::stoull(value, &pos, 0);
            if (pos != value.size()) {
                std::cout << value << " contains invalid characters (line " << lineNumber << ")" << std::endl;
                return 0;
            }
            return uval;
        }
        catch (const std::invalid_argument&)
        {
            std::cout << value << " not a number (line " << lineNumber << ")" << std::endl;
        }
        catch (const std::out_of_range&)
        {
            std::cout << value << " number out of range (line " << lineNumber << ")" << std::endl;
        }
    }
    catch (const std::out_of_range&)
    {
        std::cout << value << " number out of range (line " << lineNumber << ")" << std::endl;
    }

    return 0;
}