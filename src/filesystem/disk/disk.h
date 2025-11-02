#pragma once

#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <stdint.h>

typedef struct Disk {
    FILE* f;
    
    uint64_t current_position;
    uint64_t size;
} Disk;

uint64_t Disk_Read(Disk* disk, uint8_t* buffer, uint64_t offset, uint64_t size);
uint64_t Disk_Write(Disk* disk, uint8_t* buffer, uint64_t offset, uint64_t size);
Disk* Disk_CreateFromFile(FILE* raw);
void Disk_Close(Disk* disk);
