#pragma once

#include "../Encoder.hpp"

namespace Encoder
{
    namespace x86
    {
        class Encoder : public ::Encoder::Encoder
        {
        public:
            Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
            ~Encoder() = default;

            std::vector<uint8_t> EncodeInstruction() override;
        };
    }
}
