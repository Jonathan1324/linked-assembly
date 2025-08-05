#pragma once

#include "Encoder.hpp"

std::vector<std::string> Encoder::Encoder::getDependencies(Parser::Immediate immediate)
{
    std::vector<std::string> deps;
    for (const auto& operand : immediate.operands)
    {
        if (std::holds_alternative<Parser::String>(operand))
        {
            const Parser::String& str = std::get<Parser::String>(operand);
            deps.push_back(str.value);
        }
    }
    return deps;
}

bool Encoder::Encoder::hasPos(Constant& c, std::unordered_set<std::string>& visited)
{
    if (c.hasPos == HasPos::TRUE) return true;
    if (c.hasPos == HasPos::FALSE) return false;

    if (visited.count(c.name))
        throw Exception::SemanticError("Circular dependency at " + c.name); // TODO: no line or column
    
    visited.insert(c.name);

    auto deps = getDependencies(c.expression);

    for (const auto& depName : deps)
    {
        auto labelIt = labels.find(depName);
        if (labelIt != labels.end())
        {
            c.hasPos = HasPos::TRUE;
            visited.erase(c.name);
            return true;
        }
        
        auto constIt = constants.find(depName);
        if (constIt == constants.end())
            throw Exception::InternalError("Unknown dependency: " + depName);

        if (hasPos(constIt->second, visited))
        {
            c.hasPos = HasPos::TRUE;
            visited.erase(c.name);
            return true;
        }
    }

    c.hasPos = HasPos::FALSE;
    visited.erase(c.name);
    return false;
}