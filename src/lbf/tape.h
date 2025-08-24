#pragma once
#include <stdint.h>

#define INITIAL_SIZE_MIN 32
#define INITIAL_SIZE_MAX 32768
#define TAPE_WIDTH uint8_t

typedef struct
{
    TAPE_WIDTH* tape;
    uint64_t length;
    uint64_t pos;
} Tape;

Tape Tape_Create(uint64_t initialSize);
void Tape_Destroy(Tape* tape);
void Tape_Ensure(Tape* tape, uint64_t new_pos);
void Tape_Increase(Tape* tape, uint64_t count);
void Tape_Decrease(Tape* tape, uint64_t count);
void Tape_Right(Tape* tape, uint64_t count);
void Tape_Left(Tape* tape, uint64_t count);
TAPE_WIDTH Tape_Get(Tape* tape);
void Tape_Set(Tape* tape, TAPE_WIDTH val);