#pragma once

#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdint.h>

typedef struct Disk {
    FILE* f;
    
    uint64_t current_position;
    uint64_t size;
} Disk;

uint64_t Disk_Read(Disk* disk, void* buffer, uint64_t offset, uint64_t size);
uint64_t Disk_Write(Disk* disk, void* buffer, uint64_t offset, uint64_t size);
Disk* Disk_CreateFromFile(FILE* raw, uint64_t max_size);
void Disk_Close(Disk* disk);
