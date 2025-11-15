#include "mbr.h"

MBR_Disk* MBR_CreateDisk(Disk* disk)
{
    if (!disk) return NULL;

    MBR_Disk* mbr = calloc(1, sizeof(MBR_Disk));
    if (!mbr) return NULL;
    mbr->disk = disk;

    mbr->bootsector.signature = 0xAA55;

    if (MBR_WriteBootsector(mbr) != 0) {
        free(mbr);
        return NULL;
    }

    return mbr;
}

MBR_Disk* MBR_OpenDisk(Disk* disk)
{
    if (!disk) return NULL;

    MBR_Disk* mbr = calloc(1, sizeof(MBR_Disk));
    if (!mbr) return NULL;
    mbr->disk = disk;

    if (MBR_ReadBootsector(mbr) != 0) {
        free(mbr);
        return NULL;
    }

    return mbr;
}

void MBR_CloseDisk(MBR_Disk* mbr)
{
    if (!mbr) return;

    if (MBR_WriteBootsector(mbr) != 0) {
        // TODO
    }

    Disk_Close(mbr->disk);
}

int MBR_WriteBootsector(MBR_Disk* mbr)
{
    if (!mbr) return 1;
    uint64_t written = Disk_Write(mbr->disk, &mbr->bootsector, 0, sizeof(MBR_Bootsector));
    if (written != sizeof(MBR_Bootsector)) return 1;
    return 0;
}

int MBR_ReadBootsector(MBR_Disk* mbr)
{
    if (!mbr) return 1;
    uint64_t read = Disk_Read(mbr->disk, &mbr->bootsector, 0, sizeof(MBR_Bootsector));
    if (read != sizeof(MBR_Bootsector)) return 1;
    return 0;
}
