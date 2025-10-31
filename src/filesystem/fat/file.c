#include "fat.h"
#include <stdlib.h>

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

        if (fseek(f->fs->f, abs, SEEK_SET) != 0) break;
        if (fread(out, 1, chunk, f->fs->f) != chunk) break;

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

    if (f->is_root_directory) {
        uint32_t abs = f->fs->root_offset + offset;
        if (offset > f->size) return 0;
        uint32_t remaining = size;
        if (offset + remaining > f->size) remaining = f->size - offset;
        if (fseek(f->fs->f, abs, SEEK_SET) != 0) return 0;
        if (fwrite(buffer, 1, remaining, f->fs->f) != remaining) return 0;
        return remaining;
    }

    uint8_t* out = buffer;
    uint32_t cluster_size = f->fs->bootsector.header.bytes_per_sector * f->fs->bootsector.header.sectors_per_cluster;

    uint32_t remaining = size;
    if (offset + remaining > f->size) {
        uint32_t old_end = f->size;
        uint32_t reserve = offset + remaining - f->size;
        if (FAT12_ReserveSpace(f, reserve) != 0) return 0;
        if (offset > old_end) {
            // fill in the rest
            uint32_t uninitialized = offset - old_end;
            uint8_t zero_buffer[CHUNK_SIZE] = {0};
            
            uint32_t off = old_end;
            while (uninitialized > 0) {
                uint32_t chunk = (uninitialized > CHUNK_SIZE) ? CHUNK_SIZE : uninitialized;
                if (FAT12_WriteToFileRaw(f, off, zero_buffer, chunk) != chunk) {
                    // TODO: warning
                }
                off += chunk;
                uninitialized -= chunk;
            }
        }
    }

    uint16_t cluster = f->first_cluster;
    if (cluster < 2) return 0;

    uint32_t skip = offset / cluster_size;
    uint32_t off = offset % cluster_size;

    while (skip--) {
        cluster = FAT12_ReadFATEntry(f->fs, cluster);
        if (cluster >= 0xFF8) return 0;
    }

    uint32_t written = 0;

    while (remaining > 0 && cluster < 0xFF8) {
        uint32_t abs = f->fs->data_offset + (cluster - 2) * cluster_size + off;
        uint32_t chunk = cluster_size - off;
        if (chunk > remaining) chunk = remaining;

        if (fseek(f->fs->f, abs, SEEK_SET) != 0) break;
        if (fwrite(out, 1, chunk, f->fs->f) != chunk) break;

        written += chunk;
        remaining -= chunk;
        out += chunk;
        off = 0;

        if (remaining > 0) cluster = FAT12_ReadFATEntry(f->fs, cluster);
    }

    return written;
}

int FAT12_ReserveSpace(FAT12_File* f, uint32_t extra)
{
    if (!f || f->is_root_directory) return 1;

    uint32_t cluster_size = f->fs->bootsector.header.bytes_per_sector * f->fs->bootsector.header.sectors_per_cluster;
    uint32_t total_size = f->size + extra;
    uint32_t needed_clusters = (total_size + cluster_size - 1) / cluster_size;
    uint32_t current_clusters = (f->size + cluster_size - 1) / cluster_size;

    FAT_DirectoryEntry entry;
    if (fseek(f->fs->f, f->directory_entry_offset, SEEK_SET) != 0) return 1;
    if (fread(&entry, sizeof(FAT_DirectoryEntry), 1, f->fs->f) != 1) return 1;
    entry.file_size = total_size;

    if (needed_clusters > current_clusters) {
        uint32_t new_clusters = needed_clusters - current_clusters;

        uint16_t* clusters = (uint16_t*)malloc(new_clusters * sizeof(uint16_t));
        if (FAT12_FindFreeClusters(f->fs, clusters, new_clusters) != 0) return 1;

        uint16_t cluster = f->first_cluster;
        if (cluster == 0) {
            f->first_cluster = clusters[0];
            entry.first_cluster = clusters[0];
        } else {
            uint16_t last = f->first_cluster;
            while (1) {
                uint16_t next = FAT12_ReadFATEntry(f->fs, last);
                if (next >= 0xFF8) break;
                last = next;
            }
            FAT12_WriteFATEntry(f->fs, last, clusters[0]);
        }

        for (uint32_t i = 0; i < new_clusters; i++) {
            uint16_t next = (i + 1 < new_clusters) ? clusters[i + 1] : 0xFFF;
            FAT12_WriteFATEntry(f->fs, clusters[i], next);
        }

        free(clusters);
    }

    if (fseek(f->fs->f, f->directory_entry_offset, SEEK_SET) != 0) return 1;
    if (fwrite(&entry, sizeof(FAT_DirectoryEntry), 1, f->fs->f) != 1) return 1;
    f->size = total_size;

    return 0;
}
