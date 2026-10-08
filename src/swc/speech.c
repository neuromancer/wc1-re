/* Host reader for the demo's AIFF speech; Sound Manager is replaced by SDL.
 * CODE_03 SpoolSoundFile +0x6b02 consumes the original recording names.
 * COMM/SSND layouts and format were checked against all 92 demo recordings. */
#include "swc.h"

#include <string.h>

int SwcReadSpeech(const char *path, SwcBuffer *pcm)
{
    static const uint8_t sampleRate[10] = {0x40, 0x0c, 0xac, 0x44, 0, 0, 0, 0, 0, 0};
    SwcBuffer file = {0};
    SDL_RWops *reader = NULL;
    uint32_t formSize;
    uint32_t size;
    uint32_t samples = 0;
    uint32_t offset;
    uint16_t channels;
    uint16_t bits;
    size_t cursor;
    size_t end;
    size_t soundOffset = 0;
    size_t soundSize = 0;
    int haveFormat = 0;
    int result = -1;

    memset(pcm, 0, sizeof(*pcm));
    file.data = SDL_LoadFile(path, &file.size);
    if (file.data == NULL)
        return -1;
    if (file.size < 12 || file.size > INT32_MAX ||
        memcmp(file.data, "FORM", 4) != 0 || memcmp(file.data + 8, "AIFF", 4) != 0) {
        SDL_SetError("Invalid SWC AIFF header: %s", path);
        goto done;
    }
    reader = SDL_RWFromConstMem(file.data, (int)file.size);
    if (reader == NULL)
        goto done;
    SDL_RWseek(reader, 4, RW_SEEK_SET);
    formSize = SDL_ReadBE32(reader);
    if (formSize < 4 || formSize > file.size - 8)
        goto invalid;
    end = (size_t)formSize + 8;
    for (cursor = 12; cursor < end; cursor += size + (size & 1)) {
        if (end - cursor < 8)
            goto invalid;
        SDL_RWseek(reader, (Sint64)cursor + 4, RW_SEEK_SET);
        size = SDL_ReadBE32(reader);
        cursor += 8;
        if ((uint64_t)size + (size & 1) > end - cursor) {
            /* All 92 demo files undercount FORM by four bytes, apparently
             * excluding the AIFF form type. Their final SSND chunk reaches
             * physical EOF and contains every sample declared by COMM.
             * Permit only that exact overrun; never clip a truncated chunk
             * or extend an ordinary FORM just because trailing bytes exist. */
            if (file.size - end != 4 ||
                memcmp(file.data + cursor - 8, "SSND", 4) != 0 ||
                (uint64_t)size + (size & 1) != file.size - cursor)
                goto invalid;
            end = file.size;
        }
        if (memcmp(file.data + cursor - 8, "COMM", 4) == 0) {
            if (haveFormat || size != 18)
                goto invalid;
            channels = SDL_ReadBE16(reader);
            samples = SDL_ReadBE32(reader);
            bits = SDL_ReadBE16(reader);
            if (channels != 1 || bits != 16 ||
                memcmp(file.data + cursor + 8, sampleRate, sizeof(sampleRate)) != 0) {
                SDL_SetError("Unsupported SWC speech format: %s", path);
                goto done;
            }
            haveFormat = 1;
        } else if (memcmp(file.data + cursor - 8, "SSND", 4) == 0) {
            if (soundOffset != 0 || size < 8)
                goto invalid;
            offset = SDL_ReadBE32(reader);
            if (offset > size - 8)
                goto invalid;
            soundOffset = cursor + 8 + offset;
            soundSize = size - 8 - offset;
        }
    }
    if (!haveFormat || soundOffset == 0 || samples == 0 || samples > soundSize / 2)
        goto invalid;
    pcm->size = (size_t)samples * 2;
    pcm->data = SDL_malloc(pcm->size);
    if (pcm->data == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    memcpy(pcm->data, file.data + soundOffset, pcm->size);
    result = 0;
    goto done;
invalid:
    SDL_SetError("Truncated or invalid SWC AIFF chunks: %s", path);
done:
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(file.data);
    return result;
}
