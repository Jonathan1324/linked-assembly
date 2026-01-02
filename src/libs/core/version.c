#include "version.h"

#include <stdio.h>

void printVersion()
{
    printf("LCT version %s\n", VERSION); 
    printf("Compiled on %s\n", __DATE__);
    printf("License: BSD 3-Clause\n");
    fflush(stdout);
}
