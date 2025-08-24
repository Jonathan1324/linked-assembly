#include <stdio.h>
#include <string.h>
#include <version.h>
#include <stdlib.h>
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

    size_t capacity = 1024;
    uint64_t programSize = 0;
    char *program = malloc(capacity);
    if (!program)
    {
        perror("malloc");
        fclose(f);
        return 1;
    }

    int c;
    int inComment = 0;
    uint64_t line = 1;
    while ((c = fgetc(f)) != EOF)
    {
        if (c == '\n') line++;

        if (inComment != 0)
        {
            if (c == '\n') inComment = 0;
            continue;
        }

        if (c == '#')
        {
            inComment = 1;
            continue;
        }

        switch (c)
        {
            case '>': case '<': case '+': case '-':
            case '.': case ',': case '[': case ']':
                if (programSize >= capacity)
                {
                    capacity *= 2;
                    char *tmp = realloc(program, capacity);
                    if (!tmp)
                    {
                        perror("realloc");
                        free(program);
                        fclose(f);
                        return 1;
                    }
                    program = tmp;
                }
                program[programSize++] = (char)c;
                break;

            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;

            default:
                /*
                fprintf(stderr, "Error: Unknown char '%c' at line %zu\n", c, line);
                free(program);
                fclose(f);
                return 1;
                */
                break;
        }
    }
    fclose(f);

    // Interpret
    for (uint64_t i = 0; i < programSize; i++)
    {
        switch (program[i])
        {
            case '>': Tape_Right(&tape); break;
            case '<': Tape_Left(&tape); break;
            case '+': Tape_Increase(&tape); break;
            case '-': Tape_Decrease(&tape); break;
            case '.': fputc(Tape_Get(&tape), stdout); break;
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
                            free(program);
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
                            free(program);
                            return 1;
                        }
                        i--;
                        if (program[i] == ']') loop++;
                        else if (program[i] == '[') loop--;
                    }
                }
                break;
        }
    }

    free(program);
    return 0;
}