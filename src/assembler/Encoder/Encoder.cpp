#include "Encoder.hpp"

Encoder::Encoder(Context _context, Architecture _arch, BitMode _bits)
    : context(_context), arch(_arch), bits(_bits)
{

}

#include "x86/Encoder.hpp"

// FIXME: only temporary solution
Encoder* getEncoder(Context _context, Architecture _arch, BitMode _bits)
{
    return new x86::Encoder(_context, _arch, _bits);
}
