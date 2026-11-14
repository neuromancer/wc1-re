/* Renders WC1's DOS music, or a sound effect, through the MT-32 path to a
 * 22050 Hz WAV file with the player's own ROMs, and reports its levels.
 *
 *   mt32-render GAME_DIR ROM_DIR TRACK SECONDS OUT.wav [MACHINE]
 *
 * A negative TRACK plays sound effect -TRACK instead. Needs a build with
 * MODERN_MT32=1. Keep the output, like the ROMs, out of the repository. */
#include "wc1.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RENDER_RATE 22050

static unsigned char *LoadSection(const char *path, unsigned int section,
                                  size_t *size)
{
    char resolved[PATH_MAX];
    unsigned char *archive;
    unsigned char *data;
    size_t archiveSize;

    if (!SdlResolvePath(path, resolved, sizeof(resolved)))
        return 0;
    archive = SDL_LoadFile(resolved, &archiveSize);
    if (archive == 0)
        return 0;
    if (!SdlExtractOriginPacketSection(archive, archiveSize, section, &data, size))
        data = 0;
    SDL_free(archive);
    return data;
}

static void WriteLittleEndian(FILE *file, unsigned long value, int bytes)
{
    while (bytes-- > 0) {
        fputc((int)(value & 0xff), file);
        value >>= 8;
    }
}

static int WriteWave(const char *path, const short *samples, size_t frames)
{
    FILE *file = fopen(path, "wb");
    unsigned long dataSize = (unsigned long)frames * 4UL;

    if (file == 0)
        return 0;
    fwrite("RIFF", 1, 4, file);
    WriteLittleEndian(file, 36 + dataSize, 4);
    fwrite("WAVEfmt ", 1, 8, file);
    WriteLittleEndian(file, 16, 4);
    WriteLittleEndian(file, 1, 2);
    WriteLittleEndian(file, 2, 2);
    WriteLittleEndian(file, RENDER_RATE, 4);
    WriteLittleEndian(file, RENDER_RATE * 4UL, 4);
    WriteLittleEndian(file, 4, 2);
    WriteLittleEndian(file, 16, 2);
    fwrite("data", 1, 4, file);
    WriteLittleEndian(file, dataSize, 4);
    fwrite(samples, sizeof(short), frames * 2, file);
    return fclose(file) == 0;
}

int main(int argc, char **argv)
{
    char description[PATH_MAX + 256];
    char romDirectory[PATH_MAX];
    unsigned char *timbres;
    unsigned char *midi = 0;
    size_t timbreSize;
    size_t midiSize = 0;
    SdlMt32Synth *synth;
    SdlOriginFxPlayer *player;
    short *samples;
    size_t frames;
    size_t frame;
    double sum = 0;
    int peak = 0;
    int track;

    if (argc < 6) {
        fprintf(stderr, "usage: %s GAME_DIR ROM_DIR TRACK SECONDS OUT.wav [MACHINE]\n", argv[0]);
        return 2;
    }
    if (realpath(argv[2], romDirectory) == 0) {
        fprintf(stderr, "Cannot find %s.\n", argv[2]);
        return 1;
    }
    SdlSetMt32Roms(romDirectory, argc > 6 ? argv[6] : "");
    if (!SdlDescribeMt32Roms(romDirectory, SdlGetMt32Machine(), description,
                             sizeof(description))) {
        fprintf(stderr, "%s\n", description);
        return 1;
    }
    printf("ROMs: %s\n", description);
    if (chdir(argv[1]) != 0) {
        fprintf(stderr, "Cannot enter %s.\n", argv[1]);
        return 1;
    }
    track = atoi(argv[3]);
    frames = (size_t)(atof(argv[4]) * RENDER_RATE);
    timbres = LoadSection("GAMEDAT/WINGLDR.TIM", 0, &timbreSize);
    if (timbres == 0 || (track >= 0 &&
            (midi = LoadSection("GAMEDAT/MUSIC.MID", (unsigned int)track, &midiSize)) == 0)) {
        fprintf(stderr, "Cannot load GAMEDAT/WINGLDR.TIM or song %d of GAMEDAT/MUSIC.MID.\n",
                track);
        return 1;
    }
    synth = SdlCreateMt32Synth(RENDER_RATE, timbres, timbreSize);
    if (synth == 0) {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }
    if (track >= 0) {
        player = SdlCreateOriginFxMt32Player(midi, midiSize, synth);
    } else {
        player = SdlCreateOriginFxMt32SoundPlayer(synth);
        if (player != 0 && !SdlPlayOriginFxSoundEffect(player, (unsigned int)-track,
                                                       127, 64, 1, 1)) {
            fprintf(stderr, "Sound effect %d did not start.\n", -track);
            return 1;
        }
    }
    samples = (short *)calloc(frames * 2 + 2, sizeof(short));
    if (player == 0 || samples == 0) {
        fprintf(stderr, "Cannot create the MT-32 player.\n");
        return 1;
    }
    for (frame = 0; frame < frames; frame += 1024) {
        unsigned int count = (unsigned int)(frames - frame < 1024 ? frames - frame : 1024);

        if (track >= 0)
            SdlRenderOriginFxPlayer(player, samples + frame * 2, count, 0x7fff);
        else
            SdlMixOriginFxSoundEffects(player, samples + frame * 2, count, 0x7fff);
    }
    for (frame = 0; frame < frames * 2; frame++) {
        int value = abs(samples[frame]);

        sum += (double)samples[frame] * samples[frame];
        if (value > peak)
            peak = value;
    }
    printf("%s %d: %.1f s, RMS %.0f, peak %d, %s\n", track >= 0 ? "Song" : "Sound effect",
           track >= 0 ? track : -track, (double)frames / RENDER_RATE,
           frames != 0 ? sqrt(sum / (double)(frames * 2)) : 0.0, peak,
           SdlOriginFxPlayerFinished(player) ? "finished" : "still playing");
    if (!WriteWave(argv[5], samples, frames)) {
        fprintf(stderr, "Cannot write %s.\n", argv[5]);
        return 1;
    }
    SdlDestroyOriginFxPlayer(player);
    SdlDestroyMt32Synth(synth);
    free(samples);
    free(midi);
    free(timbres);
    return 0;
}
