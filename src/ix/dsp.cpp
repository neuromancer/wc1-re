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
#include <dsound.h>
#include <stdlib.h>
#include <string.h>

#define IX_DSP_FILE "D:\\rnd\\prj\\ix\\win95\\dsp\\dsp.cpp"

HANDLE g_hMixerThread_00597d00;
HWND g_hDspWindow_00597d04;
HANDLE g_hMixerWakeEvent_00597d08;
unsigned int g_dwMixerWriteOffset_00597d0c;
LPDIRECTSOUND g_pDirectSound_00597d10;
unsigned int g_dwMixerBufferSize_00597d14;
unsigned int g_dwDspFlags_00597d18;
DWORD g_dwMixerThreadId_00597d1c;
LPDIRECTSOUNDBUFFER g_pMixerBuffer_00597d20;
unsigned int g_dwDspTick_00598128;
LPDIRECTSOUNDBUFFER g_pPrimarySoundBuffer_0059812c;
int g_nVoicesAllocated_00598604;
void *(__cdecl *g_pIxMalloc_00471990)(unsigned int) = malloc;
void (__cdecl *g_pIxFree_00471994)(void *) = free;

/* Function start: 0x00444910 */   /* source line 62 */
int ix_dsp_init(void)
{
    if ((g_dwDspFlags_00597d18 & 1) == 0) {
        g_nVoiceCount_00598600 = 32;
        g_nStreamCount_00598130 = 2;
        g_dwMixerWriteOffset_00597d0c = 0;
        InitializeCriticalSection(&g_csMixer_005985e8);
        g_hMixerWakeEvent_00597d08 = CreateEventA(0, TRUE, FALSE, 0);
        ix_dsp_build_pan_tables();
        g_hMixerThread_00597d00 = CreateThread(
            0, 0x1000, ix_mixer_thread_proc, 0, 0,
            &g_dwMixerThreadId_00597d1c);
        if (g_hMixerThread_00597d00 == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_DSP_FILE, 62);
            ix_log_printf("Failed to start mixer");
            exit(-1);
        }
        g_dwDspFlags_00597d18 |= 1;
    }
    return 0;
}

/* Function start: 0x004449CF */
void ix_dsp_shutdown(void)
{
    if ((g_dwDspFlags_00597d18 & 1) != 0) {
        if ((g_dwDspFlags_00597d18 & 4) != 0) {
            g_dwDspFlags_00597d18 &= ~4U;
            SetEvent(g_hMixerWakeEvent_00597d08);
            WaitForSingleObject(g_hMixerThread_00597d00, INFINITE);
        }
        if (g_pDirectSound_00597d10 != 0) {
            g_pDirectSound_00597d10->Release();
            g_pDirectSound_00597d10 = 0;
        }
        DeleteCriticalSection(&g_csMixer_005985e8);
        CloseHandle(g_hMixerWakeEvent_00597d08);
        g_dwDspFlags_00597d18 &= 0x7ffffffe;
    }
}

/* Function start: 0x00444A62 */
void ix_dsp_configure(int option, void *value)
{
    switch (option) {
    case 0:
        g_hDspWindow_00597d04 = (HWND)value;
        break;
    case 1:
        g_pDirectSound_00597d10 = (LPDIRECTSOUND)value;
        break;
    case 2:
        if ((g_dwDspFlags_00597d18 & 1) != 0)
            SetEvent(g_hMixerWakeEvent_00597d08);
        break;
    case 3:
        if (value != 0)
            g_dwDspFlags_00597d18 |= 0x10;
        else
            g_dwDspFlags_00597d18 &= ~0x10U;
        break;
    case 4:
        g_pIxMalloc_00471990 = (void *(__cdecl *)(unsigned int))value;
        break;
    case 5:
        g_pIxFree_00471994 = (void (__cdecl *)(void *))value;
        break;
    }
}

/* Function start: 0x00444B25 */
BOOL CALLBACK ix_dsp_open_driver(LPGUID guid, LPSTR description,
                                 LPSTR module, LPVOID context)
{
    HRESULT result;

    if (g_pDirectSound_00597d10 != 0) {
        g_pDirectSound_00597d10->Release();
        g_pDirectSound_00597d10 = 0;
    }
    result = DirectSoundCreate(guid, &g_pDirectSound_00597d10, 0);
    if (result == DS_OK) {
        result = g_pDirectSound_00597d10->SetCooperativeLevel(
            g_hDspWindow_00597d04,
            (g_dwDspFlags_00597d18 & 0x10) == 0
                ? DSSCL_WRITEPRIMARY : DSSCL_EXCLUSIVE);
        if (result == DS_OK) {
            ix_log_printf("sound driver: %s [%s]\n", description, module);
            return FALSE;
        }
        if (g_pDirectSound_00597d10 != 0) {
            g_pDirectSound_00597d10->Release();
            g_pDirectSound_00597d10 = 0;
        }
    }
    return TRUE;
}

/* Function start: 0x00444BFD */
unsigned int ix_dsp_get_tick(void)
{
    return g_dwDspTick_00598128;
}

/* Function start: 0x00444C12 */
unsigned int ix_dsp_get_flags(void)
{
    return g_dwDspFlags_00597d18;
}

/* Function start: 0x00444C27 */
void ix_dsp_set_config_bit1(int enabled)
{
    if (enabled != 0)
        g_dwDspFlags_00597d18 |= 2;
    else
        g_dwDspFlags_00597d18 &= ~2U;
}

/* Function start: 0x00444C54 */
void ix_dsp_set_config_bit3(int enabled)
{
    if (enabled != 0)
        g_dwDspFlags_00597d18 |= 8;
    else
        g_dwDspFlags_00597d18 &= ~8U;
}

/* Function start: 0x00444C81 */
int ix_dsp_get_voice_count(void)
{
    return g_nVoiceCount_00598600;
}

/* Function start: 0x00444C96 */   /* source line(s) 176: can't change voice count while voices are in use */
void ix_dsp_set_voice_count(int voiceCount)
{
    if (g_nVoicesAllocated_00598604 != 0 ||
        g_nStreamsAllocated_00598134 != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_DSP_FILE, 176);
        ix_log_printf("can't change voice count while voices are in use");
    } else {
        if (voiceCount >= 0)
            g_nVoiceCount_00598600 = voiceCount < 32 ? voiceCount : 32;
        else
            g_nVoiceCount_00598600 = 0;
    }
}

/* Function start: 0x00444D18 */
int ix_dsp_get_stream_count(void)
{
    return g_nStreamCount_00598130;
}

/* Function start: 0x00444D2D */   /* source line(s) 187: can't change stream count while voices are in use */
void ix_dsp_set_stream_count(int streamCount)
{
    if (g_nVoicesAllocated_00598604 != 0 ||
        g_nStreamsAllocated_00598134 != 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_DSP_FILE, 187);
        ix_log_printf("can't change stream count while voices are in use");
    } else {
        if (streamCount >= 0)
            g_nStreamCount_00598130 = streamCount < 2 ? streamCount : 2;
        else
            g_nStreamCount_00598130 = 0;
    }
}

/* Function start: 0x00444DAF */
unsigned short ix_dsp_get_master_volume(void)
{
    return g_nMasterVolume_0047198c;
}

/* Function start: 0x00444DC5 */
void ix_dsp_set_master_volume(unsigned short volume)
{
    int voice;

    if ((volume & 0xffff) >= 0) {
        g_nMasterVolume_0047198c = (unsigned short)(
            (volume & 0xffff) < 0xffff ? (volume & 0xffff) : 0xffff);
    } else {
        g_nMasterVolume_0047198c = 0;
    }
    for (voice = 0; voice < g_nVoiceCount_00598600; voice++) {
        if ((g_voices_005981a8[voice].flags & IX_VOICE_ACTIVE) != 0)
            ix_dspv_recalc_mix(voice);
    }
}

/* Function start: 0x00444E52 */
void ix_dsp_build_pan_tables(void)
{
    short left;
    short distance;
    short position;
    short scale;
    short right;

    scale = 0x3ff;
    left = 0x7fff;
    for (position = 0; position < 0x40; position++) {
        distance = (short)(0x20 - position);
        right = (short)(distance * scale);
        g_anPanTable_00597d28[position * 2] = left;
        g_anPanTable_00597d28[position * 2 + 1] = right;
        g_anPanTable_00597d28[0x100 + position * 2] =
            (short)(left * -1);
        g_anPanTable_00597d28[0x100 + position * 2 + 1] =
            (short)(right * -1);
    }
    right = (short)0x8001;
    for (position = 0x40; position < 0x80; position++) {
        distance = (short)(0x60 - position);
        left = (short)(distance * scale);
        g_anPanTable_00597d28[position * 2] = left;
        g_anPanTable_00597d28[position * 2 + 1] = right;
        g_anPanTable_00597d28[0x100 + position * 2] =
            (short)(left * -1);
        g_anPanTable_00597d28[0x100 + position * 2 + 1] =
            (short)(right * -1);
    }
}

/* Function start: 0x00444F97 */
const char *ix_dsp_result_to_text(int result)
{
    switch (result) {
    case DS_OK:
        return "The function succeeded.";
    case DSERR_ALLOCATED:
        return "The function failed because resources (such as a priority level) were already in use by another caller.";
    case DSERR_ALREADYINITIALIZED:
        return "This object is already initialized.";
    case DSERR_BADFORMAT:
        return "The specified wave format is not supported.";
    case DSERR_BUFFERLOST:
        return "The buffer memory has been lost and must be restored.";
    case DSERR_CONTROLUNAVAIL:
        return "The control (volume, pan, and so forth) requested by the caller is not available.";
    case DSERR_INVALIDCALL:
        return "This function is not valid for the current state of this object";
    case DSERR_INVALIDPARAM:
        return "An invalid parameter was passed to the returning function.";
    case DSERR_NOAGGREGATION:
        return "This object does not support aggregation.";
    case DSERR_NODRIVER:
        return "No sound driver is available for use.";
    case DSERR_OUTOFMEMORY:
        return "The DirectSound subsystem couldn't allocate sufficient memory to complete the caller's request.";
    case DSERR_PRIOLEVELNEEDED:
        return "The caller does not have the priority level required for the function to succeed.";
    case E_NOINTERFACE:
        return "The requested COM interface is not available.";
    default:
        return "Unknow dsound error!";
    }
}

/* Function start: 0x00445123 */
void *ix_dsp_alloc(unsigned int bytes)
{
    return g_pIxMalloc_00471990(bytes);
}

/* Function start: 0x00445140 */
#pragma function(memcpy)
void *ix_dsp_copy(void *destination, const void *source, unsigned int bytes)
{
    return memcpy(destination, source, bytes);
}

/* Function start: 0x00445164 */
void ix_dsp_free(void *memory)
{
    g_pIxFree_00471994(memory);
}

/* Function start: 0x00445190 */
void ix_dsp_static_initializer(void)
{
    ix_dsp_static_initialization_hook();
}

/* Function start: 0x004451A5 */
void ix_dsp_static_initialization_hook(void)
{
}
