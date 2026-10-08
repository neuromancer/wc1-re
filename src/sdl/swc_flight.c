/* SDL2 first-mission host. Mission state, ship setup, flight dynamics, view
 * selection and navigation belong to the existing WC1 core. SWC object
 * definitions, combat, campaign progression and the complete HUD remain open. */
#include "wc1.h"
#include "swc.h"

#include <stdio.h>
#include <string.h>

typedef struct SwcFlightImages {
    SDL_Texture *views[37];
} SwcFlightImages;

static int swcFlightActive;
static int swcMouseAfterburner;
static int swcRightClickPending;
static Uint32 swcLastRightClick;

int SdlSwcFlightActive(void)
{
    return swcFlightActive;
}

SDL_Texture *SdlCreateSwcTexture(SDL_Renderer *renderer,
                                  const SwcBuffer *set, uint32_t index,
                                  const SDL_Color colors[256])
{
    SwcFrame frame;
    SDL_Surface *surface;
    SDL_Texture *texture = NULL;
    uint8_t *pixels;
    uint32_t *row;
    SDL_Color color;
    int x;
    int y;

    if (SwcGetFramePtr(set, index, &frame) != 0)
        return NULL;
    if (frame.width == 0 || frame.height == 0) {
        SDL_SetError("Empty SWC flight image");
        return NULL;
    }
    pixels = SDL_malloc((size_t)frame.width * frame.height);
    if (pixels == NULL) {
        SDL_OutOfMemory();
        return NULL;
    }
    if (SwcDecodeFrame(&frame, pixels, (size_t)frame.width * frame.height) != 0) {
        SDL_free(pixels);
        return NULL;
    }
    surface = SDL_CreateRGBSurfaceWithFormat(0, frame.width, frame.height,
                                             32, SDL_PIXELFORMAT_ARGB8888);
    if (surface != NULL) {
        for (y = 0; y < frame.height; y++) {
            row = (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch);
            for (x = 0; x < frame.width; x++) {
                color = colors[pixels[y * frame.width + x]];
                row[x] = pixels[y * frame.width + x] == 0 ? 0 :
                    UINT32_C(0xff000000) | (uint32_t)color.r << 16 |
                    (uint32_t)color.g << 8 | color.b;
            }
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture != NULL)
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surface);
    }
    SDL_free(pixels);
    return texture;
}

static int SwcLoadFlightImages(SDL_Renderer *renderer, const SwcCmf *cockpit,
                                const SwcCmf *space, const SDL_Color colors[256],
                                enum ObjectType type, SwcFlightImages *images)
{
    SwcBuffer set = {0};
    const SwcCmf *cmf;
    char chunkType[5];
    uint32_t count;
    uint32_t index;
    int result = -1;

    if (images->views[0] != NULL)
        return 0;
    cmf = aObjectTypeData[type].objectClass == OBJECT_CLASS_CAPITAL_SHIP ? space : cockpit;
    SDL_snprintf(chunkType, sizeof(chunkType), "%s%02d", cmf == space ? "SH" : "ST", type);
    if (SwcCMGetChunk(cmf, chunkType, 1, &set) != 0)
        return -1;
    if (SwcGetFrameCount(&set, &count) != 0)
        goto done;
    if (count != SDL_arraysize(images->views)) {
        SDL_SetError("Expected 37 SWC ship views in %s/1", chunkType);
        goto done;
    }
    for (index = 0; index < count; index++) {
        images->views[index] = SdlCreateSwcTexture(renderer, &set, index, colors);
        if (images->views[index] == NULL)
            goto done;
    }
    result = 0;
done:
    SDL_free(set.data);
    return result;
}

static int SwcStartMissionShips(void)
{
    short records[10];
    short record;
    short count = 1;
    short index;
    short prior;

    records[0] = nPlayerMissionShipIndex;
    for (index = 0; index < 18; index++) {
        record = index < 8 ? nInitialMissionShipIndices[index] :
            aMissionNavPoints[nMissionEntryNavPoint].missionShips[index - 8];
        if (record < 0)
            continue;
        for (prior = 0; prior < count && records[prior] != record; prior++) {
        }
        if (prior != count)
            continue;
        if (count == (short)SDL_arraysize(records))
            return SDL_SetError("SWC entry nav exceeds WC1's ten active ships");
        if ((int)aMissionShips[record].type < 0 ||
            aObjectTypeData[aMissionShips[record].type].objectClass < OBJECT_CLASS_SHIP)
            return SDL_SetError("Unsupported SWC entry-nav object");
        records[count++] = record;
    }
    memset(aeObjectClass, 0, sizeof(aeObjectClass));
    memset(nShipMissionIndices, 0xff, sizeof(nShipMissionIndices));
    nCurrentNavPoint = nMissionEntryNavPoint;
    stCampaignState.playerShipType = aMissionShips[nPlayerMissionShipIndex].type;
    bInitialFormationSetup = 1;
    for (index = 0; index < count; index++) {
        record = records[index];
        if (aMissionShips[record].leaderMissionIndex != -1 &&
            find_ship_index(aMissionShips[record].leaderMissionIndex) == -1)
            return SDL_SetError("SWC entry formation needs its leader initialized first");
        set_objects_data(index, aMissionShips[record].type, -1);
        Set_up_ship_info(index, record, (signed char)nCurrentNavPoint);
    }
    bInitialFormationSetup = 0;
    Build_objective_list();
    initialize_direction_view_frames();
    nNavPointerObject = -1;
    nCameraViewMode = 0;
    nCannedSceneMode = 0;
    acVduModeStackDepth[1] = 0;
    ausVduModeStack[8] = 5;
    nScreenWidth = SWC_FRAME_WIDTH;
    copy_frame(0, EYE_OBJECT);
    aShipPosition[EYE_OBJECT] = aShipPosition[0];
    asObjectCollisionRadius[EYE_OBJECT] = asObjectCollisionRadius[0];
    generate_stars();
    return count;
}

static void SwcResetFlightInput(void)
{
    bCurrentKey = 0;
    cPreviousKey = 0;
    nPitchInput = 0;
    nYawInput = 0;
    nRollInput = 0;
    bFlightRollLatch = 0;
    bMouseCursorVisible = 0;
    bMouseAfterburnerControl = 0;
    nMouseYawInput = 0;
    nMousePitchInput = 0;
    swcMouseAfterburner = 0;
    swcRightClickPending = 0;
}

static void SwcCentreFlightMouse(void)
{
    SwcResetFlightInput();
    nHostMouseMessageX = (stSpaceBuffer.left + stSpaceBuffer.right) / 2;
    nHostMouseMessageY = (stSpaceBuffer.top + stSpaceBuffer.bottom) / 2;
    WarpMouseTo((short)nHostMouseMessageX, (short)nHostMouseMessageY);
    bMouseCursorVisible = 1;
}

static void SwcFlightTick(const Uint8 *keys, int shipCount)
{
    short object;
    short throttle;
    int secondaryButton;

    /* Retain WC1's key priority, diagonal keys, ramp and direction reversal.
       Its Win32 key polling is supplied by the same SDL sample for the whole
       tick, including modifiers and the existing deterministic check path. */
    SdlSetKeyboardSnapshot(keys);
    cPreviousKey = (signed char)bCurrentKey;
    bCurrentKey = (unsigned char)PollKeyboardState();
    if (bCurrentKey == 0 && bMouseCursorVisible) {
        /* Feed the original mouse branch in player_input. A held position is
           sampled each tick, so right-button throttle is independent of SDL
           motion-event frequency. Button actions that fire weapons stay out
           of the queue; the right button is only a motion modifier here. */
        secondaryButton = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK) != 0;
        QueueInputEvent(13, (unsigned short)nHostMouseMessageX,
                        (unsigned short)nHostMouseMessageY, 0, 0, secondaryButton, 0);
        player_input();
        /* WC1 recentres the pointer when right-button roll/throttle ends. */
        nHostMouseMessageX = stMouseCursorState.x;
        nHostMouseMessageY = stMouseCursorState.y;
    } else {
        /* Keyboard steering takes over until the next mouse movement.
           Release each keyboard axis independently, even if another is held. */
        if (!(nUpArrowKeyState || nDownArrowKeyState || nHomeKeyState ||
              nPageUpKeyState || nEndKeyState || nPageDownKeyState))
            nPitchInput = 0;
        if (!(nLeftArrowKeyState || nRightArrowKeyState || nHomeKeyState ||
              nPageUpKeyState || nEndKeyState || nPageDownKeyState))
            nYawInput = 0;
        if (!(nInsertKeyState || nDeleteKeyState ||
              nOemCommaKeyState || nOemPeriodKeyState))
            nRollInput = 0;
        if (bCurrentKey == 0x4c) {
            SwcCentreFlightMouse();
        } else if (bCurrentKey != 0) {
            bMouseAfterburnerControl = 0;
            process_player_input();
        } else {
            bFlightRollLatch = 0;
        }
    }
    SdlSetKeyboardSnapshot(NULL);

    /* Movement commands from WC1's HandleSpaceFlightControls. Poll held keys
       separately so thrust can change while steering, without host key repeat. */
    throttle = (short)((keys[SDL_SCANCODE_EQUALS] || keys[SDL_SCANCODE_KP_PLUS]) -
                       (keys[SDL_SCANCODE_MINUS] || keys[SDL_SCANCODE_KP_MINUS]));
    if (keys[SDL_SCANCODE_BACKSPACE])
        anShipSpeed[0] = 0;
    else if (throttle != 0)
        accelerate(throttle);
    if (keys[SDL_SCANCODE_TAB] || keys[SDL_SCANCODE_KP_MULTIPLY] ||
        (swcMouseAfterburner && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK)))
        your_afterburner();
    players_flight_dynamics();
    for (object = 0; object < shipCount; object++) {
        rotate_object(object);
        accelerate_and_move_object(object);
    }
    copy_frame(0, EYE_OBJECT);
    aShipPosition[EYE_OBJECT] = aShipPosition[0];
    aShipVelocity[EYE_OBJECT] = aShipVelocity[0];
    update_star_field();
    for (object = 34; object < 42; object++)
        accelerate_and_move_object(object);
    nSpaceFrame++;
}

static int SwcDrawFlight(SDL_Renderer *renderer, SDL_Texture *cockpit,
                          SwcFlightImages images[OBJECT_TYPE_COUNT], int shipCount,
                          int cockpitless, int showMap)
{
    SDL_Rect rectangle;
    SDL_Texture *texture;
    short object;
    short frame;
    short sorted[10];
    short index;
    short prior;
    int centreY = cockpitless ? 120 : 80;
    int x;
    int y;
    int scale;

    transform_objects_to_your_view();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (SDL_RenderClear(renderer) != 0)
        return -1;
    SDL_SetRenderDrawColor(renderer, 160, 175, 200, 255);
    for (object = 34; object < 49; object++) {
        if (asObjectScreenX[object] != (short)0x8001)
            SDL_RenderDrawPoint(renderer, 160 + asObjectScreenX[object],
                                 centreY - asObjectScreenY[object]);
    }
    for (object = 1; object < shipCount; object++) {
        index = object - 1;
        while (index > 0 && asObjectDistance[sorted[index - 1]] < asObjectDistance[object]) {
            sorted[index] = sorted[index - 1];
            index--;
        }
        sorted[index] = object;
    }
    for (index = 0; index < shipCount - 1; index++) {
        object = sorted[index];
        if (asObjectScreenX[object] == (short)0x8001)
            continue;
        frame = asObjectViewFrame[object];
        if (frame < 0 || frame >= 37)
            return SDL_SetError("WC1 selected an unsupported SWC ship view: %d", frame);
        texture = images[aeObjectType[object]].views[frame];
        if (texture == NULL)
            return SDL_SetError("SWC flight image is missing");
        if (SDL_QueryTexture(texture, NULL, NULL, &rectangle.w, &rectangle.h) != 0)
            return -1;
        scale = (unsigned short)asObjectScreenScale[object];
        rectangle.w = SDL_max(1, rectangle.w * scale / 256);
        rectangle.h = SDL_max(1, rectangle.h * scale / 256);
        rectangle.x = 160 + asObjectScreenX[object] - rectangle.w / 2;
        rectangle.y = centreY - asObjectScreenY[object] - rectangle.h / 2;
        if (SDL_RenderCopyEx(renderer, texture, NULL, &rectangle,
                             asObjectScreenAngle[object], NULL,
                             asObjectFlip[object] ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) != 0)
            return -1;
    }
    SDL_SetRenderDrawColor(renderer, 70, 230, 130, 255);
    object = nNavPointerObject;
    if (object >= 0 && asObjectScreenX[object] != (short)0x8001) {
        rectangle.x = 156 + asObjectScreenX[object];
        rectangle.y = centreY - asObjectScreenY[object] - 4;
        rectangle.w = rectangle.h = 9;
        SDL_RenderDrawRect(renderer, &rectangle);
    }
    if (!cockpitless) {
        if (SDL_RenderCopy(renderer, cockpit, NULL, NULL) != 0 ||
            SdlDrawSwcCockpit() != 0)
            return -1;
    }
    SDL_SetRenderDrawColor(renderer, 70, 230, 130, 255);
    SDL_RenderDrawLine(renderer, 155, centreY, 158, centreY);
    SDL_RenderDrawLine(renderer, 162, centreY, 165, centreY);
    SDL_RenderDrawLine(renderer, 160, centreY - 5, 160, centreY - 2);
    SDL_RenderDrawLine(renderer, 160, centreY + 2, 160, centreY + 5);
    if (showMap) {
        SetScale();
        rectangle.x = 80;
        rectangle.y = 22;
        rectangle.w = 160;
        rectangle.h = 146;
        SDL_SetRenderDrawColor(renderer, 4, 10, 22, 255);
        SDL_RenderFillRect(renderer, &rectangle);
        SDL_SetRenderDrawColor(renderer, 70, 230, 130, 255);
        for (index = 0; index < cMissionObjectiveCount; index++) {
            short mapX;
            short mapY;

            nav_getxy(&mapX, &mapY, aMissionObjectives[index].position.x,
                       aMissionObjectives[index].position.z);
            x = 85 + mapX;
            y = 27 + mapY;
            rectangle.x = x - 2;
            rectangle.y = y - 2;
            rectangle.w = rectangle.h = index == cCurrentObjective ? 7 : 4;
            SDL_RenderDrawRect(renderer, &rectangle);
            prior = (short)((index + 1) % cMissionObjectiveCount);
            nav_getxy(&mapX, &mapY, aMissionObjectives[prior].position.x,
                       aMissionObjectives[prior].position.z);
            SDL_RenderDrawLine(renderer, x, y, 85 + mapX, 27 + mapY);
        }
    }
    return 0;
}

int SdlRunSwcMission(const char *missionPath, int checkOnly, int cockpitless)
{
    SwcCmf cockpitCmf = {0};
    SwcCmf spaceCmf = {0};
    SwcBuffer cockpitSet = {0};
    SwcFlightImages images[OBJECT_TYPE_COUNT] = {{{0}}};
    SDL_Color colors[256];
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *cockpit = NULL;
    Viewport savedSpaceBuffer = stSpaceBuffer;
    Viewport *savedMouseViewport = stMouseCursorState.viewport;
    void (*savedInputPump)(void) = pEventManagerPump;
    int savedCockpitlessView = bCockpitlessView;
    SDL_Event event;
    FixedVector start;
    Uint8 checkKeys[SDL_NUM_SCANCODES] = {0};
    Uint32 previous;
    Uint32 current;
    Uint32 tickInterval;
    Uint32 elapsed = 0;
    char path[PATH_MAX];
    char title[256];
    char failure[256];
    int shipCount;
    int object;
    int view;
    int ticks = 0;
    int showMap = 0;
    int paused = 0;
    int focused = 1;
    int done = 0;
    int result = -1;

    nCampaignDataSet = 0;
    if (SdlLoadSwcMissionData(missionPath, 1, 0) != 0)
        goto done;
    if (!SdlResolvePath("CMFs/PCShipV00.CMF", path, sizeof(path)) ||
        SwcCMOpen(path, &cockpitCmf) != 0)
        goto done;
    if (!SdlResolvePath("CMFs/Spaceflight.CMF", path, sizeof(path)) ||
        SwcCMOpen(path, &spaceCmf) != 0)
        goto done;
    SdlResolvePath("SuperWingCommanderDemo.rsrc", path, sizeof(path));
    if (SwcReadPalette(path, 251, colors) != 0) {
        SdlResolvePath("Super Wing Commander Demo/..namedfork/rsrc", path, sizeof(path));
        if (SwcReadPalette(path, 251, colors) != 0) {
            SDL_SetError("Cannot read the SWC palette. Preserve the demo's resource fork "
                         "or place the raw fork in SuperWingCommanderDemo.rsrc.");
            goto done;
        }
    }
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
        goto done;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    window = SDL_CreateWindow("Super Wing Commander - Enyo 1 (experimental flight)",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 720,
                               checkOnly ? SDL_WINDOW_HIDDEN : SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        goto done;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL || SDL_RenderSetLogicalSize(renderer, 320, 240) != 0)
        goto done;
    if (SwcCMGetChunk(&cockpitCmf, "PC00", 1, &cockpitSet) != 0)
        goto done;
    cockpit = SdlCreateSwcTexture(renderer, &cockpitSet, 0, colors);
    if (cockpit == NULL)
        goto done;
    if (SdlInitSwcCockpit(renderer, &cockpitCmf, &spaceCmf, colors) != 0)
        goto done;
    swcFlightActive = 1;
    srand(1);
    shipCount = SwcStartMissionShips();
    if (shipCount < 0)
        goto done;
    SwcResetFlightInput();
    FlushInputEvents();
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBuffer;
    /* SWC's view is centred in this rectangle in either cockpit mode. The
       WC1 cockpitless camera offsets do not apply to the 320x240 SDL view. */
    bCockpitlessView = 0;
    SetViewportRect(&stSpaceBuffer, 0, 0, 319, cockpitless ? 239 : 159);
    bHostPrimaryMouseButton = 0;
    bHostSecondaryMouseButton = 0;
    nSpaceFrame = 0;
    SetSpaceFlightFrameTiming();
    tickInterval = (Uint32)nFrameIntervalMs;
    for (object = 1; object < shipCount; object++) {
        if (SwcLoadFlightImages(renderer, &cockpitCmf, &spaceCmf, colors,
                                aeObjectType[object], &images[aeObjectType[object]]) != 0)
            goto done;
    }
    start = aShipPosition[0];
    previous = SDL_GetTicks();
    if (!checkOnly) {
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
        SDL_SetWindowMouseGrab(window, SDL_TRUE);
        SwcCentreFlightMouse();
    }
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                done = 1;
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    focused = 0;
                    elapsed = 0;
                    SwcResetFlightInput();
                    SDL_SetWindowMouseGrab(window, SDL_FALSE);
                } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    focused = 1;
                    previous = SDL_GetTicks();
                    if (!checkOnly && !paused) {
                        SDL_SetWindowMouseGrab(window, SDL_TRUE);
                        SwcCentreFlightMouse();
                    }
                } else if ((event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                            event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) &&
                           !checkOnly && focused && !paused) {
                    SDL_SetWindowMouseGrab(window, SDL_FALSE);
                    SDL_SetWindowMouseGrab(window, SDL_TRUE);
                }
            }
            if (!checkOnly && focused && !paused) {
                /* SDL_RenderSetLogicalSize already converts these positions
                   to 320x240, including window scaling and letterboxing. */
                if (event.type == SDL_MOUSEMOTION) {
                    nHostMouseMessageX = SDL_clamp(event.motion.x,
                                                   stSpaceBuffer.left, stSpaceBuffer.right);
                    nHostMouseMessageY = SDL_clamp(event.motion.y,
                                                   stSpaceBuffer.top, stSpaceBuffer.bottom);
                    bMouseCursorVisible = 1;
                } else if ((event.type == SDL_MOUSEBUTTONDOWN ||
                            event.type == SDL_MOUSEBUTTONUP) &&
                           event.button.button == SDL_BUTTON_RIGHT) {
                    nHostMouseMessageX = SDL_clamp(event.button.x,
                                                   stSpaceBuffer.left, stSpaceBuffer.right);
                    nHostMouseMessageY = SDL_clamp(event.button.y,
                                                   stSpaceBuffer.top, stSpaceBuffer.bottom);
                    if (event.type == SDL_MOUSEBUTTONDOWN) {
                        /* WC1 uses a timing-only window of 20 ticks at 60 Hz.
                           Recentring after the first click must not cancel it. */
                        swcMouseAfterburner = swcRightClickPending &&
                            (Uint32)(event.button.timestamp - swcLastRightClick) <=
                                20 * 1000 / 60;
                        swcLastRightClick = event.button.timestamp;
                        swcRightClickPending = 1;
                    } else {
                        swcMouseAfterburner = 0;
                    }
                    bMouseCursorVisible = 1;
                }
            }
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    done = 1;
                    break;
                case SDLK_n:
                    cycle_next_objective();
                    break;
                case SDLK_c:
                    cockpitless = !cockpitless;
                    SetViewportRect(&stSpaceBuffer, 0, 0, 319, cockpitless ? 239 : 159);
                    if (!checkOnly && focused && !paused)
                        SwcCentreFlightMouse();
                    break;
                case SDLK_m:
                    showMap = !showMap;
                    break;
                case SDLK_p:
                    paused = !paused;
                    elapsed = 0;
                    previous = SDL_GetTicks();
                    SwcResetFlightInput();
                    SDL_SetWindowMouseGrab(window, !checkOnly && focused && !paused
                                                   ? SDL_TRUE : SDL_FALSE);
                    if (!checkOnly && focused && !paused)
                        SwcCentreFlightMouse();
                    break;
                default:
                    break;
                }
            }
        }
        if (done)
            break;
        current = SDL_GetTicks();
        if (focused && !paused)
            elapsed += SDL_min(current - previous, 250);
        else
            elapsed = 0;
        previous = current;
        if (checkOnly) {
            /* Deterministic exercise of the same input/dynamics/render path. */
            checkKeys[SDL_SCANCODE_RIGHT] = ticks < 100;
            checkKeys[SDL_SCANCODE_EQUALS] = 1;
            elapsed = tickInterval;
        }
        while (elapsed >= tickInterval) {
            SwcFlightTick(checkOnly ? checkKeys : SDL_GetKeyboardState(NULL), shipCount);
            elapsed -= tickInterval;
            ticks++;
        }
        if (SwcDrawFlight(renderer, cockpit, images, shipCount, cockpitless, showMap) != 0)
            goto done;
        SDL_RenderPresent(renderer);
        SDL_snprintf(title, sizeof(title),
                     "SWC Enyo 1%s | %s | speed %ld (set %d)%s | mouse/arrows: steer RMB: roll/throttle +/-: speed Tab: boost N: nav M: map C: cockpit P: pause Esc: exit",
                     paused || !focused ? " (paused)" : "",
                     aMissionObjectives[(int)cCurrentObjective].name,
                     MultiplyFixed(Vector_magnitude(&aShipVelocity[0]), 0xa00) >> 8,
                     (anShipSpeed[0] >> 8) * 10,
                     aeSpecialManeuver[0] == SPECIAL_MANEUVER_AFTERBURNER ? " afterburner" : "");
        SDL_SetWindowTitle(window, title);
        if (checkOnly && ticks == 120) {
            if (memcmp(&start, &aShipPosition[0], sizeof(start)) == 0 || cMissionObjectiveCount != 4) {
                SDL_SetError("SWC first mission did not advance the shared WC1 flight state");
                goto done;
            }
            printf("SWC Enyo 1 flight: %d ships, %d objectives, %d WC1 simulation ticks, SDL2 rendering: OK\n",
                   shipCount, cMissionObjectiveCount, ticks);
            done = 1;
        }
        if (!checkOnly)
            SDL_Delay(8);
    }
    result = 0;
done:
    SDL_strlcpy(failure, SDL_GetError(), sizeof(failure));
    SdlSetKeyboardSnapshot(NULL);
    SwcResetFlightInput();
    if (swcFlightActive) {
        FlushInputEvents();
        SdlEndJoystickSpaceflight();
        SetCinematicFrameTiming();
    }
    stSpaceBuffer = savedSpaceBuffer;
    stMouseCursorState.viewport = savedMouseViewport;
    SetEventManagerPump(savedInputPump);
    bCockpitlessView = savedCockpitlessView;
    swcFlightActive = 0;
    if (window != NULL)
        SDL_SetWindowMouseGrab(window, SDL_FALSE);
    for (object = 0; object < OBJECT_TYPE_COUNT; object++) {
        for (view = 0; view < 37; view++)
            SDL_DestroyTexture(images[object].views[view]);
    }
    SdlFreeSwcCockpit();
    SDL_DestroyTexture(cockpit);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_free(cockpitSet.data);
    SwcCMClose(&spaceCmf);
    SwcCMClose(&cockpitCmf);
    if (result != 0)
        SDL_SetError("%s", failure);
    return result;
}
