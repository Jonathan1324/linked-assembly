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

    FAT12_File root;
    root.fs = fs;
    root.is_directory = 1;
    root.is_root_directory = 1;
    root.size = fs->root_size;
    root.first_cluster = 0;

    FAT_DirectoryEntry entry_test_txt = {0};
    memset(entry_test_txt.name, ' ', 8);
    memset(entry_test_txt.ext, ' ', 3);
    entry_test_txt.attribute = FAT_ENTRY_ARCHIVE;

    entry_test_txt.name[0] = 'T';
    entry_test_txt.name[1] = 'E';
    entry_test_txt.name[2] = 'S';
    entry_test_txt.name[3] = 'T';
    entry_test_txt.ext[0] = 'T';
    entry_test_txt.ext[1] = 'X';
    entry_test_txt.ext[2] = 'T';

    uint32_t rel_offset = FAT12_AddDirectoryEntry(&root, &entry_test_txt);
    
    FAT12_File test_txt;
    test_txt.fs = fs;
    test_txt.size = 0;
    test_txt.first_cluster = 0;
    test_txt.directory_entry_offset = FAT12_GetAbsoluteOffset(&root, rel_offset);
    test_txt.is_root_directory = 0;
    test_txt.is_directory = 0;

    uint8_t buffer_test_txt[16];
    memset(buffer_test_txt, 'A', sizeof(buffer_test_txt));
    FAT12_WriteToFileRaw(&test_txt, 0, buffer_test_txt, sizeof(buffer_test_txt));

    FAT_DirectoryEntry entry_folder = {0};
    memset(entry_folder.name, ' ', 8);
    memset(entry_folder.ext, ' ', 3);
    entry_folder.attribute = FAT_ENTRY_DIRECTORY;

    entry_folder.name[0] = 'F';
    entry_folder.name[1] = 'O';
    entry_folder.name[2] = 'L';
    entry_folder.name[3] = 'D';
    entry_folder.name[4] = 'E';
    entry_folder.name[5] = 'R';

    rel_offset = FAT12_AddDirectoryEntry(&root, &entry_folder);
    
    FAT12_File folder;
    folder.fs = fs;
    folder.size = 0;
    folder.first_cluster = 0;
    folder.directory_entry_offset = FAT12_GetAbsoluteOffset(&root, rel_offset);
    folder.is_root_directory = 0;
    folder.is_directory = 1;

    FAT_DirectoryEntry folder_dot = {0};
    memset(folder_dot.name, ' ', 8);
    memset(folder_dot.ext, ' ', 3);
    folder_dot.name[0] = '.';
    folder_dot.attribute = FAT_ENTRY_DIRECTORY;
    folder_dot.first_cluster = folder.first_cluster;
    FAT12_AddDirectoryEntry(&folder, &folder_dot);

    FAT_DirectoryEntry folder_dotdot = {0};
    memset(folder_dotdot.name, ' ', 8);
    memset(folder_dotdot.ext, ' ', 3);
    folder_dotdot.name[0] = '.';
    folder_dotdot.name[1] = '.';
    folder_dotdot.attribute = FAT_ENTRY_DIRECTORY;
    folder_dotdot.first_cluster = root.first_cluster;
    FAT12_AddDirectoryEntry(&folder, &folder_dotdot);

    FAT_DirectoryEntry entry_folder__text_txt = {0};
    memset(entry_folder__text_txt.name, ' ', 8);
    memset(entry_folder__text_txt.ext, ' ', 3);
    entry_folder__text_txt.attribute = FAT_ENTRY_ARCHIVE;

    entry_folder__text_txt.name[0] = 'T';
    entry_folder__text_txt.name[1] = 'E';
    entry_folder__text_txt.name[2] = 'X';
    entry_folder__text_txt.name[3] = 'T';
    entry_folder__text_txt.ext[0] = 'T';
    entry_folder__text_txt.ext[1] = 'X';
    entry_folder__text_txt.ext[2] = 'T';

    rel_offset = FAT12_AddDirectoryEntry(&folder, &entry_folder__text_txt);
    
    FAT12_File folder__text_txt;
    folder__text_txt.fs = fs;
    folder__text_txt.size = 0;
    folder__text_txt.first_cluster = 0;
    folder__text_txt.directory_entry_offset = FAT12_GetAbsoluteOffset(&folder, rel_offset);
    folder__text_txt.is_root_directory = 0;
    folder__text_txt.is_directory = 0;

    uint8_t buf[16];
    memset(buf, 'E', sizeof(buf));
    uint32_t t = FAT12_WriteToFileRaw(&folder__text_txt, 0, buf, sizeof(buf));
    printf("FAT12_WriteToFileRaw: %i\n", t);

    fclose(test);

    FAT12_CloseFilesystem(fs);
    fclose(f);
    return 0;
}
