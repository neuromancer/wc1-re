/* Synthetic decoder/mixer checks plus an optional user's FM Towns disc.
 * No game files are written or embedded. */
#include "../src/sdl/fmtowns.c"

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        exit(1);
    }
}

static void CheckPcm(void)
{
    unsigned char data[36] = {0};
    TownsSample sample = {0};
    short output[24];
    data[12] = 4;
    data[20] = 4;
    data[24] = 0xe2;
    data[25] = 0x10; /* 4322 / 0.098 = 44102 Hz, one source sample per frame. */
    data[28] = 60;
    data[32] = 1;
    data[33] = 0x81;
    data[34] = 0x7f;
    data[35] = 0x80;
    Require(TownsDecodeSample(data, sizeof(data), &sample), "decode sign/magnitude PCM");
    Require(sample.count == 4 && sample.pcm[0] == 256 && sample.pcm[1] == -256 &&
            sample.pcm[2] == 32512 && sample.pcm[3] == 0, "PCM polarity and zero");
    effectGain = 128;
    channels[0] = (TownsChannel){&sample, 0, 1, -1, 1, 127, 0};
    TownsMix(0, (Uint8 *)output, sizeof(output));
    Require(output[0] == 256 && output[1] == 0 && output[2] == -256 &&
            output[8] == 256 && output[16] == 256, "left pan and sample loop");
    SDL_zero(channels);
    SDL_free(sample.pcm);
    data[12] = 7;
    Require(!TownsDecodeSample(data, sizeof(data), &sample), "reject truncated PCM");
    data[12] = 4;
    data[16] = 5;
    Require(!TownsDecodeSample(data, sizeof(data), &sample), "reject invalid loop");
    Require(!TownsDecodeSample(data, 31, &sample), "reject short header");
}

static void CheckInactiveAudio(void)
{
    nMusicTrackComplete = 0;
    nCurrentMusicTrack = 30;
    Require(!SdlPlayTownsEffect(12, 127, 64, -1, 0), "inactive backend cannot start effects");
    Require(!SdlStopTownsEffect(TOWNS_EFFECT_HANDLE | 1), "inactive backend leaves sound stops to the game");
    SdlServiceTownsAudio();
    SdlStopTownsEffects();
    SdlStopTownsRadio();
    SdlShutdownTownsAudio();
    Require(nMusicTrackComplete == 0 && nCurrentMusicTrack == 30,
            "inactive backend preserves other editions' music state");
}

static TownsChannel *PlayingEffect(int handle)
{
    int index;
    for (index = 0; index < 7; index++) {
        if (channels[index].handle == handle && channels[index].sample)
            return &channels[index];
    }
    return 0;
}

static void CheckMusic(void)
{
    short output[4096 * 2];
    Uint64 power = 0;
    unsigned int index, frame;
    nMusicPlaybackMode = 4;
    nMusicVolumeSetting = 20;
    nCurrentMusicTrack = 30;
    SdlTownsMusicCommand(30, 2);
    if (!cdFile) {
        puts("SKIP: CD regression (no local TOWNS.CUE).");
        return;
    }
    Require(cdCue == 5, "rec room retains disc track 5");
    for (frame = 0; frame < 40; frame++) {
        SdlServiceTownsAudio();
        TownsMix(0, (Uint8 *)output, sizeof(output));
        for (index = 0; index < SDL_arraysize(output); index++)
            power += abs(output[index]);
    }
    Require(power > 1000000, "existing CD playback remains audible");
    nMusicVolumeSetting = 0;
    SdlServiceTownsAudio();
    TownsMix(0, (Uint8 *)output, sizeof(output));
    for (index = 0; index < SDL_arraysize(output); index++)
        Require(output[index] == 0, "music volume zero mutes output");
    nCurrentMusicTrack = -1;
    SdlServiceTownsAudio();
    Require(!cdFile && !cdCount && nMusicTrackComplete, "stop clears buffered music");
}

static void CheckGameEffects(void)
{
    short output[4096 * 2];
    int afterburner, alarm, shot, index;
    Uint64 position;
    Require(SdlPlayGameSoundEffect(12, -1, 0), "game starts afterburner");
    afterburner = bAfterburnerSfxEnabled;
    for (index = 0; index < 30; index++)
        TownsMix(0, (Uint8 *)output, sizeof(output));
    Require(PlayingEffect(afterburner) != 0, "SND loop survives beyond sample duration");
    position = PlayingEffect(afterburner)->position;
    Require(SdlPlayGameSoundEffect(12, -1, 0) && bAfterburnerSfxEnabled == afterburner &&
            PlayingEffect(afterburner)->position == position, "repeated afterburner does not restart");
    shot = SdlPlayTownsEffect(8, 127, 64, -1, 0);
    for (index = 1; index >= 0; index--) {
        aeObjectClass[index] = OBJECT_CLASS_SHIP;
        aeShipTacticArray[index] = TACTIC_CRUISE;
        aeSpecialManeuvers[index] = SPECIAL_MANEUVER_AFTERBURNER;
        asShipAfterburnerTimer[index] = 1;
        accelerate_and_move_object(index);
        if (index == 1)
            Require(PlayingEffect(afterburner) != 0, "another ship cannot stop the player's afterburner");
    }
    Require(!PlayingEffect(afterburner) && PlayingEffect(shot), "afterburner stop preserves gunfire");
    Require(SdlPlayGameSoundEffect(32, -1, 0), "damage alarm starts through game path");
    alarm = nCurrentDamageAlarmSfxHandle;
    Require(PlayingEffect(alarm) != 0, "damage alarm publishes its stop handle");
    FlushSoundEffectsAndLog(alarm, 1);
    Require(!PlayingEffect(alarm) && PlayingEffect(shot), "alarm stop preserves gunfire");
    SdlStopTownsEffects();
    bIxAudioEnabled = 0;
    PlaySnowStaticSound();
    for (index = 0; index < 7; index++) {
        if (channels[index].sample && channels[index].effect == 23)
            break;
    }
    Require(index < 7, "cockpit static works without Saga audio");
    TownsMix(0, (Uint8 *)output, sizeof(output));
    position = channels[index].position;
    PlaySnowStaticSound();
    Require(channels[index].position == position, "static refresh does not restart its sample");
    SdlStopTownsEffects();
    pFireworkShape = 0; /* Exercise sound events without raster assets. */
    aFireworksList[0].frame = 0;
    aFireworksList[0].x = 160;
    TheEndFireWorks(0, 1);
    Require(PlayingEffect(aFireworksList[0].soundHandle) != 0 &&
            PlayingEffect(aFireworksList[0].soundHandle)->effect == 37,
            "fireworks start the Towns boom effect");
    shot = SdlPlayTownsEffect(8, 127, 64, -1, 0);
    aFireworksList[0].frame = 7;
    TheEndFireWorks(0, 1);
    Require(!PlayingEffect(aFireworksList[0].soundHandle) && PlayingEffect(shot),
            "firework end stops only its own sound");
    SdlStopTownsEffects();
}

int main(int argc, char **argv)
{
    unsigned int bank, speaker, message, variant, played = 0, index;
    short output[4096 * 2];
    CheckPcm();
    CheckInactiveAudio();
    if (argc < 2) {
        puts("PASS: Towns PCM decoding, bounds, pan and looping.");
        return 0;
    }
    Require(chdir(argv[1]) == 0, "enter supplied Towns installation");
    Require(SdlInitializeTownsAudio(), "initialize real Towns audio");
    SDL_PauseAudioDevice(townsDevice, 1);
    nSfxVolumeSetting = 20;
    SdlServiceTownsAudio();
    CheckMusic();
    CheckGameEffects();
    for (index = 1; index < 39; index++) {
        Require(SdlPlayTownsEffect(index, 127, 64, -1, index == 12), effectNames[index]);
        Uint64 power = 0;
        unsigned int frame, sample;
        for (frame = 0; frame < 50; frame++) {
            TownsMix(0, (Uint8 *)output, sizeof(output));
            for (sample = 0; sample < SDL_arraysize(output); sample++)
                power += abs(output[sample]);
        }
        Require(power > 0, "effect produces audible PCM");
        SdlStopTownsEffects();
    }
    for (bank = 0; bank < 2; bank++) {
        nCampaignDataSet = bank ? 2 : 0;
        for (speaker = 0; speaker < 14; speaker++) {
            for (message = 0; message < 11; message++) {
                for (variant = 0; variant < 6; variant++) {
                    const char *name = radioNames[bank][speaker][message][variant];
                    if (!*name)
                        continue;
                    Require(SdlPlayTownsRadio(speaker, message, (short)variant - 1), name);
                    Require(SdlTownsRadioPlaying(), "radio lifetime starts");
                    SdlStopTownsRadio();
                    Require(!SdlTownsRadioPlaying(), "radio stops");
                    played++;
                }
            }
        }
    }
    Require(played == 398, "all original radio table entries exercised");
    SdlShutdownTownsAudio();
    SDL_Quit();
    printf("PASS: game effect routing, loops/stops, 38 effects, %u radio entries and PCM checks.\n", played);
    return 0;
}
