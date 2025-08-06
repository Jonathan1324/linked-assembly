#pragma once

#include "../Encoder/Encoder.hpp"

namespace Section
{
    void setAlignments(std::vector<Encoder::Section>& sections, Architecture arch, BitMode bits);
}