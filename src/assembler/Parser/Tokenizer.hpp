#pragma once

#include "../Context.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <StringPool.hpp>
#include <cstdint>

namespace Token
{
    enum class Type
    {
        Token,
        String,
        Character,
        Operator,
        Comma,
        Punctuation,
        Bracket,
        EOL,
        _EOF,
        ExternLabel
    };

    inline const char* to_string(Type type)
    {
        switch (type)
        {
            case Type::Token:       return "__Token_______";
            case Type::String:      return "__String______";
            case Type::Character:   return "__Character___";
            case Type::Operator:    return "__Operator____";
            case Type::Comma:       return "__Comma_______";
            case Type::Punctuation: return "__Punctuation_";
            case Type::Bracket:     return "__Bracket_____";
            case Type::EOL:         return "_EOL__________";
            case Type::_EOF:        return "EOF___________";
            case Type::ExternLabel: return "__Extern_Label";
            default:                return "_______Unknown";
        }
    }

    struct Token
    {
        Type type;
        std::string value;
        size_t line;
        size_t column;
        uint64_t file;

        Token(Type t, std::string v, size_t l, size_t c, uint64_t f)
            : type(t), value(std::move(v)), line(l), column(c), file(f) {}

        std::string what(const Context* context) const;
    };

    class Tokenizer
    {
    public:
        Tokenizer(const Context& _context);

        void clear();
        void tokenize(std::istream* input);
        std::vector<Token> getTokens();
        void print();
    private:
        const Context* context;
        std::vector<Token> tokens;
    };
}
