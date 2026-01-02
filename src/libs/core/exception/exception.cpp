#include "exception.hpp"

#include <cstring>

Exception::Exception(Type _type, const std::string& _message, int _line, int _column, const char* _filename, size_t filenameLength)
    : std::runtime_error(_message), type(_type), message(_message), line(_line), column(_column)
{
    if (!filename || filenameLength == 0) return;
    filename = static_cast<char*>(std::malloc(filenameLength));
    if (!filename) return;
    std::memcpy(filename, _filename, filenameLength);
}

Exception::~Exception()
{
    std::free(filename);
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
    if (filename)
        os << " in file " << filename;
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

Exception Exception::ArgumentError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::ArgumentError, message, line, column, nullptr, filenameLength);
}

Exception Exception::IOError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::IOError, message, line, column, nullptr, filenameLength);
}

Exception Exception::ParseError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::ParseError, message, line, column, nullptr, filenameLength);
}

Exception Exception::SyntaxError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::SyntaxError, message, line, column, nullptr, filenameLength);
}

Exception Exception::SemanticError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::SemanticError, message, line, column, nullptr, filenameLength);
}

Exception Exception::UndefinedSymbol(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::UndefinedSymbol, message, line, column, nullptr, filenameLength);
}

Exception Exception::OverflowError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::OverflowError, message, line, column, nullptr, filenameLength);
}

Exception Exception::InternalError(const std::string& message, int line, int column, const char* filename)
{
    size_t filenameLength;
    if (filename) {
        filenameLength = std::strlen(filename) + 1;
    } else {
        filenameLength = 0;
    }
    return Exception(Type::InternalError, message, line, column, nullptr, filenameLength);
}