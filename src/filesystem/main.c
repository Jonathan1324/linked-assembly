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

    FILE* test = fopen("roots/test/test.txt", "rb");
    if (!test) {
        FAT12_CloseFilesystem(fs);
        fclose(f);
        return 1;
    }
    fseek(test, 0, SEEK_END);
    long length = ftell(test);
    fseek(test, 0, SEEK_SET);
    FAT12_CreateFileFromStream(fs, test, "test.txt",
                               now, now, now,
                               length);
    fclose(test);

    FAT12_File fat12_f;
    fat12_f.fs = fs;
    fat12_f.is_root_directory = 0;
    fat12_f.size = length;
    fat12_f.first_cluster = 2;
    fat12_f.directory_entry_offset = fs->root_offset;

    uint8_t buffer[600] = {0};
    uint32_t read = FAT12_ReadFromFileRaw(&fat12_f, 0, &buffer, 600);
    printf("Read: %u\n", read);
    for (int i = 0; i < 600; i++) {
        printf("%x ", buffer[i]);
    }
    fputc('\n', stdout);

    FAT12_File root_dir;
    root_dir.fs = fs;
    root_dir.is_root_directory = 1;

    uint8_t buffer2[16];
    for (int i = 0; i < 16; i++) {
        buffer2[i] = (uint8_t)i;
    }
    uint32_t written = FAT12_WriteToFileRaw(&root_dir, 32, &buffer2, 16);
    printf("Written: %u\n", written);

    FAT12_CloseFilesystem(fs);
    fclose(f);
    return 0;
}
