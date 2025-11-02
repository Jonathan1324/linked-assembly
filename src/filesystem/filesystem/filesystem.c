#include "filesystem.h"
#include <stdlib.h>
#include <string.h>

char** SeparatePaths(const char* path, uint32_t* out_count)
{
    if (!path) return NULL;

    uint32_t count = 0;
    const char* p = path;
    while (*p) {
        if (*p == '/') count++;
        p++;
    }
    count++; // last

    char** parts = (char**)calloc(count + 1, sizeof(char*));
    if (!parts) return NULL;

    uint32_t i = 0;
    const char* start = path;
    for (p = path; *p; p++) {
        if (*p == '/') {
            uint64_t len = p - start;
            if (len > 0) {
                parts[i] = (char*)malloc(len + 1);
                memcpy(parts[i], start, len);
                parts[i][len] = '\0';
                i++;
            }
            start = p + 1;
        }
    }
    if (*start) {
        uint64_t len = strlen(start);
        parts[i] = (char*)malloc(len + 1);
        memcpy(parts[i], start, len + 1);
        i++;
    }

    *out_count = i;
    parts[i] = NULL;
    return parts;
}

Filesystem* Filesystem_CreateFromFAT(FAT_Filesystem* fat_fs)
{
    Filesystem* fs = (Filesystem*)malloc(sizeof(Filesystem));
    if (!fs) return NULL;
    fs->fat_fs = fat_fs;

    fs->static_root.fs = fs;
    fs->static_root.fat_f = fat_fs->root;
    fs->root = &fs->static_root;

    return fs;
}

void Filesystem_Close(Filesystem* fs)
{
    if (!fs) return;
    FAT_CloseFilesystem(fs->fat_fs);
    free(fs);
}

Filesystem_File* Filesystem_CreateEntry(Filesystem_File* parent, const char* name,  int is_directory, int is_hidden, int is_system, int64_t creation, int64_t last_modification, int64_t last_access)
{
    if (!parent || !name) return NULL;
    Filesystem_File* file = (Filesystem_File*)malloc(sizeof(Filesystem_File));
    if (!file) return NULL;
    file->fs = parent->fs;

    //TODO: dynamically choose if to use lfn
    file->fat_f = FAT_CreateEntry(parent->fat_f, name, is_directory, is_hidden, is_system, creation, last_modification, last_access, 1);
    if (!file->fat_f) {
        free(file);
        return NULL;
    }

    return file;
}

Filesystem_File* Filesystem_FindEntry(Filesystem_File* parent, const char* name)
{
    if (!parent || !name) return NULL;
    Filesystem_File* file = (Filesystem_File*)malloc(sizeof(Filesystem_File));
    if (!file) return NULL;
    file->fs = parent->fs;

    file->fat_f = FAT_FindEntry(parent->fat_f, name);
    if (!file->fat_f) {
        free(file);
        return NULL;
    }

    return file;
}

Filesystem_File* Filesystem_OpenEntry(Filesystem_File* parent, const char* name, int is_directory, int is_hidden, int is_system, int64_t creation, int64_t last_modification, int64_t last_access)
{
    if (!parent || !name) return NULL;
    Filesystem_File* entry = Filesystem_FindEntry(parent, name);
    if (entry) return entry;
    return Filesystem_CreateEntry(parent, name, is_directory, is_hidden, is_system, creation, last_modification, last_access);
}

Filesystem_File* Filesystem_OpenPath(Filesystem_File* current_path, const char* path, int create_parents, int is_directory, int is_hidden, int is_system, int64_t creation, int64_t last_modification, int64_t last_access)
{
    uint32_t path_out_count;
    char** entries = SeparatePaths(path, &path_out_count);
    if (!entries) return NULL;

    for (uint32_t i = 0; i < path_out_count; i++) {
        char* name = entries[i];
        if (!name) {
            Filesystem_CloseEntry(current_path);
            return NULL;
        }

        Filesystem_File* new_entry;
        if (i+1 < path_out_count) {
            if (create_parents) new_entry = Filesystem_OpenEntry(current_path, name, 1, is_hidden, is_system, creation, last_modification, last_access);
            else                new_entry = Filesystem_FindEntry(current_path, name);
        } else {
            new_entry = Filesystem_OpenEntry(current_path, name, is_directory, is_hidden, is_system, creation, last_modification, last_access);
        }
        if (!new_entry) {
            Filesystem_CloseEntry(current_path);
            return NULL;
        }
        if (i > 0) Filesystem_CloseEntry(current_path);
        current_path = new_entry;
    }

    return current_path;
}

void Filesystem_CloseEntry(Filesystem_File* file)
{
    if (!file) return;
    FAT_CloseEntry(file->fat_f);
    free(file);
}
