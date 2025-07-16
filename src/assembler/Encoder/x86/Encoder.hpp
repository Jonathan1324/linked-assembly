#pragma once

#include "../Encoder.hpp"

namespace x86
{
    class Encoder : public ::Encoder
    {
    public:
        Encoder(Context _context, Architecture _arch, BitMode _bits);
        ~Encoder() = default;
    };
}