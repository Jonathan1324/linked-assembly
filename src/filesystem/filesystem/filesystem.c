#include "filesystem.h"
#include <stdlib.h>
#include <string.h>
#include "../native/directory.h"

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

Filesystem_File* Filesystem_OpenPath(Filesystem_File* current_path, const char* path, int create_file, int create_parents, int is_directory, int is_hidden, int is_system, int64_t creation, int64_t last_modification, int64_t last_access)
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
            if (create_file) new_entry = Filesystem_OpenEntry(current_path, name, is_directory, is_hidden, is_system, creation, last_modification, last_access);
            else             new_entry = Filesystem_FindEntry(current_path, name);
        }
        if (!new_entry) {
            Filesystem_CloseEntry(current_path);
            return NULL;
        }
        if (i > 0) Filesystem_CloseEntry(current_path);
        current_path = new_entry;
        free(name);
    }

    free(entries);

    return current_path;
}

void Filesystem_CloseEntry(Filesystem_File* file)
{
    if (!file) return;
    FAT_CloseEntry(file->fat_f);
    free(file);
}

uint64_t Filesystem_ReadFromFile(Filesystem_File* file, uint64_t offset, uint8_t* buffer, uint64_t size)
{
    if (!file) return 0;
    return FAT_ReadFromFile(file->fat_f, (uint32_t)offset, buffer, (uint32_t)size);
}

uint64_t Filesystem_WriteToFile(Filesystem_File* file, uint64_t offset, uint8_t* buffer, uint64_t size)
{
    if (!file) return 0;
    return FAT_WriteToFile(file->fat_f, (uint32_t)offset, buffer, (uint32_t)size);
}


// TODO: Don't Override
int Filesystem_SyncPathsToFS(Filesystem_File* dir, const char* path, const char* o_path)
{
    if (!dir || !path || !o_path) return 1;

    PathType type = Path_GetType(o_path);
    int is_hidden = 0; //TODO
    int is_system = 0; //TODO

    int64_t creation = 0;
    int64_t last_modification = 0;
    int64_t last_access = 0;

    if (type == TYPE_FILE) {
        Filesystem_File* fs_f = Filesystem_OpenPath(dir, path, 1, 1, 0, is_hidden, is_system, creation, last_modification, last_access);
        FILE* o_f = fopen(o_path, "rb");

        if (!fs_f || !o_f) {
            if (fs_f) Filesystem_CloseEntry(fs_f);
            if (o_f) fclose(o_f);
            fprintf(stderr, "Couldn't add file '%s' as '%s'\n", o_path, path);
            return 1;
        }

        char buffer[512];
        uint64_t offset = 0;
        uint64_t read = 1;
        while (read) {
            read = (uint64_t)fread(buffer, 1, 512, o_f);
            if (Filesystem_WriteToFile(fs_f, offset, buffer, read) != read) {
                fprintf(stderr, "Couldn't write %llu bytes to %s\n", read, path);
                fclose(o_f);
                Filesystem_CloseEntry(fs_f);
                return 1;
            }
            offset += read;
        }

        fclose(o_f);
        Filesystem_CloseEntry(fs_f);

    } else if (type == TYPE_DIR) {
        uint64_t sub_entry_count;
        char** sub_entries = Path_ListDir(o_path, &sub_entry_count);
        if (!sub_entries) {
            fprintf(stderr, "Couldn't get entries of '%s'\n", o_path);
            return;
        }

        for (uint64_t i = 0; i < sub_entry_count; i++) {
            char* entry = sub_entries[i];

            char* name = strrchr(entry, '/');
            uint64_t path_len = strlen(path);
            uint64_t name_len = strlen(name);
            char* new_path = (char*)malloc(path_len + name_len + 1);
            if (!new_path) {
                printf("Warning: Couldn't sync '%s' to '%s'", entry, new_path);
                free(entry);
                continue;
            }
            strcpy(new_path, path);
            strcpy(new_path + path_len, name);
            new_path[path_len + name_len] = '\0';

            if (Filesystem_SyncPathsToFS(dir, new_path, entry) != 0) {
                printf("Warning: Couldn't sync '%s' to '%s'", entry, new_path);
                free(entry);
                continue;
            }

            free(new_path);
            free(entry);
        }

        free(sub_entries);
    }

    return 0;
}

// TODO: Don't Override
int Filesystem_SyncPathsFromFS(Filesystem_File* dir, const char* path, const char* o_path)
{
    if (!dir || !path || !o_path) return 1;

    Filesystem_File* tmp_f = Filesystem_OpenPath(dir, path, 0, 0, 0, 0, 0, 0, 0, 0);
    if (!tmp_f) return 1;
    PathType type = tmp_f->fat_f->is_directory ? TYPE_DIR : TYPE_FILE;
    Filesystem_CloseEntry(tmp_f);
    int is_hidden = 0; //TODO
    int is_system = 0; //TODO

    int64_t creation = 0;
    int64_t last_modification = 0;
    int64_t last_access = 0;

    if (type == TYPE_FILE) {
        Filesystem_File* fs_f = Filesystem_OpenPath(dir, path, 0, 0, 0, is_hidden, is_system, creation, last_modification, last_access);
        FILE* o_f = fopen(o_path, "w+b"); //truncate

        if (!fs_f || !o_f) {
            if (fs_f) Filesystem_CloseEntry(fs_f);
            if (o_f) fclose(o_f);
            fprintf(stderr, "Couldn't add file '%s' as '%s'\n", o_path, path);
            return 1;
        }

        char buffer[512];
        uint64_t offset = 0;
        uint64_t read = 1;
        while (read) {
            read = Filesystem_ReadFromFile(fs_f, offset, buffer, 512);
            if (fwrite(buffer, 1, read, o_f) != read) {
                fprintf(stderr, "Couldn't write %llu bytes to %s\n", read, o_path);
                fclose(o_f);
                Filesystem_CloseEntry(fs_f);
                return 1;
            }
            offset += read;
        }

        fclose(o_f);
        Filesystem_CloseEntry(fs_f);

    } else if (type == TYPE_DIR) {
        uint64_t sub_entry_count;
        char** sub_entries = Path_ListDir(o_path, &sub_entry_count);
        if (!sub_entries) {
            fprintf(stderr, "Couldn't get entries of '%s'\n", o_path);
            return;
        }

        for (uint64_t i = 0; i < sub_entry_count; i++) {
            char* entry = sub_entries[i];

            char* name = strrchr(entry, '/');
            uint64_t path_len = strlen(path);
            uint64_t name_len = strlen(name);
            char* new_path = (char*)malloc(path_len + name_len + 1);
            if (!new_path) {
                printf("Warning: Couldn't sync '%s' to '%s'", entry, new_path);
                free(entry);
                continue;
            }
            strcpy(new_path, path);
            strcpy(new_path + path_len, name);
            new_path[path_len + name_len] = '\0';

            if (Filesystem_SyncPathsFromFS(dir, new_path, entry) != 0) {
                printf("Warning: Couldn't sync '%s' to '%s'", entry, new_path);
                free(entry);
                continue;
            }

            free(new_path);
            free(entry);
        }

        free(sub_entries);
    }

    return 0;
}
