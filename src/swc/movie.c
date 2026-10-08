/* SWC Mac LMov data/codec adaptation. CODE_12 owns the original movie
 * engine; SDL presentation replaces QuickDraw and Sound Manager separately.
 * See docs/SWC.md and config/swc-provenance.json for evidence and limits. */
#include "swc.h"

#include <string.h>

/* CODE_12 +0x1fc4 LMovieOpen, +0x1e5e LMFillPixMap, and the audio length
 * handling in LMovieTask +0x1c42. Persisted handles/pointers are ignored. */
int SwcMovieOpen(const char *path, SwcMovie *movie)
{
    SDL_RWops *reader = NULL;
    SwcMovieRecord *record;
    uint32_t metadataSize;
    uint32_t flags;
    uint32_t paletteSize;
    uint32_t sampleRate;
    uint32_t audioSize;
    uint32_t audioBlockSize;
    uint32_t index;
    uint16_t sampleBits;
    uint16_t audioFlags;
    uint8_t background;
    size_t audioCopied = 0;
    size_t count;

    memset(movie, 0, sizeof(*movie));
    movie->file.data = SDL_LoadFile(path, &movie->file.size);
    if (movie->file.data == NULL)
        return -1;
    if (movie->file.size < 184 || movie->file.size > 64 * 1024 * 1024 ||
        memcmp(movie->file.data, "LMov", 4) != 0) {
        SDL_SetError("Invalid SWC LMov header: %s", path);
        goto fail;
    }
    reader = SDL_RWFromConstMem(movie->file.data, (int)movie->file.size);
    if (reader == NULL)
        goto fail;
    SDL_RWseek(reader, 4, RW_SEEK_SET);
    if (SDL_ReadBE32(reader) != UINT32_C(0x00010000)) {
        SDL_SetError("Unsupported SWC LMov version: %s", path);
        goto fail;
    }
    metadataSize = SDL_ReadBE32(reader);
    movie->recordCount = SDL_ReadBE32(reader);
    movie->framesPerSecond = SDL_ReadBE32(reader);
    flags = SDL_ReadBE32(reader);
    movie->height = SDL_ReadBE16(reader);
    movie->width = SDL_ReadBE16(reader);
    background = SDL_ReadU8(reader);
    if (movie->recordCount == 0 ||
        movie->recordCount > (movie->file.size - 184) / 16 ||
        metadataSize != 184 + movie->recordCount * 16 ||
        movie->framesPerSecond == 0 || movie->framesPerSecond > 60 ||
        movie->width != SWC_FRAME_WIDTH || movie->height != SWC_FRAME_HEIGHT ||
        (flags & ~UINT32_C(1)) != 0) {
        SDL_SetError("Unsupported SWC LMov layout: %s", path);
        goto fail;
    }
    SDL_RWseek(reader, 0x9c, RW_SEEK_SET);
    audioBlockSize = SDL_ReadBE32(reader);
    sampleRate = SDL_ReadBE32(reader);
    sampleBits = SDL_ReadBE16(reader);
    audioSize = SDL_ReadBE32(reader);
    audioFlags = SDL_ReadBE16(reader);
    paletteSize = SDL_ReadBE32(reader);
    if (paletteSize != 2056 || paletteSize > movie->file.size - metadataSize) {
        SDL_SetError("Invalid SWC LMov color table: %s", path);
        goto fail;
    }
    if ((flags & 1) != 0) {
        /* The demo uses unsigned 8-bit mono PCM. SDL accepts integer Hz;
         * round the original 16.16 rate (22254.545... in these files). */
        if (sampleBits != 8 || audioFlags != 2 || audioBlockSize == 0 ||
            audioSize == 0 || audioSize > movie->file.size ||
            sampleRate < UINT32_C(8000) * 65536 ||
            sampleRate > UINT32_C(48000) * 65536) {
            SDL_SetError("Unsupported SWC LMov audio: %s", path);
            goto fail;
        }
        movie->sampleRate = (int)(((uint64_t)sampleRate + 32768) >> 16);
        movie->audio.size = audioSize;
        movie->audio.data = SDL_malloc(movie->audio.size);
        if (movie->audio.data == NULL) {
            SDL_OutOfMemory();
            goto fail;
        }
    }
    SDL_RWseek(reader, metadataSize + 6, RW_SEEK_SET);
    if (SDL_ReadBE16(reader) != 255) {
        SDL_SetError("SWC LMov needs 256 palette entries: %s", path);
        goto fail;
    }
    for (index = 0; index < 256; index++) {
        /* LMovieOpen +0x2590 uses table order, not ColorSpec.value. */
        SDL_ReadBE16(reader);
        movie->colors[index].r = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        movie->colors[index].g = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        movie->colors[index].b = (uint8_t)(SDL_ReadBE16(reader) >> 8);
        movie->colors[index].a = 255;
    }
    movie->records = SDL_calloc(movie->recordCount, sizeof(*movie->records));
    movie->pixels = SDL_malloc((size_t)movie->width * movie->height);
    if (movie->records == NULL || movie->pixels == NULL) {
        SDL_OutOfMemory();
        goto fail;
    }
    memset(movie->pixels, background, (size_t)movie->width * movie->height);
    SDL_RWseek(reader, 184, RW_SEEK_SET);
    for (index = 0; index < movie->recordCount; index++) {
        record = &movie->records[index];
        record->flags = SDL_ReadBE32(reader);
        SDL_ReadBE32(reader); /* Cached Mac pointer, never a host address. */
        record->size = SDL_ReadBE32(reader);
        record->offset = SDL_ReadBE32(reader);
        if ((record->flags != 1 && record->flags != 2 && record->flags != 0x11) ||
            record->size == 0 || record->offset < metadataSize + paletteSize ||
            record->offset > movie->file.size ||
            record->size > movie->file.size - record->offset) {
            SDL_SetError("Invalid SWC LMov record %u: %s", index, path);
            goto fail;
        }
        if (record->flags == 2) {
            movie->frameCount++;
        } else {
            if ((flags & 1) == 0 || record->size != audioBlockSize) {
                SDL_SetError("Invalid SWC LMov audio block: %s", path);
                goto fail;
            }
            /* A 0x11 marker need not be the last stored audio block. The
             * sample count trims the last block and discards padded audio. */
            count = SDL_min(record->size, movie->audio.size - audioCopied);
            memcpy(movie->audio.data + audioCopied,
                   movie->file.data + record->offset, count);
            audioCopied += count;
        }
    }
    if (movie->frameCount == 0 || audioCopied != movie->audio.size) {
        SDL_SetError("Incomplete SWC LMov streams: %s", path);
        goto fail;
    }
    SDL_RWclose(reader);
    return 0;

fail:
    if (reader != NULL)
        SDL_RWclose(reader);
    SwcMovieClose(movie);
    return -1;
}

/* CODE_12 +0x1eea LMovieClose: host-owned buffers replace Mac handles. */
void SwcMovieClose(SwcMovie *movie)
{
    SDL_free(movie->pixels);
    SDL_free(movie->records);
    SDL_free(movie->audio.data);
    SDL_free(movie->file.data);
    memset(movie, 0, sizeof(*movie));
}

/* CODE_12 +0x2898 LMovieDrawFrameMinRect. Keep the preceding frame for skip
 * commands. The host adds bounds/exact-consumption checks and uploads the
 * whole image instead of returning a QuickDraw dirty rectangle. */
static int SwcMovieDrawFrame(SwcMovie *movie, const SwcMovieRecord *record)
{
    const uint8_t *source = movie->file.data + record->offset;
    size_t input = 0;
    size_t count;
    unsigned int x = 0;
    unsigned int y = 0;
    uint8_t command;
    uint8_t operation;
    uint8_t *destination;

    while (y < movie->height) {
        if (input == record->size)
            return SDL_SetError("Truncated SWC movie scanline");
        command = source[input++];
        operation = command & 0xe0;
        if (operation == 0) {
            x = 0;
            y++;
            continue;
        }
        if (operation == 0xe0)
            continue;
        count = command & 0x1f;
        if (operation == 0x40 || operation == 0x80 || operation == 0xc0) {
            if (input == record->size)
                return SDL_SetError("Truncated SWC movie run length");
            count = (count << 8) | source[input++];
        }
        count++;
        if (count > movie->width - x)
            return SDL_SetError("SWC movie run exceeds scanline");
        destination = movie->pixels + (size_t)y * movie->width + x;
        if (operation == 0x60 || operation == 0x80) {
            if (count > record->size - input)
                return SDL_SetError("Truncated SWC movie literal");
            memcpy(destination, source + input, count);
            input += count;
        } else if (operation == 0xa0 || operation == 0xc0) {
            if (input == record->size)
                return SDL_SetError("Truncated SWC movie repeat");
            memset(destination, source[input++], count);
        }
        x += (unsigned int)count;
    }
    if (input != record->size)
        return SDL_SetError("Trailing bytes in SWC movie frame");
    return 0;
}

/* CODE_12 LMovieTask +0x18c4..+0x199e: advance past audio records and apply
 * each video delta in order, including frames the presenter may skip. */
int SwcMovieNextFrame(SwcMovie *movie)
{
    const SwcMovieRecord *record;

    while (movie->nextRecord < movie->recordCount) {
        record = &movie->records[movie->nextRecord++];
        if (record->flags == 2) {
            if (SwcMovieDrawFrame(movie, record) != 0)
                return -1;
            return 1;
        }
    }
    return 0;
}
