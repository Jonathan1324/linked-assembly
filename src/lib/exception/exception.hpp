#pragma once

#include <string>
#include <stdexcept>
#include <iostream>
#include <ostream>

class Exception : public std::runtime_error
{
public:
    enum class Type
    {
        ArgumentError,
        IOError,
        ParseError,
        SyntaxError,
        UndefinedSymbol,
        RedefinitionError,
        TypeError,
        OverflowError,
        SemanticError,

        InternalError
    };

    Exception(Type type, const std::string& message, int line);

    const char* what() const noexcept override;
    Type type() const noexcept;

    void print(std::ostream& os = std::cerr) const;


    static Exception ArgumentError(const std::string& message);
    static Exception IOError(const std::string& message);
    static Exception ParseError(const std::string& message, int line);
    static Exception SyntaxError(const std::string& message, int line);
    static Exception SemanticError(const std::string& message, int line);
    static Exception UndefinedSymbol(const std::string& message);
    static Exception OverflowError(const std::string& message, int line);
    static Exception InternalError(const std::string& message);

private:
    Type type_;
    std::string message_;
    int line_;

    std::string typeToString() const;
};