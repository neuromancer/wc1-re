#include "wc1.h"

typedef struct SdlJoystickDevice {
    SDL_GameController *controller;
    SDL_Joystick *joystick;
    SDL_JoystickID instanceId;
    Uint8 hatState;
    int rumbleSupport; /* -1 unavailable, 0 untested, 1 available */
} SdlJoystickDevice;

typedef enum SdlJoystickMode {
    SDL_PORT_JOYSTICK_ORIGINAL,
    SDL_PORT_JOYSTICK_FOUR_BUTTON_TWO_AXIS,
    SDL_PORT_JOYSTICK_FOUR_BUTTON_FOUR_AXIS
} SdlJoystickMode;

typedef enum SdlJoystickAxesMode {
    SDL_PORT_JOYSTICK_AXES_TWIN_STICK_ROLL,
    SDL_PORT_JOYSTICK_AXES_TWIN_STICK_YAW,
    SDL_PORT_JOYSTICK_AXES_HOTAS_YAW,
    SDL_PORT_JOYSTICK_AXES_HOTAS_ROLL,
    SDL_PORT_JOYSTICK_AXES_LINEAR_THROTTLE,
    SDL_PORT_JOYSTICK_AXES_RUDDER_YAW,
    SDL_PORT_JOYSTICK_AXES_RUDDER_ROLL
} SdlJoystickAxesMode;

typedef enum SdlJoystickButton {
    SDL_PORT_JOYSTICK_BUTTON_A,
    SDL_PORT_JOYSTICK_BUTTON_B,
    SDL_PORT_JOYSTICK_BUTTON_X,
    SDL_PORT_JOYSTICK_BUTTON_Y,
    SDL_PORT_JOYSTICK_BUTTON_LEFT_SHOULDER,
    SDL_PORT_JOYSTICK_BUTTON_RIGHT_SHOULDER,
    SDL_PORT_JOYSTICK_BUTTON_LEFT_TRIGGER,
    SDL_PORT_JOYSTICK_BUTTON_RIGHT_TRIGGER,
    SDL_PORT_JOYSTICK_BUTTON_BACK = 8,
    SDL_PORT_JOYSTICK_BUTTON_START,
    SDL_PORT_JOYSTICK_BUTTON_LEFT_STICK,
    SDL_PORT_JOYSTICK_BUTTON_RIGHT_STICK,
    SDL_PORT_JOYSTICK_BUTTON_DPAD_UP,
    SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN,
    SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT,
    SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT
} SdlJoystickButton;

static SdlJoystickDevice g_aSdlJoystickDevices[2];
static int g_bSdlJoystickDebug;
static int g_bSdlJoystickInputStarted;
static int g_bSdlJoystickRumbleEnabled;
static int g_bSdlJoystickSpaceflightActive;
static int g_bSdlTwoAxisModifierActive;
static int g_nSdlCommunicationMenuSelection;
static int g_nSdlRumblingJoystick = -1;
static Uint16 g_wSdlRumbleLow;
static Uint16 g_wSdlRumbleHigh;
static Uint32 g_dwSdlRumbleDeadline;
static SdlJoystickMode g_eSdlJoystickMode;
static SdlJoystickAxesMode g_eSdlJoystickAxesMode;

int SdlSetJoystickMode(const char *name)
{
    if (strcmp(name, "original") == 0)
        g_eSdlJoystickMode = SDL_PORT_JOYSTICK_ORIGINAL;
    else if (strcmp(name, "4button-2axis") == 0)
        g_eSdlJoystickMode = SDL_PORT_JOYSTICK_FOUR_BUTTON_TWO_AXIS;
    else if (strcmp(name, "4button-4axis") == 0)
        g_eSdlJoystickMode = SDL_PORT_JOYSTICK_FOUR_BUTTON_FOUR_AXIS;
    else
        return 0;
    return 1;
}

int SdlSetJoystickAxesMode(const char *name)
{
    if (strcmp(name, "twin-stick-roll") == 0)
        g_eSdlJoystickAxesMode =
            SDL_PORT_JOYSTICK_AXES_TWIN_STICK_ROLL;
    else if (strcmp(name, "twin-stick-yaw") == 0)
        g_eSdlJoystickAxesMode =
            SDL_PORT_JOYSTICK_AXES_TWIN_STICK_YAW;
    else if (strcmp(name, "hotas-yaw") == 0)
        g_eSdlJoystickAxesMode = SDL_PORT_JOYSTICK_AXES_HOTAS_YAW;
    else if (strcmp(name, "hotas-roll") == 0)
        g_eSdlJoystickAxesMode = SDL_PORT_JOYSTICK_AXES_HOTAS_ROLL;
    else if (strcmp(name, "linear-throttle") == 0)
        g_eSdlJoystickAxesMode =
            SDL_PORT_JOYSTICK_AXES_LINEAR_THROTTLE;
    else if (strcmp(name, "rudder-yaw") == 0)
        g_eSdlJoystickAxesMode = SDL_PORT_JOYSTICK_AXES_RUDDER_YAW;
    else if (strcmp(name, "rudder-roll") == 0)
        g_eSdlJoystickAxesMode = SDL_PORT_JOYSTICK_AXES_RUDDER_ROLL;
    else
        return 0;
    return 1;
}

void SdlEnableJoystickDebug(void)
{
    g_bSdlJoystickDebug = 1;
}

int SdlJoystickHasEnhancedControls(void)
{
    return g_eSdlJoystickMode != SDL_PORT_JOYSTICK_ORIGINAL;
}

void SdlEnableJoystickRumble(void)
{
    g_bSdlJoystickRumbleEnabled = 1;
}

static void SdlStopJoystickRumble(void)
{
    SdlJoystickDevice *device;

    if (g_nSdlRumblingJoystick < 0 ||
        g_nSdlRumblingJoystick >= 2)
        return;
    device = &g_aSdlJoystickDevices[g_nSdlRumblingJoystick];
    if (device->joystick != 0 && device->rumbleSupport > 0)
        SDL_JoystickRumble(device->joystick, 0, 0, 0);
    g_nSdlRumblingJoystick = -1;
}

static void SdlSendJoystickRumble(int slot, Uint16 low, Uint16 high)
{
    SdlJoystickDevice *device;

    device = &g_aSdlJoystickDevices[slot];
    if (device->joystick == 0 || device->rumbleSupport < 0)
        return;
    /* Brief renewals expire safely if a pause or modal dialog stops updates. */
    if (SDL_JoystickRumble(device->joystick, low, high, 100) != 0) {
        device->rumbleSupport = -1;
        if (g_bSdlJoystickDebug) {
            printf("SDL joystick slot %d does not support rumble: %s\n",
                   slot, SDL_GetError());
            fflush(stdout);
        }
        SDL_ClearError();
        return;
    }
    if (device->rumbleSupport == 0 && g_bSdlJoystickDebug) {
        printf("SDL joystick slot %d rumble enabled\n", slot);
        fflush(stdout);
    }
    device->rumbleSupport = 1;
    g_nSdlRumblingJoystick = slot;
}

static void SdlUpdateJoystickRumble(void)
{
    Uint16 low;
    Uint16 high;
    Uint32 now;
    int activeSlot;

    if (!g_bSdlJoystickRumbleEnabled)
        return;
    activeSlot = (int)nCurrentActiveInputDevice;
    if (!g_bSdlJoystickSpaceflightActive || activeSlot < 0 ||
        activeSlot >= 2 ||
        g_aSdlJoystickDevices[activeSlot].joystick == 0) {
        SdlStopJoystickRumble();
        return;
    }
    if (g_nSdlRumblingJoystick != -1 &&
        g_nSdlRumblingJoystick != activeSlot)
        SdlStopJoystickRumble();

    low = 0;
    high = 0;
    if (anShipFuelsTable[0] > 0 &&
        aeSpecialManeuvers[0] ==
            SPECIAL_MANEUVER_AFTERBURNER &&
        asShipAfterburnerTimer[0] > 0) {
        low = 0x6000;
        high = 0x3000;
    }

    now = SDL_GetTicks();
    if (g_dwSdlRumbleDeadline != 0 &&
        !SDL_TICKS_PASSED(now, g_dwSdlRumbleDeadline)) {
        if (low < g_wSdlRumbleLow)
            low = g_wSdlRumbleLow;
        if (high < g_wSdlRumbleHigh)
            high = g_wSdlRumbleHigh;
    } else {
        g_wSdlRumbleLow = 0;
        g_wSdlRumbleHigh = 0;
        g_dwSdlRumbleDeadline = 0;
    }

    if (low == 0 && high == 0) {
        SdlStopJoystickRumble();
        return;
    }
    SdlSendJoystickRumble(activeSlot, low, high);
}

static void SdlQueueJoystickRumble(Uint16 low, Uint16 high,
                                      Uint32 duration)
{
    Uint32 deadline;
    Uint32 now;

    if (!g_bSdlJoystickRumbleEnabled)
        return;

    now = SDL_GetTicks();
    deadline = now + duration;
    if (deadline == 0)
        deadline = 1;
    if (g_dwSdlRumbleDeadline == 0 ||
        SDL_TICKS_PASSED(now, g_dwSdlRumbleDeadline)) {
        g_wSdlRumbleLow = low;
        g_wSdlRumbleHigh = high;
        g_dwSdlRumbleDeadline = deadline;
    } else {
        if (g_wSdlRumbleLow < low)
            g_wSdlRumbleLow = low;
        if (g_wSdlRumbleHigh < high)
            g_wSdlRumbleHigh = high;
        if (SDL_TICKS_PASSED(deadline, g_dwSdlRumbleDeadline))
            g_dwSdlRumbleDeadline = deadline;
    }
    SdlUpdateJoystickRumble();
}

void SdlQueueJoystickWeaponRumble(int weaponType)
{
    Uint16 low;
    Uint16 high;
    Uint32 duration;

    /* Lasers fire continuously, so feedback here quickly becomes noise. */
    switch ((enum ObjectType)weaponType) {
    case OBJECT_TYPE_LASER_CANNON:
        return;
    case OBJECT_TYPE_NEUTRON_PARTICLE_GUN:
        low = 0x6000;
        high = 0x7800;
        duration = 75;
        break;
    case OBJECT_TYPE_MASS_DRIVER_CANNON:
    case OBJECT_TYPE_TURRET:
        low = 0x7800;
        high = 0x6000;
        duration = 90;
        break;
    case OBJECT_TYPE_DUMB_FIRE_MISSILE:
        low = 0x9000;
        high = 0x6000;
        duration = 150;
        break;
    case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
        low = 0x8800;
        high = 0x5800;
        duration = 170;
        break;
    case OBJECT_TYPE_FF_MISSILE:
        low = 0x8000;
        high = 0x6800;
        duration = 180;
        break;
    case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
        low = 0x9000;
        high = 0x5800;
        duration = 170;
        break;
    case OBJECT_TYPE_TORPEDO:
        low = 0xa000;
        high = 0x6800;
        duration = 200;
        break;
    default:
        return;
    }
    SdlQueueJoystickRumble(low, high, duration);
}

void SdlQueueJoystickDamageRumble(int damage)
{
    if (damage <= 0)
        return;
    if (damage < 16) {
        SdlQueueJoystickRumble(0x5800, 0x7000, 90);
    } else if (damage < 64) {
        SdlQueueJoystickRumble(0x7800, 0x8000, 130);
    } else {
        SdlQueueJoystickRumble(0xa000, 0x9000, 180);
    }
}

void SdlQueueJoystickCollisionRumble(int collisionSpeed)
{
    if (collisionSpeed <= 0)
        return;
    if (collisionSpeed < 10) {
        SdlQueueJoystickRumble(0x6800, 0x5000, 110);
    } else if (collisionSpeed < 30) {
        SdlQueueJoystickRumble(0x8800, 0x6800, 160);
    } else {
        SdlQueueJoystickRumble(0xb000, 0x7800, 220);
    }
}

void SdlLogJoystickEvent(const SDL_Event *event)
{
    const char *name;

    if (!g_bSdlJoystickDebug)
        return;

    switch (event->type) {
    case SDL_JOYAXISMOTION:
        printf("SDL joystick %d axis %u = %d\n",
               (int)event->jaxis.which, (unsigned int)event->jaxis.axis,
               (int)event->jaxis.value);
        break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
        printf("SDL joystick %d button %u %s\n",
               (int)event->jbutton.which,
               (unsigned int)event->jbutton.button,
               event->type == SDL_JOYBUTTONDOWN ? "down" : "up");
        break;
    case SDL_JOYHATMOTION:
        printf("SDL joystick %d hat %u = 0x%02x\n",
               (int)event->jhat.which, (unsigned int)event->jhat.hat,
               (unsigned int)event->jhat.value);
        break;
    case SDL_CONTROLLERAXISMOTION:
        name = SDL_GameControllerGetStringForAxis(
            (SDL_GameControllerAxis)event->caxis.axis);
        printf("SDL controller %d axis %s (%u) = %d\n",
               (int)event->caxis.which, name != 0 ? name : "unknown",
               (unsigned int)event->caxis.axis,
               (int)event->caxis.value);
        break;
    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        name = SDL_GameControllerGetStringForButton(
            (SDL_GameControllerButton)event->cbutton.button);
        printf("SDL controller %d button %s (%u) %s\n",
               (int)event->cbutton.which,
               name != 0 ? name : "unknown",
               (unsigned int)event->cbutton.button,
               event->type == SDL_CONTROLLERBUTTONDOWN ? "down" : "up");
        break;
    default:
        return;
    }
    fflush(stdout);
}

static void SdlCloseJoystick(SdlJoystickDevice *device)
{
    if (device->joystick != 0 && device->rumbleSupport > 0)
        SDL_JoystickRumble(device->joystick, 0, 0, 0);
    if (g_nSdlRumblingJoystick >= 0 &&
        device == &g_aSdlJoystickDevices[
                      g_nSdlRumblingJoystick])
        g_nSdlRumblingJoystick = -1;
    if (device->controller != 0)
        SDL_GameControllerClose(device->controller);
    else if (device->joystick != 0)
        SDL_JoystickClose(device->joystick);
    device->controller = 0;
    device->joystick = 0;
    device->instanceId = -1;
    device->hatState = SDL_HAT_CENTERED;
    device->rumbleSupport = 0;
}

static int SdlFindJoystick(SDL_JoystickID instanceId)
{
    int slot;

    slot = 0;
    while (slot < 2) {
        if (g_aSdlJoystickDevices[slot].joystick != 0 &&
            g_aSdlJoystickDevices[slot].instanceId == instanceId)
            return slot;
        slot++;
    }
    return -1;
}

static int SdlFindFreeJoystickSlot(void)
{
    int slot;

    slot = 0;
    while (slot < 2) {
        if (g_aSdlJoystickDevices[slot].joystick == 0)
            return slot;
        slot++;
    }
    return -1;
}

static void SdlOpenJoystick(int deviceIndex)
{
    SDL_GameController *controller;
    SDL_Joystick *joystick;
    SDL_JoystickID instanceId;
    SdlJoystickDevice *device;
    int slot;

    instanceId = SDL_JoystickGetDeviceInstanceID(deviceIndex);
    if (instanceId < 0 || SdlFindJoystick(instanceId) != -1)
        return;
    slot = SdlFindFreeJoystickSlot();
    if (slot == -1)
        return;

    controller = 0;
    joystick = 0;
    if (SDL_IsGameController(deviceIndex)) {
        controller = SDL_GameControllerOpen(deviceIndex);
        if (controller != 0)
            joystick = SDL_GameControllerGetJoystick(controller);
    }
    if (joystick == 0) {
        if (controller != 0)
            SDL_GameControllerClose(controller);
        controller = 0;
        joystick = SDL_JoystickOpen(deviceIndex);
    }
    if (joystick == 0)
        return;

    device = &g_aSdlJoystickDevices[slot];
    device->controller = controller;
    device->joystick = joystick;
    device->instanceId = SDL_JoystickInstanceID(joystick);
    device->hatState = SDL_HAT_CENTERED;
    device->rumbleSupport = 0;
#if SDL_VERSION_ATLEAST(2, 0, 18)
    if (g_bSdlJoystickRumbleEnabled) {
        if (SDL_JoystickHasRumble(joystick) == SDL_FALSE)
            device->rumbleSupport = -1;
        printf("SDL joystick slot %d %s rumble support\n", slot,
               device->rumbleSupport < 0 ? "does not expose" : "exposes");
        fflush(stdout);
    }
#endif
    if (g_bSdlJoystickDebug) {
        const char *name = SDL_JoystickName(joystick);

        printf("SDL joystick slot %d opened: %s, instance %d, %s, "
               "%d axes, %d buttons, %d hats\n",
               slot, name != 0 ? name : "unknown",
               (int)device->instanceId,
               controller != 0 ? "mapped controller" : "raw joystick",
               SDL_JoystickNumAxes(joystick),
               SDL_JoystickNumButtons(joystick),
               SDL_JoystickNumHats(joystick));
        fflush(stdout);
    }
}

static void SdlRefreshJoysticks(void)
{
    SdlJoystickDevice *device;
    int deviceIndex;
    int slot;

    slot = 0;
    while (slot < 2) {
        device = &g_aSdlJoystickDevices[slot];
        if (device->joystick != 0 &&
            SDL_JoystickGetAttached(device->joystick) == SDL_FALSE)
            SdlCloseJoystick(device);
        slot++;
    }

    deviceIndex = 0;
    while (deviceIndex < SDL_NumJoysticks() &&
           SdlFindFreeJoystickSlot() != -1) {
        SdlOpenJoystick(deviceIndex);
        deviceIndex++;
    }
}

static void SdlSelectSwcJoystick(void)
{
    int slot = (int)nCurrentActiveInputDevice;

    if (slot >= 0 && slot < 2 && g_aSdlJoystickDevices[slot].joystick != NULL)
        return;
    nCurrentActiveInputDevice = -1;
    for (slot = 0; slot < 2; slot++) {
        if (g_aSdlJoystickDevices[slot].joystick != NULL) {
            nCurrentActiveInputDevice = (short)slot;
            break;
        }
    }
}

static Sint16 SdlReadJoystickAxis(
    const SdlJoystickDevice *device, int axis)
{
    SDL_GameControllerAxis controllerAxis;

    if (device->joystick == 0)
        return 0;
    if (device->controller != 0) {
        switch (axis) {
        case 0:
            controllerAxis = SDL_CONTROLLER_AXIS_LEFTX;
            break;
        case 1:
            controllerAxis = SDL_CONTROLLER_AXIS_LEFTY;
            break;
        case 2:
            controllerAxis = SDL_CONTROLLER_AXIS_RIGHTX;
            break;
        case 3:
            controllerAxis = SDL_CONTROLLER_AXIS_RIGHTY;
            break;
        default:
            return 0;
        }
        return SDL_GameControllerGetAxis(device->controller,
                                         controllerAxis);
    }
    if (SDL_JoystickNumAxes(device->joystick) <= axis)
        return 0;
    return SDL_JoystickGetAxis(device->joystick, axis);
}

static int SdlReadJoystickButton(
    const SdlJoystickDevice *device, int button)
{
    SDL_GameControllerButton controllerButton;

    if (device->joystick == 0)
        return 0;
    if (device->controller != 0) {
        switch (button) {
        case 0:
            controllerButton = SDL_CONTROLLER_BUTTON_A;
            break;
        case 1:
            controllerButton = SDL_CONTROLLER_BUTTON_B;
            break;
        case 2:
            controllerButton = SDL_CONTROLLER_BUTTON_X;
            break;
        case 3:
            controllerButton = SDL_CONTROLLER_BUTTON_Y;
            break;
        default:
            return 0;
        }
        return SDL_GameControllerGetButton(device->controller,
                                           controllerButton) != 0;
    }
    if (SDL_JoystickNumButtons(device->joystick) <= button)
        return 0;
    return SDL_JoystickGetButton(device->joystick, button) != 0;
}

BOOL SdlReadJoystick(unsigned int deviceIndex, JOYINFO *information)
{
    SdlJoystickDevice *device;
    unsigned int buttonState;
    int buttonIndex;

    if (deviceIndex >= 2)
        return FALSE;
    g_bSdlJoystickInputStarted = 1;
    SDL_JoystickUpdate();
    SdlRefreshJoysticks();
    device = &g_aSdlJoystickDevices[deviceIndex];
    if (device->joystick == 0)
        return FALSE;

    information->wXpos =
        (unsigned int)((int)SdlReadJoystickAxis(device, 0) + 32768);
    information->wYpos =
        (unsigned int)((int)SdlReadJoystickAxis(device, 1) + 32768);
    information->wZpos = 0;
    buttonState = 0;
    if (pCurrentEventManagerPump == get_player_input &&
        g_eSdlJoystickMode != SDL_PORT_JOYSTICK_ORIGINAL) {
        if (SdlReadJoystickButton(device, 0))
            buttonState = 1;
    } else {
        buttonIndex = 0;
        while (buttonIndex < 2) {
            if (SdlReadJoystickButton(device, buttonIndex))
                buttonState |= 1U << buttonIndex;
            buttonIndex++;
        }
    }
    if (deviceIndex != 0)
        buttonState <<= 2;
    information->wButtons = buttonState;
    return TRUE;
}

static int SdlControllerHasAxis(
    const SdlJoystickDevice *device, SDL_GameControllerAxis axis)
{
    SDL_GameControllerButtonBind binding;

    binding = SDL_GameControllerGetBindForAxis(device->controller, axis);
    return binding.bindType != SDL_CONTROLLER_BINDTYPE_NONE;
}

static int SdlReadExtraJoystickAxes(Sint16 *first, Sint16 *second)
{
    SdlJoystickDevice *device;
    SdlJoystickDevice *other;
    int activeSlot;

    activeSlot = (int)nCurrentActiveInputDevice;
    if (activeSlot < 0 || activeSlot >= 2)
        return 0;
    device = &g_aSdlJoystickDevices[activeSlot];
    if (device->joystick == 0)
        return 0;

    if (device->controller != 0) {
        if (SdlControllerHasAxis(
                device, SDL_CONTROLLER_AXIS_RIGHTX)) {
            *first = SdlReadJoystickAxis(device, 2);
            if (SdlControllerHasAxis(
                    device, SDL_CONTROLLER_AXIS_RIGHTY)) {
                *second = SdlReadJoystickAxis(device, 3);
                return 2;
            }
            return 1;
        }
    } else if (SDL_JoystickNumAxes(device->joystick) >= 3) {
        *first = SdlReadJoystickAxis(device, 2);
        if (SDL_JoystickNumAxes(device->joystick) >= 4) {
            *second = SdlReadJoystickAxis(device, 3);
            return 2;
        }
        return 1;
    }

    other = &g_aSdlJoystickDevices[activeSlot == 0 ? 1 : 0];
    if (other->joystick == 0)
        return 0;
    *first = SdlReadJoystickAxis(other, 0);
    *second = SdlReadJoystickAxis(other, 1);
    return 2;
}

static short SdlNormalizeFlightAxis(Sint16 raw)
{
    int magnitude;
    int normalized;
    int value;

    value = (int)raw;
    magnitude = value < 0 ? -value : value;
    if (magnitude <= 8192)
        return 0;
    normalized = (magnitude - 8192) * 9 / (32767 - 8192);
    if (normalized > 9)
        normalized = 9;
    return (short)(value < 0 ? -normalized : normalized);
}

int SdlReadSwcJoystickFlightSample(InputDeviceSample *sample)
{
    SdlJoystickDevice *device;
    int button;

    SDL_zero(*sample);
    SDL_JoystickUpdate();
    SdlRefreshJoysticks();
    SdlSelectSwcJoystick();
    if (nCurrentActiveInputDevice < 0)
        return 0;
    device = &g_aSdlJoystickDevices[nCurrentActiveInputDevice];
    sample->x = SdlNormalizeFlightAxis(SdlReadJoystickAxis(device, 0));
    sample->y = SdlNormalizeFlightAxis(SdlReadJoystickAxis(device, 1));
    for (button = 0; button < 4; button++) {
        if (SdlReadJoystickButton(device, button))
            sample->buttons |= 1U << button;
    }
    /* The shared enhanced axis layouts consume this normalized sample.
     * SDL axes already have known ranges; no legacy j.cal file is needed. */
    aInputDevSamples[nCurrentActiveInputDevice] = *sample;
    return 1;
}

static void SdlApplyLinearThrottle(Sint16 raw)
{
    int maximumSpeed;
    int targetSpeed;
    unsigned int position;

    maximumSpeed = (int)asShipMaximumSpeedArray[0] << 8;
    position = (unsigned int)((int)raw + 32768);
    targetSpeed = (int)(((int64_t)(65535U - position) * maximumSpeed) /
                        65535);
    celerate(0, targetSpeed - anShipSpeed[0]);
}

void SdlApplyJoystickFlightControls(void)
{
    InputDeviceSample *sample;
    Sint16 firstRaw;
    Sint16 secondRaw;
    short first;
    short second;
    int activeSlot;
    int axisCount;

    g_bSdlJoystickSpaceflightActive = 1;
    if ((pCurrentEventManagerPump != get_player_input && !SdlSwcFlightActive()) ||
        g_eSdlJoystickMode == SDL_PORT_JOYSTICK_ORIGINAL) {
        SdlUpdateJoystickRumble();
        return;
    }
    activeSlot = (int)nCurrentActiveInputDevice;
    if (activeSlot < 0 || activeSlot >= 2) {
        SdlUpdateJoystickRumble();
        return;
    }
    sample = &aInputDevSamples[activeSlot];
    if (SdlReadJoystickButton(
            &g_aSdlJoystickDevices[activeSlot], 2))
        your_afterburner();
    SdlUpdateJoystickRumble();

    if (g_eSdlJoystickMode ==
            SDL_PORT_JOYSTICK_FOUR_BUTTON_TWO_AXIS) {
        if (SdlReadJoystickButton(
                &g_aSdlJoystickDevices[activeSlot], 3)) {
            g_bSdlTwoAxisModifierActive = 1;
            stRotationInput.y = 0;
            stRotationInput.x = 0;
            stRotationInput.z = (short)sample->x;
            accelerate((short)-(sample->y / 2));
        } else if (g_bSdlTwoAxisModifierActive) {
            g_bSdlTwoAxisModifierActive = 0;
            stRotationInput.z = 0;
            stRotationInput.y = (short)sample->x;
            stRotationInput.x = (short)-sample->y;
        }
        return;
    }

    axisCount = SdlReadExtraJoystickAxes(&firstRaw, &secondRaw);
    if (axisCount == 0)
        return;
    first = SdlNormalizeFlightAxis(firstRaw);
    second = axisCount > 1 ? SdlNormalizeFlightAxis(secondRaw) : 0;

    switch (g_eSdlJoystickAxesMode) {
    case SDL_PORT_JOYSTICK_AXES_TWIN_STICK_ROLL:
        stRotationInput.z = first;
        if (axisCount > 1)
            accelerate((short)-(second / 2));
        break;
    case SDL_PORT_JOYSTICK_AXES_TWIN_STICK_YAW:
        stRotationInput.z = (short)sample->x;
        stRotationInput.y = first;
        if (axisCount > 1)
            accelerate((short)-(second / 2));
        break;
    case SDL_PORT_JOYSTICK_AXES_HOTAS_YAW:
        stRotationInput.z = (short)sample->x;
        stRotationInput.y = first;
        if (axisCount > 1)
            SdlApplyLinearThrottle(secondRaw);
        break;
    case SDL_PORT_JOYSTICK_AXES_HOTAS_ROLL:
        stRotationInput.z = first;
        if (axisCount > 1)
            SdlApplyLinearThrottle(secondRaw);
        break;
    case SDL_PORT_JOYSTICK_AXES_LINEAR_THROTTLE:
        if (axisCount > 1 &&
            g_aSdlJoystickDevices[activeSlot].controller != 0)
            SdlApplyLinearThrottle(secondRaw);
        else
            SdlApplyLinearThrottle(firstRaw);
        break;
    case SDL_PORT_JOYSTICK_AXES_RUDDER_YAW:
        stRotationInput.z = (short)sample->x;
        stRotationInput.y = first;
        break;
    case SDL_PORT_JOYSTICK_AXES_RUDDER_ROLL:
        stRotationInput.z = first;
        break;
    }
}

void SdlEndJoystickSpaceflight(void)
{
    g_bSdlJoystickSpaceflightActive = 0;
    g_bSdlTwoAxisModifierActive = 0;
    g_wSdlRumbleLow = 0;
    g_wSdlRumbleHigh = 0;
    g_dwSdlRumbleDeadline = 0;
    SdlStopJoystickRumble();
}

static int SdlControllerButtonIndex(int button)
{
    switch ((SDL_GameControllerButton)button) {
    case SDL_CONTROLLER_BUTTON_A:
        return SDL_PORT_JOYSTICK_BUTTON_A;
    case SDL_CONTROLLER_BUTTON_B:
        return SDL_PORT_JOYSTICK_BUTTON_B;
    case SDL_CONTROLLER_BUTTON_X:
        return SDL_PORT_JOYSTICK_BUTTON_X;
    case SDL_CONTROLLER_BUTTON_Y:
        return SDL_PORT_JOYSTICK_BUTTON_Y;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return SDL_PORT_JOYSTICK_BUTTON_LEFT_SHOULDER;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return SDL_PORT_JOYSTICK_BUTTON_RIGHT_SHOULDER;
    case SDL_CONTROLLER_BUTTON_BACK:
        return SDL_PORT_JOYSTICK_BUTTON_BACK;
    case SDL_CONTROLLER_BUTTON_START:
        return SDL_PORT_JOYSTICK_BUTTON_START;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        return SDL_PORT_JOYSTICK_BUTTON_LEFT_STICK;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        return SDL_PORT_JOYSTICK_BUTTON_RIGHT_STICK;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return SDL_PORT_JOYSTICK_BUTTON_DPAD_UP;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT;
    default:
        return -1;
    }
}

static void SdlQueueScanCodePress(unsigned short scanCode)
{
    /* player_input samples one transition before consuming the remaining
       queue, so lead with the release for this impulse. */
    QueueInputEvent(4, 0, 0, scanCode, 0, 0, 0);
    QueueInputEvent(3, 0, 0, scanCode, 0, 0, 0);
}

int SdlGetCommunicationMenuSelection(void)
{
    if (nCommMenuChoiceCount <= 0) {
        g_nSdlCommunicationMenuSelection = 0;
        return -1;
    }
    if (nCommMenuReuseMode == 0 ||
        g_nSdlCommunicationMenuSelection >=
            nCommMenuChoiceCount)
        g_nSdlCommunicationMenuSelection = 0;
    return g_nSdlCommunicationMenuSelection;
}

static int SdlHandleCommunicationDpad(int button)
{
    int selection;

    if (get_mode(1) != 4)
        return 0;
    selection = SdlGetCommunicationMenuSelection();
    switch ((SdlJoystickButton)button) {
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_UP:
        if (selection != -1) {
            if (selection == 0)
                selection = nCommMenuChoiceCount;
            g_nSdlCommunicationMenuSelection = selection - 1;
            InvalidateVduMode(1);
        }
        return 1;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN:
        if (selection != -1) {
            selection++;
            if (selection >= nCommMenuChoiceCount)
                selection = 0;
            g_nSdlCommunicationMenuSelection = selection;
            InvalidateVduMode(1);
        }
        return 1;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT:
        SdlQueueScanCodePress(0x01);
        return 1;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT:
        if (selection != -1)
            SdlQueueScanCodePress(
                (unsigned short)(selection + 2));
        return 1;
    default:
        return 0;
    }
}

static unsigned short SdlFlightScanCodeForButton(int button)
{
    switch ((SdlJoystickButton)button) {
    case SDL_PORT_JOYSTICK_BUTTON_B:
        return 0x1c;
    case SDL_PORT_JOYSTICK_BUTTON_Y:
        if (g_eSdlJoystickMode ==
                SDL_PORT_JOYSTICK_FOUR_BUTTON_FOUR_AXIS)
            return 0x14;
        break;
    case SDL_PORT_JOYSTICK_BUTTON_LEFT_SHOULDER:
        return 0x22;
    case SDL_PORT_JOYSTICK_BUTTON_RIGHT_SHOULDER:
        return 0x11;
    case SDL_PORT_JOYSTICK_BUTTON_LEFT_TRIGGER:
    case SDL_PORT_JOYSTICK_BUTTON_LEFT_STICK:
        return 0x31;
    case SDL_PORT_JOYSTICK_BUTTON_RIGHT_TRIGGER:
    case SDL_PORT_JOYSTICK_BUTTON_RIGHT_STICK:
        return 0x1e;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_UP:
        return 0x2b;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN:
        return 0x0e;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT:
        return 0x2e;
    case SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT:
        return 0x26;
    default:
        break;
    }
    return 0;
}

static int SdlControllerMapsRawButton(
    const SdlJoystickDevice *device, int rawButton)
{
    SDL_GameControllerButtonBind binding;
    int controllerButton;

    controllerButton = 0;
    while (controllerButton < SDL_CONTROLLER_BUTTON_MAX) {
        binding = SDL_GameControllerGetBindForButton(
            device->controller,
            (SDL_GameControllerButton)controllerButton);
        if (binding.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON &&
            binding.value.button == rawButton)
            return 1;
        controllerButton++;
    }
    return 0;
}

void SdlTranslateSwcJoystickEvent(SDL_Event *event, int flight)
{
    SdlJoystickDevice *device;
    SDL_JoystickID instance;
    SDL_Keycode key = SDLK_UNKNOWN;
    Uint32 timestamp = event->common.timestamp;
    Uint8 hatPressed;
    unsigned short scanCode = 0;
    int controller = event->type == SDL_CONTROLLERBUTTONDOWN ||
                     event->type == SDL_CONTROLLERBUTTONUP;
    int pressed;
    int button;
    int selection;
    int slot;

    instance = controller ? event->cbutton.which :
        event->type == SDL_JOYHATMOTION ? event->jhat.which : event->jbutton.which;
    SdlRefreshJoysticks();
    SdlSelectSwcJoystick();
    slot = SdlFindJoystick(instance);
    if (slot < 0 || slot != (int)nCurrentActiveInputDevice) {
        event->type = 0;
        return;
    }
    device = &g_aSdlJoystickDevices[slot];
    if (event->type == SDL_JOYHATMOTION) {
        if (device->controller != NULL || event->jhat.hat != 0) {
            event->type = 0;
            return;
        }
        hatPressed = (Uint8)(event->jhat.value & ~device->hatState);
        device->hatState = event->jhat.value;
        pressed = 1;
        if (hatPressed & SDL_HAT_UP)
            button = SDL_PORT_JOYSTICK_BUTTON_DPAD_UP;
        else if (hatPressed & SDL_HAT_DOWN)
            button = SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN;
        else if (hatPressed & SDL_HAT_LEFT)
            button = SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT;
        else if (hatPressed & SDL_HAT_RIGHT)
            button = SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT;
        else {
            event->type = 0;
            return;
        }
    } else if (controller) {
        if (device->controller == NULL) {
            event->type = 0;
            return;
        }
        button = SdlControllerButtonIndex(event->cbutton.button);
        pressed = event->type == SDL_CONTROLLERBUTTONDOWN;
    } else {
        button = event->jbutton.button;
        pressed = event->type == SDL_JOYBUTTONDOWN;
        if (device->controller != NULL && SdlControllerMapsRawButton(device, button)) {
            event->type = 0;
            return;
        }
    }
    if (button == SDL_PORT_JOYSTICK_BUTTON_BACK)
        key = flight ? SDLK_ESCAPE : SDLK_BACKSPACE;
    else if (button == SDL_PORT_JOYSTICK_BUTTON_START)
        key = SDLK_p;
    else if (button == SDL_PORT_JOYSTICK_BUTTON_A)
        key = flight ? SDLK_SPACE : SDLK_RETURN;
    else if (!flight) {
        switch ((SdlJoystickButton)button) {
        case SDL_PORT_JOYSTICK_BUTTON_B: key = SDLK_BACKSPACE; break;
        case SDL_PORT_JOYSTICK_BUTTON_X: key = SDLK_SPACE; break;
        case SDL_PORT_JOYSTICK_BUTTON_DPAD_UP: key = SDLK_UP; break;
        case SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN: key = SDLK_DOWN; break;
        case SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT: key = SDLK_LEFT; break;
        case SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT: key = SDLK_RIGHT; break;
        default: break;
        }
    } else if (SdlJoystickHasEnhancedControls()) {
        if (IsCommChoiceMenuOpen() && button >= SDL_PORT_JOYSTICK_BUTTON_DPAD_UP &&
            button <= SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT) {
            selection = SdlGetCommunicationMenuSelection();
            if (pressed && selection >= 0) {
                if (button == SDL_PORT_JOYSTICK_BUTTON_DPAD_UP)
                    g_nSdlCommunicationMenuSelection =
                        (selection + nCommMenuChoiceCount - 1) % nCommMenuChoiceCount;
                else if (button == SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN)
                    g_nSdlCommunicationMenuSelection = (selection + 1) % nCommMenuChoiceCount;
                else if (button == SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT)
                    scanCode = (unsigned short)(selection + 2);
            }
            if (button == SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT)
                key = SDLK_c;
        } else {
            scanCode = SdlFlightScanCodeForButton(button);
        }
        if (scanCode >= 2 && scanCode <= 8)
            key = SDLK_1 + scanCode - 2;
        else {
            switch (scanCode) {
            case 0x0e: key = SDLK_BACKSPACE; break;
            case 0x11: key = SDLK_w; break;
            case 0x14: key = SDLK_t; break;
            case 0x1c: key = SDLK_RETURN; break;
            case 0x1e: key = SDLK_a; break;
            case 0x22: key = SDLK_g; break;
            case 0x26: key = SDLK_l; break;
            case 0x2b: key = SDLK_BACKSLASH; break;
            case 0x2e: key = SDLK_c; break;
            case 0x31: key = SDLK_n; break;
            default: break;
            }
        }
    }
    SDL_zero(*event);
    if (key != SDLK_UNKNOWN) {
        event->type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
        event->key.timestamp = timestamp;
        event->key.state = pressed ? SDL_PRESSED : SDL_RELEASED;
        event->key.keysym.sym = key;
        event->key.keysym.scancode = SDL_GetScancodeFromKey(key);
    }
}

void SdlHandleJoystickButtonEvent(SDL_JoystickID instanceId,
                                      int button, int pressed,
                                      int controllerEvent)
{
    SdlJoystickDevice *device;
    unsigned short eventType;
    unsigned short scanCode;
    int spaceflightActive;
    int slot;

    slot = SdlFindJoystick(instanceId);
    if (slot == -1 || slot != (int)nCurrentActiveInputDevice)
        return;
    device = &g_aSdlJoystickDevices[slot];
    if (controllerEvent) {
        if (device->controller == 0)
            return;
        button = SdlControllerButtonIndex(button);
    } else if (device->controller != 0 &&
               SdlControllerMapsRawButton(device, button)) {
        return;
    }

    spaceflightActive = g_bSdlJoystickSpaceflightActive &&
        pCurrentEventManagerPump == get_player_input &&
        nArcadeState == 0;
    eventType = pressed ? 3 : 4;
    if (button == SDL_PORT_JOYSTICK_BUTTON_BACK) {
        if (pressed)
            bEscapePressedFlag = 1;
        if (bKeyEventQueueEnabled != 0)
            QueueInputEvent(eventType, 0, 0, 0x1b, 0, 0, 0);
        QueueInputEvent(eventType, 0, 0, 0x01, 0, 0, 0);
        return;
    }
    if (button == SDL_PORT_JOYSTICK_BUTTON_START) {
        if (spaceflightActive && pressed)
            SdlQueueScanCodePress(0x19);
        return;
    }
    if (!pressed)
        return;
    if (button == SDL_PORT_JOYSTICK_BUTTON_Y &&
        !spaceflightActive && bKeyEventQueueEnabled != 0) {
        QueueInputEvent(3, 0, 0, 'Y', 0, 0, 0);
        QueueInputEvent(3, 0, 0, 0x15, 0, 0, 0);
        return;
    }
    if (!spaceflightActive ||
        g_eSdlJoystickMode == SDL_PORT_JOYSTICK_ORIGINAL)
        return;

    if (SdlHandleCommunicationDpad(button))
        return;
    scanCode = SdlFlightScanCodeForButton(button);
    if (scanCode == 0)
        return;
    if (g_bSdlJoystickDebug) {
        printf("SDL joystick action button %d -> scan 0x%02x\n",
               button, (unsigned int)scanCode);
        fflush(stdout);
    }
    SdlQueueScanCodePress(scanCode);
}

void SdlHandleJoystickHatEvent(SDL_JoystickID instanceId,
                                  Uint8 hat, Uint8 value)
{
    SdlJoystickDevice *device;
    Uint8 pressed;
    int slot;

    slot = SdlFindJoystick(instanceId);
    if (slot == -1 || slot != (int)nCurrentActiveInputDevice ||
        hat != 0)
        return;
    device = &g_aSdlJoystickDevices[slot];
    if (device->controller != 0)
        return;

    pressed = (Uint8)(value & ~device->hatState);
    device->hatState = value;
    if ((pressed & SDL_HAT_UP) != 0)
        SdlHandleJoystickButtonEvent(
            instanceId, SDL_PORT_JOYSTICK_BUTTON_DPAD_UP, 1, 0);
    else if ((pressed & SDL_HAT_DOWN) != 0)
        SdlHandleJoystickButtonEvent(
            instanceId, SDL_PORT_JOYSTICK_BUTTON_DPAD_DOWN, 1, 0);
    else if ((pressed & SDL_HAT_LEFT) != 0)
        SdlHandleJoystickButtonEvent(
            instanceId, SDL_PORT_JOYSTICK_BUTTON_DPAD_LEFT, 1, 0);
    else if ((pressed & SDL_HAT_RIGHT) != 0)
        SdlHandleJoystickButtonEvent(
            instanceId, SDL_PORT_JOYSTICK_BUTTON_DPAD_RIGHT, 1, 0);
}

BOOL SdlReadJoystickAxisRange(unsigned int deviceIndex,
                                 unsigned int *xMinimum,
                                 unsigned int *xMaximum,
                                 unsigned int *yMinimum,
                                 unsigned int *yMaximum)
{
    if (deviceIndex >= 2)
        return FALSE;
    SdlRefreshJoysticks();
    if (g_aSdlJoystickDevices[deviceIndex].joystick == 0)
        return FALSE;
    *xMinimum = 0;
    *xMaximum = 0xffff;
    *yMinimum = 0;
    *yMaximum = 0xffff;
    return TRUE;
}

void SdlHandleJoystickDeviceEvent(Uint32 type, Sint32 which)
{
    int slot;

    if (type == SDL_JOYDEVICEREMOVED) {
        slot = SdlFindJoystick((SDL_JoystickID)which);
        if (slot != -1)
            SdlCloseJoystick(&g_aSdlJoystickDevices[slot]);
    }
    SdlRefreshJoysticks();
    if (SdlSwcFlightActive()) {
        SdlSelectSwcJoystick();
        return;
    }
    if (type == SDL_JOYDEVICEADDED &&
        g_bSdlJoystickInputStarted &&
        nCurrentActiveInputDevice == -1)
        LoadJoystickCalibrationFile(9, 9, 1, 1);
}

void SdlShutdownJoysticks(void)
{
    int slot;

    slot = 0;
    while (slot < 2) {
        SdlCloseJoystick(&g_aSdlJoystickDevices[slot]);
        slot++;
    }
    g_bSdlJoystickInputStarted = 0;
    g_bSdlJoystickSpaceflightActive = 0;
    g_bSdlTwoAxisModifierActive = 0;
    g_nSdlCommunicationMenuSelection = 0;
}
