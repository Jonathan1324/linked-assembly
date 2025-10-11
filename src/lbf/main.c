#include <stdio.h>
#include <string.h>
#include <version.h>
#include <stdlib.h>
#include "tape.h"
#include "interpreter.h"
#include "compiler.h"

void printHelp(const char* programName)
{
    puts("This is a brainfuck interpreter.");
    fprintf(stdout, "Usage: %s [-c] <input.bf> [-o output.bf]\n", programName);
}

int main(int argc, const char *argv[])
{
    int transpile = 0;
    const char* inputFile = NULL;
    const char* outputFile = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printVersion();
            return 0;
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            transpile = 1;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "Usage: %s [-c] <input.bf> [-o output.bf]\n", argv[0]);
                return 1;
            }
            outputFile = argv[++i];
        }
        else
        {
            inputFile = argv[i];
        }
    }

    if (!inputFile)
    {
        fprintf(stderr, "Usage: %s [-c] <input.bf> [-o output.bf]\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(inputFile, "r");
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

    if (transpile != 0)
    {
        FILE* out = fopen(outputFile, "w");
        if (!out)
        {
            perror("Output file open");
            free(meta);
            free(program);
            return 1;
        }
        
        compile(program, meta, programSize, out);

        fclose(out);
    }
    else
    {
        interpret(program, meta, programSize);
    }
    
    free(program);
    free(meta);
    return 0;
}