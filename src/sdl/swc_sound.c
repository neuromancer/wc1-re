/* SWC CODE_01 sound player, with SDL replacing the Mac Sound Manager.
 * Four voices share the snd bank for effects and inflight speech. One device
 * also mixes QuickTime music and the current movie/carrier PCM stream. */
#include "wc1.h"
#include "swc.h"

#include <stdio.h>

typedef struct SwcSoundChannel {
    int sound;
    int volume;
    int held;
    uint64_t position; /* 16.16 sample position. */
} SwcSoundChannel;

static SwcSound swcSounds[501];
static SwcSoundChannel swcSoundChannels[4];
SDL_AudioDeviceID swcAudioDevice;
static int swcSoundInitialized;
static SwcBuffer swcPcm;
static uint64_t swcPcmPosition; /* 32.32 mono sample position. */
static uint64_t swcPcmStep;
static int swcPcmSampleBytes;
static int swcPcmHeld;
static float swcSoundGain = 1.0f;

/* Sound Manager completion and sample scheduling are owned by SDL here.
 * No game state, allocation, resource lookup or renderer calls on this thread. */
static void SwcMixSound(void *context, Uint8 *stream, int bytes)
{
    float *output = (float *)stream;
    int frame;
    int channel;
    int frames = bytes / (int)(2 * sizeof(*output));
    float mixed;
    size_t position;

    (void)context;
    SDL_memset(stream, 0, bytes);
    SdlMixSwcMusic(output, frames);
    for (frame = 0; frame < frames; frame++) {
        mixed = 0;
        for (channel = 0; channel < 4; channel++) {
            SwcSoundChannel *voice = &swcSoundChannels[channel];
            SwcSound *sample;
            uint64_t end;

            if (voice->sound == 0 || voice->held)
                continue;
            sample = &swcSounds[voice->sound];
            end = (uint64_t)sample->samples.size << 16;
            if (voice->sound == 12) {
                /* CODE_01 play_looped_sound +0x66be/+0x66c6 overrides
                   the stored loop with [0x23f8,0x32f5) sample offsets. */
                end = UINT64_C(0x32f5) << 16;
                if (voice->position >= end)
                    voice->position = (UINT64_C(0x23f8) << 16) +
                        (voice->position - end) % (UINT64_C(0xefd) << 16);
            } else if (voice->position >= end) {
                voice->sound = 0;
                continue;
            }
            mixed += (sample->samples.data[voice->position >> 16] - 128) *
                voice->volume / 32768.0f;
            voice->position += sample->rate / 48000U;
        }
        position = (size_t)(swcPcmPosition >> 32) * swcPcmSampleBytes;
        if (!swcPcmHeld && position < swcPcm.size) {
            if (swcPcmSampleBytes == 1)
                mixed += (swcPcm.data[position] - 128) / 128.0f;
            else
                mixed += (int16_t)((swcPcm.data[position] << 8) |
                                    swcPcm.data[position + 1]) / 32768.0f;
            swcPcmPosition += swcPcmStep;
        }
        mixed *= swcSoundGain;
        output[frame * 2] = SDL_clamp(output[frame * 2] + mixed, -1.0f, 1.0f);
        output[frame * 2 + 1] = SDL_clamp(output[frame * 2 + 1] + mixed, -1.0f, 1.0f);
    }
}

/* CODE_01 shut_down_sounds +0x655c. Close joins the callback before freeing. */
void SdlFreeSwcSound(void)
{
    size_t sound;

    if (swcAudioDevice != 0)
        SDL_CloseAudioDevice(swcAudioDevice);
    swcAudioDevice = 0;
    SdlFreeSwcMusic();
    SDL_free(swcPcm.data);
    SDL_zero(swcPcm);
    swcPcmPosition = swcPcmStep = 0;
    swcPcmSampleBytes = 0;
    for (sound = 0; sound < SDL_arraysize(swcSounds); sound++) {
        SDL_free(swcSounds[sound].samples.data);
        SDL_zero(swcSounds[sound]);
    }
    SDL_zero(swcSoundChannels);
    if (swcSoundInitialized)
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    swcSoundInitialized = 0;
}

/* CODE_01 init_sound +0x6052 and CMGetChunk("snd ",id).
 * Cache the small demo bank before flight so playback never reads from disk. */
int SdlInitSwcSound(const char *resourceFork)
{
    SwcCmf cmf = {0};
    SwcBuffer resource = {0};
    SDL_AudioSpec desired;
    char path[4096];
    size_t index;
    int result = -1;

    SdlFreeSwcSound();
    if (!SdlResolvePath("CMFs/Sounds.CMF", path, sizeof(path)) ||
        SwcCMOpen(path, &cmf) != 0)
        goto done;
    for (index = 0; index < cmf.chunkCount; index++) {
        const SwcChunkInfo *chunk = &cmf.chunks[index];

        if (memcmp(chunk->type, "snd ", 4) != 0)
            continue;
        if (chunk->id == 0 || chunk->id >= SDL_arraysize(swcSounds) ||
            swcSounds[chunk->id].samples.data != NULL) {
            SDL_SetError("Invalid or duplicate SWC sound ID %u", chunk->id);
            goto done;
        }
        if (SwcCMGetChunk(&cmf, "snd ", chunk->id, &resource) != 0 ||
            SwcReadSound(&resource, &swcSounds[chunk->id]) != 0)
            goto done;
        SDL_free(resource.data);
        SDL_zero(resource);
    }
    /* stop_afterburner_sound +0x67fc uses GetResource, not the CMF bank.
     * The February 1995 build (Mac Magazin 08) has neither that routine
     * nor snd/500; its afterburner loop just stops. */
    if (SwcReadResource(resourceFork, "snd ", 500, &resource) == 0) {
        if (SwcReadSound(&resource, &swcSounds[500]) != 0)
            goto done;
    } else {
        SDL_ClearError();
    }
    if (swcSounds[12].samples.size < 0x32f5) {
        SDL_SetError("SWC afterburner sample is shorter than its loop");
        goto done;
    }
    result = 0;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        goto unavailable;
    swcSoundInitialized = 1;
    SDL_zero(desired);
    desired.freq = 48000;
    desired.format = AUDIO_F32SYS;
    desired.channels = 2;
    desired.samples = 512;
    desired.callback = SwcMixSound;
    swcAudioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
    if (swcAudioDevice == 0)
        goto unavailable;
    SDL_PauseAudioDevice(swcAudioDevice, 0);
    goto done;
unavailable:
    fprintf(stderr, "SWC audio unavailable: %s\n", SDL_GetError());
done:
    SDL_free(resource.data);
    SwcCMClose(&cmf);
    if (result != 0)
        SdlFreeSwcSound();
    return result;
}

void SdlPauseSwcSound(int paused)
{
    if (swcAudioDevice != 0)
        SDL_PauseAudioDevice(swcAudioDevice, paused != 0);
}

/* CODE_01 play_sound +0x62dc, TalkerInit +0x422c, MovieDisplay +0x05da and
 * IntroFilm +0x14a0 give Sound Manager channels DATA/0 +0xa7f2 percent of
 * their level, or none while sound is off; music has its own volume. */
void SdlApplySwcSoundOptions(void)
{
    if (swcAudioDevice != 0)
        SDL_LockAudioDevice(swcAudioDevice);
    swcSoundGain = swcOptions.sound ? swcOptions.soundVolume / 100.0f : 0.0f;
    if (swcAudioDevice != 0)
        SDL_UnlockAudioDevice(swcAudioDevice);
}

/* ModalDialog suspends the game loops; speech, movie audio and playing
 * effects wait for the dialog while music and its own sounds continue. */
void SdlHoldSwcSound(int held)
{
    int channel;

    if (swcAudioDevice == 0)
        return;
    SDL_LockAudioDevice(swcAudioDevice);
    swcPcmHeld = held;
    for (channel = 0; channel < 4; channel++)
        swcSoundChannels[channel].held = held && swcSoundChannels[channel].sound != 0;
    SDL_UnlockAudioDevice(swcAudioDevice);
}

/* CODE_01 stop_all_sounds +0x65e8 only stops the flight sound channels. */
void SdlStopSwcSounds(void)
{
    if (swcAudioDevice != 0)
        SDL_LockAudioDevice(swcAudioDevice);
    SDL_zero(swcSoundChannels);
    if (swcAudioDevice != 0)
        SDL_UnlockAudioDevice(swcAudioDevice);
}

/* Shared replacement for LMovie's Sound Manager buffers and SpoolSoundFile.
 * Copy on the main thread; the callback only reads and advances the cursor. */
int SdlQueueSwcPcm(const SwcBuffer *pcm, int rate, SDL_AudioFormat format)
{
    uint8_t *copy;
    uint8_t *previous;
    int sampleBytes = format == AUDIO_U8 ? 1 : 2;

    if ((format != AUDIO_U8 && format != AUDIO_S16MSB) || rate < 1 || rate > 192000 ||
        pcm->size % sampleBytes != 0 || pcm->size > 64 * 1024 * 1024)
        return SDL_SetError("Unsupported SWC foreground PCM");
    if (pcm->size == 0) {
        SdlClearSwcPcm();
        return 0;
    }
    if (swcAudioDevice == 0)
        return 0;
    copy = SDL_malloc(pcm->size);
    if (copy == NULL)
        return SDL_OutOfMemory();
    SDL_memcpy(copy, pcm->data, pcm->size);
    SDL_LockAudioDevice(swcAudioDevice);
    previous = swcPcm.data;
    swcPcm = (SwcBuffer){copy, pcm->size};
    swcPcmPosition = 0;
    swcPcmStep = ((uint64_t)rate << 32) / 48000;
    swcPcmSampleBytes = sampleBytes;
    SDL_UnlockAudioDevice(swcAudioDevice);
    SDL_free(previous);
    return 0;
}

void SdlClearSwcPcm(void)
{
    uint8_t *previous;

    if (swcAudioDevice != 0)
        SDL_LockAudioDevice(swcAudioDevice);
    previous = swcPcm.data;
    SDL_zero(swcPcm);
    swcPcmPosition = 0;
    if (swcAudioDevice != 0)
        SDL_UnlockAudioDevice(swcAudioDevice);
    SDL_free(previous);
}

int SdlSwcPcmPlaying(void)
{
    int playing;

    if (swcAudioDevice == 0)
        return 0;
    SDL_LockAudioDevice(swcAudioDevice);
    playing = (swcPcmPosition >> 32) * swcPcmSampleBytes < swcPcm.size;
    SDL_UnlockAudioDevice(swcAudioDevice);
    return playing;
}

/* Sound Manager's playback clock, expressed in the 60 Hz units used by
 * CODE_12 GetMouthPos. Reading the mixer position keeps lips in step with
 * paused audio and frames delayed by loading or rendering. */
Uint32 SdlSwcPcmTicks(void)
{
    uint64_t frames = 0;

    if (swcAudioDevice == 0)
        return 0;
    SDL_LockAudioDevice(swcAudioDevice);
    if (swcPcmStep != 0 && swcPcmSampleBytes != 0) {
        frames = SDL_min(swcPcmPosition,
            ((uint64_t)swcPcm.size / swcPcmSampleBytes) << 32) / swcPcmStep;
        /* The callback fills 512 frames ahead of the audible output. */
        frames = frames > 512 ? frames - 512 : 0;
    }
    SDL_UnlockAudioDevice(swcAudioDevice);
    return (Uint32)(frames * 60 / 48000);
}

/* CODE_01 play_sound +0x626e: already playing IDs are not restarted;
 * idle channels are reused, and a full four-channel mixer rejects the play. */
int SdlPlaySwcSound(int sound, int volume)
{
    int channel;
    int idle = -1;

    if (swcAudioDevice == 0 || sound <= 0 ||
        sound >= (int)SDL_arraysize(swcSounds) || swcSounds[sound].samples.data == NULL)
        return -1;
    SDL_LockAudioDevice(swcAudioDevice);
    for (channel = 0; channel < 4; channel++) {
        if (swcSoundChannels[channel].sound == sound) {
            SDL_UnlockAudioDevice(swcAudioDevice);
            return 1;
        }
        if (idle == -1 && swcSoundChannels[channel].sound == 0)
            idle = channel;
    }
    if (idle != -1) {
        swcSoundChannels[idle].sound = sound;
        swcSoundChannels[idle].volume = SDL_clamp(volume, 0, 255);
        swcSoundChannels[idle].held = 0;
        swcSoundChannels[idle].position = 0;
    }
    SDL_UnlockAudioDevice(swcAudioDevice);
    return idle != -1;
}

/* CODE_01 stop_sound +0x61c8 / is_sound_playing +0x5f8e. */
void SdlStopSwcSound(int sound)
{
    int channel;

    if (swcAudioDevice == 0)
        return;
    SDL_LockAudioDevice(swcAudioDevice);
    for (channel = 0; channel < 4; channel++) {
        if (swcSoundChannels[channel].sound == sound)
            swcSoundChannels[channel].sound = 0;
    }
    SDL_UnlockAudioDevice(swcAudioDevice);
}

int SdlSwcSoundPlaying(int sound)
{
    int channel;
    int playing = 0;

    if (swcAudioDevice == 0)
        return 0;
    SDL_LockAudioDevice(swcAudioDevice);
    for (channel = 0; channel < 4; channel++) {
        if (swcSoundChannels[channel].sound == sound)
            playing = 1;
    }
    SDL_UnlockAudioDevice(swcAudioDevice);
    return playing;
}

/* CODE_01 stop_afterburner_sound +0x678a replaces the loop in its own
 * channel with snd/500, preserving the current volume. */
void SdlStopSwcAfterburnerSound(void)
{
    int channel;

    if (swcAudioDevice == 0)
        return;
    SDL_LockAudioDevice(swcAudioDevice);
    for (channel = 0; channel < 4; channel++) {
        if (swcSoundChannels[channel].sound == 12) {
            swcSoundChannels[channel].sound = swcSounds[500].samples.data != NULL ? 500 : 0;
            swcSoundChannels[channel].position = 0;
        }
    }
    SDL_UnlockAudioDevice(swcAudioDevice);
}

/* CODE_09 sound_effect +0x1508: mono volume falls by one per 100 units.
 * CODE_05 fire_weapon +0x379a distinguishes the Kilrathi laser (snd/26). */
int SdlSwcSoundEffect(int sound, int object)
{
    FixedVector delta;
    int volume = 127;

    if (object != -1) {
        if (object < 0 || object >= SPACE_OBJECT_COUNT)
            return 0;
        ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT], &aShipPositionsArray[object], &delta);
        volume -= (Vector_magnitude(&delta) / 100) >> 8;
        if (volume < 10)
            return 1;
        if (sound == 8) {
            int owner = acObjOwnersTable[object];

            /* fire_weapon passes the projectile for attenuation, but the
               original selects the laser sample from the firing ship's side. */
            if (owner >= 0 && owner < (int)SDL_arraysize(aeShipSide) &&
                aeShipSide[owner] != SIDE_IMPERIAL)
                sound = 26;
        }
    }
    return SdlPlaySwcSound(sound, volume);
}
