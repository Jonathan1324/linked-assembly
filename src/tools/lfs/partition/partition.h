#pragma once

#include <stdio.h>
#include <stdint.h>
#include "../disk/disk.h"

typedef unsigned char Partition_Type;
#define PARTITION_MBR   ((Partition_Type)1)

typedef struct Partition {
    Disk* disk;

    uint64_t offset;
    uint64_t size;

    int read_only;
} Partition;

uint64_t Partition_Read(Partition* partition, void* buffer, uint64_t offset, uint64_t size);
uint64_t Partition_Write(Partition* partition, void* buffer, uint64_t offset, uint64_t size);
Partition* Partition_Create(Disk* disk, uint64_t offset, uint64_t size, int read_only);
void Partition_Close(Partition* partition);
