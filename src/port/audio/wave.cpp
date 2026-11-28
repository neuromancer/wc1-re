#include "wc1.h"

#define SDL_WAVE_CENTRE_PAN 64
#define SDL_WAVE_MAXIMUM_PAN 127
#define SDL_WAVE_SOUND_EFFECT_GAIN_DIVISOR 2

static unsigned short g_nSdlPendingWavePan;
static int g_nSdlPendingWavePanDepth;

extern "C" IxSound *SdlNewWaveSound(IxSample *sample)
{
    IxSound *sound;

    sound = ix_system_new_sound(sample);
    if (sound != 0 && g_nSdlPendingWavePanDepth != 0)
        sound->ix_system_sound_set_pan(g_nSdlPendingWavePan);
    return sound;
}

extern "C" void SdlPlayWaveWithPan(
    const char *filename, int looping, int volume, int pan)
{
    unsigned short previousPan;
    int previousDepth;

    if (pan < 0)
        pan = 0;
    else if (pan > SDL_WAVE_MAXIMUM_PAN)
        pan = SDL_WAVE_MAXIMUM_PAN;
    previousPan = g_nSdlPendingWavePan;
    previousDepth = g_nSdlPendingWavePanDepth;
    g_nSdlPendingWavePan = (unsigned short)(
        (SDL_WAVE_CENTRE_PAN - pan) * 0x100);
    g_nSdlPendingWavePanDepth++;
    /* The shipped effects peak at full scale while streamed music retains
       roughly 8 dB of headroom.  Reserve 6 dB for the music before ix mixes
       overlapping effects, notably the player's paired weapon sounds. */
    playWAVE(filename, looping,
             volume / SDL_WAVE_SOUND_EFFECT_GAIN_DIVISOR);
    g_nSdlPendingWavePanDepth = previousDepth;
    g_nSdlPendingWavePan = previousPan;
}
