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
    int voice;

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
    voice = g_nVoiceCount_00598600 + stream;
    g_voices_005981a8[voice].cursor = g_streams_00598138[stream].playPos
                                    + g_streams_00598138[stream].buffer;
    g_voices_005981a8[voice].flags |= IX_VOICE_ACTIVE;
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
void ix_dsps_set_volume(int stream, unsigned int vol)
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
    v->volume = (unsigned short)(((vol & 0xffff) * 0x7fff) / 0xffff);
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
