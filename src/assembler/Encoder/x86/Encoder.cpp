#include "Encoder.hpp"

#include "../evaluate.hpp"

x86::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser* _parser)
    : ::Encoder(_context, _arch, _bits, _parser)
{
    
}

std::vector<uint8_t> x86::Encoder::EncodeInstruction()
{
    // TODO
    return {};
}