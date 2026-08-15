/* SDL2-only OriginFX/AdLib playback for the audio shipped by WC1 DOS. */
#include "wc1.h"

#include <stdio.h>
#include <stdlib.h>

#define WC1_SDL_MUSIC_PATH_SIZE 4096
#define WC1_SDL_ADLIB_TIMBRE_SECTION 1

static CRITICAL_SECTION g_stWc1SdlDosMusicAudioCriticalSection;
static SDL_mutex *g_pWc1SdlDosMusicMutex;
static unsigned char *g_pWc1SdlDosMusicArchive;
static unsigned char *g_pWc1SdlDosAdlibTimbres;
static Wc1SdlOriginFxPlayer *g_pWc1SdlOriginFxPlayer;
static Wc1SdlOriginFxPlayer *g_pWc1SdlOriginFxSoundPlayer;
static size_t g_nWc1SdlDosMusicArchiveSize;
static size_t g_nWc1SdlDosAdlibTimbreSize;
static unsigned int g_nWc1SdlDosMusicGain;
static unsigned int g_nWc1SdlDosSoundGain;
static unsigned int g_nWc1SdlDosRapidFireTag;
static int g_nWc1SdlActiveMusicTrack = -1;
static int g_nWc1SdlMusicVolumeSetting = -1;
static int g_nWc1SdlSoundVolumeSetting = -1;
static int g_bWc1SdlDosMusicAudioCriticalSectionInitialized;
static int g_bWc1SdlDosMusicInitialized;

static unsigned char *Wc1SdlLoadDosMusicFile(
    const char *const *candidates, unsigned int candidateCount,
    size_t *fileSize)
{
    unsigned char *fileData;
    char resolved[WC1_SDL_MUSIC_PATH_SIZE];
    unsigned int candidateIndex;

    candidateIndex = 0;
    while (candidateIndex < candidateCount) {
        if (Wc1SdlResolvePath(
                candidates[candidateIndex], resolved, sizeof(resolved))) {
            fileData = (unsigned char *)SDL_LoadFile(resolved, fileSize);
            if (fileData != 0)
                return fileData;
        }
        candidateIndex++;
    }
    return 0;
}

static void Wc1SdlMixDosAdlibMusic(void *stream, unsigned int byteCount)
{
    unsigned int frameCount;

    SDL_memset(stream, 0, byteCount);
    if (g_pWc1SdlDosMusicMutex == 0)
        return;
    SDL_LockMutex(g_pWc1SdlDosMusicMutex);
    frameCount = byteCount / (sizeof(short) * 2U);
    if (g_pWc1SdlOriginFxPlayer != 0) {
        Wc1SdlRenderOriginFxPlayer(
            g_pWc1SdlOriginFxPlayer, (short *)stream,
            frameCount, g_nWc1SdlDosMusicGain);
    }
    if (g_pWc1SdlOriginFxSoundPlayer != 0) {
        Wc1SdlMixOriginFxSoundEffects(
            g_pWc1SdlOriginFxSoundPlayer, (short *)stream,
            frameCount, g_nWc1SdlDosSoundGain);
    }
    SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
}

static void Wc1SdlDeleteDosAdlibTrack(void)
{
    Wc1SdlDestroyOriginFxPlayer(g_pWc1SdlOriginFxPlayer);
    g_pWc1SdlOriginFxPlayer = 0;
    g_nWc1SdlActiveMusicTrack = -1;
}

static unsigned int Wc1SdlCalculateDosAudioGain(int volumeSetting)
{
    int level;
    int tableIndex;

    tableIndex = volumeSetting / 2;
    if (tableIndex < 0)
        tableIndex = 0;
    else if (tableIndex > 10)
        tableIndex = 10;
    level = g_anVolumeLevels_00469fc8[tableIndex];
    if (level < 0)
        level = 0;
    else if (level > 64000)
        level = 64000;
    return (unsigned int)((long)level * 0x7fffL / 64000L);
}

static void Wc1SdlUpdateDosAdlibMusicVolume(void)
{
    if (g_nWc1SdlMusicVolumeSetting == g_nMusicVolumeSetting_00469fc0 &&
        g_nWc1SdlSoundVolumeSetting == g_nSfxVolumeSetting_00469fbc)
        return;
    if (g_nWc1SdlMusicVolumeSetting !=
        g_nMusicVolumeSetting_00469fc0) {
        g_nWc1SdlMusicVolumeSetting = g_nMusicVolumeSetting_00469fc0;
        g_nWc1SdlDosMusicGain =
            Wc1SdlCalculateDosAudioGain(g_nWc1SdlMusicVolumeSetting);
    }
    if (g_nWc1SdlSoundVolumeSetting !=
        g_nSfxVolumeSetting_00469fbc) {
        g_nWc1SdlSoundVolumeSetting = g_nSfxVolumeSetting_00469fbc;
        g_nWc1SdlDosSoundGain =
            Wc1SdlCalculateDosAudioGain(g_nWc1SdlSoundVolumeSetting);
    }
}

int Wc1SdlInitializeDosAdlibMusic(void)
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

    if (g_bWc1SdlDosMusicInitialized != 0)
        return 1;
    g_pWc1SdlDosMusicArchive = Wc1SdlLoadDosMusicFile(
        musicCandidates, 2, &g_nWc1SdlDosMusicArchiveSize);
    if (g_pWc1SdlDosMusicArchive == 0) {
        fprintf(stderr, "Unable to load DOS GAMEDAT/MUSIC.MID.\n");
        return 0;
    }
    timbreArchive = Wc1SdlLoadDosMusicFile(
        timbreCandidates, 2, &timbreArchiveSize);
    if (timbreArchive == 0) {
        fprintf(stderr, "Unable to load DOS GAMEDAT/WINGLDR.TIM.\n");
        goto failed;
    }
    if (!Wc1SdlExtractOriginPacketSection(
            timbreArchive, timbreArchiveSize,
            WC1_SDL_ADLIB_TIMBRE_SECTION,
            &g_pWc1SdlDosAdlibTimbres,
            &g_nWc1SdlDosAdlibTimbreSize)) {
        SDL_free(timbreArchive);
        fprintf(stderr, "Unable to decode DOS AdLib timbres.\n");
        goto failed;
    }
    SDL_free(timbreArchive);

    g_pWc1SdlOriginFxSoundPlayer = Wc1SdlCreateOriginFxSoundPlayer(
        g_pWc1SdlDosAdlibTimbres,
        g_nWc1SdlDosAdlibTimbreSize);
    if (g_pWc1SdlOriginFxSoundPlayer == 0) {
        fprintf(stderr, "Unable to initialize DOS AdLib sound effects.\n");
        goto failed;
    }

    g_pWc1SdlDosMusicMutex = SDL_CreateMutex();
    if (g_pWc1SdlDosMusicMutex == 0)
        goto failed;
    InitializeCriticalSection(
        &g_stWc1SdlDosMusicAudioCriticalSection);
    g_bWc1SdlDosMusicAudioCriticalSectionInitialized = 1;
    g_bWc1SdlDosMusicInitialized = 1;
    Wc1SdlUpdateDosAdlibMusicVolume();
    if (!Wc1SdlStartAudio(
            Wc1SdlMixDosAdlibMusic,
            &g_stWc1SdlDosMusicAudioCriticalSection, 0))
        goto failed;
    fprintf(stderr, "DOS OriginFX/AdLib audio enabled.\n");
    return 1;

failed:
    Wc1SdlShutdownDosAdlibMusic();
    return 0;
}

int Wc1SdlPlayDosSoundEffect(int soundNumber, int volume, int pan,
                             int tag, int priority)
{
    int result;

    if (g_bWc1SdlDosMusicInitialized == 0 ||
        g_pWc1SdlDosMusicMutex == 0 ||
        g_pWc1SdlOriginFxSoundPlayer == 0)
        return 0;
    SDL_LockMutex(g_pWc1SdlDosMusicMutex);
    if (soundNumber == 8) {
        tag = 64 + (int)(g_nWc1SdlDosRapidFireTag & 1U);
        g_nWc1SdlDosRapidFireTag++;
    }
    result = Wc1SdlPlayOriginFxSoundEffect(
        g_pWc1SdlOriginFxSoundPlayer,
        (unsigned int)soundNumber, volume, pan, tag, priority);
    SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
    return result;
}

void Wc1SdlStopDosSoundEffects(void)
{
    DAT_005a7cec = 0;
    if (g_pWc1SdlDosMusicMutex == 0 ||
        g_pWc1SdlOriginFxSoundPlayer == 0)
        return;
    SDL_LockMutex(g_pWc1SdlDosMusicMutex);
    Wc1SdlStopOriginFxSoundEffects(g_pWc1SdlOriginFxSoundPlayer);
    SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
}

void Wc1SdlServiceDosAdlibMusic(void)
{
    Wc1SdlOriginFxPlayer *player;
    unsigned char *midi;
    size_t midiSize;
    int desiredTrack;
    int finishedTrack;

    if (g_bWc1SdlDosMusicInitialized == 0)
        return;
    SDL_LockMutex(g_pWc1SdlDosMusicMutex);
    Wc1SdlUpdateDosAdlibMusicVolume();
    if (g_pWc1SdlOriginFxPlayer != 0 &&
        Wc1SdlOriginFxPlayerFinished(g_pWc1SdlOriginFxPlayer)) {
        finishedTrack = g_nWc1SdlActiveMusicTrack;
        Wc1SdlDeleteDosAdlibTrack();
        g_nMusicTrackComplete_0046aa04 = 1;
        if (g_nCurrentMusicTrack_0046aa14 == finishedTrack)
            g_nCurrentMusicTrack_0046aa14 = -1;
    }
    desiredTrack = g_nCurrentMusicTrack_0046aa14;
    if (desiredTrack == g_nWc1SdlActiveMusicTrack) {
        SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
        return;
    }
    if (desiredTrack < 0) {
        Wc1SdlDeleteDosAdlibTrack();
        g_nMusicTrackComplete_0046aa04 = 1;
        SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
        return;
    }
    SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);

    midi = 0;
    midiSize = 0;
    if (!Wc1SdlExtractOriginPacketSection(
            g_pWc1SdlDosMusicArchive,
            g_nWc1SdlDosMusicArchiveSize,
            (unsigned int)desiredTrack, &midi, &midiSize)) {
        fprintf(stderr, "Unable to decode DOS music track %d.\n",
                desiredTrack);
        g_nCurrentMusicTrack_0046aa14 = -1;
        g_nMusicTrackComplete_0046aa04 = 1;
        return;
    }
    player = Wc1SdlCreateOriginFxPlayer(
        midi, midiSize, g_pWc1SdlDosAdlibTimbres,
        g_nWc1SdlDosAdlibTimbreSize);
    free(midi);
    if (player == 0) {
        fprintf(stderr, "Unable to parse DOS music track %d.\n",
                desiredTrack);
        g_nCurrentMusicTrack_0046aa14 = -1;
        g_nMusicTrackComplete_0046aa04 = 1;
        return;
    }

    SDL_LockMutex(g_pWc1SdlDosMusicMutex);
    if (g_nCurrentMusicTrack_0046aa14 != desiredTrack) {
        SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
        Wc1SdlDestroyOriginFxPlayer(player);
        return;
    }
    Wc1SdlDeleteDosAdlibTrack();
    g_pWc1SdlOriginFxPlayer = player;
    g_nWc1SdlActiveMusicTrack = desiredTrack;
    g_nMusicTrackComplete_0046aa04 = 0;
    SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
}

void Wc1SdlShutdownDosAdlibMusic(void)
{
    if (g_bWc1SdlDosMusicInitialized != 0)
        Wc1SdlStopAudio();
    if (g_pWc1SdlDosMusicMutex != 0)
        SDL_LockMutex(g_pWc1SdlDosMusicMutex);
    Wc1SdlDeleteDosAdlibTrack();
    Wc1SdlStopOriginFxSoundEffects(g_pWc1SdlOriginFxSoundPlayer);
    Wc1SdlDestroyOriginFxPlayer(g_pWc1SdlOriginFxSoundPlayer);
    g_pWc1SdlOriginFxSoundPlayer = 0;
    if (g_pWc1SdlDosMusicMutex != 0)
        SDL_UnlockMutex(g_pWc1SdlDosMusicMutex);
    if (g_pWc1SdlDosMusicMutex != 0) {
        SDL_DestroyMutex(g_pWc1SdlDosMusicMutex);
        g_pWc1SdlDosMusicMutex = 0;
    }
    if (g_bWc1SdlDosMusicAudioCriticalSectionInitialized != 0) {
        DeleteCriticalSection(
            &g_stWc1SdlDosMusicAudioCriticalSection);
        g_bWc1SdlDosMusicAudioCriticalSectionInitialized = 0;
    }
    SDL_free(g_pWc1SdlDosMusicArchive);
    g_pWc1SdlDosMusicArchive = 0;
    free(g_pWc1SdlDosAdlibTimbres);
    g_pWc1SdlDosAdlibTimbres = 0;
    g_nWc1SdlDosMusicArchiveSize = 0;
    g_nWc1SdlDosAdlibTimbreSize = 0;
    g_nWc1SdlMusicVolumeSetting = -1;
    g_nWc1SdlSoundVolumeSetting = -1;
    g_nWc1SdlDosMusicGain = 0;
    g_nWc1SdlDosSoundGain = 0;
    g_nWc1SdlDosRapidFireTag = 0;
    g_bWc1SdlDosMusicInitialized = 0;
}
