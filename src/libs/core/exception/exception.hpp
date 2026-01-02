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

    Exception(Type _type, const std::string& _message, int _line, int _column,  const char* _filename, size_t filenameLength);
    ~Exception();

    const char* what() const noexcept override;
    Type getType() const noexcept;

    void print(std::ostream& os = std::cerr) const;


    static Exception ArgumentError(const std::string& message, int line, int column, const char* filename);
    static Exception IOError(const std::string& message, int line, int column, const char* filename = nullptr);
    static Exception ParseError(const std::string& message, int line, int column, const char* filename = nullptr);
    static Exception SyntaxError(const std::string& message, int line, int column, const char* filename = nullptr);
    static Exception SemanticError(const std::string& message, int line, int column, const char* filename = nullptr);
    static Exception UndefinedSymbol(const std::string& message, int line, int column, const char* filename = nullptr);
    static Exception OverflowError(const std::string& message, int line, int column, const char* filename = nullptr);
    static Exception InternalError(const std::string& message, int line, int column, const char* filename = nullptr);
    /*
    static Exception ArgumentError(const std::string& message, int line = -1, int column = -1);
    static Exception IOError(const std::string& message, int line = -1, int column = -1);
    static Exception ParseError(const std::string& message, int line = -1, int column = -1);
    static Exception SyntaxError(const std::string& message, int line = -1, int column = -1);
    static Exception SemanticError(const std::string& message, int line = -1, int column = -1);
    static Exception UndefinedSymbol(const std::string& message, int line = -1, int column = -1);
    static Exception OverflowError(const std::string& message, int line = -1, int column = -1);
    static Exception InternalError(const std::string& message, int line = -1, int column = -1);
    */

private:
    Type type;
    std::string message;
    int line;
    int column;
    char* filename = nullptr;

    std::string typeToString() const;
};