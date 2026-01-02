#include "warning.hpp"

Warning::Warning(Type _type, const std::string& _message, int _line, int _column)
    : type(_type), message(_message), line(_line), column(_column)
{

}

const char* Warning::what() const noexcept
{
    return message.c_str();
}

Warning::Type Warning::getType() const noexcept
{
    return type;
}

void Warning::print(std::ostream& os) const
{
    std::string type = typeToString();
    if (!type.empty())
        os << "(" << type << ") ";
    if (line >= 0)
        os << "On line " << line;
    if (line >= 0)
        os << " in column" << column;
    os << ": " << message << std::endl;
}

std::string Warning::typeToString() const
{
    switch (type)
    {
        case Type::Argument:        return "Argument";

        case Type::None: default:   return "";
    }
}

Warning Warning::GeneralWarning(const std::string& message, int line, int column)
{
    return Warning(Type::None, message, line, column);
}

Warning Warning::ArgumentWarning(const std::string& message, int line, int column)
{
    return Warning(Type::Argument, message, line, column);
}


WarningManager::WarningManager()
{

}

void WarningManager::add(const Warning& w)
{
    warnings_.push_back(w);
}

void WarningManager::printAll(std::ostream& os) const
{
    for (const Warning& w : warnings_)
    {
        os << "[Warning] ";
        w.print(os);
    }
}

bool WarningManager::hasWarnings() const
{
    return !warnings_.empty();
}

void WarningManager::clear()
{
    warnings_.clear();
}