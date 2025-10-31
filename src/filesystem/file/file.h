#pragma once

#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdint.h>

typedef struct File {
    FILE* f;
    uint64_t current_position;
    uint64_t size;
} File;

uint64_t File_Read(File* f, uint8_t* buffer, uint64_t offset, uint64_t size);
uint64_t File_Write(File* f, uint8_t* buffer, uint64_t offset, uint64_t size);
File* File_Create(FILE* raw);
void File_Close(File* f);
