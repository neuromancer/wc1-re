#include "wc1.h"

#include <stdio.h>
#include <string.h>

int main(int argumentCount, char **arguments)
{
    SDL_Window *window;
    Uint32 windowFlags;
    int checkOnly;

    checkOnly = argumentCount == 2 && strcmp(arguments[1], "--check") == 0;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
                 SDL_INIT_JOYSTICK) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    windowFlags = SDL_WINDOW_RESIZABLE;
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

    DIBinstall((HWND)window);
    DAT_005a89a0 = (HWND)window;
    Wc1SdlStartEventPump();
    if (checkOnly) {
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
    } else {
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
        Wc1GameMain((short)(argumentCount - 1), arguments);
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        DestroyGlobalDebugOverlayConsole();
        if ((g_dwStreamerState_00597cd0 & 1) != 0)
            ix_streamer_destroy();
        ServiceAudioStream();
    }

    DIBunInstall();
    Wc1SdlShutdownJoysticks();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
