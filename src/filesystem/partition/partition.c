#include "partition.h"

#include <stdlib.h>

uint64_t Partition_Read(Partition* partition, uint8_t* buffer, uint64_t offset, uint64_t size)
{
    if (!buffer || !partition || offset >= partition->size || size == 0) return 0;

    if (offset + size > partition->size) size = partition->size - offset;

    return File_Read(partition->file, buffer, partition->offset + offset, size);
}

uint64_t Partition_Write(Partition* partition, uint8_t* buffer, uint64_t offset, uint64_t size)
{
    if (!buffer || !partition || offset >= partition->size || size == 0) return 0;

    if (offset + size > partition->size) size = partition->size - offset;

    return File_Write(partition->file, buffer, partition->offset + offset, size);
}

Partition* Partition_Create(File* f, uint64_t offset, uint64_t size)
{
    if (!f) return NULL;
    Partition* partition = (Partition*)malloc(sizeof(Partition));
    if (!partition) return NULL;
    partition->file = f;
    partition->offset = offset;
    partition->size = size;
    return partition;
}

void Partition_Close(Partition* partition)
{
    free(partition);
}
