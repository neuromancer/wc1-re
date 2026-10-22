/*
 *  ix streamer worker thread.
 *
 *  Original object range: 0x00443B10 - 0x004446FF (seven functions).
 *  INT3 fill separates this group from the streamer API and stream-file
 *  methods. The thread.cpp assert paths anchor its interior; see ORDER.md.
 */
#include "ix.h"
#include <stdlib.h>
#include <string.h>
#ifndef SDL_PORT
#include <process.h>
#include <crtdbg.h>
#endif
#include <fcntl.h>

#pragma function(memcpy)

#define IX_THREAD_FILE "D:\\Rnd\\Prj\\ix\\Src\\Streamer\\thread.cpp"

/* Function start: 0x00443B10 */
DWORD WINAPI ix_streamer_thread_proc(void *parameter)
{
    DWORD timeout;

    timeout = 0;
    EnterCriticalSection(&csIxStreamerThread);
    dwIxStreamerStatus |= IX_STREAMER_THREAD_RUNNING;
    while ((dwIxStreamerStatus & IX_STREAMER_SHUTDOWN) == 0) {
        dwStreamThreadTick++;
        EnterCriticalSection(&csStreamer);
        if ((dwIxStreamerStatus & IX_STREAMER_FILE_OPEN) != 0) {
            if ((dwIxStreamerStatus &
                 IX_STREAMER_REPREPARE_AUDIO) != 0) {
                if ((dwIxStreamerStatus &
                     IX_STREAMER_DSP_PLAYING) != 0) {
                    ix_dsps_stop(0);
                    dwIxStreamerStatus &=
                        ~IX_STREAMER_DSP_PLAYING;
                }
                ix_dsps_prepare(0);
                dwIxStreamerStatus &=
                    ~(IX_STREAMER_REPREPARE_AUDIO |
                      IX_STREAMER_END_TRIGGERED);
            }
            if ((dwIxStreamerStatus &
                 (IX_STREAMER_HAS_AUDIO | IX_STREAMER_AUDIO_PLAYING)) ==
                (IX_STREAMER_HAS_AUDIO | IX_STREAMER_AUDIO_PLAYING)) {
                timeout = ix_streamer_service_audio();
                if ((dwIxStreamerStatus &
                     IX_STREAMER_AUDIO_PAUSED) != 0) {
                    if ((dwIxStreamerStatus &
                         IX_STREAMER_DSP_PAUSED) == 0) {
                        ix_dsps_stop(0);
                        dwIxStreamerStatus |=
                            IX_STREAMER_DSP_PAUSED;
                    }
                } else {
                    if ((dwIxStreamerStatus &
                         IX_STREAMER_DSP_PAUSED) != 0) {
                        ix_dsps_play(0);
                        dwIxStreamerStatus &=
                            ~IX_STREAMER_DSP_PAUSED;
                    }
                }
            } else {
                timeout = ix_thread_service_streams();
                if ((dwIxStreamerStatus &
                     IX_STREAMER_DSP_PLAYING) != 0) {
                    ix_dsps_stop(0);
                    dwIxStreamerStatus &=
                        ~(IX_STREAMER_DSP_PLAYING |
                          IX_STREAMER_DSP_PAUSED);
                }
            }
            LeaveCriticalSection(&csStreamer);
            WaitForSingleObject(hStreamerWakeupEvent, timeout);
            ResetEvent(hStreamerWakeupEvent);
        } else {
            if ((dwIxStreamerStatus & IX_STREAMER_DSP_PLAYING) != 0) {
                ix_dsps_stop(0);
                dwIxStreamerStatus &=
                    ~(IX_STREAMER_DSP_PLAYING | IX_STREAMER_DSP_PAUSED);
            }
            LeaveCriticalSection(&csStreamer);
            WaitForSingleObject(hStreamerWakeupEvent, INFINITE);
            ResetEvent(hStreamerWakeupEvent);
        }
    }
    dwIxStreamerStatus &= ~IX_STREAMER_THREAD_RUNNING;
    LeaveCriticalSection(&csIxStreamerThread);
    (void)parameter;
    return 0;
}

/* Function start: 0x00443CC0 */
unsigned int ix_streamer_service_audio(void)
{
    unsigned int bytesUntilStop;

    if ((dwIxStreamerStatus & IX_STREAMER_END_TRIGGERED) != 0) {
        bytesUntilStop = pStreamerPackageInfo->audioBufferSize -
                         ix_dsps_get_buffer_free(0);
        Sleep((bytesUntilStop * 1000U) /
              dwStreamBytesPerSecond);
        dwIxStreamerStatus &= 0xfffff7db;
        ix_dsps_stop(0);
    } else if ((dwIxStreamerStatus &
                IX_STREAMER_DSP_PLAYING) == 0) {
        dwIxStreamerStatus |= IX_STREAMER_DSP_PLAYING;
        ix_dsps_prepare(0);
        while (ix_thread_get_audio_chunk_size() <
               (unsigned int)ix_dsps_get_buffer_free(0))
            ix_thread_lock_stream_buffer();
        ix_dsps_play(0);
    } else {
        while (ix_thread_get_audio_chunk_size() <
               (unsigned int)ix_dsps_get_buffer_free(0))
            ix_thread_lock_stream_buffer();
    }
    return ix_thread_service_streams();
}

/* Function start: 0x00443DA6 */   /* source line(s) 196: File chunk without packet! */
void ix_thread_handle_file_chunk(IxStreamFile *streamFile)
{
    unsigned int packagePosition;
    unsigned int chunkIndex;
    IxStreamerFileChunk *fileChunk;
    unsigned int chunksToScan;
    unsigned int chunkBytes;
    unsigned int innerOffset;
    IxStreamerFileEntry *packageRecord;
    unsigned int copyBytes;
    unsigned char *readPointer;
    unsigned int pivot;
    unsigned int firstPacket;
    unsigned int endPacket;
    unsigned int *packet;

    packageRecord = streamFile->entry;
    streamFile->serviceTick = dwStreamThreadTick;
    if ((dwIxStreamerStatus & 0x100) != 0) {
        ix_file_seek(streamFile->file, streamFile->position);
        ix_file_read(streamFile->file, streamFile->destination,
                     streamFile->remaining);
        EnterCriticalSection(&csStreamerFileList);
        streamFile->flags &= ~IX_STREAM_FILE_READING;
        if (streamFile->previous != 0)
            streamFile->previous->next = streamFile->next;
        if (streamFile->next == 0)
            pStreamReadRequests = streamFile->previous;
        else
            streamFile->next->previous = streamFile->previous;
        streamFile->previous = pStreamIdleFiles;
        streamFile->next = 0;
        if (pStreamIdleFiles != 0)
            pStreamIdleFiles->next = streamFile;
        pStreamIdleFiles = streamFile;
        LeaveCriticalSection(&csStreamerFileList);
        SetEvent(streamFile->completionEvent);
        return;
    }

    chunkIndex = packageRecord->firstChunk;
    chunksToScan = packageRecord->chunkCount;
    fileChunk = &pStreamFileBlocks[chunkIndex];
    packagePosition = (unsigned int)ix_file_tell(
        pStreamerPackFile);
    while (chunksToScan--) {
        if (fileChunk->fileOffset <= streamFile->position &&
            fileChunk->fileEnd > streamFile->position) {
            chunkBytes = fileChunk->fileEnd - fileChunk->fileOffset;
            if (chunkIndex != nIxStreamerCurrentFileChunk) {
                firstPacket = fileChunk->firstPacket;
                endPacket = fileChunk->packetCount + firstPacket;
                packet = 0;
                nIxStreamerCurrentFileChunk = chunkIndex;
                while (endPacket > firstPacket) {
                    pivot = (endPacket + firstPacket) >> 1;
                    packet = &pStreamerPacketOffsetTable[pivot];
                    if (*packet < packagePosition)
                        firstPacket = pivot + 1;
                    else if (*packet > packagePosition)
                        endPacket = pivot;
                    else
                        break;
                }
                if (packet == 0) {
                    ix_log_printf("Fatal [%s - %d]:\n", IX_THREAD_FILE, 196);
                    ix_log_printf("File chunk without packet!");
                    exit(-1);
                }
                if (fileChunk->packedSize < 0) {
                    ix_file_seek(pStreamerPackFile, *packet);
                    ix_file_read(pStreamerPackFile,
                                 pStreamerLzoBuffer,
                                 -fileChunk->packedSize);
#ifdef SDL_PORT
                    ix_lzo1x_decompress(
                        pStreamerLzoBuffer,
                        pIxStreamerInputBuffer, chunkBytes,
                        (unsigned int)-fileChunk->packedSize);
#else
                    ix_lzo1x_decompress(pStreamerLzoBuffer,
                                        pIxStreamerInputBuffer,
                                        chunkBytes);
#endif
                } else {
                    ix_file_seek(pStreamerPackFile, *packet);
                    ix_file_read(pStreamerPackFile,
                                 pIxStreamerInputBuffer,
                                 fileChunk->packedSize);
                }
            }
            readPointer = pIxStreamerInputBuffer;
            copyBytes = streamFile->remaining;
            innerOffset = streamFile->position - fileChunk->fileOffset;
            chunkBytes = chunkBytes - innerOffset;
            readPointer += innerOffset;
            if (copyBytes > chunkBytes)
                copyBytes = chunkBytes;
            memcpy(streamFile->destination, readPointer, copyBytes);
            streamFile->destination += copyBytes;
            streamFile->position += copyBytes;
            streamFile->remaining = streamFile->remaining - copyBytes;
            if (streamFile->remaining == 0) {
                EnterCriticalSection(&csStreamerFileList);
                if (streamFile->previous != 0)
                    streamFile->previous->next = streamFile->next;
                if (streamFile->next == 0)
                    pStreamReadRequests = streamFile->previous;
                else
                    streamFile->next->previous = streamFile->previous;
                streamFile->previous = pStreamIdleFiles;
                streamFile->next = 0;
                if (pStreamIdleFiles != 0)
                    pStreamIdleFiles->next = streamFile;
                pStreamIdleFiles = streamFile;
                streamFile->flags &= ~IX_STREAM_FILE_READING;
                LeaveCriticalSection(&csStreamerFileList);
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
    IxStreamFile *bestFile;

    EnterCriticalSection(&csStreamerFileList);
    if (pStreamReadRequests != 0) {
        streamFile = pStreamReadRequests;
        bestFile = 0;
        while (streamFile != 0) {
            if (bestFile != 0) {
                if (bestFile->priority < streamFile->priority)
                    bestFile = streamFile;
                else if (streamFile->serviceTick < bestFile->serviceTick)
                    bestFile = streamFile;
            } else {
                bestFile = streamFile;
            }
            streamFile = streamFile->previous;
        }
        LeaveCriticalSection(&csStreamerFileList);
        if (bestFile != 0)
            ix_thread_handle_file_chunk(bestFile);
    } else {
        LeaveCriticalSection(&csStreamerFileList);
    }

    if (pStreamReadRequests != 0)
        return 0;
    if ((dwIxStreamerStatus & 4) == 0 ||
        (dwIxStreamerStatus & 8) != 0)
        return 1000;
    bytesUntilRefill = ix_thread_get_audio_chunk_size() -
                       ix_dsps_get_buffer_free(0);
    if (bytesUntilRefill > 0) {
        bytesUntilRefill += 0x16f8;
        return (bytesUntilRefill * 1000U) /
               dwStreamBytesPerSecond;
    }
    return 0;
}

/* Function start: 0x00444316 */
void ix_thread_advance_audio_chunk(void)
{
    if (pStreamedMusicAudioChunks[
            nStreamerAudioBlock].triggerCount > 0) {
        unsigned int triggerIndex;
        unsigned int triggerCount;

        triggerCount = pStreamedMusicAudioChunks[
            nStreamerAudioBlock].triggerCount;
        triggerIndex = pStreamedMusicAudioChunks[
            nStreamerAudioBlock].firstTrigger;
        while (triggerCount--) {
            if (pStreamTriggerList[triggerIndex].tag == 'A') {
                nStreamCallStackIndex =
                    (nStreamCallStackIndex - 1) & 0x1f;
                nStreamerAudioBlock =
                    adwStreamCallStack[
                        nStreamCallStackIndex];
                return;
            } else if (pStreamTriggerList[triggerIndex].tag == '@') {
                dwIxStreamerStatus |= 0x800;
            } else if (pStreamTriggerList[triggerIndex].tag ==
                       cStreamerBranchTag) {
                adwStreamCallStack[
                    nStreamCallStackIndex] =
                        nStreamerAudioBlock;
                nStreamCallStackIndex =
                    (nStreamCallStackIndex + 1) & 0x1f;
                nStreamerAudioBlock =
                    pStreamTriggerList[triggerIndex].audioChunk;
                cStreamerBranchTag = -1;
                return;
            }
            triggerIndex++;
        }
    }

    if (pStreamedMusicAudioChunks[
            nStreamerAudioBlock].branchCount > 0) {
        unsigned int branchesToCheck;
        unsigned int candidateBranch;
        unsigned int bestBranch;
        unsigned int bestDistance;
        unsigned int candidateDistance;

        branchesToCheck = pStreamedMusicAudioChunks[
            nStreamerAudioBlock].branchCount - 1;
        candidateBranch = pStreamedMusicAudioChunks[
            nStreamerAudioBlock].firstBranch;
        bestDistance =
            ((pMusicStreamBranchList[candidateBranch].intensity -
              bStreamerIntensityLevel) >> 31 ^
             (pMusicStreamBranchList[candidateBranch].intensity -
              bStreamerIntensityLevel)) -
            ((pMusicStreamBranchList[candidateBranch].intensity -
              bStreamerIntensityLevel) >> 31);
        bestBranch = candidateBranch;
        candidateBranch++;
        while (branchesToCheck--) {
            candidateDistance =
                ((pMusicStreamBranchList[candidateBranch].intensity -
                  bStreamerIntensityLevel) >> 31 ^
                 (pMusicStreamBranchList[candidateBranch].intensity -
                  bStreamerIntensityLevel)) -
                ((pMusicStreamBranchList[candidateBranch].intensity -
                  bStreamerIntensityLevel) >> 31);
            if (candidateDistance < bestDistance) {
                bestDistance = candidateDistance;
                bestBranch = candidateBranch;
            }
            candidateBranch++;
        }
        nStreamerAudioBlock =
            pMusicStreamBranchList[bestBranch].audioChunk;
        return;
    }
    nStreamerAudioBlock++;
    if (pStreamerPackageInfo->audioChunkCount - 1 <=
        nStreamerAudioBlock)
        nStreamerAudioBlock = 0;
}

/* Function start: 0x004445C9 */   /* source line(s) 394: failed to lock stream buffer */
void ix_thread_lock_stream_buffer(void)
{
    unsigned int lockedBytes;
    unsigned int unreadBytes;
    unsigned int packageOffset;
    unsigned char *buffer;

    unreadBytes = ix_thread_get_audio_chunk_size();
    if (unreadBytes > 0) {
        packageOffset = pStreamedMusicAudioChunks[
            nStreamerAudioBlock].fileOffset;
        while (unreadBytes > 0) {
            ix_dsps_lock(0, unreadBytes, &buffer, &lockedBytes);
            if (buffer == 0) {
                ix_log_printf("Fatal [%s - %d]:\n", IX_THREAD_FILE, 394);
                ix_log_printf("failed to lock stream buffer");
                exit(-1);
            }
            ix_file_seek(pStreamerPackFile, packageOffset);
            ix_file_read(pStreamerPackFile, buffer,
                         lockedBytes);
            unreadBytes = unreadBytes - lockedBytes;
            packageOffset += lockedBytes;
            ix_dsps_unlock(0);
        }
    }
    ix_thread_advance_audio_chunk();
}

/* Function start: 0x004446A6 */
unsigned int ix_thread_get_audio_chunk_size(void)
{
    unsigned int fileOffset;

    if (nStreamerAudioBlock != (unsigned int)-1) {
        fileOffset = pStreamedMusicAudioChunks[
            nStreamerAudioBlock].fileOffset;
        return pStreamedMusicAudioChunks[
            nStreamerAudioBlock].fileEnd - fileOffset;
    }
    return 0;
}
