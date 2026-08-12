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
#include <string.h>

IxVoice        g_voices_005981a8[34];
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
    if (on != 0)
        g_voices_005981a8[voice].flags |= IX_VOICE_FLAG4;
    else
        g_voices_005981a8[voice].flags &= ~IX_VOICE_FLAG4;
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
void ix_dspv_set_pan(int voice, unsigned short angle)
{
    IxVoice *state;
    short step;
    short delta;
    short pos;

    if (voice < 0 || g_nVoiceCount_00598600 <= voice) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 93);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    state = &g_voices_005981a8[voice];
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

/* Function start: 0x446CB1 */   /* source line 131 */
void ix_dspv_set_frequency(int voice, unsigned int hz)
{
    if (voice < 0 || g_nVoiceCount_00598600 <= voice) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_DSPV_FILE, 131);
        ix_log_printf("%d Invalid voice index!", voice);
        exit(-1);
    }
    g_voices_005981a8[voice].rate =
        (short)(((int)(hz & 0xffff) << 8) / IX_MIXER_BASE_RATE);
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
    IxVoice *state;
    short pos;
    int gain;

    state = &g_voices_005981a8[voice];
    pos = state->panPos;
    gain = ((unsigned int)state->volume
            * (unsigned int)g_nMasterVolume_0047198c) / 0xffff;
    EnterCriticalSection(&g_csMixer_005985e8);
    state->leftGain =
        (short)((int)g_anPanTable_00597d28[pos * 2] * gain >> 0x10);
    state->rightGain =
        (short)((int)g_anPanTable_00597d28[pos * 2 + 1] * gain >> 0x10);
    state->leftGainHi = (unsigned char)(state->leftGain >> 8);
    state->rightGainHi = (unsigned char)(state->rightGain >> 8);
    LeaveCriticalSection(&g_csMixer_005985e8);
}

/* Function start: 0x00446F74 */
#ifdef WC1_SDL
void ix_dspv_mix(void *outputBuffer, unsigned int outputBytes)
{
    short *output;
    unsigned int frameCount;
    int voice;

    memset(outputBuffer, 0, outputBytes);
    output = (short *)outputBuffer;
    frameCount = outputBytes / 4;
    voice = 0;
    while (voice < g_nVoiceCount_00598600 + g_nStreamCount_00598130) {
        IxVoice *state;

        state = &g_voices_005981a8[voice];
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
#else
__declspec(naked) void ix_dspv_mix(void *outputBuffer,
                                   unsigned int outputBytes)
{
    __asm {
        push ebp
        mov ebp, esp
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        mov eax, 0
        mov edi, dword ptr [ebp + 8]
        mov ecx, dword ptr [ebp + 0xc]
        shr ecx, 2
        rep stosd
        mov ebx, offset g_voices_005981a8
        mov ecx, dword ptr [g_nVoiceCount_00598600]
        add ecx, dword ptr [g_nStreamCount_00598130]

    voice_loop:
        mov eax, dword ptr [ebx]
        test eax, 2
        jz next_voice
        push ecx
        mov edi, dword ptr [ebp + 8]
        mov ecx, dword ptr [ebp + 0xc]
        shr ecx, 2
        mov esi, dword ptr [ebx + 4]
        test eax, 8
        jnz mix_16bit
        test eax, 0x10
        jnz mix_8bit_stereo

    mix_8bit_mono_sample:
        mov al, byte ptr [esi]
        mov dl, al
        imul byte ptr [ebx + 0x17]
        rol eax, 0x10
        mov al, dl
        imul byte ptr [ebx + 0x16]

    mix_8bit_mono_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_8bit_mono_left_done
        jnc mix_8bit_mono_left_positive
        mov ax, 0x8000
        jmp mix_8bit_mono_left_done

    mix_8bit_mono_left_positive:
        mov ax, 0x7fff

    mix_8bit_mono_left_done:
        ror eax, 0x10
        ror edx, 0x10
        add ax, dx
        jno mix_8bit_mono_right_done
        jnc mix_8bit_mono_right_positive
        mov ax, 0x8000
        jmp mix_8bit_mono_right_done

    mix_8bit_mono_right_positive:
        mov ax, 0x7fff

    mix_8bit_mono_right_done:
        rol eax, 0x10
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 0x10]
        add dx, word ptr [ebx + 0x12]
        mov word ptr [ebx + 0x10], dx
        and word ptr [ebx + 0x10], 0xff
        shr dx, 8
        jz mix_8bit_mono_accumulate
        add esi, edx
        cmp esi, dword ptr [ebx + 0xc]
        jl mix_8bit_mono_sample
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_8bit_mono_sample

    mix_8bit_stereo:
        cmp esi, dword ptr [ebx + 0xc]
        jge mix_8bit_stereo_at_end

    mix_8bit_stereo_sample:
        mov al, byte ptr [esi + 1]
        imul byte ptr [ebx + 0x17]
        rol eax, 0x10
        mov al, byte ptr [esi]
        imul byte ptr [ebx + 0x16]

    mix_8bit_stereo_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_8bit_stereo_left_done
        jnc mix_8bit_stereo_left_positive
        mov ax, 0x8000
        jmp mix_8bit_stereo_left_done

    mix_8bit_stereo_left_positive:
        mov ax, 0x7fff

    mix_8bit_stereo_left_done:
        ror eax, 0x10
        ror edx, 0x10
        add ax, dx
        jno mix_8bit_stereo_right_done
        jnc mix_8bit_stereo_right_positive
        mov ax, 0x8000
        jmp mix_8bit_stereo_right_done

    mix_8bit_stereo_right_positive:
        mov ax, 0x7fff

    mix_8bit_stereo_right_done:
        rol eax, 0x10
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 0x10]
        add dx, word ptr [ebx + 0x12]
        mov word ptr [ebx + 0x10], dx
        and word ptr [ebx + 0x10], 0xff
        shr dx, 8
        jz mix_8bit_stereo_accumulate
        shl dx, 1
        add esi, edx
        jmp mix_8bit_stereo

    mix_8bit_stereo_at_end:
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_8bit_stereo_sample

    mix_16bit:
        test eax, 0x10
        jnz mix_16bit_stereo

    mix_16bit_mono:
        cmp esi, dword ptr [ebx + 0xc]
        jge mix_16bit_mono_at_end

    mix_16bit_mono_sample:
        mov ax, word ptr [esi]
        push ax
        imul word ptr [ebx + 0x1a]
        mov ax, dx
        rol eax, 0x10
        pop ax
        imul word ptr [ebx + 0x18]
        mov ax, dx

    mix_16bit_mono_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_16bit_mono_left_done
        jnc mix_16bit_mono_left_positive
        mov ax, 0x8000
        jmp mix_16bit_mono_left_done

    mix_16bit_mono_left_positive:
        mov ax, 0x7fff

    mix_16bit_mono_left_done:
        ror eax, 0x10
        ror edx, 0x10
        add ax, dx
        jno mix_16bit_mono_right_done
        jnc mix_16bit_mono_right_positive
        mov ax, 0x8000
        jmp mix_16bit_mono_right_done

    mix_16bit_mono_right_positive:
        mov ax, 0x7fff

    mix_16bit_mono_right_done:
        rol eax, 0x10
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 0x10]
        add dx, word ptr [ebx + 0x12]
        mov word ptr [ebx + 0x10], dx
        and word ptr [ebx + 0x10], 0xff
        shr dx, 8
        jz mix_16bit_mono_accumulate
        shl dx, 1
        add esi, edx
        jmp mix_16bit_mono

    mix_16bit_mono_at_end:
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_16bit_mono_sample

    mix_16bit_stereo:
        cmp esi, dword ptr [ebx + 0xc]
        jge mix_16bit_stereo_at_end

    mix_16bit_stereo_sample:
        mov ax, word ptr [esi + 2]
        imul word ptr [ebx + 0x1a]
        mov ax, dx
        rol eax, 0x10
        mov ax, word ptr [esi]
        imul word ptr [ebx + 0x18]
        mov ax, dx

    mix_16bit_stereo_accumulate:
        mov edx, dword ptr [edi]
        add ax, dx
        jno mix_16bit_stereo_left_done
        jnc mix_16bit_stereo_left_positive
        mov ax, 0x8000
        jmp mix_16bit_stereo_left_done

    mix_16bit_stereo_left_positive:
        mov ax, 0x7fff

    mix_16bit_stereo_left_done:
        ror eax, 0x10
        ror edx, 0x10
        add ax, dx
        jno mix_16bit_stereo_right_done
        jnc mix_16bit_stereo_right_positive
        mov ax, 0x8000
        jmp mix_16bit_stereo_right_done

    mix_16bit_stereo_right_positive:
        mov ax, 0x7fff

    mix_16bit_stereo_right_done:
        rol eax, 0x10
        mov dword ptr [edi], eax
        add edi, 4
        dec ecx
        jz voice_done
        mov edx, 0
        mov dx, word ptr [ebx + 0x10]
        add dx, word ptr [ebx + 0x12]
        mov word ptr [ebx + 0x10], dx
        and word ptr [ebx + 0x10], 0xff
        shr dx, 8
        jz mix_16bit_stereo_accumulate
        shl dx, 2
        add esi, edx
        jmp mix_16bit_stereo

    mix_16bit_stereo_at_end:
        test dword ptr [ebx], 4
        jz stop_voice
        mov esi, dword ptr [ebx + 8]
        jmp mix_16bit_stereo_sample

    stop_voice:
        and dword ptr [ebx], 0xfffffffd

    voice_done:
        mov dword ptr [ebx + 4], esi
        pop ecx

    next_voice:
        dec ecx
        jz mix_done
        lea ebx, [ebx + 0x20]
        jmp voice_loop

    mix_done:
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        leave
        ret
    }
}
#endif
