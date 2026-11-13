/* Mapped controllers' analog triggers and View button through the real SDL
 * event and polling paths of WC1 and SWC, on an SDL virtual controller. No
 * game assets are needed. */
#include "wc1.h"

static SDL_Joystick *pad;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

/* Virtual controller buttons and axes; the triggers are half axes, 0 when
   released. */
enum { PAD_A = 0, PAD_BACK = 4 };
enum { PAD_LEFT_TRIGGER = 4, PAD_RIGHT_TRIGGER = 5 };

static void SetAxis(int axis, Sint16 value)
{
    Require(SDL_JoystickSetVirtualAxis(pad, axis, value) == 0, "move a virtual axis");
    SDL_JoystickUpdate();
    SdlPumpEvents();
}

static void SetButton(int button, int pressed)
{
    Require(SDL_JoystickSetVirtualButton(pad, button, pressed) == 0, "press a virtual button");
    SDL_JoystickUpdate();
    SdlPumpEvents();
}

static unsigned int PolledButtons(void)
{
    JOYINFO information;

    Require(SdlReadJoystick(0, &information), "poll the controller as WC1's joystick");
    return information.wButtons;
}

/* The key presses queued for the game since the last call. */
static int QueuedPress(short value)
{
    InputEvent *event;
    int found = 0;

    for (event = pInputEventHead; event != NULL; event = event->next) {
        if (event->type == INPUT_EVENT_KEY_DOWN && event->value == value)
            found = 1;
    }
    FlushInputEvents();
    return found;
}

static int AnyQueuedPress(void)
{
    InputEvent *event;
    int found = 0;

    for (event = pInputEventHead; event != NULL; event = event->next)
        found |= event->type == INPUT_EVENT_KEY_DOWN;
    FlushInputEvents();
    return found;
}

static SDL_Event SwcAxis(SDL_JoystickID instance, Uint8 axis, Sint16 value, int flight)
{
    SDL_Event event = {0};

    event.type = SDL_CONTROLLERAXISMOTION;
    event.caxis.which = instance;
    event.caxis.axis = axis;
    event.caxis.value = value;
    SdlTranslateSwcJoystickEvent(&event, flight);
    return event;
}

static SDL_Event SwcButton(SDL_JoystickID instance, Uint8 button, int pressed, int flight)
{
    SDL_Event event = {0};

    event.type = pressed ? SDL_CONTROLLERBUTTONDOWN : SDL_CONTROLLERBUTTONUP;
    event.cbutton.which = instance;
    event.cbutton.button = button;
    event.cbutton.state = pressed ? SDL_PRESSED : SDL_RELEASED;
    SdlTranslateSwcJoystickEvent(&event, flight);
    return event;
}

static int IsKey(const SDL_Event *event, Uint32 type, SDL_Keycode key)
{
    return event->type == type && event->key.keysym.sym == key;
}

int main(int argc, char **argv)
{
    static const struct {
        int view;
        unsigned short key;
    } wc1Views[] = {
        {0, SCAN_F2}, {2, SCAN_F3}, {1, SCAN_F4}, {3, SCAN_F5}, {4, SCAN_F1}, {14, SCAN_F1}
    };
    static const struct {
        int view;
        SDL_Keycode key;
    } swcViews[] = {{0, SDLK_F2}, {14, SDLK_F3}, {4, SDLK_F1}};
    InputDeviceSample sample;
    SDL_JoystickID instance;
    SDL_Event event;
    char guid[33];
    char mapping[512];
    size_t index;
    int device;

    (void)argc;
    (void)argv;
    SDL_setenv("SDL_VIDEODRIVER", "dummy", 1);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    Require(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == 0,
            "initialize headless SDL input");
    nEventManagerActive = 1;
    bInputEventPoolInitialized = 1;
    device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 6, 15, 0);
    Require(device >= 0, "attach a virtual controller");
    SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(device), guid, sizeof(guid));
    SDL_snprintf(mapping, sizeof(mapping),
        "%s,Gamepad probe,a:b0,b:b1,x:b2,y:b3,back:b4,guide:b5,start:b6,leftstick:b7,"
        "rightstick:b8,leftshoulder:b9,rightshoulder:b10,dpup:b11,dpdown:b12,dpleft:b13,"
        "dpright:b14,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:+a4,righttrigger:+a5,",
        guid);
    Require(SDL_GameControllerAddMapping(mapping) >= 0 && SDL_IsGameController(device),
            "map the virtual controller with analog triggers");
    pad = SDL_JoystickOpen(device);
    Require(pad != NULL, "open the virtual controller");
    instance = SDL_JoystickInstanceID(pad);
    SdlPumpEvents();
    nCurrentActiveInputDevice = 0;
    Require(SdlSetJoystickMode("4button-4axis"), "four-button, four-axis layout");

    /* WC1 polling: in flight RT is button 1, with hysteresis; not elsewhere. */
    SetEventManagerPump(get_player_input);
    Require(PolledButtons() == 0, "released triggers press nothing");
    SetAxis(PAD_RIGHT_TRIGGER, 32767);
    Require(PolledButtons() == JOY_BUTTON1, "RT fires the guns in flight");
    SetAxis(PAD_RIGHT_TRIGGER, 12000);
    Require(PolledButtons() == JOY_BUTTON1, "a trigger stays pressed until below a quarter");
    SetAxis(PAD_RIGHT_TRIGGER, 4000);
    Require(PolledButtons() == 0, "a trigger releases below a quarter");
    SetAxis(PAD_RIGHT_TRIGGER, 12000);
    Require(PolledButtons() == 0, "and presses again only past half travel");
    SetEventManagerPump(PollMenuInputDevices);
    SetAxis(PAD_RIGHT_TRIGGER, 32767);
    Require(PolledButtons() == 0, "outside flight the triggers do nothing");
    SetButton(PAD_A, 1);
    Require(PolledButtons() == JOY_BUTTON1, "A still clicks in menus");
    SetButton(PAD_A, 0);
    SetAxis(PAD_RIGHT_TRIGGER, 0);
    Require(SdlSetJoystickMode("original"), "original layout");
    SetEventManagerPump(get_player_input);
    SetAxis(PAD_LEFT_TRIGGER, 32767);
    Require(PolledButtons() == JOY_BUTTON2, "in the original layout LT is button 2");
    SetAxis(PAD_LEFT_TRIGGER, 0);
    Require(SdlSetJoystickMode("4button-4axis"), "four-button, four-axis layout again");

    /* WC1 events in flight: LT fires a missile (Return); RT queues nothing,
       as A does; View steps through F1-F5 without toggling the cockpit. */
    SdlApplyJoystickFlightControls();
    nArcadeState = 0;
    FlushInputEvents();
    SetAxis(PAD_LEFT_TRIGGER, 32767);
    Require(QueuedPress(SCAN_RETURN), "LT fires a missile in flight");
    SetAxis(PAD_LEFT_TRIGGER, 0);
    Require(!AnyQueuedPress(), "releasing LT presses nothing");
    SetAxis(PAD_RIGHT_TRIGGER, 32767);
    SetAxis(PAD_RIGHT_TRIGGER, 0);
    Require(!AnyQueuedPress(), "RT's guns are polled, like A's");
    for (index = 0; index < SDL_arraysize(wc1Views); index++) {
        nCameraViewMode = wc1Views[index].view;
        bF1KeyLatch = 0;
        SetButton(PAD_BACK, 1);
        SetButton(PAD_BACK, 0);
        Require(QueuedPress(wc1Views[index].key) &&
                bF1KeyLatch == (wc1Views[index].key == SCAN_F1),
                "View steps front, left, right, rear, chase, then front");
    }
    Require(!bEscapePressedFlag, "View is not Escape in flight");
    SetEventManagerPump(PollMenuInputDevices);
    SetButton(PAD_BACK, 1);
    SetButton(PAD_BACK, 0);
    Require(QueuedPress(SCAN_ESCAPE) && bEscapePressedFlag, "View is Escape outside flight");
    bEscapePressedFlag = 0;
    SetEventManagerPump(get_player_input);
    SdlSetJoystickRetryMode(1);
    SetAxis(PAD_LEFT_TRIGGER, 32767);
    Require(!AnyQueuedPress(), "the retry prompt takes no triggers");
    SdlSetJoystickRetryMode(0);
    SetAxis(PAD_LEFT_TRIGGER, 0);
    FlushInputEvents();

    /* SWC: the host translates trigger events in flight only, and polls
       them into its flight sample. */
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 32767, 1);
    Require(IsKey(&event, SDL_KEYDOWN, SDLK_SPACE), "SWC: RT fires the guns (Space)");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 0, 1);
    Require(IsKey(&event, SDL_KEYUP, SDLK_SPACE), "SWC: releasing RT releases Space");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERLEFT, 32767, 1);
    Require(IsKey(&event, SDL_KEYDOWN, SDLK_RETURN), "SWC: LT fires a missile (Return)");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERLEFT, 20000, 1);
    Require(event.type == 0, "SWC: no repeated press while LT stays down");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERLEFT, 0, 1);
    Require(IsKey(&event, SDL_KEYUP, SDLK_RETURN), "SWC: releasing LT releases Return");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 32767, 0);
    Require(event.type == 0, "SWC: outside flight the triggers do nothing");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 0, 0);
    Require(event.type == 0, "SWC: and their release is consumed too");
    event = SwcAxis(instance, SDL_CONTROLLER_AXIS_LEFTX, 32767, 1);
    Require(event.type == SDL_CONTROLLERAXISMOTION, "SWC: the sticks are left for polling");
    for (index = 0; index < SDL_arraysize(swcViews); index++) {
        nCameraViewMode = swcViews[index].view;
        event = SwcButton(instance, SDL_CONTROLLER_BUTTON_BACK, 1, 1);
        Require(IsKey(&event, SDL_KEYDOWN, swcViews[index].key),
                "SWC: View steps cockpit, battle, chase, then cockpit");
        event = SwcButton(instance, SDL_CONTROLLER_BUTTON_BACK, 0, 1);
        Require(event.type == 0, "SWC: releasing View sends nothing");
    }
    event = SwcButton(instance, SDL_CONTROLLER_BUTTON_BACK, 1, 0);
    Require(IsKey(&event, SDL_KEYDOWN, SDLK_ESCAPE), "SWC: View is Escape outside flight");
    SetAxis(PAD_RIGHT_TRIGGER, 32767);
    SetAxis(PAD_LEFT_TRIGGER, 32767);
    Require(SdlReadSwcJoystickFlightSample(&sample) &&
            (sample.buttons & (JOY_BUTTON1 | JOY_BUTTON2)) == (JOY_BUTTON1 | JOY_BUTTON2),
            "SWC: held triggers are buttons 1 and 2 in the flight sample");
    SetAxis(PAD_RIGHT_TRIGGER, 0);
    SetAxis(PAD_LEFT_TRIGGER, 0);
    Require(SdlReadSwcJoystickFlightSample(&sample) && sample.buttons == 0,
            "SWC: released triggers leave the sample");

    SdlShutdownJoysticks();
    SDL_JoystickClose(pad);
    Require(SDL_JoystickDetachVirtual(device) == 0, "detach the virtual controller");
    SDL_Quit();
    puts("PASS: controller triggers fire in WC1 and SWC flight, and View cycles the views.");
    return 0;
}
