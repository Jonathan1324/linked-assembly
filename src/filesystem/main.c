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

    FAT_DirectoryEntry entry_test_txt = {0};
    FAT_ParseName("test.txt", entry_test_txt.name, entry_test_txt.ext);
    entry_test_txt.attribute = FAT_ENTRY_ARCHIVE;
    FAT12_File* test_txt = FAT12_CreateEntry(fs->root, &entry_test_txt, 0);

    uint8_t buffer_test_txt[16];
    memset(buffer_test_txt, 'B', sizeof(buffer_test_txt));
    FAT12_WriteToFile(test_txt, 0, buffer_test_txt, sizeof(buffer_test_txt));


    FAT_DirectoryEntry entry_folder = {0};
    FAT_ParseName("folder", entry_folder.name, entry_folder.ext);
    entry_folder.attribute = FAT_ENTRY_DIRECTORY;
    FAT12_File* folder = FAT12_CreateEntry(fs->root, &entry_folder, 1);
    FAT12_AddDotsToDirectory(folder, fs->root);


    FAT_DirectoryEntry entry_folder__text_txt = {0};
    FAT_ParseName("text.txt", entry_folder__text_txt.name, entry_folder__text_txt.ext);
    entry_folder__text_txt.attribute = FAT_ENTRY_ARCHIVE;
    FAT12_File* folder__text_txt = FAT12_CreateEntry(folder, &entry_folder__text_txt, 0);

    uint8_t buf[16];
    memset(buf, 'E', sizeof(buf));
    FAT12_WriteToFile(folder__text_txt, 0, buf, sizeof(buf));


    FAT12_CloseEntry(test_txt);
    FAT12_CloseEntry(folder);
    FAT12_CloseEntry(folder__text_txt);

    fclose(test);

    FAT12_CloseFilesystem(fs);
    fclose(f);
    return 0;
}
