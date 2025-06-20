#include "evaluate.hpp"

#include <iostream>
#include "../escapes.hpp"

unsigned long long evaluate(std::string str, std::unordered_map<std::string, std::string> constants, int lineNumber, bool floatingPoint)
{
    std::string value = str;
    if (constants.find(str) != constants.end())
        value = constants[str];

    if (str.find('\'') == 0)
    {
        if (escapeSign.find(str[1]) != escapeSign.end())
        {
            unsigned char val = escapeSign.at(str[1]);
            if (val == hex)
            {
                //TODO: hex number
            }
            else if (val == octal)
            {
                if (str[2] == '\'')
                    return 0;
                //TODO: octal number
            }
            return val;
        }
        else
        {
            std::cout << value << " invalid symbol after '\\' (line " << lineNumber << ")" << std::endl;
            return 0;
        }
    }

    //TODO: floating point
    (void)floatingPoint;

    int base = 0;

    if (value.find('b') == value.size() - 1
     || value.find('y') == value.size() - 1)
    {
        value = value.substr(0, value.size() - 1);
        base = 2;
    }
    else if (value.find('o') == value.size() - 1
          || value.find('q') == value.size() - 1)
    {
        value = value.substr(0, value.size() - 1);
        base = 8;
    }
    else if (value.find('d') == value.size() - 1
          || value.find('t') == value.size() - 1)
    {
        value = value.substr(0, value.size() - 1);
        base = 10;
    }
    else if (value.find('h') == value.size() - 1
          || value.find('x') == value.size() - 1)
    {
        value = value.substr(0, value.size() - 1);
        base = 16;
    }
    if (value.find("0b") == 0
     || value.find("0y") == 0)
    {
        value = value.substr(2);
        base = 2;
    }
    else if (value.find("0o") == 0
          || value.find("0q") == 0)
    {
        value = value.substr(2);
        base = 8;
    }
    else if (value.find("0d") == 0
          || value.find("0t") == 0)
    {
        value = value.substr(2);
        base = 10;
    }
    else if (value.find("0x") == 0
          || value.find("0h") == 0)
    {
        value = value.substr(2);
        base = 16;
    }
    
    size_t pos = 0;
    try
    {
        // Try signed first
        long long sval = std::stoll(value, &pos, base);
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
            unsigned long long uval = std::stoull(value, &pos, base);
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