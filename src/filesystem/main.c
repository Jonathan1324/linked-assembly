#include <stdio.h>

#include "fat/fat.h"

int main(int argc, const char *argv[])
{
    const char* output = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            fprintf(stderr, "Unknown long option: '%s'\n", argv[i]);
            return 1;
        }
        else if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'o':
                    if (i + 1 >= argc) {
                        fputs("No output file set after '-o'\n", stderr);
                        return 1;
                    }
                    output = argv[++i];
                    break;

                default:
                    fprintf(stderr, "Unknown argument: '-%c'\n", argv[i][1]);
                    return 1;
            }
        }
        else {
            // TODO
        }
    }

    if (!output) {
        fputs("Output file is required\n", stderr);
        return 1;
    }

    FILE* f = fopen(output, "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    FAT_WriteBootsector(f, "TEST", "MYDISK", 0x12345678);

    fclose(f);

    return 0;
}
