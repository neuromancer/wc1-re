#include "wc1.h"

#include "video_internal.h"
#include "slint/launcher_api.h"

#include <stdio.h>
#include <string.h>

static const char *const g_apszSdlLauncherJoystickModes[] = {
    "original", "4button-2axis", "4button-4axis", "custom", "steam-deck"
};

static const char *const g_apszSdlLauncherMusic[] = {
    "default", "adlib", "mt32"
};

static const char *const g_apszSdlLauncherSound[] = {
    "default", "adlib", "mt32"
};

static const char *const g_apszSdlLauncherJoystickAxes[] = {
    "twin-stick-roll", "twin-stick-yaw", "hotas-yaw", "hotas-roll",
    "linear-throttle", "rudder-yaw", "rudder-roll"
};

static int SdlFindLauncherChoice(const char *name,
                                 const char *const *choices, size_t choiceCount)
{
    size_t choice;

    for (choice = 0; choice < choiceCount; choice++) {
        if (strcmp(name, choices[choice]) == 0)
            return choice;
    }
    return -1;
}

/* The game later changes into GAMEDAT, so keep the ROM folder absolute. */
static int SdlSetMt32RomArgument(const char *directory, SdlLauncherOptions *options)
{
    char current[PATH_MAX];

    if (directory[0] == '\0')
        return 0;
    if (directory[0] == '/' || directory[0] == '\\' ||
        (directory[0] != '\0' && directory[1] == ':'))
        return SDL_strlcpy(options->mt32Roms, directory, sizeof(options->mt32Roms)) <
            sizeof(options->mt32Roms);
    return GetCurrentDirectoryA(sizeof(current), current) &&
        SDL_snprintf(options->mt32Roms, sizeof(options->mt32Roms), "%s/%s", current,
                     directory) < (int)sizeof(options->mt32Roms);
}

static int SdlParsePortArguments(int *argumentCount, char **arguments,
                                  int *useLauncherGui,
                                  SdlLauncherOptions *options)
{
    char *argument;
    int argumentIndex;
    int outputArgumentIndex;

    outputArgumentIndex = 1;
    memset(options, 0, sizeof(*options));
    options->joystickDeadZone = SDL_JOYSTICK_DEAD_ZONE_ORIGINAL;
    options->soundDevice = SDL_LAUNCHER_SOUND_UNSET;
    options->flightFpsHalves = SDL_LAUNCHER_FLIGHT_FPS_NORMAL;
#ifdef WC1_STATIC_GUI
    *useLauncherGui = *argumentCount == 1;
#else
    *useLauncherGui = 0;
#endif
    argumentIndex = 1;
    while (argumentIndex < *argumentCount) {
        argument = arguments[argumentIndex];
        if (strcmp(argument, "--gui") == 0) {
            *useLauncherGui = 1;
        } else if (strcmp(argument, "--transfer-pilot") == 0) {
            options->transferPilot = 1;
        } else if (strcmp(argument, "--credits") == 0) {
            options->swcCredits = 1;
        } else if (strncmp(argument, "--campaign=", 11) == 0) {
            if (strcmp(argument + 11, "wc1") == 0)
                options->campaign = 0;
            else if (strcmp(argument + 11, "sm1") == 0)
                options->campaign = 1;
            else if (strcmp(argument + 11, "sm2") == 0)
                options->campaign = 2;
            else {
                fprintf(stderr, "Unknown campaign: %s (use wc1, sm1 or sm2).\n", argument + 11);
                return 0;
            }
        } else if (strncmp(argument, "--voices=", 9) == 0) {
            if (!argument[9] || SDL_strlcpy(options->voiceCatalog, argument + 9,
                                           sizeof(options->voiceCatalog)) >= sizeof(options->voiceCatalog)) {
                fprintf(stderr, "Invalid voice catalog path.\n");
                return 0;
            }
        } else if (strcmp(argument, "--fullscreen") == 0 ||
                   strcmp(argument, "--windowed") == 0) {
            options->fullscreen = strcmp(argument, "--fullscreen") == 0;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_FULLSCREEN;
        } else if (strcmp(argument, "--autosave") == 0 ||
                   strcmp(argument, "--no-autosave") == 0) {
            options->autosave = strcmp(argument, "--autosave") == 0;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_AUTOSAVE;
        } else if (strncmp(argument, "--language=", 11) == 0) {
            if (strcmp(argument + 11, "en") == 0)
                options->language = 0;
            else if (strcmp(argument + 11, "es") == 0)
                options->language = 1;
            else if (strcmp(argument + 11, "ja") == 0)
                options->language = 2;
            else if (strcmp(argument + 11, "fr") == 0)
                options->language = 3;
            else {
                fprintf(stderr, "Unknown language: %s (use en, es, ja or fr).\n", argument + 11);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_LANGUAGE;
        } else if (strcmp(argument, "--check") == 0) {
            fprintf(stderr, "The --check runtime mode has been removed.\n");
            return 0;
        } else if (strcmp(argument, "--enhanced") == 0) {
            options->enhancedRenderer = 1;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_ENHANCED;
        } else if (strcmp(argument, "--cheats") == 0) {
            options->cheatMode = 1;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_CHEATS;
        } else if (strcmp(argument, "--joystick-debug") == 0) {
            SdlEnableJoystickDebug();
        } else if (strcmp(argument, "--joystick-rumble") == 0) {
            options->joystickRumble = 1;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_RUMBLE;
        } else if (strncmp(argument, "--joystick-mode=", 16) == 0) {
            options->joystickMode = SdlFindLauncherChoice(
                argument + 16, g_apszSdlLauncherJoystickModes,
                SDL_arraysize(g_apszSdlLauncherJoystickModes));
            if (options->joystickMode < 0) {
                fprintf(stderr, "Unknown joystick mode: %s\n",
                        argument + 16);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_JOYSTICK_MODE;
        } else if (strncmp(argument, "--joystick-axes=", 16) == 0) {
            options->joystickAxes = SdlFindLauncherChoice(
                argument + 16, g_apszSdlLauncherJoystickAxes,
                SDL_arraysize(g_apszSdlLauncherJoystickAxes));
            if (options->joystickAxes < 0) {
                fprintf(stderr, "Unknown joystick axes mode: %s\n",
                        argument + 16);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_JOYSTICK_AXES;
        } else if (strncmp(argument, "--joystick-dead-zone=", 21) == 0) {
            char *end;
            long percent = strtol(argument + 21, &end, 10);

            if (end == argument + 21 || *end != '\0' || percent < 0 ||
                percent > SDL_JOYSTICK_DEAD_ZONE_MAXIMUM) {
                fprintf(stderr, "Invalid joystick dead zone: %s (use 0 to %d percent; "
                        "the original is %d).\n", argument + 21,
                        SDL_JOYSTICK_DEAD_ZONE_MAXIMUM, SDL_JOYSTICK_DEAD_ZONE_ORIGINAL);
                return 0;
            }
            options->joystickDeadZone = (int)percent;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_JOYSTICK_DEAD_ZONE;
        } else if (strncmp(argument, "--music=", 8) == 0) {
            options->musicDevice = SdlFindLauncherChoice(
                argument + 8, g_apszSdlLauncherMusic,
                SDL_arraysize(g_apszSdlLauncherMusic));
            if (options->musicDevice < 0) {
                fprintf(stderr, "Unknown music: %s (use default, adlib or mt32)\n",
                        argument + 8);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_MUSIC;
        } else if (strncmp(argument, "--sound=", 8) == 0) {
            options->soundDevice = SdlFindLauncherChoice(
                argument + 8, g_apszSdlLauncherSound,
                SDL_arraysize(g_apszSdlLauncherSound));
            if (options->soundDevice < 0) {
                fprintf(stderr, "Unknown sound: %s (use default, adlib or mt32)\n",
                        argument + 8);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_SOUND;
        } else if (strncmp(argument, "--flight-fps=", 13) == 0) {
            options->flightFpsHalves = SdlParseFlightFps(argument + 13);
            if (options->flightFpsHalves < 0) {
                fprintf(stderr, "Invalid spaceflight frame rate: %s "
                        "(8 to 32, in half steps)\n", argument + 13);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_FLIGHT_FPS;
        } else if (strncmp(argument, "--mt32-roms=", 12) == 0) {
            if (!SdlSetMt32RomArgument(argument + 12, options)) {
                fprintf(stderr, "Invalid MT-32 ROM folder: %s\n", argument + 12);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_MT32_ROMS;
            /* A ROM folder alone asks for the MT-32. */
            if (!(options->explicitOptions & SDL_LAUNCHER_EXPLICIT_MUSIC)) {
                options->musicDevice = SDL_LAUNCHER_MUSIC_MT32;
                options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_MUSIC;
            }
        } else if (strncmp(argument, "--mt32-model=", 13) == 0) {
            if (SDL_strlcpy(options->mt32Model, argument + 13, sizeof(options->mt32Model)) >=
                sizeof(options->mt32Model)) {
                fprintf(stderr, "Invalid MT-32 model: %s\n", argument + 13);
                return 0;
            }
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_MT32_MODEL;
        } else if (strcmp(argument, "--mt32-check") == 0) {
            char description[PATH_MAX + 256];
            int usable = SdlDescribeMt32Roms(options->mt32Roms, options->mt32Model,
                                             description, sizeof(description));

            printf("%s\n", description);
            exit(usable ? 0 : 1);
        } else if (strcmp(argument, "--ega") == 0) {
            options->egaDither = 1;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_EGA;
        } else if (strcmp(argument, "--hires-hud") == 0) {
            options->hiResHudFonts = 1;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_HIRES_HUD;
        } else if (strcmp(argument, "--interpolate") == 0) {
            options->frameInterpolation = 1;
            options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_INTERPOLATION;
        } else {
            if ((argument[0] == '-' ? argument[1] : argument[0]) == 'c') {
                options->cockpitlessView = 1;
                options->explicitOptions |= SDL_LAUNCHER_EXPLICIT_COCKPITLESS;
            }
            arguments[outputArgumentIndex++] = argument;
        }
        argumentIndex++;
    }
    *argumentCount = outputArgumentIndex;
    arguments[outputArgumentIndex] = 0;
    return 1;
}

/* WC1 data, DOS or Kilrathi Saga, always has GAMEDAT/MODULE.000. */
static int SdlHasWc1GameData(void)
{
    char path[PATH_MAX];
    SDL_RWops *file;
    Sint64 size;

    if (!SdlResolvePath("GAMEDAT/MODULE.000", path, sizeof(path)))
        return 0;
    file = SDL_RWFromFile(path, "rb");
    if (file == NULL) {
        SDL_ClearError();
        return 0;
    }
    size = SDL_RWsize(file);
    SDL_RWclose(file);
    return size > 0;
}

int SdlHasSwcResources(const char *directory)
{
    char path[PATH_MAX];
    int found = SdlFindSwcResources(directory, path, sizeof(path));

    SDL_ClearError();
    return found;
}

static int SdlOpenLauncherGui(SdlLauncherOptions *options)
{
#ifdef WC1_STATIC_GUI
    return SdlRunLauncherGui(options);
#else
    (void)options;
    fprintf(stderr,
            "The graphical launcher is not included in this build.\n"
            "Build it with 'make modern-gui'.\n");
    return SDL_LAUNCHER_UNAVAILABLE;
#endif
}

static int SdlApplyLauncherOptions(const SdlLauncherOptions *options)
{
    char directory[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    if (options->campaign < 0 || options->campaign > 2 ||
        (options->swcDemo && options->campaign != 0) ||
        (options->transferPilot && !options->campaign)) {
        fprintf(stderr, "Invalid campaign for the selected game.\n");
        return 0;
    }
    if (options->swcCredits && !options->swcDemo) {
        fprintf(stderr, "Credits are available only for Super Wing Commander.\n");
        return 0;
    }
    nLaunchCampaign = options->campaign;
    nPilotTransferState = options->transferPilot != 0;
    if (options->joystickMode < SDL_LAUNCHER_JOYSTICK_ORIGINAL ||
        options->joystickMode > SDL_LAUNCHER_JOYSTICK_STEAM_DECK ||
        options->joystickAxes < SDL_LAUNCHER_AXES_TWIN_STICK_ROLL ||
        options->joystickAxes > SDL_LAUNCHER_AXES_RUDDER_ROLL) {
        fprintf(stderr, "Invalid launcher joystick options.\n");
        return 0;
    }
    SdlSetJoystickMode(g_apszSdlLauncherJoystickModes[options->joystickMode]);
    if (!SdlSetJoystickSource(options->joystickSource)) {
        fprintf(stderr, "The GUI returned an invalid joystick source.\n");
        return 0;
    }
    SdlSetJoystickAxesMode(g_apszSdlLauncherJoystickAxes[options->joystickAxes]);
    SdlSetPadBindings(options->padBindings);
    if (!SdlSetJoystickDeadZone(options->joystickDeadZone)) {
        fprintf(stderr, "Invalid joystick dead zone.\n");
        return 0;
    }
    SdlSetKeyBindings(options->keyBindings);
    SdlConfigureAutosave(options->autosave);
    if (options->musicDevice < SDL_LAUNCHER_MUSIC_DEFAULT ||
        options->musicDevice > SDL_LAUNCHER_MUSIC_MT32) {
        fprintf(stderr, "Invalid music selection.\n");
        return 0;
    }
    SdlSetMusicDevice(options->musicDevice);
    if (options->soundDevice < SDL_LAUNCHER_SOUND_DEFAULT ||
        options->soundDevice > SDL_LAUNCHER_SOUND_MT32) {
        fprintf(stderr, "Invalid sound selection.\n");
        return 0;
    }
    SdlSetSoundDevice(options->soundDevice);
    if (options->flightFpsHalves < SDL_LAUNCHER_FLIGHT_FPS_MINIMUM ||
        options->flightFpsHalves > SDL_LAUNCHER_FLIGHT_FPS_MAXIMUM) {
        fprintf(stderr, "Invalid spaceflight frame rate.\n");
        return 0;
    }
    fSpaceFlightFrameRate = options->flightFpsHalves / 2.0f;
#ifdef WC1_STATIC_GUI
    /* A rate set in flight is kept for the launcher's Speed choice. */
    SdlSetFlightFpsSaver(SdlSaveLauncherFlightFps);
#endif
    SdlSetMt32Roms(options->mt32Roms, options->mt32Model);
    if (*options->voiceCatalog && !options->swcDemo && !SdlLoadVoiceCatalog(options->voiceCatalog)) {
        fprintf(stderr, "Cannot load voices: %s\n", SDL_GetError());
        return 0;
    }
    if (!*options->voiceCatalog && !options->swcDemo) {
        if (!GetCurrentDirectoryA(sizeof(directory), directory) || !SdlLoadInstalledVoices(directory)) {
            fprintf(stderr, "Cannot load voices: %s\n", SDL_GetError());
            return 0;
        }
    }
    /* A player's German fan translation of the DOS WC.EXE brings its own UI
     * text; the selectable catalogs translate the English data. */
    if (!options->swcDemo && !SdlUsingTownsData() &&
        SdlResolvePath("WC.EXE", directory, sizeof(directory)) &&
        SdlLoadGermanText(directory) > 0)
        fprintf(stderr, "Using the German text in WC.EXE%s.\n",
                options->language == 1 || options->language == 3 ? " instead of the selected language" : "");
    if (!SdlSetTownsLanguage(options->language == 2) ||
        !SdlSetLanguage(options->language != 2 && (options->swcDemo || SdlUsingTownsData() ||
                                                   SdlUsingGermanText()) ? 0 : options->language)) {
        fprintf(stderr, "Cannot load game language: %s\n", SDL_GetError());
        return 0;
    }
    /* Apply these only after the dialog so checked command-line defaults can
     * be turned off before the video and input systems are initialized. */
    if (options->joystickRumble)
        SdlEnableJoystickRumble();
    if (options->egaDither && !options->swcDemo)
        SdlEnableEgaDither();
    SdlSetHiResHudFonts(options->hiResHudFonts && !options->swcDemo);
    SdlSetFrameInterpolation(options->frameInterpolation && !options->swcDemo);
    if (options->cheatMode) {
        /* CheckLauncherAndConfig (0x401d10) applies this same combination
         * for Saga's Cheater registry setting. Both SDL data paths share it. */
        nOriginDevUnlock = 1;
        bPlayerVulnerable = 0;
        bPlayerCollisionResponse = 0;
    }
    return 1;
}

/* SdlCalculateOutputViewport presents a 4:3 picture, so the window opens at
 * that shape: three times the 320x240 frame, or twice where the desktop is
 * too short for that and a title bar. */
static void SdlGetInitialWindowSize(int *width, int *height)
{
    SDL_Rect usable;
    int scale;

    scale = 3;
    if (SDL_GetDisplayUsableBounds(0, &usable) == 0 &&
        usable.h < 240 * scale + 64)
        scale = 2;
    *width = 320 * scale;
    *height = *width * SDL_PORT_DISPLAY_ASPECT_HEIGHT /
        SDL_PORT_DISPLAY_ASPECT_WIDTH;
}

static void SdlApplyLegacyArguments(int argumentCount, char **arguments)
{
    const char *argument;
    char command;
    int argumentIndex;

    argumentIndex = 1;
    while (argumentIndex < argumentCount) {
        argument = arguments[argumentIndex];
        if (strcmp(argument, "$#SAGA.EXE") == 0)
            bShowKilrathiSagaCredits = 1;
        command = argument[0] == '-' ? argument[1] : argument[0];
        switch (command) {
        case 'b':
            *(unsigned char *)&bPlayerCollisionResponse = 0;
            break;
        case 'c':
            bCockpitEnabled = 0;
            break;
        case 'f':
            bShowFrameRate = 1;
            break;
        case 'k':
            *(unsigned char *)&bPlayerVulnerable = 0;
            break;
        case 'q':
            bDirectDrawModeCascadeEnabled = 0;
            break;
        default:
            break;
        }
        argumentIndex++;
    }
}

int main(int argumentCount, char **arguments)
{
    SDL_Window *window;
    Uint32 windowFlags;
    int windowWidth;
    int windowHeight;
    int gameResult;
    int launcherResult;
    int useLauncherGui;
    int useEnhancedRenderer;
    int usingDosData;
    char swcPath[PATH_MAX];
    SdlLauncherOptions launcherOptions;

    if (!SdlParsePortArguments(&argumentCount, arguments,
                               &useLauncherGui, &launcherOptions))
        return 1;
    if (useLauncherGui) {
        launcherResult = SdlOpenLauncherGui(&launcherOptions);
        if (launcherResult == SDL_LAUNCHER_CANCELLED)
            return 0;
        if (launcherResult != SDL_LAUNCHER_ACCEPTED)
            return 1;
    } else {
        /* Like the launcher's check, the working directory selects the SWC
         * demo when it holds CMFs/Data.CMF and no WC1 game data. */
        launcherOptions.swcDemo = SdlFindSwcMissionData(swcPath, sizeof(swcPath)) &&
                                  !SdlHasWc1GameData();
    }
    SdlResolveLauncherSound(&launcherOptions);
    if (!SdlApplyLauncherOptions(&launcherOptions))
        return 1;
    useEnhancedRenderer = launcherOptions.enhancedRenderer;
    if (useEnhancedRenderer)
        SdlSetVideoBackend(SDL_PORT_VIDEO_BACKEND_GL_SHARP_BILINEAR);
    if (launcherOptions.swcDemo) {
        if (!SdlFindSwcMissionData(swcPath, sizeof(swcPath))) {
            fprintf(stderr, "SWC needs CMFs/Data.CMF.\n");
            return 1;
        }
        gameResult = SdlRunSwcMission(swcPath, launcherOptions.cockpitlessView,
                                     launcherOptions.fullscreen, launcherOptions.swcCredits);
        if (gameResult != 0) {
            fprintf(stderr, "Cannot start SWC: %s\n", SDL_GetError());
            if (useLauncherGui)
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Super Wing Commander",
                                         SDL_GetError(), NULL);
        }
        return gameResult != 0;
    }
    /* The build defines SDL_MAIN_HANDLED, so tell SDL the process is already
     * up rather than letting it rename main() out from under us. */
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
                 SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    windowFlags = SDL_WINDOW_RESIZABLE;
    if (launcherOptions.fullscreen)
        windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    if (!SdlConfigureVideoWindow(&windowFlags)) {
        fprintf(stderr, "SDL video configuration failed: %s\n",
                SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SdlGetInitialWindowSize(&windowWidth, &windowHeight);
    window =
        SDL_CreateWindow("Wing Commander SDL2 port", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight,
                         windowFlags);
    if (window == 0) {
        fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    /* Preserve Windows' no-IME game controls without disabling Japanese entry
     * process-wide. Other hosts change only for Japanese text fields. */
#ifndef _WIN32
    if (SdlUsingTownsJapanese())
#endif
    SDL_StopTextInput();
    if (useLauncherGui) {
        /* Hand focus from Slint's native event loop to the SDL game window. */
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
        SDL_PumpEvents();
    }
    if (useEnhancedRenderer)
        fprintf(stderr,
                "Experimental enhanced rendering enabled "
                "(GL sharp bilinear).\n");

    DIBinstall(window);
    hMainWindow = window;
    SdlStartEventPump();
    CheckLauncherAndConfig();
    usingDosData = SdlUsingDosData();
    if (usingDosData || SdlUsingTownsData()) {
        /* Only Saga supplies the ix audio streams. */
        bIxAudioEnabled = 0;
    }
    if (SdlUsingTownsData() && !SdlInitializeTownsAudio())
        fprintf(stderr, "FM Towns audio unavailable: %s\n", SDL_GetError());
    /* Kilrathi Saga data plays the intro with OriginFX in the enhanced
     * renderer, and its music or its sound effects with OriginFX on an
     * AdLib or MT-32 when the player chose them. */
    if (usingDosData || (!SdlUsingTownsData() &&
                         (useEnhancedRenderer ||
                          launcherOptions.musicDevice != SDL_LAUNCHER_MUSIC_DEFAULT ||
                          launcherOptions.soundDevice != SDL_LAUNCHER_SOUND_DEFAULT))) {
        if (!SdlInitializeOriginFxAudio(usingDosData)) {
            if (usingDosData) {
                fprintf(stderr,
                        "DOS audio is unavailable.\n");
            } else {
                fprintf(stderr, "%s is unavailable.\n",
                        launcherOptions.musicDevice != SDL_LAUNCHER_MUSIC_DEFAULT
                            ? "DOS music"
                            : launcherOptions.soundDevice != SDL_LAUNCHER_SOUND_DEFAULT
                            ? "DOS sound effects" : "OriginFX intro music");
            }
        }
    }
    SdlApplyLegacyArguments(argumentCount, arguments);
    if (useLauncherGui)
        bCockpitEnabled = !launcherOptions.cockpitlessView;
    MonoDebug_install();
    InitializeAudioSystem(window);
    InitializeAudioStreamer(window);
    srand(time(0));
    InitGameClockEpoch();
    CreateDebugOverlayConsole(0, window, 60, 20);
    nSessionStartTime = time(0);
    pCurrentEventManagerPump = 0;
    SDL_ShowCursor(SDL_DISABLE);
    gameResult = GameMain((argumentCount - 1), arguments);
    SdlSetMouseGrab(0);
    SDL_ShowCursor(SDL_ENABLE);
    DestroyGlobalDebugOverlayConsole();
    if ((dwIxStreamerStatus & 1) != 0)
        ix_streamer_destroy();
    ServiceAudioStream();
    SdlShutdownOriginFxAudio();
    SdlShutdownTownsAudio();
    SdlSetTownsLanguage(0);

    DIBunInstall();
    SdlShutdownJoysticks();
    SDL_DestroyWindow(window);
    SdlShutdownTranslations();
    SdlShutdownVoices();
    SDL_Quit();
    return gameResult;
}
