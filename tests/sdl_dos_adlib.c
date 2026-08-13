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

int main(void)
{
    if (!CheckSyntheticSong()) {
        fprintf(stderr, "Synthetic OriginFX/AdLib song test failed\n");
        return 1;
    }
    if (!CheckInvalidInputs()) {
        fprintf(stderr, "Invalid OriginFX input test failed\n");
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
    if (!CheckRetailDosSongs()) {
        fprintf(stderr, "Retail DOS OriginFX song test failed\n");
        return 1;
    }
    return 0;
}
