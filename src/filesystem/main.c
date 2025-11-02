#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat/fat.h"
#include "file/file.h"

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

    FILE* f_raw = fopen(image_file, "w+b");
    if (!f_raw) {
        perror("fopen");
        return 1;
    }
    File* f = File_Create(f_raw);
    if (!f) {
        fclose(f_raw);
        return 1;
    }

    Partition* partition = Partition_Create(f, 0, 104857600);
    if (!partition) {
        File_Close(f);
        return 1;
    }

    /*
    FAT_Filesystem* fs = FAT_CreateEmptyFilesystem(partition, FAT12,
                                                   "mkfs.fat",                                   // oem name
                                                   "NO NAME",                                    // volume label
                                                   0x12345678,                                   // volume id
                                                   partition->size,                              // total size in bytes
                                                   512,                                          // bytes per sector
                                                   1,                                            // sectors per cluster
                                                   1,                                            // reserved_sectors
                                                   2,                                            // number of fats
                                                   224,                                          // max root directory entries
                                                   18,                                           // sectors per track
                                                   2,                                            // number of heads
                                                   0x00,                                         // drive number
                                                   FAT_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY144     // media descriptor
    );
    */
    /*
    FAT_Filesystem* fs = FAT_CreateEmptyFilesystem(partition, FAT16,
                                                   "mkfs.fat",
                                                   "NO NAME",
                                                   0x12345678,
                                                   partition->size,
                                                   512,
                                                   4,
                                                   4,
                                                   2,
                                                   512,
                                                   32,
                                                   2,
                                                   0x80,
                                                   FAT_BOOTSECTOR_MEDIA_DESCRIPTOR_DISK
    );
    */
    FAT_Filesystem* fs = FAT_CreateEmptyFilesystem(partition, FAT32,
                                                   "mkfs.fat",
                                                   "NO NAME",
                                                   0x12345678,
                                                   partition->size,
                                                   512,
                                                   1,
                                                   32,
                                                   2,
                                                   0,
                                                   32,
                                                   8,
                                                   0x80,
                                                   FAT_BOOTSECTOR_MEDIA_DESCRIPTOR_DISK
    );
    if (!fs) {
        File_Close(f);
        Partition_Close(partition);
        return 1;
    }

    FILE* test = fopen("roots/test/test.txt", "rb");
    if (!test) {
        FAT_CloseFilesystem(fs);
        Partition_Close(partition);
        File_Close(f);
        return 1;
    }

    FAT_File* test_txt = FAT_CreateEntry(fs->root, "test.txt", FAT_ENTRY_ARCHIVE, 0, 0, 0, 0, 1);

    uint64_t offset = 0;
    uint8_t buffer[512];
    while (1) {
        size_t read = fread(&buffer, 1, sizeof(buffer), test);
        if (read <= 0) break;
        FAT_WriteToFile(test_txt, offset, buffer, read);
        offset += read;
    }

    fclose(test);
    FAT_CloseEntry(test_txt);

    FAT_File* test_dict = FAT_CreateEntry(fs->root, "test_dict", FAT_ENTRY_DIRECTORY, 1, 0, 0, 0, 1);
    FAT_File* test1 = FAT_CreateEntry(test_dict, "test.txt", FAT_ENTRY_ARCHIVE, 0, 0, 0, 0, 1);
    FAT_File* test2 = FAT_CreateEntry(test_dict, "test_2.txt", FAT_ENTRY_ARCHIVE, 0, 0, 0, 0, 1);

    FAT_CloseEntry(test_dict);
    FAT_CloseEntry(test1);
    FAT_CloseEntry(test2);

    FAT_CloseFilesystem(fs);
    Partition_Close(partition);
    File_Close(f);

    return 0;
}
