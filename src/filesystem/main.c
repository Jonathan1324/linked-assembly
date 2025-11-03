#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fat/fat.h"
#include "disk/disk.h"
#include "filesystem/filesystem.h"
#include "version.h"

#include "native/directory.h"

typedef uint8_t FS_Action;
#define FS_NONE     0x0
#define FS_CREATE   0x1
#define FS_INSERT   0x2
#define FS_EXTRACT  0x4

void print_help(const char* name)
{
    fputs("Usage:\n", stderr);
    fprintf(stderr, "- %s create <image> (--type fat12/fat16/fat32) (--root <host path>)\n", name);
    fprintf(stderr, "- %s insert <host path> <image> (--path <image path>)\n", name);
    fprintf(stderr, "- %s extract <image path> <image> (--path <host path>)\n", name);
}

int main(int argc, const char *argv[])
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_help(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printVersion();
            return 0;
        }
    }

    int format;
    int truncate;

    FS_Action fs_actions = FS_NONE;

    const char* image_file = NULL;
    const char* root_file = NULL;

    const char* insert_file = NULL;

    const char* extract_file = NULL;

    int allow_path_flag = 0;
    int allow_root_flag = 0;

    const char* path = NULL;

    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "create") == 0) {
        format = 1;
        truncate = 1;
        fs_actions |= FS_CREATE;
        allow_root_flag = 1;

        if (argc < 3) {
            print_help(argv[0]);
            return 1;
        }
        image_file = argv[2];
    } else if (strcmp(argv[1], "insert") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_INSERT;

        if (argc < 4) {
            print_help(argv[0]);
            return 1;
        }
        image_file = argv[3];
        
        insert_file = argv[2];
        path = argv[2];
        allow_path_flag = 1;
    } else if (strcmp(argv[1], "extract") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_EXTRACT;

        if (argc < 4) {
            print_help(argv[0]);
            return 1;
        }
        image_file = argv[3];
        
        extract_file = argv[2];
        path = argv[2];
        allow_path_flag = 1;
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
        if (format && strcmp(argv[i], "--type") == 0) {
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
        } else if (allow_path_flag && strcmp(argv[i], "--path") == 0) {
            i++;
            if (argc < i) {
                fputs("No path after '--path'\n", stderr);
                return 1;
            }
            path = argv[i];
        } else if (allow_root_flag && strcmp(argv[i], "--root") == 0) {
            i++;
            if (argc < i) {
                fputs("No path after '--root'\n", stderr);
                return 1;
            }
            root_file = argv[i];
        }
    }

    //FILE* f = fopen(image_file, "r+b");
    FILE* f = NULL;
    if (truncate) f = fopen(image_file, "w+b");
    else          f = fopen(image_file, "r+b");
    if (!f) {
        perror("fopen");
        return 1;
    }
    Disk* disk = Disk_CreateFromFile(f);
    if (!disk) {
        fclose(f);
        return 1;
    }

    if (!format) {
        // Currently only checking for FAT
        FAT_Bootsector bootsector;
        if (Disk_Read(disk, &bootsector, 0, sizeof(FAT_Bootsector)) != sizeof(FAT_Bootsector)) {
            Disk_Close(disk);
            fputs("Couldn't read bootsector\n", stderr);
            return 1;
        }

        if (memcmp(bootsector.fat12_fat16.header.filesystem_type, "FAT12", 5) == 0) {
            fs_type = FILESYSTEM_FAT12;
        } else if (memcmp(bootsector.fat12_fat16.header.filesystem_type, "FAT16", 5) == 0) {
            fs_type = FILESYSTEM_FAT16;
        } else if (memcmp(bootsector.fat32.header.filesystem_type, "FAT32", 5) == 0) {
            fs_type = FILESYSTEM_FAT32;
        }
    }

    uint64_t partition_size;
    if (format) {
        switch (fs_type) {
            case FILESYSTEM_FAT12: partition_size = 1474560; break;   // 1.44 MB
            case FILESYSTEM_FAT16: partition_size = 16777216; break;  // 16 MB
            case FILESYSTEM_FAT32: partition_size = 104857600; break; // 100 MB
        }
    } else {
        partition_size = disk->size;
    }

    Partition* partition = Partition_Create(disk, 0, partition_size, 0);
    if (!partition) {
        Disk_Close(disk);
        return 1;
    }

    Fat_Version fat_version;
    switch (fs_type) {
        case FILESYSTEM_FAT12: fat_version = FAT_VERSION_12; break;
        case FILESYSTEM_FAT16: fat_version = FAT_VERSION_16; break;
        case FILESYSTEM_FAT32: fat_version = FAT_VERSION_32; break;
        default:
            Partition_Close(partition);
            Disk_Close(disk);
            fputs("Unknown FS: internal\n", stderr);
            return 1;
    }

    FAT_Filesystem* fat_fs = NULL;
    if (format) {
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
    } else {
        fat_fs = FAT_OpenFilesystem(partition, fat_version, 0);
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

    if (allow_root_flag && root_file) {
        int result = Filesystem_SyncPathsToFS(fs->root, "/", root_file);
        if (result != 0) {
            printf("Warning: Couldn't sync '%s' to the root\n", root_file);
        }
    }

    if (fs_actions & FS_INSERT) {
        int result = Filesystem_SyncPathsToFS(fs->root, path, insert_file);
        if (result != 0) {
            printf("Warning: Couldn't sync '%s' to '%s'\n", insert_file, path);
        }
    }

    if (fs_actions & FS_EXTRACT) {
        int result = Filesystem_SyncPathsFromFS(fs->root, extract_file, path);
        if (result != 0) {
            printf("Warning: Couldn't sync '%s' to '%s'\n", path, extract_file);
        }
    }

    Filesystem_Close(fs);
    Partition_Close(partition);
    Disk_Close(disk);

    return 0;
}
