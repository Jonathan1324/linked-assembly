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

    Partition* partition = Partition_Create(f, 0, 1474560);
    if (!partition) {
        File_Close(f);
        return 1;
    }

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

    FAT_File* folder = FAT_CreateEntry(fs->root, "folder", FAT_ENTRY_DIRECTORY, 1, 0, 0, 0, 1);

    FAT_File* folder_test = FAT_CreateEntry(folder, "test.txt", FAT_ENTRY_ARCHIVE, 0, 0, 0, 0, 1);
    FAT_File* folder_test2 = FAT_CreateEntry(folder, "test2.txt", FAT_ENTRY_ARCHIVE, 0, 0, 0, 0, 1);

    FAT_File* test_find = FAT_FindEntry(fs->root, "folder");

    fclose(test);
    FAT_CloseEntry(test_txt);
    FAT_CloseEntry(folder);
    FAT_CloseEntry(folder_test);
    FAT_CloseEntry(folder_test2);
    FAT_CloseEntry(test_find);

    FAT_CloseFilesystem(fs);
    Partition_Close(partition);
    File_Close(f);

    /*
    FAT_DirectoryEntry entry_folder = {0};
    FAT_ParseName("folder", entry_folder.name, entry_folder.ext);
    entry_folder.attribute = FAT_ENTRY_DIRECTORY;
    FAT12_File* folder = FAT_CreateEntry(fs->root, &entry_folder, 1);
    FAT_AddDotsToDirectory(folder, fs->root);


    FAT_DirectoryEntry entry_folder__text_txt = {0};
    FAT_ParseName("text.txt", entry_folder__text_txt.name, entry_folder__text_txt.ext);
    entry_folder__text_txt.attribute = FAT_ENTRY_ARCHIVE;
    FAT12_File* folder__text_txt = FAT_CreateEntry(folder, &entry_folder__text_txt, 0);

    uint8_t buf[16];
    memset(buf, 'E', sizeof(buf));
    FAT_WriteToFile(folder__text_txt, 0, buf, sizeof(buf));

    FAT_CloseEntry(folder);
    FAT_CloseEntry(folder__text_txt);
    */

    return 0;
}
