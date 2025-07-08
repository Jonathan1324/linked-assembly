#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

namespace Token
{
    enum class Type
    {
        Token,
        Comma,
        EOL
    };

    inline const char* to_string(Type type) {
            switch (type) {
                case Type::Token: return "Token";
                case Type::Comma: return "Comma";
                case Type::EOL:   return "EOL";
                default:          return "Unknown";
            }
        }

    struct Token
    {
        Type type;
        std::string value;
        size_t line;
        size_t column;

        Token(Type t, std::string v, size_t l, size_t c)
            : type(t), value(std::move(v)), line(l), column(c) {}
            
        void print() const
        {
            std::cout << "Token (Type=" << to_string(type) << ") '" << value
                << "' in line " << line
                << " at column " << column;
        }
    };

    class Tokenizer
    {
    public:
        Tokenizer();

        std::vector<Token> tokenize(std::istream& input);
        void print(std::vector<Token> tokens, const char* indent);
    };
}