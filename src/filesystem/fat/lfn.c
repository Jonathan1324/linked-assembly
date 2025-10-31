#include "fat.h"

static uint32_t utf8_to_utf16(const char* input, uint16_t** out)
{
    uint32_t len = strlen(input);
    uint16_t* buf = malloc(sizeof(uint16_t) * (len + 1) * 2);
    if (!buf) return 0;
    uint32_t outlen = 0;

    const uint8_t* p = (const uint8_t*)input;

    while (*p)
    {
        uint32_t code = 0;

        if ((*p & 0x80) == 0x00) {
            code = *p++;
        } else if ((*p & 0xE0) == 0xC0) {
            code = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {
            code = ((*p & 0x0F) << 12) |
                   ((p[1] & 0x3F) << 6) |
                   (p[2] & 0x3F);
            p += 3;
        } else {
            // unsupported → replace
            code = '?';
            p++;
        }

        buf[outlen++] = (uint16_t)code;
    }

    *out = buf;
    return outlen;
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
        while (k < 3 && index > lens[k]) {
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
