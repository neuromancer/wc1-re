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

/* Function start: 0x00442750 */   /* source line(s) 60;63;75: Streamer already inited! | Failed to init DSP | Failed to start streamer_thread! */
/* TODO: ix_streamer_init */

/* Function start: 0x0044286F */   /* source line(s) 87: Streamer not ready for destroy */
/* TODO: ix_streamer_destroy */

/* Function start: 0x0044291E */
/* TODO: ix_streamer_wake_thread */

/* Function start: 0x0044293E */   /* source line(s) 117: Can't change dev_mode while stream files are open! */
/* TODO: ix_streamer_set_dev_mode */

/* Function start: 0x004429B6 */   /* source line(s) 129;142;146;150;151;157;168;177;186;195;204;238;240: Streamer not ready to open stream file! | Failed to open stream file. | Streamer failed to */
/* TODO: ix_streamer_open_stream_file */

/* Function start: 0x0044307A */   /* source line(s) 250;251: Streamer not ready to open stream file! | Stream file not open! */
/* TODO: ix_streamer_close_stream_file */

/* Function start: 0x004431F3 */   /* source line(s) 286: Stream has no audio */
/* TODO: ix_streamer_audio_play */

/* Function start: 0x00443253 */   /* source line(s) 295: Stream has no audio */
/* TODO: ix_streamer_audio_stop */

/* Function start: 0x004432B6 */   /* source line(s) 304: Stream has no audio */
/* TODO: ix_streamer_audio_pause */

/* Function start: 0x0044330F */   /* source line(s) 312: Stream has no audio */
/* TODO: ix_streamer_audio_set_flag400 */

/* Function start: 0x0044336B */
/* TODO: ix_streamer_set_intensity */

/* Function start: 0x004433AC */
/* TODO: ReadDAT00597c78 */

/* Function start: 0x004433C1 */
/* TODO: ix_streamer_set_branch_tag */

/* Function start: 0x00443419 */
/* TODO: ix_streamer_get_branch_tag */

/* Function start: 0x0044342E */   /* source line(s) 342: Stream has no audio */
/* TODO: ix_streamer_audio_branch_to_tag */

/* Function start: 0x004435BE */
extern "C" void ix_streamer_set_volume(unsigned short volume)
{
    g_nStreamerVolume_00470e84 = volume;
    if (volume > 0xfffe)
        g_nStreamerVolume_00470e84 = 0xffff;
    if ((g_dwStreamerState_00597cd0 & 2) != 0) {
        EnterCriticalSection(&g_csStreamer_00597ce0);
        ix_dsps_set_volume(0, g_nStreamerVolume_00470e84);
        LeaveCriticalSection(&g_csStreamer_00597ce0);
    }
}

/* Function start: 0x0044363B */
/* TODO: ReadDAT00470e84 */

/* Function start: 0x00443651 */
/* TODO: ReadDAT00597cc8 */

/* Function start: 0x00443666 */
/* TODO: ix_streamer_seek_chunk */

/* Function start: 0x004436C0 */
/* TODO: ix_streamer_hash_name */

/* Function start: 0x00443755 */
/* TODO: ix_streamer_find_entry */

/* Function start: 0x004437E3 */   /* source line(s) 512;526;535;549: open_file failed to allocate memory! | Failed to find_file in stream, %s! | open_file fail */
/* TODO: ix_streamer_open_file */

/* Function start: 0x00443A5D */
/* TODO: ix_streamer_release_stream */

/* Function start: 0x00443B10 */
/* TODO: ix_streamer_thread_proc -- the worker thread main loop:
 * waits on the wake event, services the audio stream, and drives
 * ix_dsps_prepare/play/stop from the state word at 0x00597CD0. */

/* Function start: 0x00443CC0 */
/* TODO: ix_streamer_service_audio */
