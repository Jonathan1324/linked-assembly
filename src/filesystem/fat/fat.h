#pragma once

#include <stdint.h>
#include "../partition/partition.h"

#define CHUNK_SIZE 512

static const uint32_t FAT12_MAX_CLUSTERS = 0xFFF;

#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY144     0xF0 // 1.44 MB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY120     0xF4 // 1.2 MB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY720     0xF9 // 720 KB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY400     0xFD // 400 KB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY360_OLD 0xFF // 360 KB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY360     0xF6 // 360 KB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY320     0xF7 // 320 KB
#define FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_DISK          0xF8

#define FAT12_BOOTSECTOR_NO_EXTENDED_BOOT_SIGNATURE     0x00
#define FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE_OLD    0x28
#define FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE        0x29

#define FAT_BUFFER_SIZE 1024

#define FAT_ENTRY_DELETED (char)0xE5

#define FAT_ENTRY_READ_ONLY     0x01
#define FAT_ENTRY_HIDDEN        0x02
#define FAT_ENTRY_SYSTEM        0x04
#define FAT_ENTRY_VOLUME_LABEL  0x08
#define FAT_ENTRY_DIRECTORY     0x10
#define FAT_ENTRY_ARCHIVE       0x20

typedef struct FAT12_Bootsector_Header {
    char oem_name[8];
    
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;

    uint8_t number_of_fats;
    uint16_t max_root_directory_entries;
    uint16_t total_sectors;

    uint8_t media_descriptor;

    uint16_t fat_size; // in sectors

    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    
    uint32_t hidden_sectors;
    uint32_t large_total_sectors; // if total_sectors > 65535, else 0

    uint8_t drive_number;

    uint8_t reserved;
    uint8_t boot_signature;

    // Only when FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE
    uint32_t volume_id;

    // Only when FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE or FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE_OLD
    char volume_label[11];
    char filesystem_type[8];

} __attribute__((packed)) FAT12_Bootsector_Header;

#define FAT_BOOTSECTOR_SIGNATURE 0x55AA

typedef struct FAT12_Bootsector {
    uint8_t jump[3];
    FAT12_Bootsector_Header header;
    uint8_t bootcode[448];
    uint16_t signature;
} __attribute__((packed)) FAT12_Bootsector;

typedef struct FAT_DirectoryEntry {
    char name[8];
    char ext[3];

    uint8_t attribute;

    uint8_t reserved;

    uint8_t creation_time_tenths; // tenths of second
    uint16_t creation_time; // HHMMSS (Hour-Minute-Second)
    uint16_t creation_date;

    uint16_t last_access_date;
    
    uint16_t first_cluster_high; // Only FAT32

    uint16_t last_modification_time;
    uint16_t last_modification_date;

    uint16_t first_cluster; // low word

    uint32_t file_size; // bytes

} __attribute__((packed)) FAT_DirectoryEntry;

typedef struct FAT_LFNEntry {
    uint8_t order;

    uint16_t name1[5];

    uint8_t attr;
    uint8_t reserved;
    uint8_t checksum;

    uint16_t name2[6];

    uint16_t reserved2;

    uint16_t name3[2];

} __attribute__((packed)) FAT_LFNEntry;

typedef struct FAT12_Filesystem FAT12_Filesystem;

typedef struct FAT12_File {
    FAT12_Filesystem* fs;

    uint32_t size;

    uint32_t first_cluster;

    uint32_t directory_entry_offset; // absolute in bytes from file start

    int is_root_directory;
    int is_directory;

} FAT12_File;

struct FAT12_Filesystem {
    Partition* partition;
    FAT12_Bootsector bootsector;

    FAT12_File* root;
    FAT12_File static_root;

    uint8_t fat_buffer[FAT_BUFFER_SIZE];
    uint64_t fat_buffer_start;

    uint64_t size; // in bytes

    uint64_t fat_offset;    // in bytes
    uint64_t fat_size;      // in bytes

    uint64_t root_offset;   // in bytes
    uint64_t root_size;     // in bytes

    uint64_t data_offset;   // in bytes
    uint64_t data_size;     // in bytes

    uint64_t cluster_size;  // in bytes

};

// Functions:

int FAT_ParseName(const char* name, char fat_name[8], char fat_ext[3]);
FAT_LFNEntry* FAT_CreateLFNEntries(const char* name, uint32_t* out_count, uint8_t checksum);
uint16_t* FAT_CombineLFN(FAT_LFNEntry* entries, uint32_t entry_count, uint32_t* len);
uint8_t FAT_GetChecksum(const char shortname[11]);
static inline uint8_t FAT_CreateChecksum(FAT_DirectoryEntry* entry)
{
    char full[11];
    for (int i = 0; i < 8; i++) full[i] = entry->name[i];
    for (int i = 0; i < 3; i++) full[i+8] = entry->ext[i];
    return FAT_GetChecksum(full);
}

void FAT_EncodeTime(int64_t epoch, uint16_t* fat_date, uint16_t* fat_time, uint8_t* tenths);

int FAT12_FlushFATBuffer(FAT12_Filesystem* fs);
int FAT12_LoadFATBuffer(FAT12_Filesystem* fs, uint32_t offset);

uint32_t FAT12_ReadFromFileRaw(FAT12_File* f, uint32_t offset, uint8_t* buffer, uint32_t size);
uint32_t FAT12_WriteToFileRaw(FAT12_File* f, uint32_t offset, uint8_t* buffer, uint32_t size);
int FAT12_ReserveSpace(FAT12_File* f, uint32_t extra, int update_entry_size);
uint32_t FAT12_GetAbsoluteOffset(FAT12_File* f, uint32_t relative_offset);

uint32_t FAT12_AddDirectoryEntry(FAT12_File* directory, FAT_DirectoryEntry* entry, FAT_LFNEntry* lfn_entries, uint32_t lfn_count);
int FAT12_AddDotsToDirectory(FAT12_File* directory, FAT12_File* parent);

int FAT12_GetDirectoryEntry(FAT12_File* f, FAT_DirectoryEntry* entry);
int FAT12_SetDirectoryEntry(FAT12_File* f, FAT_DirectoryEntry* entry);

FAT12_File* FAT12_CreateEntryRaw(FAT12_File* dir, FAT_DirectoryEntry* entry, int is_directory, FAT_LFNEntry* lfn_entries, uint32_t lfn_count);
FAT12_File* FAT12_CreateEntry(FAT12_File* parent, const char* name, uint8_t attribute, int is_directory, int64_t creation, int64_t last_modification, int64_t last_access, int use_lfn);
void FAT12_CloseEntry(FAT12_File* entry);

FAT12_File* FAT12_FindEntry(FAT12_File* parent, const char* name);

static inline uint32_t FAT12_ReadFromFile(FAT12_File* f, uint32_t offset, uint8_t* buffer, uint32_t size)
{
    if (!f || f->is_directory) return 0;
    return FAT12_ReadFromFileRaw(f, offset, buffer, size);
}

static inline uint32_t FAT12_WriteToFile(FAT12_File* f, uint32_t offset, uint8_t* buffer, uint32_t size)
{
    if (!f || f->is_directory) return 0;
    return FAT12_WriteToFileRaw(f, offset, buffer, size);
}

// Initializes an empty FAT12 Filesystem
FAT12_Filesystem* FAT12_CreateEmptyFilesystem(Partition* partition,
                                              const char* oem_name, const char* volume_label, uint32_t volume_id,
                                              uint32_t total_size, uint32_t bytes_per_sector, uint8_t sectors_per_cluster,
                                              uint16_t reserved_sectors, uint8_t number_of_fats, uint16_t max_root_directory_entries,
                                              uint16_t sectors_per_track, uint16_t number_of_heads, uint8_t drive_number,
                                              uint8_t media_descriptor );

void FAT12_CloseFilesystem(FAT12_Filesystem* fs);


uint32_t FAT12_ReadFATEntry(FAT12_Filesystem* fs, uint32_t cluster);
int FAT12_WriteFATEntry(FAT12_Filesystem* fs, uint32_t cluster, uint32_t value);

uint32_t FAT12_FindNextFreeCluster(FAT12_Filesystem* fs, uint32_t start_cluster);
int FAT12_FindFreeClusters(FAT12_Filesystem* fs, uint32_t* cluster_array, uint32_t count);

// EmptyFS:

// Writes a FAT12 bootsector to the given file stream 'f' using the specified OEM name and volume label.
// Parameters:
//   f      - Pointer to the file stream to write the bootsector to.
//   oem    - OEM name string (up to 8 characters).
//   volume - Volume label string (up to 11 characters).
int FAT12_WriteBootsector(FAT12_Filesystem* fs,
                          const char* oem_name, const char* volume_label, uint32_t volume_id,
                          uint32_t total_size, uint32_t bytes_per_sector, uint8_t sectors_per_cluster,
                          uint16_t reserved_sectors, uint8_t number_of_fats, uint16_t max_root_directory_entries,
                          uint16_t sectors_per_track, uint16_t number_of_heads, uint8_t drive_number,
                          uint8_t media_descriptor);

// Seeks to fs->fat_offset and writes FAT
// Parameters:
//   fs     - FAT12_Filesystem struct
int FAT12_WriteEmptyFAT(FAT12_Filesystem* fs);
// Copies fs->fat_offset + fs->fat_size*src to fs->fat_offset + fs->fat_size*dst
// Parameters:
//   fs     - FAT12_Filesystem struct
//   dst    - index of destination for the FAT
//   src    - FAT that needs to be copied
int FAT12_CopyFAT(FAT12_Filesystem* fs, uint8_t dst, uint8_t src);

// Creates an empty root directory
// Parameters:
//   fs     - FAT12_Filesystem struct
int FAT12_WriteEmptyRootDir(FAT12_Filesystem* fs);



static uint32_t utf8_to_utf16(const char* input, uint16_t** out)
{
    uint32_t len = strlen(input);
    uint16_t* buf = malloc(sizeof(uint16_t) * (len + 1) * 2);
    if (!buf) return 0;
    uint32_t outlen = 0;

    const uint8_t* p = (const uint8_t*)input;

    while (*p)
    {
        uint32_t code = 0;

        if ((*p & 0x80) == 0x00) {
            code = *p++;
        } else if ((*p & 0xE0) == 0xC0) {
            code = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {
            code = ((*p & 0x0F) << 12) |
                   ((p[1] & 0x3F) << 6) |
                   (p[2] & 0x3F);
            p += 3;
        } else {
            // unsupported → replace
            code = '?';
            p++;
        }

        buf[outlen++] = (uint16_t)code;
    }

    *out = buf;
    return outlen;
}
