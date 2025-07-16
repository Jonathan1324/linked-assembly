#pragma once

#include <Architecture.hpp>
#include <vector>
#include "../Context.hpp"
#include "../Parser/Parser.hpp"

class Encoder
{
public:
    Encoder(const Context& _context, Architecture _arch, BitMode _bits);
    virtual ~Encoder() = default;

    void Print();
protected:
    Context context;
    Architecture arch;
    BitMode bits;
};

Encoder* getEncoder(const Context& _context, Architecture _arch, BitMode _bits);