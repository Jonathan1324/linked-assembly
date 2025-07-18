#include "Encoder.hpp"

Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser* _parser)
    : context(_context), arch(_arch), bits(_bits), parser(_parser)
{

}

void Encoder::Print()
{
    
}

#include "x86/Encoder.hpp"

// FIXME: only temporary solution
Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser* parser)
{
    return new x86::Encoder(context, arch, bits, parser);
}
