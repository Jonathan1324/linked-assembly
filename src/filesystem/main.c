#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat/fat.h"
#include <time.h>

int main(int argc, const char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s create <filename>\n", argv[0]);
        return 1;
    }


    if (strcmp(argv[1], "create") != 0) {
        fprintf(stderr, "Only create is implemented yet\n");
        return 1;
    }

    const char* image_file = argv[2];

    if (!image_file) {
        fputs("Output file is required\n", stderr);
        return 1;
    }

    FILE* f = fopen(image_file, "w+b");
    if (!f) {
        perror("fopen");
        return 1;
    }

    FAT12_Filesystem* fs = FAT12_CreateEmptyFilesystem(f,
                                                       "mkfs.fat",                                   // oem name
                                                       "NO NAME",                                    // volume label
                                                       0x12345678,                                   // volume id
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

    int64_t now = time(NULL);

    FILE* test = fopen("test.py", "rb");
    if (!test) {
        FAT12_CloseFilesystem(fs);
        fclose(f);
        return 1;
    }
    fseek(test, 0, SEEK_END);
    long length = ftell(test);
    FAT12_CreateFileFromStream(fs, test, "test.txt",
                               now, now, now,
                               length);
    fclose(test);

    FAT12_CloseFilesystem(fs);
    fclose(f);
    return 0;
}
