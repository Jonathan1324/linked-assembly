#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat/fat.h"
#include "disk/disk.h"
#include "filesystem/filesystem.h"

typedef Filesystem_Action;
#define FILESYSTEM_FORMAT   ((Filesystem_Action)1)

void print_help(const char* name)
{
    fputs("Usage:\n", stderr);
    fprintf(stderr, "- %s create <image> (--type fat12/fat16/fat32)\n", name);
}

int main(int argc, const char *argv[])
{
    Filesystem_Action action;
    const char* image_file = NULL;

    if (strcmp(argv[1], "create") == 0) {
        action = FILESYSTEM_FORMAT;

        if (argc < 3) {
            print_help(argv[0]);
            return 1;
        }
        image_file = argv[2];
    } else {
        fputs("Unknown action\n", stderr);
        return 1;
    }

    if (!image_file) {
        fputs("Output file is required\n", stderr);
        return 1;
    }

    Filesystem_Type fs_type = FILESYSTEM_FAT32; // default

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--type") == 0) {
            i++;
            if (argc < i) {
                fputs("No type after '--type'\n", stderr);
                return 1;
            }
            if (
                (argv[i][0] == 'f' || argv[i][0] == 'F') &&
                (argv[i][1] == 'a' || argv[i][1] == 'A') &&
                (argv[i][2] == 't' || argv[i][2] == 'T')
            ) {
                //FAT
                if      (argv[i][3] == '1' && argv[i][4] == '2') fs_type = FILESYSTEM_FAT12;
                else if (argv[i][3] == '1' && argv[i][4] == '6') fs_type = FILESYSTEM_FAT16;
                else if (argv[i][3] == '3' && argv[i][4] == '2') fs_type = FILESYSTEM_FAT32;
                else fprintf("Unknown FS: %s\n", argv[i]);
            } else {
                fprintf(stderr, "Unknown FS: %s\n", argv[i]);
            }
        }
    }

    Fat_Version fat_version;

    switch (fs_type) {
        case FILESYSTEM_FAT12: fat_version = FAT_VERSION_12; break;
        case FILESYSTEM_FAT16: fat_version = FAT_VERSION_16; break;
        case FILESYSTEM_FAT32: fat_version = FAT_VERSION_32; break;
        default:
            fputs("Unknown FS: internal\n", stderr);
            return 1;
    }

    //FILE* f = fopen(image_file, "r+b");
    FILE* f = fopen(image_file, "w+b"); // truncate
    if (!f) {
        perror("fopen");
        return 1;
    }
    Disk* disk = Disk_CreateFromFile(f);
    if (!disk) {
        fclose(f);
        return 1;
    }

    uint64_t partition_size;
    switch (fs_type) {
        case FILESYSTEM_FAT12: partition_size = 1474560; break;   // 1.44 MB
        case FILESYSTEM_FAT16: partition_size = 16777216; break;  // 16 MB
        case FILESYSTEM_FAT32: partition_size = 104857600; break; // 100 MB
    }

    Partition* partition = Partition_Create(disk, 0, partition_size, 0);
    if (!partition) {
        Disk_Close(disk);
        return 1;
    }

    FAT_Filesystem* fat_fs = NULL;
    switch (action) {
        case FILESYSTEM_FORMAT:
        {
            // Format FS

            const char* oem_name = "lfs";
            const char* volume_name = "NO NAME";
            uint32_t volume_id = 0x12345678;

            uint32_t bytes_per_sector = 512;
            uint32_t sectors_per_cluster = fat_version == FAT_VERSION_16 ? 4 : 1;
            uint16_t reserved_sectors;
            switch (fat_version) {
                case FAT_VERSION_12: reserved_sectors = 1; break;
                case FAT_VERSION_16: reserved_sectors = 4; break;
                case FAT_VERSION_32: reserved_sectors = 32; break;
            }
            uint8_t number_of_fats = 2;
            uint16_t max_root_directory_entries;
            switch (fat_version) {
                case FAT_VERSION_12: max_root_directory_entries = 224; break;
                case FAT_VERSION_16: max_root_directory_entries = 512; break;
                case FAT_VERSION_32: max_root_directory_entries = 0; break;
            }
            uint16_t sectors_per_track = fat_version == FAT_VERSION_12 ? 18 : 32;
            uint16_t number_of_heads = fat_version == FAT_VERSION_32 ? 8 : 2;
            uint8_t drive_number = fat_version == FAT_VERSION_12 ? 0x00 : 0x80;
            uint8_t media_descriptor = fat_version == FAT_VERSION_12 ? FAT_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY144 : FAT_BOOTSECTOR_MEDIA_DESCRIPTOR_DISK;

            fat_fs = FAT_CreateEmptyFilesystem(partition, fat_version,
                                               oem_name, volume_name, volume_id,
                                               partition->size, bytes_per_sector,
                                               sectors_per_cluster, reserved_sectors,
                                               number_of_fats, max_root_directory_entries,
                                               sectors_per_track, number_of_heads,
                                               drive_number, media_descriptor);

            break;
        }

        default:
            fputs("Unknown action\n", stderr);
            Partition_Close(partition);
            Disk_Close(disk);
            return 1;
    }
    if (!fat_fs) {
        Partition_Close(partition);
        Disk_Close(disk);
        return 1;
    }

    Filesystem* fs = Filesystem_CreateFromFAT(fat_fs);
    if (!fs) {
        FAT_CloseFilesystem(fs);
        Partition_Close(partition);
        Disk_Close(disk);
        return 1;
    }

    //Filesystem_File* test_f = Filesystem_OpenPath(fs->root, "test_dir/test.txt", 1, 0, 0, 0, 0, 0, 0);
    //Filesystem_CloseEntry(test_f);

    Filesystem_Close(fs);
    Partition_Close(partition);
    Disk_Close(disk);

    return 0;
}
