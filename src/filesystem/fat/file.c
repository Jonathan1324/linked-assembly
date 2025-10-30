#include "fat.h"

uint32_t FAT12_ReadFromFileRaw(FAT12_File* f, uint32_t offset, uint8_t* buffer, uint32_t size)
{
    if (!f || !buffer) return 0;
    if (offset >= f->size) return 0;

    uint32_t remaining = size;
    uint32_t file_pos = offset;
    uint8_t* out = buffer;

    if (offset + size > f->size) remaining = f->size - offset;

    if (f->is_root_directory) {
        uint32_t abs = f->fs->root_offset + offset;
        if (fseek(f->fs->f, abs, SEEK_SET) != 0) return 0;
        if (fread(out, 1, remaining, f->fs->f) != remaining) return 0;
        return remaining;
    }

    uint32_t cluster_size = f->fs->bootsector.header.bytes_per_sector * f->fs->bootsector.header.sectors_per_cluster;

    uint16_t cluster = f->first_cluster;
    if (cluster < 2) return 0;

    uint32_t skip = file_pos / cluster_size;
    uint32_t off = file_pos % cluster_size;

    while (skip--) {
        cluster = FAT12_ReadFATEntry(f->fs, cluster);
        if (cluster >= 0xFF8) return 0;
    }

    uint32_t total_read = 0;
    while (remaining > 0 && cluster < 0xFF8) {
        uint32_t abs = f->fs->data_offset + (cluster - 2) * cluster_size + off;
        uint32_t chunk = cluster_size - off;
        if (chunk > remaining) chunk = remaining;

        if (fseek(f->fs->f, abs, SEEK_SET) != 0) return total_read;
        if (fread(out, 1, chunk, f->fs->f) != chunk) return total_read;

        out += chunk;
        remaining -= chunk;
        total_read += chunk;
        off = 0;

        if (remaining > 0) cluster = FAT12_ReadFATEntry(f->fs, cluster);
    }

    return total_read;
}

uint32_t FAT12_WriteToFileRaw(FAT12_File* f, uint32_t offset, uint8_t* buffer, uint32_t size)
{
    if (!f || !buffer) return 0;

    uint8_t* out = buffer;

    if (f->is_root_directory) {
        uint32_t abs = f->fs->root_offset + offset;
        if (offset > f->size) return 0;
        uint32_t remaining = size;
        if (offset + remaining > f->size) remaining = f->size - offset;
        if (fseek(f->fs->f, abs, SEEK_SET) != 0) return 0;
        if (fwrite(out, 1, remaining, f->fs->f) != remaining) return 0;
        return remaining;
    } else {
        // TODO
        return 0;
    }
}
