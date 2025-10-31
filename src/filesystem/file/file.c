#include "file.h"

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

uint64_t File_Read(File* f, uint8_t* buffer, uint64_t offset, uint64_t size)
{
    if (!f || !buffer || offset >= f->size || size == 0) return 0;

    if (offset != f->current_position) {
        if (FileSeek(f->f, offset, SEEK_SET) != 0) return 0;
        f->current_position = offset;
    }

    if (offset + size > f->size) size = f->size - offset;
    uint64_t read = fread(buffer, 1, size, f->f);
    f->current_position += read;
    return read;
}

uint64_t File_Write(File* f, uint8_t* buffer, uint64_t offset, uint64_t size)
{
    if (!f || !buffer || size == 0) return 0;

    if (offset != f->current_position) {
        if (FileSeek(f->f, offset, SEEK_SET) != 0) return 0;
        f->current_position = offset;
    }

    uint64_t written = fwrite(buffer, 1, size, f->f);
    if (offset + written > f->size) f->size = offset + written;
    f->current_position += written;
    fflush(f->f);
    return written;
}

File* File_Create(FILE* raw)
{
    File* f = (File*)malloc(sizeof(File));
    if (!f) return NULL;
    f->f = raw;

    int r = FileSeek(raw, 0, SEEK_END);
    if (FileSeek(raw, 0, SEEK_END) != 0) {
        free(f);
        return NULL;
    }
    file_offset_t size = FileTell(raw);
    if (FileSeek(raw, 0, SEEK_SET) != 0) {
        free(f);
        return NULL;
    }
    if (size < 0) {
        free(f);
        return NULL;
    }
    f->size = (uint64_t)size;

    f->current_position = 0;

    return f;
}

void File_Close(File* f)
{
    fclose(f->f);
    free(f);
}
