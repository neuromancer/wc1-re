#include "swc.h"

#include <string.h>

int SwcGetFrameCount(const SwcBuffer *set, uint32_t *count)
{
    if (set->size < 4)
        return SDL_SetError("Truncated SWC frame count");
    *count = (uint32_t)set->data[0] << 24 | (uint32_t)set->data[1] << 16 |
             (uint32_t)set->data[2] << 8 | set->data[3];
    if (*count > (set->size - 4) / 14)
        return SDL_SetError("SWC frame count exceeds set size");
    return 0;
}

/* SWC CODE_01 +0x05f4: GetFramePtr. The original returns A0, not D0.
 * Frame headers are 14 bytes; only the payload length is rounded to four.
 * This host version also validates the index and the borrowed payload. */
int SwcGetFramePtr(const SwcBuffer *set, uint32_t index, SwcFrame *frame)
{
    size_t offset;
    size_t payloadSize;
    size_t paddedSize;
    uint32_t count;
    uint32_t current;
    SDL_RWops *reader;

    memset(frame, 0, sizeof(*frame));
    if (SwcGetFrameCount(set, &count) != 0)
        return -1;
    if (index >= count)
        return SDL_SetError("SWC frame %u is outside set of %u", index, count);
    offset = 4;
    for (current = 0; current <= index; current++) {
        if (set->size - offset < 14)
            return SDL_SetError("Truncated SWC frame header");
        reader = SDL_RWFromConstMem(set->data + offset, 14);
        if (reader == NULL)
            return -1;
        frame->encoding = SDL_ReadBE16(reader);
        frame->x = (int16_t)SDL_ReadBE16(reader);
        frame->y = (int16_t)SDL_ReadBE16(reader);
        frame->width = SDL_ReadBE16(reader);
        frame->height = SDL_ReadBE16(reader);
        payloadSize = SDL_ReadBE32(reader);
        SDL_RWclose(reader);
        offset += 14;
        if (payloadSize > SIZE_MAX - 3)
            return SDL_SetError("SWC frame size overflow");
        paddedSize = (payloadSize + 3) & ~(size_t)3;
        if (paddedSize > set->size - offset)
            return SDL_SetError("Truncated SWC frame payload/padding");
        if (frame->width > INT16_MAX || frame->height > INT16_MAX ||
            (frame->encoding != 1 && frame->encoding != 2))
            return SDL_SetError("Unsupported SWC frame format");
        frame->pixels = set->data + offset;
        frame->size = payloadSize;
        offset += paddedSize;
    }
    return 0;
}

/* CODE_01 +0x17aa DrawRLEImage confirms encoding 2's byte commands and
 * transparent index zero. Decode here; clipping/composition belong to SDL. */
int SwcDecodeFrame(const SwcFrame *frame, uint8_t *pixels, size_t capacity)
{
    size_t pixelCount;

    pixelCount = (size_t)frame->width * frame->height;
    if (capacity < pixelCount)
        return SDL_SetError("SWC frame destination is too small");
    if (frame->encoding == 2)
        return SwcDecompressRLE(frame->pixels, frame->size, pixels, pixelCount);
    if (frame->encoding != 1 || frame->size != pixelCount)
        return SDL_SetError("Invalid SWC raw frame length or encoding");
    if (pixelCount != 0)
        memcpy(pixels, frame->pixels, pixelCount);
    return 0;
}
