#include "exception.hpp"

Exception::Exception(Type type, const std::string& message, int line)
    : std::runtime_error(message), type_(type), message_(message), line_(line)
{

}

const char* Exception::what() const noexcept
{
    return message_.c_str();
}

Exception::Type Exception::type() const noexcept
{
    return type_;
}

void Exception::print(std::ostream& os) const
{
    std::string type = typeToString();
    if (!type.empty())
        os << "[" << type << "] ";
    if (line_ >= 0)
        os << "Line " << line_ << ": ";
    os << message_ << std::endl;
}

std::string Exception::typeToString() const
{
    switch (type_)
    {
        case Type::ArgumentError:       return "ArgumentError";
        case Type::IOError:             return "IOError";
        case Type::ParseError:          return "ParseError";
        case Type::SyntaxError:         return "SyntaxError";
        case Type::SemanticError:       return "SemanticError";
        case Type::UndefinedSymbol:     return "UndefinedSymbol";
        case Type::OverflowError:       return "OverflowError";
        case Type::InternalError:       return "InternalError";

        default:                        return "";
    }
}

Exception Exception::ArgumentError(const std::string& message)
{
    return Exception(Type::ArgumentError, message, -1);
}

Exception Exception::IOError(const std::string& message)
{
    return Exception(Type::IOError, message, -1);
}

Exception Exception::ParseError(const std::string& message, int line)
{
    return Exception(Type::ParseError, message, line);
}

Exception Exception::SyntaxError(const std::string& message, int line)
{
    return Exception(Type::ParseError, message, line);
}

Exception Exception::SemanticError(const std::string& message, int line)
{
    return Exception(Type::SemanticError, message, line);
}

Exception Exception::UndefinedSymbol(const std::string& message)
{
    return Exception(Type::UndefinedSymbol, message, -1);
}

Exception Exception::OverflowError(const std::string& message, int line)
{
    return Exception(Type::OverflowError, message, line);
}

Exception Exception::InternalError(const std::string& message)
{
    return Exception(Type::InternalError, message, -1);
}