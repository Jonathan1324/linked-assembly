#include "version.h"

#include <stdio.h>

void printVersion(const char* version)
{
    printf("linked-assembly version %s\n", version); 
    printf("Compiled on %s\n", __DATE__);
    printf("License: BSD 3-Clause\n");
    fflush(stdout);
}