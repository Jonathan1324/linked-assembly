#pragma once

#include "../Encoder.hpp"

namespace x86
{
    class Encoder : public ::Encoder
    {
    public:
        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser* _parser);
        ~Encoder() = default;

        void Encode() override;
    };
}
