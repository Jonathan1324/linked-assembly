#pragma once
#include <stdio.h>
#include <stdint.h>

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

// Writes a FAT12 bootsector to the given file stream 'f' using the specified OEM name and volume label.
// Parameters:
//   f      - Pointer to the file stream to write the bootsector to.
//   oem    - OEM name string (up to 8 characters).
//   volume - Volume label string (up to 11 characters).
int FAT_WriteBootsector(FILE* f, const char* oem_name, const char* volume_label, uint32_t volume_id);
