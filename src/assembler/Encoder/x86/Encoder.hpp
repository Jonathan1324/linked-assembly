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

        protected:
            std::vector<uint8_t> _EncodeInstruction(const Parser::Instruction::Instruction& instruction) override;
            uint64_t _GetSize(const Parser::Instruction::Instruction& instruction) override;
            std::vector<uint8_t> _EncodePadding(size_t length) override;
        };
    }
}
