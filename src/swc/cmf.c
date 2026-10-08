/* SDL host adaptation of CODE_09's CMF manager. See docs/SWC.md for the
 * original entry offsets and the intentionally different host API. */
#include "swc.h"

#include <string.h>

int SwcCMOpen(const char *path, SwcCmf *cmf)
{
    SDL_RWops *reader;
    uint32_t tocOffset;
    uint32_t tocSize;
    uint32_t typeOffset;
    uint32_t groupCount;
    uint32_t entryCount;
    uint32_t group;
    uint32_t entry;
    uint32_t flagsSize;
    size_t cursor;
    size_t tocEnd;
    size_t chunkCount;
    size_t pass;
    char type[4];
    SwcChunkInfo *chunk;

    memset(cmf, 0, sizeof(*cmf));
    cmf->data = SDL_LoadFile(path, &cmf->size);
    if (cmf->data == NULL)
        return -1;
    if (cmf->size < 28 || cmf->size > INT32_MAX ||
        memcmp(cmf->data, "CMF1", 4) != 0) {
        SDL_SetError("Invalid CMF1 header: %s", path);
        goto fail;
    }
    reader = SDL_RWFromConstMem(cmf->data, (int)cmf->size);
    if (reader == NULL)
        goto fail;
    SDL_RWseek(reader, 12, RW_SEEK_SET);
    tocOffset = SDL_ReadLE32(reader);
    tocSize = SDL_ReadLE32(reader);
    if (tocOffset < 28 || tocOffset > cmf->size || tocSize < 12 ||
        tocSize > cmf->size - tocOffset) {
        SDL_SetError("CMF table is outside the file: %s", path);
        goto failReader;
    }
    tocEnd = (size_t)tocOffset + tocSize;
    SDL_RWseek(reader, tocOffset, RW_SEEK_SET);
    typeOffset = SDL_ReadLE32(reader);
    if (typeOffset < 8 || typeOffset > tocSize - 4) {
        SDL_SetError("Invalid CMF type table: %s", path);
        goto failReader;
    }
    /* Validate/count first, then allocate and fill. No disk handle or cached
     * flag is interpreted as a live host pointer. */
    for (pass = 0; pass < 2; pass++) {
        cursor = (size_t)tocOffset + typeOffset;
        SDL_RWseek(reader, (Sint64)cursor, RW_SEEK_SET);
        groupCount = SDL_ReadLE32(reader);
        cursor += 4;
        if (groupCount > (tocEnd - cursor) / 8) {
            SDL_SetError("Truncated CMF type table: %s", path);
            goto failReader;
        }
        chunkCount = 0;
        for (group = 0; group < groupCount; group++) {
            if (tocEnd - cursor < 8) {
                SDL_SetError("Truncated CMF group: %s", path);
                goto failReader;
            }
            SDL_RWread(reader, type, 1, 4);
            entryCount = SDL_ReadLE32(reader);
            cursor += 8;
            if (entryCount > (tocEnd - cursor) / 16) {
                SDL_SetError("Truncated CMF entries: %s", path);
                goto failReader;
            }
            for (entry = 0; entry < entryCount; entry++) {
                if (pass == 0) {
                    SDL_RWseek(reader, 16, RW_SEEK_CUR);
                } else {
                    chunk = &cmf->chunks[chunkCount];
                    memcpy(chunk->type, type, 4);
                    chunk->id = SDL_ReadLE32(reader);
                    chunk->offset = SDL_ReadLE32(reader);
                    flagsSize = SDL_ReadLE32(reader);
                    chunk->size = flagsSize & UINT32_C(0x00ffffff);
                    chunk->flags = flagsSize & UINT32_C(0xff000000);
                    SDL_RWseek(reader, 4, RW_SEEK_CUR);
                    if (chunk->offset < 28 || chunk->offset > cmf->size ||
                        chunk->size > cmf->size - chunk->offset) {
                        SDL_SetError("CMF chunk %.4s/%u is outside %s",
                                     type, chunk->id, path);
                        goto failReader;
                    }
                }
                cursor += 16;
                chunkCount++;
            }
        }
        if (pass == 0 && chunkCount != 0) {
            cmf->chunks = SDL_calloc(chunkCount, sizeof(*cmf->chunks));
            if (cmf->chunks == NULL) {
                SDL_OutOfMemory();
                goto failReader;
            }
            cmf->chunkCount = chunkCount;
        }
    }
    SDL_RWclose(reader);
    return 0;

failReader:
    SDL_RWclose(reader);
fail:
    SwcCMClose(cmf);
    return -1;
}

void SwcCMClose(SwcCmf *cmf)
{
    SDL_free(cmf->chunks);
    SDL_free(cmf->data);
    memset(cmf, 0, sizeof(*cmf));
}

/* SWC CODE_09 +0x22f6: CMGetChunk. Resource lookup/decompression only;
 * classic Mac handle caching and purge state are not part of this host API. */
int SwcCMGetChunk(const SwcCmf *cmf, const char type[4], uint32_t id,
                  SwcBuffer *chunk)
{
    size_t index;
    const SwcChunkInfo *info;
    const uint8_t *source;
    uint32_t decodedSize;

    memset(chunk, 0, sizeof(*chunk));
    for (index = 0; index < cmf->chunkCount; index++) {
        info = &cmf->chunks[index];
        if (info->id != id || memcmp(info->type, type, 4) != 0)
            continue;
        source = cmf->data + info->offset;
        chunk->size = info->size;
        if ((info->flags & SWC_CMF_COMPRESSED) != 0) {
            if (info->size < 4)
                return SDL_SetError("Truncated CMF RLE size: %.4s/%u", type, id);
            decodedSize = (uint32_t)source[0] << 24 |
                          (uint32_t)source[1] << 16 |
                          (uint32_t)source[2] << 8 | source[3];
            /* Host allocation limit; the demo's largest decoded chunk is
             * below 2 MiB. Also reject lengths impossible for this codec. */
            if (decodedSize == 0 || decodedSize > 64 * 1024 * 1024 ||
                (uint64_t)decodedSize > (uint64_t)(info->size - 4) * 64)
                return SDL_SetError("Invalid CMF RLE size: %.4s/%u", type, id);
            chunk->size = decodedSize;
        }
        chunk->data = SDL_malloc(chunk->size != 0 ? chunk->size : 1);
        if (chunk->data == NULL) {
            chunk->size = 0;
            return SDL_OutOfMemory();
        }
        if ((info->flags & SWC_CMF_COMPRESSED) != 0) {
            if (SwcDecompressRLE(source + 4, info->size - 4,
                                 chunk->data, chunk->size) != 0) {
                SDL_free(chunk->data);
                memset(chunk, 0, sizeof(*chunk));
                return -1;
            }
        } else if (chunk->size != 0) {
            memcpy(chunk->data, source, chunk->size);
        }
        return 0;
    }
    return SDL_SetError("CMF chunk %.4s/%u not found", type, id);
}

/* SWC CODE_09 +0x2b26: CMDecompressRLE, byte loop +0x2ba4..+0x2bfe.
 * The caller handles the BE output length and allocation. Bounds and exact
 * stream consumption are host checks added around the recovered codec. */
int SwcDecompressRLE(const uint8_t *source, size_t sourceSize,
                     uint8_t *destination, size_t destinationSize)
{
    size_t input;
    size_t output;
    size_t count;
    uint8_t control;

    input = 0;
    output = 0;
    while (output < destinationSize) {
        if (input == sourceSize)
            return SDL_SetError("Truncated SWC RLE command");
        control = source[input++];
        count = (control & 0x7f) + 1;
        if (count > destinationSize - output)
            return SDL_SetError("SWC RLE run exceeds output size");
        if ((control & 0x80) != 0) {
            if (input == sourceSize)
                return SDL_SetError("Truncated SWC RLE repeat");
            memset(destination + output, source[input++], count);
        } else {
            if (count > sourceSize - input)
                return SDL_SetError("Truncated SWC RLE literal");
            memcpy(destination + output, source + input, count);
            input += count;
        }
        output += count;
    }
    if (input != sourceSize)
        return SDL_SetError("Trailing bytes in SWC RLE stream");
    return 0;
}
