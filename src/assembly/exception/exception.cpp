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
        case Type::ArgumentError:  return "ArgumentError";
        case Type::IOError:        return "IOError";

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