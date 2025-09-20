#include "interpreter.h"

#include <stdio.h>

void interpret(char* program, uint64_t* meta, uint64_t programSize)
{
    Tape tape = Tape_Create(INITIAL_SIZE_MIN);

    for (uint64_t i = 0; i < programSize; i++)
    {
        switch (program[i])
        {
            case '>': Tape_Right(&tape, meta[i]); break;
            case '<': Tape_Left(&tape, meta[i]); break;
            case '+': Tape_Increase(&tape, meta[i]); break;
            case '-': Tape_Decrease(&tape, meta[i]); break;
            case '.': fputc(Tape_Get(&tape), stdout); break;
            case ',': Tape_Set(&tape, (TAPE_WIDTH)getchar()); break;

            case '[': if (Tape_Get(&tape) == 0) i = meta[i]; break;
            case ']': if (Tape_Get(&tape) != 0) i = meta[i]; break;
        }
    }

    Tape_Destroy(&tape);
}
