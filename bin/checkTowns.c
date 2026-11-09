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

static void CheckJapaneseRoomLabels(TextContext *context)
{
    static const char *const labels[] = {
        "Talk to SHOTGLASS.", "Check pilot scores", "Enter barracks",
        "Fly training mission", "Check poster out", "Save this campaign  ",
        "Mission Hangar", "Return to the Bar", "Quit Wing Commander",
        "View your medals", "Autosave (empty)", "Reserved for autosave"
    };
    unsigned char expected[64000];
    unsigned char *pixels = context->viewport->pixels;
    char text[256], pilot[64], *savedGame = 0;
    unsigned int index;
    context->text = text;
    context->alignment = 2;
    for (index = 0; index < SDL_arraysize(labels); index++) {
        const char *translated = C_("font0", labels[index]);
        Require(SdlMeasureTownsText(translated) > 0, "room label has Japanese translation");
        memset(pixels, 0, 64000);
        DrawTextAt(context, 0, 188, translated, 2);
        memcpy(expected, pixels, sizeof(expected));
        memset(pixels, 0, 64000);
        DrawRoomMenuLabel(context, labels[index]);
        Require(!memcmp(expected, pixels, sizeof(expected)) && context->cursorY == 188,
                "actual hotspot label renders Japanese on the bottom line");
    }
    snprintf(pilot, sizeof(pilot), C_("font0", "Talk to %s."), "WWWWWWWWWWWWW");
    DrawRoomMenuLabel(context, pilot);
    Require(SdlMeasureTownsText(pilot) > 0 && strstr(pilot, "WWWWWWWWWWWWW"),
            "dynamic pilot labels retain callsign and Japanese action");
    SetAwakenBarracksMenuLabel(&savedGame, 0, 0, "WWWWWWWWWWWWWWWWWWW");
    Require(savedGame && SdlMeasureTownsText(savedGame) > 0, "saved-game hover label translated");
    DrawRoomMenuLabel(context, savedGame);
    FreeBarracksMenuLabel(&savedGame);
    snprintf(text, sizeof(text), C_("font0", "Awaken %s? (Y/N)"), "WWWWWWWWWWWWWWWWWWW");
    Require(strlen(text) < 52, "Japanese load confirmation fits original buffer");
    snprintf(text, sizeof(text), C_("font0", "Replace %s? (Y/N)"), "WWWWWWWWWWWWWWWWWWW");
    Require(strlen(text) < 52, "Japanese replace confirmation fits original buffer");
    Require(strlen(C_("font0", "Quit Wing Commander? (Y/N)")) < 52,
            "Japanese quit confirmation fits original buffer");
    context->text = 0;
    context->alignment = 0;
    puts("PASS: Japanese room hotspots, pilot/save labels and confirmation bounds through PO lookup.");
}

static void CheckJapaneseText(void)
{
    unsigned char pixels[64000], expected[64000], *font, *archive, *packet;
    unsigned short rows[200];
    Viewport viewport = {pixels, rows, 0, 0, 319, 199, 0};
    TextContext context = {0};
    PilotRecord pilot = {"WWWWWWWWWWWWW", "WWWWWWWWWWWWW", 0, 0, 0, 0, 0};
    CampaignDate date = {100, 2654};
    unsigned int index, file, section, count = 0, radioCount = 0;
    size_t size;
    char name[64];
    const char *japanese = "A\x93\xfa\x96\x7b\x8c\xea" "B"; /* A + Japanese + B */
    Require(SdlSetTownsLanguage(1), "enable Japanese from the user's Towns disc");
    Require(SdlSetLanguage(2), "load external Japanese UI catalog");
    font = PacketLoad("GAMEDAT/FONTS.FNT", 0, 0, 0, 0);
    Require(font != 0, "retain original Latin font");
    for (index = 0; index < 200; index++)
        rows[index] = index * 320;
    context.viewport = &viewport;
    context.font = font;
    context.colour = 1;
    context.backgroundColour = 0xff;
    CheckJapaneseRoomLabels(&context);
    SetTextContext(&context);
    Require(MeasureTextPixelWidthClamped(japanese) == 32, "mixed half/full-width metrics");
    memset(pixels, 0, sizeof(pixels));
    DrawTextAt(&context, 0, 0, (char *)japanese, 2);
    Require(context.cursorX == 176 && context.cursorY == 0, "center Japanese using glyph widths");
    Require(memchr(pixels, 1, sizeof(pixels)) != 0, "Japanese glyphs draw pixels");
    SetTextCursor(0, 0);
    DrawFormattedText("%s", "\x93\xfa\x96\x7b\x8c\xea");
    Require(context.cursorX == 24, "formatted character stream preserves Shift-JIS pairs");
    viewport.left = viewport.top = 2;
    viewport.right = viewport.bottom = 5;
    memset(pixels, 0x55, sizeof(pixels));
    DrawTextAt(&context, -2, -2, (char *)japanese, 0);
    for (index = 0; index < sizeof(pixels); index++) {
        int x = index % 320, y = index / 320;
        if (x < 2 || x > 5 || y < 2 || y > 5)
            Require(pixels[index] == 0x55, "clip all four edges before writing pixels");
    }
    viewport.left = viewport.top = 0;
    viewport.right = 319;
    viewport.bottom = 199;
    DrawTextAt(&context, 316, 198, "\x93\n\x93", 0);
    /* A pair crossing the old HUD byte limit must remain a single glyph. */
    memset(name, 'A', 46);
    strcpy(name + 46, japanese + 1);
    memset(pixels, 0, sizeof(pixels));
    viewport.left = 18;
    viewport.top = 14;
    viewport.right = 301;
    viewport.bottom = 74;
    DrawTextAt(&context, 18, 14, name, 2);
    memcpy(expected, pixels, sizeof(pixels));
    viewport.left = viewport.top = 0;
    viewport.right = 319;
    viewport.bottom = 199;
    memset(pixels, 0, sizeof(pixels));
    stHudMessageTextContext = context;
    cCurrentCockpitView = 0;
    bCockpitlessView = 0;
    print_message_text(name, 1);
    Require(!memcmp(expected, pixels, sizeof(pixels)), "HUD does not split Shift-JIS pairs");
    stCampaignState.currentPilot = &pilot;
    pCurrentCampaignDate = &date;
    pElapsedCampaignDatePtr = &date;
    for (index = 0; index < 8; index++)
        apWingmanPilotsList[index] = &pilot;
    memcpy(abSeriesAuxData, "Gimle", 6);
    Require(!strcmp(AddPCName("$Y/$D"), "2654/100"), "Japanese year/day substitutions");
    Require((unsigned char)*AddPCName("$R") >= 0x80, "rank read from original executable");
    for (file = 0; file < 12; file++) {
        if (file < 3)
            SDL_snprintf(name, sizeof(name), "GAMEDAT/BRIEFING.00%u", file);
        else
            SDL_snprintf(name, sizeof(name), "GAMEDAT/MIDGAME.V0%u", file - 3);
        archive = SDL_LoadFile(SdlTownsFilename(name), &size);
        Require(archive != 0, "resolve Japanese dialogue resource");
        unsigned int sections = (Towns32(archive + 4) & 0xffffff) / 4 - 1;
        for (section = 0; section < sections; section++) {
            unsigned int start = Towns32(archive + 4 + section * 4) & 0xffffff;
            unsigned int end = section + 1 < sections ?
                Towns32(archive + 8 + section * 4) & 0xffffff : size;
            unsigned int pair, pairs = file < 3 ? (section == 0 ? 7 : section < 3 ? 1 : 5) : 1;
            if (start == end || (file >= 3 && section != 5 && section != 6))
                continue;
            packet = PacketLoad(name, section, 0, 0, 0);
            Require(packet && !memcmp(packet, archive + start, end - start),
                    "game packet loader selects Japanese bytes");
            for (pair = 0; pair < pairs; pair++) {
                unsigned int scene = Towns32(packet + pair * 8);
                unsigned int text = Towns32(packet + pair * 8 + 4);
                for (index = scene; index + 13 <= text; index += 13) {
                    if ((signed char)packet[index] == -2)
                        continue;
                    const char *line = (const char *)packet + text +
                        packet[index + 7] + 256 * packet[index + 8];
                    DrawTextAt(&context, 0, 160, AddPCName(line), 2);
                    Require(context.cursorY + 8 <= 200, "dialogue fits subtitle region");
                    count++;
                }
            }
            ReleasePacketHandle(packet);
        }
        SDL_free(archive);
    }
    for (file = 0; file < 2; file++) {
        nCampaignDataSet = file ? 2 : 0;
        short fd = OpenDataFileOrDie("GAMEDAT/COMMUNIC.DAT");
        archive = SDL_LoadFile(file ? "GAMEDAT/COMMSM2.JAT" : "GAMEDAT/COMMUNIC.JAT", &size);
        Require(fd >= 0 && archive && size == 14 * 11 * 80, "select campaign Japanese radio bank");
        for (index = 0; index < size; index += 80) {
            char line[80];
            Require(ReadDataFileAtOffset(fd, index, 80, line) && !memcmp(line, archive + index, 80),
                    "game reads Japanese radio records");
            for (pilot.rank = 0; pilot.rank < 5; pilot.rank++) {
                char *expanded = ExpandCommMessageTokens(line);
                Require(strlen(expanded) < sizeof(szHudMessageBuf), "expanded radio fits HUD storage");
                DrawTextAt(&context, 0, 0, expanded, 2);
                radioCount++;
            }
        }
        CloseDataFile(fd);
        SDL_free(archive);
    }
    ReleasePacketHandle(font);
    Require(SdlSetLanguage(0), "disable UI translation");
    Require(SdlSetTownsLanguage(0), "disable Japanese");
    Require(SdlMeasureTownsText(japanese) == -1 &&
            !strcmp(SdlTownsFilename("BRIEFING.000"), "BRIEFING.000"),
            "English rendering and resources unchanged when disabled");
    stCampaignState.currentPilot = 0;
    pCurrentCampaignDate = 0;
    pElapsedCampaignDatePtr = 0;
    pCurTextContext = 0;
    memset(&stHudMessageTextContext, 0, sizeof(stHudMessageTextContext));
    memset(apWingmanPilotsList, 0, 8 * sizeof(apWingmanPilotsList[0]));
    nCampaignDataSet = 0;
    printf("PASS: Japanese metrics, clipping, tokens, %u dialogue lines and %u radio/rank combinations.\n",
           count, radioCount);
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
    CheckJapaneseText();
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
