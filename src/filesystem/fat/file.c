#include "fat.h"

#include <string.h>
#include <stdlib.h>

uint32_t FAT_ReadFromFileRaw(FAT_File* f, uint32_t offset, uint8_t* buffer, uint32_t size)
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

    uint32_t cluster = f->first_cluster;
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

uint32_t FAT_WriteToFileRaw(FAT_File* f, uint32_t offset, uint8_t* buffer, uint32_t size)
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
        int r = FAT_ReserveSpace(f, reserve, !f->is_directory);
        if (r != 0) return 0;
        if (offset > old_end) {
            // fill in the rest
            uint32_t uninitialized = offset - old_end;
            uint8_t zero_buffer[CHUNK_SIZE] = {0};
            
            uint32_t off = old_end;
            while (uninitialized > 0) {
                uint32_t chunk = (uninitialized > CHUNK_SIZE) ? CHUNK_SIZE : uninitialized;
                // TODO: set to zero with a better way
                if (FAT_WriteToFileRaw(f, off, zero_buffer, chunk) != chunk) {
                    // warning
                }
                off += chunk;
                uninitialized -= chunk;
            }
        }
    }

    uint32_t cluster = f->first_cluster;
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

int FAT_ReserveSpace(FAT_File* f, uint32_t extra, int update_entry_size)
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

        uint32_t* clusters = (uint32_t*)malloc(new_clusters * sizeof(uint32_t));
        if (FAT12_FindFreeClusters(f->fs, clusters, new_clusters) != 0) return 1;

        uint32_t cluster = f->first_cluster;
        if (cluster == 0) {
            f->first_cluster = clusters[0];
            entry.first_cluster = clusters[0];
        } else {
            uint32_t last = f->first_cluster;
            while (1) {
                uint32_t next = FAT12_ReadFATEntry(f->fs, last);
                if (next >= 0xFF8) break;
                last = next;
            }
            FAT12_WriteFATEntry(f->fs, last, clusters[0]);
        }

        for (uint32_t i = 0; i < new_clusters; i++) {
            uint32_t next = (i + 1 < new_clusters) ? clusters[i + 1] : 0xFFF;
            FAT12_WriteFATEntry(f->fs, clusters[i], next);
        }

        free(clusters);
    }

    if (Partition_Write(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;
    f->size = total_size;

    return 0;
}

uint32_t FAT_GetAbsoluteOffset(FAT_File* f, uint32_t relative_offset)
{
    if (!f) return 0;

    if (relative_offset > f->size) return 0;

    if (f->is_root_directory) {
        return f->fs->root_offset + relative_offset;
    }

    uint32_t cluster = f->first_cluster;
    if (cluster < 2) return 0;

    uint32_t skip = relative_offset / f->fs->cluster_size;
    uint32_t off = relative_offset % f->fs->cluster_size;

    while (skip--) {
        cluster = FAT12_ReadFATEntry(f->fs, cluster);
        if (cluster >= 0xFF8) return 0;
    }

    return f->fs->data_offset + (cluster - 2) * f->fs->cluster_size + off;
}

int FAT_GetDirectoryEntry(FAT_File* f, FAT_DirectoryEntry* entry)
{
    if (!f || !entry) return 1;
    if (Partition_Read(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;
    return 0;
}

int FAT_SetDirectoryEntry(FAT_File* f, FAT_DirectoryEntry* entry)
{
    if (!f || !entry) return 1;
    if (Partition_Write(f->fs->partition, &entry, f->directory_entry_offset, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry)) return 1;
    return 0;
}

FAT_File* FAT_CreateEntryRaw(FAT_File* dir, FAT_DirectoryEntry* entry, int is_directory, FAT_LFNEntry* lfn_entries, uint32_t lfn_count)
{
    if (!entry) return NULL;

    FAT_File* f = (FAT_File*)malloc(sizeof(FAT_File));
    if (!f) return NULL;
    
    uint32_t rel_offset = FAT_AddDirectoryEntry(dir, entry, lfn_entries, lfn_count);

    f->fs = dir->fs;
    f->size = 0;
    f->first_cluster = 0;
    f->directory_entry_offset = FAT_GetAbsoluteOffset(dir, rel_offset);
    f->is_root_directory = 0;
    f->is_directory = is_directory;

    return f;
}

void FAT_CloseEntry(FAT_File* entry)
{
    if (!entry) return;
    free(entry);
}

FAT_File* FAT_CreateEntry(FAT_File* parent, const char* name, uint8_t attribute, int is_directory, int64_t creation, int64_t last_modification, int64_t last_access, int use_lfn)
{
    if (!parent || !parent->is_directory || !name) return NULL;

    FAT_DirectoryEntry entry;
    if (FAT_ParseName(name, entry.name, entry.ext) != 0) return NULL;

    entry.attribute = attribute;
    entry.reserved = 0;
    entry.first_cluster_high = 0;
    entry.first_cluster = 0;
    entry.file_size = 0;

    FAT_EncodeTime(creation, &entry.creation_date, &entry.creation_time, &entry.creation_time_tenths);

    uint8_t tenths;
    FAT_EncodeTime(last_modification, &entry.last_modification_date, &entry.last_modification_time, &tenths);

    uint16_t time;
    FAT_EncodeTime(last_access, &entry.last_access_date, &time, &tenths);

    uint8_t checksum = FAT_CreateChecksum(&entry);
    uint32_t lfn_count = 0;
    FAT_LFNEntry* lfn_entries = NULL;
    if (use_lfn) {
        lfn_entries = FAT_CreateLFNEntries(name, &lfn_count, checksum);
        if (!lfn_entries) return NULL;
    }

    FAT_File* file = FAT_CreateEntryRaw(parent, &entry, is_directory, lfn_entries, lfn_count);

    if (lfn_entries) free(lfn_entries);

    if (is_directory) FAT_AddDotsToDirectory(file, parent);

    return file;
}

FAT_File* FAT_FindEntry(FAT_File* parent, const char* name)
{
    if (!parent || !parent->is_directory || !name) return NULL;

    uint16_t* name16 = NULL;
    uint32_t nameLen = utf8_to_utf16(name, &name16);

    char short_name[8];
    char short_ext[3];
    if (FAT_ParseName(name, short_name, short_ext) != 0) return NULL;

    uint32_t offset = 0;
    FAT_DirectoryEntry entry;

    FAT_LFNEntry* lfn_entries = NULL;
    uint32_t lfn_count = 0;

    while(1) {
        uint32_t r = FAT_ReadFromFileRaw(parent, offset, (uint8_t*)&entry, sizeof(FAT_DirectoryEntry));
        if (r != sizeof(FAT_DirectoryEntry)) break;

        if (entry.name[0] == 0x00) break; // End of directory

        if (entry.name[0] == FAT_ENTRY_DELETED) {
            offset += sizeof(FAT_DirectoryEntry);
            continue;
        }

        if (entry.attribute == 0x0F) {
            
            FAT_LFNEntry* new_entries = (FAT_LFNEntry*)realloc(lfn_entries, sizeof(FAT_LFNEntry) * (lfn_count + 1));
            if (!new_entries) {
                free(lfn_entries);
                free(name16);
                return NULL;
            }
            lfn_entries = new_entries;
            memcpy(&lfn_entries[lfn_count], &entry, sizeof(FAT_LFNEntry));
            lfn_count++;

            offset += sizeof(FAT_DirectoryEntry);
            continue;
        }

        if (lfn_count > 0) {
            uint32_t utf16_len = 0;
            uint16_t* lfn_utf16 = FAT_CombineLFN(lfn_entries, lfn_count, &utf16_len);

            // FIXME: not working yet
            if (lfn_utf16) {
                if (utf16_len == nameLen && memcmp(name16, lfn_utf16, utf16_len) == 0) {
                    free(name16);
                    free(lfn_utf16);
                    free(lfn_entries);

                    FAT_File* file = (FAT_File*)malloc(sizeof(FAT_File));
                    if (!file) return NULL;

                    if (entry.attribute & FAT_ENTRY_DIRECTORY) {
                        file->fs = parent->fs;
                        file->first_cluster = entry.first_cluster;
                        file->directory_entry_offset = FAT_GetAbsoluteOffset(parent, offset);
                        file->is_root_directory = 0;
                        file->is_directory = 1;

                        uint32_t cluster_count = 0;
                        uint32_t cluster = entry.first_cluster;
                        while (cluster < 0xFF8) {
                            if (cluster == 0) break;
                            cluster_count++;
                            cluster = FAT12_ReadFATEntry(file->fs, cluster);
                        }
                        file->size = file->fs->cluster_size * cluster_count;
                    } else {
                        file->fs = parent->fs;
                        file->size = entry.file_size;
                        file->first_cluster = entry.first_cluster;
                        file->directory_entry_offset = FAT_GetAbsoluteOffset(parent, offset);
                        file->is_root_directory = 0;
                        file->is_directory = 0;
                    }

                    return file;
                }
            } else {
                // TODO: warning
            }

            free(lfn_entries);
            lfn_entries = NULL;
            lfn_count = 0;
        }

        if (memcmp(entry.name, short_name, 8) == 0 && memcmp(entry.ext, short_ext, 3) == 0) {
            free(name16);
            free(lfn_entries);

            FAT_File* file = (FAT_File*)malloc(sizeof(FAT_File));
            if (!file) return NULL;

            if (entry.attribute & FAT_ENTRY_DIRECTORY) {
                file->fs = parent->fs;
                file->first_cluster = entry.first_cluster;
                file->directory_entry_offset = FAT_GetAbsoluteOffset(parent, offset);
                file->is_root_directory = 0;
                file->is_directory = 1;

                uint32_t cluster_count = 0;
                uint32_t cluster = entry.first_cluster;
                while (cluster < 0xFF8) {
                    cluster_count++;
                    cluster = FAT12_ReadFATEntry(file->fs, cluster);
                }
                file->size = file->fs->cluster_size * cluster_count;
            } else {
                file->fs = parent->fs;
                file->size = entry.file_size;
                file->first_cluster = entry.first_cluster;
                file->directory_entry_offset = FAT_GetAbsoluteOffset(parent, offset);
                file->is_root_directory = 0;
                file->is_directory = 0;
            }

            return file;
        }

        offset += sizeof(entry);
    }

    free(name16);
    free(lfn_entries);

    return NULL;
}
