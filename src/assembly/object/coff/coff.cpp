#include "coff.hpp"

#include "../../util/buffer.hpp"

namespace COFF {
    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed, Context& context)
    {
        Data data;

        (void)bits;
        (void)arch;
        (void)endianness;
        (void)encoded;
        (void)parsed;
        (void)context;
        //TODO

        return data;
    }

    void write(std::ofstream& out, Endianness endianness, Data& data, Context& context)
    {
        (void)out;
        (void)endianness;
        (void)data;
        (void)context;
        //TODO
    }
}