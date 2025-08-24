#include <stdio.h>
#include <string.h>
#include <version.h>
#include "tape.h"

void printHelp()
{
    
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file.bf>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printHelp();
        return 0;
    }
    else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0)
    {
        printVersion();
        return 0;
    }

    Tape tape;
    Tape_Init(&tape);

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror("File open");
        return 1;
    }

    char program[100000];
    size_t programSize = fread(program, 1, sizeof(program), f);
    fclose(f);

    for (size_t i = 0; i < programSize; i++)
    {
        switch (program[i])
        {
            case '>': Tape_Right(&tape); break;
            case '<': Tape_Left(&tape); break;
            case '+': Tape_Increase(&tape); break;
            case '-': Tape_Decrease(&tape); break;
            case '.': putchar(Tape_Get(&tape)); break;
            case ',': Tape_Set(&tape, (TAPE_WIDTH)getchar()); break;

            case '[':
                if (Tape_Get(&tape) == 0)
                {
                    int loop = 1;
                    while (loop > 0)
                    {
                        i++;
                        if (i >= programSize)
                        {
                            fputs("Error: unmatched '['", stderr);
                            return 1;
                        }
                        if (program[i] == '[') loop++;
                        else if (program[i] == ']') loop--;
                    }
                }
                break;
            case ']':
                if (Tape_Get(&tape) != 0)
                {
                    int loop = 1;
                    while (loop > 0)
                    {
                        if (i == 0)
                        {
                            fputs("Error: unmatched ']'", stderr);
                            return 1;
                        }
                        i--;
                        if (program[i] == ']') loop++;
                        else if (program[i] == '[') loop--;
                    }
                }
                break;

            case '#':
                while (i < programSize && program[i] != '\n') i++;
                break;

            case '\n':
            case ' ':
                break;

            default:
                fprintf(stderr, "Error: Unknown char '%c'\n", program[i]);
                return 1;
        }
    }

    return 0;
}