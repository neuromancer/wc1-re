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

unsigned int *g_pStreamerPacketOffsets_00597bd0;
unsigned int g_dwStreamerThreadTick_00597bd4;
FILE *g_pStreamerPackageFile_00597bdc;
IxStreamFile *g_pStreamerIdleFiles_00597be0;
unsigned int g_adwStreamerBranchStack_00597be8[32];
unsigned char *g_pStreamerCompressedBuffer_00597c68;
IxStreamFile *g_pStreamerReadQueue_00597c6c;
unsigned int g_nStreamerAudioBufferSize_00597c70;
unsigned char *g_pStreamerFileBuffer_00597c74;
unsigned char g_bStreamerIntensity_00597c78;
HANDLE g_hStreamerThread_00597c7c;
IxStreamerFileEntry *g_pStreamerFileEntries_00597c80;
IxStreamerHeader *g_pStreamerHeader_00597c84;
IxStreamerAudioChunk *g_pStreamerAudioChunks_00597c88;
IxStreamerBranch *g_pStreamerBranches_00597c8c;
IxStreamerFileChunk *g_pStreamerFileChunks_00597c90;
CRITICAL_SECTION g_csStreamerFileQueue_00597c98;
CRITICAL_SECTION g_csStreamerThread_00597cb0;
unsigned int g_nStreamerAudioChunk_00597cc8;
DWORD g_dwStreamerThreadId_00597ccc;
extern "C" unsigned int g_dwStreamerState_00597cd0 = 0;
HANDLE g_hStreamerWakeEvent_00597cd4;
unsigned int g_nStreamerFileChunk_00597cd8;
unsigned int g_nStreamerBytesPerSecond_00597cdc;
CRITICAL_SECTION g_csStreamer_00597ce0;
IxStreamerTrigger *g_pStreamerTriggers_00597cf8;

unsigned short g_nStreamerVolume_00470e84 = 0xffff;
char g_cStreamerBranchTag_00470e88 = -1;
unsigned int g_nStreamerBranchStackIndex_00470e8c = 0;
unsigned int g_dwStreamerId_0047117c = 0x4d525453;

/* Function start: 0x46BD50 */   /* source line(s) 60;63;75: Streamer already inited! | Failed to init DSP | Failed to start streamer_thread! */
extern "C" int ix_streamer_init(void)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_INITIALIZED) != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 60);
        ix_log_printf("Streamer already inited!");
        return -1;
    }
    if (ix_dsp_init() != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 63);
        ix_log_printf("Failed to init DSP");
        return -1;
    }
    InitializeCriticalSection(&g_csStreamerFileQueue_00597c98);
    InitializeCriticalSection(&g_csStreamer_00597ce0);
    InitializeCriticalSection(&g_csStreamerThread_00597cb0);
    g_hStreamerWakeEvent_00597cd4 = CreateEventA(0, TRUE, FALSE, 0);
    g_hStreamerThread_00597c7c =
        CreateThread(0, 0x1000, ix_streamer_thread_proc, 0, 0,
                     &g_dwStreamerThreadId_00597ccc);
    if (g_hStreamerThread_00597c7c == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 75);
        ix_log_printf("Failed to start streamer_thread!");
        exit(-1);
    }
    g_dwStreamerState_00597cd0 |= IX_STREAMER_INITIALIZED;
    g_dwStreamerState_00597cd0 |= IX_STREAMER_DEVELOPER_MODE;
    return 0;
}

/* Function start: 0x46BE6F */   /* source line(s) 87: Streamer not ready for destroy */
#ifdef WC1_SDL
extern "C"
#endif
void ix_streamer_destroy(void)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 87);
        ix_log_printf("Streamer not ready for destroy");
        return;
    }
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_FILE_OPEN) != 0)
        ix_streamer_close_stream_file();
    g_dwStreamerState_00597cd0 |= IX_STREAMER_SHUTDOWN;
    SetEvent(g_hStreamerWakeEvent_00597cd4);
#ifdef WC1_SDL
    WaitForSingleObject(g_hStreamerThread_00597c7c, INFINITE);
    CloseHandle(g_hStreamerThread_00597c7c);
    g_hStreamerThread_00597c7c = 0;
#else
    EnterCriticalSection(&g_csStreamerThread_00597cb0);
#endif
    CloseHandle(g_hStreamerWakeEvent_00597cd4);
    DeleteCriticalSection(&g_csStreamerFileQueue_00597c98);
    DeleteCriticalSection(&g_csStreamer_00597ce0);
    DeleteCriticalSection(&g_csStreamerThread_00597cb0);
    ix_dsp_shutdown();
    g_dwStreamerState_00597cd0 &= ~IX_STREAMER_INITIALIZED;
#ifdef WC1_SDL
    g_dwStreamerState_00597cd0 &=
        ~(IX_STREAMER_SHUTDOWN | IX_STREAMER_THREAD_RUNNING);
#endif
}

/* Function start: 0x469EA6 */
extern "C" void ix_streamer_configure(int option, void *value)
{
    ix_dsp_configure(option, value);
}

/* Function start: 0x46BF3E */   /* source line(s) 117: Can't change dev_mode while stream files are open! */
void ix_streamer_set_dev_mode(int mode)
{
    if (g_pStreamerIdleFiles_00597be0 != 0 ||
        g_pStreamerReadQueue_00597c6c != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 117);
        ix_log_printf("Can't change dev_mode while stream files are open!");
        exit(-1);
    }
    if (mode != 0)
        g_dwStreamerState_00597cd0 |= IX_STREAMER_DEVELOPER_MODE;
    else
        g_dwStreamerState_00597cd0 &= ~IX_STREAMER_DEVELOPER_MODE;
}

/* Function start: 0x46BFB6 */   /* source line(s) 129;142;146;150;151;157;168;177;186;195;204;238;240: Streamer not ready to open stream file! | Failed to open stream file. | Streamer failed to */
extern "C" int ix_streamer_open_stream_file(char *path)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 129);
        ix_log_printf("Streamer not ready to open stream file!");
        return -1;
    }
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_FILE_OPEN) != 0)
        ix_streamer_close_stream_file();
    g_pStreamerIdleFiles_00597be0 = 0;
    g_nStreamerAudioChunk_00597cc8 = 0;
    g_nStreamerFileChunk_00597cd8 = (unsigned int)-1;
    g_nStreamerBranchStackIndex_00470e8c = 0;
    g_pStreamerPackageFile_00597bdc = ix_file_open(path, 1);
    if (g_pStreamerPackageFile_00597bdc == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 142);
        ix_log_printf("Failed to open stream file.");
        return -1;
    }
    g_pStreamerHeader_00597c84 = (IxStreamerHeader *)
        g_pIxMalloc_00471990(sizeof(IxStreamerHeader));
    if (g_pStreamerHeader_00597c84 == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 146);
        ix_log_printf("Streamer failed to allocate.");
        exit(-1);
    }
    ix_file_read(g_pStreamerPackageFile_00597bdc,
                 g_pStreamerHeader_00597c84, sizeof(IxStreamerHeader));
    if (g_pStreamerHeader_00597c84->id != g_dwStreamerId_0047117c) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 150);
        ix_log_printf("Invalid stream ID");
        return -1;
    }
    if (g_pStreamerHeader_00597c84->version != 1) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 151);
        ix_log_printf("Invalid stream version");
        return -1;
    }
    if (g_pStreamerHeader_00597c84->audioChunkCount != 0) {
        g_pStreamerAudioChunks_00597c88 = (IxStreamerAudioChunk *)
            g_pIxMalloc_00471990(
                g_pStreamerHeader_00597c84->audioChunkCount *
                sizeof(IxStreamerAudioChunk));
        if (g_pStreamerAudioChunks_00597c88 == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 157);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerHeader_00597c84->audioChunkTableOffset);
        ix_file_read(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerAudioChunks_00597c88,
                     g_pStreamerHeader_00597c84->audioChunkCount *
                     sizeof(IxStreamerAudioChunk));
        g_dwStreamerState_00597cd0 |= IX_STREAMER_HAS_AUDIO;
    }
    if (g_pStreamerHeader_00597c84->branchCount != 0) {
        g_pStreamerBranches_00597c8c = (IxStreamerBranch *)
            g_pIxMalloc_00471990(g_pStreamerHeader_00597c84->branchCount *
                                 sizeof(IxStreamerBranch));
        if (g_pStreamerBranches_00597c8c == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 168);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerHeader_00597c84->branchTableOffset);
        ix_file_read(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerBranches_00597c8c,
                     g_pStreamerHeader_00597c84->branchCount *
                     sizeof(IxStreamerBranch));
    }
    if (g_pStreamerHeader_00597c84->triggerCount != 0) {
        g_pStreamerTriggers_00597cf8 = (IxStreamerTrigger *)
            g_pIxMalloc_00471990(g_pStreamerHeader_00597c84->triggerCount *
                                 sizeof(IxStreamerTrigger));
        if (g_pStreamerTriggers_00597cf8 == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 177);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerHeader_00597c84->triggerTableOffset);
        ix_file_read(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerTriggers_00597cf8,
                     g_pStreamerHeader_00597c84->triggerCount *
                     sizeof(IxStreamerTrigger));
    }
    if (g_pStreamerHeader_00597c84->fileEntryCount != 0) {
        g_pStreamerFileEntries_00597c80 = (IxStreamerFileEntry *)
            g_pIxMalloc_00471990(
                g_pStreamerHeader_00597c84->fileEntryCount *
                sizeof(IxStreamerFileEntry));
        if (g_pStreamerFileEntries_00597c80 == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 186);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerHeader_00597c84->fileEntryTableOffset);
        ix_file_read(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerFileEntries_00597c80,
                     g_pStreamerHeader_00597c84->fileEntryCount *
                     sizeof(IxStreamerFileEntry));
    }
    if (g_pStreamerHeader_00597c84->fileChunkCount != 0) {
        g_pStreamerFileChunks_00597c90 = (IxStreamerFileChunk *)
            g_pIxMalloc_00471990(
                g_pStreamerHeader_00597c84->fileChunkCount *
                sizeof(IxStreamerFileChunk));
        if (g_pStreamerFileChunks_00597c90 == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 195);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerHeader_00597c84->fileChunkTableOffset);
        ix_file_read(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerFileChunks_00597c90,
                     g_pStreamerHeader_00597c84->fileChunkCount *
                     sizeof(IxStreamerFileChunk));
    }
    if (g_pStreamerHeader_00597c84->packetCount != 0) {
        g_pStreamerPacketOffsets_00597bd0 = (unsigned int *)
            g_pIxMalloc_00471990(
                g_pStreamerHeader_00597c84->packetCount *
                sizeof(unsigned int));
        if (g_pStreamerPacketOffsets_00597bd0 == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 204);
            ix_log_printf("Streamer failed to allocate.");
            exit(-1);
        }
        ix_file_seek(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerHeader_00597c84->packetTableOffset);
        ix_file_read(g_pStreamerPackageFile_00597bdc,
                     g_pStreamerPacketOffsets_00597bd0,
                     g_pStreamerHeader_00597c84->packetCount *
                     sizeof(unsigned int));
    }
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_HAS_AUDIO) != 0) {
        g_nStreamerAudioBufferSize_00597c70 =
            g_pStreamerHeader_00597c84->audioBufferSize;
        ix_dsp_configure(0x100,
            (void *)g_pStreamerHeader_00597c84->dspConfigValue);
        ix_dsps_alloc(0, g_nStreamerAudioBufferSize_00597c70,
                      g_pStreamerHeader_00597c84->frequency,
                      g_pStreamerHeader_00597c84->bitsPerSample,
                      g_pStreamerHeader_00597c84->channels);
        ix_dsps_set_volume(0, g_nStreamerVolume_00470e84);
        g_nStreamerBytesPerSecond_00597cdc =
            (g_pStreamerHeader_00597c84->bitsPerSample >> 3) *
            g_pStreamerHeader_00597c84->channels *
            g_pStreamerHeader_00597c84->frequency;
    }
    g_pStreamerCompressedBuffer_00597c68 = (unsigned char *)
        g_pIxMalloc_00471990(g_pStreamerHeader_00597c84->fileBufferSize);
    if (g_pStreamerCompressedBuffer_00597c68 == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 238);
        ix_log_printf("Failed to allocate file_buffer!");
        exit(-1);
    }
    g_pStreamerFileBuffer_00597c74 = (unsigned char *)
        g_pIxMalloc_00471990(g_pStreamerHeader_00597c84->fileBufferSize);
    if (g_pStreamerFileBuffer_00597c74 == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_STREAMER_FILE, 240);
        ix_log_printf("Failed to allocate file_buffer!");
        exit(-1);
    }
    g_dwStreamerState_00597cd0 |= IX_STREAMER_FILE_OPEN;
    SetEvent(g_hStreamerWakeEvent_00597cd4);
    return 0;
}

/* Function start: 0x46C67A */   /* source line(s) 250;251: Streamer not ready to open stream file! | Stream file not open! */
extern "C" void ix_streamer_close_stream_file(void)
{
    IxStreamFile *streamFile;
    IxStreamFile *previous;

    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_INITIALIZED) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 250);
        ix_log_printf("Streamer not ready to open stream file!");
        return;
    }
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_FILE_OPEN) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 251);
        ix_log_printf("Stream file not open!");
        return;
    }
    streamFile = g_pStreamerIdleFiles_00597be0;
    while (streamFile != 0) {
        previous = streamFile->previous;
        ix_streamer_close_file(streamFile);
        streamFile = previous;
    }
    EnterCriticalSection(&g_csStreamer_00597ce0);
    g_dwStreamerState_00597cd0 &= 0xffffff91;
    g_pIxFree_00471994(g_pStreamerHeader_00597c84);
    g_pIxFree_00471994(g_pStreamerAudioChunks_00597c88);
    g_pIxFree_00471994(g_pStreamerBranches_00597c8c);
    g_pIxFree_00471994(g_pStreamerTriggers_00597cf8);
    g_pIxFree_00471994(g_pStreamerFileEntries_00597c80);
    g_pIxFree_00471994(g_pStreamerFileChunks_00597c90);
    g_pIxFree_00471994(g_pStreamerPacketOffsets_00597bd0);
    g_pIxFree_00471994(g_pStreamerCompressedBuffer_00597c68);
    g_pIxFree_00471994(g_pStreamerFileBuffer_00597c74);
    ix_dsps_free(0);
    ix_file_close(g_pStreamerPackageFile_00597bdc);
    LeaveCriticalSection(&g_csStreamer_00597ce0);
    SetEvent(g_hStreamerWakeEvent_00597cd4);
}

/* Function start: 0x46C7F3 */   /* source line(s) 286: Stream has no audio */
extern "C" void ix_streamer_audio_play(void)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 286);
        ix_log_printf("Stream has no audio");
        return;
    }
    g_dwStreamerState_00597cd0 |= IX_STREAMER_AUDIO_PLAYING;
    g_dwStreamerState_00597cd0 &= ~IX_STREAMER_AUDIO_PAUSED;
    SetEvent(g_hStreamerWakeEvent_00597cd4);
}

/* Function start: 0x46C853 */   /* source line(s) 295: Stream has no audio */
extern "C" void ix_streamer_audio_stop(void)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 295);
        ix_log_printf("Stream has no audio");
        return;
    }
    g_dwStreamerState_00597cd0 &= ~IX_STREAMER_AUDIO_PLAYING;
    ix_streamer_seek_chunk(0);
    SetEvent(g_hStreamerWakeEvent_00597cd4);
}

/* Function start: 0x46C8B6 */   /* source line(s) 304: Stream has no audio */
void ix_streamer_audio_pause(void)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 304);
        ix_log_printf("Stream has no audio");
        return;
    }
    g_dwStreamerState_00597cd0 |= IX_STREAMER_AUDIO_PAUSED;
    SetEvent(g_hStreamerWakeEvent_00597cd4);
}

/* Function start: 0x46C90F */   /* source line(s) 312: Stream has no audio */
void ix_streamer_audio_reprepare(void)
{
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 312);
        ix_log_printf("Stream has no audio");
        return;
    }
    g_dwStreamerState_00597cd0 |= IX_STREAMER_REPREPARE_AUDIO;
    SetEvent(g_hStreamerWakeEvent_00597cd4);
}

/* Function start: 0x46C96B */
extern "C" void ix_streamer_set_intensity(unsigned char intensity)
{
    if (intensity >= 0) {
        g_bStreamerIntensity_00597c78 = intensity < 100 ? intensity : 100;
    } else {
        g_bStreamerIntensity_00597c78 = 0;
    }
}

/* Function start: 0x46C9AC */
unsigned char ix_streamer_get_intensity(void)
{
    return g_bStreamerIntensity_00597c78;
}

/* Function start: 0x46C9C1 */
extern "C" void ix_streamer_set_trigger(char trigger)
{
    if (trigger == -1) {
        g_cStreamerBranchTag_00470e88 = -1;
    } else {
        if (trigger >= 0) {
            g_cStreamerBranchTag_00470e88 =
                trigger < 0x40 ? trigger : 0x40;
        } else {
            g_cStreamerBranchTag_00470e88 = 0;
        }
    }
}

/* Function start: 0x46CA19 */
char ix_streamer_get_trigger(void)
{
    return g_cStreamerBranchTag_00470e88;
}

/* Function start: 0x46CA2E */   /* source line(s) 342: Stream has no audio */
extern "C" void ix_streamer_force_trigger(char trigger)
{
    unsigned int triggerCount;
    unsigned int chunk;
    unsigned int chunkCount;
    unsigned int triggerIndex;

    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_HAS_AUDIO) == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 342);
        ix_log_printf("Stream has no audio");
        return;
    }
    chunk = g_nStreamerAudioChunk_00597cc8;
    chunkCount = g_pStreamerHeader_00597c84->audioChunkCount;
    EnterCriticalSection(&g_csStreamer_00597ce0);
    while (-1 < trigger && chunkCount--) {
        if (g_pStreamerAudioChunks_00597c88[chunk].triggerCount > 0) {
            triggerCount =
                g_pStreamerAudioChunks_00597c88[chunk].triggerCount;
            triggerIndex =
                g_pStreamerAudioChunks_00597c88[chunk].firstTrigger;
            while (triggerCount--) {
                if (g_pStreamerTriggers_00597cf8[triggerIndex].tag ==
                    trigger) {
                    g_adwStreamerBranchStack_00597be8[
                        g_nStreamerBranchStackIndex_00470e8c] =
                            g_nStreamerAudioChunk_00597cc8;
                    g_nStreamerBranchStackIndex_00470e8c =
                        (g_nStreamerBranchStackIndex_00470e8c + 1) & 0x1f;
                    g_nStreamerAudioChunk_00597cc8 =
                        g_pStreamerTriggers_00597cf8[
                            triggerIndex].audioChunk;
                    trigger = -1;
                    break;
                }
                triggerIndex++;
            }
        }
        chunk++;
        if (g_pStreamerHeader_00597c84->audioChunkCount - 1 <= chunk)
            chunk = 0;
    }
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_DSP_PLAYING) != 0) {
        ix_dsps_stop(0);
        g_dwStreamerState_00597cd0 &= ~IX_STREAMER_DSP_PLAYING;
    }
    LeaveCriticalSection(&g_csStreamer_00597ce0);
    SetEvent(g_hStreamerWakeEvent_00597cd4);
}

/* Function start: 0x46CBBE */
extern "C" void ix_streamer_set_volume(unsigned short volume)
{
    if (volume >= 0) {
        g_nStreamerVolume_00470e84 =
            volume < 0xffff ? volume : 0xffff;
    } else {
        g_nStreamerVolume_00470e84 = 0;
    }
    if ((g_dwStreamerState_00597cd0 & 2) != 0) {
        EnterCriticalSection(&g_csStreamer_00597ce0);
        ix_dsps_set_volume(0, g_nStreamerVolume_00470e84);
        LeaveCriticalSection(&g_csStreamer_00597ce0);
    }
}

/* Function start: 0x46CC3B */
unsigned short ix_streamer_get_volume(void)
{
    return g_nStreamerVolume_00470e84;
}

/* Function start: 0x428DE0 */
unsigned int ix_streamer_get_audio_chunk(void)
{
    return g_nStreamerAudioChunk_00597cc8;
}

/* Function start: 0x46CC66 */
void ix_streamer_seek_chunk(unsigned int chunk)
{
    EnterCriticalSection(&g_csStreamer_00597ce0);
    if (chunk >= 0) {
        g_nStreamerAudioChunk_00597cc8 =
            g_pStreamerHeader_00597c84->audioChunkCount - 1 < chunk
                ? g_pStreamerHeader_00597c84->audioChunkCount - 1
                : chunk;
    } else {
        g_nStreamerAudioChunk_00597cc8 = 0;
    }
    LeaveCriticalSection(&g_csStreamer_00597ce0);
}

/* Function start: 0x46CCC0 */
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

/* Function start: 0x46CD55 */
IxStreamerFileEntry *ix_streamer_find_entry(unsigned int hash)
{
    unsigned int last;
    IxStreamerFileEntry *entry;
    unsigned int first;
    unsigned int middle;

    first = 0;
    last = g_pStreamerHeader_00597c84->fileEntryCount;
    while (last > first) {
        middle = (first + last) >> 1;
        entry = &g_pStreamerFileEntries_00597c80[middle];
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

/* Function start: 0x46CDE3 */   /* source line(s) 512;526;535;549: open_file failed to allocate memory! | Failed to find_file in stream, %s! | open_file fail */
IxStreamFile *ix_streamer_open_file(unsigned char *name,
                                    unsigned char priority)
{
    IxStreamFile *streamFile;
    IxStreamerFileEntry *entry;
    FILE *file;

    streamFile = 0;
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_DEVELOPER_MODE) == 0) {
        entry = ix_streamer_find_entry(ix_streamer_hash_name(name));
        if (entry != 0) {
            streamFile = (IxStreamFile *)
                g_pIxMalloc_00471990(sizeof(IxStreamFile));
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
            EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
            streamFile->previous = g_pStreamerIdleFiles_00597be0;
            streamFile->next = 0;
            if (g_pStreamerIdleFiles_00597be0 != 0)
                g_pStreamerIdleFiles_00597be0->next = streamFile;
            g_pStreamerIdleFiles_00597be0 = streamFile;
            LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
        } else {
            ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 526);
            ix_log_printf("Failed to find_file in stream, %s!", name);
        }
    } else {
        file = ix_file_open((char *)name, 1);
        if (file != 0) {
            streamFile = (IxStreamFile *)
                g_pIxMalloc_00471990(sizeof(IxStreamFile));
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
            EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
            streamFile->previous = g_pStreamerIdleFiles_00597be0;
            streamFile->next = 0;
            if (g_pStreamerIdleFiles_00597be0 != 0)
                g_pStreamerIdleFiles_00597be0->next = streamFile;
            g_pStreamerIdleFiles_00597be0 = streamFile;
            LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
        } else {
            ix_log_printf("Warning [%s - %d]:\n", IX_STREAMER_FILE, 549);
            ix_log_printf("Failed to find_file in stream, %s!", name);
        }
    }
    return streamFile;
}

/* Function start: 0x46D05D */
void ix_streamer_close_file(IxStreamFile *streamFile)
{
    streamFile->ix_stream_file_wait();
    EnterCriticalSection(&g_csStreamerFileQueue_00597c98);
    if (streamFile->previous != 0)
        streamFile->previous->next = streamFile->next;
    if (streamFile->next == 0)
        g_pStreamerIdleFiles_00597be0 = streamFile->previous;
    else
        streamFile->next->previous = streamFile->previous;
    LeaveCriticalSection(&g_csStreamerFileQueue_00597c98);
    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_DEVELOPER_MODE) != 0)
        ix_file_close(streamFile->file);
    CloseHandle(streamFile->completionEvent);
    g_pIxFree_00471994(streamFile);
}

/* Function start: 0x46D110 */
DWORD WINAPI ix_streamer_thread_proc(void *parameter)
{
    DWORD timeout;

    timeout = 0;
    EnterCriticalSection(&g_csStreamerThread_00597cb0);
    g_dwStreamerState_00597cd0 |= IX_STREAMER_THREAD_RUNNING;
    while ((g_dwStreamerState_00597cd0 & IX_STREAMER_SHUTDOWN) == 0) {
        g_dwStreamerThreadTick_00597bd4++;
        EnterCriticalSection(&g_csStreamer_00597ce0);
        if ((g_dwStreamerState_00597cd0 & IX_STREAMER_FILE_OPEN) != 0) {
            if ((g_dwStreamerState_00597cd0 &
                 IX_STREAMER_REPREPARE_AUDIO) != 0) {
                if ((g_dwStreamerState_00597cd0 &
                     IX_STREAMER_DSP_PLAYING) != 0) {
                    ix_dsps_stop(0);
                    g_dwStreamerState_00597cd0 &=
                        ~IX_STREAMER_DSP_PLAYING;
                }
                ix_dsps_prepare(0);
                g_dwStreamerState_00597cd0 &=
                    ~(IX_STREAMER_REPREPARE_AUDIO |
                      IX_STREAMER_END_TRIGGERED);
            }
            if ((g_dwStreamerState_00597cd0 &
                 (IX_STREAMER_HAS_AUDIO | IX_STREAMER_AUDIO_PLAYING)) ==
                (IX_STREAMER_HAS_AUDIO | IX_STREAMER_AUDIO_PLAYING)) {
                timeout = ix_streamer_service_audio();
                if ((g_dwStreamerState_00597cd0 &
                     IX_STREAMER_AUDIO_PAUSED) != 0) {
                    if ((g_dwStreamerState_00597cd0 &
                         IX_STREAMER_DSP_PAUSED) == 0) {
                        ix_dsps_stop(0);
                        g_dwStreamerState_00597cd0 |=
                            IX_STREAMER_DSP_PAUSED;
                    }
                } else {
                    if ((g_dwStreamerState_00597cd0 &
                         IX_STREAMER_DSP_PAUSED) != 0) {
                        ix_dsps_play(0);
                        g_dwStreamerState_00597cd0 &=
                            ~IX_STREAMER_DSP_PAUSED;
                    }
                }
            } else {
                timeout = ix_thread_service_streams();
                if ((g_dwStreamerState_00597cd0 &
                     IX_STREAMER_DSP_PLAYING) != 0) {
                    ix_dsps_stop(0);
                    g_dwStreamerState_00597cd0 &=
                        ~(IX_STREAMER_DSP_PLAYING |
                          IX_STREAMER_DSP_PAUSED);
                }
            }
            LeaveCriticalSection(&g_csStreamer_00597ce0);
            WaitForSingleObject(g_hStreamerWakeEvent_00597cd4, timeout);
            ResetEvent(g_hStreamerWakeEvent_00597cd4);
        } else {
            if ((g_dwStreamerState_00597cd0 & IX_STREAMER_DSP_PLAYING) != 0) {
                ix_dsps_stop(0);
                g_dwStreamerState_00597cd0 &=
                    ~(IX_STREAMER_DSP_PLAYING | IX_STREAMER_DSP_PAUSED);
            }
            LeaveCriticalSection(&g_csStreamer_00597ce0);
            WaitForSingleObject(g_hStreamerWakeEvent_00597cd4, INFINITE);
            ResetEvent(g_hStreamerWakeEvent_00597cd4);
        }
    }
    g_dwStreamerState_00597cd0 &= ~IX_STREAMER_THREAD_RUNNING;
    LeaveCriticalSection(&g_csStreamerThread_00597cb0);
    (void)parameter;
    return 0;
}

/* Function start: 0x46D2C0 */
unsigned int ix_streamer_service_audio(void)
{
    unsigned int bytesUntilStop;
    register unsigned int chunkSize;

    if ((g_dwStreamerState_00597cd0 & IX_STREAMER_END_TRIGGERED) != 0) {
        bytesUntilStop = g_pStreamerHeader_00597c84->audioBufferSize -
                         ix_dsps_get_buffer_free(0);
        Sleep((bytesUntilStop * 1000U) /
              g_nStreamerBytesPerSecond_00597cdc);
        g_dwStreamerState_00597cd0 &= 0xfffff7db;
        ix_dsps_stop(0);
    } else if ((g_dwStreamerState_00597cd0 &
                IX_STREAMER_DSP_PLAYING) == 0) {
        g_dwStreamerState_00597cd0 |= IX_STREAMER_DSP_PLAYING;
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
