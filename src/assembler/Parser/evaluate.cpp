#include "evaluate.hpp"

#include <Exception.hpp>

static int64_t sign_extend(uint64_t val, size_t size_bytes)
{
    size_t bits = size_bytes * 8;
    uint64_t mask = 1ULL << (bits - 1);
    if (val & mask)
    {
        uint64_t extend_mask = ~((1ULL << bits) - 1);
        return static_cast<int64_t>(val | extend_mask);
    }
    else
    {
        return static_cast<int64_t>(val & ((1ULL << bits) - 1));
    }
}

uint64_t evalInteger(std::string str, size_t size, int lineNumber, int column)
{
    std::string& value = str;

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
    uint64_t rawValue = 0;
    try
    {
        // Try signed first
        long long sval = std::stoll(value, &pos, base);
        if (pos != value.size())
            throw Exception::SemanticError(value + " contains invalid characters", lineNumber, column);
        
        // Convert negative signed to unsigned two's complement equivalent
        rawValue = static_cast<uint64_t>(sval);
    }
    catch (const std::invalid_argument&)
    {
        // Failed signed parse, try unsigned
        pos = 0;
        try
        {
            uint64_t uval = std::stoull(value, &pos, base);
            if (pos != value.size())
                throw Exception::SemanticError(value + " contains invalid characters", lineNumber, column);
            rawValue = uval;
        }
        catch (const std::invalid_argument&)
        {
            throw Exception::SemanticError(value + " not a number", lineNumber, column);
        }
        catch (const std::out_of_range&)
        {
            throw Exception::OverflowError(value + " number out of range", lineNumber, column);
        }
    }
    catch (const std::out_of_range&)
    {
        throw Exception::OverflowError(value + " number out of range", lineNumber, column);
    }

    if (size >= 8)
        return rawValue;
    else
    {
        uint64_t mask = (1ULL << (size * 8)) - 1;
        return rawValue & mask;
    }

    return 0;
}