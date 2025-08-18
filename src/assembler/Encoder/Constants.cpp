#include "Encoder.hpp"
#include <limits>

void Encoder::Encoder::resolveConstants(bool withPos)
{
    std::unordered_set<std::string> visited;
    
    for (auto& [name, constant] : constants)
    {
        if (!constant.resolved)
        {
            if (withPos)
                resolveConstantWithPos(constant, visited);
            else
                resolveConstantWithoutPos(constant, visited);
        }
    }
}

bool Encoder::Encoder::Resolvable(const Parser::Immediate& immediate)
{
    for (const auto& dep : getDependencies(immediate))
    {
        auto itLabel = labels.find(dep);
        if (itLabel != labels.end())
            if (!itLabel->second.resolved) return false;

        auto itConstant = constants.find(dep);
        if (itConstant != constants.end())
            if (!itConstant->second.resolved) return false;
    }
    return true;
}

std::vector<std::string> Encoder::Encoder::getDependencies(const Parser::Immediate& immediate)
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

bool Encoder::Encoder::resolveConstantWithoutPos(Constant& c, std::unordered_set<std::string>& visited)
{
    if (c.resolved) return true;
    if (c.hasPos == HasPos::TRUE) return false;

    if (visited.count(c.name))
        throw Exception::SemanticError("Circular dependency at " + c.name); // FIXME: no line or column
    visited.insert(c.name);

    for (const auto& dep : getDependencies(c.expression))
    {
        if (labels.count(dep))
        {
            c.hasPos = HasPos::TRUE;
            visited.erase(c.name);
            return false;
        }

        auto it = constants.find(dep);
        if (it == constants.end())
            throw Exception::InternalError("Unknown dependency: " + dep);

        if (!resolveConstantWithoutPos(it->second, visited))
        {
            c.hasPos = HasPos::TRUE;
            visited.erase(c.name);
            return false;
        }
    }

    Evaluation evaluated = Evaluate(c.expression, 0, 0, &c.section);
    if (evaluated.relocationPossible) c.relocationPossible = true;
    if (evaluated.relocationPossible && evaluated.useOffset)
    {
        c.useOffset = true;
        c.off = evaluated.offset;
        c.usedSection = evaluated.usedSection;
    }
    else
    {
        const Int128& value = evaluated.result;

        if (value < static_cast<Int128>(std::numeric_limits<int64_t>::min()) ||
            value > static_cast<Int128>(std::numeric_limits<int64_t>::max()))
            throw Exception::OverflowError("Constant '" + c.name + "' too big for a signed 64-bit integer"); // FIXME: no line or column
        c.value = static_cast<int64_t>(value);
    }
    c.resolved = true;
    c.prePass = true;

    c.hasPos = HasPos::FALSE;
    visited.erase(c.name);
    return true;
}

bool Encoder::Encoder::resolveConstantWithPos(Constant& c, std::unordered_set<std::string>& visited)
{
    if (c.resolved) return true;
    
    if (visited.count(c.name))
        throw Exception::SemanticError("Circular dependency at " + c.name); // FIXME: no line or column
    visited.insert(c.name);

    for (const auto& dep : getDependencies(c.expression))
    {
        if (labels.count(dep)) continue;
        
        auto it = constants.find(dep);
        if (it == constants.end())
            throw Exception::InternalError("Unknown dependency: " + dep);

        if (!resolveConstantWithPos(it->second, visited))
            throw Exception::InternalError("Couldn't resolve constant '" + dep + "'");
    }

    Evaluation evaluated = Evaluate(c.expression, c.bytesWritten, c.offset, &c.section);
    if (evaluated.relocationPossible) c.relocationPossible = true;
    if (evaluated.relocationPossible && evaluated.useOffset)
    {
        c.useOffset = true;
        c.off = evaluated.offset;
        c.usedSection = evaluated.usedSection;
    }
    else
    {
        const Int128& value = evaluated.result;
    
        if (value < static_cast<Int128>(std::numeric_limits<int64_t>::min()) ||
            value > static_cast<Int128>(std::numeric_limits<int64_t>::max()))
            throw Exception::OverflowError("Constant '" + c.name + "' too big for a signed 64-bit integer"); // FIXME: no line or column
        c.value = static_cast<int64_t>(value);
    }
    c.resolved = true;

    visited.erase(c.name);
    return true;
}