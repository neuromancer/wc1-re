/* SDL replacement for Resource Manager/GetCTable, not recovered game code.
 * Accept a raw resource fork, macOS's ..namedfork/rsrc, or AppleDouble. */
#include "swc.h"

#include <string.h>

int SwcReadResource(const char *resourceFork, const char typeName[4], int16_t id,
                    SwcBuffer *resource)
{
    uint8_t *data;
    uint8_t *fileData;
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
    size_t typeOffset;
    size_t referenceOffset;
    size_t mapEnd;
    size_t position;
    char resourceType[4];
    int16_t resourceId;
    int result;

    memset(resource, 0, sizeof(*resource));
    fileData = SDL_LoadFile(resourceFork, &size);
    if (fileData == NULL)
        return -1;
    data = fileData;
    result = -1;
    reader = NULL;
    if (size < 16 || size > INT32_MAX)
        goto malformed;
    reader = SDL_RWFromConstMem(data, (int)size);
    if (reader == NULL)
        goto done;
    if (SDL_ReadBE32(reader) == UINT32_C(0x00051607)) {
        /* AppleDouble entry 2 carries the resource fork. Other entries
         * (Finder information, dates, etc.) are not resource data. */
        if (size < 26 || SDL_ReadBE32(reader) != UINT32_C(0x00020000))
            goto malformed;
        SDL_RWseek(reader, 24, RW_SEEK_SET);
        referenceCount = SDL_ReadBE16(reader);
        if (referenceCount > (size - 26) / 12)
            goto malformed;
        for (reference = 0; reference < referenceCount; reference++) {
            type = SDL_ReadBE32(reader);
            resourceOffset = SDL_ReadBE32(reader);
            resourceSize = SDL_ReadBE32(reader);
            if (type != 2)
                continue;
            if (resourceOffset > size || resourceSize > size - resourceOffset ||
                resourceSize < 16)
                goto malformed;
            data += resourceOffset;
            size = resourceSize;
            break;
        }
        if (reference == referenceCount)
            goto malformed;
        SDL_RWclose(reader);
        reader = SDL_RWFromConstMem(data, (int)size);
        if (reader == NULL)
            goto done;
    }
    SDL_RWseek(reader, 0, RW_SEEK_SET);
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
        if (memcmp(resourceType, typeName, 4) != 0)
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
            if (resourceSize == 0 || resourceSize > dataSize - resourceOffset - 4)
                goto malformed;
            resource->data = SDL_malloc(resourceSize);
            if (resource->data == NULL) {
                SDL_OutOfMemory();
                goto done;
            }
            resource->size = resourceSize;
            memcpy(resource->data, data + dataOffset + resourceOffset + 4, resourceSize);
            result = 0;
            goto done;
        }
    }
    SDL_SetError("Resource %.4s/%d not found in %s", typeName, id, resourceFork);
    goto done;

malformed:
    SDL_SetError("Malformed Mac resource fork: %s", resourceFork);
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(fileData);
    return result;
}

/* CODE_12 ShowSplash +0x4bca gets PICT/129. This reads the demo's single
 * PackBitsRect image, not a general QuickDraw picture interpreter. */
int SwcReadSplash(const char *resourceFork, SwcBuffer *pixels, SDL_Color colors[256])
{
    static const uint8_t rectangle[8] = {0, 0, 0, 0, 0, 240, 1, 64};
    SwcBuffer picture = {0};
    SDL_RWops *reader = NULL;
    size_t cursor = 0x880;
    size_t end;
    size_t output;
    size_t count;
    unsigned int control;
    int row;
    int color;
    int result = -1;

    memset(pixels, 0, sizeof(*pixels));
    if (SwcReadResource(resourceFork, "PICT", 129, &picture) != 0)
        return -1;
    if (picture.size < cursor ||
        memcmp(picture.data + 2, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x0a, "\x00\x11\x02\xff\x0c\x00", 6) != 0 ||
        memcmp(picture.data + 0x28, "\x00\x1e\x00\x01\x00\x0a", 6) != 0 ||
        memcmp(picture.data + 0x2e, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x36, "\x00\x98\x81\x40", 4) != 0 ||
        memcmp(picture.data + 0x3a, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x42, "\x00\x00\x00\x00", 4) != 0 ||
        memcmp(picture.data + 0x52, "\x00\x00\x00\x08\x00\x01\x00\x08", 8) != 0 ||
        memcmp(picture.data + 0x6a, "\x80\x00\x00\xff", 4) != 0 ||
        memcmp(picture.data + 0x86e, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x876, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x87e, "\x00\x40", 2) != 0)
        goto invalid;
    reader = SDL_RWFromConstMem(picture.data, (int)picture.size);
    if (reader == NULL)
        goto done;
    SDL_RWseek(reader, 0x6e, RW_SEEK_SET);
    for (color = 0; color < 256; color++) {
        SDL_ReadBE16(reader); /* ctFlags 0x8000 selects table order. */
        colors[color].r = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        colors[color].g = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        colors[color].b = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        colors[color].a = SDL_ALPHA_OPAQUE;
    }
    pixels->size = SWC_FRAME_WIDTH * SWC_FRAME_HEIGHT;
    pixels->data = SDL_malloc(pixels->size);
    if (pixels->data == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    for (row = 0; row < SWC_FRAME_HEIGHT; row++) {
        if (picture.size - cursor < 2)
            goto invalid;
        count = (size_t)picture.data[cursor] << 8 | picture.data[cursor + 1];
        cursor += 2;
        if (count > picture.size - cursor)
            goto invalid;
        end = cursor + count;
        output = 0;
        while (cursor < end) {
            control = picture.data[cursor++];
            if (control == 128)
                continue;
            count = control < 128 ? control + 1 : 257 - control;
            if (count > SWC_FRAME_WIDTH - output)
                goto invalid;
            if (control < 128) {
                if (count > end - cursor)
                    goto invalid;
                memcpy(pixels->data + row * SWC_FRAME_WIDTH + output,
                       picture.data + cursor, count);
                cursor += count;
            } else {
                if (cursor == end)
                    goto invalid;
                memset(pixels->data + row * SWC_FRAME_WIDTH + output,
                       picture.data[cursor++], count);
            }
            output += count;
        }
        if (output != SWC_FRAME_WIDTH)
            goto invalid;
    }
    cursor += cursor & 1;
    if (cursor > picture.size || picture.size - cursor != 2 ||
        memcmp(picture.data + cursor, "\x00\xff", 2) != 0)
        goto invalid;
    result = 0;
    goto done;
invalid:
    SDL_SetError("Unsupported or malformed SWC splash PICT/129");
done:
    if (result != 0) {
        SDL_free(pixels->data);
        memset(pixels, 0, sizeof(*pixels));
    }
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(picture.data);
    return result;
}

/* CODE_01 DoAdjustVolumes +0x5016 draws PICT/128 ("SoundControl") off
 * screen: one 66x118 one-bit PackBitsRect, index 0 white and 1 black. */
int SwcReadSoundControl(const char *resourceFork, SwcBuffer *pixels)
{
    static const uint8_t rectangle[8] = {0, 0, 0, 0, 0, 118, 0, 66};
    SwcBuffer picture = {0};
    uint8_t packed[10];
    size_t cursor = 0x90;
    size_t end;
    size_t output;
    size_t count;
    unsigned int control;
    int row;
    int x;
    int result = -1;

    memset(pixels, 0, sizeof(*pixels));
    if (SwcReadResource(resourceFork, "PICT", 128, &picture) != 0)
        return -1;
    if (picture.size < cursor ||
        memcmp(picture.data + 2, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x0a, "\x00\x11\x02\xff\x0c\x00", 6) != 0 ||
        memcmp(picture.data + 0x36, "\x00\x98\x80\x0a", 4) != 0 ||
        memcmp(picture.data + 0x3a, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x54, "\x00\x01\x00\x01\x00\x01", 6) != 0 ||
        memcmp(picture.data + 0x6c, "\x00\x01", 2) != 0 ||
        memcmp(picture.data + 0x70, "\xff\xff\xff\xff\xff\xff", 6) != 0 ||
        memcmp(picture.data + 0x78, "\x00\x00\x00\x00\x00\x00", 6) != 0 ||
        memcmp(picture.data + 0x7e, rectangle, sizeof(rectangle)) != 0 ||
        memcmp(picture.data + 0x86, rectangle, sizeof(rectangle)) != 0)
        goto invalid;
    pixels->size = 66 * 118;
    pixels->data = SDL_malloc(pixels->size);
    if (pixels->data == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    /* Ten-byte rows are below 251 bytes, so each has a one-byte count. */
    for (row = 0; row < 118; row++) {
        if (cursor == picture.size)
            goto invalid;
        count = picture.data[cursor++];
        if (count > picture.size - cursor)
            goto invalid;
        end = cursor + count;
        output = 0;
        while (cursor < end) {
            control = picture.data[cursor++];
            if (control == 128)
                continue;
            count = control < 128 ? control + 1 : 257 - control;
            if (count > sizeof(packed) - output)
                goto invalid;
            if (control < 128) {
                if (count > end - cursor)
                    goto invalid;
                memcpy(packed + output, picture.data + cursor, count);
                cursor += count;
            } else {
                if (cursor == end)
                    goto invalid;
                memset(packed + output, picture.data[cursor++], count);
            }
            output += count;
        }
        if (output != sizeof(packed))
            goto invalid;
        for (x = 0; x < 66; x++)
            pixels->data[row * 66 + x] = packed[x >> 3] >> (7 - (x & 7)) & 1;
    }
    cursor += cursor & 1;
    if (cursor > picture.size || picture.size - cursor != 2 ||
        memcmp(picture.data + cursor, "\x00\xff", 2) != 0)
        goto invalid;
    result = 0;
    goto done;
invalid:
    SDL_SetError("Unsupported or malformed SWC volume controls PICT/128");
done:
    if (result != 0) {
        SDL_free(pixels->data);
        memset(pixels, 0, sizeof(*pixels));
    }
    SDL_free(picture.data);
    return result;
}

int SwcReadPalette(const char *resourceFork, int16_t id, SDL_Color colors[256])
{
    SwcBuffer resource = {0};
    SDL_RWops *reader = NULL;
    uint16_t flags;
    uint16_t count;
    uint16_t index;
    uint32_t color;
    int result = -1;

    if (SwcReadResource(resourceFork, "clut", id, &resource) != 0)
        return -1;
    if (resource.size < 8)
        goto malformed;
    reader = SDL_RWFromConstMem(resource.data, (int)resource.size);
    if (reader == NULL)
        goto done;
    SDL_RWseek(reader, 4, RW_SEEK_SET);
    flags = SDL_ReadBE16(reader);
    count = SDL_ReadBE16(reader);
    if (count > 255 || (size_t)(count + 1) * 8 > resource.size - 8)
        goto malformed;
    for (color = 0; color < 256; color++)
        colors[color] = (SDL_Color){0, 0, 0, SDL_ALPHA_OPAQUE};
    for (color = 0; color <= count; color++) {
        index = SDL_ReadBE16(reader);
        if (flags & 0x8000)
            index = (uint16_t)color;
        if (index > 255)
            goto malformed;
        colors[index].r = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        colors[index].g = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        colors[index].b = (uint8_t)(SDL_ReadBE16(reader) >> 8);
    }
    result = 0;
    goto done;
malformed:
    SDL_SetError("Malformed Mac palette clut/%d: %s", id, resourceFork);
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(resource.data);
    return result;
}
