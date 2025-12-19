#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <version.h>

#include "jokes.h"

void printHelp(char* name)
{
    fputs("This tool tells you a joke!\n", stdout);
    fprintf(stdout, "Usage: %s\n", name);
}

int main(int argc, char* argv[])
{
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
    }

    srand(time(NULL));

    const unsigned int joke_index = rand() % joke_count;
    const char* joke = jokes[joke_index];

    puts(joke);

    return 0;
}
