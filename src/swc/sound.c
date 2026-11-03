/* SDL replacement for Sound Manager's standard snd resource decoder.
 * CODE_01 play_sound +0x626e passes the CMF chunk to SndPlay. Every demo
 * snd chunk uses format 1, sampledSynth, bufferCmd and a standard header. */
#include "swc.h"

#include <string.h>

int SwcReadSound(const SwcBuffer *resource, SwcSound *sound)
{
    SDL_RWops *reader;
    uint32_t header;
    uint32_t length;
    int result = -1;

    memset(sound, 0, sizeof(*sound));
    if (resource->size < 42 || resource->size > INT32_MAX)
        return SDL_SetError("Truncated SWC sound resource");
    reader = SDL_RWFromConstMem(resource->data, resource->size);
    if (reader == NULL)
        return -1;
    if (SDL_ReadBE16(reader) != 1 || SDL_ReadBE16(reader) != 1 ||
        SDL_ReadBE16(reader) != 5 || SDL_ReadBE32(reader) != 0xa0 ||
        SDL_ReadBE16(reader) != 1 || SDL_ReadBE16(reader) != 0x8051 ||
        SDL_ReadBE16(reader) != 0)
        goto invalid;
    header = SDL_ReadBE32(reader);
    if (header < 20 || header > resource->size - 22)
        goto invalid;
    SDL_RWseek(reader, header, RW_SEEK_SET);
    if (SDL_ReadBE32(reader) != 0)
        goto invalid;
    length = SDL_ReadBE32(reader);
    sound->rate = SDL_ReadBE32(reader);
    if (length == 0 || length > resource->size - header - 22 ||
        sound->rate < (4000U << 16) || sound->rate > (48000U << 16) ||
        resource->data[header + 20] != 0)
        goto invalid;
    sound->samples.data = SDL_malloc(length);
    if (sound->samples.data == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    sound->samples.size = length;
    memcpy(sound->samples.data, resource->data + header + 22, length);
    result = 0;
    goto done;
invalid:
    SDL_SetError("Unsupported or malformed SWC snd resource");
done:
    SDL_RWclose(reader);
    return result;
}
