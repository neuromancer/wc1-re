/* Host reader for the demo's AIFF speech; Sound Manager is replaced by SDL.
 * CODE_03 SpoolSoundFile +0x6b02 consumes the original recording names.
 * COMM/SSND layouts and formats were checked against all 102 demo recordings:
 * the 92 conversation lines and intro.11 are 16-bit at 11025 Hz, intro.03..10
 * 8-bit at the Mac's 22254.545 Hz and Stranded.aif 8-bit at 11025 Hz. */
#include "swc.h"

#include <string.h>

/* COMM stores the sample rate as an 80-bit IEEE 754 extended value; the
 * integer part is kept (22254 Hz for the Mac's 22254.545 Hz rate). */
static int SwcAiffRate(const uint8_t extended[10])
{
    int exponent = ((extended[0] & 0x7f) << 8 | extended[1]) - 16383;
    uint64_t mantissa = 0;
    int index;

    for (index = 2; index < 10; index++)
        mantissa = mantissa << 8 | extended[index];
    if ((extended[0] & 0x80) != 0 || exponent < 10 || exponent > 16)
        return 0;
    return (int)(mantissa >> (63 - exponent));
}

int SwcReadSpeech(const char *path, SwcBuffer *pcm, int *rate)
{
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
    *rate = 0;
    /* Archives of the HFS disc serve Stranded.aif as MacBinary. */
    if (SwcLoadDataFork(path, &file) != 0)
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
            *rate = SwcAiffRate(file.data + cursor + 8);
            if (channels != 1 || (bits != 8 && bits != 16) || *rate == 0) {
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
    if (!haveFormat || soundOffset == 0 || samples == 0 || samples > soundSize / (bits / 8))
        goto invalid;
    pcm->size = (size_t)samples * 2;
    pcm->data = SDL_malloc(pcm->size);
    if (pcm->data == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    if (bits == 16) {
        memcpy(pcm->data, file.data + soundOffset, pcm->size);
    } else {
        /* CODE_14 stranded_sequence spools the 8-bit Stranded.aif. Widen its
         * signed samples to the 16-bit big-endian PCM every caller queues. */
        for (cursor = 0; cursor < samples; cursor++) {
            pcm->data[cursor * 2] = file.data[soundOffset + cursor];
            pcm->data[cursor * 2 + 1] = 0;
        }
    }
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

/* CODE_12 ParseMarks +0x00de/GetMouthPos +0x036a. The phoneme table is
 * expanded DATA/0 +0x0908. Each character advances five 60 Hz ticks;
 * "th" and "ch" consume two letters, h retains the preceding mouth. */
int SwcReadSpeechMarks(const char *path, int rate, SwcSpeechMark **marks, size_t *count)
{
    static const int phonemes[26] = {
        0, 1, 8, 7, 1, 5, 8, 0, 0, 8, 0, 9, 6,
        7, 2, 6, 2, 1, 7, 1, 3, 5, 3, 7, 1, 1
    };
    SwcBuffer file = {0};
    SDL_RWops *reader = NULL;
    uint32_t size;
    uint32_t sample;
    uint64_t tick;
    size_t cursor;
    size_t end;
    size_t length;
    size_t letter;
    unsigned int records;
    unsigned int record;
    int character;
    int mouth = 10;
    int previous = 10;
    int result = -1;

    *marks = NULL;
    *count = 0;
    if (SwcLoadDataFork(path, &file) != 0)
        return -1;
    if (file.size < 12 || file.size > INT32_MAX ||
        memcmp(file.data, "FORM", 4) != 0 || memcmp(file.data + 8, "AIFF", 4) != 0)
        goto invalid;
    reader = SDL_RWFromConstMem(file.data, (int)file.size);
    if (reader == NULL)
        goto done;
    /* SwcReadSpeech validates COMM/SSND before this optional metadata read.
     * Scan physical chunks, including the demo's four-byte FORM undercount. */
    for (cursor = 12; cursor < file.size; cursor += size + (size & 1)) {
        if (file.size - cursor < 8)
            goto invalid;
        SDL_RWseek(reader, (Sint64)cursor + 4, RW_SEEK_SET);
        size = SDL_ReadBE32(reader);
        cursor += 8;
        if ((uint64_t)size + (size & 1) > file.size - cursor)
            goto invalid;
        if (memcmp(file.data + cursor - 8, "MARK", 4) != 0)
            continue;
        /* One output per letter plus the closed-mouth event for each word
         * fits inside the source chunk's byte count. Bound host allocation. */
        if (size < 2 || size > 1024 * 1024)
            goto invalid;
        *marks = SDL_calloc(size, sizeof(**marks));
        if (*marks == NULL) {
            SDL_OutOfMemory();
            goto done;
        }
        end = cursor + size;
        records = SDL_ReadBE16(reader);
        cursor += 2;
        for (record = 0; record < records; record++) {
            if (end - cursor < 7)
                goto invalid;
            SDL_RWseek(reader, (Sint64)cursor + 2, RW_SEEK_SET);
            sample = SDL_ReadBE32(reader);
            length = SDL_ReadU8(reader);
            cursor += 7;
            if (length + ((length + 1) & 1) > end - cursor)
                goto invalid;
            tick = (uint64_t)sample * 60 / (unsigned int)rate;
            for (letter = 0; letter < length; letter++) {
                character = SDL_tolower(file.data[cursor + letter]);
                if ((character == 't' || character == 'c') && letter + 1 < length &&
                    SDL_tolower(file.data[cursor + letter + 1]) == 'h') {
                    mouth = character == 't' ? 9 : 4;
                    letter++;
                } else if (character >= 'a' && character <= 'z') {
                    if (character != 'h')
                        mouth = phonemes[character - 'a'];
                } else {
                    mouth = previous;
                }
                previous = mouth;
                tick += 5;
                (*marks)[*count].tick = (uint32_t)tick;
                (*marks)[(*count)++].frame = mouth <= 9 ? mouth : -1;
            }
            (*marks)[*count].tick = (uint32_t)(tick + 5);
            (*marks)[(*count)++].frame = -1;
            cursor += length + ((length + 1) & 1);
        }
        if (cursor != end)
            goto invalid;
        break;
    }
    result = 0;
    goto done;
invalid:
    SDL_SetError("Malformed SWC AIFF mouth markers: %s", path);
done:
    if (result != 0) {
        SDL_free(*marks);
        *marks = NULL;
        *count = 0;
    }
    if (reader != NULL)
        SDL_RWclose(reader);
    SDL_free(file.data);
    return result;
}
