#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fat/fat.h"
#include "disk/disk.h"
#include "filesystem/filesystem.h"
#include "version.h"

#include "native/directory.h"

typedef uint8_t FS_Action;
#define FS_NONE     0x00
#define FS_CREATE   0x01
#define FS_INSERT   0x02
#define FS_EXTRACT  0x04
#define FS_LIST     0x08
#define FS_REMOVE   0x10

void print_help(const char* name, FILE* s)
{
    fputs("Usage:\n", s);
    fprintf(s, "> %s create fat12|fat16|fat32 <image> [--size B/K/M/G/T] [--boot <file of bootsector>] [--root <host path>] [flags]\n", name);
    fprintf(s, "> %s insert <host path> <image> [--path <image path>] [flags]\n", name);
    fprintf(s, "> %s extract <image path> <image> [--path <host path>] [flags]\n", name);
    fprintf(s, "> %s remove <image path> <image> [flags]", name);
    fprintf(s, "> %s list <image path> <image> [flags]\n", name);
    fprintf(s, "> %s listall <image> [flags]\n", name);
    fputc('\n', s);
    fputs("Flags:\n", s);
    fputs("> --no-lfn                   Disable long file names for FAT\n", s);
    fputs("> --read-only                Open image in read-only mode\n", s);
    fputs("> --force-bootsector         Force the bootsector file to be written and don't override header and signature\n", s);
    fputs("> --save                     Don't delete directories recursively\n", s);
}

uint64_t parse_size(const char* size_str)
{
    if (!size_str) return 0;

    uint64_t len = strlen(size_str);
    if (len == 0) return 0;

    uint64_t multiplier = 1;
    char last = size_str[len - 1];
    switch (toupper(last)) {
        case 'T': multiplier = 1024ULL*1024*1024*1024; len--; break;
        case 'G': multiplier = 1024ULL*1024*1024; len--; break;
        case 'M': multiplier = 1024ULL*1024; len--; break;
        case 'K': multiplier = 1024ULL; len--; break;
        case 'B': default: multiplier = 1; break;
    }

    uint64_t whole = 0;
    uint64_t fraction = 0;
    uint64_t frac_divisor = 1;
    int after_dot = 0;

    for (size_t i = 0; i < len; i++) {
        char c = size_str[i];
        if (c == '.') {
            after_dot = 1;
            continue;
        }
        if (!isdigit(c)) break;

        if (!after_dot) {
            whole = whole * 10 + (c - '0');
        } else {
            if (frac_divisor < 1000000000000000000ULL) {
                fraction = fraction * 10 + (c - '0');
                frac_divisor *= 10;
            }
        }
    }

    uint64_t result = whole * multiplier;
    if (fraction > 0) {
        result += (fraction * multiplier) / frac_divisor;
    }

    return result;
}

int main(int argc, const char *argv[])
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_help(argv[0], stdout);
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

    const char* remove_path = NULL;

    int allow_path_flag = 0;
    int allow_root_flag = 0;
    int allow_size = 0;

    const char* list_path = NULL;

    const char* path = NULL;

    int allow_bootcode_flag = 0;

    Filesystem_Type fs_type;

    if (argc < 2) {
        print_help(argv[0], stderr);
        return 1;
    }

    int arg_start = 1;

    if (strcmp(argv[1], "create") == 0) {
        format = 1;
        truncate = 1;
        fs_actions |= FS_CREATE;
        allow_root_flag = 1;
        allow_size = 1;
        allow_bootcode_flag = 1;

        if (argc < 3) {
            print_help(argv[0], stderr);
            return 1;
        }
        image_file = argv[3];

        const char* fs_name = argv[2];
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

        arg_start += 3;
    } else if (strcmp(argv[1], "insert") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_INSERT;

        if (argc < 4) {
            print_help(argv[0], stderr);
            return 1;
        }
        image_file = argv[3];
        
        insert_file = argv[2];
        path = argv[2];
        allow_path_flag = 1;

        arg_start += 3;
    } else if (strcmp(argv[1], "extract") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_EXTRACT;

        if (argc < 4) {
            print_help(argv[0], stderr);
            return 1;
        }
        image_file = argv[3];
        
        extract_file = argv[2];
        path = argv[2];
        allow_path_flag = 1;

        arg_start += 3;
    } else if (strcmp(argv[1], "remove") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_REMOVE;

        if (argc < 4) {
            print_help(argv[0], stderr);
            return 1;
        }
        image_file = argv[3];

        remove_path = argv[2];

        arg_start += 3;
    } else if(strcmp(argv[1], "list") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_LIST;

        if (argc < 4) {
            print_help(argv[0], stderr);
            return 1;
        }
        image_file = argv[3];

        list_path = argv[2];

        arg_start += 3;
    } else if(strcmp(argv[1], "listall") == 0) {
        format = 0;
        truncate = 0;
        fs_actions |= FS_LIST;

        if (argc < 3) {
            print_help(argv[0], stderr);
            return 1;
        }
        image_file = argv[2];

        list_path = "/";

        arg_start += 2;
    } else {
        fputs("Unknown action\n", stderr);
        return 1;
    }

    if (!image_file) {
        fputs("Output file is required\n", stderr);
        return 1;
    }

    int fat_use_lfn = 1;
    int read_only = 0;
    uint64_t fs_size = 0;

    int save = 0;

    const char* bootcode_file = NULL;
    int force_bootsector = 0;

    for (int i = arg_start; i < argc; i++) {
        if (allow_path_flag && strcmp(argv[i], "--path") == 0) {
            i++;
            if (argc < i) {
                fputs("No path after '--path'\n", stderr);
                return 1;
            }
            uint64_t len = strlen(argv[i]);
            //if (argv[i][len-1] == '/') argv[i][len-1] = '\0';
            path = argv[i];
        } else if (allow_root_flag && strcmp(argv[i], "--root") == 0) {
            i++;
            if (argc < i) {
                fputs("No path after '--root'\n", stderr);
                return 1;
            }
            uint64_t len = strlen(argv[i]);
            //if (argv[i][len-1] == '/') argv[i][len-1] = '\0';
            root_file = argv[i];
        } else if (allow_size && strcmp(argv[i], "--size") == 0) {
            i++;
            if (argc < i) {
                fputs("No size after '--size'\n", stderr);
                return 1;
            }
            fs_size = parse_size(argv[i]);
        } else if (allow_bootcode_flag && strcmp(argv[i], "--boot") == 0) {
            i++;
            if (argc < i) {
                fputs("No file after '--boot'\n", stderr);
                return 1;
            }
            bootcode_file = argv[i];
        }

        else if (strcmp(argv[i], "--no-lfn") == 0) {
            fat_use_lfn = 0;
        } else if (strcmp(argv[i], "--read-only") == 0) {
            read_only = 1;
        } else if (strcmp(argv[i], "--force-bootsector") == 0) {
            force_bootsector = 1;
        } else if (strcmp(argv[i], "--save") == 0) {
            save = 1;
        }

        else {
            fprintf(stderr, "Unknown argument '%s'\n", argv[i]);
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

    if (format) {
        if (fs_size == 0) {
            switch (fs_type) {
                case FILESYSTEM_FAT12: fs_size = 1474560; break;   // 1440 KB
                case FILESYSTEM_FAT16: fs_size = 16777216; break;  // 16 MB
                case FILESYSTEM_FAT32: fs_size = 104857600; break; // 100 MB
            }
        }
    } else {
        fs_size = disk->size;
    }

    Partition* partition = Partition_Create(disk, 0, fs_size, read_only);
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

    uint8_t bootsector_buffer[512];
    if (bootcode_file) {
        FILE* bootcode_f = fopen(bootcode_file, "rb");
        if (!bootcode_f) {
            Partition_Close(partition);
            Disk_Close(disk);
            fprintf(stderr, "Couldn't open '%s'\n", bootcode_file);
            return 1;
        }
        if (fread(bootsector_buffer, sizeof(bootsector_buffer), 1, bootcode_f) != 1) {
            Partition_Close(partition);
            Disk_Close(disk);
            fprintf(stderr, "Couldn't read 512 bytes of '%s'\n", bootcode_file);
            return 1;
        }
        fclose(bootcode_f);
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

        // TODO
        fat_fs = FAT_CreateEmptyFilesystem(partition, fat_version, (bootcode_file ? bootsector_buffer : NULL), force_bootsector,
                                           oem_name, volume_name, volume_id,
                                           partition->size, bytes_per_sector,
                                           sectors_per_cluster, reserved_sectors,
                                           number_of_fats, max_root_directory_entries,
                                           sectors_per_track, number_of_heads,
                                           drive_number, media_descriptor);
    } else {
        fat_fs = FAT_OpenFilesystem(partition, fat_version, read_only);
    }

    if (!fat_fs) {
        Partition_Close(partition);
        Disk_Close(disk);
        return 1;
    }

    Filesystem* fs = Filesystem_CreateFromFAT(fat_fs, fat_use_lfn);
    if (!fs) {
        Filesystem_Close(fs);
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

    if (fs_actions & FS_REMOVE) {
        int result = Filesystem_DeletePath(fs->root, remove_path, save);
        if (result == 2) {
            printf("Warning: Directory '%s' isn't empty\n", remove_path);
        } else if (result != 0) {
            printf("Warning: Couldn't remove '%s'\n", remove_path);
        }
    }

    if (fs_actions & FS_LIST) {
        Filesystem_File* l = Filesystem_OpenPath(fs->root, list_path, 0, 0, 0, 0, 0, 0, 0, 0);
        if (!l) {
            printf("Warning: Couldn't open '%s'\n", list_path);
        }
        if (Filesystem_PrintAll(l, list_path, 0) != 0) {
            printf("Warning: Couldn't list '%s'\n", list_path);
        }
        Filesystem_CloseEntry(l);
    }

    Filesystem_Close(fs);
    Partition_Close(partition);
    Disk_Close(disk);

    return 0;
}
