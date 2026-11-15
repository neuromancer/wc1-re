/* SDL2-only OriginFX/AdLib playback for WC1 DOS and Kilrathi Saga data. */
#include "wc1.h"

#include <stdio.h>
#include <stdlib.h>

#define SDL_PORT_MUSIC_PATH_SIZE 4096
#define SDL_PORT_ADLIB_TIMBRE_SECTION 1
#define SDL_SOUND_METRES_PER_VOLUME_STEP 500L
#define SDL_SOUND_FULL_VOLUME 127
#define SDL_SOUND_AUDIBLE_VOLUME 10
#define SDL_SOUND_CENTRE_PAN 64

static CRITICAL_SECTION g_stSdlDosMusicAudioCriticalSection;
static SDL_mutex *g_pSdlDosMusicMutex;
static unsigned char *g_pSdlDosMusicArchive;
static unsigned char *g_pSdlDosAdlibTimbres;
static SdlOriginFxPlayer *g_pSdlOriginFxPlayer;
static SdlOriginFxPlayer *g_pSdlQueuedOriginFxPlayer;
static SdlOriginFxPlayer *g_pSdlOriginFxSoundPlayer;
static size_t g_nSdlDosMusicArchiveSize;
static size_t g_nSdlDosAdlibTimbreSize;
static unsigned int g_nSdlDosMusicGain;
static unsigned int g_nSdlDosSoundGain;
static unsigned int g_nSdlDosRapidFireTag;
static int g_nSdlActiveMusicTrack = -1;
static int g_nSdlQueuedMusicTrack = -1;
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

/* As MusicTimerIsr (035:12d9) does, start a queued song once the playing
 * one has ended, here at the sample where it ended. Call with the music
 * mutex held. */
static void SdlMixOriginFxSong(short *samples, unsigned int frameCount)
{
    unsigned int mixedFrames;

    mixedFrames = 0;
    while (g_pSdlOriginFxPlayer != 0 && mixedFrames < frameCount) {
        mixedFrames += SdlMixOriginFxPlayer(
            g_pSdlOriginFxPlayer, samples + mixedFrames * 2,
            frameCount - mixedFrames, g_nSdlDosMusicGain);
        if (!SdlOriginFxPlayerFinished(g_pSdlOriginFxPlayer) ||
            g_pSdlQueuedOriginFxPlayer == 0)
            break;
        SdlDestroyOriginFxPlayer(g_pSdlOriginFxPlayer);
        g_pSdlOriginFxPlayer = g_pSdlQueuedOriginFxPlayer;
        g_nSdlActiveMusicTrack = g_nSdlQueuedMusicTrack;
        g_pSdlQueuedOriginFxPlayer = 0;
        g_nSdlQueuedMusicTrack = -1;
    }
}

static void SdlMixDosAdlibMusic(void *stream, unsigned int byteCount)
{
    unsigned int frameCount;

    SDL_memset(stream, 0, byteCount);
    if (g_pSdlDosMusicMutex == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    frameCount = byteCount / (sizeof(short) * 2U);
    SdlMixOriginFxSong((short *)stream, frameCount);
    if (g_pSdlOriginFxSoundPlayer != 0) {
        SdlMixOriginFxSoundEffects(
            g_pSdlOriginFxSoundPlayer, stream,
            frameCount, g_nSdlDosSoundGain);
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

static void SdlDeleteDosAdlibTrack(void)
{
    SdlDestroyOriginFxPlayer(g_pSdlOriginFxPlayer);
    g_pSdlOriginFxPlayer = 0;
    g_nSdlActiveMusicTrack = -1;
}

static void SdlDeleteQueuedOriginFxSong(void)
{
    SdlDestroyOriginFxPlayer(g_pSdlQueuedOriginFxPlayer);
    g_pSdlQueuedOriginFxPlayer = 0;
    g_nSdlQueuedMusicTrack = -1;
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
    SDL_free(timbreArchive);

    if (useStandaloneAudio != 0) {
        g_pSdlOriginFxSoundPlayer = SdlCreateOriginFxSoundPlayer(
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

/* The pan, 0 left to 127 right as every effect player takes it, for a unit
 * direction from the camera. Its component along the camera's right vector
 * is positive for objects that transform_objects_to_your_view draws right of
 * the view's centre. */
int SdlSoundEffectPan(const FixedVector *direction)
{
    int scaledPan;
    int pan;

    scaledPan = dot_product(direction, &aShipRightVector[EYE_OBJECT]) *
        SDL_SOUND_CENTRE_PAN;
    if (scaledPan < 0)
        scaledPan = -((-scaledPan + 0xff) / 0x100);
    else
        scaledPan /= 0x100;
    pan = SDL_SOUND_CENTRE_PAN + scaledPan;
    if (pan < 0)
        pan = 0;
    else if (pan > 127)
        pan = 127;
    return pan;
}

int SdlPlayGameSoundEffect(int soundNumber, int sourceObject, int looping)
{
    FixedVector delta;
    long magnitude;
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
        pan = SdlSoundEffectPan(&delta);
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
        /* The driver takes a priority where the game passes looping;
         * UpdateFlightSound (003:0587) plays the fly-by at priority 8. */
        if (!SdlPlayDosSoundEffect(
                soundNumber, volume, pan, sourceObject,
                soundNumber == 2 ? 8 : looping))
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
    SdlMixOriginFxSong(samples, frameCount);
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

static SdlOriginFxPlayer *SdlLoadOriginFxSong(int track)
{
    SdlOriginFxPlayer *player;
    unsigned char *midi;
    size_t midiSize;

    midi = 0;
    midiSize = 0;
    if (!SdlExtractOriginPacketSection(
            g_pSdlDosMusicArchive,
            g_nSdlDosMusicArchiveSize,
            track, &midi, &midiSize)) {
        fprintf(stderr, "Unable to decode OriginFX music track %d.\n",
                track);
        return 0;
    }
    player = SdlCreateOriginFxPlayer(
        midi, midiSize, g_pSdlDosAdlibTimbres,
        g_nSdlDosAdlibTimbreSize);
    free(midi);
    if (player == 0)
        fprintf(stderr, "Unable to parse OriginFX music track %d.\n",
                track);
    return player;
}

void SdlServiceOriginFxMusic(void)
{
    SdlServiceTownsAudio();
    if (g_bSdlDosMusicInitialized == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlUpdateDosAdlibMusicVolume();
    /* Kilrathi Saga data streams its music; OriginFX plays only the
     * intro's track 19 there. */
    if (g_bSdlOriginFxServicesAllTracks == 0 &&
        nCurrentMusicTrack != 19) {
        SdlDeleteQueuedOriginFxSong();
        SdlDeleteDosAdlibTrack();
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
        return;
    }
    if (g_pSdlOriginFxPlayer != 0 &&
        SdlOriginFxPlayerFinished(g_pSdlOriginFxPlayer)) {
        if (g_pSdlQueuedOriginFxPlayer != 0) {
            SdlDeleteDosAdlibTrack();
            g_pSdlOriginFxPlayer = g_pSdlQueuedOriginFxPlayer;
            g_nSdlActiveMusicTrack = g_nSdlQueuedMusicTrack;
            g_pSdlQueuedOriginFxPlayer = 0;
            g_nSdlQueuedMusicTrack = -1;
        } else {
            SdlDeleteDosAdlibTrack();
            nMusicTrackComplete = 1;
        }
    }
    /* SpaceFuneral (151:14e5) sets the branch selector, which picks the
     * funeral arrangement of track 32. */
    SdlSetOriginFxPlayerBranch(
        g_pSdlOriginFxPlayer, nFuneralSequenceActive);
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

/* MusicCommand (003:00d6) for the game's spacetrack. While a change is
 * queued, or the track is the one playing, the command is ignored.
 * Commands 1 and 3 queue the track. The playing song then leaves at its
 * next exit, which keeps launch music and fanfares whole and moves combat
 * music on at a bar line. Commands 0 and 2 start the track at once. The
 * game's command-2 fades last one 60 Hz tick at most. */
int SdlOriginFxMusicCommand(int track, int command)
{
    SdlOriginFxPlayer *player;
    int playing;

    if (g_bSdlDosMusicInitialized == 0 || track < 0)
        return 0;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    playing = g_pSdlOriginFxPlayer != 0 &&
        !SdlOriginFxPlayerFinished(g_pSdlOriginFxPlayer);
    if (g_pSdlQueuedOriginFxPlayer != 0 ||
        (playing && g_nSdlActiveMusicTrack == track)) {
        SDL_UnlockMutex(g_pSdlDosMusicMutex);
        return 0;
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);

    player = SdlLoadOriginFxSong(track);
    if (player == 0)
        return 0;
    SdlSetOriginFxPlayerBranch(player, nFuneralSequenceActive);
    SDL_LockMutex(g_pSdlDosMusicMutex);
    playing = g_pSdlOriginFxPlayer != 0 &&
        !SdlOriginFxPlayerFinished(g_pSdlOriginFxPlayer);
    if ((command == 1 || command == 3) && playing) {
        SdlLeaveOriginFxPlayerAtLoop(g_pSdlOriginFxPlayer);
        g_pSdlQueuedOriginFxPlayer = player;
        g_nSdlQueuedMusicTrack = track;
    } else {
        SdlDeleteDosAdlibTrack();
        g_pSdlOriginFxPlayer = player;
        g_nSdlActiveMusicTrack = track;
        nMusicTrackComplete = 0;
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
    return 1;
}

/* SetMusBreakpt is where WC.EXE queues no song (QueueNextSong(0, 0), as
 * in StartQueuedMusic): the playing song ends at its next exit, and the
 * game waits for that end. */
void SdlOriginFxMusicBreakpoint(void)
{
    if (g_bSdlDosMusicInitialized == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlDeleteQueuedOriginFxSong();
    SdlLeaveOriginFxPlayerAtLoop(g_pSdlOriginFxPlayer);
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

/* StopMusic: stops the song at once and drops a queued one, as
 * FadeOutMusic(0) (035:149e) does. */
void SdlStopOriginFxMusic(void)
{
    if (g_bSdlDosMusicInitialized == 0)
        return;
    SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlDeleteQueuedOriginFxSong();
    if (g_pSdlOriginFxPlayer != 0) {
        SdlDeleteDosAdlibTrack();
        nMusicTrackComplete = 1;
    }
    SDL_UnlockMutex(g_pSdlDosMusicMutex);
}

void SdlShutdownOriginFxAudio(void)
{
    if (g_bSdlOriginFxOwnsAudioDevice != 0)
        SdlStopAudio();
    if (g_pSdlDosMusicMutex != 0)
        SDL_LockMutex(g_pSdlDosMusicMutex);
    SdlDeleteQueuedOriginFxSong();
    SdlDeleteDosAdlibTrack();
    SdlStopOriginFxSoundEffects(g_pSdlOriginFxSoundPlayer);
    SdlDestroyOriginFxPlayer(g_pSdlOriginFxSoundPlayer);
    g_pSdlOriginFxSoundPlayer = 0;
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
