#include "wc1sdl.h"

static SDL_AudioDeviceID g_wAudioDevice;
static Wc1SdlAudioMixer g_pAudioMixer;
static CRITICAL_SECTION *g_pAudioCriticalSection;
static unsigned int *g_pAudioTick;

static void Wc1SdlAudioCallback(void *userData, Uint8 *stream, int bytes)
{
    (void)userData;
    EnterCriticalSection(g_pAudioCriticalSection);
    g_pAudioMixer(stream, (unsigned int)bytes);
    Wc1SdlMixOriginFxMusic(
        (short *)stream,
        (unsigned int)bytes / (sizeof(short) * 2U));
    if (g_pAudioTick != 0)
        *g_pAudioTick = *g_pAudioTick + 1;
    LeaveCriticalSection(g_pAudioCriticalSection);
}

int Wc1SdlStartAudio(Wc1SdlAudioMixer mixer,
                     CRITICAL_SECTION *criticalSection,
                     unsigned int *tick)
{
    SDL_AudioSpec desired;

    if (g_wAudioDevice != 0)
        return TRUE;
    if (mixer == 0 || criticalSection == 0 || criticalSection->mutex == 0)
        return FALSE;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return FALSE;
    SDL_zero(desired);
    desired.freq = 22050;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1470;
    desired.callback = Wc1SdlAudioCallback;
    g_pAudioMixer = mixer;
    g_pAudioCriticalSection = criticalSection;
    g_pAudioTick = tick;
    g_wAudioDevice = SDL_OpenAudioDevice(0, 0, &desired, 0, 0);
    if (g_wAudioDevice == 0) {
        g_pAudioMixer = 0;
        g_pAudioCriticalSection = 0;
        g_pAudioTick = 0;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return FALSE;
    }
    SDL_PauseAudioDevice(g_wAudioDevice, 0);
    return TRUE;
}

void Wc1SdlStopAudio(void)
{
    if (g_wAudioDevice != 0) {
        SDL_CloseAudioDevice(g_wAudioDevice);
        g_wAudioDevice = 0;
    }
    g_pAudioMixer = 0;
    g_pAudioCriticalSection = 0;
    g_pAudioTick = 0;
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
