/* libFuzzer-driven SWC flight controls; use make check-fuzz-swc.
 * Input: data set (0-2, or 3 for the simulator), series (1-based; ignored by
 * the simulator), mission (0-based), start (bits 0-3: flight-path legs
 * travelled before control begins), little-endian PRNG seed (4 bytes), then
 * pairs of action and hold bytes. Action bits 0-5 select a control; bit 6
 * keeps the controls already held. Hold bits 0-5 give 1..64 ticks and bits
 * 6-7 multiply them by 1, 2, 4 or 8. The SWC directory is only read.
 * WC1_SWC_FLIGHT_TRACE prints where each input started and ended, and
 * WC1_SWC_FLIGHT_KEYS rebinds keys as the launcher does. */
#include <dlfcn.h>
#include "wc1.h"
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

#include "../src/sdl/swc_flight.c"

enum { ACTION_NONE, ACTION_KEY, ACTION_POINTER, ACTION_BUTTONS, ACTION_WHEEL, ACTION_EJECT };

/* The Mac menus' key equivalents: Command on macOS, Control elsewhere. */
#ifdef __APPLE__
#define MENU_KEY KMOD_GUI
#else
#define MENU_KEY KMOD_CTRL
#endif

typedef struct FlightAction {
    unsigned char kind;
    SDL_Keycode key;
    short x; /* key modifiers, pointer x, button mask or wheel step */
    short y;
} FlightAction;

/* Keys repeat SdlRunSwcMission's flight handlers (the Mac demo's keys, its
 * menus' Command keys and F1-F3) and SwcFlightTick's held controls; pointer,
 * buttons and wheel repeat its mouse handlers. The computer (C) and pause
 * are modal and need a separate driver; ACTION_EJECT performs the computer's
 * Eject page, which has no key of its own. */
static const FlightAction actions[] = {
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

typedef char FlightActionsFitSixBits[SDL_arraysize(actions) <= 64 ? 1 : -1];

static Uint8 keys[SDL_NUM_SCANCODES];
static Uint32 buttons;
static Uint32 flightClock;
static unsigned char missionCounts[3][13];
static SwcCmf fuzzCockpitCmf, fuzzSpaceCmf;
static SwcFlightImages fuzzImages[OBJECT_TYPE_COUNT];
static SDL_Color fuzzColors[256];
static ObjectTypeData fuzzSavedObjects[OBJECT_TYPE_COUNT];
static char fuzzMissionPath[PATH_MAX];
static char fuzzResourcePath[PATH_MAX];
static SDL_Renderer *fuzzRenderer;
static int fuzzCockpitless;
static int tickLimit = 600;
static int inputActive;
static int traceInputs;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s (%s)\n", message, SDL_GetError());
        abort();
    }
}

static void CheckFuzzExit(void)
{
    /* Original fatal errors may call exit(0); make libFuzzer retain the input. */
    Require(!inputActive, "game exited during SWC flight input or initialization");
}

static int FlightContinues(void)
{
    return nArcadeState == 0 && !bCurrentPlayerDestroyed;
}

/* One host simulation tick, as SdlRunSwcMission's flight loop. */
static void FlightStep(int draw)
{
    flightClock += SDL_max(1, nFrameIntervalMs);
    nTickCount60Hz = (unsigned int)((uint64_t)flightClock * 60 / 1000);
    /* As the host: rebound keys and buttons stand in for the ones SWC reads. */
    SwcFlightTick(SdlSwcKeymapKeyboardState(keys), SdlSwcKeymapMouseButtons(buttons));
    if (draw)
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
static void ApplyFlightAction(const FlightAction *action)
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

static void ReleaseFlightControls(void)
{
    /* Releasing the afterburner button, whichever one it is. */
    if (SdlSwcKeymapMouseButtons(buttons) & SDL_BUTTON_RMASK)
        swcMouseAfterburner = 0;
    buttons = 0;
    memset(keys, 0, sizeof(keys));
}

/* Escort waits per input: about three minutes at the SWC frame rate. */
#define ESCORT_WAIT_TICKS 3600

/* Reach a later part of the mission as autopilot arrival would: the player
 * stops short of the next flight-path objective with the formation that
 * auto_pilot_sequence takes along, ReleaseStaleNavTarget enters an active
 * nav point's sphere, and the game's own objective checks register the
 * arrival, advancing the flight path and applying the nav triggers. */
static void TravelFlightPath(int legs)
{
    FixedVector destination;
    signed char travelling[10];
    short formationSlot;
    short ship;
    int previousLeg;
    int tick;
    int waited = 0;

    while (legs-- > 0 && FlightContinues() && cCurrentNavPtIdx >= 0 &&
           cCurrentNavPtIdx < (int)SDL_arraysize(abFlightPathList) &&
           abFlightPathList[cCurrentNavPtIdx] != -1) {
        previousLeg = cCurrentNavPtIdx;
        destination = aMissionObjectivesArray[abFlightPathList[cCurrentNavPtIdx]].position;
        if ((unsigned short)distance_from_point(0, &destination) >= 1500) {
            for (ship = 0; ship < 10; ship++)
                travelling[ship] = ship != 0 && aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
                    aeSpecialManeuvers[ship] != SPECIAL_MANEUVER_UNKNOWN_9 &&
                    aeShipSide[ship] == SIDE_IMPERIAL &&
                    is_team_member(nShipMissionIndices[ship]) && !kilrathi_near(ship, 10000);
            point_at(0, destination);
            aShipPositionsArray[0] = destination;
            position_relative_ijk(&aShipPositionsArray[0], 0, 0, 0, -1000);
            formationSlot = 0;
            for (ship = 1; ship < 10; ship++) {
                if (!travelling[ship])
                    continue;
                auto_position(ship, &formationSlot);
                copy_frame(0, ship);
                anShipSpeed[ship] = anShipSpeed[0];
                aShipVelocity[ship] = aShipVelocity[0];
            }
            /* Enter the sphere from the arrival view, as house_keep does after
               manual flight: set_up_action_sphere explodes the ships it removes
               while they are on screen, so stale departure positions would
               destroy ships left far behind. */
            SwcSelectFlightView(nCameraViewMode);
            SwcTransformObjectsToView();
            ReleaseStaleNavTarget();
        }
        /* update_objective_location checks one objective per tick. */
        for (tick = 0; tick <= cMissionObjectiveCountValue && FlightContinues(); tick++)
            FlightStep(0);
        /* Wait there like the player: SdlSwcFlagReached holds the leg while an
           escorted ship is on its way, and that ship registers the arrival
           itself (cruise_to_destination's flag_reached(..., 1)). */
        for (tick = 0; waited < ESCORT_WAIT_TICKS && cCurrentNavPtIdx == previousLeg &&
             escorting_a_ship() && find_ship_index(anShipMissionShips[0]) != -1 &&
             FlightContinues(); tick++, waited++)
            FlightStep(0);
        if (cCurrentNavPtIdx == previousLeg) {
            /* An inactive nav point, objectives still hidden, or a late escort. */
            if (traceInputs)
                fprintf(stderr, "TRACE leg %d stops at objective %d (type %d, range %d) "
                        "after waiting %d ticks\n", previousLeg, abFlightPathList[previousLeg],
                        aMissionObjectivesArray[abFlightPathList[previousLeg]].type,
                        distance_from_point(0, &aMissionObjectivesArray[
                            abFlightPathList[previousLeg]].position), tick);
            break;
        }
    }
}

static int ActionIndex(unsigned char kind, SDL_Keycode key, short x)
{
    int index;

    for (index = 0; index < (int)SDL_arraysize(actions); index++) {
        if (actions[index].kind == kind && actions[index].key == key &&
            actions[index].x == x)
            return index;
    }
    Require(0, "seed action");
    return 0;
}

static void WriteSeed(const char *directory, int campaign, int series, int mission, int legs)
{
    char path[PATH_MAX];
    SDL_RWops *file;
    unsigned char seed[32] = {
        (unsigned char)campaign, (unsigned char)series, (unsigned char)mission,
        (unsigned char)legs, 0, 0, 0, 0
    };
    size_t size = 8;

    /* Throttle, steer while firing, aim and click, cycle weapons, fire a
     * missile, order the wingman, light the afterburner, then try autopilot. */
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_EQUALS, 0); seed[size++] = 31;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_UP, 0); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_SPACE, 0) | 0x40; seed[size++] = 31;
    seed[size++] = ActionIndex(ACTION_POINTER, 0, 160); seed[size++] = 7;
    seed[size++] = ActionIndex(ACTION_BUTTONS, 0, SDL_BUTTON_LMASK); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_g, 0); seed[size++] = 0;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_w, 0); seed[size++] = 0;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_RETURN, 0); seed[size++] = 7;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_b, 0); seed[size++] = 3;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_r, 0); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_TAB, 0); seed[size++] = 15;
    seed[size++] = ActionIndex(ACTION_KEY, SDLK_a, 0); seed[size++] = 31;
    SDL_snprintf(path, sizeof(path), "%s/%s%d-%d-%d-leg%d", directory,
                 campaign == 3 ? "simulator" : "swc", campaign, series, mission, legs);
    file = SDL_RWFromFile(path, "rb");
    if (file != NULL) {
        SDL_RWclose(file);
        return;
    }
    file = SDL_RWFromFile(path, "wb");
    Require(file != NULL && SDL_RWwrite(file, seed, size, 1) == 1 &&
            SDL_RWclose(file) == 0, "write SWC flight seed");
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    SDL_Window *window;
    SwcBuffer campaign = {0};
    unsigned char *savedCampaign = pMissionCampaignInfo;
    const char *directory = getenv("WC1_SWC_DATA");
    const char *seeds = getenv("WC1_SWC_FLIGHT_SEEDS");
    const char *ticks = getenv("WC1_FLIGHT_TICKS");
    char path[PATH_MAX];
    int set;
    int series;
    int mission;
    int legs;
    (void)argc;
    (void)argv;

    Require(atexit(CheckFuzzExit) == 0, "register early-exit check");
    inputActive = 1;
    traceInputs = getenv("WC1_SWC_FLIGHT_TRACE") != NULL;
    /* Changed keys, as the launcher passes them (src/sdl/keymap.c). */
    if (getenv("WC1_SWC_FLIGHT_KEYS") != NULL)
        SdlSetKeyBindings(getenv("WC1_SWC_FLIGHT_KEYS"));
    if (ticks)
        tickLimit = atoi(ticks);
    Require(tickLimit > 0 && tickLimit <= 3600, "tick budget in 1..3600");
    Require(directory != NULL && chdir(directory) == 0,
            "set WC1_SWC_DATA to an SWC directory containing CMFs");
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
    for (set = 0; set < 3; set++) {
        nCampaignDataSet = set;
        if (SdlLoadSwcCampaignData(fuzzMissionPath, &campaign) != 0) {
            SDL_ClearError();
            continue;
        }
        for (series = 1; series <= 13; series++) {
            missionCounts[set][series - 1] = pMissionCampaignInfo[(series - 1) * 90 + 2];
            for (mission = 0; seeds != NULL && mission < missionCounts[set][series - 1]; mission++)
                for (legs = 0; legs < 4; legs++)
                    WriteSeed(seeds, set, series, mission, legs);
        }
        pMissionCampaignInfo = savedCampaign;
    }
    SDL_free(campaign.data);
    for (mission = 0; seeds != NULL && mission < 4; mission++)
        WriteSeed(seeds, 3, 0, mission, 0);
    nCampaignDataSet = 0;
    inputActive = 0;
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    const FlightAction *action;
    int campaign;
    int series;
    int mission;
    int simulator;
    int shipCount;
    int object;
    int tick = 0;
    int hold;
    int frame;
    int startLeg;
    int reachedLeg;
    size_t position;
    unsigned int seed;

    if (size < 10)
        return -1;
    campaign = data[0];
    series = data[1];
    mission = data[2];
    simulator = campaign == 3;
    if (simulator) {
        if (mission > 3)
            return -1;
        series = 0;
    } else if (campaign > 2 || series < 1 || series > 13 ||
               mission >= missionCounts[campaign][series - 1]) {
        return -1;
    }
    inputActive = 1;
    seed = (unsigned int)data[4] | ((unsigned int)data[5] << 8) |
           ((unsigned int)data[6] << 16) | ((unsigned int)data[7] << 24);
    srand(seed);
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
    startLeg = cCurrentNavPtIdx;
    TravelFlightPath(data[3] & 15);
    reachedLeg = cCurrentNavPtIdx;
    for (position = 8; position + 1 < size && tick < tickLimit && FlightContinues();
         position += 2) {
        if (!(data[position] & 0x40))
            ReleaseFlightControls();
        action = &actions[(data[position] & 63) % SDL_arraysize(actions)];
        hold = (1 + (data[position + 1] & 63)) << (data[position + 1] >> 6);
        ApplyFlightAction(action);
        for (frame = 0; frame < hold && tick < tickLimit && FlightContinues(); frame++, tick++)
            FlightStep(1);
    }
    if (traceInputs)
        fprintf(stderr, "TRACE set %d series %d mission %d: nav %d -> %d (%d legs asked) "
                "-> %d, %d ticks, arcade state %d, destroyed %d, speed %d at %d %d %d\n", campaign,
                series, mission, startLeg, reachedLeg, data[3] & 15, cCurrentNavPtIdx, tick,
                nArcadeState, bCurrentPlayerDestroyed, anShipSpeed[0], aShipPositionsArray[0].x,
                aShipPositionsArray[0].y, aShipPositionsArray[0].z);
    SdlStopSwcSounds();
    ReleaseFlightControls();
    SwcResetFlightInput();
    if (IsCommChoiceMenuOpen())
        CloseCommChoiceMenu();
    nTrainSimActive = 0;
    inputActive = 0;
    return 0;
}
