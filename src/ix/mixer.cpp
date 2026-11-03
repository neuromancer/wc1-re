/**
 * @file
 * @ingroup ixlib
 * @brief DirectSound driver opening and the mixer service thread.
 *
 * Originally `D:\rnd\prj\ix\win95\dsp\mixer.cpp`, address range
 * 445f60-44679f, recovered from the module's own assert `__FILE__` anchors.
 */
#include "ix.h"
#ifndef SDL_PORT
#include <dsound.h>
#endif
#include <stdlib.h>
#include <string.h>

#pragma function(memset)

#define IX_MIXER_FILE "D:\\rnd\\prj\\ix\\win95\\dsp\\mixer.cpp"

extern unsigned int dwSoundBufferSize;
extern unsigned int dwMixerWriteOffset;
extern HANDLE hIxPlaybackThread;
extern HWND hPlaybackWindow;
extern HANDLE hAudioWakeEvent;
extern LPDIRECTSOUND pDspDevice;
extern unsigned int dwDspFlags;
extern LPDIRECTSOUNDBUFFER pMixerBuffer;
extern unsigned int dwDspTick;
extern LPDIRECTSOUNDBUFFER pDspPrimaryBuffer;

BOOL CALLBACK ix_dsp_open_driver(LPGUID guid, LPSTR description,
                                 LPSTR module, LPVOID context);

/* Function start: 0x00445F60 */   /* source lines 46, 50, 60, 63, 81, 84, 96 */
DWORD WINAPI ix_mixer_thread_proc(void *parameter)
{
#ifdef SDL_PORT
    (void)parameter;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    if (!SdlStartAudio(ix_dspv_mix, &csMixer,
                          &dwDspTick)) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 50);
        ix_log_printf("Failed to init SDL audio: %s", SDL_GetError());
        exit(-1);
    }
    while ((dwDspFlags & 4) != 0) {
        WaitForSingleObject(hAudioWakeEvent, INFINITE);
        ResetEvent(hAudioWakeEvent);
    }
    SdlStopAudio();
    return 0;
#else
    DSCAPS soundCaps;
    PCMWAVEFORMAT outputFormat;
    DSBUFFERDESC bufferDescriptor;
    HRESULT resultCode;
    DSBCAPS primaryCaps;

    dwDspFlags |= 4;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    DirectSoundEnumerateA(ix_dsp_open_driver, 0);
    if ((dwDspFlags & 0x10) == 0 &&
        pDspDevice == 0) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 46);
        ix_log_printf("Failed to find write primary driver, using secondary buffer");
        dwDspFlags |= 0x10;
        DirectSoundEnumerateA(ix_dsp_open_driver, 0);
    }
    if (pDspDevice == 0) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 50);
        ix_log_printf("Failed to init directsound");
        exit(-1);
    }

    memset(&soundCaps, 0, 0x60);
    soundCaps.dwSize = 0x60;
    pDspDevice->GetCaps(&soundCaps);
    if ((dwDspFlags & 0x10) == 0 &&
        (soundCaps.dwFlags &
         (DSCAPS_PRIMARY16BIT | DSCAPS_PRIMARYSTEREO)) !=
        (DSCAPS_PRIMARY16BIT | DSCAPS_PRIMARYSTEREO)) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 60);
        ix_log_printf("sound driver doesn't support our primary buffer format, using secondary buffer");
        dwDspFlags |= 0x10;
        DirectSoundEnumerateA(ix_dsp_open_driver, 0);
        if (pDspDevice == 0) {
            ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 63);
            ix_log_printf("Failed to init directsound");
            exit(-1);
        }
    }

    memset(&outputFormat, 0, 0x10);
    outputFormat.wf.wFormatTag = WAVE_FORMAT_PCM;
    outputFormat.wf.nChannels = 2;
    outputFormat.wf.nSamplesPerSec = 22050;
    outputFormat.wf.nBlockAlign = 4;
    outputFormat.wf.nAvgBytesPerSec = 88200;
    outputFormat.wBitsPerSample = 16;

    memset(&bufferDescriptor, 0, 0x14);
    bufferDescriptor.dwSize = 0x14;
    bufferDescriptor.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
    resultCode = pDspDevice->CreateSoundBuffer(
        &bufferDescriptor, &pDspPrimaryBuffer, 0);
    if (resultCode != DS_OK) {
        ix_log_printf("Fatal [%s - %d]:\n", IX_MIXER_FILE, 81);
        ix_log_printf("Failed to create primary buffer");
        exit(-1);
    }

    resultCode = pDspPrimaryBuffer->SetFormat(
        (LPWAVEFORMATEX)&outputFormat);
    if (resultCode != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 84);
        ix_log_printf("Failed to SetFormat, %s",
                      ix_dsp_result_to_text(resultCode));
    }
    if ((dwDspFlags & 0x10) != 0) {
        memset(&bufferDescriptor, 0, 0x14);
        bufferDescriptor.dwSize = 0x14;
        bufferDescriptor.dwFlags = DSBCAPS_CTRLVOLUME;
        bufferDescriptor.dwBufferBytes = 0x8000;
        bufferDescriptor.lpwfxFormat = (LPWAVEFORMATEX)&outputFormat;
        resultCode = pDspDevice->CreateSoundBuffer(
            &bufferDescriptor, &pMixerBuffer, 0);
        if (resultCode != DS_OK) {
            ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 96);
            ix_log_printf("Failed to CreateSoundBuffer, %s",
                          ix_dsp_result_to_text(resultCode));
            return -1;
        }
        dwSoundBufferSize = 0x8000;
        ix_log_printf("secondary buffer, size = %d\n",
                      dwSoundBufferSize);
    } else {
        memset(&primaryCaps, 0, 0x14);
        primaryCaps.dwSize = 0x14;
        pDspPrimaryBuffer->GetCaps(&primaryCaps);
        pMixerBuffer = pDspPrimaryBuffer;
        dwSoundBufferSize = primaryCaps.dwBufferBytes;
        ix_log_printf("primary buffer, size = %d\n",
                      dwSoundBufferSize);
    }

    pDspPrimaryBuffer->SetVolume(
        10000 - (nMasterVolume * 10000) / 0xffff);
    while ((dwDspFlags & 4) != 0) {
        ix_mixer_service();
        if ((dwDspFlags & 4) != 0) {
            WaitForSingleObject(hAudioWakeEvent, 1000);
            ResetEvent(hAudioWakeEvent);
            pMixerBuffer->Restore();
        }
    }

    if (pMixerBuffer == pDspPrimaryBuffer) {
        pDspPrimaryBuffer = 0;
    } else if (pDspPrimaryBuffer != 0) {
        pDspPrimaryBuffer->Release();
        pDspPrimaryBuffer = 0;
    }
    if (pMixerBuffer != 0) {
        pMixerBuffer->Release();
        pMixerBuffer = 0;
    }
    if (pDspDevice != 0) {
        pDspDevice->Release();
        pDspDevice = 0;
    }
    return 0;
#endif
}

/* Function start: 0x004463FC */   /* source line(s) 150;157;161;176;186;196: Failed to Lock, %s | Failed to Unlock, %s | Failed to Play, %s | Failed to get current pos */
int ix_mixer_service(void)
{
#ifdef SDL_PORT
    return 0;
#else
    DWORD delay;
    HRESULT startupResult;
    void *initialBuffer;
    DWORD startupBytes;

    delay = 0x42;
    startupResult = pMixerBuffer->Lock(
        0, 0x2df0, &initialBuffer, &startupBytes, 0, 0, 0);
    if (startupResult != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 150);
        ix_log_printf("Failed to Lock, %s", ix_dsp_result_to_text(startupResult));
        return -1;
    }
    EnterCriticalSection(&csMixer);
    ix_dspv_mix(initialBuffer, startupBytes);
    LeaveCriticalSection(&csMixer);
    startupResult = pMixerBuffer->Unlock(initialBuffer, startupBytes, 0, 0);
    if (startupResult != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 157);
        ix_log_printf("Failed to Unlock, %s", ix_dsp_result_to_text(startupResult));
        return -1;
    }
    startupResult = pMixerBuffer->Play(0, 0, DSBPLAY_LOOPING);
    if (startupResult != DS_OK) {
        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 161);
        ix_log_printf("Failed to Play, %s", ix_dsp_result_to_text(startupResult));
        return -1;
    }

    dwMixerWriteOffset = 0x2df0;
    while ((dwDspFlags & 4) != 0) {
        /* Separate scopes preserve the original refill stack slots. */
        DWORD secondBytes;
        DWORD playCursor;
        {
            HRESULT result;
            DWORD hardwareWriteCursor;
            int bufferedBytes;

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
            if (playCursor > dwMixerWriteOffset)
                playCursor -= dwSoundBufferSize;
            bufferedBytes = dwMixerWriteOffset - playCursor;
            delay = (unsigned int)(bufferedBytes * 0x42) / 0x16f8;

            {
                DWORD firstBytes;
                void *firstBuffer;
                {
                    void *secondBuffer;
                    DWORD requestedBytes;

                    requestedBytes = 0x16f8;
                    result = pMixerBuffer->Lock(
                        dwMixerWriteOffset, requestedBytes,
                        &firstBuffer, &firstBytes,
                        &secondBuffer, &secondBytes, 0);
                    if (result != DS_OK) {
                        ix_log_printf("Warning [%s - %d]:\n", IX_MIXER_FILE, 186);
                        ix_log_printf("Failed to Lock, %s",
                                      ix_dsp_result_to_text(result));
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
                        ix_log_printf("Failed to Unlock, %s",
                                      ix_dsp_result_to_text(result));
                        return -1;
                    }
                    dwMixerWriteOffset += firstBytes;
                    if (secondBuffer != 0) {
                        dwMixerWriteOffset = secondBytes;
                    } else {
                        if (dwMixerWriteOffset >= dwSoundBufferSize)
                            dwMixerWriteOffset = 0;
                    }
                }
            }
        }
    }
    return 0;
#endif
}
