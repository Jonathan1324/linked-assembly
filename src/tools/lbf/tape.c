#include "tape.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Tape Tape_Create(uint64_t initialSize)
{
    Tape tape;
    tape.tape = calloc(initialSize, sizeof(TAPE_WIDTH));
    tape.length = initialSize;
    tape.pos = 0;
    return tape;
}

void Tape_Destroy(Tape* tape)
{
    if (tape->tape)
    {
        free(tape->tape);
        tape->tape = NULL;
    }
}

void Tape_Ensure(Tape* tape, uint64_t new_pos)
{
    if (new_pos >= tape->length)
    {
        uint64_t new_length = tape->length * 2;
        while (new_length <= new_pos) new_length *= 2;

        tape->tape = realloc(tape->tape, new_length * sizeof(TAPE_WIDTH));
        if (!tape->tape) { /* TODO */ }
        memset(tape->tape + tape->length, 0, (new_length - tape->length) * sizeof(TAPE_WIDTH));
        tape->length = new_length;
    }
}

void Tape_Increase(Tape* tape, uint64_t count)
{
    tape->tape[tape->pos] += count;
}

void Tape_Decrease(Tape* tape, uint64_t count)
{
    tape->tape[tape->pos] -= count;
}

void Tape_Right(Tape* tape, uint64_t count)
{
    tape->pos += count;
    Tape_Ensure(tape, tape->pos);
}

void Tape_Left(Tape* tape, uint64_t count)
{
    if (count > tape->pos)
    {
        // TODO: Error, can't go below 0
        tape->pos = 0;
    }
    else
        tape->pos -= count;
}

TAPE_WIDTH Tape_Get(Tape* tape)
{
    return tape->tape[tape->pos];
}

void Tape_Set(Tape* tape, TAPE_WIDTH val)
{
    tape->tape[tape->pos] = val;
}
