#pragma once
#include <cpp/cpp.h>

HCB

#include <stdint.h>

typedef struct
{
    uint8_t Magic[8];
    uint8_t Version;
    uint8_t HeaderSize;
    uint8_t Padding[6];

    uint32_t CacheHeaderEntrySize;
    uint32_t CacheHeaderEntryCount;
    uint64_t CacheHeaderTableOffset;

    uint64_t NameStringTableSize;
    uint64_t NameStringTableOffset;

    uint64_t ValueStringTableSize;
    uint64_t ValueStringTableOffset;
} __attribute__((packed)) CacheHeader;

typedef struct
{
    uint64_t NameIndex;
    uint64_t NameLength;
    uint64_t ValueIndex;
    uint64_t ValueLength;
} __attribute__((packed)) CacheTableEntry;

typedef struct
{
    char* name;
    uint64_t name_length;
    char* value;
    uint64_t value_length;
} CacheTableEntryBuffer;

typedef struct
{
    CacheHeader* headerBuffer;
    CacheTableEntryBuffer* entries;
} CacheBuffer;

uint64_t ParseCacheFile(const char* path);
void WriteCacheFile(uint64_t buf_ptr, const char* path);
void FreeCacheBuffer(uint64_t buf_ptr);
void AddToCache(uint64_t buf_ptr, const char* name, uint64_t name_length, const char* value, uint64_t value_length);
const char* ReadFromCache(uint64_t buf_ptr, const char* name, uint64_t name_length, uint64_t* value_length);

HCE
