#include "fat.h"
#include <stdlib.h>
#include <string.h>

int FAT_WriteBootsector(FILE* f, const char* oem_name, const char* volume_label, uint32_t volume_id)
{
    FAT12_Bootsector bootsector = {0};

    // TODO: currently just setting the jump for x86
    bootsector.jump[0] = 0xEB;  // jmp short
    bootsector.jump[1] = 0x3C;  // end of header
    bootsector.jump[2] = 0x90;  // nop

    bootsector.signature = 0xAA55;

    memset(bootsector.header.oem_name, ' ', 8);
    for (int i = 0; i < 8; i++) {
        if (oem_name[i] == 0) break;
        bootsector.header.oem_name[i] = oem_name[i];
    }

    bootsector.header.bytes_per_sector = 512;
    bootsector.header.sectors_per_cluster = 1;
    bootsector.header.reserved_sectors = 1; // TODO: set dynamically
    bootsector.header.number_of_fats = 2;
    bootsector.header.max_root_directory_entries = 224;
    bootsector.header.total_sectors = 2880;
    bootsector.header.media_descriptor = FAT12_BOOTSECTOR_MEDIA_DESCRIPTOR_FLOPPY144;
    bootsector.header.fat_size = 9;
    bootsector.header.sectors_per_track = 18;
    bootsector.header.number_of_heads = 2;
    bootsector.header.hidden_sectors = 0;
    bootsector.header.large_total_sectors = 0;
    bootsector.header.drive_number = 0;
    bootsector.header.reserved = 0;
    bootsector.header.boot_signature = FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE;

    memset(bootsector.header.volume_label, ' ', 11);
    for (int i = 0; i < 11; i++) {
        if (volume_label[i] == 0) break;
        bootsector.header.volume_label[i] = volume_label[i];
    }
    bootsector.header.filesystem_type[0] = 'F';
    bootsector.header.filesystem_type[1] = 'A';
    bootsector.header.filesystem_type[2] = 'T';
    bootsector.header.filesystem_type[3] = '1';
    bootsector.header.filesystem_type[4] = '2';
    bootsector.header.filesystem_type[5] = ' ';
    bootsector.header.filesystem_type[6] = ' ';
    bootsector.header.filesystem_type[7] = ' ';

    bootsector.header.volume_id = volume_id;

    size_t written = fwrite(&bootsector, sizeof(bootsector), 1, f);
    if (written != 1) {
        perror("fwrite");
        return 1;
    }

    return 0;
}
