#include <stdio.h>
#include <stdlib.h>
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

    FILE* f = fopen(output, "w+b");
    if (!f) {
        perror("fopen");
        return 1;
    }

    FAT12_Filesystem* fs = FAT12_CreateEmptyFilesystem(f,
                                                       "mkfs.fat",                                   // oem name
                                                       "NO NAME",                                    // volume label
                                                       0xEB241F36,                                   // volume id
                                                       1474560,                                      // total size in bytes
                                                       512,                                          // bytes per sector
                                                       1,                                            // sectors per cluster
                                                       1,                                            // reserved_sectors
                                                       2,                                            // number of fats
                                                       224,                                          // max root directory entries
                                                       18,                                           // sectors per track
                                                       2,                                            // number of heads
                                                       0x00,                                         // drive number
                                                       FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY144   // media descriptor
    );
    if (!fs) {
        fclose(f);
        return 1;
    }

    free(fs);
    fclose(f);
    return 0;
}
