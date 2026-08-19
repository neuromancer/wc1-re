/*
 *  ix / D:\Rnd\Prj\ix\Src\Streamer\streamer.cpp
 *
 *  Original address range: 0x00442750 - 0x00443DA5   (25 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"
#include <stdlib.h>

#define IX_STREAMER_FILE "D:\\Rnd\\Prj\\ix\\Src\\Streamer\\streamer.cpp"

unsigned int *pStreamerPacketOffsets;
unsigned int dwStreamerThreadTick;
FILE *pStreamerPackageFile;
IxStreamFile *pStreamerIdleFiles;
unsigned int adwStreamerBranchStack[32];
unsigned char *pStreamerCompressedBuffer;
IxStreamFile *pStreamerReadQueue;
unsigned int nStreamerAudioBufferSize;
unsigned char *pStreamerFileBuffer;
unsigned char bStreamerIntensity;
HANDLE hStreamerThread;
IxStreamerFileEntry *pStreamerFileEntries;
IxStreamerHeader *pStreamerHeader;
IxStreamerAudioChunk *pStreamerAudioChunks;
IxStreamerBranch *pStreamerBranches;
IxStreamerFileChunk *pStreamerFileChunks;
CRITICAL_SECTION csStreamerFileQueue;
CRITICAL_SECTION csStreamerThread;
unsigned int nStreamerAudioChunk;
DWORD dwStreamerThreadId;
extern "C" unsigned int dwStreamerState = 0;
HANDLE hStreamerWakeEvent;
unsigned int nStreamerFileChunk;
unsigned int nStreamerBytesPerSecond;
CRITICAL_SECTION csStreamer;
IxStreamerTrigger *pStreamerTriggers;

unsigned short nStreamerVolume = 0xffff;
char cStreamerBranchTag = -1;
unsigned int nStreamerBranchStackIndex = 0;
unsigned int dwStreamerId = 0x4d525453;

/* Function start: 0x00442750 */   /* source line(s) 60;63;75: Streamer already inited! | Failed to init DSP | Failed to start streamer_thread! */
extern "C" int ix_streamer_init(void)
{
    if ((dwStreamerState & IX_STREAMER_INITIALIZED) != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 60);
        ix_log_printf("Streamer already inited!");
        return -1;
    }
    if (ix_dsp_init() != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 63);
        ix_log_printf("Failed to init DSP");
        return -1;
    }
    InitializeCriticalSection(&csStreamerFileQueue);
    InitializeCriticalSection(&csStreamer);
    InitializeCriticalSection(&csStreamerThread);
    hStreamerWakeEvent = CreateEventA(0, TRUE, FALSE, 0);
    hStreamerThread =
        CreateThread(0, 0x1000, ix_streamer_thread_proc, 0, 0,
                     &dwStreamerThreadId);
    if (hStreamerThread == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 75);
        ix_log_printf("Failed to start streamer_thread!");
        exit(-1);
    }
    dwStreamerState |= IX_STREAMER_INITIALIZED;
    dwStreamerState |= IX_STREAMER_DEVELOPER_MODE;
    return 0;
}

/* Function start: 0x0044286F */   /* source line(s) 87: Streamer not ready for destroy */
#ifdef WC1_SDL
extern "C"
#endif
void ix_streamer_destroy(void)
{
    if ((dwStreamerState & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 87);
        ix_log_printf("Streamer not ready for destroy");
        return;
    }
    if ((dwStreamerState & IX_STREAMER_FILE_OPEN) != 0)
        ix_streamer_close_stream_file();
    dwStreamerState |= IX_STREAMER_SHUTDOWN;
    SetEvent(hStreamerWakeEvent);
#ifdef WC1_SDL
    WaitForSingleObject(hStreamerThread, INFINITE);
    CloseHandle(hStreamerThread);
    hStreamerThread = 0;
#else
    EnterCriticalSection(&csStreamerThread);
#endif
    CloseHandle(hStreamerWakeEvent);
    DeleteCriticalSection(&csStreamerFileQueue);
    DeleteCriticalSection(&csStreamer);
    DeleteCriticalSection(&csStreamerThread);
    ix_dsp_shutdown();
    dwStreamerState &= ~IX_STREAMER_INITIALIZED;
#ifdef WC1_SDL
    dwStreamerState &=
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
    if (pStreamerIdleFiles != 0 ||
        pStreamerReadQueue != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 117);
        ix_log_printf("Can't change dev_mode while stream files are open!");
        exit(-1);
    }
    if (mode != 0)
        dwStreamerState |= IX_STREAMER_DEVELOPER_MODE;
    else
        dwStreamerState &= ~IX_STREAMER_DEVELOPER_MODE;
}

/* Function start: 0x004429B6 */   /* source line(s) 129;142;146;150;151;157;168;177;186;195;204;238;240: Streamer not ready to open stream file! | Failed to open stream file. | Streamer failed to */
extern "C" int ix_streamer_open_stream_file(char *path)
{
    if ((dwStreamerState & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 129);
        ix_log_printf("Streamer not ready to open stream file!");
        return -1;
    }
    if ((dwStreamerState & IX_STREAMER_FILE_OPEN) != 0)
        ix_streamer_close_stream_file();
    pStreamerIdleFiles = 0;
    nStreamerAudioChunk = 0;
    nStreamerFileChunk = (unsigned int)-1;
    nStreamerBranchStackIndex = 0;
    pStreamerPackageFile = ix_file_open(path, 1);
    if (pStreamerPackageFile == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 142);
        ix_log_printf("Failed to open stream file.");
        return -1;
    }
    pStreamerHeader = (IxStreamerHeader *)
        pIxMalloc(sizeof(IxStreamerHeader));
    if (pStreamerHeader == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 146);
        ix_log_printf("Streamer failed to allocate.");
        exit(-1);
    }
    ix_file_read(pStreamerPackageFile,
                 pStreamerHeader, sizeof(IxStreamerHeader));
    if (pStreamerHeader->id != dwStreamerId) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 150);
        ix_log_printf("Invalid stream ID");
        return -1;
    }
    if (pStreamerHeader->version != 1) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 151);
        ix_log_printf("Invalid stream version");
        return -1;
    }
    if (pStreamerHeader->audioChunkCount != 0) {
        pStreamerAudioChunks = (IxStreamerAudioChunk *)
            pIxMalloc(
                pStreamerHeader->audioChunkCount *
                sizeof(IxStreamerAudioChunk));
        if (pStreamerAudioChunks == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 157);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackageFile,
                     pStreamerHeader->audioChunkTableOffset);
        ix_file_read(pStreamerPackageFile,
                     pStreamerAudioChunks,
                     pStreamerHeader->audioChunkCount *
                     sizeof(IxStreamerAudioChunk));
        dwStreamerState |= IX_STREAMER_HAS_AUDIO;
    }
    if (pStreamerHeader->branchCount != 0) {
        pStreamerBranches = (IxStreamerBranch *)
            pIxMalloc(pStreamerHeader->branchCount *
                                 sizeof(IxStreamerBranch));
        if (pStreamerBranches == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 168);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackageFile,
                     pStreamerHeader->branchTableOffset);
        ix_file_read(pStreamerPackageFile,
                     pStreamerBranches,
                     pStreamerHeader->branchCount *
                     sizeof(IxStreamerBranch));
    }
    if (pStreamerHeader->triggerCount != 0) {
        pStreamerTriggers = (IxStreamerTrigger *)
            pIxMalloc(pStreamerHeader->triggerCount *
                                 sizeof(IxStreamerTrigger));
        if (pStreamerTriggers == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 177);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackageFile,
                     pStreamerHeader->triggerTableOffset);
        ix_file_read(pStreamerPackageFile,
                     pStreamerTriggers,
                     pStreamerHeader->triggerCount *
                     sizeof(IxStreamerTrigger));
    }
    if (pStreamerHeader->fileEntryCount != 0) {
        pStreamerFileEntries = (IxStreamerFileEntry *)
            pIxMalloc(
                pStreamerHeader->fileEntryCount *
                sizeof(IxStreamerFileEntry));
        if (pStreamerFileEntries == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 186);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackageFile,
                     pStreamerHeader->fileEntryTableOffset);
        ix_file_read(pStreamerPackageFile,
                     pStreamerFileEntries,
                     pStreamerHeader->fileEntryCount *
                     sizeof(IxStreamerFileEntry));
    }
    if (pStreamerHeader->fileChunkCount != 0) {
        pStreamerFileChunks = (IxStreamerFileChunk *)
            pIxMalloc(
                pStreamerHeader->fileChunkCount *
                sizeof(IxStreamerFileChunk));
        if (pStreamerFileChunks == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 195);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackageFile,
                     pStreamerHeader->fileChunkTableOffset);
        ix_file_read(pStreamerPackageFile,
                     pStreamerFileChunks,
                     pStreamerHeader->fileChunkCount *
                     sizeof(IxStreamerFileChunk));
    }
    if (pStreamerHeader->packetCount != 0) {
        pStreamerPacketOffsets = (unsigned int *)
            pIxMalloc(
                pStreamerHeader->packetCount *
                sizeof(unsigned int));
        if (pStreamerPacketOffsets == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 204);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(pStreamerPackageFile,
                     pStreamerHeader->packetTableOffset);
        ix_file_read(pStreamerPackageFile,
                     pStreamerPacketOffsets,
                     pStreamerHeader->packetCount *
                     sizeof(unsigned int));
    }
    if ((dwStreamerState & IX_STREAMER_HAS_AUDIO) != 0) {
        nStreamerAudioBufferSize =
            pStreamerHeader->audioBufferSize;
        ix_dsp_configure(0x100,
            (void *)pStreamerHeader->dspConfigValue);
        ix_dsps_alloc(0, nStreamerAudioBufferSize,
                      pStreamerHeader->frequency,
                      pStreamerHeader->bitsPerSample,
                      pStreamerHeader->channels);
        ix_dsps_set_volume(0, nStreamerVolume);
        nStreamerBytesPerSecond =
            (pStreamerHeader->bitsPerSample >> 3) *
            pStreamerHeader->channels *
            pStreamerHeader->frequency;
    }
    pStreamerCompressedBuffer = (unsigned char *)
        pIxMalloc(pStreamerHeader->fileBufferSize);
    if (pStreamerCompressedBuffer == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 238);
        ix_log_printf("Failed to allocate file_buffer!");
        exit(-1);
    }
    pStreamerFileBuffer = (unsigned char *)
        pIxMalloc(pStreamerHeader->fileBufferSize);
    if (pStreamerFileBuffer == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 240);
        ix_log_printf("Failed to allocate file_buffer!");
        exit(-1);
    }
    dwStreamerState |= IX_STREAMER_FILE_OPEN;
    SetEvent(hStreamerWakeEvent);
    return 0;
}

/* Function start: 0x0044307A */   /* source line(s) 250;251: Streamer not ready to open stream file! | Stream file not open! */
extern "C" void ix_streamer_close_stream_file(void)
{
    IxStreamFile *streamFile;
    IxStreamFile *previous;

    if ((dwStreamerState & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 250);
        ix_log_printf("Streamer not ready to open stream file!");
        return;
    }
    if ((dwStreamerState & IX_STREAMER_FILE_OPEN) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 251);
        ix_log_printf("Stream file not open!");
        return;
    }
    streamFile = pStreamerIdleFiles;
    while (streamFile != 0) {
        previous = streamFile->previous;
        ix_streamer_close_file(streamFile);
        streamFile = previous;
    }
    EnterCriticalSection(&csStreamer);
    dwStreamerState &= 0xffffff91;
    pIxFree(pStreamerHeader);
    pIxFree(pStreamerAudioChunks);
    pIxFree(pStreamerBranches);
    pIxFree(pStreamerTriggers);
    pIxFree(pStreamerFileEntries);
    pIxFree(pStreamerFileChunks);
    pIxFree(pStreamerPacketOffsets);
    pIxFree(pStreamerCompressedBuffer);
    pIxFree(pStreamerFileBuffer);
    ix_dsps_free(0);
    ix_file_close(pStreamerPackageFile);
    LeaveCriticalSection(&csStreamer);
    SetEvent(hStreamerWakeEvent);
}

/* Function start: 0x004431F3 */   /* source line(s) 286: Stream has no audio */
extern "C" void ix_streamer_audio_play(void)
{
    if ((dwStreamerState & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 286);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwStreamerState |= IX_STREAMER_AUDIO_PLAYING;
    dwStreamerState &= ~IX_STREAMER_AUDIO_PAUSED;
    SetEvent(hStreamerWakeEvent);
}

/* Function start: 0x00443253 */   /* source line(s) 295: Stream has no audio */
extern "C" void ix_streamer_audio_stop(void)
{
    if ((dwStreamerState & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 295);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwStreamerState &= ~IX_STREAMER_AUDIO_PLAYING;
    ix_streamer_seek_chunk(0);
    SetEvent(hStreamerWakeEvent);
}

/* Function start: 0x004432B6 */   /* source line(s) 304: Stream has no audio */
void ix_streamer_audio_pause(void)
{
    if ((dwStreamerState & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 304);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwStreamerState |= IX_STREAMER_AUDIO_PAUSED;
    SetEvent(hStreamerWakeEvent);
}

/* Function start: 0x0044330F */   /* source line(s) 312: Stream has no audio */
void ix_streamer_audio_reprepare(void)
{
    if ((dwStreamerState & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 312);
        ix_log_printf("Stream has no audio");
        return;
    }
    dwStreamerState |= IX_STREAMER_REPREPARE_AUDIO;
    SetEvent(hStreamerWakeEvent);
}

/* Function start: 0x0044336B */
extern "C" void ix_streamer_set_intensity(unsigned char intensity)
{
    if (intensity >= 0) {
        bStreamerIntensity = intensity < 100 ? intensity : 100;
    } else {
        bStreamerIntensity = 0;
    }
}

/* Function start: 0x004433AC */
unsigned char ix_streamer_get_intensity(void)
{
    return bStreamerIntensity;
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
char ix_streamer_get_trigger(void)
{
    return cStreamerBranchTag;
}

/* Function start: 0x0044342E */   /* source line(s) 342: Stream has no audio */
extern "C" void ix_streamer_force_trigger(char trigger)
{
    unsigned int triggerCount;
    unsigned int chunk;
    unsigned int chunkCount;
    unsigned int triggerIndex;

    if ((dwStreamerState & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 342);
        ix_log_printf("Stream has no audio");
        return;
    }
    chunk = nStreamerAudioChunk;
    chunkCount = pStreamerHeader->audioChunkCount;
    EnterCriticalSection(&csStreamer);
    while (-1 < trigger && chunkCount--) {
        if (pStreamerAudioChunks[chunk].triggerCount > 0) {
            triggerCount =
                pStreamerAudioChunks[chunk].triggerCount;
            triggerIndex =
                pStreamerAudioChunks[chunk].firstTrigger;
            while (triggerCount--) {
                if (pStreamerTriggers[triggerIndex].tag ==
                    trigger) {
                    adwStreamerBranchStack[
                        nStreamerBranchStackIndex] =
                            nStreamerAudioChunk;
                    nStreamerBranchStackIndex =
                        (nStreamerBranchStackIndex + 1) & 0x1f;
                    nStreamerAudioChunk =
                        pStreamerTriggers[
                            triggerIndex].audioChunk;
                    trigger = -1;
                    break;
                }
                triggerIndex++;
            }
        }
        chunk++;
        if (pStreamerHeader->audioChunkCount - 1 <= chunk)
            chunk = 0;
    }
    if ((dwStreamerState & IX_STREAMER_DSP_PLAYING) != 0) {
        ix_dsps_stop(0);
        dwStreamerState &= ~IX_STREAMER_DSP_PLAYING;
    }
    LeaveCriticalSection(&csStreamer);
    SetEvent(hStreamerWakeEvent);
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
    if ((dwStreamerState & 2) != 0) {
        EnterCriticalSection(&csStreamer);
        ix_dsps_set_volume(0, nStreamerVolume);
        LeaveCriticalSection(&csStreamer);
    }
}

/* Function start: 0x0044363B */
unsigned short ix_streamer_get_volume(void)
{
    return nStreamerVolume;
}

/* Function start: 0x00443651 */
unsigned int ix_streamer_get_audio_chunk(void)
{
    return nStreamerAudioChunk;
}

/* Function start: 0x00443666 */
void ix_streamer_seek_chunk(unsigned int chunk)
{
    EnterCriticalSection(&csStreamer);
    if (chunk >= 0) {
        nStreamerAudioChunk =
            pStreamerHeader->audioChunkCount - 1 < chunk
                ? pStreamerHeader->audioChunkCount - 1
                : chunk;
    } else {
        nStreamerAudioChunk = 0;
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
    unsigned int last;
    IxStreamerFileEntry *entry;
    unsigned int first;
    unsigned int middle;

    first = 0;
    last = pStreamerHeader->fileEntryCount;
    while (last > first) {
        middle = (first + last) >> 1;
        entry = &pStreamerFileEntries[middle];
        if (entry->nameHash < hash) {
            first = middle + 1;
        } else if (entry->nameHash > hash) {
            last = middle;
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
    if ((dwStreamerState & IX_STREAMER_DEVELOPER_MODE) == 0) {
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
            EnterCriticalSection(&csStreamerFileQueue);
            streamFile->previous = pStreamerIdleFiles;
            streamFile->next = 0;
            if (pStreamerIdleFiles != 0)
                pStreamerIdleFiles->next = streamFile;
            pStreamerIdleFiles = streamFile;
            LeaveCriticalSection(&csStreamerFileQueue);
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
            EnterCriticalSection(&csStreamerFileQueue);
            streamFile->previous = pStreamerIdleFiles;
            streamFile->next = 0;
            if (pStreamerIdleFiles != 0)
                pStreamerIdleFiles->next = streamFile;
            pStreamerIdleFiles = streamFile;
            LeaveCriticalSection(&csStreamerFileQueue);
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
    EnterCriticalSection(&csStreamerFileQueue);
    if (streamFile->previous != 0)
        streamFile->previous->next = streamFile->next;
    if (streamFile->next == 0)
        pStreamerIdleFiles = streamFile->previous;
    else
        streamFile->next->previous = streamFile->previous;
    LeaveCriticalSection(&csStreamerFileQueue);
    if ((dwStreamerState & IX_STREAMER_DEVELOPER_MODE) != 0)
        ix_file_close(streamFile->file);
    CloseHandle(streamFile->completionEvent);
    pIxFree(streamFile);
}

/* Function start: 0x00443B10 */
DWORD WINAPI ix_streamer_thread_proc(void *parameter)
{
    DWORD timeout;

    timeout = 0;
    EnterCriticalSection(&csStreamerThread);
    dwStreamerState |= IX_STREAMER_THREAD_RUNNING;
    while ((dwStreamerState & IX_STREAMER_SHUTDOWN) == 0) {
        dwStreamerThreadTick++;
        EnterCriticalSection(&csStreamer);
        if ((dwStreamerState & IX_STREAMER_FILE_OPEN) != 0) {
            if ((dwStreamerState &
                 IX_STREAMER_REPREPARE_AUDIO) != 0) {
                if ((dwStreamerState &
                     IX_STREAMER_DSP_PLAYING) != 0) {
                    ix_dsps_stop(0);
                    dwStreamerState &=
                        ~IX_STREAMER_DSP_PLAYING;
                }
                ix_dsps_prepare(0);
                dwStreamerState &=
                    ~(IX_STREAMER_REPREPARE_AUDIO |
                      IX_STREAMER_END_TRIGGERED);
            }
            if ((dwStreamerState &
                 (IX_STREAMER_HAS_AUDIO | IX_STREAMER_AUDIO_PLAYING)) ==
                (IX_STREAMER_HAS_AUDIO | IX_STREAMER_AUDIO_PLAYING)) {
                timeout = ix_streamer_service_audio();
                if ((dwStreamerState &
                     IX_STREAMER_AUDIO_PAUSED) != 0) {
                    if ((dwStreamerState &
                         IX_STREAMER_DSP_PAUSED) == 0) {
                        ix_dsps_stop(0);
                        dwStreamerState |=
                            IX_STREAMER_DSP_PAUSED;
                    }
                } else {
                    if ((dwStreamerState &
                         IX_STREAMER_DSP_PAUSED) != 0) {
                        ix_dsps_play(0);
                        dwStreamerState &=
                            ~IX_STREAMER_DSP_PAUSED;
                    }
                }
            } else {
                timeout = ix_thread_service_streams();
                if ((dwStreamerState &
                     IX_STREAMER_DSP_PLAYING) != 0) {
                    ix_dsps_stop(0);
                    dwStreamerState &=
                        ~(IX_STREAMER_DSP_PLAYING |
                          IX_STREAMER_DSP_PAUSED);
                }
            }
            LeaveCriticalSection(&csStreamer);
            WaitForSingleObject(hStreamerWakeEvent, timeout);
            ResetEvent(hStreamerWakeEvent);
        } else {
            if ((dwStreamerState & IX_STREAMER_DSP_PLAYING) != 0) {
                ix_dsps_stop(0);
                dwStreamerState &=
                    ~(IX_STREAMER_DSP_PLAYING | IX_STREAMER_DSP_PAUSED);
            }
            LeaveCriticalSection(&csStreamer);
            WaitForSingleObject(hStreamerWakeEvent, INFINITE);
            ResetEvent(hStreamerWakeEvent);
        }
    }
    dwStreamerState &= ~IX_STREAMER_THREAD_RUNNING;
    LeaveCriticalSection(&csStreamerThread);
    (void)parameter;
    return 0;
}

/* Function start: 0x00443CC0 */
unsigned int ix_streamer_service_audio(void)
{
    unsigned int bytesUntilStop;
    register unsigned int chunkSize;

    if ((dwStreamerState & IX_STREAMER_END_TRIGGERED) != 0) {
        bytesUntilStop = pStreamerHeader->audioBufferSize -
                         ix_dsps_get_buffer_free(0);
        Sleep((bytesUntilStop * 1000U) /
              nStreamerBytesPerSecond);
        dwStreamerState &= 0xfffff7db;
        ix_dsps_stop(0);
    } else if ((dwStreamerState &
                IX_STREAMER_DSP_PLAYING) == 0) {
        dwStreamerState |= IX_STREAMER_DSP_PLAYING;
        ix_dsps_prepare(0);
        for (;;) {
            chunkSize = ix_thread_get_audio_chunk_size();
            if (chunkSize >= (unsigned int)ix_dsps_get_buffer_free(0))
                break;
            ix_thread_lock_stream_buffer();
        }
        ix_dsps_play(0);
    } else {
        for (;;) {
            chunkSize = ix_thread_get_audio_chunk_size();
            if (chunkSize >= (unsigned int)ix_dsps_get_buffer_free(0))
                break;
            ix_thread_lock_stream_buffer();
        }
    }
    return ix_thread_service_streams();
}
