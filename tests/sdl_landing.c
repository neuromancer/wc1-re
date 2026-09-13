#include "wc1.h"

#include <stdio.h>
#include <string.h>

int main(int argumentCount, char **arguments)
{
    SDL_Window *window;
    short ship;
    signed char damage;

    if (argumentCount != 2 || SdlChangeDirectory(arguments[1]) != 0) {
        fprintf(stderr, "Usage: %s <installed game directory>\n", arguments[0]);
        return 1;
    }
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow("Landing test", 0, 0, 320, 200,
                              SDL_WINDOW_HIDDEN);
    if (window == 0) {
        fprintf(stderr, "SDL window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    DIBinstall((HWND)window);
    hMainWindow = (HWND)window;
    SdlStartEventPump();
    InitGameClockEpoch();
    bExpandedMemoryAvailable = 1;
    nMusicPlaybackMode = 0;
    bIxAudioEnabled = 0;
    nWaitForMusicEnabled = 0;
    if (SdlChangeDirectory("gamedat") != 0) {
        fprintf(stderr, "The game directory must contain GAMEDAT\n");
        return 1;
    }
    LoadInstallDat();
    SdlChangeDirectory("..");
    ResetCampaignData();
    SetCinematicFrameTiming();
    nVideoMode = 0x13;
    LoadOriginFxDrivers();

    /* Exercise the actual animation and text output. Damage levels 1 and 3
     * used to read across adjacent 64-bit pointers; level 2 chose entry 1. */
    for (ship = OBJECT_TYPE_HORNET; ship <= OBJECT_TYPE_RAPTOR; ship++) {
        stCampaignState.playerShipType = (enum ObjectType)ship;
        for (damage = 0; damage < 4; damage++) {
            fprintf(stderr, "Landing ship=%d damage=%d\n", ship, damage);
            if (landing(damage) != 0 ||
                strcmp(szDefaultTextBuffer,
                       apszLandingDamageComments[damage]) != 0 ||
                stSceneBuffer.pixels != 0 ||
                stScreen.top != 0 || stScreen.bottom != 199) {
                fprintf(stderr, "Landing did not complete correctly\n");
                return 1;
            }
        }
    }

    EMShutDown();
    DIBunInstall();
    SDL_DestroyWindow(window);
    SDL_Quit();
    fprintf(stderr, "All 16 landing animations passed\n");
    return 0;
}
