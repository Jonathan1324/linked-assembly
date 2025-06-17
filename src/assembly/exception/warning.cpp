#include "warning.hpp"

Warning::Warning(Type type, const std::string& message, int line)
    : type_(type), message_(message), line_(line)
{

}

const char* Warning::what() const noexcept
{
    return message_.c_str();
}

Warning::Type Warning::type() const noexcept
{
    return type_;
}

void Warning::print(std::ostream& os) const
{
    std::string type = typeToString();
    if (!type.empty())
        os << "[" << type << "] ";
    if (line_ >= 0)
        os << "Line " << line_ << ": ";
    os << message_ << std::endl;
}

std::string Warning::typeToString() const
{
    switch (type_)
    {
        case Type::Argument:        return "Argument";

        case Type::None: default:   return "";
    }
}

Warning Warning::GeneralWarning(const std::string& message)
{
    return Warning(Type::None, message, -1);
}

Warning Warning::ArgumentWarning(const std::string& message)
{
    return Warning(Type::Argument, message, -1);
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