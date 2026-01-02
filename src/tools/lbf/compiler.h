#pragma once

#include "tape.h"
#include <stdio.h>

void compile(char* program, uint64_t* meta, uint64_t programSize, FILE* out);
