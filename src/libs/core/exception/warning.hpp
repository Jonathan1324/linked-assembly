#pragma once

#include <string>
#include <iostream>
#include <ostream>
#include <vector>

class Warning
{
public:
    enum class Type
    {
        None,
        Argument
    };

    Warning(Type _type, const std::string& _message, int _line, int _column);

    const char* what() const noexcept;
    Type getType() const noexcept;

    void print(std::ostream& os = std::cerr) const;


    static Warning GeneralWarning(const std::string& message, int line = -1, int column = -1);
    static Warning ArgumentWarning(const std::string& message, int line = -1, int column = -1);

private:
    Type type;
    std::string message;
    int line;
    int column;

    std::string typeToString() const;
};

class WarningManager
{
public:
    WarningManager();

    void add(const Warning& w);
    void printAll(std::ostream& os = std::cerr) const;
    bool hasWarnings() const;
    void clear();

private:
    std::vector<Warning> warnings_;
};
