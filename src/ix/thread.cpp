/*
 *  ix / D:\Rnd\Prj\ix\Src\Streamer\thread.cpp
 *
 *  Original address range: 0x00443DA6 - 0x0044490F   (11 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"
#include <stdlib.h>
#include <string.h>

#pragma function(memcpy)

#define IX_THREAD_FILE "D:\\Rnd\\Prj\\ix\\Src\\Streamer\\thread.cpp"

/* Function start: 0x00443DA6 */   /* source line(s) 196: File chunk without packet! */
void ix_thread_handle_file_chunk(IxStreamFile *streamFile)
{
    unsigned int packagePosition;
    IxStreamerFileChunk *fileChunk;
    unsigned int chunkIndex;
    unsigned int chunkCount;
    IxStreamerFileEntry *entry;
    unsigned int chunkOffset;
    unsigned int chunkBytes;
    unsigned char *source;
    unsigned int copyBytes;
    unsigned int middle;
    unsigned int firstPacket;
    unsigned int *packet;
    unsigned int lastPacket;

    entry = streamFile->entry;
    streamFile->serviceTick = dwStreamerThreadTick;
    if ((dwStreamerState & 0x100) != 0) {
        ix_file_seek(streamFile->file, streamFile->position);
        ix_file_read(streamFile->file, streamFile->destination,
                     streamFile->remaining);
        EnterCriticalSection(&csStreamerFileQueue);
        streamFile->flags &= ~IX_STREAM_FILE_READING;
        if (streamFile->previous != 0)
            streamFile->previous->next = streamFile->next;
        if (streamFile->next == 0)
            pStreamerReadQueue = streamFile->previous;
        else
            streamFile->next->previous = streamFile->previous;
        streamFile->previous = pStreamerIdleFiles;
        streamFile->next = 0;
        if (pStreamerIdleFiles != 0)
            pStreamerIdleFiles->next = streamFile;
        pStreamerIdleFiles = streamFile;
        LeaveCriticalSection(&csStreamerFileQueue);
        SetEvent(streamFile->completionEvent);
        return;
    }

    chunkIndex = entry->firstChunk;
    chunkCount = entry->chunkCount;
    fileChunk = &pStreamerFileChunks[chunkIndex];
    packagePosition = (unsigned int)ix_file_tell(
        pStreamerPackageFile);
    while (chunkCount--) {
        if (fileChunk->fileOffset <= streamFile->position &&
            fileChunk->fileEnd > streamFile->position) {
            chunkBytes = fileChunk->fileEnd - fileChunk->fileOffset;
            if (chunkIndex != nStreamerFileChunk) {
                firstPacket = fileChunk->firstPacket;
                lastPacket = fileChunk->packetCount + firstPacket;
                packet = 0;
                nStreamerFileChunk = chunkIndex;
                while (lastPacket > firstPacket) {
                    middle = (lastPacket + firstPacket) >> 1;
                    packet = &pStreamerPacketOffsets[middle];
                    if (*packet < packagePosition)
                        firstPacket = middle + 1;
                    else if (*packet > packagePosition)
                        lastPacket = middle;
                    else
                        break;
                }
                if (packet == 0) {
                    ix_log_printf("Fatal [%s - %d]:\n", IX_THREAD_FILE, 196);
                    ix_log_printf("File chunk without packet!");
                    exit(-1);
                }
                if (fileChunk->packedSize < 0) {
                    ix_file_seek(pStreamerPackageFile, *packet);
                    ix_file_read(pStreamerPackageFile,
                                 pStreamerCompressedBuffer,
                                 -fileChunk->packedSize);
#ifdef WC1_SDL
                    ix_lzo1x_decompress(
                        pStreamerCompressedBuffer,
                        pStreamerFileBuffer, chunkBytes,
                        (unsigned int)-fileChunk->packedSize);
#else
                    ix_lzo1x_decompress(pStreamerCompressedBuffer,
                                        pStreamerFileBuffer,
                                        chunkBytes);
#endif
                } else {
                    ix_file_seek(pStreamerPackageFile, *packet);
                    ix_file_read(pStreamerPackageFile,
                                 pStreamerFileBuffer,
                                 fileChunk->packedSize);
                }
            }
            source = pStreamerFileBuffer;
            copyBytes = streamFile->remaining;
            chunkOffset = streamFile->position - fileChunk->fileOffset;
            chunkBytes = chunkBytes - chunkOffset;
            source += chunkOffset;
            if (copyBytes > chunkBytes)
                copyBytes = chunkBytes;
            memcpy(streamFile->destination, source, copyBytes);
            streamFile->destination += copyBytes;
            streamFile->position += copyBytes;
            streamFile->remaining = streamFile->remaining - copyBytes;
            if (streamFile->remaining == 0) {
                EnterCriticalSection(&csStreamerFileQueue);
                if (streamFile->previous != 0)
                    streamFile->previous->next = streamFile->next;
                if (streamFile->next == 0)
                    pStreamerReadQueue = streamFile->previous;
                else
                    streamFile->next->previous = streamFile->previous;
                streamFile->previous = pStreamerIdleFiles;
                streamFile->next = 0;
                if (pStreamerIdleFiles != 0)
                    pStreamerIdleFiles->next = streamFile;
                pStreamerIdleFiles = streamFile;
                streamFile->flags &= ~IX_STREAM_FILE_READING;
                LeaveCriticalSection(&csStreamerFileQueue);
                SetEvent(streamFile->completionEvent);
            }
            return;
        }
        fileChunk++;
        chunkIndex++;
    }
}

/* Function start: 0x004441C6 */
unsigned int ix_thread_service_streams(void)
{
    int bytesUntilRefill;
    IxStreamFile *streamFile;
    IxStreamFile *selected;

    EnterCriticalSection(&csStreamerFileQueue);
    if (pStreamerReadQueue != 0) {
        streamFile = pStreamerReadQueue;
        selected = 0;
        while (streamFile != 0) {
            if (selected == 0)
                selected = streamFile;
            else if (selected->priority < streamFile->priority)
                selected = streamFile;
            else if (streamFile->serviceTick < selected->serviceTick)
                selected = streamFile;
            streamFile = streamFile->previous;
        }
        LeaveCriticalSection(&csStreamerFileQueue);
        if (selected != 0)
            ix_thread_handle_file_chunk(selected);
    } else {
        LeaveCriticalSection(&csStreamerFileQueue);
    }

    if (pStreamerReadQueue != 0)
        return 0;
    if ((dwStreamerState & 4) == 0 ||
        (dwStreamerState & 8) != 0)
        return 1000;
    bytesUntilRefill = ix_thread_get_audio_chunk_size() -
                       ix_dsps_get_buffer_free(0);
    if (bytesUntilRefill > 0) {
        bytesUntilRefill += 0x16f8;
        return (bytesUntilRefill * 1000U) /
               nStreamerBytesPerSecond;
    }
    return 0;
}

/* Function start: 0x00444316 */
void ix_thread_advance_audio_chunk(void)
{
    if (pStreamerAudioChunks[
            nStreamerAudioChunk].triggerCount > 0) {
        unsigned int triggerIndex;
        unsigned int triggerCount;

        triggerCount = pStreamerAudioChunks[
            nStreamerAudioChunk].triggerCount;
        triggerIndex = pStreamerAudioChunks[
            nStreamerAudioChunk].firstTrigger;
        while (triggerCount--) {
            if (pStreamerTriggers[triggerIndex].tag == 'A') {
                nStreamerBranchStackIndex =
                    (nStreamerBranchStackIndex - 1) & 0x1f;
                nStreamerAudioChunk =
                    adwStreamerBranchStack[
                        nStreamerBranchStackIndex];
                return;
            }
            if (pStreamerTriggers[triggerIndex].tag == '@') {
                dwStreamerState |= 0x800;
            } else if (pStreamerTriggers[triggerIndex].tag ==
                       cStreamerBranchTag) {
                adwStreamerBranchStack[
                    nStreamerBranchStackIndex] =
                        nStreamerAudioChunk;
                nStreamerBranchStackIndex =
                    (nStreamerBranchStackIndex + 1) & 0x1f;
                nStreamerAudioChunk =
                    pStreamerTriggers[triggerIndex].audioChunk;
                cStreamerBranchTag = -1;
                return;
            }
            triggerIndex++;
        }
    }

    if (pStreamerAudioChunks[
            nStreamerAudioChunk].branchCount > 0) {
        unsigned int branchCount;
        unsigned int branchIndex;
        unsigned int bestDistance;
        unsigned int selectedBranch;
        unsigned int distance;

        branchCount = pStreamerAudioChunks[
            nStreamerAudioChunk].branchCount - 1;
        branchIndex = pStreamerAudioChunks[
            nStreamerAudioChunk].firstBranch;
        bestDistance =
            ((pStreamerBranches[branchIndex].intensity -
              bStreamerIntensity) >> 31 ^
             (pStreamerBranches[branchIndex].intensity -
              bStreamerIntensity)) -
            ((pStreamerBranches[branchIndex].intensity -
              bStreamerIntensity) >> 31);
        selectedBranch = branchIndex;
        branchIndex++;
        while (branchCount--) {
            distance =
                ((pStreamerBranches[branchIndex].intensity -
                  bStreamerIntensity) >> 31 ^
                 (pStreamerBranches[branchIndex].intensity -
                  bStreamerIntensity)) -
                ((pStreamerBranches[branchIndex].intensity -
                  bStreamerIntensity) >> 31);
            if (distance < bestDistance) {
                bestDistance = distance;
                selectedBranch = branchIndex;
            }
            branchIndex++;
        }
        nStreamerAudioChunk =
            pStreamerBranches[selectedBranch].audioChunk;
    } else {
        nStreamerAudioChunk++;
        if (pStreamerHeader->audioChunkCount - 1 <=
            nStreamerAudioChunk)
            nStreamerAudioChunk = 0;
    }
}

/* Function start: 0x004445C9 */   /* source line(s) 394: failed to lock stream buffer */
void ix_thread_lock_stream_buffer(void)
{
    unsigned int remaining;
    unsigned int lockedBytes;
    unsigned char *buffer;
    unsigned int fileOffset;

    remaining = ix_thread_get_audio_chunk_size();
    if (remaining > 0) {
        fileOffset = pStreamerAudioChunks[
            nStreamerAudioChunk].fileOffset;
        while (remaining > 0) {
            ix_dsps_lock(0, remaining, &buffer, &lockedBytes);
            if (buffer == 0) {
                ix_log_printf("Fatal [%s - %d]:\n", IX_THREAD_FILE, 394);
                ix_log_printf("failed to lock stream buffer");
                exit(-1);
            }
            ix_file_seek(pStreamerPackageFile, fileOffset);
            ix_file_read(pStreamerPackageFile, buffer,
                         lockedBytes);
            remaining = remaining - lockedBytes;
            fileOffset += lockedBytes;
            ix_dsps_unlock(0);
        }
    }
    ix_thread_advance_audio_chunk();
}

/* Function start: 0x004446A6 */
unsigned int ix_thread_get_audio_chunk_size(void)
{
    unsigned int fileOffset;

    if (nStreamerAudioChunk != (unsigned int)-1) {
        fileOffset = pStreamerAudioChunks[
            nStreamerAudioChunk].fileOffset;
        return pStreamerAudioChunks[
            nStreamerAudioChunk].fileEnd - fileOffset;
    }
    return 0;
}

/* Function start: 0x00444700 */
unsigned int IxStreamFile::ix_stream_file_read(void *newDestination,
                                                unsigned int bytes)
{
    ix_stream_file_wait();
    EnterCriticalSection(&csStreamerFileQueue);
    if (position + bytes > size)
        bytes = size - position;
    if (bytes > 0) {
        destination = (unsigned char *)newDestination;
        remaining = bytes;
        flags |= IX_STREAM_FILE_READING;
        if (previous != 0)
            previous->next = next;
        if (next == 0)
            pStreamerIdleFiles = previous;
        else
            next->previous = previous;
        previous = pStreamerReadQueue;
        next = 0;
        if (pStreamerReadQueue != 0)
            pStreamerReadQueue->next = this;
        pStreamerReadQueue = this;
    }
    LeaveCriticalSection(&csStreamerFileQueue);
    SetEvent(hStreamerWakeEvent);
    return bytes;
}

/* Function start: 0x00444809 */
void IxStreamFile::ix_stream_file_seek(unsigned int newPosition)
{
    ix_stream_file_wait();
    if (newPosition >= 0)
        position = size < newPosition ? size : newPosition;
    else
        position = 0;
}

/* Function start: 0x0044485B */
unsigned int IxStreamFile::ix_stream_file_tell(void)
{
    ix_stream_file_wait();
    return position;
}

/* Function start: 0x0044487F */
unsigned int IxStreamFile::ix_stream_file_size(void)
{
    return size;
}

/* Function start: 0x0044489B */
void IxStreamFile::ix_stream_file_wait(void)
{
    if ((flags & IX_STREAM_FILE_READING) != 0) {
        WaitForSingleObject(completionEvent, INFINITE);
        ResetEvent(completionEvent);
    }
}

/* Function start: 0x004448D9 */
int IxStreamFile::ix_stream_file_is_reading(void)
{
    if ((flags & IX_STREAM_FILE_READING) != 0)
        return 1;
    return 0;
}
