#include "fat.h"

uint32_t FAT12_AddDirectoryEntry(FAT12_File* directory, FAT_DirectoryEntry* entry)
{
    if (!entry || !directory || !directory->is_directory) return 0xFFFF; // TODO: error

    uint32_t offset = 0;
    FAT_DirectoryEntry tmp;

    while (FAT12_ReadFromFileRaw(directory, offset, (uint8_t*)&tmp, sizeof(tmp)) == sizeof(tmp)) {
        if (tmp.name[0] == 0x00 || tmp.name[0] == FAT_ENTRY_DELETED) {
            if (FAT12_WriteToFileRaw(directory, offset, (uint8_t*)entry, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry))
                return 0xFFFF;
            return offset;
        }
        offset += sizeof(FAT_DirectoryEntry);
    }

    // no empty slot
    if (FAT12_WriteToFileRaw(directory, offset, (uint8_t*)entry, sizeof(FAT_DirectoryEntry)) != sizeof(FAT_DirectoryEntry))
        return 0xFFFF;

    return offset;
}

int FAT12_AddDotsToDirectory(FAT12_File* directory, FAT12_File* parent)
{
    if (!directory || !directory->is_directory) return 1;

    FAT_DirectoryEntry dot = {0};
    dot.name[0] = '.';
    dot.name[1] = ' ';
    dot.name[2] = ' ';
    dot.name[3] = ' ';
    dot.name[4] = ' ';
    dot.name[5] = ' ';
    dot.name[6] = ' ';
    dot.name[7] = ' ';
    dot.ext[0] = ' ';
    dot.ext[1] = ' ';
    dot.ext[2] = ' ';
    dot.attribute = FAT_ENTRY_DIRECTORY;
    dot.first_cluster = directory->first_cluster;
    uint32_t rel_offset_dot = FAT12_AddDirectoryEntry(directory, &dot);

    FAT_DirectoryEntry dotdot = {0};
    dotdot.name[0] = '.';
    dotdot.name[1] = '.';
    dotdot.name[2] = ' ';
    dotdot.name[3] = ' ';
    dotdot.name[4] = ' ';
    dotdot.name[5] = ' ';
    dotdot.name[6] = ' ';
    dotdot.name[7] = ' ';
    dotdot.ext[0] = ' ';
    dotdot.ext[1] = ' ';
    dotdot.ext[2] = ' ';
    dotdot.attribute = FAT_ENTRY_DIRECTORY;
    dotdot.first_cluster = parent->first_cluster;
    uint32_t rel_offset_dotdot = FAT12_AddDirectoryEntry(directory, &dotdot);

    return 0;
}
