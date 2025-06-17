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

    Warning(Type type, const std::string& message, int line);

    const char* what() const noexcept;
    Type type() const noexcept;

    void print(std::ostream& os = std::cerr) const;


    static Warning GeneralWarning(const std::string& message);
    static Warning ArgumentWarning(const std::string& message);

private:
    Type type_;
    std::string message_;
    int line_;

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