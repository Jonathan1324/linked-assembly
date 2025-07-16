#pragma once

#include <Architecture.hpp>
#include <vector>
#include "../Context.hpp"
#include "../Parser/Parser.hpp"

class Encoder
{
public:
    Encoder(Context _context, Architecture _arch, BitMode _bits);
    virtual ~Encoder() = default;

protected:
    Context context;
    Architecture arch;
    BitMode bits;
};

Encoder* getEncoder(Context _context, Architecture _arch, BitMode _bits);