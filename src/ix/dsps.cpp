/*
 *  ix / D:\rnd\prj\ix\win95\dsp\dsps.cpp
 *
 *  Original address range: 0x004451B5 - 0x00445F5F   (10 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors.
 *  Built /Od /Oi like the rest of ix -- do not optimise this file.
 *
 *  A "stream" is a ring buffer fed by the streamer thread and played through a
 *  voice slot allocated after the regular voices: voice[voice_count + index].
 */
#include "ix.h"
#include <stdlib.h>

IxStream g_streams_00598138[8];
int      g_nStreamCount_00598130;
int      g_nStreamsAllocated_00598134;

/* Function start: 0x4451B5 */   /* source lines 26, 27, 28, 32 */
void ix_dsps_alloc(int stream, unsigned int size, int freq, int bps, int channels)
{
    IxVoice *v;

    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 26);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 27);
        ix_log_printf("stream already allocated!");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_PLAYING) != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 28);
        ix_log_printf("stream still playing!");
        exit(-1);
    }
    g_streams_00598138[stream].buffer = new unsigned char[size];
    if (g_streams_00598138[stream].buffer == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 32);
        ix_log_printf("failed to allocate stream buffer");
        exit(-1);
    }
    g_streams_00598138[stream].size = size;
    g_streams_00598138[stream].writePos = 0;
    g_streams_00598138[stream].playPos = g_streams_00598138[stream].writePos;
    g_streams_00598138[stream].pending = g_streams_00598138[stream].playPos;
    InitializeCriticalSection(&g_streams_00598138[stream].cs);

    v = &g_voices_005981a8[g_nVoiceCount_00598600 + stream];
    v->flags = IX_VOICE_FLAG4 | 1;
    if (bps == 16)
        v->flags |= IX_VOICE_16BIT;
    if (channels == 2)
        v->flags |= IX_VOICE_STEREO;
    v->cursor = g_streams_00598138[stream].buffer;
    v->start = v->cursor;
    v->end = v->cursor + size;
    v->volume = 0x7fff;
    v->leftGain = v->volume;
    v->rightGain = v->volume;
    v->leftGainHi = (unsigned char)((unsigned short)v->volume >> 8);
    v->rightGainHi = (unsigned char)((unsigned short)v->volume >> 8);
    v->field_10 = 0;
    v->rate = (short)((freq << 8) / IX_MIXER_BASE_RATE);
    g_nStreamsAllocated_00598134 = g_nStreamsAllocated_00598134 + 1;
    g_streams_00598138[stream].flags |= IX_STREAM_ALLOCATED;
}

/* Function start: 0x44546B */   /* source lines 62, 63 */
void ix_dsps_free(int stream)
{
    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 62);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 63);
        ix_log_printf("stream is not allocated!");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_PLAYING) != 0)
        ix_dsps_stop(stream);
    DeleteCriticalSection(&g_streams_00598138[stream].cs);
    delete g_streams_00598138[stream].buffer;
    g_nStreamsAllocated_00598134 = g_nStreamsAllocated_00598134 - 1;
    g_streams_00598138[stream].flags &= ~IX_STREAM_ALLOCATED;
}

/* Function start: 0x445582 */   /* source lines 77, 78, 79 */
void ix_dsps_prepare(int stream)
{
    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 77);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 78);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_PLAYING) != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 79);
        ix_log_printf("stream is playing!");
        exit(-1);
    }
    g_streams_00598138[stream].playPos = 0;
    g_streams_00598138[stream].pending = g_streams_00598138[stream].playPos;
    g_streams_00598138[stream].writePos = g_streams_00598138[stream].pending;
    g_voices_005981a8[g_nVoiceCount_00598600 + stream].cursor =
        g_streams_00598138[stream].buffer;
}

/* Function start: 0x4456D8 */   /* source lines 87, 88, 89 */
void ix_dsps_play(int stream)
{
    IxVoice *v;

    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 87);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 88);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_PLAYING) != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 89);
        ix_log_printf("stream is already playing!");
        exit(-1);
    }
    v = &g_voices_005981a8[g_nVoiceCount_00598600 + stream];
    v->cursor = g_streams_00598138[stream].playPos
              + g_streams_00598138[stream].buffer;
    v->flags |= IX_VOICE_ACTIVE;
    g_streams_00598138[stream].flags |= IX_STREAM_PLAYING;
}

/* Function start: 0x445808 */   /* source lines 99, 100, 101 */
void ix_dsps_stop(int stream)
{
    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 99);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 100);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_PLAYING) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 101);
        ix_log_printf("stream isnt playing!");
        exit(-1);
    }
    g_voices_005981a8[g_nVoiceCount_00598600 + stream].flags &= ~IX_VOICE_ACTIVE;
    g_streams_00598138[stream].flags &= ~IX_STREAM_PLAYING;
}

/* Function start: 0x445906 */   /* source lines 109, 110 */
void ix_dsps_set_volume(int stream, unsigned short vol)
{
    IxVoice *v;

    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 109);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 110);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    v = &g_voices_005981a8[g_nVoiceCount_00598600 + stream];
    v->volume =
        (unsigned short)(((int)(vol & 0xffff) * 0x7fff) / 0xffff);
    v->leftGain = v->volume;
    v->rightGain = v->volume;
    v->leftGainHi = (unsigned char)((unsigned short)v->volume >> 8);
    v->rightGainHi = (unsigned char)((unsigned short)v->volume >> 8);
}

/* Function start: 0x445A0B */   /* source line 122 */
unsigned int ix_dsps_get_flags(int stream)
{
    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 122);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    return g_streams_00598138[stream].flags;
}

/* Function start: 0x445A6F */   /* source lines 128, 129 */
int ix_dsps_get_buffer_free(int stream)
{
    int elapsed;
    unsigned int played;

    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 128);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 129);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    elapsed = 0;
    EnterCriticalSection(&g_streams_00598138[stream].cs);
    played = g_voices_005981a8[g_nVoiceCount_00598600 + stream].cursor
           - g_streams_00598138[stream].buffer;
    if ((int)g_streams_00598138[stream].pending < 0) {
        g_streams_00598138[stream].pending = 0;
        g_streams_00598138[stream].playPos = played;
        LeaveCriticalSection(&g_streams_00598138[stream].cs);
        return g_streams_00598138[stream].size;
    }
    if (g_streams_00598138[stream].playPos < played) {
        elapsed = played - g_streams_00598138[stream].playPos;
        g_streams_00598138[stream].pending =
            g_streams_00598138[stream].pending - elapsed;
    }
    else if (g_streams_00598138[stream].playPos != played) {
        elapsed = (g_streams_00598138[stream].size
                   - g_streams_00598138[stream].playPos) + played;
        g_streams_00598138[stream].pending =
            g_streams_00598138[stream].pending - elapsed;
    }
    g_streams_00598138[stream].playPos = played;
    LeaveCriticalSection(&g_streams_00598138[stream].cs);
    return g_streams_00598138[stream].size - g_streams_00598138[stream].pending;
}

/* Function start: 0x00445CDB */   /* source lines 169, 172, 173 */
void ix_dsps_lock(int stream, unsigned int requestedBytes,
                  unsigned char **buffer, unsigned int *lockedBytes)
{
    IxStream *s;

    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 169);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    s = &g_streams_00598138[stream];
    if ((g_streams_00598138[stream].flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 172);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    if ((g_streams_00598138[stream].flags & IX_STREAM_LOCKED) != 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 173);
        ix_log_printf("stream is already locked!");
        exit(-1);
    }
    s->lockPtr = s->buffer + s->writePos;
    if (s->writePos + requestedBytes > s->size)
        requestedBytes = s->size - s->writePos;
    s->lockLen = requestedBytes;
    *buffer = s->lockPtr;
    *lockedBytes = s->lockLen;
    g_streams_00598138[stream].flags |= IX_STREAM_LOCKED;
}

/* Function start: 0x00445E3C */   /* source lines 188, 191, 192 */
void ix_dsps_unlock(int stream)
{
    IxStream *s;

    if (stream < 0 || stream >= g_nStreamCount_00598130) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 188);
        ix_log_printf("invalid stream index");
        exit(-1);
    }
    s = &g_streams_00598138[stream];
    if ((s->flags & IX_STREAM_ALLOCATED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 191);
        ix_log_printf("stream is not ready!");
        exit(-1);
    }
    if ((s->flags & IX_STREAM_LOCKED) == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPS_FILE, 192);
        ix_log_printf("stream isn't locked!");
        exit(-1);
    }
    s->writePos += s->lockLen;
    if (s->writePos >= s->size)
        s->writePos = s->writePos - s->size;
    s->pending += s->lockLen;
    s->flags &= ~IX_STREAM_LOCKED;
}
