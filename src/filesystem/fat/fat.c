#include "fat.h"
#include <stdlib.h>
#include <string.h>

int FAT12_WriteBootsector(FAT12_Filesystem* fs,
                          const char* oem_name, const char* volume_label, uint32_t volume_id,
                          uint32_t total_size, uint32_t bytes_per_sector, uint8_t sectors_per_cluster,
                          uint16_t reserved_sectors, uint8_t number_of_fats, uint16_t max_root_directory_entries,
                          uint16_t sectors_per_track, uint16_t number_of_heads, uint8_t drive_number,
                          uint8_t media_descriptor)
{
    if (!fs) return 1;
    memset(&fs->bootsector, 0, sizeof(FAT12_Bootsector));

    fs->bootsector.signature = 0xAA55;

    memset(fs->bootsector.header.oem_name, ' ', 8);
    for (int i = 0; i < 8; i++) {
        if (oem_name[i] == 0) break;
        fs->bootsector.header.oem_name[i] = oem_name[i];
    }

    uint32_t total_sectors = total_size / bytes_per_sector;
    uint32_t root_dir_sectors = (max_root_directory_entries * sizeof(FAT_DirectoryEntry) + bytes_per_sector - 1) / bytes_per_sector;

    uint32_t total_clusters;
    uint32_t fat_bytes;
    uint32_t fat_sectors = 1;
    uint32_t data_sectors = total_size / bytes_per_sector - (reserved_sectors + number_of_fats * fat_sectors + root_dir_sectors);

    const int MAX_ITERATIONS = 10;
    int iteration = 0;

    while (1) {
        data_sectors = total_sectors - (reserved_sectors + number_of_fats * fat_sectors + root_dir_sectors);
        total_clusters = data_sectors / sectors_per_cluster;
        if (total_clusters > FAT12_MAX_CLUSTERS) {
            fprintf(stderr, "Warning: Too many clusters for FAT12 (%u). Reducing to max %u.\n",
                    total_clusters, FAT12_MAX_CLUSTERS);
            total_clusters = FAT12_MAX_CLUSTERS;
            data_sectors = total_clusters * sectors_per_cluster;
        }

        fat_bytes = (total_clusters * 3 + 1) / 2;
        uint32_t new_fat_sectors = (fat_bytes + bytes_per_sector - 1) / bytes_per_sector;
        if (new_fat_sectors == fat_sectors) break;
        fat_sectors = new_fat_sectors;

        if (iteration++ > MAX_ITERATIONS) {
            fprintf(stderr, "Max iterations reached for calculating fat_sectors. Using %u\n", fat_sectors);
            break;
        }
    }

    int use_large_total_sectors = total_sectors > 65535;
    
    fs->bootsector.header.bytes_per_sector = bytes_per_sector;
    fs->bootsector.header.sectors_per_cluster = sectors_per_cluster;
    fs->bootsector.header.reserved_sectors = reserved_sectors;
    fs->bootsector.header.number_of_fats = number_of_fats;
    fs->bootsector.header.max_root_directory_entries = max_root_directory_entries;
    fs->bootsector.header.total_sectors = use_large_total_sectors ? 0 : (uint16_t)total_sectors;
    fs->bootsector.header.media_descriptor = media_descriptor;
    fs->bootsector.header.fat_size = fat_sectors;
    fs->bootsector.header.sectors_per_track = sectors_per_track;
    fs->bootsector.header.number_of_heads = number_of_heads;
    fs->bootsector.header.hidden_sectors = 0;
    fs->bootsector.header.large_total_sectors = use_large_total_sectors ? total_sectors : 0;
    fs->bootsector.header.drive_number = drive_number;
    fs->bootsector.header.reserved = 0;
    fs->bootsector.header.boot_signature = FAT12_BOOTSECTOR_EXTENDED_BOOT_SIGNATURE;

    memset(fs->bootsector.header.volume_label, ' ', 11);
    for (int i = 0; i < 11; i++) {
        if (volume_label[i] == 0) break;
        fs->bootsector.header.volume_label[i] = volume_label[i];
    }
    fs->bootsector.header.filesystem_type[0] = 'F';
    fs->bootsector.header.filesystem_type[1] = 'A';
    fs->bootsector.header.filesystem_type[2] = 'T';
    fs->bootsector.header.filesystem_type[3] = '1';
    fs->bootsector.header.filesystem_type[4] = '2';
    fs->bootsector.header.filesystem_type[5] = ' ';
    fs->bootsector.header.filesystem_type[6] = ' ';
    fs->bootsector.header.filesystem_type[7] = ' ';

    fs->bootsector.header.volume_id = volume_id;

    // TODO: currently just setting code for x86
    fs->bootsector.jump[0] = 0xEB;  // jmp short
    fs->bootsector.jump[1] = 0x3C;  // end of header
    fs->bootsector.jump[2] = 0x90;  // nop

    uint8_t code[29] = {0x0E, 0x1F, 0xBE, 0x5B, 0x7C, 0xAC, 0x22, 0xC0, 0x74, 0x0B,
                      0x56, 0xB4, 0x0E, 0xBB, 0x07, 0x00, 0xCD, 0x10, 0x5E, 0xEB,
                      0xF0, 0x32, 0xE4, 0xCD, 0x16, 0xCD, 0x19, 0xEB, 0xFE};

    char data[100] = "This is not a bootable disk.  Please insert a bootable floppy and\r\npress any key to try again ... \r\n";

    memset(fs->bootsector.bootcode, 0, sizeof(fs->bootsector.bootcode));
    memcpy(fs->bootsector.bootcode, code, sizeof(code));
    memcpy(fs->bootsector.bootcode + sizeof(code), data, sizeof(data));

    size_t written = fwrite(&fs->bootsector, sizeof(FAT12_Bootsector), 1, fs->f);
    if (written != 1) {
        perror("fwrite");
        return 1;
    }
    return 0;
}

#define CHUNK_SIZE 512

int FAT12_WriteEmptyFAT(FAT12_Filesystem* fs)
{
    if (!fs) return 1;

    if (fseek(fs->f, fs->fat_offset, SEEK_SET) != 0) return 1;

    uint32_t fat_size = fs->fat_size;
    uint32_t written = 0;

    uint8_t header[3] = { fs->bootsector.header.media_descriptor, 0xFF, 0xFF };
    if (fwrite(header, 1, 3, fs->f) != 3) return 1;
    written += 3;

    uint8_t zero_block[CHUNK_SIZE] = {0};
    while (written < fat_size) {
        uint32_t chunk = (fat_size - written) < CHUNK_SIZE ? (fat_size - written) : CHUNK_SIZE;
        if (fwrite(zero_block, 1, chunk, fs->f) != chunk) return 1;
        written += chunk;
    }

    return 0;
}

int FAT12_CopyFAT(FAT12_Filesystem* fs, uint8_t dst, uint8_t src)
{
    if (!fs) return 1;

    uint32_t offset_dst = fs->fat_offset + fs->fat_size * dst;
    uint32_t offset_src = fs->fat_offset + fs->fat_size * src;

    uint8_t buf[CHUNK_SIZE];
    size_t remaining = fs->fat_size;

    while (remaining > 0) {
        size_t to_copy = remaining < CHUNK_SIZE ? remaining : CHUNK_SIZE;

        if (fseek(fs->f, offset_src, SEEK_SET) != 0) return 1;
        if (fread(buf, 1, to_copy, fs->f) != to_copy) return 1;

        if (fseek(fs->f, offset_dst, SEEK_SET) != 0) return 1;
        if (fwrite(buf, 1, to_copy, fs->f) != to_copy) return 1;

        offset_src += to_copy;
        offset_dst += to_copy;
        remaining -= to_copy;
    }

    return 0;
}

int FAT12_WriteEmptyRootDir(FAT12_Filesystem* fs)
{
    if (!fs) return 1;

    uint32_t root_size = fs->bootsector.header.max_root_directory_entries * sizeof(FAT_DirectoryEntry);
    uint32_t root_offset = fs->fat_offset + fs->fat_size * fs->bootsector.header.number_of_fats;
    
    if (fseek(fs->f, root_offset, SEEK_SET) != 0) return 1;

    uint8_t zero_block[CHUNK_SIZE] = {0};
    uint32_t written = 0;

    while (written < root_size) {
        uint32_t chunk = (root_size - written) < CHUNK_SIZE ? (root_size - written) : CHUNK_SIZE;
        if (fwrite(zero_block, 1, chunk, fs->f) != chunk) return 1;
        written += chunk;
    }

    return 0;
}
