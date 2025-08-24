#pragma once
#include <stdint.h>

#define TAPE_SIZE 256
#define TAPE_WIDTH uint8_t

typedef struct
{
    TAPE_WIDTH tape[TAPE_SIZE];
    uint64_t pos;
} Tape;

void Tape_Init(Tape* tape);
void Tape_Increase(Tape* tape);
void Tape_Decrease(Tape* tape);
void Tape_Right(Tape* tape);
void Tape_Left(Tape* tape);
TAPE_WIDTH Tape_Get(Tape* tape);
void Tape_Set(Tape* tape, TAPE_WIDTH val);