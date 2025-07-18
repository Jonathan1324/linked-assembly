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

    void Print();
protected:
    Context context;
    Architecture arch;
    BitMode bits;

    const Parser* parser = nullptr;
};

Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser* parser);
