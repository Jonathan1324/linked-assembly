#pragma once

#include "parser.hpp"
#include "encoder/encoder.hpp"

void printParsed(Parsed parsed);
void printEncoded(const Encoded& encoded, int indent = 0);