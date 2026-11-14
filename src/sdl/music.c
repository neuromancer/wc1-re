/* SDL2-only OriginFX/AdLib playback for WC1 DOS and Kilrathi Saga data. */
#include "wc1.h"

#include <stdio.h>
#include <stdlib.h>

#define SDL_PORT_MUSIC_PATH_SIZE 4096
/* WINGLDR.TIM, like STRAX.DRV, has a section per device: 0 MT-32, 1 AdLib,
 * 2 PC speaker. */
#define SDL_PORT_MT32_TIMBRE_SECTION 0
#define SDL_PORT_ADLIB_TIMBRE_SECTION 1
/* SdlStartAudio's rate, which the OriginFX players render at. */
#define SDL_PORT_ORIGINFX_SAMPLE_RATE 22050
#define SDL_SOUND_METRES_PER_VOLUME_STEP 500L
#define SDL_SOUND_FULL_VOLUME 127
#define SDL_SOUND_AUDIBLE_VOLUME 10
#define SDL_SOUND_CENTRE_PAN 64

static CRITICAL_SECTION g_stSdlDosMusicAudioCriticalSection;
static SDL_mutex *g_pSdlDosMusicMutex;
static unsigned char *g_pSdlDosMusicArchive;
static unsigned char *g_pSdlDosAdlibTimbres;
static SdlOriginFxPlayer *g_pSdlOriginFxPlayer;
static SdlOriginFxPlayer *g_pSdlOriginFxSoundPlayer;
/* With MT-32 ROMs, one emulated MT-32 plays the music across tracks and
 * another the sound effects, as the port's AdLib players have their own
 * chips. */
static SdlMt32Synth *g_pSdlMt32MusicSynth;
static SdlMt32Synth *g_pSdlMt32SoundSynth;
static size_t g_nSdlDosMusicArchiveSize;
static size_t g_nSdlDosAdlibTimbreSize;
static unsigned int g_nSdlDosMusicGain;
static unsigned int g_nSdlDosSoundGain;
static unsigned int g_nSdlDosRapidFireTag;
static int g_nSdlActiveMusicTrack = -1;
static int g_nSdlMusicVolumeSetting = -1;
static int g_nSdlSoundVolumeSetting = -1;
static int g_bSdlDosMusicAudioCriticalSectionInitialized;
static int g_bSdlDosMusicInitialized;
static int g_bSdlOriginFxOwnsAudioDevice;
static int g_bSdlOriginFxServicesAllTracks;

static unsigned char *SdlLoadDosMusicFile(
    const char *const *candidates, unsigned int candidateCount,
    size_t *fileSize)
{
    unsigned char *fileData;
    char resolved[SDL_PORT_MUSIC_PATH_SIZE];
    unsigned int candidateIndex;

    candidateIndex = 0;
    while (candidateIndex < candidateCount) {
        if (SdlResolvePath(
                candidates[candidateIndex], resolved, sizeof(resolved))) {
            fileData = SDL_LoadFile(resolved, fileSize);
            if (fileData != 0)
                return fileData;
        }
        candidateIndex++;
    }
    return 0;
}

static void SdlMixDosAdlibMusic(void *stream, unsigned int byteCount)
{
    unsigned int frameCount;

    SDL_memset(stream, 0, byteCount);
    if (g_pSdlDosMusicMutex == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    frameCount = byteCount / (sizeof(short) * 2U);
    if (g_pSdlOriginFxPlayer != 0) {
        SdlRenderOriginFxPlayer(
            g_pSdlOriginFxPlayer, stream,
            frameCount, g_nSdlDosMusicGain);
    } else if (g_pSdlMt32MusicSynth != 0) {
        /* Between tracks the MT-32's released notes and reverb fade out. */
        short *samples = (short *)stream;
        unsigned int index;

        SdlMt32Render(g_pSdlMt32MusicSynth, samples, frameCount);
        for (index = 0; index < frameCount * 2U; index++)
            samples[index] = (short)((long)samples[index] *
                                     (long)g_nSdlDosMusicGain / 0x7fffL);
    }
    if (g_pSdlOriginFxSoundPlayer != 0) {
        SdlMixOriginFxSoundEffects(
            g_pSdlOriginFxSoundPlayer, stream,
            frameCount, g_nSdlDosSoundGain);
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

static void SdlDeleteDosAdlibTrack(void)
{
    if (g_pSdlOriginFxPlayer != 0)
        SdlMt32AllNotesOff(g_pSdlMt32MusicSynth);
    SdlDestroyOriginFxPlayer(g_pSdlOriginFxPlayer);
    g_pSdlOriginFxPlayer = 0;
    g_nSdlActiveMusicTrack = -1;
}

/* With MT-32 ROMs, both synths load the MT-32 instruments; without them or
 * when a synth fails, the game keeps its AdLib music. */
static void SdlStartDosMt32(const unsigned char *timbreArchive,
                            size_t timbreArchiveSize, int soundEffects)
{
    unsigned char *timbres;
    size_t timbreSize;
    char description[PATH_MAX + 256];

    if (!SdlUsingMt32())
        return;
    if (!SdlDescribeMt32Roms(SdlGetMt32RomDirectory(), SdlGetMt32Machine(),
                             description, sizeof(description))) {
        fprintf(stderr, "MT-32 unavailable: %s\n", description);
        return;
    }
    if (!SdlExtractOriginPacketSection(timbreArchive, timbreArchiveSize,
                                       SDL_PORT_MT32_TIMBRE_SECTION,
                                       &timbres, &timbreSize)) {
        fprintf(stderr, "Unable to decode the MT-32 instruments in WINGLDR.TIM.\n");
        return;
    }
    g_pSdlMt32MusicSynth = SdlCreateMt32Synth(
        SDL_PORT_ORIGINFX_SAMPLE_RATE, timbres, timbreSize);
    if (g_pSdlMt32MusicSynth != 0 && soundEffects) {
        g_pSdlMt32SoundSynth = SdlCreateMt32Synth(
            SDL_PORT_ORIGINFX_SAMPLE_RATE, timbres, timbreSize);
    }
    free(timbres);
    if (g_pSdlMt32MusicSynth == 0 || (soundEffects && g_pSdlMt32SoundSynth == 0)) {
        fprintf(stderr, "MT-32 unavailable: %s\n", SDL_GetError());
        SdlDestroyMt32Synth(g_pSdlMt32MusicSynth);
        SdlDestroyMt32Synth(g_pSdlMt32SoundSynth);
        g_pSdlMt32MusicSynth = 0;
        g_pSdlMt32SoundSynth = 0;
        return;
    }
    fprintf(stderr, "MT-32 music%s: %s\n", soundEffects ? " and sound effects" : "",
            description);
}

static unsigned int SdlCalculateDosAudioGain(int volumeSetting)
{
    int level;
    int tableIndex;

    tableIndex = volumeSetting / 2;
    if (tableIndex < 0)
        tableIndex = 0;
    else if (tableIndex > 10)
        tableIndex = 10;
    level = anVolumeLevels[tableIndex];
    if (level < 0)
        level = 0;
    else if (level > 64000)
        level = 64000;
    return ((long)level * 0x7fffL / 64000L);
}

static void SdlUpdateDosAdlibMusicVolume(void)
{
    if (g_nSdlMusicVolumeSetting == nMusicVolumeSetting &&
        g_nSdlSoundVolumeSetting == nSfxVolumeSetting)
        return;
    if (g_nSdlMusicVolumeSetting !=
        nMusicVolumeSetting) {
        g_nSdlMusicVolumeSetting = nMusicVolumeSetting;
        g_nSdlDosMusicGain =
            SdlCalculateDosAudioGain(g_nSdlMusicVolumeSetting);
    }
    if (g_nSdlSoundVolumeSetting !=
        nSfxVolumeSetting) {
        g_nSdlSoundVolumeSetting = nSfxVolumeSetting;
        g_nSdlDosSoundGain =
            SdlCalculateDosAudioGain(g_nSdlSoundVolumeSetting);
    }
}

int SdlInitializeOriginFxAudio(int useStandaloneAudio)
{
    const char *musicCandidates[2] = {
        "GAMEDAT/MUSIC.MID",
        "MUSIC.MID"
    };
    const char *timbreCandidates[2] = {
        "GAMEDAT/WINGLDR.TIM",
        "WINGLDR.TIM"
    };
    unsigned char *timbreArchive;
    size_t timbreArchiveSize;

    if (g_bSdlDosMusicInitialized != 0)
        return 1;
    g_pSdlDosMusicArchive = SdlLoadDosMusicFile(
        musicCandidates, 2, &g_nSdlDosMusicArchiveSize);
    if (g_pSdlDosMusicArchive == 0) {
        fprintf(stderr, "Unable to load GAMEDAT/MUSIC.MID.\n");
        return 0;
    }
    timbreArchive = SdlLoadDosMusicFile(
        timbreCandidates, 2, &timbreArchiveSize);
    if (timbreArchive == 0) {
        fprintf(stderr, "Unable to load GAMEDAT/WINGLDR.TIM.\n");
        goto failed;
    }
    if (!SdlExtractOriginPacketSection(
            timbreArchive, timbreArchiveSize,
            SDL_PORT_ADLIB_TIMBRE_SECTION,
            &g_pSdlDosAdlibTimbres,
            &g_nSdlDosAdlibTimbreSize)) {
        SDL_free(timbreArchive);
        fprintf(stderr, "Unable to decode OriginFX AdLib timbres.\n");
        goto failed;
    }
    SdlStartDosMt32(timbreArchive, timbreArchiveSize, useStandaloneAudio != 0);
    SDL_free(timbreArchive);

    if (useStandaloneAudio != 0) {
        g_pSdlOriginFxSoundPlayer = g_pSdlMt32SoundSynth != 0
            ? SdlCreateOriginFxMt32SoundPlayer(g_pSdlMt32SoundSynth)
            : SdlCreateOriginFxSoundPlayer(
                g_pSdlDosAdlibTimbres,
                g_nSdlDosAdlibTimbreSize);
        if (g_pSdlOriginFxSoundPlayer == 0) {
            fprintf(stderr,
                    "Unable to initialize DOS AdLib sound effects.\n");
            goto failed;
        }
    }

    g_pSdlDosMusicMutex = SDL_CreateMutex();
    if (g_pSdlDosMusicMutex == 0)
        goto failed;
    if (useStandaloneAudio != 0) {
        InitializeCriticalSection(
            &g_stSdlDosMusicAudioCriticalSection);
        g_bSdlDosMusicAudioCriticalSectionInitialized = 1;
    }
    g_bSdlDosMusicInitialized = 1;
    g_bSdlOriginFxServicesAllTracks = useStandaloneAudio != 0;
    SdlUpdateDosAdlibMusicVolume();
    if (useStandaloneAudio != 0) {
        if (!SdlStartAudio(
                SdlMixDosAdlibMusic,
                &g_stSdlDosMusicAudioCriticalSection, 0))
            goto failed;
        g_bSdlOriginFxOwnsAudioDevice = 1;
        fprintf(stderr, "DOS OriginFX/AdLib audio enabled.\n");
    } else {
        fprintf(stderr, "OriginFX intro music enabled.\n");
    }
    return 1;

failed:
    SdlShutdownOriginFxAudio();
    return 0;
}

int SdlPlayDosSoundEffect(int soundNumber, int volume, int pan,
                             int tag, int priority)
{
    int result;

    if (g_bSdlDosMusicInitialized == 0 ||
        g_pSdlDosMusicMutex == 0 ||
        g_pSdlOriginFxSoundPlayer == 0)
        return 0;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    if (soundNumber == 8) {
        tag = 64 + (int)(g_nSdlDosRapidFireTag & 1U);
        g_nSdlDosRapidFireTag++;
    }
    result = SdlPlayOriginFxSoundEffect(
        g_pSdlOriginFxSoundPlayer,
        soundNumber, volume, pan, tag, priority);
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
    return result;
}

int SdlHandlesGameSoundEffects(void)
{
    return 1;
}

int SdlPlayGameSoundEffect(int soundNumber, int sourceObject, int looping)
{
    FixedVector delta;
    long magnitude;
    int scaledPan;
    int stereoOffset;
    int distance;
    int volume;
    int pan;

    if (SdlSwcFlightActive())
        return SdlSwcSoundEffect(soundNumber, sourceObject);

    magnitude = 0;
    pan = SDL_SOUND_CENTRE_PAN;
    if (sourceObject != -1) {
        if (sourceObject < 0 || sourceObject >= SPACE_OBJECT_COUNT)
            return 0;
        ComputeVectorDelta(
            &aShipPositionsArray[EYE_OBJECT],
            &aShipPositionsArray[sourceObject], &delta);
        magnitude = Vector_magnitude(&delta);
        NormalizeFixedVector(&delta);
        stereoOffset = dot_product(
            &delta, &aShipRightVector[EYE_OBJECT]);
        scaledPan = stereoOffset * SDL_SOUND_CENTRE_PAN;
        if (scaledPan < 0)
            scaledPan = -((-scaledPan + 0xff) / 0x100);
        else
            scaledPan /= 0x100;
        pan -= scaledPan;
        if (pan < 0)
            pan = 0;
        else if (pan > 127)
            pan = 127;
    }

    if (SdlUsingDosData() || SdlUsingTownsData()) {
        volume = SDL_SOUND_FULL_VOLUME;
        if (sourceObject != -1) {
            volume -= (int)((magnitude /
                             SDL_SOUND_METRES_PER_VOLUME_STEP) >> 8);
        }
        if (volume < 0)
            volume = 0;
        if (volume < SDL_SOUND_AUDIBLE_VOLUME)
            return 0;
        if (SdlUsingTownsData())
            return SdlPlayTownsEffect(soundNumber, volume, pan, sourceObject, looping) != 0;
        if (!SdlPlayDosSoundEffect(
                soundNumber, volume, pan, sourceObject, looping))
            return 0;
        if (sourceObject != -1)
            aiSoundEffectSourceActiveArray[sourceObject] = 1;
        if (sourceObject == -1)
            bAfterburnerSfxEnabled = soundNumber == 12;
        return 1;
    }

    if (sourceObject != -1) {
        distance = magnitude > 32000 ? 32000 : (int)magnitude;
    } else {
        distance = 32000;
    }
    if (distance < 10)
        return 0;
    if (sourceObject != -1)
        aiSoundEffectSourceActiveArray[sourceObject] = 1;
    sprintf(szSfxWavePath, "sfx%02i.wav", soundNumber - 1);
    SdlPlayWaveWithPan(szSfxWavePath, looping, distance, pan);
    return 1;
}

void SdlStopDosSoundEffects(void)
{
    SdlStopTownsEffects();
    bAfterburnerSfxEnabled = 0;
    if (g_pSdlDosMusicMutex == 0 ||
        g_pSdlOriginFxSoundPlayer == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlStopOriginFxSoundEffects(g_pSdlOriginFxSoundPlayer);
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

void SdlMixOriginFxMusic(short *samples, unsigned int frameCount)
{
    if (g_bSdlDosMusicInitialized == 0 ||
        g_bSdlOriginFxOwnsAudioDevice != 0 ||
        g_pSdlDosMusicMutex == 0 || samples == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    if (g_pSdlOriginFxPlayer != 0) {
        SdlMixOriginFxPlayer(
            g_pSdlOriginFxPlayer, samples,
            frameCount, g_nSdlDosMusicGain);
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

int SdlGetOriginFxMusicSequencePosition(void)
{
    int position;

    if (g_bSdlDosMusicInitialized == 0 ||
        g_pSdlDosMusicMutex == 0)
        return -1;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    if (g_pSdlOriginFxPlayer == 0) {
        position = -1;
    } else {
        position = SdlOriginFxPlayerSequencePosition(
            g_pSdlOriginFxPlayer);
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
    return position;
}

void SdlServiceOriginFxMusic(void)
{
    SdlOriginFxPlayer *player;
    unsigned char *midi;
    size_t midiSize;
    int desiredTrack;
    int finishedTrack;

    SdlServiceTownsAudio();
    if (g_bSdlDosMusicInitialized == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlUpdateDosAdlibMusicVolume();
    desiredTrack = nCurrentMusicTrack;
    if (g_bSdlOriginFxServicesAllTracks == 0 &&
        desiredTrack != 19) {
        SdlDeleteDosAdlibTrack();
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
        return;
    }
    if (g_pSdlOriginFxPlayer != 0 &&
        SdlOriginFxPlayerFinished(g_pSdlOriginFxPlayer)) {
        finishedTrack = g_nSdlActiveMusicTrack;
        SdlDeleteDosAdlibTrack();
        nMusicTrackComplete = 1;
        if (nCurrentMusicTrack == finishedTrack)
            nCurrentMusicTrack = -1;
    }
    desiredTrack = nCurrentMusicTrack;
    if (desiredTrack == g_nSdlActiveMusicTrack) {
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
        return;
    }
    if (desiredTrack < 0) {
        SdlDeleteDosAdlibTrack();
        nMusicTrackComplete = 1;
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
        return;
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);

    midi = 0;
    midiSize = 0;
    if (!SdlExtractOriginPacketSection(
            g_pSdlDosMusicArchive,
            g_nSdlDosMusicArchiveSize,
            desiredTrack, &midi, &midiSize)) {
        fprintf(stderr, "Unable to decode OriginFX music track %d.\n",
                desiredTrack);
        nCurrentMusicTrack = -1;
        nMusicTrackComplete = 1;
        return;
    }
    player = g_pSdlMt32MusicSynth != 0
        ? SdlCreateOriginFxMt32Player(midi, midiSize, g_pSdlMt32MusicSynth)
        : SdlCreateOriginFxPlayer(
            midi, midiSize, g_pSdlDosAdlibTimbres,
            g_nSdlDosAdlibTimbreSize);
    free(midi);
    if (player == 0) {
        fprintf(stderr, "Unable to parse OriginFX music track %d.\n",
                desiredTrack);
        nCurrentMusicTrack = -1;
        nMusicTrackComplete = 1;
        return;
    }

    SDL_LockMutex(g_pSdlDosMusicMutex);
    if (nCurrentMusicTrack != desiredTrack) {
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
        SdlDestroyOriginFxPlayer(player);
        return;
    }
    SdlDeleteDosAdlibTrack();
    g_pSdlOriginFxPlayer = player;
    g_nSdlActiveMusicTrack = desiredTrack;
    nMusicTrackComplete = 0;
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

void SdlShutdownOriginFxAudio(void)
{
    if (g_bSdlOriginFxOwnsAudioDevice != 0)
        SdlStopAudio();
    if (g_pSdlDosMusicMutex != 0)
        SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlDeleteDosAdlibTrack();
    SdlStopOriginFxSoundEffects(g_pSdlOriginFxSoundPlayer);
    SdlDestroyOriginFxPlayer(g_pSdlOriginFxSoundPlayer);
    g_pSdlOriginFxSoundPlayer = 0;
    SdlDestroyMt32Synth(g_pSdlMt32MusicSynth);
    SdlDestroyMt32Synth(g_pSdlMt32SoundSynth);
    g_pSdlMt32MusicSynth = 0;
    g_pSdlMt32SoundSynth = 0;
    if (g_pSdlDosMusicMutex != 0)
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
    if (g_pSdlDosMusicMutex != 0) {
        SDL_DestroyMutex(g_pSdlDosMusicMutex);
        g_pSdlDosMusicMutex = 0;
    }
    if (g_bSdlDosMusicAudioCriticalSectionInitialized != 0) {
        DeleteCriticalSection(
            &g_stSdlDosMusicAudioCriticalSection);
        g_bSdlDosMusicAudioCriticalSectionInitialized = 0;
    }
    SDL_free(g_pSdlDosMusicArchive);
    g_pSdlDosMusicArchive = 0;
    free(g_pSdlDosAdlibTimbres);
    g_pSdlDosAdlibTimbres = 0;
    g_nSdlDosMusicArchiveSize = 0;
    g_nSdlDosAdlibTimbreSize = 0;
    g_nSdlMusicVolumeSetting = -1;
    g_nSdlSoundVolumeSetting = -1;
    g_nSdlDosMusicGain = 0;
    g_nSdlDosSoundGain = 0;
    g_nSdlDosRapidFireTag = 0;
    g_bSdlOriginFxOwnsAudioDevice = 0;
    g_bSdlOriginFxServicesAllTracks = 0;
    g_bSdlDosMusicInitialized = 0;
}
