#include "evaluate.hpp"

#include <Exception.hpp>

uint64_t evalInteger(std::string str, std::unordered_map<std::string, std::string> constants, int lineNumber)
{
    // TODO: operations
    std::string value = str;
    if (constants.find(str) != constants.end())
        value = constants[str];

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
        if (pos != value.size())
            throw Exception::SemanticError(value + " contains invalid characters", lineNumber);
        
        // Convert negative signed to unsigned two's complement equivalent
        return static_cast<uint64_t>(sval);
    }
    catch (const std::invalid_argument&)
    {
        // Failed signed parse, try unsigned
        pos = 0;
        try
        {
            uint64_t uval = std::stoull(value, &pos, base);
            if (pos != value.size())
                throw Exception::SemanticError(value + " contains invalid characters", lineNumber);
        
            return uval;
        }
        catch (const std::invalid_argument&)
        {
            throw Exception::SemanticError(value + " not a number", lineNumber);
        }
        catch (const std::out_of_range&)
        {
            throw Exception::OverflowError(value + " number out of range", lineNumber);
        }
    }
    catch (const std::out_of_range&)
    {
        throw Exception::OverflowError(value + " number out of range", lineNumber);
    }

    return 0;
}