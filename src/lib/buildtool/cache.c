#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// TODO: create better function for hashing
static uint64_t hashFunc(const char* key, uint64_t len) {
    uint64_t h = 14695981039346656037ULL; // FNV offset
    for (uint64_t i = 0; i < len; i++) {
        h ^= (unsigned char)key[i];
        h *= 1099511628211ULL; // FNV prime
    }
    return h;
}

void hashInit(HashMapEntry*** table, uint32_t capacity)
{
    *table = (HashMapEntry**)calloc(capacity, sizeof(HashMapEntry*));
}

void hashInsert(CacheBuffer* buffer, const char* key, uint64_t key_len, uint64_t index)
{
    uint64_t h = hashFunc(key, key_len);
    uint32_t bucket = h % buffer->hash_capacity;

    HashMapEntry* entry = malloc(sizeof(HashMapEntry));
    entry->key     = malloc(key_len);
    memcpy(entry->key, key, key_len);
    entry->key_len = key_len;
    entry->index   = index;

    entry->next    = buffer->hash_table[bucket];
    buffer->hash_table[bucket] = entry;
    buffer->hash_count++;
}

HashMapEntry* hashGet(CacheBuffer* buffer, const char* key, uint64_t key_len)
{
    uint64_t h = hashFunc(key, key_len);
    uint32_t bucket = h % buffer->hash_capacity;

    HashMapEntry* e = buffer->hash_table[bucket];
    while (e)
    {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0)
            return e;
        e = e->next;
    }
    return NULL;
}

uint64_t hashLookup(CacheBuffer* buffer, const char* key, uint64_t key_len) {
    HashMapEntry* e = hashGet(buffer, key, key_len);
    return e ? e->index : UINT64_MAX;
}

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

uint8_t createBuffer(CacheBuffer* buffer)
{
    if (!buffer) return 1;

    buffer->headerBuffer = createHeader();
    if (!buffer->headerBuffer)
    {
        free(buffer);
        return 1;
    }

    buffer->headerBuffer->CacheHeaderTableOffset = 0x40;
    buffer->headerBuffer->NameStringTableOffset = 0x40;
    buffer->headerBuffer->ValueStringTableOffset = 0x40;

    buffer->entries = NULL;

    return 0;
}

uint8_t readBuffer(CacheBuffer* buffer, FILE* f)
{
    if (!f || !buffer) return 1;

    // Read header
    CacheHeader* header = (CacheHeader*)malloc(sizeof(CacheHeader));
    if (!header) return 1;

    if (fread(header, sizeof(CacheHeader), 1, f) != 1)
    {
        free(header);
        return 1;
    }

    uint8_t m[8] = MAGIC;
    if (memcmp(header->Magic, m, sizeof(m)) != 0)
    {
        free(header);
        return 1;
    }

    if (header->Version != 1)
    {
        // TODO
        free(header);
        return 1;
    }

    if (header->HeaderSize != sizeof(CacheHeader))
    {
        free(header);
        return 1;
    }

    // Read entries
    CacheTableEntry* raw_entries = (CacheTableEntry*)malloc(sizeof(CacheTableEntry) * header->CacheHeaderEntryCount);
    if (!raw_entries)
    {
        free(header);
        return 1;
    }
    fseek(f, header->CacheHeaderTableOffset, SEEK_SET);
    if (fread(raw_entries, sizeof(CacheTableEntry), header->CacheHeaderEntryCount, f) != header->CacheHeaderEntryCount)
    {
        free(raw_entries);
        free(header);
        return 1;
    }

    // Read stringtables
    char* name_table = (char*)malloc(header->NameStringTableSize);
    if (!name_table)
    {
        free(raw_entries);
        free(header);
        return 1;
    }
    fseek(f, header->NameStringTableOffset, SEEK_SET);
    if (fread(name_table, 1, header->NameStringTableSize, f) != header->NameStringTableSize)
    {
        free(name_table);
        free(raw_entries);
        free(header);
        return 1;
    }

    char* value_table = (char*)malloc(header->ValueStringTableSize);
    if (!value_table)
    {
        free(name_table);
        free(raw_entries);
        free(header);
        return 1;
    }
    fseek(f, header->ValueStringTableOffset, SEEK_SET);
    if (fread(value_table, 1, header->ValueStringTableSize, f) != header->ValueStringTableSize)
    {
        free(value_table);
        free(name_table);
        free(raw_entries);
        free(header);
        return 1;
    }

    // Create buffer
    buffer->headerBuffer = header;
    buffer->entries = (CacheTableEntryBuffer*)malloc(sizeof(CacheTableEntryBuffer) * header->CacheHeaderEntryCount);
    if (!buffer->entries) {
        free(buffer);
        free(value_table);
        free(name_table);
        free(raw_entries);
        free(header);
        return 1;
    }

    // Fill entry names
    for (uint32_t i = 0; i < header->CacheHeaderEntryCount; ++i)
    {
        uint64_t name_len  = raw_entries[i].NameLength;
        uint64_t value_len = raw_entries[i].ValueLength;

        buffer->entries[i].name = (char*)malloc(name_len);
        buffer->entries[i].name_length = name_len;
        memcpy(buffer->entries[i].name, &name_table[raw_entries[i].NameIndex], name_len);

        buffer->entries[i].value = (char*)malloc(value_len);
        buffer->entries[i].value_length = value_len;
        memcpy(buffer->entries[i].value, &value_table[raw_entries[i].ValueIndex], value_len);

        hashInsert(buffer, buffer->entries[i].name, buffer->entries[i].name_length, i);
    }

    free(name_table);
    free(value_table);
    free(raw_entries);

    return 0;
}

uint64_t ParseCacheFile(const char* path)
{
    CacheBuffer* buffer = (CacheBuffer*)malloc(sizeof(CacheBuffer));

    buffer->hash_capacity = 128;
    buffer->hash_count = 0;
    hashInit(&buffer->hash_table, buffer->hash_capacity);

    FILE* f = fopen(path, "rb");
    if (f)
    {
        readBuffer(buffer, f);
        fclose(f);
    }
    else
    {
        createBuffer(buffer);
    }

    return (uint64_t)(uintptr_t)buffer;
}

void AddToCache(uint64_t buf_ptr, const char* name, uint64_t name_length, const char* value, uint64_t value_length)
{
    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer || !name || !value) return;

    uint32_t count = buffer->headerBuffer->CacheHeaderEntryCount;

    // Check, if entry already exists
    uint64_t index = hashLookup(buffer, name, name_length);
    if (index != UINT64_MAX)
    {
        free(buffer->entries[index].value);
        buffer->entries[index].value = (char*)malloc(value_length);
        memcpy(buffer->entries[index].value, value, value_length);
        buffer->entries[index].value_length = value_length;
        return;
    }

    // new entry
    CacheTableEntryBuffer* new_entries = (CacheTableEntryBuffer*)realloc(
        buffer->entries,
        sizeof(CacheTableEntryBuffer) * (count + 1)
    );
    if (!new_entries) return;

    buffer->entries = new_entries;

    buffer->entries[count].name = (char*)malloc(name_length);
    memcpy(buffer->entries[count].name, name, name_length);
    buffer->entries[count].name_length = name_length;

    buffer->entries[count].value = (char*)malloc(value_length);
    memcpy(buffer->entries[count].value, value, value_length);
    buffer->entries[count].value_length = value_length;

    hashInsert(buffer, buffer->entries[count].name, name_length, count);

    buffer->headerBuffer->CacheHeaderEntryCount++;
}

const char* ReadFromCache(uint64_t buf_ptr, const char* name, uint64_t name_length, uint64_t* value_length)
{
    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer || !buffer->headerBuffer || !buffer->entries || !name) return NULL;

    uint64_t index = hashLookup(buffer, name, name_length);
    if (index != UINT64_MAX) {
        if (value_length) *value_length = buffer->entries[index].value_length;
        return buffer->entries[index].value;
    }

    if (value_length) *value_length = 0;
    return NULL;
}

void WriteCacheFile(uint64_t buf_ptr, const char* path)
{
    if (!path) return;

    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer || !buffer->headerBuffer) return;

    FILE* f = fopen(path, "wb");
    if (!f) return;

    CacheHeader header = *buffer->headerBuffer;
    uint32_t entry_count = header.CacheHeaderEntryCount;

    uint32_t used_count = 0;
    uint32_t name_table_size = 0;
    uint32_t value_table_size = 0;

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        used_count++;
        if (buffer->entries[i].name) name_table_size += buffer->entries[i].name_length;
        if (buffer->entries[i].value) value_table_size += buffer->entries[i].value_length;
    }

    header.CacheHeaderEntryCount = used_count;
    header.NameStringTableSize = name_table_size;
    header.ValueStringTableSize = value_table_size;

    header.CacheHeaderTableOffset = sizeof(CacheHeader);
    header.NameStringTableOffset = header.CacheHeaderTableOffset + used_count * sizeof(CacheTableEntry);
    header.ValueStringTableOffset = header.NameStringTableOffset + name_table_size;

    fwrite(&header, sizeof(CacheHeader), 1, f);

    uint64_t current_name_index = 0;
    uint64_t current_value_index = 0;
    for (uint32_t i = 0; i < entry_count; ++i)
    {
        CacheTableEntry entry;
        entry.NameIndex = current_name_index;
        entry.NameLength  = buffer->entries[i].name_length;
        entry.ValueIndex = current_value_index;
        entry.ValueLength = buffer->entries[i].value_length;
        fwrite(&entry, sizeof(entry), 1, f);

        if (buffer->entries[i].name) current_name_index += buffer->entries[i].name_length;
        if (buffer->entries[i].value) current_value_index += buffer->entries[i].value_length;
    }

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (buffer->entries[i].name)
        {
            fwrite(buffer->entries[i].name, buffer->entries[i].name_length, 1, f);
        }
    }

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        if (buffer->entries[i].value)
        {
            fwrite(buffer->entries[i].value, buffer->entries[i].value_length, 1, f);
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

    if (buffer->hash_table) {
        for (uint32_t i = 0; i < buffer->hash_capacity; i++)
        {
            HashMapEntry* entry = buffer->hash_table[i];
            while (entry)
            {
                HashMapEntry* next = entry->next;
                if (entry->key) free(entry->key);
                free(entry);
                entry = next;
            }
        }
        free(buffer->hash_table);
    }

    if (buffer->headerBuffer) free (buffer->headerBuffer);

    if (buffer) free(buffer);
}

void CleanCache(uint64_t buf_ptr)
{
    CacheBuffer* buffer = (CacheBuffer*)(uintptr_t)buf_ptr;
    if (!buffer) return;

    // TODO
}
