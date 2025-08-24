#include "tape.h"

void Tape_Init(Tape* tape)
{
    for (int i = 0; i < TAPE_SIZE; i++) tape->tape[i] = 0;
    tape->pos = 0;
}

void Tape_Increase(Tape* tape)
{
    tape->tape[tape->pos]++;
}

void Tape_Decrease(Tape* tape)
{
    tape->tape[tape->pos]--;
}

void Tape_Right(Tape* tape)
{
    tape->pos++;
    if (tape->pos >= TAPE_SIZE) tape->pos -= TAPE_SIZE;
}

void Tape_Left(Tape* tape)
{
    if (tape->pos == 0) tape->pos = TAPE_SIZE - 1;
    else tape->pos--;
}

TAPE_WIDTH Tape_Get(Tape* tape)
{
    return tape->tape[tape->pos];
}

void Tape_Set(Tape* tape, TAPE_WIDTH val)
{
    tape->tape[tape->pos] = val;
}
