/* Synthetic PCM/scene integration fixture. No recordings are embedded. */
#include "wc1sdl.h"
static Uint32 voiceTestTicks;
static Uint32 VoiceTestClock(void)
{
    return voiceTestTicks;
}
#define SDL_GetTicks VoiceTestClock
#include "../src/sdl/audio.c"
#undef SDL_GetTicks
#include "../src/sdl/speech.c"

void EnterCriticalSection(CRITICAL_SECTION *section)
{
    SDL_LockMutex(section->mutex);
}

void LeaveCriticalSection(CRITICAL_SECTION *section)
{
    SDL_UnlockMutex(section->mutex);
}

void SdlMixOriginFxMusic(short *stream, unsigned int frames)
{
    (void)stream;
    (void)frames;
}

static void Silence(void *stream, unsigned int bytes)
{
    SDL_memset(stream, 0, bytes);
}

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        exit(1);
    }
}

static long Mix(void)
{
    short output[256];
    long sum = 0;
    size_t index;
    voiceTestTicks += 6;
    SdlAudioCallback(0, (Uint8 *)output, sizeof(output));
    for (index = 0; index < SDL_arraysize(output); index++)
        sum += output[index];
    return sum;
}

static void CheckMouthTiming(const char *path)
{
    short commands[] = {2, 1, 9, 50, 3, 1, -1};
    Uint8 *output;
    Require(SdlVoiceMouthFrame(commands, 5) == 5, "unvoiced line preserves original mouth animation");
    Require(SdlPlayVoiceWav(path), "load speech/pause fixture");
    voiceTestTicks += 2000;
    Require(SdlVoicePlaying() && SdlVoiceMouthFrame(commands, 5) == 9,
            "mouth stays closed until the audio callback starts playback");
    output = SDL_calloc(1, voiceBytes);
    Require(output != 0, "allocate test mix buffer");
    SdlAudioCallback(0, output, voiceBytes);
    SDL_free(output);
    Require(SdlVoicePlaying() && SdlVoiceMouthFrame(commands, 5) == 9,
            "queued audio is not considered already played");
    voiceTestTicks += 150;
    Require(SdlVoiceMouthFrame(commands, 5) == 2, "first phoneme follows first spoken segment");
    voiceTestTicks += 200;
    Require(SdlVoiceMouthFrame(commands, 5) == 9, "mouth closes in the recorded pause");
    voiceTestTicks += 200;
    Require(SdlVoiceMouthFrame(commands, 5) == 3, "second phoneme resumes after the pause");
    voiceTestTicks += 200;
    Require(SdlVoicePlaying() && SdlVoiceMouthFrame(commands, 5) == 9, "silent tail remains closed");
    voiceTestTicks += 100;
    Require(!SdlVoicePlaying() && SdlVoiceMouthFrame(commands, 5) == 9, "completion closes the mouth");
    SdlStopVoice();
    Require(SdlVoiceMouthFrame(commands, 5) == 5, "skip clears speech timing for the next line");
}

static void CheckRadio(void)
{
    Require(SdlPlayRadioVoice("GAMEDAT/communic.dat", 0, 0, 2, "Hello $R $C") && Mix() > 0,
            "radio lookup accepts the installed filename");
    Require(SdlRadioVoicePlaying() && !SdlSceneVoicePlaying(), "radio does not hold scene timers");
    Require(SdlPlayRadioVoice("COMMUNIC.DAT", 1, 0, 2, "Hello $R $C") && Mix() < 0,
            "identical text selects the correct speaker");
    Require(SdlPlayRadioVoice("COMMUNIC.DAT", 2, 0, 0, "Hello $R $C") && Mix() > 0,
            "junior rank selects its recording");
    Require(SdlPlayRadioVoice("COMMUNIC.DAT", 2, 0, 4, "Hola $R $C") && Mix() < 0,
            "Spanish radio selects the senior rank recording");
    Require(!SdlPlayRadioVoice("COMMUNIC.DAT", 2, 0, 3, "Hello $R $C") && !SdlVoicePlaying(),
            "missing rank stays silent and stops the previous transmission");
    Require(!SdlPlayRadioVoice("COMMSM2.DAT", 0, 0, 0, "Hello $R $C"), "SM2 never reuses base radio lines");
    Require(!SdlPlayRadioVoice("COMMUNIC.DAT", 0, 0, 0, "Different line"), "changed text cannot play an incorrect recording");
    Require(SdlPlayRadioVoice("COMMUNIC.DAT", 0, 0, 0, "Hello $R $C"), "restart radio");
    SdlStopRadioVoice();
    Require(!SdlVoicePlaying(), "closing communications stops radio audio");
}

int main(int argc, char **argv)
{
    CRITICAL_SECTION critical;
    unsigned char *english, *spanish;
    size_t englishSize, spanishSize;
    int installed = argc == 5 && strcmp(argv[1], "--installed") == 0;

    if (argc == 4 && strcmp(argv[1], "--detect") == 0) {
        char directory[4096];
        Require(SdlFindVoiceDirectory(argv[2], directory, sizeof(directory)) == atoi(argv[3]), "detect installed voice layout");
        return 0;
    }

    Require(argc == 5, "catalog, English fixture, Spanish fixture, speech/pause WAV arguments");
    SDL_setenv("SDL_AUDIODRIVER", "dummy", 1);
    critical.mutex = SDL_CreateMutex();
    Require(critical.mutex != 0 && SdlStartAudio(Silence, &critical, 0), "start silent audio device");
    SDL_PauseAudioDevice(g_wAudioDevice, 1);
    Require(installed ? SdlLoadInstalledVoices(argv[2]) && voiceCount != 0 : SdlLoadVoiceCatalog(argv[1]), "load voice catalog");
    english = SDL_LoadFile(argv[installed ? 3 : 2], &englishSize);
    spanish = SDL_LoadFile(argv[installed ? 4 : 3], &spanishSize);
    Require(english && spanish, "read scene fixtures");
    SdlRegisterVoicePacket("GAMEDAT/BRIEFING.000", 1, english, englishSize);
    Require(SdlPlaySceneVoice(english + 8, english + 8), "play first English record");
    SdlStopRadioVoice();
    Require(SdlSceneVoicePlaying() && !SdlRadioVoicePlaying(), "radio cleanup leaves scene speech playing");
    Require(SdlVoicePlaying() && Mix() > 0, "resample unsigned mono WAV into positive stereo PCM");
    Require(SdlPlaySceneVoice(english + 8, english + 21), "same text in another record selects another voice");
    Require(Mix() < 0, "second context chooses negative PCM");
    SdlStopVoice();
    Require(!SdlVoicePlaying() && Mix() == 0, "skip stops voice immediately");
    Require(SdlPlaySceneVoice(english + 8, english + 8), "restart recording");
    while (SdlVoicePlaying())
        Mix();
    Require(!SdlVoicePlaying(), "recording completes without extending playback");
    SdlRegisterVoicePacket("BRIEFING.000", 1, spanish, spanishSize);
    Require(SdlPlaySceneVoice(spanish + 8, spanish + 8) && Mix() > 0, "Spanish subtitles retain English voice");
    Require(!SdlPlaySceneVoice(english + 8, english + 34), "end-of-scene sentinel stays silent");
    SdlForgetVoicePacket(english);
    Require(!SdlPlaySceneVoice(english + 8, english + 8), "freed packet cannot trigger a recording");
    SdlRegisterVoicePacket("BRIEFING.001", 1, english, englishSize);
    Require(!SdlPlaySceneVoice(english + 8, english + 8), "Secret Missions does not reuse WC1 voices");
    SdlRegisterVoicePacket("BRIEFING.000", 1, english, 9);
    Require(!SdlPlaySceneVoice(english + 8, english + 8), "truncated packet stays silent");
    if (!installed) {
        CheckRadio();
        CheckMouthTiming(argv[4]);
    }
    SdlShutdownVoices();
    Require(!SdlVoicePlaying(), "shutdown stops playback");
    SDL_free(english);
    SDL_free(spanish);
    SdlStopAudio();
    SDL_DestroyMutex(critical.mutex);
    SDL_Quit();
    puts("PASS: PO contexts, mixing, playback clock, mouth timing/pauses, radio speakers/ranks, Spanish subtitles and packet lifetime.");
    return 0;
}
