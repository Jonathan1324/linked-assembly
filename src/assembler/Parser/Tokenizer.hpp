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
        String,
        Character,
        Comma,
        Punctuation,
        Macro,
        Bracket,
        EOL,
        _EOF
    };

    inline const char* to_string(Type type)
    {
        switch (type)
        {
            case Type::Token:       return "__Token_______";
            case Type::String:      return "__String______";
            case Type::Character:   return "__Character___";
            case Type::Comma:       return "__Comma_______";
            case Type::Macro:       return "__Macro_______";
            case Type::Punctuation: return "__Punctuation_";
            case Type::Bracket:     return "__Bracket_____";
            case Type::EOL:         return "_EOL__________";
            case Type::_EOF:        return "EOF___________";
            default:                return "_______Unknown";
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
            std::cout << "Token (Type=" << to_string(type) << ")";
            switch (type)
            {
                case Type::Comma:
                case Type::Macro:
                case Type::EOL:
                    std::cout << " in line " << line << " at column " << column;
                    break;
                case Type::_EOF:
                    std::cout << " at line " << line << std::endl;
                    break;

                case Type::Character:
                    std::cout << " 0x" << std::hex << static_cast<int>(static_cast<unsigned char>(value[0])) << std::dec << " ('" << value[0] << "') in line " << line << " at column " << column;
                    break;
                
                case Type::Token:
                case Type::String:
                case Type::Bracket:
                case Type::Punctuation:
                default:
                    std::cout << " '" << value << "' in line " << line << " at column " << column;
                    break;
            }
        }
    };

    class Tokenizer
    {
    public:
        Tokenizer();

        void clear();
        void tokenize(std::istream& input);
        std::vector<Token> getTokens();
        void print();
    private:
        std::vector<Token> tokens;
    };
}