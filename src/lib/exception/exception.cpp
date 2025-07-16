#include "exception.hpp"

Exception::Exception(Type _type, const std::string& _message, int _line, int _column)
    : std::runtime_error(message), type(_type), message(_message), line(_line), column(_column)
{

}

const char* Exception::what() const noexcept
{
    return message.c_str();
}

Exception::Type Exception::getType() const noexcept
{
    return type;
}

void Exception::print(std::ostream& os) const
{
    std::string type = typeToString();
    if (!type.empty())
        os << "[" << type << "] ";
    if (line >= 0)
        os << "On line " << line;
    if (column >= 0)
        os << " in column " << column;
    os << ": " << message << std::endl;
}

std::string Exception::typeToString() const
{
    switch (type)
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

Exception Exception::ArgumentError(const std::string& message, int line, int column)
{
    return Exception(Type::ArgumentError, message, line, column);
}

Exception Exception::IOError(const std::string& message, int line, int column)
{
    return Exception(Type::IOError, message, line, column);
}

Exception Exception::ParseError(const std::string& message, int line, int column)
{
    return Exception(Type::ParseError, message, line, column);
}

Exception Exception::SyntaxError(const std::string& message, int line, int column)
{
    return Exception(Type::ParseError, message, line, column);
}

Exception Exception::SemanticError(const std::string& message, int line, int column)
{
    return Exception(Type::SemanticError, message, line, column);
}

Exception Exception::UndefinedSymbol(const std::string& message, int line, int column)
{
    return Exception(Type::UndefinedSymbol, message, line, column);
}

Exception Exception::OverflowError(const std::string& message, int line, int column)
{
    return Exception(Type::OverflowError, message, line, column);
}

Exception Exception::InternalError(const std::string& message, int line, int column)
{
    return Exception(Type::InternalError, message, line, column);
}