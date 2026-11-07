/* SDL-only corrections to decoded Origin packets and loaded stream metadata.
 * No disk writes.
 * Both GetPacketSize and PacketLoad go through OpenPacketSection, so callers
 * allocating their own buffers see exactly the same size as the reader.
 *
 * Patches identify a filename, packet section or stream offset, length and
 * CRC32. Unknown or already corrected assets pass through unchanged. Edits
 * are relative to the original decoded section or loaded stream record.
 * See data_patches.h and bin/buildDataPatches.py for the documented fixes. */
#include "wc1.h"

typedef struct SdlDataEdit {
    unsigned int offset;
    unsigned int removeSize;
    unsigned int insertSize;
    unsigned int literalOffset;
} SdlDataEdit;

typedef struct SdlDataPatch {
    const char *filename;
    unsigned int location; /* Packet section index or absolute stream offset. */
    unsigned int sourceSize;
    Uint32 sourceCrc;
    unsigned int resultSize;
    Uint32 resultCrc;
    const SdlDataEdit *edits;
    size_t editCount;
    const unsigned char *literals;
    size_t literalSize;
} SdlDataPatch;

#include "data_patches.h"

static const char *SdlDataBasename(const char *filename)
{
    const char *basename = filename;
    const char *cursor;

    for (cursor = filename; *cursor != '\0'; cursor++) {
        if (*cursor == '/' || *cursor == '\\')
            basename = cursor + 1;
    }
    return basename;
}

static int SdlApplyDataEdits(const SdlDataPatch *patch,
                             const unsigned char *source,
                             unsigned char *result)
{
    size_t sourcePosition = 0;
    size_t resultPosition = 0;
    size_t index;

    for (index = 0; index < patch->editCount; index++) {
        const SdlDataEdit *edit = &patch->edits[index];
        size_t copySize;

        if (edit->offset < sourcePosition ||
            edit->offset > patch->sourceSize ||
            edit->removeSize > patch->sourceSize - edit->offset ||
            edit->literalOffset > patch->literalSize ||
            edit->insertSize > patch->literalSize - edit->literalOffset)
            return 0;
        copySize = edit->offset - sourcePosition;
        if (copySize > patch->resultSize - resultPosition)
            return 0;
        memcpy(result + resultPosition, source + sourcePosition, copySize);
        resultPosition += copySize;
        if (edit->insertSize > patch->resultSize - resultPosition)
            return 0;
        memcpy(result + resultPosition,
               patch->literals + edit->literalOffset, edit->insertSize);
        resultPosition += edit->insertSize;
        sourcePosition = edit->offset + edit->removeSize;
    }
    if (patch->sourceSize - sourcePosition !=
        patch->resultSize - resultPosition)
        return 0;
    memcpy(result + resultPosition, source + sourcePosition,
           patch->sourceSize - sourcePosition);
    return SDL_crc32(0, result, patch->resultSize) == patch->resultCrc;
}

int SdlPatchPacketSection(const char *filename, short section,
                         PacketSectionHandle *handle)
{
    const SdlDataPatch *patch;
    const char *basename = SdlDataBasename(filename);
    unsigned char *stored = 0;
    unsigned char *decoded = 0;
    unsigned char *result = 0;
    size_t decodedSize;
    size_t readSize;
    size_t writtenSize;
    size_t index;
    Uint32 crc;
    int count;
    int success = 0;
    short error = nPacketError;

    for (index = 0; index < SDL_arraysize(packetPatches); index++) {
        patch = &packetPatches[index];
        if (section == patch->location &&
            SDL_strcasecmp(basename, patch->filename) == 0 &&
            (handle->compression == 1 ||
             handle->dataSize == patch->sourceSize))
            break;
    }
    if (index == SDL_arraysize(packetPatches))
        return 1;

    /* Bound temporary storage before reading an archive supplied by the
     * player. A short read must never hash uninitialized memory. */
    nPacketError = 6;
    if (handle->dataSize == 0 || handle->dataSize > 16U * 1024U * 1024U)
        return 0;
    stored = malloc(handle->dataSize);
    if (stored == 0) {
        nPacketError = 1;
        return 0;
    }
    if (_lseek(handle->file, handle->dataOffset, SEEK_SET) == -1)
        goto done;
    readSize = 0;
    while (readSize < handle->dataSize) {
        count = _read(handle->file, stored + readSize,
                      handle->dataSize - readSize);
        if (count <= 0)
            goto done;
        readSize += count;
    }
    decodedSize = handle->dataSize;
    if (handle->compression == 1) {
        if (readSize < 4)
            goto done;
        decodedSize = (Uint32)stored[0] | ((Uint32)stored[1] << 8) |
            ((Uint32)stored[2] << 16) | ((Uint32)stored[3] << 24);
    }
    for (index = 0; index < SDL_arraysize(packetPatches); index++) {
        patch = &packetPatches[index];
        if (section == patch->location && decodedSize == patch->sourceSize &&
            SDL_strcasecmp(basename, patch->filename) == 0)
            break;
    }
    if (index == SDL_arraysize(packetPatches)) {
        success = 1;
        goto done;
    }
    if (handle->compression == 1) {
        decoded = malloc(decodedSize);
        if (decoded == 0) {
            nPacketError = 1;
            goto done;
        }
        if (!SdlDecompressOriginLzw(stored + 4, readSize - 4, decoded,
                                    decodedSize, &writtenSize))
            goto done;
    } else {
        decoded = stored;
        stored = 0;
    }
    crc = SDL_crc32(0, decoded, decodedSize);
    for (index = 0; index < SDL_arraysize(packetPatches); index++) {
        patch = &packetPatches[index];
        if (section == patch->location && decodedSize == patch->sourceSize &&
            crc == patch->sourceCrc &&
            SDL_strcasecmp(basename, patch->filename) == 0)
            break;
    }
    if (index == SDL_arraysize(packetPatches)) {
        success = 1;
        goto done;
    }
    result = malloc(patch->resultSize);
    if (result == 0) {
        nPacketError = 1;
        goto done;
    }
    if (!SdlApplyDataEdits(patch, decoded, result))
        goto done;
    success = 1;

done:
    free(stored);
    free(decoded);
    if (_lseek(handle->file, handle->dataOffset, SEEK_SET) == -1) {
        nPacketError = 6;
        success = 0;
    }
    if (!success) {
        /* Never silently use a larger original after a size query succeeded
         * with a smaller patch: the caller may already own that small buffer. */
        free(result);
        return 0;
    }
    if (result != 0) {
        handle->patchedData = result;
        handle->dataSize = patch->resultSize;
        handle->compression = 0;
    }
    nPacketError = error;
    return 1;
}

/* Stream tables are already decoded and owned by ix. Use the same guarded
 * splice engine as packet sections, but retain their allocation and size.
 * Offsets refer to the original stream file; only the loaded bytes change. */
void SdlPatchStreamData(const char *filename, unsigned int fileOffset,
                        void *data, size_t size)
{
    const char *basename = SdlDataBasename(filename);
    unsigned char *bytes = data;
    size_t index;

    for (index = 0; index < SDL_arraysize(streamPatches); index++) {
        const SdlDataPatch *patch = &streamPatches[index];
        unsigned char *result;
        size_t offset;

        if (SDL_strcasecmp(basename, patch->filename) != 0 ||
            patch->location < fileOffset ||
            patch->sourceSize != patch->resultSize)
            continue;
        offset = patch->location - fileOffset;
        if (offset > size || patch->sourceSize > size - offset ||
            SDL_crc32(0, bytes + offset, patch->sourceSize) != patch->sourceCrc)
            continue;
        result = malloc(patch->resultSize);
        if (result == 0)
            continue;
        if (SdlApplyDataEdits(patch, bytes + offset, result))
            memcpy(bytes + offset, result, patch->resultSize);
        free(result);
    }
}
