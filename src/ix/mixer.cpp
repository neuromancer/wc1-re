/*
 *  ix / D:\rnd\prj\ix\win95\dsp\mixer.cpp
 *
 *  Original address range: 0x00445F60 - 0x004467C4   (4 functions)
 *  Range is exact: recovered from this module's own assert __FILE__ anchors in
 *  the shipped debug build (see docs/ORDER.md).
 *
 *  Implement in ADDRESS ORDER -- MSVC emits functions in source order, so the
 *  list below is the original source order of this file.
 */
#include "ix.h"
#ifndef WC1_SDL
#include <dsound.h>
#endif
#include <stdlib.h>
#include <string.h>

#pragma function(memset)

#define IX_MIXER_FILE "D:\\rnd\\prj\\ix\\win95\\dsp\\mixer.cpp"

extern HANDLE hMixerThread;
extern HWND hDspWindow;
extern HANDLE hMixerWakeEvent;
extern unsigned int dwMixerWriteOffset;
extern LPDIRECTSOUND pDirectSound;
extern unsigned int dwMixerBufferSize;
extern unsigned int dwDspFlags;
extern LPDIRECTSOUNDBUFFER pMixerBuffer;
extern unsigned int dwDspTick;
extern LPDIRECTSOUNDBUFFER pPrimarySoundBuffer;

BOOL CALLBACK ix_dsp_open_driver(LPGUID guid, LPSTR description,
                                 LPSTR module, LPVOID context);

/* Function start: 0x00445F60 */   /* source lines 46, 50, 60, 63, 81, 84, 96 */
DWORD WINAPI ix_mixer_thread_proc(void *parameter)
{
#ifdef WC1_SDL
    (void)parameter;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    if (!Wc1SdlStartAudio(ix_dspv_mix, &csMixer,
                          &dwDspTick)) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 50);
        ix_log_printf("Failed to init SDL audio: %s", SDL_GetError());
        exit(-1);
    }
    while ((dwDspFlags & 4) != 0) {
        WaitForSingleObject(hMixerWakeEvent, INFINITE);
        ResetEvent(hMixerWakeEvent);
    }
    Wc1SdlStopAudio();
    return 0;
#else
    DSCAPS driverCaps;
    PCMWAVEFORMAT format;
    DSBUFFERDESC bufferDescription;
    HRESULT result;
    DSBCAPS bufferCaps;

    dwDspFlags |= 4;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    DirectSoundEnumerateA(ix_dsp_open_driver, 0);
    if ((dwDspFlags & 0x10) == 0 &&
        pDirectSound == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 46);
        ix_log_printf("Failed to find write primary driver, using secondary buffer");
        dwDspFlags |= 0x10;
        DirectSoundEnumerateA(ix_dsp_open_driver, 0);
    }
    if (pDirectSound == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 50);
        ix_log_printf("Failed to init directsound");
        exit(-1);
    }

    memset(&driverCaps, 0, 0x60);
    driverCaps.dwSize = 0x60;
    pDirectSound->GetCaps(&driverCaps);
    if ((dwDspFlags & 0x10) == 0 &&
        (driverCaps.dwFlags &
         (DSCAPS_PRIMARY16BIT | DSCAPS_PRIMARYSTEREO)) !=
        (DSCAPS_PRIMARY16BIT | DSCAPS_PRIMARYSTEREO)) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 60);
        ix_log_printf("sound driver doesn't support our primary buffer format, using secondary buffer");
        dwDspFlags |= 0x10;
        DirectSoundEnumerateA(ix_dsp_open_driver, 0);
        if (pDirectSound == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 63);
            ix_log_printf("Failed to init directsound");
            exit(-1);
        }
    }

    memset(&format, 0, 0x10);
    format.wf.wFormatTag = WAVE_FORMAT_PCM;
    format.wf.nChannels = 2;
    format.wf.nSamplesPerSec = 22050;
    format.wf.nBlockAlign = 4;
    format.wf.nAvgBytesPerSec = 88200;
    format.wBitsPerSample = 16;

    memset(&bufferDescription, 0, 0x14);
    bufferDescription.dwSize = 0x14;
    bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    result = pDirectSound->CreateSoundBuffer(
        &bufferDescription, &pPrimarySoundBuffer, 0);
    if (result != DS_OK) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 81);
        ix_log_printf("Failed to create primary buffer");
        exit(-1);
    }

    result = pPrimarySoundBuffer->SetFormat(
        (LPWAVEFORMATEX)&format);
    if (result != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 84);
        ix_log_printf("Failed to SetFormat, %s",
                      ix_dsp_result_to_text(result));
    }
    if ((dwDspFlags & 0x10) != 0) {
        memset(&bufferDescription, 0, 0x14);
        bufferDescription.dwSize = 0x14;
        bufferDescription.dwFlags = DSBCAPS_CTRLVOLUME;
        bufferDescription.dwBufferBytes = 0x8000;
        bufferDescription.lpwfxFormat = (LPWAVEFORMATEX)&format;
        result = pDirectSound->CreateSoundBuffer(
            &bufferDescription, &pMixerBuffer, 0);
        if (result != DS_OK) {
            ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 96);
            ix_log_printf("Failed to CreateSoundBuffer, %s",
                          ix_dsp_result_to_text(result));
            return (DWORD)-1;
        }
        dwMixerBufferSize = 0x8000;
        ix_log_printf("secondary buffer, size = %d\n",
                      dwMixerBufferSize);
    } else {
        memset(&bufferCaps, 0, 0x14);
        bufferCaps.dwSize = 0x14;
        pPrimarySoundBuffer->GetCaps(&bufferCaps);
        pMixerBuffer = pPrimarySoundBuffer;
        dwMixerBufferSize = bufferCaps.dwBufferBytes;
        ix_log_printf("primary buffer, size = %d\n",
                      dwMixerBufferSize);
    }

    pPrimarySoundBuffer->SetVolume(
        10000 - (nMasterVolume * 10000) / 0xffff);
    while ((dwDspFlags & 4) != 0) {
        ix_mixer_service();
        if ((dwDspFlags & 4) != 0) {
            WaitForSingleObject(hMixerWakeEvent, 1000);
            ResetEvent(hMixerWakeEvent);
            pMixerBuffer->Restore();
        }
    }

    if (pMixerBuffer == pPrimarySoundBuffer) {
        pPrimarySoundBuffer = 0;
    } else if (pPrimarySoundBuffer != 0) {
        pPrimarySoundBuffer->Release();
        pPrimarySoundBuffer = 0;
    }
    if (pMixerBuffer != 0) {
        pMixerBuffer->Release();
        pMixerBuffer = 0;
    }
    if (pDirectSound != 0) {
        pDirectSound->Release();
        pDirectSound = 0;
    }
    return 0;
#endif
}

/* Function start: 0x004463FC */   /* source line(s) 150;157;161;176;186;196: Failed to Lock, %s | Failed to Unlock, %s | Failed to Play, %s | Failed to get current pos */
int ix_mixer_service(void)
{
#ifdef WC1_SDL
    return 0;
#else
    DWORD delay;
    HRESULT result;
    void *firstBuffer;
    DWORD firstBytes;
    void *secondBuffer;
    DWORD secondBytes;
    DWORD playCursor;
    DWORD hardwareWriteCursor;
    int bufferedBytes;

    delay = 0x42;
    result = pMixerBuffer->Lock(
        0, 0x2df0, &firstBuffer, &firstBytes, 0, 0, 0);
    if (result != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 150);
        ix_log_printf("Failed to Lock, %s", ix_dsp_result_to_text(result));
        return -1;
    }
    EnterCriticalSection(&csMixer);
    ix_dspv_mix(firstBuffer, firstBytes);
    LeaveCriticalSection(&csMixer);
    result = pMixerBuffer->Unlock(firstBuffer, firstBytes, 0, 0);
    if (result != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 157);
        ix_log_printf("Failed to Unlock, %s", ix_dsp_result_to_text(result));
        return -1;
    }
    result = pMixerBuffer->Play(0, 0, DSBPLAY_LOOPING);
    if (result != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 161);
        ix_log_printf("Failed to Play, %s", ix_dsp_result_to_text(result));
        return -1;
    }

    dwMixerWriteOffset = 0x2df0;
    result = DS_OK;
    while ((dwDspFlags & 4) != 0) {
        Sleep(delay);
        dwDspTick++;
        result = pMixerBuffer->GetCurrentPosition(
            &playCursor, &hardwareWriteCursor);
        if (result != DS_OK) {
            ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 176);
            ix_log_printf("Failed to get current position, %s",
                          ix_dsp_result_to_text(result));
            return -1;
        }
        if (dwMixerWriteOffset < playCursor)
            playCursor -= dwMixerBufferSize;
        bufferedBytes = dwMixerWriteOffset - playCursor;
        delay = (unsigned int)(bufferedBytes * 0x42) / 0x16f8;

        result = pMixerBuffer->Lock(
            dwMixerWriteOffset, 0x16f8,
            &firstBuffer, &firstBytes, &secondBuffer, &secondBytes, 0);
        if (result != DS_OK) {
            ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 186);
            ix_log_printf("Failed to Lock, %s", ix_dsp_result_to_text(result));
            return -1;
        }
        EnterCriticalSection(&csMixer);
        ix_dspv_mix(firstBuffer, firstBytes);
        if (secondBuffer != 0)
            ix_dspv_mix(secondBuffer, secondBytes);
        LeaveCriticalSection(&csMixer);
        result = pMixerBuffer->Unlock(
            firstBuffer, firstBytes, secondBuffer, secondBytes);
        if (result != DS_OK) {
            ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 196);
            ix_log_printf("Failed to Unlock, %s", ix_dsp_result_to_text(result));
            return -1;
        }
        dwMixerWriteOffset += firstBytes;
        if (secondBuffer != 0) {
            dwMixerWriteOffset = secondBytes;
        } else {
            if (dwMixerWriteOffset >= dwMixerBufferSize)
                dwMixerWriteOffset = 0;
        }
        result = DS_OK;
    }
    return 0;
#endif
}

/* Function start: 0x004467A0 */
void ix_mixer_static_initializer(void)
{
    ix_mixer_static_initialization_hook();
}

/* Function start: 0x004467B5 */
void ix_mixer_static_initialization_hook(void)
{
}
