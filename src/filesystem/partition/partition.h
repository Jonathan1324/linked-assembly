#pragma once

#include <stdio.h>
#include <stdint.h>
#include "../disk/disk.h"

typedef struct Partition {
    Disk* disk;

    uint64_t offset;
    uint64_t size;
} Partition;

uint64_t Partition_Read(Partition* partition, uint8_t* buffer, uint64_t offset, uint64_t size);
uint64_t Partition_Write(Partition* partition, uint8_t* buffer, uint64_t offset, uint64_t size);
Partition* Partition_Create(Disk* disk, uint64_t offset, uint64_t size);
void Partition_Close(Partition* partition);
