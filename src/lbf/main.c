#include <stdio.h>
#include <string.h>
#include <version.h>
#include <stdlib.h>
#include "tape.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

void printHelp()
{
    puts("This is a brainfuck interpreter.");
    puts("Usage: lbf <file.bf>");
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

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror("File open");
        return 1;
    }

    size_t capacity = 1024;
    uint64_t rawProgramSize = 0;
    char *rawProgram = malloc(capacity);
    if (!rawProgram)
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
                if (rawProgramSize >= capacity)
                {
                    capacity *= 2;
                    char *tmp = realloc(rawProgram, capacity);
                    if (!tmp)
                    {
                        perror("realloc");
                        free(rawProgram);
                        fclose(f);
                        return 1;
                    }
                    rawProgram = tmp;
                }
                rawProgram[rawProgramSize++] = (char)c;
                break;

            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;

            default:
                /*
                fprintf(stderr, "Error: Unknown char '%c' at line %zu\n", c, line);
                free(rawProgram);
                fclose(f);
                return 1;
                */
                break;
        }
    }
    fclose(f);

    // Optimize
    uint64_t programSize = 0;
    for (uint64_t i = 0; i < rawProgramSize; i++)
    {
        char c = rawProgram[i];
        if (c == '[' || c == ']' || c == '.' || c == ',')
        {
            programSize++;
        }
        else if (c == '+' || c == '-' || c == '<' || c == '>')
        {
            programSize++;
            while (i + 1 < rawProgramSize && rawProgram[i + 1] == c)
                i++;
        }
    }

    char* program = malloc(programSize);
    uint64_t* meta = malloc(programSize * sizeof(uint64_t));

    uint64_t* stack = malloc(programSize * sizeof(uint64_t));
    uint64_t sp = 0;

    uint64_t maxEstimate = 0;
    uint64_t estimate = 0;

    uint64_t index = 0;
    for (uint64_t i = 0; i < rawProgramSize; i++)
    {
        char c = rawProgram[i];
        if (c == '[')
        {
            program[index] = '[';
            stack[sp++] = index;
        }
        else if (c == ']')
        {
            program[index] = ']';
            if (sp == 0)
            {
                fputs("Unmatched ']'", stderr);
                free(rawProgram);
                free(stack);
                free(meta);
                free(program);
                return 1;
            }
            uint64_t j = stack[--sp];
            meta[index] = j;
            meta[j] = index;
        }
        else if (c == '.' || c == ',')
        {
            program[index] = c;
        }
        else /* '<', '>', '+', '-' */
        {
            uint64_t count = 1;
            while (i + 1 < rawProgramSize && rawProgram[i + 1] == c)
            {
                count++;
                i++;
            }
            program[index] = c;
            meta[index] = count;

            if (c == '>')
            {
                estimate += count * (sp + 1);
                maxEstimate = MAX(estimate, maxEstimate);
            }
            else if (c == '<')
            {
                if (estimate < count * (sp + 1)) estimate = 0;
                else estimate -= count * (sp + 1);
            }
        }
        index++;
    }
    free(rawProgram);
    free(stack);
    if (sp != 0)
    {
        fputs("Unmatched '['", stderr);
        free(meta);
        free(program);
        return 1;
    }

    if (estimate < INITIAL_SIZE_MIN) estimate = INITIAL_SIZE_MIN;
    else if (estimate > INITIAL_SIZE_MAX) estimate = INITIAL_SIZE_MAX;
    else estimate = maxEstimate;

    Tape tape = Tape_Create(estimate);

    // Interpret
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
    free(program);
    free(meta);
    return 0;
}