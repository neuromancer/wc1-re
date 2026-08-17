#include "wc1.h"

#include "video_internal.h"

#include <stdio.h>
#include <string.h>

static int Wc1SdlParsePortArguments(int *argumentCount, char **arguments,
                                    int *useEnhancedRenderer)
{
    char *argument;
    int argumentIndex;
    int outputArgumentIndex;

    outputArgumentIndex = 1;
    *useEnhancedRenderer = 0;
    argumentIndex = 1;
    while (argumentIndex < *argumentCount) {
        argument = arguments[argumentIndex];
        if (strcmp(argument, "--enhanced") == 0) {
            *useEnhancedRenderer = 1;
        } else if (strcmp(argument, "--joystick-debug") == 0) {
            Wc1SdlEnableJoystickDebug();
        } else if (strcmp(argument, "--joystick-rumble") == 0) {
            Wc1SdlEnableJoystickRumble();
        } else if (strncmp(argument, "--joystick-mode=", 16) == 0) {
            if (!Wc1SdlSetJoystickMode(argument + 16)) {
                fprintf(stderr, "Unknown joystick mode: %s\n",
                        argument + 16);
                return 0;
            }
        } else if (strncmp(argument, "--joystick-axes=", 16) == 0) {
            if (!Wc1SdlSetJoystickAxesMode(argument + 16)) {
                fprintf(stderr, "Unknown joystick axes mode: %s\n",
                        argument + 16);
                return 0;
            }
        } else {
            arguments[outputArgumentIndex++] = argument;
        }
        argumentIndex++;
    }
    *argumentCount = outputArgumentIndex;
    arguments[outputArgumentIndex] = 0;
    return 1;
}

static void Wc1SdlApplyLegacyArguments(int argumentCount, char **arguments)
{
    const char *argument;
    char command;
    int argumentIndex;

    argumentIndex = 1;
    while (argumentIndex < argumentCount) {
        argument = arguments[argumentIndex];
        if (strcmp(argument, "$#SAGA.EXE") == 0)
            g_bShowKilrathiSagaCredits_0046506c = 1;
        command = argument[0] == '-' ? argument[1] : argument[0];
        switch (command) {
        case 'b':
            *(unsigned char *)&DAT_0046a000 = 0;
            break;
        case 'c':
            g_bCockpitEnabled_0046507c = 0;
            break;
        case 'f':
            g_bShowFrameRate_00465070 = 1;
            break;
        case 'k':
            *(unsigned char *)&DAT_00469ffc = 0;
            break;
        case 'q':
            g_bDirectDrawModeCascadeEnabled_00465074 = 0;
            break;
        default:
            break;
        }
        argumentIndex++;
    }
}

static int Wc1SdlRunRuntimeChecks(void)
{
    g_aShipWeapons_0059cab0[1][0] = 2;
    remove_weapon(1, 0);
    if (g_aShipWeapons_0059cab0[1][0] != 1)
        return 1;

    g_acShipTarget_0059ce60[0] = -1;
    g_cTargetDisplayObject_0046c06c = -1;
    g_nRenderedSpaceFrame_0059d61a = 1;
    DrawTargetRangeReadout();

    g_aeObjectClass_0059d100[1] = OBJECT_CLASS_SHIP;
    g_acObjectOwner_0059ce20[1] = -1;
    g_nYourWingman_0046c04c = -1;
    send_appropriate_message(1, 0);

    g_acShipTarget_0059ce60[0] = -1;
    g_aeShipManeuver_0059dcb0[0] = MANEUVER_NONE;
    perform_maneuver(0);

    g_pSnowStaticSound_00476550 = ix_system_new_sound(0);
    if (g_pSnowStaticSound_00476550 == 0)
        return 1;
    stop_all_sounds();
    if (g_pSnowStaticSound_00476550 != 0)
        return 1;

    g_cCockpitView_0059dab0 = 4;
    vdu_polygon(2, 50);
    if (DAT_005a6be0.left != -99)
        return 1;

    g_anShipMissionShip_0059d4b0[1] = -1;
    g_aeShipObjective_0059d200[1] = OBJECTIVE_HOME_BASE;
    strike_mission(1);
    if (g_aeShipMissionType_0059c3f0[1] != MISSION_TYPE_ROUT)
        return 1;

    g_nCommPortraitIndex_0046afd0 = -1;
    FreeCommDisplayResources();
    return 0;
}

int main(int argumentCount, char **arguments)
{
    SDL_Window *window;
    Uint32 windowFlags;
    int checkOnly;
    int gameResult;
    int useEnhancedRenderer;
    int usingDosData;

    if (!Wc1SdlParsePortArguments(&argumentCount, arguments,
                                   &useEnhancedRenderer))
        return 1;
    if (useEnhancedRenderer) {
        Wc1SdlSetVideoBackend(
            WC1_SDL_VIDEO_BACKEND_GL_SHARP_BILINEAR);
    }
    checkOnly = argumentCount == 2 && strcmp(arguments[1], "--check") == 0;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
                 SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    windowFlags = SDL_WINDOW_RESIZABLE;
    if (!Wc1SdlConfigureVideoWindow(&windowFlags)) {
        fprintf(stderr, "SDL video configuration failed: %s\n",
                SDL_GetError());
        SDL_Quit();
        return 1;
    }
    if (checkOnly)
        windowFlags |= SDL_WINDOW_HIDDEN;
    window =
        SDL_CreateWindow("Wing Commander SDL2 port", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 960, 600, windowFlags);
    if (window == 0) {
        fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    if (useEnhancedRenderer)
        fprintf(stderr,
                "Experimental enhanced rendering enabled "
                "(GL sharp bilinear).\n");

    DIBinstall((HWND)window);
    DAT_005a89a0 = (HWND)window;
    Wc1SdlStartEventPump();
    if (checkOnly) {
        gameResult = Wc1SdlRunRuntimeChecks();
    } else {
        CheckLauncherAndConfig();
        usingDosData = Wc1SdlUsingDosData();
        if (usingDosData) {
            /* DOS audio drivers cannot be used by the native SDL2 host. */
            g_bIxAudioEnabled_00465058 = 0;
        }
        if (usingDosData || useEnhancedRenderer) {
            if (!Wc1SdlInitializeOriginFxAudio(usingDosData)) {
                if (usingDosData) {
                    fprintf(stderr,
                            "DOS audio is unavailable.\n");
                } else {
                    fprintf(stderr,
                            "OriginFX intro music is unavailable.\n");
                }
            }
        }
        Wc1SdlApplyLegacyArguments(argumentCount, arguments);
        MonoDebug_install();
        InitializeAudioSystem((HWND)window);
        InitializeAudioStreamer((HWND)window);
        srand((unsigned int)time(0));
        InitGameClockEpoch();
        CreateDebugOverlayConsole(0, (HWND)window, 60, 20);
        DAT_005a8a44 = (unsigned int)time(0);
        DAT_0059ab2c = 0;
        SDL_SetWindowMouseGrab(window, SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
        gameResult = Wc1GameMain((short)(argumentCount - 1), arguments);
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        DestroyGlobalDebugOverlayConsole();
        if ((g_dwStreamerState_00597cd0 & 1) != 0)
            ix_streamer_destroy();
        ServiceAudioStream();
        Wc1SdlShutdownOriginFxAudio();
    }

    DIBunInstall();
    Wc1SdlShutdownJoysticks();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return gameResult;
}
