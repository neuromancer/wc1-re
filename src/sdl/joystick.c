#include "wc1.h"

typedef struct Wc1SdlJoystickDevice {
    SDL_GameController *controller;
    SDL_Joystick *joystick;
    SDL_JoystickID instanceId;
    Uint8 hatState;
} Wc1SdlJoystickDevice;

typedef enum Wc1SdlJoystickMode {
    WC1_SDL_JOYSTICK_ORIGINAL,
    WC1_SDL_JOYSTICK_FOUR_BUTTON_TWO_AXIS,
    WC1_SDL_JOYSTICK_FOUR_BUTTON_FOUR_AXIS
} Wc1SdlJoystickMode;

typedef enum Wc1SdlJoystickAxesMode {
    WC1_SDL_JOYSTICK_AXES_TWIN_STICK_ROLL,
    WC1_SDL_JOYSTICK_AXES_TWIN_STICK_YAW,
    WC1_SDL_JOYSTICK_AXES_HOTAS_YAW,
    WC1_SDL_JOYSTICK_AXES_HOTAS_ROLL,
    WC1_SDL_JOYSTICK_AXES_LINEAR_THROTTLE,
    WC1_SDL_JOYSTICK_AXES_RUDDER_YAW,
    WC1_SDL_JOYSTICK_AXES_RUDDER_ROLL
} Wc1SdlJoystickAxesMode;

typedef enum Wc1SdlJoystickButton {
    WC1_SDL_JOYSTICK_BUTTON_A,
    WC1_SDL_JOYSTICK_BUTTON_B,
    WC1_SDL_JOYSTICK_BUTTON_X,
    WC1_SDL_JOYSTICK_BUTTON_Y,
    WC1_SDL_JOYSTICK_BUTTON_LEFT_SHOULDER,
    WC1_SDL_JOYSTICK_BUTTON_RIGHT_SHOULDER,
    WC1_SDL_JOYSTICK_BUTTON_LEFT_TRIGGER,
    WC1_SDL_JOYSTICK_BUTTON_RIGHT_TRIGGER,
    WC1_SDL_JOYSTICK_BUTTON_BACK = 8,
    WC1_SDL_JOYSTICK_BUTTON_START,
    WC1_SDL_JOYSTICK_BUTTON_LEFT_STICK,
    WC1_SDL_JOYSTICK_BUTTON_RIGHT_STICK,
    WC1_SDL_JOYSTICK_BUTTON_DPAD_UP,
    WC1_SDL_JOYSTICK_BUTTON_DPAD_DOWN,
    WC1_SDL_JOYSTICK_BUTTON_DPAD_LEFT,
    WC1_SDL_JOYSTICK_BUTTON_DPAD_RIGHT
} Wc1SdlJoystickButton;

static Wc1SdlJoystickDevice g_aWc1SdlJoystickDevices[2];
static int g_bWc1SdlJoystickDebug;
static int g_bWc1SdlJoystickInputStarted;
static int g_bWc1SdlJoystickSpaceflightActive;
static int g_bWc1SdlTwoAxisModifierActive;
static int g_nWc1SdlCommunicationMenuSelection;
static Wc1SdlJoystickMode g_eWc1SdlJoystickMode;
static Wc1SdlJoystickAxesMode g_eWc1SdlJoystickAxesMode;

int Wc1SdlSetJoystickMode(const char *name)
{
    if (strcmp(name, "original") == 0)
        g_eWc1SdlJoystickMode = WC1_SDL_JOYSTICK_ORIGINAL;
    else if (strcmp(name, "4button-2axis") == 0)
        g_eWc1SdlJoystickMode = WC1_SDL_JOYSTICK_FOUR_BUTTON_TWO_AXIS;
    else if (strcmp(name, "4button-4axis") == 0)
        g_eWc1SdlJoystickMode = WC1_SDL_JOYSTICK_FOUR_BUTTON_FOUR_AXIS;
    else
        return 0;
    return 1;
}

int Wc1SdlSetJoystickAxesMode(const char *name)
{
    if (strcmp(name, "twin-stick-roll") == 0)
        g_eWc1SdlJoystickAxesMode =
            WC1_SDL_JOYSTICK_AXES_TWIN_STICK_ROLL;
    else if (strcmp(name, "twin-stick-yaw") == 0)
        g_eWc1SdlJoystickAxesMode =
            WC1_SDL_JOYSTICK_AXES_TWIN_STICK_YAW;
    else if (strcmp(name, "hotas-yaw") == 0)
        g_eWc1SdlJoystickAxesMode = WC1_SDL_JOYSTICK_AXES_HOTAS_YAW;
    else if (strcmp(name, "hotas-roll") == 0)
        g_eWc1SdlJoystickAxesMode = WC1_SDL_JOYSTICK_AXES_HOTAS_ROLL;
    else if (strcmp(name, "linear-throttle") == 0)
        g_eWc1SdlJoystickAxesMode =
            WC1_SDL_JOYSTICK_AXES_LINEAR_THROTTLE;
    else if (strcmp(name, "rudder-yaw") == 0)
        g_eWc1SdlJoystickAxesMode = WC1_SDL_JOYSTICK_AXES_RUDDER_YAW;
    else if (strcmp(name, "rudder-roll") == 0)
        g_eWc1SdlJoystickAxesMode = WC1_SDL_JOYSTICK_AXES_RUDDER_ROLL;
    else
        return 0;
    return 1;
}

void Wc1SdlEnableJoystickDebug(void)
{
    g_bWc1SdlJoystickDebug = 1;
}

void Wc1SdlLogJoystickEvent(const SDL_Event *event)
{
    const char *name;

    if (!g_bWc1SdlJoystickDebug)
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

static void Wc1SdlCloseJoystick(Wc1SdlJoystickDevice *device)
{
    if (device->controller != 0)
        SDL_GameControllerClose(device->controller);
    else if (device->joystick != 0)
        SDL_JoystickClose(device->joystick);
    device->controller = 0;
    device->joystick = 0;
    device->instanceId = -1;
    device->hatState = SDL_HAT_CENTERED;
}

static int Wc1SdlFindJoystick(SDL_JoystickID instanceId)
{
    int slot;

    slot = 0;
    while (slot < 2) {
        if (g_aWc1SdlJoystickDevices[slot].joystick != 0 &&
            g_aWc1SdlJoystickDevices[slot].instanceId == instanceId)
            return slot;
        slot++;
    }
    return -1;
}

static int Wc1SdlFindFreeJoystickSlot(void)
{
    int slot;

    slot = 0;
    while (slot < 2) {
        if (g_aWc1SdlJoystickDevices[slot].joystick == 0)
            return slot;
        slot++;
    }
    return -1;
}

static void Wc1SdlOpenJoystick(int deviceIndex)
{
    SDL_GameController *controller;
    SDL_Joystick *joystick;
    SDL_JoystickID instanceId;
    Wc1SdlJoystickDevice *device;
    int slot;

    instanceId = SDL_JoystickGetDeviceInstanceID(deviceIndex);
    if (instanceId < 0 || Wc1SdlFindJoystick(instanceId) != -1)
        return;
    slot = Wc1SdlFindFreeJoystickSlot();
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

    device = &g_aWc1SdlJoystickDevices[slot];
    device->controller = controller;
    device->joystick = joystick;
    device->instanceId = SDL_JoystickInstanceID(joystick);
    device->hatState = SDL_HAT_CENTERED;
    if (g_bWc1SdlJoystickDebug) {
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

static void Wc1SdlRefreshJoysticks(void)
{
    Wc1SdlJoystickDevice *device;
    int deviceIndex;
    int slot;

    slot = 0;
    while (slot < 2) {
        device = &g_aWc1SdlJoystickDevices[slot];
        if (device->joystick != 0 &&
            SDL_JoystickGetAttached(device->joystick) == SDL_FALSE)
            Wc1SdlCloseJoystick(device);
        slot++;
    }

    deviceIndex = 0;
    while (deviceIndex < SDL_NumJoysticks() &&
           Wc1SdlFindFreeJoystickSlot() != -1) {
        Wc1SdlOpenJoystick(deviceIndex);
        deviceIndex++;
    }
}

static Sint16 Wc1SdlReadJoystickAxis(
    const Wc1SdlJoystickDevice *device, int axis)
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

static int Wc1SdlReadJoystickButton(
    const Wc1SdlJoystickDevice *device, int button)
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

BOOL Wc1SdlReadJoystick(unsigned int deviceIndex, JOYINFO *information)
{
    Wc1SdlJoystickDevice *device;
    unsigned int buttonState;
    int buttonIndex;

    if (deviceIndex >= 2)
        return FALSE;
    g_bWc1SdlJoystickInputStarted = 1;
    SDL_JoystickUpdate();
    Wc1SdlRefreshJoysticks();
    device = &g_aWc1SdlJoystickDevices[deviceIndex];
    if (device->joystick == 0)
        return FALSE;

    information->wXpos =
        (unsigned int)((int)Wc1SdlReadJoystickAxis(device, 0) + 32768);
    information->wYpos =
        (unsigned int)((int)Wc1SdlReadJoystickAxis(device, 1) + 32768);
    information->wZpos = 0;
    buttonState = 0;
    if (DAT_0059ab2c == get_player_input &&
        g_eWc1SdlJoystickMode != WC1_SDL_JOYSTICK_ORIGINAL) {
        if (Wc1SdlReadJoystickButton(device, 0))
            buttonState = 1;
    } else {
        buttonIndex = 0;
        while (buttonIndex < 2) {
            if (Wc1SdlReadJoystickButton(device, buttonIndex))
                buttonState |= 1U << buttonIndex;
            buttonIndex++;
        }
    }
    if (deviceIndex != 0)
        buttonState <<= 2;
    information->wButtons = buttonState;
    return TRUE;
}

static int Wc1SdlControllerHasAxis(
    const Wc1SdlJoystickDevice *device, SDL_GameControllerAxis axis)
{
    SDL_GameControllerButtonBind binding;

    binding = SDL_GameControllerGetBindForAxis(device->controller, axis);
    return binding.bindType != SDL_CONTROLLER_BINDTYPE_NONE;
}

static int Wc1SdlReadExtraJoystickAxes(Sint16 *first, Sint16 *second)
{
    Wc1SdlJoystickDevice *device;
    Wc1SdlJoystickDevice *other;
    int activeSlot;

    activeSlot = (int)g_nActiveInputDevice_005a819c;
    if (activeSlot < 0 || activeSlot >= 2)
        return 0;
    device = &g_aWc1SdlJoystickDevices[activeSlot];
    if (device->joystick == 0)
        return 0;

    if (device->controller != 0) {
        if (Wc1SdlControllerHasAxis(
                device, SDL_CONTROLLER_AXIS_RIGHTX)) {
            *first = Wc1SdlReadJoystickAxis(device, 2);
            if (Wc1SdlControllerHasAxis(
                    device, SDL_CONTROLLER_AXIS_RIGHTY)) {
                *second = Wc1SdlReadJoystickAxis(device, 3);
                return 2;
            }
            return 1;
        }
    } else if (SDL_JoystickNumAxes(device->joystick) >= 3) {
        *first = Wc1SdlReadJoystickAxis(device, 2);
        if (SDL_JoystickNumAxes(device->joystick) >= 4) {
            *second = Wc1SdlReadJoystickAxis(device, 3);
            return 2;
        }
        return 1;
    }

    other = &g_aWc1SdlJoystickDevices[activeSlot == 0 ? 1 : 0];
    if (other->joystick == 0)
        return 0;
    *first = Wc1SdlReadJoystickAxis(other, 0);
    *second = Wc1SdlReadJoystickAxis(other, 1);
    return 2;
}

static short Wc1SdlNormalizeFlightAxis(Sint16 raw)
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

static void Wc1SdlApplyLinearThrottle(Sint16 raw)
{
    int maximumSpeed;
    int targetSpeed;
    unsigned int position;

    maximumSpeed = (int)g_asShipMaximumSpeed_0059c440[0] << 8;
    position = (unsigned int)((int)raw + 32768);
    targetSpeed = (int)(((int64_t)(65535U - position) * maximumSpeed) /
                        65535);
    celerate(0, targetSpeed - g_anShipSpeed_0059b320[0]);
}

void Wc1SdlApplyJoystickFlightControls(void)
{
    InputDeviceSample *sample;
    Sint16 firstRaw;
    Sint16 secondRaw;
    short first;
    short second;
    int activeSlot;
    int axisCount;

    g_bWc1SdlJoystickSpaceflightActive = 1;
    if (DAT_0059ab2c != get_player_input ||
        g_eWc1SdlJoystickMode == WC1_SDL_JOYSTICK_ORIGINAL)
        return;
    activeSlot = (int)g_nActiveInputDevice_005a819c;
    if (activeSlot < 0 || activeSlot >= 2)
        return;
    sample = &g_aInputDeviceSamples_005a81f0[activeSlot];
    if (Wc1SdlReadJoystickButton(
            &g_aWc1SdlJoystickDevices[activeSlot], 2))
        your_afterburner();

    if (g_eWc1SdlJoystickMode ==
            WC1_SDL_JOYSTICK_FOUR_BUTTON_TWO_AXIS) {
        if (Wc1SdlReadJoystickButton(
                &g_aWc1SdlJoystickDevices[activeSlot], 3)) {
            g_bWc1SdlTwoAxisModifierActive = 1;
            g_nYawInput_0059d3f2 = 0;
            g_nPitchInput_0059d3f0 = 0;
            g_nRollInput_0059d3f4 = (short)sample->x;
            accelerate((short)-(sample->y / 2));
        } else if (g_bWc1SdlTwoAxisModifierActive) {
            g_bWc1SdlTwoAxisModifierActive = 0;
            g_nRollInput_0059d3f4 = 0;
            g_nYawInput_0059d3f2 = (short)sample->x;
            g_nPitchInput_0059d3f0 = (short)-sample->y;
        }
        return;
    }

    axisCount = Wc1SdlReadExtraJoystickAxes(&firstRaw, &secondRaw);
    if (axisCount == 0)
        return;
    first = Wc1SdlNormalizeFlightAxis(firstRaw);
    second = axisCount > 1 ? Wc1SdlNormalizeFlightAxis(secondRaw) : 0;

    switch (g_eWc1SdlJoystickAxesMode) {
    case WC1_SDL_JOYSTICK_AXES_TWIN_STICK_ROLL:
        g_nRollInput_0059d3f4 = first;
        if (axisCount > 1)
            accelerate((short)-(second / 2));
        break;
    case WC1_SDL_JOYSTICK_AXES_TWIN_STICK_YAW:
        g_nRollInput_0059d3f4 = (short)sample->x;
        g_nYawInput_0059d3f2 = first;
        if (axisCount > 1)
            accelerate((short)-(second / 2));
        break;
    case WC1_SDL_JOYSTICK_AXES_HOTAS_YAW:
        g_nRollInput_0059d3f4 = (short)sample->x;
        g_nYawInput_0059d3f2 = first;
        if (axisCount > 1)
            Wc1SdlApplyLinearThrottle(secondRaw);
        break;
    case WC1_SDL_JOYSTICK_AXES_HOTAS_ROLL:
        g_nRollInput_0059d3f4 = first;
        if (axisCount > 1)
            Wc1SdlApplyLinearThrottle(secondRaw);
        break;
    case WC1_SDL_JOYSTICK_AXES_LINEAR_THROTTLE:
        if (axisCount > 1 &&
            g_aWc1SdlJoystickDevices[activeSlot].controller != 0)
            Wc1SdlApplyLinearThrottle(secondRaw);
        else
            Wc1SdlApplyLinearThrottle(firstRaw);
        break;
    case WC1_SDL_JOYSTICK_AXES_RUDDER_YAW:
        g_nRollInput_0059d3f4 = (short)sample->x;
        g_nYawInput_0059d3f2 = first;
        break;
    case WC1_SDL_JOYSTICK_AXES_RUDDER_ROLL:
        g_nRollInput_0059d3f4 = first;
        break;
    }
}

void Wc1SdlEndJoystickSpaceflight(void)
{
    g_bWc1SdlJoystickSpaceflightActive = 0;
}

static int Wc1SdlControllerButtonIndex(int button)
{
    switch ((SDL_GameControllerButton)button) {
    case SDL_CONTROLLER_BUTTON_A:
        return WC1_SDL_JOYSTICK_BUTTON_A;
    case SDL_CONTROLLER_BUTTON_B:
        return WC1_SDL_JOYSTICK_BUTTON_B;
    case SDL_CONTROLLER_BUTTON_X:
        return WC1_SDL_JOYSTICK_BUTTON_X;
    case SDL_CONTROLLER_BUTTON_Y:
        return WC1_SDL_JOYSTICK_BUTTON_Y;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return WC1_SDL_JOYSTICK_BUTTON_LEFT_SHOULDER;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return WC1_SDL_JOYSTICK_BUTTON_RIGHT_SHOULDER;
    case SDL_CONTROLLER_BUTTON_BACK:
        return WC1_SDL_JOYSTICK_BUTTON_BACK;
    case SDL_CONTROLLER_BUTTON_START:
        return WC1_SDL_JOYSTICK_BUTTON_START;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        return WC1_SDL_JOYSTICK_BUTTON_LEFT_STICK;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        return WC1_SDL_JOYSTICK_BUTTON_RIGHT_STICK;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return WC1_SDL_JOYSTICK_BUTTON_DPAD_UP;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return WC1_SDL_JOYSTICK_BUTTON_DPAD_DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return WC1_SDL_JOYSTICK_BUTTON_DPAD_LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return WC1_SDL_JOYSTICK_BUTTON_DPAD_RIGHT;
    default:
        return -1;
    }
}

static void Wc1SdlQueueScanCodePress(unsigned short scanCode)
{
    /* player_input samples one transition before consuming the remaining
       queue, so lead with the release for this impulse. */
    QueueInputEvent(4, 0, 0, scanCode, 0, 0, 0);
    QueueInputEvent(3, 0, 0, scanCode, 0, 0, 0);
}

int Wc1SdlGetCommunicationMenuSelection(void)
{
    if (g_nCommMenuChoiceCount_0046af60 <= 0) {
        g_nWc1SdlCommunicationMenuSelection = 0;
        return -1;
    }
    if (g_nCommMenuReuseMode_0046af64 == 0 ||
        g_nWc1SdlCommunicationMenuSelection >=
            g_nCommMenuChoiceCount_0046af60)
        g_nWc1SdlCommunicationMenuSelection = 0;
    return g_nWc1SdlCommunicationMenuSelection;
}

static int Wc1SdlHandleCommunicationDpad(int button)
{
    int selection;

    if (get_mode(1) != 4)
        return 0;
    selection = Wc1SdlGetCommunicationMenuSelection();
    switch ((Wc1SdlJoystickButton)button) {
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_UP:
        if (selection != -1) {
            if (selection == 0)
                selection = g_nCommMenuChoiceCount_0046af60;
            g_nWc1SdlCommunicationMenuSelection = selection - 1;
            InvalidateVduMode(1);
        }
        return 1;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_DOWN:
        if (selection != -1) {
            selection++;
            if (selection >= g_nCommMenuChoiceCount_0046af60)
                selection = 0;
            g_nWc1SdlCommunicationMenuSelection = selection;
            InvalidateVduMode(1);
        }
        return 1;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_LEFT:
        Wc1SdlQueueScanCodePress(0x01);
        return 1;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_RIGHT:
        if (selection != -1)
            Wc1SdlQueueScanCodePress(
                (unsigned short)(selection + 2));
        return 1;
    default:
        return 0;
    }
}

static unsigned short Wc1SdlFlightScanCodeForButton(int button)
{
    switch ((Wc1SdlJoystickButton)button) {
    case WC1_SDL_JOYSTICK_BUTTON_B:
        return 0x1c;
    case WC1_SDL_JOYSTICK_BUTTON_Y:
        if (g_eWc1SdlJoystickMode ==
                WC1_SDL_JOYSTICK_FOUR_BUTTON_FOUR_AXIS)
            return 0x14;
        break;
    case WC1_SDL_JOYSTICK_BUTTON_LEFT_SHOULDER:
        return 0x22;
    case WC1_SDL_JOYSTICK_BUTTON_RIGHT_SHOULDER:
        return 0x11;
    case WC1_SDL_JOYSTICK_BUTTON_LEFT_TRIGGER:
    case WC1_SDL_JOYSTICK_BUTTON_LEFT_STICK:
        return 0x31;
    case WC1_SDL_JOYSTICK_BUTTON_RIGHT_TRIGGER:
    case WC1_SDL_JOYSTICK_BUTTON_RIGHT_STICK:
        return 0x1e;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_UP:
        return 0x2b;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_DOWN:
        return 0x0e;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_LEFT:
        return 0x2e;
    case WC1_SDL_JOYSTICK_BUTTON_DPAD_RIGHT:
        return 0x26;
    default:
        break;
    }
    return 0;
}

static int Wc1SdlControllerMapsRawButton(
    const Wc1SdlJoystickDevice *device, int rawButton)
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

void Wc1SdlHandleJoystickButtonEvent(SDL_JoystickID instanceId,
                                      int button, int pressed,
                                      int controllerEvent)
{
    Wc1SdlJoystickDevice *device;
    unsigned short eventType;
    unsigned short scanCode;
    int spaceflightActive;
    int slot;

    slot = Wc1SdlFindJoystick(instanceId);
    if (slot == -1 || slot != (int)g_nActiveInputDevice_005a819c)
        return;
    device = &g_aWc1SdlJoystickDevices[slot];
    if (controllerEvent) {
        if (device->controller == 0)
            return;
        button = Wc1SdlControllerButtonIndex(button);
    } else if (device->controller != 0 &&
               Wc1SdlControllerMapsRawButton(device, button)) {
        return;
    }

    spaceflightActive = g_bWc1SdlJoystickSpaceflightActive &&
        DAT_0059ab2c == get_player_input &&
        g_nArcadeState_00469fb0 == 0;
    eventType = pressed ? 3 : 4;
    if (button == WC1_SDL_JOYSTICK_BUTTON_BACK) {
        if (pressed)
            DAT_0059ab58 = 1;
        if (DAT_0046505c != 0)
            QueueInputEvent(eventType, 0, 0, 0x1b, 0, 0, 0);
        QueueInputEvent(eventType, 0, 0, 0x01, 0, 0, 0);
        return;
    }
    if (button == WC1_SDL_JOYSTICK_BUTTON_START) {
        if (spaceflightActive && pressed)
            Wc1SdlQueueScanCodePress(0x19);
        return;
    }
    if (!pressed)
        return;
    if (button == WC1_SDL_JOYSTICK_BUTTON_Y &&
        !spaceflightActive && DAT_0046505c != 0) {
        QueueInputEvent(3, 0, 0, 'Y', 0, 0, 0);
        QueueInputEvent(3, 0, 0, 0x15, 0, 0, 0);
        return;
    }
    if (!spaceflightActive ||
        g_eWc1SdlJoystickMode == WC1_SDL_JOYSTICK_ORIGINAL)
        return;

    if (Wc1SdlHandleCommunicationDpad(button))
        return;
    scanCode = Wc1SdlFlightScanCodeForButton(button);
    if (scanCode == 0)
        return;
    if (g_bWc1SdlJoystickDebug) {
        printf("SDL joystick action button %d -> scan 0x%02x\n",
               button, (unsigned int)scanCode);
        fflush(stdout);
    }
    Wc1SdlQueueScanCodePress(scanCode);
}

void Wc1SdlHandleJoystickHatEvent(SDL_JoystickID instanceId,
                                  Uint8 hat, Uint8 value)
{
    Wc1SdlJoystickDevice *device;
    Uint8 pressed;
    int slot;

    slot = Wc1SdlFindJoystick(instanceId);
    if (slot == -1 || slot != (int)g_nActiveInputDevice_005a819c ||
        hat != 0)
        return;
    device = &g_aWc1SdlJoystickDevices[slot];
    if (device->controller != 0)
        return;

    pressed = (Uint8)(value & ~device->hatState);
    device->hatState = value;
    if ((pressed & SDL_HAT_UP) != 0)
        Wc1SdlHandleJoystickButtonEvent(
            instanceId, WC1_SDL_JOYSTICK_BUTTON_DPAD_UP, 1, 0);
    else if ((pressed & SDL_HAT_DOWN) != 0)
        Wc1SdlHandleJoystickButtonEvent(
            instanceId, WC1_SDL_JOYSTICK_BUTTON_DPAD_DOWN, 1, 0);
    else if ((pressed & SDL_HAT_LEFT) != 0)
        Wc1SdlHandleJoystickButtonEvent(
            instanceId, WC1_SDL_JOYSTICK_BUTTON_DPAD_LEFT, 1, 0);
    else if ((pressed & SDL_HAT_RIGHT) != 0)
        Wc1SdlHandleJoystickButtonEvent(
            instanceId, WC1_SDL_JOYSTICK_BUTTON_DPAD_RIGHT, 1, 0);
}

BOOL Wc1SdlReadJoystickAxisRange(unsigned int deviceIndex,
                                 unsigned int *xMinimum,
                                 unsigned int *xMaximum,
                                 unsigned int *yMinimum,
                                 unsigned int *yMaximum)
{
    if (deviceIndex >= 2)
        return FALSE;
    Wc1SdlRefreshJoysticks();
    if (g_aWc1SdlJoystickDevices[deviceIndex].joystick == 0)
        return FALSE;
    *xMinimum = 0;
    *xMaximum = 0xffff;
    *yMinimum = 0;
    *yMaximum = 0xffff;
    return TRUE;
}

void Wc1SdlHandleJoystickDeviceEvent(Uint32 type, Sint32 which)
{
    int slot;

    if (type == SDL_JOYDEVICEREMOVED) {
        slot = Wc1SdlFindJoystick((SDL_JoystickID)which);
        if (slot != -1)
            Wc1SdlCloseJoystick(&g_aWc1SdlJoystickDevices[slot]);
    }
    Wc1SdlRefreshJoysticks();
    if (type == SDL_JOYDEVICEADDED &&
        g_bWc1SdlJoystickInputStarted &&
        g_nActiveInputDevice_005a819c == -1)
        LoadJoystickCalibrationFile(9, 9, 1, 1);
}

void Wc1SdlShutdownJoysticks(void)
{
    int slot;

    slot = 0;
    while (slot < 2) {
        Wc1SdlCloseJoystick(&g_aWc1SdlJoystickDevices[slot]);
        slot++;
    }
    g_bWc1SdlJoystickInputStarted = 0;
    g_bWc1SdlJoystickSpaceflightActive = 0;
    g_bWc1SdlTwoAxisModifierActive = 0;
    g_nWc1SdlCommunicationMenuSelection = 0;
}
