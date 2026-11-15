/* Super Wing Commander for the flight fuzzer (flight.h): SwcFlightTick driven
 * through the handlers of SdlRunSwcMission's flight loop. runFuzzer.py runs
 * it on an SWC directory, which is only read. */
#include <dlfcn.h>
#include "flight.h"
#include "swc.h"

/* The autopilot camera paces itself with SDL time and window focus. Run it
 * on a faster clock, with the hidden window treated as focused. */
#define TIME_SCALE 64

static Uint32 (*RealTicks)(void);
static Uint64 (*RealCounter)(void);
static void (*RealDelay)(Uint32);
static Uint32 (*RealWindowFlags)(SDL_Window *);

static void BindSdl(void)
{
    RealTicks = (Uint32 (*)(void))dlsym(RTLD_NEXT, "SDL_GetTicks");
    RealCounter = (Uint64 (*)(void))dlsym(RTLD_NEXT, "SDL_GetPerformanceCounter");
    RealDelay = (void (*)(Uint32))dlsym(RTLD_NEXT, "SDL_Delay");
    RealWindowFlags = (Uint32 (*)(SDL_Window *))dlsym(RTLD_NEXT, "SDL_GetWindowFlags");
}

Uint32 SDL_GetTicks(void)
{
    if (RealTicks == NULL)
        BindSdl();
    return (Uint32)((Uint64)RealTicks() * TIME_SCALE);
}

Uint64 SDL_GetPerformanceCounter(void)
{
    if (RealCounter == NULL)
        BindSdl();
    return RealCounter() * TIME_SCALE;
}

void SDL_Delay(Uint32 ms)
{
    if (RealDelay == NULL)
        BindSdl();
    RealDelay(ms / TIME_SCALE);
}

Uint32 SDL_GetWindowFlags(SDL_Window *window)
{
    if (RealWindowFlags == NULL)
        BindSdl();
    return RealWindowFlags(window) | SDL_WINDOW_INPUT_FOCUS;
}

/* The harness reaches swc_flight.c's per-tick flight step and drawing. */
#include "../src/sdl/swc_flight.c"

/* The Mac menus' key equivalents: Command on macOS, Control elsewhere. */
#ifdef __APPLE__
#define MENU_KEY KMOD_GUI
#else
#define MENU_KEY KMOD_CTRL
#endif

/* Keys repeat SdlRunSwcMission's flight handlers (the Mac demo's keys, its
 * menus' Command keys and F1-F3) and SwcFlightTick's held controls; pointer,
 * buttons and wheel repeat its mouse handlers. The computer (C) and pause
 * are modal and need a separate driver; ACTION_EJECT performs the computer's
 * Eject page, which has no key of its own. */
const FlightAction flightActions[] = {
    {ACTION_NONE, 0, 0, 0},
    {ACTION_KEY, SDLK_UP, 0, 0}, {ACTION_KEY, SDLK_DOWN, 0, 0},
    {ACTION_KEY, SDLK_LEFT, 0, 0}, {ACTION_KEY, SDLK_RIGHT, 0, 0},
    {ACTION_KEY, SDLK_i, 0, 0}, {ACTION_KEY, SDLK_k, 0, 0},
    {ACTION_KEY, SDLK_j, 0, 0}, {ACTION_KEY, SDLK_l, 0, 0},
    {ACTION_KEY, SDLK_1, 0, 0}, {ACTION_KEY, SDLK_2, 0, 0}, {ACTION_KEY, SDLK_3, 0, 0},
    {ACTION_KEY, SDLK_4, 0, 0}, {ACTION_KEY, SDLK_6, 0, 0}, {ACTION_KEY, SDLK_7, 0, 0},
    {ACTION_KEY, SDLK_8, 0, 0}, {ACTION_KEY, SDLK_9, 0, 0},
    {ACTION_KEY, SDLK_COMMA, 0, 0}, {ACTION_KEY, SDLK_PERIOD, 0, 0},
    {ACTION_KEY, SDLK_SPACE, 0, 0}, {ACTION_KEY, SDLK_SPACE, 0, 0},
    {ACTION_KEY, SDLK_EQUALS, 0, 0}, {ACTION_KEY, SDLK_MINUS, 0, 0},
    {ACTION_KEY, SDLK_TAB, 0, 0}, {ACTION_KEY, SDLK_RETURN, 0, 0},
    {ACTION_KEY, SDLK_g, 0, 0}, {ACTION_KEY, SDLK_w, 0, 0},
    {ACTION_KEY, SDLK_t, 0, 0}, {ACTION_KEY, SDLK_a, 0, 0},
    {ACTION_KEY, SDLK_b, 0, 0}, {ACTION_KEY, SDLK_r, 0, 0},
    {ACTION_KEY, SDLK_h, 0, 0}, {ACTION_KEY, SDLK_e, 0, 0},
    {ACTION_KEY, SDLK_F1, 0, 0}, {ACTION_KEY, SDLK_F2, 0, 0}, {ACTION_KEY, SDLK_F3, 0, 0},
    {ACTION_KEY, SDLK_F1, KMOD_CTRL, 0},
    {ACTION_KEY, SDLK_y, MENU_KEY, 0}, {ACTION_KEY, SDLK_d, MENU_KEY, 0},
    {ACTION_KEY, SDLK_f, MENU_KEY, 0},
    {ACTION_KEY, SDLK_EQUALS, MENU_KEY, 0}, {ACTION_KEY, SDLK_MINUS, MENU_KEY, 0},
    {ACTION_EJECT, 0, 0, 0},
    {ACTION_POINTER, 0, 40, 25}, {ACTION_POINTER, 0, 160, 25}, {ACTION_POINTER, 0, 280, 25},
    {ACTION_POINTER, 0, 40, 100}, {ACTION_POINTER, 0, 160, 100}, {ACTION_POINTER, 0, 280, 100},
    {ACTION_POINTER, 0, 40, 175}, {ACTION_POINTER, 0, 160, 175}, {ACTION_POINTER, 0, 280, 175},
    {ACTION_BUTTONS, 0, SDL_BUTTON_LMASK, 0}, {ACTION_BUTTONS, 0, SDL_BUTTON_RMASK, 0},
    {ACTION_BUTTONS, 0, SDL_BUTTON_LMASK | SDL_BUTTON_RMASK, 0},
    {ACTION_WHEEL, 0, 1, 0}, {ACTION_WHEEL, 0, -1, 0}
};
const size_t flightActionCount = SDL_arraysize(flightActions);

typedef char FlightActionsFitSixBits[SDL_arraysize(flightActions) <= 64 ? 1 : -1];

/* Throttle, steer while firing, aim and click, cycle weapons, fire a
 * missile, order the wingman, light the afterburner, then try autopilot. */
const FlightSeedStep flightSeed[] = {
    {{ACTION_KEY, SDLK_EQUALS, 0, 0}, 0, 31},
    {{ACTION_KEY, SDLK_UP, 0, 0}, 0, 15},
    {{ACTION_KEY, SDLK_SPACE, 0, 0}, 1, 31},
    {{ACTION_POINTER, 0, 160, 25}, 0, 7},
    {{ACTION_BUTTONS, 0, SDL_BUTTON_LMASK, 0}, 0, 15},
    {{ACTION_KEY, SDLK_g, 0, 0}, 0, 0},
    {{ACTION_KEY, SDLK_w, 0, 0}, 0, 0},
    {{ACTION_KEY, SDLK_RETURN, 0, 0}, 0, 7},
    {{ACTION_KEY, SDLK_b, 0, 0}, 0, 3},
    {{ACTION_KEY, SDLK_r, 0, 0}, 0, 15},
    {{ACTION_KEY, SDLK_TAB, 0, 0}, 0, 15},
    {{ACTION_KEY, SDLK_a, 0, 0}, 0, 31}
};
const size_t flightSeedLength = SDL_arraysize(flightSeed);

static Uint8 keys[SDL_NUM_SCANCODES];
static Uint32 buttons;
static Uint32 flightClock;
static SwcCmf fuzzCockpitCmf, fuzzSpaceCmf;
static SwcFlightImages fuzzImages[OBJECT_TYPE_COUNT];
static SDL_Color fuzzColors[256];
static ObjectTypeData fuzzSavedObjects[OBJECT_TYPE_COUNT];
static char fuzzMissionPath[PATH_MAX];
static char fuzzResourcePath[PATH_MAX];
static SDL_Renderer *fuzzRenderer;
static int fuzzCockpitless;

int FlightContinues(void)
{
    return nArcadeState == 0 && !bCurrentPlayerDestroyed;
}

/* One host simulation tick, as SdlRunSwcMission's flight loop. Travelling
 * the flight path draws nothing. */
void FlightStep(int frame)
{
    flightClock += SDL_max(1, nFrameIntervalMs);
    nTickCount60Hz = (unsigned int)((uint64_t)flightClock * 60 / 1000);
    /* As the host: rebound keys and buttons stand in for the ones SWC reads. */
    SwcFlightTick(SdlSwcKeymapKeyboardState(keys), SdlSwcKeymapMouseButtons(buttons));
    if (frame >= 0)
        Require(SwcDrawFlight(fuzzRenderer, swcCockpitTexture, &fuzzCockpitCmf, &fuzzSpaceCmf,
                              fuzzColors, fuzzImages, fuzzCockpitless) == 0, "draw SWC flight");
}

static void PressFlightKey(SDL_Keycode key, Uint16 modifiers)
{
    FixedVector departure;
    int menuKey = SwcMenuShortcut(modifiers);
    int plainKey = !(modifiers & (KMOD_CTRL | KMOD_GUI | KMOD_ALT));
    int view = -1;

    switch (key) {
    case SDLK_SPACE:
        if (plainKey)
            swcGunFirePending = 1;
        break;
    case SDLK_RETURN:
        if (plainKey)
            swcReleaseWeaponPending = 1;
        break;
    case SDLK_EQUALS:
    case SDLK_MINUS:
        if (menuKey && FlightContinues())
            accelerate(key == SDLK_EQUALS ? 1 : -1);
        break;
    case SDLK_a:
        if ((plainKey || menuKey) && FlightContinues() && !nTrainSimActive) {
            departure = aShipPositionsArray[0];
            auto_pilot_sequence();
            Require(nSwcCinemaResult == 0, "SWC autopilot camera");
            /* Travel can move the ship by an entire nav leg. */
            if (!equ_vector(&departure, &aShipPositionsArray[0]))
                SwcSelectFlightView(nCameraViewMode);
        }
        break;
    case SDLK_t:
        if (plainKey || menuKey)
            cycle_onscreen_targets();
        break;
    case SDLK_g:
        if (plainKey || menuKey)
            select_new_gun();
        break;
    case SDLK_w:
        if (plainKey || menuKey) {
            select_new_release_weapon((enum ObjectType)-1);
            SdlSwcLockOff();
        }
        break;
    case SDLK_b:
    case SDLK_r:
    case SDLK_h:
        if (plainKey && FlightContinues())
            SdlSwcCommunicate(key == SDLK_b ? 0 : key == SDLK_r ? 1 : 2);
        break;
    case SDLK_e:
        if (plainKey && FlightContinues())
            SdlSwcCommunicate(3);
        break;
    case SDLK_F1:
        if (modifiers & KMOD_CTRL)
            fuzzCockpitless = !fuzzCockpitless;
        else
            view = 0;
        break;
    case SDLK_F2:
        view = 14;
        break;
    case SDLK_F3:
        view = 4;
        break;
    case SDLK_y:
    case SDLK_d:
    case SDLK_f:
        if (menuKey)
            view = key == SDLK_y ? 0 : key == SDLK_d ? 14 : 4;
        break;
    default:
        break;
    }
    if (view >= 0 && FlightContinues()) {
        if (view == nCameraViewMode)
            view = 0;
        if (view != nCameraViewMode)
            SwcSelectFlightView(view);
    }
}

/* The host's mouse handlers: motion steers, the left button fires guns,
 * a right double-click lights the afterburner and both buttons release a
 * weapon (sampled by SwcFlightTick). */
void ApplyFlightAction(const FlightAction *action)
{
    Uint32 pressed;
    SDL_Event event;

    switch (action->kind) {
    case ACTION_KEY:
        keys[SDL_GetScancodeFromKey(action->key)] = 1;
        /* The host's event loop rewrites rebound keys first. */
        SDL_zero(event);
        event.type = SDL_KEYDOWN;
        event.key.keysym.scancode = SDL_GetScancodeFromKey(action->key);
        event.key.keysym.sym = action->key;
        event.key.keysym.mod = (Uint16)action->x;
        SdlSwcKeymapTranslateEvent(&event);
        if (event.type == SDL_KEYDOWN)
            PressFlightKey(event.key.keysym.sym, event.key.keysym.mod);
        break;
    case ACTION_POINTER:
        swcJoystickSteering = 0;
        nHostMouseMessageX = SDL_clamp(action->x, stSpaceBufferViewport.left,
                                       stSpaceBufferViewport.right);
        nHostMouseMessageY = SDL_clamp(action->y, stSpaceBufferViewport.top,
                                       stSpaceBufferViewport.bottom);
        bMouseCursorVisible = 1;
        break;
    case ACTION_BUTTONS:
        pressed = (Uint32)action->x & ~buttons;
        buttons |= (Uint32)action->x;
        for (Uint8 button = SDL_BUTTON_LEFT; button <= SDL_BUTTON_RIGHT; button++) {
            if (!(pressed & SDL_BUTTON(button)))
                continue;
            /* The host's event loop maps the guns and afterburner buttons
               onto SWC's left and right ones first. */
            SDL_zero(event);
            event.type = SDL_MOUSEBUTTONDOWN;
            event.button.button = button;
            SdlSwcKeymapTranslateEvent(&event);
            if (event.type == SDL_KEYDOWN) {
                PressFlightKey(event.key.keysym.sym, event.key.keysym.mod);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                swcGunFirePending = 1;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
                swcMouseAfterburner = swcRightClickPending &&
                    flightClock - swcLastRightClick <= 20 * 1000 / 60;
                swcLastRightClick = flightClock;
                swcRightClickPending = 1;
                bMouseCursorVisible = 1;
            }
        }
        break;
    case ACTION_WHEEL:
        SDL_zero(event);
        event.type = SDL_MOUSEWHEEL;
        event.wheel.y = action->x;
        SdlSwcKeymapTranslateEvent(&event);
        if (event.type == SDL_KEYDOWN)
            PressFlightKey(event.key.keysym.sym, event.key.keysym.mod);
        else if (event.type == SDL_MOUSEWHEEL && FlightContinues())
            accelerate(event.wheel.y > 0 ? 1 : -1);
        break;
    case ACTION_EJECT:
        /* SdlShowSwcComputer's Eject page activation. */
        if (FlightContinues() && acPlayerComponentDamageList[7] < 4) {
            if (acPlayerComponentDamageList[7] <= 1 ||
                RandomBelowOrEqual((acPlayerComponentDamageList[7] - 1)) == 0)
                nArcadeState = 2;
            else
                SdlPlaySwcSound(25, 127);
        }
        break;
    default:
        break;
    }
}

void ReleaseFlightControls(void)
{
    /* Releasing the afterburner button, whichever one it is. */
    if (SdlSwcKeymapMouseButtons(buttons) & SDL_BUTTON_RMASK)
        swcMouseAfterburner = 0;
    buttons = 0;
    memset(keys, 0, sizeof(keys));
}

void ViewFlightArrival(void)
{
    SwcSelectFlightView(nCameraViewMode);
    SwcTransformObjectsToView();
}

void StartFlightGame(void)
{
    SDL_Window *window;
    char path[PATH_MAX];

    Require(SdlFindSwcMissionData(fuzzMissionPath, sizeof(fuzzMissionPath)), "SWC CMFs/Data.CMF");
    /* As SdlRunSwcMission, before its startup movies. */
    SwcInitCharacters();
    nCampaignDataSet = 0;
    Require(SdlLoadSwcMissionData(fuzzMissionPath, 1, 0) == 0, "load the first SWC mission");
    Require(SdlResolvePath("CMFs/Spaceflight.CMF", path, sizeof(path)) &&
            SwcCMOpen(path, &fuzzSpaceCmf) == 0, "open Spaceflight.CMF");
    Require(SdlFindSwcResources(NULL, fuzzResourcePath, sizeof(fuzzResourcePath)) &&
            SwcReadPalette(fuzzResourcePath, 251, fuzzColors) == 0, "SWC application resources");
    SDL_SetMainReady();
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) == 0, "SDL init");
    window = SDL_CreateWindow("SWC flight fuzzer", 0, 0, 320, 240, SDL_WINDOW_HIDDEN);
    Require(window != NULL, "headless window");
    fuzzRenderer = SdlCreateSwcRenderer(window);
    Require(fuzzRenderer != NULL && SDL_RenderSetLogicalSize(fuzzRenderer, 320, 240) == 0,
            "SWC renderer");
    swcRasterCanvas = SDL_CreateTexture(fuzzRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT);
    Require(swcRasterCanvas != NULL &&
            SDL_SetTextureBlendMode(swcRasterCanvas, SDL_BLENDMODE_BLEND) == 0, "raster canvas");
    Require(SdlCreateSwcTextFont(fuzzRenderer, &swcPauseFont, 10) == 0, "SWC text font");
    swcFlightActive = 1;
    swcCinemaWindow = window;
    swcCinemaRenderer = fuzzRenderer;
    swcCinemaCockpitCmf = &fuzzCockpitCmf;
    swcCinemaSpaceCmf = &fuzzSpaceCmf;
    swcCinemaColors = fuzzColors;
    swcCinemaImages = fuzzImages;
    SwcSetObjectData(fuzzSavedObjects);
    srand(1);
    Require(SwcStartMissionShips() >= 0 &&
            SwcInitializeCockpit(stCampaignState.playerShipType, fuzzResourcePath) == 0,
            "initial SWC cockpit");
    Require(SdlInitSwcSound(fuzzResourcePath) == 0, "SWC sound");
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
}

int LoadFlightCampaign(int campaign, unsigned char counts[14])
{
    SwcBuffer campaignData = {0};
    unsigned char *savedCampaign = pMissionCampaignInfo;
    int loaded;
    int series;

    nCampaignDataSet = campaign;
    loaded = SdlLoadSwcCampaignData(fuzzMissionPath, &campaignData) == 0;
    if (loaded) {
        for (series = 1; series <= 13; series++)
            counts[series] = pMissionCampaignInfo[(series - 1) * 90 + 2];
    } else {
        SDL_ClearError();
    }
    pMissionCampaignInfo = savedCampaign;
    SDL_free(campaignData.data);
    nCampaignDataSet = 0;
    return loaded;
}

void BeginFlight(int campaign, int series, int mission)
{
    int simulator = campaign == FLIGHT_SIMULATOR;
    int shipCount;
    int object;

    ReleaseFlightControls();
    flightClock = 0;
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    SwcResetFlightInput();
    SwcInitCharacters();
    nSwcBossmanDeath = 0;
    nArcadeState = 0;
    bCurrentPlayerDestroyed = 0;
    nTrainSimActive = 0;
    nCampaignDataSet = simulator ? 0 : campaign;
    stCampaignState.campaignIndex = nCampaignDataSet;
    stCampaignState.currentSeries = series;
    stCampaignState.currentMission = mission;
    Require(SdlLoadSwcMissionData(fuzzMissionPath, series, mission) == 0, "load SWC mission");
    shipCount = SwcStartMissionShips();
    Require(shipCount >= 0, "start SWC mission ships");
    Require(SwcInitializeCockpit(simulator ? 4 : stCampaignState.playerShipType,
                                 fuzzResourcePath) == 0, "SWC cockpit");
    if (simulator) {
        /* As the host's simulator_mission label (retail ArcadeGame). */
        nTrainSimMission = mission;
        nCurrentArcadeScore = 0;
        nTrainSimActive = 1;
        nArcadeWave = 0;
        nArcadeBonusCountdown = 0;
        nCurrentWave = aMissionNavPoints[nCurrentNavigationPoint + 1].type == 2 ? 2 : -1;
        FigureArcadeTime();
    }
    /* As the host's begin_flight label, without window and mouse capture. */
    bSwcWaitingForEscort = 0;
    nSwcCinemaResult = 0;
    SwcResetFlightInput();
    SetEventManagerPump(NULL);
    stMouseCursorState.viewport = &stSpaceBufferViewport;
    bCockpitlessView = 0;
    SetViewportRect(&stSpaceBufferViewport, 0, 0, 319, 199);
    bHostPrimaryMouseButton = 0;
    bHostSecondaryMouseButton = 0;
    nCurrentSpaceFrameValue = 0;
    swcExhaustFrame = -1;
    nRenderedSpaceFrame = 0;
    fuzzCockpitless = 0;
    SwcTransformObjectsToView();
    SdlUpdateSwcCockpit();
    SetSpaceFlightFrameTiming();
    for (object = 1; object < shipCount; object++)
        Require(SwcLoadFlightImages(&fuzzCockpitCmf, &fuzzSpaceCmf, aeObjectTypeList[object],
                                    &fuzzImages[aeObjectTypeList[object]]) == 0, "SWC ship images");
}

void EndFlight(void)
{
    SdlStopSwcSounds();
    SwcResetFlightInput();
    if (IsCommChoiceMenuOpen())
        CloseCommChoiceMenu();
}
