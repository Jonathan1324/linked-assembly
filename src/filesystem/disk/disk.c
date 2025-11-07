#include "disk.h"

#include <stdlib.h>

#ifdef _WIN32
    #define FileSeek _fseeki64
    #define FileTell _ftelli64
    typedef __int64 file_offset_t;
#else
    #define FileSeek fseeko
    #define FileTell ftello
    #include <sys/types.h>
    typedef off_t file_offset_t;
#endif

uint64_t Disk_Read(Disk* disk, void* buffer, uint64_t offset, uint64_t size)
{
    if (!disk || !buffer || offset >= disk->size || size == 0) return 0;

    if (offset != disk->current_position) {
        if (FileSeek(disk->f, offset, SEEK_SET) != 0) return 0;
        disk->current_position = offset;
    }

    if (offset + size > disk->size) size = disk->size - offset;

    uint64_t read = fread(buffer, 1, size, disk->f);
    disk->current_position += read;
    return read;
}

uint64_t Disk_Write(Disk* disk, void* buffer, uint64_t offset, uint64_t size)
{
    if (!disk || !buffer || offset >= disk->size || size == 0) return 0;

    if (offset != disk->current_position) {
        if (FileSeek(disk->f, offset, SEEK_SET) != 0) return 0;
        disk->current_position = offset;
    }

    if (offset + size > disk->size) size = disk->size - offset;

    uint64_t written = fwrite(buffer, 1, size, disk->f);
    if (offset + written > disk->size) disk->size = offset + written;
    disk->current_position += written;
    fflush(disk->f);
    return written;
}

Disk* Disk_CreateFromFile(FILE* raw, uint64_t max_size)
{
    if (FileSeek(raw, 0, SEEK_SET) != 0) return NULL;

    Disk* disk = (Disk*)malloc(sizeof(Disk));
    if (!disk) return NULL;
    disk->f = raw;

    disk->size = max_size;

    disk->current_position = 0;

    return disk;
}

void Disk_Close(Disk* disk)
{
    fclose(disk->f);
    free(disk);
}
