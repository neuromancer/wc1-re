/* The DOS audio path of the game with MT-32 ROMs, through SDL's disk audio
 * driver: SdlInitializeOriginFxAudio, a song and a sound effect, measured
 * where the sound device would receive them. No window opens.
 *
 *   mt32-probe GAME_DIR OUTPUT.raw [ROM_DIR]
 *
 * Without ROM_DIR, or with one that has no ROMs, the game keeps AdLib. */
#include "wc1.h"

#include <math.h>
#include <unistd.h>

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

int main(int argc, char **argv)
{
    char output[PATH_MAX];
    char roms[PATH_MAX];
    unsigned char *audio;
    size_t size;
    size_t index;
    double sum = 0;
    int peak = 0;

    Require(argc >= 3, "usage: mt32-probe GAME_DIR OUTPUT.raw [ROM_DIR]");
    Require(realpath(argv[1], output) != 0, "find the game directory");
    SDL_snprintf(output, sizeof(output), "%s", argv[2]);
    if (argc > 3) {
        if (realpath(argv[3], roms) == 0)
            SDL_strlcpy(roms, argv[3], sizeof(roms));
        SdlSetMt32Roms(roms, "");
    }
    SDL_setenv("SDL_AUDIODRIVER", "disk", 1);
    SDL_setenv("SDL_DISKAUDIOFILE", output, 1);
    Require(chdir(argv[1]) == 0, "enter the game directory");
    nMusicVolumeSetting = 20;
    nSfxVolumeSetting = 20;
    Require(SdlInitializeOriginFxAudio(1), "start the DOS OriginFX audio");
    nCurrentMusicTrack = 0;
    SdlServiceOriginFxMusic();
    Require(SdlPlayDosSoundEffect(8, 127, 64, -1, 1), "play the laser");
    SDL_Delay(3000);
    SdlServiceOriginFxMusic();
    SdlShutdownOriginFxAudio();

    audio = SDL_LoadFile(output, &size);
    Require(audio != 0 && size >= 4 * 22050, "the disk driver wrote at least a second");
    for (index = 0; index + 1 < size; index += 2) {
        short sample = (short)(audio[index] | (audio[index + 1] << 8));

        sum += (double)sample * sample;
        if (abs(sample) > peak)
            peak = abs(sample);
    }
    printf("%.1f s of device audio, RMS %.0f, peak %d\n", (double)size / (4 * 22050),
           sqrt(sum / (double)(size / 2)), peak);
    SDL_free(audio);
    Require(peak > 0, "the device receives sound");
    puts("PASS: DOS audio reaches the device.");
    return 0;
}
