#include "Encoder.hpp"

#include "../evaluate.hpp"

Encoder::x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : ::Encoder::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> Encoder::x86::Encoder::EncodeInstruction()
{
    // TODO
    return {};
}