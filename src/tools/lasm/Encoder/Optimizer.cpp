#include "Encoder.hpp"
#include "../Section/Section.hpp"

void Encoder::Encoder::Optimize()
{
    ::Section::setAlignments(sections, arch, bits);
}