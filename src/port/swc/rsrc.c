/* SDL replacement for Resource Manager/GetCTable, not recovered game code.
 * Accept a raw resource fork, macOS's ..namedfork/rsrc, AppleDouble, or a
 * MacBinary file. */
#include "swc.h"

#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

const char *const swcApplicationForks[SWC_APPLICATION_FORKS] = {
    "SuperWingCommanderDemo.rsrc",
    "Super Wing Commander Demo.rsrc",
    "Super Wing Commander Demo/..namedfork/rsrc",
    "._Super Wing Commander Demo",
    "Super Wing Commander Demo"
};

/* MacBinary I/II/III, the form archives of HFS discs (such as discmaster's
 * copy of the demo) give files with resource forks: a 128-byte header, then
 * the data and resource forks, each padded to 128 bytes. II and III protect
 * bytes 0..123 with CRC-16/XMODEM; I leaves bytes 99..125 zero. */
static int SwcMacBinaryForks(const uint8_t *file, size_t size,
                             SwcBuffer *dataFork, SwcBuffer *resourceFork)
{
    uint32_t dataSize;
    uint32_t resourceSize;
    size_t dataOffset;
    size_t resourceOffset;
    unsigned int crc = 0;
    size_t index;
    int bit;

    if (size < 128 || file[0] != 0 || file[1] < 1 || file[1] > 63 ||
        file[74] != 0 || file[82] != 0)
        return 0;
    for (index = 2; index < 2 + (size_t)file[1]; index++) {
        if (file[index] < 0x20 || file[index] == ':')
            return 0;
    }
    for (index = 0; index < 124; index++) {
        crc ^= (unsigned int)file[index] << 8;
        for (bit = 0; bit < 8; bit++)
            crc = (crc & 0x8000 ? crc << 1 ^ 0x1021 : crc << 1) & 0xffff;
    }
    if (crc != ((unsigned int)file[124] << 8 | file[125])) {
        for (index = 99; index < 126; index++) {
            if (file[index] != 0)
                return 0;
        }
    }
    dataSize = (uint32_t)file[83] << 24 | (uint32_t)file[84] << 16 |
               (uint32_t)file[85] << 8 | file[86];
    resourceSize = (uint32_t)file[87] << 24 | (uint32_t)file[88] << 16 |
                   (uint32_t)file[89] << 8 | file[90];
    /* MacBinary II's secondary header length; zero in MacBinary I. */
    dataOffset = 128 + (((size_t)file[120] << 8 | file[121]) + 127) / 128 * 128;
    if (dataOffset > size || dataSize > size - dataOffset)
        return 0;
    resourceOffset = dataOffset + ((size_t)dataSize + 127) / 128 * 128;
    if (resourceSize != 0 && (resourceOffset > size || resourceSize > size - resourceOffset))
        return 0;
    *dataFork = (SwcBuffer){(uint8_t *)file + dataOffset, dataSize};
    *resourceFork = (SwcBuffer){(uint8_t *)file + resourceOffset, resourceSize};
    return 1;
}

int SwcLoadDataFork(const char *path, SwcBuffer *data)
{
    SwcBuffer dataFork;
    SwcBuffer resourceFork;

    data->data = SDL_LoadFile(path, &data->size);
    if (data->data == NULL) {
        data->size = 0;
        return -1;
    }
    if (SwcMacBinaryForks(data->data, data->size, &dataFork, &resourceFork)) {
        memmove(data->data, dataFork.data, dataFork.size);
        data->size = dataFork.size;
    }
    return 0;
}

int SwcReadResource(const char *resourceFork, const char typeName[4], int16_t id,
                    SwcBuffer *resource)
{
    SwcBuffer dataFork;
    SwcBuffer macBinaryFork;
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
    reader = SDL_RWFromConstMem(data, size);
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
        reader = SDL_RWFromConstMem(data, size);
        if (reader == NULL)
            goto done;
    } else if (SwcMacBinaryForks(data, size, &dataFork, &macBinaryFork)) {
        if (macBinaryFork.size < 16)
            goto malformed;
        data = macBinaryFork.data;
        size = macBinaryFork.size;
        SDL_RWclose(reader);
        reader = SDL_RWFromConstMem(data, size);
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
    SDL_RWseek(reader, typeOffset, RW_SEEK_SET);
    typeCount = SDL_ReadBE16(reader);
    typeCount = typeCount == UINT16_MAX ? 0 : typeCount + 1;
    if (typeCount > (mapEnd - typeOffset - 2) / 8)
        goto malformed;
    for (type = 0; type < typeCount; type++) {
        SDL_RWseek(reader, (typeOffset + 2 + type * 8), RW_SEEK_SET);
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
            SDL_RWseek(reader, position, RW_SEEK_SET);
            resourceId = SDL_ReadBE16(reader);
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
    reader = SDL_RWFromConstMem(picture.data, picture.size);
    if (reader == NULL)
        goto done;
    SDL_RWseek(reader, 0x6e, RW_SEEK_SET);
    for (color = 0; color < 256; color++) {
        SDL_ReadBE16(reader); /* ctFlags 0x8000 selects table order. */
        colors[color].r = (SDL_ReadBE16(reader) >> 8);
        colors[color].g = (SDL_ReadBE16(reader) >> 8);
        colors[color].b = (SDL_ReadBE16(reader) >> 8);
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
    reader = SDL_RWFromConstMem(resource.data, resource.size);
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
            index = color;
        if (index > 255)
            goto malformed;
        colors[index].r = (SDL_ReadBE16(reader) >> 8);
        colors[index].g = (SDL_ReadBE16(reader) >> 8);
        colors[index].b = (SDL_ReadBE16(reader) >> 8);
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

/* The application's signature resource and the palette the game needs. */
static int SwcIsApplicationFork(const char *path)
{
    SwcBuffer signature;
    SDL_Color colors[256];

    if (SwcReadResource(path, "MoeT", 0, &signature) != 0)
        return 0;
    SDL_free(signature.data);
    return SwcReadPalette(path, 251, colors) == 0;
}

static int SwcTryApplicationFork(const char *directory, const char *name,
                                 char *path, size_t capacity)
{
    int length;

    length = SDL_snprintf(path, capacity, "%s/%s", directory, name);
    if (length < 0 || (size_t)length >= capacity)
        return 0;
    if (SwcIsApplicationFork(path))
        return 1;
#ifdef __APPLE__
    length = SDL_snprintf(path, capacity, "%s/%s/..namedfork/rsrc", directory, name);
    if (length >= 0 && (size_t)length < capacity && SwcIsApplicationFork(path))
        return 1;
#endif
    return 0;
}

int SwcFindApplicationFork(const char *directory, char *path, size_t capacity)
{
    /* Every known copy of the application is below 1 MB; skip large files. */
    const long long sizeLimit = 16 * 1024 * 1024;
    int found = 0;
#ifdef _WIN32
    struct _finddata_t entry;
    intptr_t search;
    char *pattern;
    size_t patternSize = SDL_strlen(directory) + 3;

    pattern = SDL_malloc(patternSize);
    if (pattern == NULL)
        return 0;
    SDL_snprintf(pattern, patternSize, "%s/*", directory);
    search = _findfirst(pattern, &entry);
    SDL_free(pattern);
    if (search != -1) {
        do {
            if ((entry.attrib & _A_SUBDIR) == 0 && entry.size <= sizeLimit)
                found = SwcTryApplicationFork(directory, entry.name, path, capacity);
        } while (!found && _findnext(search, &entry) == 0);
        _findclose(search);
    }
#else
    DIR *listing = opendir(directory);
    struct dirent *entry;
    struct stat status;
    int length;

    while (listing != NULL && !found && (entry = readdir(listing)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        length = SDL_snprintf(path, capacity, "%s/%s", directory, entry->d_name);
        if (length < 0 || (size_t)length >= capacity || stat(path, &status) != 0 ||
            !S_ISREG(status.st_mode) || status.st_size > sizeLimit)
            continue;
        found = SwcTryApplicationFork(directory, entry->d_name, path, capacity);
    }
    if (listing != NULL)
        closedir(listing);
#endif
    if (!found)
        SDL_SetError("No SWC application resources in %s", directory);
    return found;
}
