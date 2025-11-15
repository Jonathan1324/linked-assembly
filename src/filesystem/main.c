#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fat/fat.h"
#include "disk/disk.h"
#include "filesystem/filesystem.h"
#include "mbr/mbr.h"
#include <version.h>
#include "args.h"

#include "native/directory.h"

void print_help(const char* name, FILE* s)
{
    fputs("Usage:\n", s);
    fprintf(s, "> %s create <image> fat12|fat16|fat32 [--size B/K/M/G/T] [--boot <file>] [--root <path>] [flags]\n", name);
    fprintf(s, "> %s format <image> fat12|fat16|fat32 [--boot <file>] [--root <path>] [flags]\n", name);
    fprintf(s, "> %s insert <image> <host path> [--path <image path>] [flags]\n", name);
    fprintf(s, "> %s extract <image> <image path> [--path <host path>] [flags]\n", name);
    fprintf(s, "> %s remove <image> <image path> [flags]\n", name);
    fprintf(s, "> %s info <image> [flags]\n", name);
    fprintf(s, "> %s list <image> <image path> [flags]\n", name);
    fputc('\n', s);
    fputs("Commands:\n", s);
    fputs("> create          Create a new image, format it, optionally set boot sector and root\n", s);
    fputs("> format          Format an existing image, optionally set boot sector and root\n", s);
    fputs("> insert          Insert a file or directory from the host into the image\n", s);
    fputs("> extract         Extract a file or directory from the image into the host\n", s);
    fputs("> remove          Remove a file or directory from the image\n", s);
    fputs("> info            Print information about the FS of the image\n", s);
    fputs("> list            List files at a specific path in the image\n", s);
    fputc('\n', s);
    fputs("Flags:\n", s);
    fputs("> --no-lfn                   Disable long file names for FAT\n", s);
    fputs("> --read-only                Open image in read-only mode\n", s);
    fputs("> --force-bootsector         Force writing bootsector without overriding header and signature\n", s);
    fputs("> --fast                     Skip filling the data area with zeros\n", s);
    fputs("> --safe                     Prevent recursive deletion of directories\n", s);
    fputs("> --count-clusters           Count free clusters on FAT12 and FAT16 when using 'info'\n", s);
    fputc('\n', s);
    fputs("Supported Filesystems:\n", s);
    fputs("> FAT12\n", s);
    fputs("> FAT16\n", s);
    fputs("> FAT32\n", s);
}

typedef unsigned char Subcommand;
#define COMMAND_NONE        ((Subcommand)0)
#define COMMAND_HELP        ((Subcommand)1)
#define COMMAND_VERSION     ((Subcommand)2)
#define COMMAND_CREATE      ((Subcommand)3)
#define COMMAND_FORMAT      ((Subcommand)4)
#define COMMAND_INSERT      ((Subcommand)5)
#define COMMAND_EXTRACT     ((Subcommand)6)
#define COMMAND_REMOVE      ((Subcommand)7)
#define COMMAND_INFO        ((Subcommand)8)
#define COMMAND_LIST        ((Subcommand)9)

int main(int argc, const char* argv[])
{
    const char* file1 = "test.img";
    const char* file2 = "test2.img";
    int swap = 0;

    FILE* tmp_image_file_1 = fopen(swap ? file2 : file1, "w+b");
    if (!tmp_image_file_1) {
        fputs("Couldn't open the image\n", stderr);
        return 1;
    }
    Disk* tmp_disk1 = Disk_CreateFromFile(tmp_image_file_1, 1073741824);
    if (!tmp_disk1) {
        fputs("Couldn't open disk\n", stderr);
        fclose(tmp_image_file_1);
        return 1;
    }

    MBR_Disk* mbr1 = MBR_CreateDisk(tmp_disk1);
    if (!mbr1) {
        fputs("Couldn't create mbr\n", stderr);
        Disk_Close(tmp_disk1);
        return 1;
    }

    FILE* tmp_image_file_2 = fopen(swap ? file1 : file2, "r+b");
    if (!tmp_image_file_2) {
        fputs("Couldn't open the image2\n", stderr);
        return 1;
    }
    Disk* tmp_disk2 = Disk_CreateFromFile(tmp_image_file_2, 1073741824);
    if (!tmp_disk2) {
        fputs("Couldn't open disk2\n", stderr);
        fclose(tmp_image_file_2);
        return 1;
    }

    MBR_Disk* mbr2 = MBR_OpenDisk(tmp_disk2);
    if (!mbr2) {
        fputs("Couldn't open mbr2\n", stderr);
        Disk_Close(tmp_disk2);
        return 1;
    }

    MBR_SetPartitionRaw(mbr1, 0, 2048 * 512, 1024 * 512, MBR_TYPE_FAT32_LBA, 1);

    MBR_CloseDisk(mbr1);
    MBR_CloseDisk(mbr2);
    return 0;


    if (argc < 2) {
        print_help(argv[0], stderr);
        return 1;
    }

    Subcommand command = COMMAND_NONE;

    if      (ARG_IS_HELP(1))        command = COMMAND_HELP;
    else if (ARG_IS_VERSION(1))     command = COMMAND_VERSION;
    else if (ARG_CMP(1, "create"))  command = COMMAND_CREATE;
    else if (ARG_CMP(1, "format"))  command = COMMAND_FORMAT;
    else if (ARG_CMP(1, "insert"))  command = COMMAND_INSERT;
    else if (ARG_CMP(1, "extract")) command = COMMAND_EXTRACT;
    else if (ARG_CMP(1, "remove"))  command = COMMAND_REMOVE;
    else if (ARG_CMP(1, "info"))    command = COMMAND_INFO;
    else if (ARG_CMP(1, "list"))    command = COMMAND_LIST;
    else {
        fprintf(stderr, "Unknown command '%s'\n", argv[1]);
        return 1;
    }

    const char* image_str = argv[2];
    char mode_str[4] = {0, '+', 'b', 0};

    switch (command) {
        case COMMAND_CREATE:
            mode_str[0] = 'w';
            break;

        case COMMAND_FORMAT: case COMMAND_INSERT: 
        case COMMAND_EXTRACT: case COMMAND_REMOVE:
        case COMMAND_LIST: case COMMAND_INFO:
            mode_str[0] = 'r';
            break;

        case COMMAND_HELP: case COMMAND_VERSION:
        default:
            image_str = NULL;
            mode_str[0] = 0;
            break;
    }

    if (mode_str[0]) {
        if (argc < 3) {
            print_help(argv[0], stderr);
            return 1;
        }
    }

    int arg_start = argc;
    switch (command)
    {
        case COMMAND_HELP:    break; // TODO
        case COMMAND_VERSION: break; // TODO
        case COMMAND_CREATE:  arg_start = 4; break;
        case COMMAND_FORMAT:  arg_start = 4; break;
        case COMMAND_INSERT:  arg_start = 4; break;
        case COMMAND_EXTRACT: arg_start = 4; break;
        case COMMAND_REMOVE:  arg_start = 4; break;
        case COMMAND_INFO:    arg_start = 3; break;
        case COMMAND_LIST:    arg_start = 4; break;
    }

    Arguments args = {0};
    if (ParseArguments(argc, argv, arg_start, &args) != 0) {
        fputs("Couldn't parse arguments\n", stderr);
        return 1;
    }

    // Commands that don't use the image
    switch (command)
    {
        case COMMAND_HELP: {
            print_help(argv[0], stdout);
            return 0;
        }
        case COMMAND_VERSION: {
            printVersion();
            return 0;
        }
    }

    uint64_t image_size = 0;
    if (mode_str[0] == 'w') image_size = args.size;
    else image_size = Path_GetSize(image_str);

    FILE* image_file = fopen(image_str, mode_str);
    if (!image_file) {
        fputs("Couldn't open the image\n", stderr);
        return 1;
    }
    Disk* disk = Disk_CreateFromFile(image_file, image_size);
    if (!disk) {
        fputs("Couldn't open disk\n", stderr);
        fclose(image_file);
        return 1;
    }

    if (args.flag_fast) {
        uint8_t b = 0;
        fseek(image_file, image_size - 1, SEEK_SET);
        fread(&b, 1, 1, image_file);
        fseek(image_file, image_size - 1, SEEK_SET);
        fwrite(&b, 1, 1, image_file);
        fseek(image_file, 0, SEEK_SET);
    }

    Partition* partition = Partition_Create(disk, 0, disk->size, args.flag_read_only);
    if (!partition) {
        fputs("Couldn't open partition\n", stderr);
        Disk_Close(disk);
        return 1;
    }

    Filesystem_Type fs_type;
    if (command != COMMAND_CREATE && command != COMMAND_FORMAT) {
        FAT_Bootsector bootsector;
        if (Partition_Read(partition, &bootsector, 0, sizeof(FAT_Bootsector)) != sizeof(FAT_Bootsector)) {
            fputs("Couldn't read bootsector of file\n", stderr);
            Partition_Close(partition);
            Disk_Close(disk);
            return 1;
        }

        if (memcmp(bootsector.fat12_fat16.header.filesystem_type, "FAT12", 5) == 0) {
            fs_type = FILESYSTEM_FAT12;
        } else if (memcmp(bootsector.fat12_fat16.header.filesystem_type, "FAT16", 5) == 0) {
            fs_type = FILESYSTEM_FAT16;
        } else if (memcmp(bootsector.fat32.header.filesystem_type, "FAT32", 5) == 0) {
            fs_type = FILESYSTEM_FAT32;
        }
    } else {
        if (argc < 4) {
            print_help(argv[0], stderr);
            Partition_Close(partition);
            Disk_Close(disk);
            return 1;
        }
        const char* fs_name = argv[3];
        if (
            (fs_name[0] == 'f' || fs_name[0] == 'F') &&
            (fs_name[1] == 'a' || fs_name[1] == 'A') &&
            (fs_name[2] == 't' || fs_name[2] == 'T')
        ) {
            //FAT
            if      (fs_name[3] == '1' && fs_name[4] == '2') fs_type = FILESYSTEM_FAT12;
            else if (fs_name[3] == '1' && fs_name[4] == '6') fs_type = FILESYSTEM_FAT16;
            else if (fs_name[3] == '3' && fs_name[4] == '2') fs_type = FILESYSTEM_FAT32;
            else fprintf(stderr, "Unknown FS: %s\n", fs_name);
        } else {
            fprintf(stderr, "Unknown FS: %s\n", fs_name);
        }
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

    uint8_t bootsector_buffer[512];
    if (args.boot_file) {
        FILE* bootcode_f = fopen(args.boot_file, "rb");
        if (!bootcode_f) {
            Partition_Close(partition);
            Disk_Close(disk);
            fprintf(stderr, "Couldn't open '%s'\n", args.boot_file);
            return 1;
        }
        if (fread(bootsector_buffer, sizeof(bootsector_buffer), 1, bootcode_f) != 1) {
            Partition_Close(partition);
            Disk_Close(disk);
            fprintf(stderr, "Couldn't read 512 bytes of '%s'\n", args.boot_file);
            return 1;
        }
        fclose(bootcode_f);
    }

    FAT_Filesystem* fat_fs = NULL;
    if (command == COMMAND_CREATE || command == COMMAND_FORMAT) {
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

        // TODO
        fat_fs = FAT_CreateEmptyFilesystem(partition, fat_version, args.flag_fast, (args.boot_file ? bootsector_buffer : NULL), args.flag_force_bootsector,
                                           oem_name, volume_name, volume_id,
                                           partition->size, bytes_per_sector,
                                           sectors_per_cluster, reserved_sectors,
                                           number_of_fats, max_root_directory_entries,
                                           sectors_per_track, number_of_heads,
                                           drive_number, media_descriptor);
    } else {
        fat_fs = FAT_OpenFilesystem(partition, fat_version, args.flag_read_only);
    }

    if (!fat_fs) {
        fputs("Couldn't open FAT FS\n", stderr);
        Partition_Close(partition);
        Disk_Close(disk);
        return 1;
    }

    int fat_use_lfn = !args.flag_no_lfn;
    Filesystem* fs = Filesystem_CreateFromFAT(fat_fs, fat_use_lfn);
    if (!fs) {
        fputs("Couldn't open FS\n", stderr);
        FAT_CloseFilesystem(fat_fs);
        Partition_Close(partition);
        Disk_Close(disk);
        return 1;
    }

    int result = 0;

    switch (command)
    {
        case COMMAND_CREATE: case COMMAND_FORMAT: {
            if (args.root_path) {
                if (Filesystem_SyncPathsToFS(fs->root, "/", args.root_path) != 0) {
                    fprintf(stderr, "Couldn't sync '%s' to the root\n", args.root_path);
                    result = 1;
                }
            }
            break;
        }
        case COMMAND_INSERT: {
            if (argc < 4) {
                print_help(argv[0], stderr);
                Partition_Close(partition);
                Disk_Close(disk);
                return 1;
            }
            const char* host_path = argv[3];
            const char* image_path = args.path ? args.path : argv[3];
            if (Filesystem_SyncPathsToFS(fs->root, image_path, host_path) != 0) {
                fprintf(stderr, "Couldn't sync '%s' to '%s'\n", host_path, image_path);
                result = 1;
            }
            break;
        }
        case COMMAND_EXTRACT: {
            if (argc < 4) {
                print_help(argv[0], stderr);
                Partition_Close(partition);
                Disk_Close(disk);
                return 1;
            }
            const char* host_path = args.path ? args.path : argv[3];
            const char* image_path = argv[3];
            if (Filesystem_SyncPathsFromFS(fs->root, image_path, host_path) != 0) {
                fprintf(stderr, "Couldn't sync '%s' to '%s'\n", image_path, host_path);
                result = 1;
            }
            break;
        }
        case COMMAND_REMOVE: {
            if (argc < 4) {
                print_help(argv[0], stderr);
                Partition_Close(partition);
                Disk_Close(disk);
                return 1;
            }
            const char* image_path = argv[3];
            int result = Filesystem_DeletePath(fs->root, image_path, args.flag_safe);
            if (result == 2) {
                fprintf(stderr, "Directory '%s' isn't empty\n", image_path);
                result = 1;
            } else if (result != 0) {
                fprintf(stderr, "Couldn't remove '%s'\n", image_path);
                result = 1;
            }
            break;
        }
        case COMMAND_INFO: {
            FAT_DumpInfo(fat_fs, stdout, args.flag_count_clusters);
            break;
        }
        case COMMAND_LIST: {
            if (argc < 4) {
                print_help(argv[0], stderr);
                Partition_Close(partition);
                Disk_Close(disk);
                return 1;
            }
            const char* image_path = argv[3];
            Filesystem_File* image_dir = Filesystem_OpenPath(fs->root, image_path, 0, 0, 0, 0, 0, 0, 0, 0);
            if (!image_dir) {
                fprintf(stderr, "Couldn't open '%s'\n", image_path);
                result = 1;
                break;
            }
            if (Filesystem_PrintAll(image_dir, image_path, 0) != 0) {
                fprintf(stderr, "Couldn't list '%s'\n", image_path);
                result = 1;
            }
            Filesystem_CloseEntry(image_dir);
            break;
        }
    }

    Filesystem_Close(fs);
    Partition_Close(partition);
    Disk_Close(disk);
    return result;
}
