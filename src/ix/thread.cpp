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
    streamFile->serviceTick = g_dwStreamerThreadTick_00597bd4;
    if ((g_dwStreamerState_00597cd0 & 0x100) != 0) {
        ix_file_seek(streamFile->file, streamFile->position);
        ix_file_read(streamFile->file, streamFile->destination,
                     streamFile->remaining);
        EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
        streamFile->flags &= ~IX_STREAM_FILE_READING;
        if (streamFile->previous != 0)
            streamFile->previous->next = streamFile->next;
        if (streamFile->next == 0)
            g_pStreamerReadQueue_00597c6c = streamFile->previous;
        else
            streamFile->next->previous = streamFile->previous;
        streamFile->previous = g_pStreamerIdleFiles_00597be0;
        streamFile->next = 0;
        if (g_pStreamerIdleFiles_00597be0 != 0)
            g_pStreamerIdleFiles_00597be0->next = streamFile;
        g_pStreamerIdleFiles_00597be0 = streamFile;
        LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
        SetEvent(streamFile->completionEvent);
        return;
    }

    chunkIndex = entry->firstChunk;
    chunkCount = entry->chunkCount;
    fileChunk = &g_pStreamerFileChunks_00597c90[chunkIndex];
    packagePosition = (unsigned int)ix_file_tell(
        g_pStreamerPackageFile_00597bdc);
    while (chunkCount--) {
        if (fileChunk->fileOffset <= streamFile->position &&
            fileChunk->fileEnd > streamFile->position) {
            chunkBytes = fileChunk->fileEnd - fileChunk->fileOffset;
            if (chunkIndex != g_nStreamerFileChunk_00597cd8) {
                firstPacket = fileChunk->firstPacket;
                lastPacket = fileChunk->packetCount + firstPacket;
                packet = 0;
                g_nStreamerFileChunk_00597cd8 = chunkIndex;
                while (lastPacket > firstPacket) {
                    middle = (lastPacket + firstPacket) >> 1;
                    packet = &g_pStreamerPacketOffsets_00597bd0[middle];
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
                    ix_file_seek(g_pStreamerPackageFile_00597bdc, *packet);
                    ix_file_read(g_pStreamerPackageFile_00597bdc,
                                 g_pStreamerCompressedBuffer_00597c68,
                                 -fileChunk->packedSize);
                    ix_lzo1x_decompress(g_pStreamerCompressedBuffer_00597c68,
                                        g_pStreamerFileBuffer_00597c74,
                                        chunkBytes);
                } else {
                    ix_file_seek(g_pStreamerPackageFile_00597bdc, *packet);
                    ix_file_read(g_pStreamerPackageFile_00597bdc,
                                 g_pStreamerFileBuffer_00597c74,
                                 fileChunk->packedSize);
                }
            }
            source = g_pStreamerFileBuffer_00597c74;
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
                EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
                if (streamFile->previous != 0)
                    streamFile->previous->next = streamFile->next;
                if (streamFile->next == 0)
                    g_pStreamerReadQueue_00597c6c = streamFile->previous;
                else
                    streamFile->next->previous = streamFile->previous;
                streamFile->previous = g_pStreamerIdleFiles_00597be0;
                streamFile->next = 0;
                if (g_pStreamerIdleFiles_00597be0 != 0)
                    g_pStreamerIdleFiles_00597be0->next = streamFile;
                g_pStreamerIdleFiles_00597be0 = streamFile;
                streamFile->flags &= ~IX_STREAM_FILE_READING;
                LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
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

    EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
    if (g_pStreamerReadQueue_00597c6c != 0) {
        streamFile = g_pStreamerReadQueue_00597c6c;
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
        LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
        if (selected != 0)
            ix_thread_handle_file_chunk(selected);
    } else {
        LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
    }

    if (g_pStreamerReadQueue_00597c6c != 0)
        return 0;
    if ((g_dwStreamerState_00597cd0 & 4) == 0 ||
        (g_dwStreamerState_00597cd0 & 8) != 0)
        return 1000;
    bytesUntilRefill = ix_thread_get_audio_chunk_size() -
                       ix_dsps_get_buffer_free(0);
    if (bytesUntilRefill > 0) {
        bytesUntilRefill += 0x16f8;
        return (bytesUntilRefill * 1000U) /
               g_nStreamerBytesPerSecond_00597cdc;
    }
    return 0;
}

/* Function start: 0x00444316 */
void ix_thread_advance_audio_chunk(void)
{
    if (g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].triggerCount > 0) {
        unsigned int triggerIndex;
        unsigned int triggerCount;

        triggerCount = g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].triggerCount;
        triggerIndex = g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].firstTrigger;
        while (triggerCount--) {
            if (g_pStreamerTriggers_00597cf8[triggerIndex].tag == 'A') {
                g_nStreamerBranchStackIndex_00470e8c =
                    (g_nStreamerBranchStackIndex_00470e8c - 1) & 0x1f;
                g_nStreamerAudioChunk_00597cc8 =
                    g_adwStreamerBranchStack_00597be8[
                        g_nStreamerBranchStackIndex_00470e8c];
                return;
            }
            if (g_pStreamerTriggers_00597cf8[triggerIndex].tag == '@') {
                g_dwStreamerState_00597cd0 |= 0x800;
            } else if (g_pStreamerTriggers_00597cf8[triggerIndex].tag ==
                       g_cStreamerBranchTag_00470e88) {
                g_adwStreamerBranchStack_00597be8[
                    g_nStreamerBranchStackIndex_00470e8c] =
                        g_nStreamerAudioChunk_00597cc8;
                g_nStreamerBranchStackIndex_00470e8c =
                    (g_nStreamerBranchStackIndex_00470e8c + 1) & 0x1f;
                g_nStreamerAudioChunk_00597cc8 =
                    g_pStreamerTriggers_00597cf8[triggerIndex].audioChunk;
                g_cStreamerBranchTag_00470e88 = -1;
                return;
            }
            triggerIndex++;
        }
    }

    if (g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].branchCount > 0) {
        unsigned int branchCount;
        unsigned int branchIndex;
        unsigned int bestDistance;
        unsigned int selectedBranch;
        unsigned int distance;

        branchCount = g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].branchCount - 1;
        branchIndex = g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].firstBranch;
        bestDistance =
            ((g_pStreamerBranches_00597c8c[branchIndex].intensity -
              g_bStreamerIntensity_00597c78) >> 31 ^
             (g_pStreamerBranches_00597c8c[branchIndex].intensity -
              g_bStreamerIntensity_00597c78)) -
            ((g_pStreamerBranches_00597c8c[branchIndex].intensity -
              g_bStreamerIntensity_00597c78) >> 31);
        selectedBranch = branchIndex;
        branchIndex++;
        while (branchCount--) {
            distance =
                ((g_pStreamerBranches_00597c8c[branchIndex].intensity -
                  g_bStreamerIntensity_00597c78) >> 31 ^
                 (g_pStreamerBranches_00597c8c[branchIndex].intensity -
                  g_bStreamerIntensity_00597c78)) -
                ((g_pStreamerBranches_00597c8c[branchIndex].intensity -
                  g_bStreamerIntensity_00597c78) >> 31);
            if (distance < bestDistance) {
                bestDistance = distance;
                selectedBranch = branchIndex;
            }
            branchIndex++;
        }
        g_nStreamerAudioChunk_00597cc8 =
            g_pStreamerBranches_00597c8c[selectedBranch].audioChunk;
    } else {
        g_nStreamerAudioChunk_00597cc8++;
        if (g_pStreamerHeader_00597c84->audioChunkCount - 1 <=
            g_nStreamerAudioChunk_00597cc8)
            g_nStreamerAudioChunk_00597cc8 = 0;
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
        fileOffset = g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].fileOffset;
        while (remaining > 0) {
            ix_dsps_lock(0, remaining, &buffer, &lockedBytes);
            if (buffer == 0) {
                ix_log_printf("Fatal [%s - %d]:\n", IX_THREAD_FILE, 394);
                ix_log_printf("failed to lock stream buffer");
                exit(-1);
            }
            ix_file_seek(g_pStreamerPackageFile_00597bdc, fileOffset);
            ix_file_read(g_pStreamerPackageFile_00597bdc, buffer,
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

    if (g_nStreamerAudioChunk_00597cc8 != (unsigned int)-1) {
        fileOffset = g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].fileOffset;
        return g_pStreamerAudioChunks_00597c88[
            g_nStreamerAudioChunk_00597cc8].fileEnd - fileOffset;
    }
    return 0;
}

/* Function start: 0x00444700 */
unsigned int IxStreamFile::ix_stream_file_read(void *newDestination,
                                                unsigned int bytes)
{
    ix_stream_file_wait();
    EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
    if (position + bytes > size)
        bytes = size - position;
    if (bytes > 0) {
        destination = (unsigned char *)newDestination;
        remaining = bytes;
        flags |= IX_STREAM_FILE_READING;
        if (previous != 0)
            previous->next = next;
        if (next == 0)
            g_pStreamerIdleFiles_00597be0 = previous;
        else
            next->previous = previous;
        previous = g_pStreamerReadQueue_00597c6c;
        next = 0;
        if (g_pStreamerReadQueue_00597c6c != 0)
            g_pStreamerReadQueue_00597c6c->next = this;
        g_pStreamerReadQueue_00597c6c = this;
    }
    LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
    SetEvent(g_hStreamerWakeEvent_00597cd4);
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
