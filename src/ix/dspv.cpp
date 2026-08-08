/*
 *  ix / D:\rnd\prj\ix\win95\dsp\dspv.cpp
 *
 *  Original address range: 0x004467C5 - 0x004471FF   (15 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Built /Od like the rest of ix: full EBP frame, EBX/ESI/EDI saved whether used
 *  or not, one shared epilogue.  Do not optimise this file.
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"
#include <stdlib.h>

IxVoice        g_voices_005981a8[32];
int            g_nVoiceCount_00598600;
unsigned short g_nMasterVolume_0047198c;
short          g_anPanTable_00597d28[512];
CRITICAL_SECTION g_csMixer_005985e8;

/* Function start: 0x4467C5 */   /* source line 23 */
void ix_dspv_set_active(int voice)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 23);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    g_voices_005981a8[voice].flags |= IX_VOICE_ACTIVE;
}

/* Function start: 0x446829 */   /* source line 29 */
void ix_dspv_clear_active(int voice)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 29);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    g_voices_005981a8[voice].flags &= ~IX_VOICE_ACTIVE;
}

/* Function start: 0x44688E */   /* source line 35 */
void ix_dspv_clear_flag4(int voice)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 35);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    g_voices_005981a8[voice].flags &= ~IX_VOICE_FLAG4;
}

/* Function start: 0x4468F2 */   /* source line 41 */
unsigned int ix_dspv_get_flags(int voice)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 41);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    return g_voices_005981a8[voice].flags;
}

/* Function start: 0x446956 */
/* No bounds check in the original -- callers are internal. */
void ix_dspv_set_flag4(int voice, int on)
{
    if (on == 0)
        g_voices_005981a8[voice].flags &= ~IX_VOICE_FLAG4;
    else
        g_voices_005981a8[voice].flags |= IX_VOICE_FLAG4;
}

/* Function start: 0x44698F */   /* source line 55 */
void ix_dspv_set_buffer(int voice, unsigned char *p, int len)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 55);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    IxVoice *v = &g_voices_005981a8[voice];

    EnterCriticalSection(&g_csMixer_005985e8);
    v->cursor = p;
    v->start = p;
    v->end = p + len;
    LeaveCriticalSection(&g_csMixer_005985e8);
}

/* Function start: 0x446A2B */   /* source line 69 */
int ix_dspv_get_position(int voice)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 69);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    IxVoice *v = &g_voices_005981a8[voice];

    return v->cursor - v->start;
}

/* Function start: 0x446AA0 */   /* source line 77 */
void ix_dspv_set_position(int voice, int off)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 77);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    IxVoice *v = &g_voices_005981a8[voice];

    v->cursor = v->start + off;
}

/* Function start: 0x446B18 */   /* source line 85 */
void ix_dspv_set_volume(int voice, short vol)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 85);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    g_voices_005981a8[voice].volume = vol;
    ix_dspv_recalc_mix(voice);
}

/* Function start: 0x446B8C */   /* source line 93 */
void ix_dspv_set_pan(int voice, unsigned int angle)
{
    short step;
    short pos;

    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 93);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    step = (short)((angle & 0xffff) >> 9);
    pos = step - g_voices_005981a8[voice].panAngle;
    if (pos > 0x40)
        pos = pos - 0x80;
    if (pos < -0x40)
        pos = pos + 0x80;
    pos = g_voices_005981a8[voice].panPos + pos;
    if (pos < 0)
        pos = pos + 0x100;
    if (pos > 0xff)
        pos = pos - 0x100;
    g_voices_005981a8[voice].panAngle = step;
    g_voices_005981a8[voice].panPos = pos;
    ix_dspv_recalc_mix(voice);
}

/* Function start: 0x446CB1 */   /* source line 131 */
void ix_dspv_set_frequency(int voice, unsigned int hz)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 131);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    g_voices_005981a8[voice].rate = (short)(((hz & 0xffff) << 8) / IX_MIXER_BASE_RATE);
}

/* Function start: 0x446D2C */   /* source lines 137, 138 */
void ix_dspv_set_bits_per_sample(int voice, int bps)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 137);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    if (bps != 8 && bps != 16) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 138);
        ix_log_printf("%d is an unsupported bps!", bps);
        exit(-1);
    }
    if (bps == 16)
        g_voices_005981a8[voice].flags |= IX_VOICE_16BIT;
    else
        g_voices_005981a8[voice].flags &= ~IX_VOICE_16BIT;
}

/* Function start: 0x446DF5 */   /* source lines 148, 149 */
void ix_dspv_set_channels(int voice, int channels)
{
    if (voice < 0 || voice >= g_nVoiceCount_00598600) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 148);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    if (channels != 1 && channels != 2) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 149);
        ix_log_printf("%d is an unsupported number of channels!", channels);
        exit(-1);
    }
    if (channels == 2)
        g_voices_005981a8[voice].flags |= IX_VOICE_STEREO;
    else
        g_voices_005981a8[voice].flags &= ~IX_VOICE_STEREO;
}

/* Function start: 0x446EBF */
void ix_dspv_recalc_mix(int voice)
{
    short pos;
    unsigned int gain;

    pos = g_voices_005981a8[voice].panPos;
    gain = ((unsigned int)g_voices_005981a8[voice].volume
            * (unsigned int)g_nMasterVolume_0047198c) / 0xffff;
    EnterCriticalSection(&g_csMixer_005985e8);
    g_voices_005981a8[voice].leftGain =
        (short)((int)g_anPanTable_00597d28[pos * 2] * gain >> 0x10);
    g_voices_005981a8[voice].rightGain =
        (short)((int)g_anPanTable_00597d28[pos * 2 + 1] * gain >> 0x10);
    g_voices_005981a8[voice].leftGainHi =
        (unsigned char)((unsigned short)g_voices_005981a8[voice].leftGain >> 8);
    g_voices_005981a8[voice].rightGainHi =
        (unsigned char)((unsigned short)g_voices_005981a8[voice].rightGain >> 8);
    LeaveCriticalSection(&g_csMixer_005985e8);
}
