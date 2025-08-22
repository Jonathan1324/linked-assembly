#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAGIC {0x7F, 'B', 'T', 'C', 'A', 'C', 'H', 'E'}

CacheHeader* createHeader()
{
    CacheHeader* header = (CacheHeader*)malloc(sizeof(CacheHeader));
    if (!header) return NULL;

    uint8_t m[8] = MAGIC;
    memcpy(header->Magic, m, sizeof(m));

    header->Version = 1;
    header->HeaderSize = sizeof(CacheHeader);

    memset(header->Padding, 0, sizeof(header->Padding));

    header->CacheHeaderEntrySize = sizeof(CacheTableEntry);
    header->CacheHeaderEntryCount = 0;

    header->NameStringTableSize = 0;

    header->ValueStringTableSize = 0;

    return header;
}

CacheBuffer* createBuffer()
{
    CacheBuffer* buffer = (CacheBuffer*)malloc(sizeof(CacheBuffer));
    if (!buffer) return 0;

    buffer->headerBuffer = createHeader();
    if (!buffer->headerBuffer)
    {
        free(buffer);
        return 0;
    }

    buffer->headerBuffer->CacheHeaderTableOffset = 0x40;
    buffer->headerBuffer->NameStringTableOffset = 0x40;
    buffer->headerBuffer->ValueStringTableOffset = 0x40;

    buffer->entries = NULL;

    return buffer;
}

CacheBuffer* readBuffer(FILE* f)
{
    if (!f) return NULL;

    // Read header
    CacheHeader* header = (CacheHeader*)malloc(sizeof(CacheHeader));
    if (!header) return NULL;

    if (fread(header, sizeof(CacheHeader), 1, f) != 1)
    {
        free(header);
        return NULL;
    }

    uint8_t m[8] = MAGIC;
    if (memcmp(header->Magic, m, sizeof(m)) != 0)
    {
        free(header);
        return NULL;
    }

    if (header->Version != 1)
    {
        // TODO
        free(header);
        return NULL;
    }

    if (header->HeaderSize != sizeof(CacheHeader))
    {
        free(header);
        return NULL;
    }

    // read entries
    CacheTableEntry* raw_entries = (CacheTableEntry*)malloc(sizeof(CacheTableEntry) * header->CacheHeaderEntryCount);
    if (!raw_entries)
    {
        free(header);
        return NULL;
    }
    fseek(f, header->CacheHeaderTableOffset, SEEK_SET);
    if (fread(raw_entries, sizeof(CacheTableEntry), header->CacheHeaderEntryCount, f) != header->CacheHeaderEntryCount)
    {
        free(raw_entries);
        free(header);
        return NULL;
    }

    // read stringtables
    char* name_table = (char*)malloc(header->NameStringTableSize);
    if (!name_table)
    {
        free(raw_entries);
        free(header);
        return NULL;
    }
    fseek(f, header->NameStringTableOffset, SEEK_SET);
    if (fread(name_table, 1, header->NameStringTableSize, f) != header->NameStringTableSize)
    {
        free(name_table);
        free(raw_entries);
        free(header);
        return NULL;
    }

    char* value_table = (char*)malloc(header->ValueStringTableSize);
    if (!value_table)
    {
        free(name_table);
        free(raw_entries);
        free(header);
        return NULL;
    }
    fseek(f, header->ValueStringTableOffset, SEEK_SET);
    if (fread(value_table, 1, header->ValueStringTableSize, f) != header->ValueStringTableSize)
    {
        free(value_table);
        free(name_table);
        free(raw_entries);
        free(header);
        return NULL;
    }

    // Create buffer
    CacheBuffer* buffer = (CacheBuffer*)malloc(sizeof(CacheBuffer));
    if (!buffer)
    {
        free(value_table);
        free(name_table);
        free(raw_entries);
        free(header);
        return NULL;
    }
    buffer->headerBuffer = header;
    buffer->entries = (CacheTableEntryBuffer*)malloc(sizeof(CacheTableEntryBuffer) * header->CacheHeaderEntryCount);
    if (!buffer->entries) {
        free(buffer);
        free(value_table);
        free(name_table);
        free(raw_entries);
        free(header);
        return NULL;
    }

    // Fill entry names
    for (uint32_t i = 0; i < header->CacheHeaderEntryCount; ++i)
    {
        uint32_t name_start = raw_entries[i].NameIndex;
        uint32_t name_end = (i + 1 < header->CacheHeaderEntryCount) ? raw_entries[i + 1].NameIndex : header->NameStringTableSize;
        uint32_t name_len = name_end - name_start;

        uint32_t value_start = raw_entries[i].ValueIndex;
        uint32_t value_end = (i + 1 < header->CacheHeaderEntryCount) ? raw_entries[i + 1].ValueIndex : header->ValueStringTableSize;
        uint32_t value_len = value_end - value_start;

        buffer->entries[i].name = (char*)malloc(name_len + 1);
        buffer->entries[i].value = (char*)malloc(value_len + 1);
        memcpy(buffer->entries[i].name, &name_table[name_start], name_len);
        memcpy(buffer->entries[i].value, &value_table[value_start], value_len);
        buffer->entries[i].name[name_len] = '\0';
        buffer->entries[i].value[value_len] = '\0';
    }

    free(name_table);
    free(value_table);
    free(raw_entries);

    return buffer;
}

void AddToCache(uint64_t buf_ptr, const char* name, const char* value)
{
    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer || !name || !name) return;

    uint32_t count = buffer->headerBuffer->CacheHeaderEntryCount;

    buffer->entries = (CacheTableEntryBuffer*)realloc(
        buffer->entries,
        sizeof(CacheTableEntryBuffer) * (count + 1)
    );

    buffer->entries[count].name = strdup(name);
    buffer->entries[count].value = strdup(value);

    buffer->headerBuffer->CacheHeaderEntryCount++;
}

const char* ReadFromCache(uint64_t buf_ptr, const char* name)
{
    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer || !buffer->headerBuffer || !buffer->entries || !name) return NULL;

    for (uint32_t i = 0; i < buffer->headerBuffer->CacheHeaderEntryCount; i++)
    {
        if (strcmp(buffer->entries[i].name, name) == 0)
        {
            return buffer->entries[i].value;
        }
    }

    return NULL;
}

uint64_t ParseCacheFile(const char* path)
{
    uintptr_t ptr = 0;
    FILE* f = fopen(path, "rb");
    if (f)
    {
        ptr = (uintptr_t)readBuffer(f);
        fclose(f);
    }
    else
    {
        ptr = (uintptr_t)createBuffer();
    }

    return (uint64_t)ptr;
}

void WriteCacheFile(uint64_t buf_ptr, const char* path)
{
    if (!path) return;

    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer || !buffer->headerBuffer) return;

    FILE* f = fopen(path, "wb");
    if (!f) return;

    CacheHeader* header = buffer->headerBuffer;
    uint32_t entry_count = header->CacheHeaderEntryCount;

    uint32_t name_table_size = 0;
    uint32_t value_table_size = 0;

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (buffer->entries[i].name) name_table_size += strlen(buffer->entries[i].name) + 1;
        if (buffer->entries[i].value) value_table_size += strlen(buffer->entries[i].value) + 1;
    }

    header->NameStringTableSize = name_table_size;
    header->ValueStringTableSize = value_table_size;

    header->CacheHeaderTableOffset = sizeof(CacheHeader);
    header->NameStringTableOffset = header->CacheHeaderTableOffset + entry_count * sizeof(CacheTableEntry);
    header->ValueStringTableOffset = header->NameStringTableOffset + name_table_size;

    fwrite(buffer->headerBuffer, sizeof(CacheHeader), 1, f);

    uint64_t current_name_index = 0;
    uint64_t current_value_index = 0;
    for (uint32_t i = 0; i < entry_count; ++i)
    {
        CacheTableEntry entry;
        entry.NameIndex = current_name_index;
        entry.ValueIndex = current_value_index;
        fwrite(&entry, sizeof(entry), 1, f);

        if (buffer->entries[i].name) current_name_index += strlen(buffer->entries[i].name) + 1;
        if (buffer->entries[i].value) current_value_index += strlen(buffer->entries[i].value) + 1;
    }

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (buffer->entries[i].name)
        {
            fwrite(buffer->entries[i].name, strlen(buffer->entries[i].name) + 1, 1, f);
        }
    }

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (buffer->entries[i].value)
        {
            fwrite(buffer->entries[i].value, strlen(buffer->entries[i].value) + 1, 1, f);
        }
    }

    fclose(f);
}

void FreeCacheBuffer(uint64_t buf_ptr)
{
    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer) return;

    if (buffer->entries) {
        uint32_t count = buffer->headerBuffer ? buffer->headerBuffer->CacheHeaderEntryCount : 0;
        for (uint32_t i = 0; i < count; ++i) {
            if (buffer->entries[i].name) free(buffer->entries[i].name);
            if (buffer->entries[i].value) free(buffer->entries[i].value);
        }
        free(buffer->entries);
    }

    if (buffer->headerBuffer) free (buffer->headerBuffer);

    if (buffer) free(buffer);
}