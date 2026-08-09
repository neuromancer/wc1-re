/*
 *  ix / D:\rnd\prj\ix\win95\dsp\dsp.cpp
 *
 *  Original address range: 0x00444910 - 0x004451B4   (21 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"
#include <stdlib.h>

unsigned int g_dwDspTick_00598128;
void (__cdecl *g_pIxFree_00471994)(void *) = free;

/* Original address: 0x00444910 */   /* source line(s) 62: Failed to start mixer */
/* TODO: ix_dsp_init */

/* Original address: 0x004449CF */
/* TODO: ix_thread_shutdown */

/* Original address: 0x00444A62 */
/* TODO: ix_thread_signal_event */

/* Original address: 0x00444B25 */
/* TODO: ix_dsp_open_driver */

/* Function start: 0x00444BFD */
unsigned int ix_dsp_get_tick(void)
{
    return g_dwDspTick_00598128;
}

/* Function start: 0x00444C12 */
/* TODO: ReadDAT00597d18 */

/* Function start: 0x00444C27 */
/* TODO: ix_dsp_set_config_bit1 */

/* Function start: 0x00444C54 */
/* TODO: ix_dsp_set_config_bit3 */

/* Function start: 0x00444C81 */
/* TODO: ix_dsp_get_voice_count */

/* Function start: 0x00444C96 */   /* source line(s) 176: can't change voice count while voices are in use */
/* TODO: ix_dsp_set_voice_count */

/* Function start: 0x00444D18 */
/* TODO: ReadDAT00598130 */

/* Function start: 0x00444D2D */   /* source line(s) 187: can't change stream count while voices are in use */
/* TODO: ix_dsp_set_stream_count */

/* Function start: 0x00444DAF */
/* TODO: ReadDAT0047198c */

/* Function start: 0x00444DC5 */
void ix_dsp_set_master_volume(unsigned short volume)
{
    int voice;

    g_nMasterVolume_0047198c = volume;
    if (volume > 0xfffe)
        g_nMasterVolume_0047198c = 0xffff;
    for (voice = 0; voice < g_nVoiceCount_00598600; voice++) {
        if ((g_voices_005981a8[voice].flags & IX_VOICE_ACTIVE) != 0)
            ix_dspv_recalc_mix(voice);
    }
}

/* Function start: 0x00444E52 */
/* TODO: ix_dsp_build_pan_tables */

/* Function start: 0x00444F97 */
/* TODO: ix_dsp_result_to_text */

/* Function start: 0x00445123 */
/* TODO: ix_dsp_alloc */

/* Function start: 0x00445140 */
/* TODO: ix_dsp_copy */

/* Function start: 0x00445164 */
/* TODO: ix_dsp_free */

/* Function start: 0x00445190 */
/* TODO: $E2 */

/* Function start: 0x004451A5 */
/* TODO: $E1 */
