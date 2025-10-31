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
        uint64_t abs = f->fs->root_offset + offset;
        if (Partition_Read(f->fs->partition, out, abs, remaining) != remaining) return 0;
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

        if (Partition_Read(f->fs->partition, out, abs, chunk) != chunk) break;

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
        if (Partition_Write(f->fs->partition, buffer, abs, remaining) != remaining) return 0;
        return remaining;
    }

    uint8_t* out = buffer;
    uint32_t cluster_size = f->fs->bootsector.header.bytes_per_sector * f->fs->bootsector.header.sectors_per_cluster;

    uint32_t remaining = size;
    if (offset + remaining > f->size) {
        uint32_t old_end = f->size;
        uint32_t reserve = offset + remaining - f->size;
        // TODO
        int r = FAT12_ReserveSpace(f, reserve, !f->is_directory);
        if (r != 0) return 0;
        if (offset > old_end) {
            // fill in the rest
            uint32_t uninitialized = offset - old_end;
            uint8_t zero_buffer[CHUNK_SIZE] = {0};
            
            uint32_t off = old_end;
            while (uninitialized > 0) {
                uint32_t chunk = (uninitialized > CHUNK_SIZE) ? CHUNK_SIZE : uninitialized;
                // TODO: set to zero with a better way
                if (FAT12_WriteToFileRaw(f, off, zero_buffer, chunk) != chunk) {
                    // warning
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

        if (Partition_Write(f->fs->partition, out, abs, chunk) != chunk) break;;

        written += chunk;
        remaining -= chunk;
        out += chunk;
        off = 0;

        if (remaining > 0) cluster = FAT12_ReadFATEntry(f->fs, cluster);
    }

    return written;
}

int FAT12_ReserveSpace(FAT12_File* f, uint32_t extra, int update_entry_size)
{
    if (!f || f->is_root_directory) return 1;

    uint32_t cluster_size = f->fs->bootsector.header.bytes_per_sector * f->fs->bootsector.header.sectors_per_cluster;
    uint32_t total_size = f->size + extra;
    uint32_t needed_clusters = (total_size + cluster_size - 1) / cluster_size;
    uint32_t current_clusters = (f->size + cluster_size - 1) / cluster_size;

    FAT_DirectoryEntry entry;
    if (Partition_Read(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;

    if (update_entry_size) entry.file_size = total_size;

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

    if (Partition_Write(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;
    f->size = total_size;

    return 0;
}

uint32_t FAT12_GetAbsoluteOffset(FAT12_File* f, uint32_t relative_offset)
{
    if (!f) return 0;

    if (relative_offset > f->size) return 0;

    if (f->is_root_directory) {
        return f->fs->root_offset + relative_offset;
    }

    uint16_t cluster = f->first_cluster;
    if (cluster < 2) return 0;

    uint32_t skip = relative_offset / f->fs->cluster_size;
    uint32_t off = relative_offset % f->fs->cluster_size;

    while (skip--) {
        cluster = FAT12_ReadFATEntry(f->fs, cluster);
        if (cluster >= 0xFF8) return 0;
    }

    return f->fs->data_offset + (cluster - 2) * f->fs->cluster_size + off;
}

int FAT12_GetDirectoryEntry(FAT12_File* f, FAT_DirectoryEntry* entry)
{
    if (!f || !entry) return 1;
    if (Partition_Read(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;
    return 0;
}

int FAT12_SetDirectoryEntry(FAT12_File* f, FAT_DirectoryEntry* entry)
{
    if (!f || !entry) return 1;
    if (Partition_Write(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;
    return 0;
}

FAT12_File* FAT12_CreateEntry(FAT12_File* dir, FAT_DirectoryEntry* entry, int is_directory, FAT_LFNEntry* lfn_entries, uint32_t lfn_count)
{
    if (!entry) return NULL;

    FAT12_File* f = (FAT12_File*)malloc(sizeof(FAT12_File));
    if (!f) return NULL;
    
    uint32_t rel_offset = FAT12_AddDirectoryEntry(dir, entry, lfn_entries, lfn_count);

    f->fs = dir->fs;
    f->size = 0;
    f->first_cluster = 0;
    f->directory_entry_offset = FAT12_GetAbsoluteOffset(dir, rel_offset);
    f->is_root_directory = 0;
    f->is_directory = is_directory;

    return f;
}

void FAT12_CloseEntry(FAT12_File* entry)
{
    if (!entry) return;
    free(entry);
}
