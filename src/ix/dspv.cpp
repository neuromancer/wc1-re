/*
 *  ix / D:\rnd\prj\ix\win95\dsp\dspv.cpp
 *
 *  Voice-control functions, followed by the separately assembled sample mixer
 *  in dspmix.asm. Source anchors and object boundaries are in docs/ORDER.md.
 *
 *  Built /Od like the rest of ix: full EBP frame, EBX/ESI/EDI saved whether used
 *  or not, one shared epilogue.  Do not optimise this file.
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#define WC1_IX_DSPV_SOURCE
#include "ix.h"
#include <stdlib.h>
#include <string.h>

IxVoice aVoices[34];             /* 0x005981A8 */
CRITICAL_SECTION csMixer;       /* 0x005985E8 */
int nVoiceCount;                /* 0x00598600 */
int nVoicesAllocated;           /* 0x00598604 */

/* Function start: 0x4467C5 */   /* source line 23 */
void ix_dspv_set_active(int voice)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 23);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    aVoices[voice].flags |= IX_VOICE_ACTIVE;
}

/* Function start: 0x446829 */   /* source line 29 */
void ix_dspv_clear_active(int voice)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 29);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    aVoices[voice].flags &= ~IX_VOICE_ACTIVE;
}

/* Function start: 0x44688E */   /* source line 35 */
void ix_dspv_clear_flag4(int voice)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 35);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    aVoices[voice].flags &= ~IX_VOICE_FLAG4;
}

/* Function start: 0x4468F2 */   /* source line 41 */
unsigned int ix_dspv_get_flags(int voice)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 41);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    return aVoices[voice].flags;
}

/* Function start: 0x446956 */
/* No bounds check in the original -- callers are internal. */
void ix_dspv_set_flag4(int voice, int on)
{
    if (on != 0)
        aVoices[voice].flags |= IX_VOICE_FLAG4;
    else
        aVoices[voice].flags &= ~IX_VOICE_FLAG4;
}

/* Function start: 0x44698F */   /* source line 55 */
void ix_dspv_set_buffer(int voice, unsigned char *p, int len)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 55);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    IxVoice *v = &aVoices[voice];

    EnterCriticalSection(&csMixer);
    v->cursor = p;
    v->start = p;
    v->end = p + len;
    LeaveCriticalSection(&csMixer);
}

/* Function start: 0x446A2B */   /* source line 69 */
int ix_dspv_get_position(int voice)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 69);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    IxVoice *v = &aVoices[voice];

    return (int)((unsigned long)v->cursor - (unsigned long)v->start);
}

/* Function start: 0x446AA0 */   /* source line 77 */
void ix_dspv_set_position(int voice, int off)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 77);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    IxVoice *v = &aVoices[voice];

    v->cursor = v->start + off;
}

/* Function start: 0x446B18 */   /* source line 85 */
void ix_dspv_set_volume(int voice, short vol)
{
    if (voice < 0 || voice >= nVoiceCount) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 85);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    aVoices[voice].volume = vol;
    ix_dspv_recalc_mix(voice);
}

/* Function start: 0x446B8C */   /* source line 93 */
void ix_dspv_set_pan(int voice, unsigned short angle)
{
    short pos;
    {
        IxVoice *state;
        {
            short delta;
            {
                short step;

                if (voice < 0 || nVoiceCount <= voice) {
                    ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 93);
                    ix_log_printf("%d Invalid voice index!", voice);
                    exit(-1);
                }
                state = &aVoices[voice];
                step = (short)((unsigned short)angle >> 9);
                delta = step - state->panAngle;
                if (delta > 0x40)
                    delta = delta - 0x80;
                if (delta < -0x40)
                    delta = delta + 0x80;
                pos = state->panPos + delta;
                if (pos < 0)
                    pos = pos + 0x100;
                if (pos >= 0x100)
                    pos = pos - 0x100;
                state->panAngle = step;
                state->panPos = pos;
                ix_dspv_recalc_mix(voice);
            }
        }
    }
}

/* Function start: 0x446CB1 */   /* source line 131 */
void ix_dspv_set_frequency(int voice, unsigned int hz)
{
    if (voice < 0 || nVoiceCount <= voice) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 131);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    aVoices[voice].rate =
        (short)(((int)(hz & 0xffff) << 8) / IX_MIXER_BASE_RATE);
}

/* Function start: 0x446D2C */   /* source lines 137, 138 */
void ix_dspv_set_bits_per_sample(int voice, int bps)
{
    if (voice < 0 || voice >= nVoiceCount) {
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
        aVoices[voice].flags |= IX_VOICE_16BIT;
    else
        aVoices[voice].flags &= ~IX_VOICE_16BIT;
}

/* Function start: 0x446DF5 */   /* source lines 148, 149 */
void ix_dspv_set_channels(int voice, int channels)
{
    if (voice < 0 || voice >= nVoiceCount) {
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
        aVoices[voice].flags |= IX_VOICE_STEREO;
    else
        aVoices[voice].flags &= ~IX_VOICE_STEREO;
}

/* Function start: 0x446EBF */
void ix_dspv_recalc_mix(int voice)
{
    IxVoice *voiceState;
    short panPosition;
    int scaledVolume;

    voiceState = &aVoices[voice];
    panPosition = voiceState->panPos;
    scaledVolume = ((unsigned int)voiceState->volume
                    * (unsigned int)nMasterVolume) / 0xffff;
    EnterCriticalSection(&csMixer);
    voiceState->leftGain =
        (short)((int)asPanGains[panPosition * 2] * scaledVolume >> 0x10);
    voiceState->rightGain =
        (short)((int)asPanGains[panPosition * 2 + 1] * scaledVolume >> 0x10);
    voiceState->leftGainHi = (unsigned char)(voiceState->leftGain >> 8);
    voiceState->rightGainHi = (unsigned char)(voiceState->rightGain >> 8);
    LeaveCriticalSection(&csMixer);
}

/* Portable counterpart of the assembled sample mixer in dspmix.asm. */
#ifdef SDL_PORT
void ix_dspv_mix(void *outputBuffer, unsigned int outputBytes)
{
    short *output;
    unsigned int frameCount;
    int voice;

    memset(outputBuffer, 0, outputBytes);
    output = (short *)outputBuffer;
    frameCount = outputBytes / 4;
    voice = 0;
    while (voice < nVoiceCount + nStreamCount) {
        IxVoice *state;

        state = &aVoices[voice];
        if ((state->flags & IX_VOICE_ACTIVE) != 0) {
            unsigned char *cursor;
            unsigned int frame;

            cursor = state->cursor;
            frame = 0;
            while (frame < frameCount) {
                int left;
                int right;
                int mixed;
                unsigned int bytesPerFrame;
                unsigned short phase;
                unsigned int advance;

                bytesPerFrame = (state->flags & IX_VOICE_16BIT) != 0 ? 2 : 1;
                if ((state->flags & IX_VOICE_STEREO) != 0)
                    bytesPerFrame *= 2;
                if (cursor >= state->end) {
                    if ((state->flags & IX_VOICE_FLAG4) == 0) {
                        state->flags &= ~IX_VOICE_ACTIVE;
                        break;
                    }
                    cursor = state->start;
                }
                if ((state->flags & IX_VOICE_16BIT) != 0) {
                    short sampleLeft;
                    short sampleRight;

                    memcpy(&sampleLeft, cursor, sizeof(sampleLeft));
                    if ((state->flags & IX_VOICE_STEREO) != 0)
                        memcpy(&sampleRight, cursor + 2, sizeof(sampleRight));
                    else
                        sampleRight = sampleLeft;
                    left = ((int)sampleLeft * state->leftGain) >> 16;
                    right = ((int)sampleRight * state->rightGain) >> 16;
                } else {
                    signed char sampleLeft;
                    signed char sampleRight;

                    sampleLeft = *(signed char *)cursor;
                    if ((state->flags & IX_VOICE_STEREO) != 0)
                        sampleRight = *(signed char *)(cursor + 1);
                    else
                        sampleRight = sampleLeft;
                    left = sampleLeft * (signed char)state->leftGainHi;
                    right = sampleRight * (signed char)state->rightGainHi;
                }
                mixed = output[frame * 2] + left;
                if (mixed > 0x7fff)
                    mixed = 0x7fff;
                else if (mixed < -0x8000)
                    mixed = -0x8000;
                output[frame * 2] = (short)mixed;
                mixed = output[frame * 2 + 1] + right;
                if (mixed > 0x7fff)
                    mixed = 0x7fff;
                else if (mixed < -0x8000)
                    mixed = -0x8000;
                output[frame * 2 + 1] = (short)mixed;
                frame++;
                if (frame == frameCount)
                    break;
                phase = (unsigned short)state->field_10 +
                        (unsigned short)state->rate;
                state->field_10 = (short)(phase & 0xff);
                advance = phase >> 8;
                if (advance != 0) {
                    cursor += advance * bytesPerFrame;
                    if (cursor >= state->end) {
                        if ((state->flags & IX_VOICE_FLAG4) == 0) {
                            state->flags &= ~IX_VOICE_ACTIVE;
                            break;
                        }
                        cursor = state->start;
                    }
                }
            }
            state->cursor = cursor;
        }
        voice++;
    }
}
#endif
