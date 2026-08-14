#include "wc1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_DOS_MUSIC_TRACK_COUNT 41

static void BuildAudibleTimbre(unsigned char *timbre,
                               unsigned int program)
{
    memset(timbre, 0, 48);
    timbre[0] = 0x01;
    timbre[1] = 0x20;
    timbre[2] = 0xf2;
    timbre[3] = 0x74;
    timbre[5] = 0x01;
    timbre[7] = 0xf2;
    timbre[8] = 0x74;
    timbre[12] = 5;
    timbre[13] = 5;
    timbre[14] = 0x60;
    timbre[47] = (unsigned char)program;
}

static void BuildTestTimbre(unsigned char *timbres)
{
    memset(timbres, 0, 49);
    timbres[0] = 1;
    BuildAudibleTimbre(timbres + 1, 0);
}

static unsigned int CountMidpointCrossings(
    const short *samples, unsigned int firstFrame,
    unsigned int frameCount)
{
    short minimum;
    short maximum;
    int midpoint;
    unsigned int crossings;
    unsigned int frame;

    minimum = samples[firstFrame * 2];
    maximum = minimum;
    frame = 1;
    while (frame < frameCount) {
        if (samples[(firstFrame + frame) * 2] < minimum)
            minimum = samples[(firstFrame + frame) * 2];
        if (samples[(firstFrame + frame) * 2] > maximum)
            maximum = samples[(firstFrame + frame) * 2];
        frame++;
    }
    midpoint = ((int)minimum + maximum) / 2;
    crossings = 0;
    frame = 1;
    while (frame < frameCount) {
        if (samples[(firstFrame + frame - 1) * 2] <= midpoint &&
            samples[(firstFrame + frame) * 2] > midpoint)
            crossings++;
        frame++;
    }
    return crossings;
}

static unsigned long long SumAbsoluteSamples(
    const short *samples, unsigned int firstFrame,
    unsigned int frameCount)
{
    unsigned long long total;
    int sample;
    unsigned int frame;

    total = 0;
    frame = 0;
    while (frame < frameCount) {
        sample = samples[(firstFrame + frame) * 2];
        if (sample < 0)
            sample = -sample;
        total += (unsigned int)sample;
        frame++;
    }
    return total;
}

static int CheckSyntheticSong(void)
{
    const unsigned char midi[37] = {
        'M', 'T', 'h', 'd',
        0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x60,
        'M', 'T', 'r', 'k',
        0x00, 0x00, 0x00, 0x0f,
        0x00, 0xc0, 0x00,
        0x00, 0x90, 0x3c, 0x7f,
        0x60, 0x80, 0x3c, 0x00,
        0x00, 0xff, 0x2f, 0x00
    };
    unsigned char timbres[49];
    short samples[512 * 2];
    Wc1SdlOriginFxPlayer *player;
    unsigned int frame;
    unsigned int index;
    int heardOutput;

    BuildTestTimbre(timbres);
    player = Wc1SdlCreateOriginFxPlayer(
        midi, sizeof(midi), timbres, sizeof(timbres));
    if (player == 0)
        return 0;
    frame = 0;
    heardOutput = 0;
    while (frame < 12000) {
        Wc1SdlRenderOriginFxPlayer(player, samples, 512, 0x7fff);
        index = 0;
        while (index < sizeof(samples) / sizeof(samples[0])) {
            if (samples[index] != 0)
                heardOutput = 1;
            index++;
        }
        frame += 512;
    }
    if (!Wc1SdlOriginFxPlayerFinished(player))
        heardOutput = 0;
    Wc1SdlDestroyOriginFxPlayer(player);
    return heardOutput;
}

static int CheckSyntheticSoundEffects(void)
{
    unsigned char timbres[49];
    short samples[4096 * 2];
    Wc1SdlOriginFxPlayer *player;
    unsigned int frame;
    unsigned int effect;
    int heardOutput;
    int result;

    BuildTestTimbre(timbres);
    player = Wc1SdlCreateOriginFxSoundPlayer(
        timbres, sizeof(timbres));
    if (player == 0)
        return 0;
    result =
        !Wc1SdlPlayOriginFxSoundEffect(player, 0, 127, 64, 1, 0) &&
        !Wc1SdlPlayOriginFxSoundEffect(player, 37, 127, 64, 1, 0) &&
        Wc1SdlPlayOriginFxSoundEffect(player, 1, 127, 64, 1, 0);
    memset(samples, 0, sizeof(samples));
    Wc1SdlMixOriginFxSoundEffects(
        player, samples, 4096, 0x7fff);
    heardOutput = 0;
    frame = 0;
    while (frame < 4096) {
        if (samples[frame * 2] != samples[frame * 2 + 1])
            result = 0;
        if (samples[frame * 2] != 0)
            heardOutput = 1;
        frame++;
    }

    Wc1SdlStopOriginFxSoundEffects(player);
    effect = 0;
    while (effect < 8) {
        if (!Wc1SdlPlayOriginFxSoundEffect(
                player, 1, 127, 64, (int)effect, 10))
            result = 0;
        effect++;
    }
    if (Wc1SdlPlayOriginFxSoundEffect(
            player, 1, 127, 64, 100, 9))
        result = 0;
    if (!Wc1SdlPlayOriginFxSoundEffect(
            player, 1, 127, 64, 0, 0))
        result = 0;
    Wc1SdlStopOriginFxSoundEffects(player);
    Wc1SdlDestroyOriginFxPlayer(player);
    return result && heardOutput;
}

static int CheckInvalidInputs(void)
{
    const unsigned char invalidMidi[4] = { 'N', 'O', 'P', 'E' };
    unsigned char timbres[49];

    BuildTestTimbre(timbres);
    return Wc1SdlCreateOriginFxPlayer(
        invalidMidi, sizeof(invalidMidi), timbres, sizeof(timbres)) == 0;
}

static int CheckOriginalPercussionMapping(void)
{
    const unsigned char percussionMidi[34] = {
        'M', 'T', 'h', 'd',
        0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x60,
        'M', 'T', 'r', 'k',
        0x00, 0x00, 0x00, 0x0c,
        0x00, 0x99, 0x26, 0x7f,
        0x60, 0x89, 0x26, 0x00,
        0x00, 0xff, 0x2f, 0x00
    };
    const unsigned char melodicMidi[37] = {
        'M', 'T', 'h', 'd',
        0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x60,
        'M', 'T', 'r', 'k',
        0x00, 0x00, 0x00, 0x0f,
        0x00, 0xc0, 0x72,
        0x00, 0x90, 0x30, 0x7f,
        0x60, 0x80, 0x30, 0x00,
        0x00, 0xff, 0x2f, 0x00
    };
    unsigned char timbres[97];
    short percussionSamples[12000 * 2];
    short melodicSamples[12000 * 2];
    Wc1SdlOriginFxPlayer *percussionPlayer;
    Wc1SdlOriginFxPlayer *melodicPlayer;
    unsigned int sampleIndex;
    int heardOutput;
    int result;

    memset(timbres, 0, sizeof(timbres));
    timbres[0] = 2;
    BuildAudibleTimbre(timbres + 1, 2);
    BuildAudibleTimbre(timbres + 49, 0x72);
    percussionPlayer = Wc1SdlCreateOriginFxPlayer(
        percussionMidi, sizeof(percussionMidi), timbres, sizeof(timbres));
    melodicPlayer = Wc1SdlCreateOriginFxPlayer(
        melodicMidi, sizeof(melodicMidi), timbres, sizeof(timbres));
    if (percussionPlayer == 0 || melodicPlayer == 0) {
        Wc1SdlDestroyOriginFxPlayer(percussionPlayer);
        Wc1SdlDestroyOriginFxPlayer(melodicPlayer);
        return 0;
    }
    Wc1SdlRenderOriginFxPlayer(
        percussionPlayer, percussionSamples, 12000, 0x7fff);
    Wc1SdlRenderOriginFxPlayer(
        melodicPlayer, melodicSamples, 12000, 0x7fff);
    result = memcmp(
        percussionSamples, melodicSamples,
        sizeof(percussionSamples)) == 0;
    heardOutput = 0;
    sampleIndex = 0;
    while (sampleIndex <
           sizeof(percussionSamples) / sizeof(percussionSamples[0])) {
        if (percussionSamples[sampleIndex] != 0)
            heardOutput = 1;
        sampleIndex++;
    }
    Wc1SdlDestroyOriginFxPlayer(percussionPlayer);
    Wc1SdlDestroyOriginFxPlayer(melodicPlayer);
    return result && heardOutput;
}

static int CheckOriginalPitchModulation(void)
{
    const unsigned char midi[37] = {
        'M', 'T', 'h', 'd',
        0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x60,
        'M', 'T', 'r', 'k',
        0x00, 0x00, 0x00, 0x0f,
        0x00, 0xc0, 0x00,
        0x00, 0x90, 0x3c, 0x7f,
        0x60, 0x80, 0x3c, 0x00,
        0x00, 0xff, 0x2f, 0x00
    };
    unsigned char plainTimbres[49];
    unsigned char modulatedTimbres[49];
    short plainSamples[4096 * 2];
    short modulatedSamples[4096 * 2];
    Wc1SdlOriginFxPlayer *plainPlayer;
    Wc1SdlOriginFxPlayer *modulatedPlayer;
    unsigned int sampleIndex;
    int heardPlain;
    int heardModulated;
    int result;

    BuildTestTimbre(plainTimbres);
    memcpy(modulatedTimbres, plainTimbres, sizeof(modulatedTimbres));
    /* STRAX.DRV initializes these from TIM bytes 16-17 and applies the
     * resulting pitch modulation in its 60 Hz service routine. */
    modulatedTimbres[1 + 16] = 20;
    modulatedTimbres[1 + 17] = 10;
    plainPlayer = Wc1SdlCreateOriginFxPlayer(
        midi, sizeof(midi), plainTimbres, sizeof(plainTimbres));
    modulatedPlayer = Wc1SdlCreateOriginFxPlayer(
        midi, sizeof(midi), modulatedTimbres, sizeof(modulatedTimbres));
    if (plainPlayer == 0 || modulatedPlayer == 0) {
        Wc1SdlDestroyOriginFxPlayer(plainPlayer);
        Wc1SdlDestroyOriginFxPlayer(modulatedPlayer);
        return 0;
    }
    Wc1SdlRenderOriginFxPlayer(
        plainPlayer, plainSamples, 4096, 0x7fff);
    Wc1SdlRenderOriginFxPlayer(
        modulatedPlayer, modulatedSamples, 4096, 0x7fff);
    result = memcmp(
        plainSamples, modulatedSamples, sizeof(plainSamples)) != 0;
    heardPlain = 0;
    heardModulated = 0;
    sampleIndex = 0;
    while (sampleIndex < sizeof(plainSamples) / sizeof(plainSamples[0])) {
        if (plainSamples[sampleIndex] != 0)
            heardPlain = 1;
        if (modulatedSamples[sampleIndex] != 0)
            heardModulated = 1;
        sampleIndex++;
    }
    Wc1SdlDestroyOriginFxPlayer(plainPlayer);
    Wc1SdlDestroyOriginFxPlayer(modulatedPlayer);
    return result && heardPlain && heardModulated;
}

static int CheckOriginalPitchEnvelope(void)
{
    const unsigned char midi[38] = {
        'M', 'T', 'h', 'd',
        0x00, 0x00, 0x00, 0x06,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x60,
        'M', 'T', 'r', 'k',
        0x00, 0x00, 0x00, 0x10,
        0x00, 0xc0, 0x00,
        0x00, 0x90, 0x3c, 0x7f,
        0x83, 0x00, 0x80, 0x3c, 0x00,
        0x00, 0xff, 0x2f, 0x00
    };
    unsigned char timbres[49];
    short samples[14000 * 2];
    Wc1SdlOriginFxPlayer *player;
    unsigned int earlyCrossings;
    unsigned int lateCrossings;

    BuildTestTimbre(timbres);
    timbres[1 + 1] = 0x3f;
    timbres[1 + 6] = 0;
    /* STRAX.DRV stores each envelope stage as rate followed by target. */
    timbres[1 + 20] = 64;
    timbres[1 + 21] = 0;
    timbres[1 + 22] = 0;
    timbres[1 + 23] = 0xf0;
    player = Wc1SdlCreateOriginFxPlayer(
        midi, sizeof(midi), timbres, sizeof(timbres));
    if (player == 0)
        return 0;
    Wc1SdlRenderOriginFxPlayer(
        player, samples, 14000, 0x7fff);
    earlyCrossings = CountMidpointCrossings(samples, 1000, 2000);
    lateCrossings = CountMidpointCrossings(samples, 10000, 2000);
    Wc1SdlDestroyOriginFxPlayer(player);
    return earlyCrossings > 10 && lateCrossings > 10 &&
        lateCrossings * 4 < earlyCrossings * 3;
}

static int CheckOriginalRhythmRelease(void)
{
    unsigned char timbres[97];
    short recoveredSamples[1024 * 2];
    short rhythmSamples[1024 * 2];
    short discardedSamples[4096 * 2];
    Wc1SdlOriginFxPlayer *recoveredPlayer;
    Wc1SdlOriginFxPlayer *rhythmPlayer;
    unsigned long long recoveredEnergy;
    unsigned long long rhythmEnergy;
    unsigned int effect;
    unsigned int elapsed;
    unsigned int frameCount;
    int result;

    memset(timbres, 0, sizeof(timbres));
    timbres[0] = 2;
    BuildAudibleTimbre(timbres + 1, 0);
    BuildAudibleTimbre(timbres + 49, 44);
    timbres[49 + 1] = 0x3f;
    timbres[49 + 6] = 0x3f;
    timbres[49 + 11] = 6;
    recoveredPlayer = Wc1SdlCreateOriginFxSoundPlayer(
        timbres, sizeof(timbres));
    rhythmPlayer = Wc1SdlCreateOriginFxSoundPlayer(
        timbres, sizeof(timbres));
    if (recoveredPlayer == 0 || rhythmPlayer == 0) {
        Wc1SdlDestroyOriginFxPlayer(recoveredPlayer);
        Wc1SdlDestroyOriginFxPlayer(rhythmPlayer);
        return 0;
    }

    result = Wc1SdlPlayOriginFxSoundEffect(
        recoveredPlayer, 23, 127, 64, 100, 0);
    memset(discardedSamples, 0, sizeof(discardedSamples));
    Wc1SdlMixOriginFxSoundEffects(
        recoveredPlayer, discardedSamples, 4096, 0x1000);

    result = result && Wc1SdlPlayOriginFxSoundEffect(
        rhythmPlayer, 23, 127, 64, 100, 0);
    elapsed = 0;
    while (elapsed < 4096 && result) {
        frameCount = 4096 - elapsed;
        if (frameCount > 1000)
            frameCount = 1000;
        memset(discardedSamples, 0,
               (size_t)frameCount * sizeof(short) * 2);
        Wc1SdlMixOriginFxSoundEffects(
            rhythmPlayer, discardedSamples, frameCount, 0x1000);
        elapsed += frameCount;
        if (elapsed < 4096)
            result = Wc1SdlPlayOriginFxSoundEffect(
                rhythmPlayer, 23, 127, 64, 100, 0);
    }

    effect = 0;
    while (effect < 8 && result) {
        result = Wc1SdlPlayOriginFxSoundEffect(
            recoveredPlayer, 1, 127, 64, (int)effect, 0);
        effect++;
    }
    effect = 0;
    while (effect < 6 && result) {
        result = Wc1SdlPlayOriginFxSoundEffect(
            rhythmPlayer, 1, 127, 64, (int)effect, 0);
        effect++;
    }
    memset(recoveredSamples, 0, sizeof(recoveredSamples));
    memset(rhythmSamples, 0, sizeof(rhythmSamples));
    Wc1SdlMixOriginFxSoundEffects(
        recoveredPlayer, recoveredSamples, 1024, 0x1000);
    Wc1SdlMixOriginFxSoundEffects(
        rhythmPlayer, rhythmSamples, 1024, 0x1000);
    recoveredEnergy = SumAbsoluteSamples(
        recoveredSamples, 128, 896);
    rhythmEnergy = SumAbsoluteSamples(
        rhythmSamples, 128, 896);
    Wc1SdlStopOriginFxSoundEffects(recoveredPlayer);
    Wc1SdlStopOriginFxSoundEffects(rhythmPlayer);
    Wc1SdlDestroyOriginFxPlayer(recoveredPlayer);
    Wc1SdlDestroyOriginFxPlayer(rhythmPlayer);
    return result && recoveredEnergy != 0 && rhythmEnergy != 0 &&
        recoveredEnergy * 100 > rhythmEnergy * 115;
}

static int CheckRetailDosSoundEffects(void)
{
    unsigned char *timbreArchive;
    unsigned char *timbres;
    short samples[512 * 2];
    Wc1SdlOriginFxPlayer *player;
    size_t timbreArchiveSize;
    size_t timbreSize;
    unsigned int soundNumber;
    unsigned int sampleIndex;
    int heardOutput;
    int result;

    timbreArchive = (unsigned char *)SDL_LoadFile(
        "data/dos/GAMEDAT/WINGLDR.TIM", &timbreArchiveSize);
    if (timbreArchive == 0)
        return 1;
    timbres = 0;
    result = Wc1SdlExtractOriginPacketSection(
        timbreArchive, timbreArchiveSize, 1, &timbres, &timbreSize);
    SDL_free(timbreArchive);
    if (!result)
        return 0;
    player = Wc1SdlCreateOriginFxSoundPlayer(timbres, timbreSize);
    free(timbres);
    if (player == 0)
        return 0;
    soundNumber = 1;
    while (soundNumber <= 36 && result) {
        Wc1SdlStopOriginFxSoundEffects(player);
        result = Wc1SdlPlayOriginFxSoundEffect(
            player, soundNumber, 127, 64,
            (int)soundNumber, 0);
        memset(samples, 0, sizeof(samples));
        Wc1SdlMixOriginFxSoundEffects(
            player, samples, 512, 0x7fff);
        heardOutput = 0;
        sampleIndex = 0;
        while (sampleIndex < sizeof(samples) / sizeof(samples[0])) {
            if (samples[sampleIndex] != 0)
                heardOutput = 1;
            sampleIndex++;
        }
        if (!heardOutput)
            result = 0;
        soundNumber++;
    }
    Wc1SdlStopOriginFxSoundEffects(player);
    Wc1SdlDestroyOriginFxPlayer(player);
    return result;
}

static int CheckRetailDosSongs(void)
{
    unsigned char *musicArchive;
    unsigned char *timbreArchive;
    unsigned char *timbres;
    unsigned char *midi;
    short samples[256 * 2];
    Wc1SdlOriginFxPlayer *player;
    size_t musicArchiveSize;
    size_t timbreArchiveSize;
    size_t timbreSize;
    size_t midiSize;
    unsigned int track;
    int result;

    musicArchive = (unsigned char *)SDL_LoadFile(
        "data/dos/GAMEDAT/MUSIC.MID", &musicArchiveSize);
    timbreArchive = (unsigned char *)SDL_LoadFile(
        "data/dos/GAMEDAT/WINGLDR.TIM", &timbreArchiveSize);
    if (musicArchive == 0 && timbreArchive == 0)
        return 1;
    if (musicArchive == 0 || timbreArchive == 0) {
        SDL_free(musicArchive);
        SDL_free(timbreArchive);
        return 0;
    }

    timbres = 0;
    result = Wc1SdlExtractOriginPacketSection(
        timbreArchive, timbreArchiveSize, 1, &timbres, &timbreSize);
    track = 0;
    while (result && track < TEST_DOS_MUSIC_TRACK_COUNT) {
        midi = 0;
        if (!Wc1SdlExtractOriginPacketSection(
                musicArchive, musicArchiveSize, track, &midi, &midiSize)) {
            result = 0;
            break;
        }
        player = Wc1SdlCreateOriginFxPlayer(
            midi, midiSize, timbres, timbreSize);
        free(midi);
        if (player == 0) {
            fprintf(stderr, "Unable to parse DOS music track %u\n", track);
            result = 0;
            break;
        }
        Wc1SdlRenderOriginFxPlayer(
            player, samples, 256, 0x7fff);
        Wc1SdlDestroyOriginFxPlayer(player);
        track++;
    }
    midi = 0;
    if (result && Wc1SdlExtractOriginPacketSection(
            musicArchive, musicArchiveSize, TEST_DOS_MUSIC_TRACK_COUNT,
            &midi, &midiSize)) {
        free(midi);
        result = 0;
    }
    free(timbres);
    SDL_free(musicArchive);
    SDL_free(timbreArchive);
    return result;
}

int main(int argumentCount, char **arguments)
{
    if (!CheckSyntheticSong()) {
        fprintf(stderr, "Synthetic OriginFX/AdLib song test failed\n");
        return 1;
    }
    if (!CheckInvalidInputs()) {
        fprintf(stderr, "Invalid OriginFX input test failed\n");
        return 1;
    }
    if (!CheckSyntheticSoundEffects()) {
        fprintf(stderr, "Synthetic OriginFX sound-effect test failed\n");
        return 1;
    }
    if (!CheckOriginalPercussionMapping()) {
        fprintf(stderr, "Original OriginFX percussion mapping test failed\n");
        return 1;
    }
    if (!CheckOriginalPitchModulation()) {
        fprintf(stderr, "Original OriginFX pitch modulation test failed\n");
        return 1;
    }
    if (!CheckOriginalPitchEnvelope()) {
        fprintf(stderr, "Original OriginFX pitch envelope test failed\n");
        return 1;
    }
    if (!CheckOriginalRhythmRelease()) {
        fprintf(stderr, "Original OriginFX rhythm release test failed\n");
        return 1;
    }
    if (!CheckRetailDosSoundEffects()) {
        fprintf(stderr, "Retail DOS OriginFX sound-effect test failed\n");
        return 1;
    }
    if (!CheckRetailDosSongs()) {
        fprintf(stderr, "Retail DOS OriginFX song test failed\n");
        return 1;
    }
    return 0;
}
