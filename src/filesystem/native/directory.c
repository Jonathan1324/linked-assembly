#include "directory.h"

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

PathType Path_GetType(const char* path)
{
#ifdef _WIN32
    DWORD attrs = GetFileAttributes(path);
    if (attrs == INVALID_FILE_ATTRIBUTES) return TYPE_OTHER;
    if (attrs & FILE_ATTRIBUTE_DIRECTORY) return TYPE_DIR;
    return TYPE_FILE;
#else
    struct stat st;
    if (stat(path, &st) != 0) return TYPE_OTHER;
    if (S_ISDIR(st.st_mode)) return TYPE_DIR;
    if (S_ISREG(st.st_mode)) return TYPE_FILE;
    return TYPE_OTHER;
#endif
}

char** Path_ListDir(const char *dir_path, uint64_t *out_count)
{
    char **list = NULL;
    uint64_t count = 0;

#ifdef _WIN32
    char search_path[1024];
    snprintf(search_path, sizeof(search_path), "%s\\*", dir_path);

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(search_path, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        *out_count = 0;
        return NULL;
    }

    do {
        if (strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0)
            continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, ffd.cFileName); // '/' verwenden

        list = realloc(list, sizeof(char*) * (count + 1));
        list[count++] = strdup(full_path);

    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

#else
    DIR *dir = opendir(dir_path);
    if (!dir) {
        *out_count = 0;
        return NULL;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        list = realloc(list, sizeof(char*) * (count + 1));
        list[count++] = strdup(full_path);
    }

    closedir(dir);
#endif

    *out_count = count;
    return list;
}
