#pragma once

#include <stdio.h>
#include <stdint.h>
#include "../file/file.h"

typedef struct Partition {
    File* file;

    uint64_t offset;
    uint64_t size;
} Partition;

uint64_t Partition_Read(Partition* partition, uint8_t* buffer, uint64_t offset, uint64_t size);
uint64_t Partition_Write(Partition* partition, uint8_t* buffer, uint64_t offset, uint64_t size);
Partition* Partition_Create(File* f, uint64_t offset, uint64_t size);
void Partition_Close(Partition* partition);
