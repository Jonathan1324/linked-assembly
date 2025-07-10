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
        Bracket,
        EOL,
        _EOF
    };

    inline const char* to_string(Type type) {
            switch (type) {
                case Type::Token:   return "__Token__";
                case Type::String:  return "__String_";
                case Type::Comma:   return "__Comma__";
                case Type::Bracket: return "_Bracket_";
                case Type::EOL:     return "___EOL___";
                case Type::_EOF:    return "___EOF___";
                default:            return "__Unknown";
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
                case Type::Bracket:
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