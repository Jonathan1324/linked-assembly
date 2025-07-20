#pragma once

#include <Architecture.hpp>
#include <vector>
#include "../Context.hpp"
#include "../Parser/Parser.hpp"

class Encoder
{
public:
    Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser* _parser);
    virtual ~Encoder() = default;

    virtual void Encode() = 0;
    void Print();
    
protected:


    Context context;
    Architecture arch;
    BitMode bits;

    const Parser* parser = nullptr;

    std::unordered_map<std::string, std::string> labelSection;
    std::unordered_map<std::string, size_t> labelOffset;
};

Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser* parser);
