/* SDL replacement for Resource Manager/GetCTable, not recovered game code.
 * Accept an extracted raw resource fork or macOS's ..namedfork/rsrc path. */
#include "swc.h"

#include <string.h>

int SwcReadPalette(const char *resourceFork, int16_t id, SDL_Color colors[256])
{
    uint8_t *data;
    size_t size;
    SDL_RWops *reader;
    uint32_t dataOffset;
    uint32_t mapOffset;
    uint32_t dataSize;
    uint32_t mapSize;
    uint32_t resourceOffset;
    uint32_t resourceSize;
    uint32_t typeCount;
    uint32_t referenceCount;
    uint32_t type;
    uint32_t reference;
    uint32_t color;
    size_t typeOffset;
    size_t referenceOffset;
    size_t mapEnd;
    size_t position;
    char resourceType[4];
    int16_t resourceId;
    uint16_t colorCount;
    uint16_t colorIndex;
    uint16_t colorFlags;
    int result;

    data = SDL_LoadFile(resourceFork, &size);
    if (data == NULL)
        return -1;
    result = -1;
    reader = NULL;
    if (size < 16 || size > INT32_MAX)
        goto malformed;
    reader = SDL_RWFromConstMem(data, (int)size);
    if (reader == NULL)
        goto done;
    dataOffset = SDL_ReadBE32(reader);
    mapOffset = SDL_ReadBE32(reader);
    dataSize = SDL_ReadBE32(reader);
    mapSize = SDL_ReadBE32(reader);
    if (dataOffset > size || dataSize > size - dataOffset ||
        mapOffset > size || mapSize > size - mapOffset || mapSize < 28)
        goto malformed;
    mapEnd = (size_t)mapOffset + mapSize;
    SDL_RWseek(reader, (Sint64)mapOffset + 24, RW_SEEK_SET);
    typeOffset = SDL_ReadBE16(reader);
    if (typeOffset < 28 || typeOffset > mapSize - 2)
        goto malformed;
    typeOffset += mapOffset;
    SDL_RWseek(reader, (Sint64)typeOffset, RW_SEEK_SET);
    typeCount = SDL_ReadBE16(reader);
    typeCount = typeCount == UINT16_MAX ? 0 : typeCount + 1;
    if (typeCount > (mapEnd - typeOffset - 2) / 8)
        goto malformed;
    for (type = 0; type < typeCount; type++) {
        SDL_RWseek(reader, (Sint64)(typeOffset + 2 + type * 8), RW_SEEK_SET);
        SDL_RWread(reader, resourceType, 1, 4);
        referenceCount = (uint32_t)SDL_ReadBE16(reader) + 1;
        referenceOffset = SDL_ReadBE16(reader);
        if (referenceOffset > mapEnd - typeOffset)
            goto malformed;
        referenceOffset += typeOffset;
        if (referenceCount > (mapEnd - referenceOffset) / 12)
            goto malformed;
        if (memcmp(resourceType, "clut", 4) != 0)
            continue;
        for (reference = 0; reference < referenceCount; reference++) {
            position = referenceOffset + reference * 12;
            SDL_RWseek(reader, (Sint64)position, RW_SEEK_SET);
            resourceId = (int16_t)SDL_ReadBE16(reader);
            if (resourceId != id)
                continue;
            SDL_RWseek(reader, 2, RW_SEEK_CUR);
            resourceOffset = SDL_ReadBE32(reader) & UINT32_C(0x00ffffff);
            if (resourceOffset > dataSize || dataSize - resourceOffset < 4)
                goto malformed;
            SDL_RWseek(reader, (Sint64)dataOffset + resourceOffset, RW_SEEK_SET);
            resourceSize = SDL_ReadBE32(reader);
            if (resourceSize < 8 || resourceSize > dataSize - resourceOffset - 4)
                goto malformed;
            SDL_RWseek(reader, 4, RW_SEEK_CUR);
            colorFlags = SDL_ReadBE16(reader);
            colorCount = SDL_ReadBE16(reader);
            if (colorCount > 255 || (size_t)(colorCount + 1) * 8 > resourceSize - 8)
                goto malformed;
            for (color = 0; color < 256; color++) {
                colors[color].r = 0;
                colors[color].g = 0;
                colors[color].b = 0;
                colors[color].a = SDL_ALPHA_OPAQUE;
            }
            for (color = 0; color <= colorCount; color++) {
                colorIndex = SDL_ReadBE16(reader);
                if ((colorFlags & 0x8000) != 0)
                    colorIndex = (uint16_t)color;
                if (colorIndex > 255)
                    goto malformed;
                colors[colorIndex].r = (uint8_t)(SDL_ReadBE16(reader) >> 8);
                colors[colorIndex].g = (uint8_t)(SDL_ReadBE16(reader) >> 8);
                colors[colorIndex].b = (uint8_t)(SDL_ReadBE16(reader) >> 8);
            }
            result = 0;
            goto done;
        }
    }
    SDL_SetError("Palette clut/%d not found in %s", id, resourceFork);
    goto done;

malformed:
    SDL_SetError("Malformed Mac palette resource fork: %s", resourceFork);
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(data);
    return result;
}
