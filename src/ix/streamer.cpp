/**
 * @file
 * @ingroup ixlib
 * @brief Streamer API and package management.
 *
 * Originally `D:\Rnd\Prj\ix\Src\Streamer\streamer.cpp`, address range
 * 442750-443b0f. The assert paths identify the source; INT3 fill before the
 * worker thread marks the object's end.
 */
#include "ix.h"
#include <stdlib.h>

#define IX_STREAMER_FILE "D:\\Rnd\\Prj\\ix\\Src\\Streamer\\streamer.cpp"

/* Package and worker state. CL places uninitialized globals by identifier
 * hash, so these spellings reproduce the original order. */
unsigned int *pStreamerPacketOffsetTable;        /* 0x00597BD0 */
unsigned int dwStreamThreadTick;                 /* 0x00597BD4 */
/* Nothing in the shipped executable references this word. */
unsigned int dwUnusedStreamerWord;               /* 0x00597BD8 */
FILE *pStreamerPackFile;                         /* 0x00597BDC */
IxStreamFile *pStreamIdleFiles;                  /* 0x00597BE0 */
unsigned int adwStreamCallStack[32];             /* 0x00597BE8 */
unsigned char *pStreamerLzoBuffer;               /* 0x00597C68 */
IxStreamFile *pStreamReadRequests;               /* 0x00597C6C */
unsigned int dwIxStreamerAudioBufferSize;        /* 0x00597C70 */
unsigned char *pIxStreamerInputBuffer;           /* 0x00597C74 */
unsigned char bStreamerIntensityLevel;           /* 0x00597C78 */
HANDLE hStreamThread;                            /* 0x00597C7C */
IxStreamerFileEntry *pStreamFileEntries;         /* 0x00597C80 */
IxStreamerHeader *pStreamerPackageInfo;          /* 0x00597C84 */
IxStreamerAudioChunk *pStreamedMusicAudioChunks; /* 0x00597C88 */
IxStreamerBranch *pMusicStreamBranchList;        /* 0x00597C8C */
IxStreamerFileChunk *pStreamFileBlocks;          /* 0x00597C90 */
CRITICAL_SECTION csStreamerFileList;             /* 0x00597C98 */
CRITICAL_SECTION csIxStreamerThread;             /* 0x00597CB0 */
unsigned int nStreamerAudioBlock;                /* 0x00597CC8 */
DWORD dwIxStreamerThreadId;                      /* 0x00597CCC */
unsigned int dwIxStreamerStatus;                 /* 0x00597CD0 */
HANDLE hStreamerWakeupEvent;                     /* 0x00597CD4 */
unsigned int nIxStreamerCurrentFileChunk;        /* 0x00597CD8 */
unsigned int dwStreamBytesPerSecond;             /* 0x00597CDC */
CRITICAL_SECTION csStreamer;                     /* 0x00597CE0 */
IxStreamerTrigger *pStreamTriggerList;           /* 0x00597CF8 */

unsigned short nStreamerVolume = 0xffff;         /* 0x00470E84 */
char cStreamerBranchTag = -1;                    /* 0x00470E88 */
unsigned int nStreamCallStackIndex = 0;          /* 0x00470E8C */

/* Function start: 0x00442750 */   /* source line(s) 60;63;75: Streamer already inited! | Failed to init DSP | Failed to start streamer_thread! */
extern "C" int ix_streamer_init(void)
{
    if ((dwIxStreamerStatus & IX_STREAMER_INITIALIZED) != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 60);
        ix_log_printf("Streamer already inited!");
        return -1;
    }
    if (ix_dsp_init() != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 63);
        ix_log_printf("Failed to init DSP");
        return -1;
    }
    InitializeCriticalSection(&csStreamerFileList);
    InitializeCriticalSection(&csStreamer);
    InitializeCriticalSection(&csIxStreamerThread);
    hStreamerWakeupEvent = CreateEventA(0, TRUE, FALSE, 0);
    hStreamThread =
        CreateThread(0, 0x1000, ix_streamer_thread_proc, 0, 0,
                     &dwIxStreamerThreadId);
    if (hStreamThread == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 75);
        ix_log_printf("Failed to start streamer_thread!");
        exit(-1);
    }
    dwIxStreamerStatus |= IX_STREAMER_INITIALIZED;
    dwIxStreamerStatus |= IX_STREAMER_DEVELOPER_MODE;
    return 0;
}

/* Function start: 0x0044286F */   /* source line(s) 87: Streamer not ready for destroy */
extern "C" void ix_streamer_destroy(void)
{
    if ((dwIxStreamerStatus & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 87);
        ix_log_printf("Streamer not ready for destroy");
        return;
    }
    if ((dwIxStreamerStatus & IX_STREAMER_FILE_OPEN) != 0)
        ix_streamer_close_stream_file();
    dwIxStreamerStatus |= IX_STREAMER_SHUTDOWN;
    SetEvent(hStreamerWakeupEvent);
#ifdef SDL_PORT
    WaitForSingleObject(hStreamThread, INFINITE);
    CloseHandle(hStreamThread);
    hStreamThread = 0;
#else
    EnterCriticalSection(&csIxStreamerThread);
#endif
    CloseHandle(hStreamerWakeupEvent);
    DeleteCriticalSection(&csStreamerFileList);
    DeleteCriticalSection(&csStreamer);
    DeleteCriticalSection(&csIxStreamerThread);
    ix_dsp_shutdown();
    dwIxStreamerStatus &= ~IX_STREAMER_INITIALIZED;
#ifdef SDL_PORT
    dwIxStreamerStatus &=
        ~(IX_STREAMER_SHUTDOWN | IX_STREAMER_THREAD_RUNNING);
#endif
}

/* Function start: 0x0044291E */
extern "C" void ix_streamer_configure(int option, void *value)
{
    ix_dsp_configure(option, value);
}

/* Function start: 0x0044293E */   /* source line(s) 117: Can't change dev_mode while stream files are open! */
void ix_streamer_set_dev_mode(int mode)
{
    if (pStreamIdleFiles != 0 ||
        pStreamReadRequests != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 117);
        ix_log_printf("Can't change dev_mode while stream files are open!");
        exit(-1);
    }
    if (mode != 0)
        dwIxStreamerStatus |= IX_STREAMER_DEVELOPER_MODE;
    else
        dwIxStreamerStatus &= ~IX_STREAMER_DEVELOPER_MODE;
}

/* Function start: 0x004429B6 */   /* source line(s) 129;142;146;150;151;157;168;177;186;195;204;238;240: Streamer not ready to open stream file! | Failed to open stream file. | Streamer failed to */
extern "C" int ix_streamer_open_stream_file(char *path)
{
    if ((dwIxStreamerStatus & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 129);
        ix_log_printf("Streamer not ready to open stream file!");
        return -1;
    }
    if ((dwIxStreamerStatus & IX_STREAMER_FILE_OPEN) != 0)
        ix_streamer_close_stream_file();
    pStreamIdleFiles = 0;
    nStreamerAudioBlock = 0;
    nIxStreamerCurrentFileChunk = (unsigned int)-1;
    nStreamCallStackIndex = 0;
    pStreamerPackFile = ix_file_open(path, 1);
    if (pStreamerPackFile == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 142);
        ix_log_printf("Failed to open stream file.");
        return -1;
    }
    pStreamerPackageInfo = (IxStreamerHeader *)
        pIxMalloc(sizeof(IxStreamerHeader));
    if (pStreamerPackageInfo == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 146);
        ix_log_printf("Streamer failed to allocate.");
        exit(-1);
    }
    ix_file_read(pStreamerPackFile,
                 pStreamerPackageInfo, sizeof(IxStreamerHeader));
    if (pStreamerPackageInfo->id != *(unsigned int *)"STRM") {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 150);
        ix_log_printf("Invalid stream ID");
        return -1;
    }
    if (pStreamerPackageInfo->version != 1) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 151);
        ix_log_printf("Invalid stream version");
        return -1;
    }
    if (pStreamerPackageInfo->audioChunkCount != 0) {
        pStreamedMusicAudioChunks = (IxStreamerAudioChunk *)
            pIxMalloc(
                pStreamerPackageInfo->audioChunkCount *
                sizeof(IxStreamerAudioChunk));
        if (pStreamedMusicAudioChunks == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 157);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackFile,
                     pStreamerPackageInfo->audioChunkTableOffset);
        ix_file_read(pStreamerPackFile,
                     pStreamedMusicAudioChunks,
                     pStreamerPackageInfo->audioChunkCount *
                     sizeof(IxStreamerAudioChunk));
        dwIxStreamerStatus |= IX_STREAMER_HAS_AUDIO;
    }
    if (pStreamerPackageInfo->branchCount != 0) {
        pMusicStreamBranchList = (IxStreamerBranch *)
            pIxMalloc(pStreamerPackageInfo->branchCount *
                                 sizeof(IxStreamerBranch));
        if (pMusicStreamBranchList == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 168);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackFile,
                     pStreamerPackageInfo->branchTableOffset);
        ix_file_read(pStreamerPackFile,
                     pMusicStreamBranchList,
                     pStreamerPackageInfo->branchCount *
                     sizeof(IxStreamerBranch));
    }
    if (pStreamerPackageInfo->triggerCount != 0) {
        pStreamTriggerList = (IxStreamerTrigger *)
            pIxMalloc(pStreamerPackageInfo->triggerCount *
                                 sizeof(IxStreamerTrigger));
        if (pStreamTriggerList == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 177);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackFile,
                     pStreamerPackageInfo->triggerTableOffset);
        ix_file_read(pStreamerPackFile,
                     pStreamTriggerList,
                     pStreamerPackageInfo->triggerCount *
                     sizeof(IxStreamerTrigger));
    }
    if (pStreamerPackageInfo->fileEntryCount != 0) {
        pStreamFileEntries = (IxStreamerFileEntry *)
            pIxMalloc(
                pStreamerPackageInfo->fileEntryCount *
                sizeof(IxStreamerFileEntry));
        if (pStreamFileEntries == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 186);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackFile,
                     pStreamerPackageInfo->fileEntryTableOffset);
        ix_file_read(pStreamerPackFile,
                     pStreamFileEntries,
                     pStreamerPackageInfo->fileEntryCount *
                     sizeof(IxStreamerFileEntry));
    }
    if (pStreamerPackageInfo->fileChunkCount != 0) {
        pStreamFileBlocks = (IxStreamerFileChunk *)
            pIxMalloc(
                pStreamerPackageInfo->fileChunkCount *
                sizeof(IxStreamerFileChunk));
        if (pStreamFileBlocks == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 195);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackFile,
                     pStreamerPackageInfo->fileChunkTableOffset);
        ix_file_read(pStreamerPackFile,
                     pStreamFileBlocks,
                     pStreamerPackageInfo->fileChunkCount *
                     sizeof(IxStreamerFileChunk));
    }
    if (pStreamerPackageInfo->packetCount != 0) {
        pStreamerPacketOffsetTable = (unsigned int *)
            pIxMalloc(
                pStreamerPackageInfo->packetCount *
                sizeof(unsigned int));
        if (pStreamerPacketOffsetTable == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 204);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackFile,
                     pStreamerPackageInfo->packetTableOffset);
        ix_file_read(pStreamerPackFile,
                     pStreamerPacketOffsetTable,
                     pStreamerPackageInfo->packetCount *
                     sizeof(unsigned int));
    }
    if ((dwIxStreamerStatus & IX_STREAMER_HAS_AUDIO) != 0) {
        dwIxStreamerAudioBufferSize =
            pStreamerPackageInfo->audioBufferSize;
        ix_dsp_configure(0x100,
            (void *)pStreamerPackageInfo->dspConfigValue);
        ix_dsps_alloc(0, dwIxStreamerAudioBufferSize,
                      pStreamerPackageInfo->frequency,
                      pStreamerPackageInfo->bitsPerSample,
                      pStreamerPackageInfo->channels);
        ix_dsps_set_volume(0, nStreamerVolume);
        dwStreamBytesPerSecond =
            (pStreamerPackageInfo->bitsPerSample >> 3) *
            pStreamerPackageInfo->channels *
            pStreamerPackageInfo->frequency;
    }
    pStreamerLzoBuffer = (unsigned char *)
        pIxMalloc(pStreamerPackageInfo->fileBufferSize);
    if (pStreamerLzoBuffer == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 238);
        ix_log_printf("Failed to allocate file_buffer!");
        exit(-1);
    }
    pIxStreamerInputBuffer = (unsigned char *)
        pIxMalloc(pStreamerPackageInfo->fileBufferSize);
    if (pIxStreamerInputBuffer == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 240);
        ix_log_printf("Failed to allocate file_buffer!");
        exit(-1);
    }
    dwIxStreamerStatus |= IX_STREAMER_FILE_OPEN;
    SetEvent(hStreamerWakeupEvent);
    return 0;
}

/* Function start: 0x0044307A */   /* source line(s) 250;251: Streamer not ready to open stream file! | Stream file not open! */
extern "C" void ix_streamer_close_stream_file(void)
{
    IxStreamFile *streamFile;
    IxStreamFile *previousFile;

    if ((dwIxStreamerStatus & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 250);
        ix_log_printf("Streamer not ready to open stream file!");
        return;
    }
    if ((dwIxStreamerStatus & IX_STREAMER_FILE_OPEN) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 251);
        ix_log_printf("Stream file not open!");
        return;
    }
    streamFile = pStreamIdleFiles;
    while (streamFile != 0) {
        previousFile = streamFile->previous;
        ix_streamer_close_file(streamFile);
        streamFile = previousFile;
    }
    EnterCriticalSection(&csStreamer);
    dwIxStreamerStatus &= 0xffffff91;
    pIxFree(pStreamerPackageInfo);
    pIxFree(pStreamedMusicAudioChunks);
    pIxFree(pMusicStreamBranchList);
    pIxFree(pStreamTriggerList);
    pIxFree(pStreamFileEntries);
    pIxFree(pStreamFileBlocks);
    pIxFree(pStreamerPacketOffsetTable);
    pIxFree(pStreamerLzoBuffer);
    pIxFree(pIxStreamerInputBuffer);
    ix_dsps_free(0);
    ix_file_close(pStreamerPackFile);
    LeaveCriticalSection(&csStreamer);
    SetEvent(hStreamerWakeupEvent);
}

/* Function start: 0x004431F3 */   /* source line(s) 286: Stream has no audio */
extern "C" void ix_streamer_audio_play(void)
{
    if ((dwIxStreamerStatus & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 286);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwIxStreamerStatus |= IX_STREAMER_AUDIO_PLAYING;
    dwIxStreamerStatus &= ~IX_STREAMER_AUDIO_PAUSED;
    SetEvent(hStreamerWakeupEvent);
}

/* Function start: 0x00443253 */   /* source line(s) 295: Stream has no audio */
extern "C" void ix_streamer_audio_stop(void)
{
    if ((dwIxStreamerStatus & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 295);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwIxStreamerStatus &= ~IX_STREAMER_AUDIO_PLAYING;
    ix_streamer_seek_chunk(0);
    SetEvent(hStreamerWakeupEvent);
}

/* Function start: 0x004432B6 */   /* source line(s) 304: Stream has no audio */
void ix_streamer_audio_pause(void)
{
    if ((dwIxStreamerStatus & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 304);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwIxStreamerStatus |= IX_STREAMER_AUDIO_PAUSED;
    SetEvent(hStreamerWakeupEvent);
}

/* Function start: 0x0044330F */   /* source line(s) 312: Stream has no audio */
void ix_streamer_audio_reprepare(void)
{
    if ((dwIxStreamerStatus & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 312);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwIxStreamerStatus |= IX_STREAMER_REPREPARE_AUDIO;
    SetEvent(hStreamerWakeupEvent);
}

/* Function start: 0x0044336B */
extern "C" void ix_streamer_set_intensity(unsigned char intensity)
{
    if (intensity >= 0) {
        bStreamerIntensityLevel = intensity < 100 ? intensity : 100;
    } else {
        bStreamerIntensityLevel = 0;
    }
}

/* Function start: 0x004433AC */
unsigned char ix_streamer_get_intensity(void)
{
    return bStreamerIntensityLevel;
}

/* Function start: 0x004433C1 */
extern "C" void ix_streamer_set_trigger(char trigger)
{
    if (trigger == -1) {
        cStreamerBranchTag = -1;
    } else {
        if (trigger >= 0) {
            cStreamerBranchTag =
                trigger < 0x40 ? trigger : 0x40;
        } else {
            cStreamerBranchTag = 0;
        }
    }
}

/* Function start: 0x00443419 */
extern "C" char ix_streamer_get_trigger(void)
{
    return cStreamerBranchTag;
}

/* Function start: 0x0044342E */   /* source line(s) 342: Stream has no audio */
extern "C" void ix_streamer_force_trigger(char trigger)
{
    unsigned int chunk;
    unsigned int chunkCount;

    if ((dwIxStreamerStatus & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 342);
        ix_log_printf("Stream has no audio");
        return;
    }
    chunk = nStreamerAudioBlock;
    chunkCount = pStreamerPackageInfo->audioChunkCount;
    EnterCriticalSection(&csStreamer);
    while (-1 < trigger && chunkCount--) {
        if (pStreamedMusicAudioChunks[chunk].triggerCount > 0) {
            unsigned int triggerCount;
            unsigned int triggerIndex;

            triggerCount =
                pStreamedMusicAudioChunks[chunk].triggerCount;
            triggerIndex =
                pStreamedMusicAudioChunks[chunk].firstTrigger;
            while (triggerCount--) {
                if (pStreamTriggerList[triggerIndex].tag ==
                    trigger) {
                    adwStreamCallStack[
                        nStreamCallStackIndex] =
                            nStreamerAudioBlock;
                    nStreamCallStackIndex =
                        (nStreamCallStackIndex + 1) & 0x1f;
                    nStreamerAudioBlock =
                        pStreamTriggerList[
                            triggerIndex].audioChunk;
                    trigger = -1;
                    break;
                }
                triggerIndex++;
            }
        }
        chunk++;
        if (pStreamerPackageInfo->audioChunkCount - 1 <= chunk)
            chunk = 0;
    }
    if ((dwIxStreamerStatus & IX_STREAMER_DSP_PLAYING) != 0) {
        ix_dsps_stop(0);
        dwIxStreamerStatus &= ~IX_STREAMER_DSP_PLAYING;
    }
    LeaveCriticalSection(&csStreamer);
    SetEvent(hStreamerWakeupEvent);
}

/* Function start: 0x004435BE */
extern "C" void ix_streamer_set_volume(unsigned short volume)
{
    if (volume >= 0) {
        nStreamerVolume =
            volume < 0xffff ? volume : 0xffff;
    } else {
        nStreamerVolume = 0;
    }
    if ((dwIxStreamerStatus & 2) != 0) {
        EnterCriticalSection(&csStreamer);
        ix_dsps_set_volume(0, nStreamerVolume);
        LeaveCriticalSection(&csStreamer);
    }
}

/* Function start: 0x0044363B */
extern "C" unsigned short ix_streamer_get_volume(void)
{
    return nStreamerVolume;
}

/* Function start: 0x00443651 */
unsigned int ix_streamer_get_audio_chunk(void)
{
    return nStreamerAudioBlock;
}

/* Function start: 0x00443666 */
void ix_streamer_seek_chunk(unsigned int chunk)
{
    EnterCriticalSection(&csStreamer);
    if (chunk >= 0) {
        nStreamerAudioBlock =
            pStreamerPackageInfo->audioChunkCount - 1 < chunk
                ? pStreamerPackageInfo->audioChunkCount - 1
                : chunk;
    } else {
        nStreamerAudioBlock = 0;
    }
    LeaveCriticalSection(&csStreamer);
}

/* Function start: 0x004436C0 */
unsigned int ix_streamer_hash_name(unsigned char *name)
{
    int position;
    unsigned int character;
    unsigned int hash;

    position = 0;
    character = *name++;
    if (character >= 'a' && character <= 'z')
        character -= 0x20;
    hash = character << 8;
    while ((character = *name++) != 0) {
        if (character >= 'a' && character <= 'z')
            character -= 0x20;
        hash += (hash >> 4) * character;
        hash += position;
        position++;
    }
    return hash;
}

/* Function start: 0x00443755 */
IxStreamerFileEntry *ix_streamer_find_entry(unsigned int hash)
{
    unsigned int lo;
    unsigned int hi;
    unsigned int mid;
    IxStreamerFileEntry *entry;

    lo = 0;
    hi = pStreamerPackageInfo->fileEntryCount;
    while (hi > lo) {
        mid = (hi + lo) >> 1;
        entry = &pStreamFileEntries[mid];
        if (entry->nameHash < hash) {
            lo = mid + 1;
        } else if (entry->nameHash > hash) {
            hi = mid;
        } else {
            return entry;
        }
    }
    return 0;
}

/* Function start: 0x004437E3 */   /* source line(s) 512;526;535;549: open_file failed to allocate memory! | Failed to find_file in stream, %s! | open_file fail */
IxStreamFile *ix_streamer_open_file(unsigned char *name,
                                    unsigned char priority)
{
    IxStreamFile *streamFile;
    IxStreamerFileEntry *entry;
    FILE *file;

    streamFile = 0;
    if ((dwIxStreamerStatus & IX_STREAMER_DEVELOPER_MODE) == 0) {
        entry = ix_streamer_find_entry(ix_streamer_hash_name(name));
        if (entry != 0) {
            streamFile = (IxStreamFile *)
                pIxMalloc(sizeof(IxStreamFile));
            if (streamFile == 0) {
                ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 512);
                ix_log_printf("open_file failed to allocate memory!");
                exit(-1);
            }
            streamFile->flags = IX_STREAM_FILE_OPEN;
            streamFile->entry = entry;
            streamFile->priority = priority;
            streamFile->position = 0;
            streamFile->completionEvent = CreateEventA(0, TRUE, FALSE, 0);
            streamFile->size = entry->size;
            EnterCriticalSection(&csStreamerFileList);
            streamFile->previous = pStreamIdleFiles;
            streamFile->next = 0;
            if (pStreamIdleFiles != 0)
                pStreamIdleFiles->next = streamFile;
            pStreamIdleFiles = streamFile;
            LeaveCriticalSection(&csStreamerFileList);
        } else {
            ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 526);
            ix_log_printf("Failed to find_file in stream, %s!", name);
        }
    } else {
        file = ix_file_open((char *)name, 1);
        if (file != 0) {
            streamFile = (IxStreamFile *)
                pIxMalloc(sizeof(IxStreamFile));
            if (streamFile == 0) {
                ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 535);
                ix_log_printf("open_file failed to allocate memory!");
                exit(-1);
            }
            streamFile->flags = IX_STREAM_FILE_OPEN;
            streamFile->file = file;
            streamFile->size = ix_file_size(file);
            streamFile->priority = priority;
            streamFile->position = 0;
            streamFile->completionEvent = CreateEventA(0, TRUE, FALSE, 0);
            EnterCriticalSection(&csStreamerFileList);
            streamFile->previous = pStreamIdleFiles;
            streamFile->next = 0;
            if (pStreamIdleFiles != 0)
                pStreamIdleFiles->next = streamFile;
            pStreamIdleFiles = streamFile;
            LeaveCriticalSection(&csStreamerFileList);
        } else {
            ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 549);
            ix_log_printf("Failed to find_file in stream, %s!", name);
        }
    }
    return streamFile;
}

/* Function start: 0x00443A5D */
void ix_streamer_close_file(IxStreamFile *streamFile)
{
    streamFile->ix_stream_file_wait();
    EnterCriticalSection(&csStreamerFileList);
    if (streamFile->previous != 0)
        streamFile->previous->next = streamFile->next;
    if (streamFile->next == 0)
        pStreamIdleFiles = streamFile->previous;
    else
        streamFile->next->previous = streamFile->previous;
    LeaveCriticalSection(&csStreamerFileList);
    if ((dwIxStreamerStatus & IX_STREAMER_DEVELOPER_MODE) != 0)
        ix_file_close(streamFile->file);
    CloseHandle(streamFile->completionEvent);
    pIxFree(streamFile);
}
