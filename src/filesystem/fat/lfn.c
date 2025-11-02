#include "fat.h"

#include <string.h>
#include <stdlib.h>

static int FAT_LFN_OrderCmp(const void* a, const void* b)
{
    const FAT_LFNEntry* A = (const FAT_LFNEntry*)a;
    const FAT_LFNEntry* B = (const FAT_LFNEntry*)b;
    return (A->order & 0x1F) - (B->order & 0x1F);
}

FAT_LFNEntry* FAT_CreateLFNEntries(const char* name, uint32_t* out_count, uint8_t checksum)
{
    uint16_t* u16 = NULL;
    uint32_t u16len = utf8_to_utf16(name, &u16);
    if (!u16) return NULL;

    uint32_t entries = (u16len + 12) / 13;
    if (entries == 0) entries = 1;

    FAT_LFNEntry* arr = calloc(entries, sizeof(FAT_LFNEntry));
    if (!arr) return NULL;

    for (uint32_t i = 0; i < entries; i++) {
        FAT_LFNEntry* e = &arr[i];

        e->attr = 0x0F;
        e->reserved = 0;
        e->reserved2 = 0;
        e->checksum = checksum;

        uint32_t start = i * 13;
        for (int j = 0; j < 13; j++) {
            uint16_t ch = (start + j < u16len) ? u16[start + j] : 0xFFFF;

            if (j < 5)
                e->name1[j] = ch;
            else if (j < 11)
                e->name2[j - 5] = ch;
            else
                e->name3[j - 11] = ch;
        }

        e->order = (uint8_t)(i + 1);
    }

    arr[entries - 1].order |= 0x40;

    uint32_t lastPos = u16len % 13;
    FAT_LFNEntry* e = &arr[entries - 1];

    uint16_t* slots[3] = { e->name1, e->name2, e->name3 };
    int lens[3] = { 5, 6, 2 };

    uint32_t index = (lastPos == 0 ? 13 : lastPos);

    if (index < 13) {
        uint32_t k = 0;
        while (k < 3 && index > (uint32_t)lens[k]) {
            index -= lens[k];
            k++;
        }
        if (k < 3) {
            slots[k][index] = 0x0000;
        }
    }

    free(u16);

    *out_count = entries;
    return arr;
}

uint8_t FAT_GetChecksum(const char shortname[11])
{
    uint8_t sum = 0;
    for (int i = 0; i < 11; i++) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + shortname[i];
    }
    return sum;
}

uint16_t* FAT_CombineLFN(FAT_LFNEntry* entries, uint32_t entry_count, uint32_t* len)
{
    if (!entries || entry_count == 0) return NULL;

    FAT_LFNEntry* list = malloc(entry_count * sizeof(FAT_LFNEntry));
    if (!list) return NULL;
    memcpy(list, entries, entry_count * sizeof(FAT_LFNEntry));
    // TODO: Remove quicksort
    qsort(list, entry_count, sizeof(FAT_LFNEntry), FAT_LFN_OrderCmp);

    uint32_t max_chars = entry_count * 13;
    uint16_t* out = malloc((max_chars + 1) * sizeof(uint16_t));
    if (!out) {
        free(list);
        return NULL;
    }

    uint32_t pos = 0;

    for (uint32_t i = 0; i < entry_count; i++)
    {
        FAT_LFNEntry* e = &list[i];

        for (int j = 0; j < 5; j++)
            out[pos++] = e->name1[j];
        for (int j = 0; j < 6; j++)
            out[pos++] = e->name2[j];
        for (int j = 0; j < 2; j++)
            out[pos++] = e->name3[j];
    }

    free(list);

    while (pos > 0 && out[pos - 1] == 0xFFFF)
        pos--;

    if (pos > 0 && out[pos - 1] == 0x0000)
        pos--;

    if (len) *len = pos;

    out[pos] = 0x0000;

    return out;
}
