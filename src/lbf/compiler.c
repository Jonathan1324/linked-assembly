#include "compiler.h"

void compile(char* program, uint64_t* meta, uint64_t programSize, FILE* out)
{
    for (uint64_t i = 0; i < programSize; i++)
    {
        char op = program[i];
        if (op == '+' || op == '-' || op == '<' || op == '>')
        {
            for (uint64_t j = 0; j < meta[i]; j++)
            {
                fputc(op, out);
            }
        }
        else
        {
            fputc(op, out);
        }
    }
}
