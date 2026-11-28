/* Gamepad and joystick bindings for WC1's Custom joystick mode, shared by
 * joystick.c and the launcher: "control=function" lines in [pad]. Buttons
 * and hats take a flight action, "cycle-views" or "roll-and-speed"; axes
 * take "pitch", "yaw", "roll", "speed" or "throttle", and "-" reverses
 * one. Right, up and a pressed trigger are positive. */
#ifndef SDL_PORT_PAD_BINDINGS_H
#define SDL_PORT_PAD_BINDINGS_H

/* Custom's input source; Automatic keeps SDL's mapped/raw device choice. */
enum SdlPadSource {
    SDL_PAD_SOURCE_AUTO,
    SDL_PAD_SOURCE_GAMEPAD,
    SDL_PAD_SOURCE_STEAM_DECK,
    SDL_PAD_SOURCE_JOYSTICK
};

#define SDL_PAD_CONTROLS 68
/* The gamepad's controls. */
#define SDL_PAD_GAMEPAD_CONTROLS 20
#define SDL_PAD_BUTTONS 16
#define SDL_PAD_LEFT_TRIGGER 6
#define SDL_PAD_RIGHT_TRIGGER 7
#define SDL_PAD_DPAD_UP 12
#define SDL_PAD_LEFT_STICK_X 16
#define SDL_PAD_LEFT_STICK_Y 17
#define SDL_PAD_RIGHT_STICK_X 18
#define SDL_PAD_RIGHT_STICK_Y 19
/* Another joystick's: its buttons, its hats' directions (up, down, left and
 * right) and its axes. */
#define SDL_PAD_STICK_BUTTON 20
#define SDL_PAD_STICK_BUTTONS 32
#define SDL_PAD_STICK_HAT 52
#define SDL_PAD_STICK_HATS 2
#define SDL_PAD_STICK_AXIS 60
#define SDL_PAD_STICK_AXES 8

static const char *const g_apszSdlPadControls[SDL_PAD_CONTROLS] = {
    "a", "b", "x", "y", "left-shoulder", "right-shoulder", "left-trigger", "right-trigger",
    "back", "start", "left-stick-click", "right-stick-click",
    "dpad-up", "dpad-down", "dpad-left", "dpad-right",
    "left-stick-x", "left-stick-y", "right-stick-x", "right-stick-y",
    "button-1", "button-2", "button-3", "button-4", "button-5", "button-6", "button-7", "button-8",
    "button-9", "button-10", "button-11", "button-12", "button-13", "button-14", "button-15",
    "button-16", "button-17", "button-18", "button-19", "button-20", "button-21", "button-22",
    "button-23", "button-24", "button-25", "button-26", "button-27", "button-28", "button-29",
    "button-30", "button-31", "button-32",
    "hat-1-up", "hat-1-down", "hat-1-left", "hat-1-right",
    "hat-2-up", "hat-2-down", "hat-2-left", "hat-2-right",
    "axis-1", "axis-2", "axis-3", "axis-4", "axis-5", "axis-6", "axis-7", "axis-8"
};

/* The axis functions, in the order that the launcher numbers them. */
#define SDL_PAD_AXIS_FUNCTIONS 5
static const char *const g_apszSdlPadAxisFunctions[SDL_PAD_AXIS_FUNCTIONS] = {
    "pitch", "yaw", "roll", "speed", "throttle"
};

static inline int SdlPadControlTakesAxis(int control)
{
    return (control >= SDL_PAD_BUTTONS && control < SDL_PAD_GAMEPAD_CONTROLS) ||
        control == SDL_PAD_LEFT_TRIGGER || control == SDL_PAD_RIGHT_TRIGGER ||
        (control >= SDL_PAD_STICK_AXIS && control < SDL_PAD_CONTROLS);
}

/* Buttons, triggers and hat directions. */
static inline int SdlPadControlTakesButton(int control)
{
    return (control >= 0 && control < SDL_PAD_BUTTONS) ||
        (control >= SDL_PAD_STICK_BUTTON && control < SDL_PAD_STICK_AXIS);
}

/* The Steam Deck preset (wc1-re#32, wc2-re#66), kept apart from [pad]. */
static inline const char *SdlSteamDeckPadFunction(int control)
{
    static const char *const functions[SDL_PAD_GAMEPAD_CONTROLS] = {
        "", "", "", "cycle-views", "next-missile", "next-gun",
        "fire-missile", "fire-guns", "autopilot", "pause",
        "afterburner", "next-target", "navigation", "damage-display",
        "communications", "lock-target", "roll", "speed", "yaw", "pitch"
    };

    return control >= 0 && control < SDL_PAD_GAMEPAD_CONTROLS ? functions[control] : "";
}

/* SDL's Steam Deck driver numbers PADDLE1..4 as R4, L4, R5, L5. Rear
 * buttons mirror the D-pad, including its communications-menu behavior.
 * Steam Input can expose the same directions on its virtual controller. */
static inline int SdlSteamDeckPaddleControl(int paddle)
{
    static const int controls[4] = {14, 12, 15, 13};

    return paddle >= 0 && paddle < 4 ? controls[paddle] : -1;
}

/* The Custom mode's default bindings: the WCAT four-button, four-axis
 * layout, with the axes the axis layout sets. */
static inline const char *SdlDefaultPadFunction(int control, int axes)
{
    static const char *const buttons[SDL_PAD_BUTTONS] = {
        "fire-guns", "fire-missile", "afterburner", "next-target",
        "next-gun", "next-missile", "fire-missile", "fire-guns",
        "cycle-views", "pause", "navigation", "autopilot",
        "full-speed", "stop", "communications", "lock-target"
    };
    /* Twin-stick and HOTAS yaw, and rudder yaw, roll with the left stick. */
    const int leftRolls = axes == 1 || axes == 2 || axes == 5;

    if (control >= 0 && control < SDL_PAD_BUTTONS)
        return buttons[control];
    if (control >= SDL_PAD_STICK_BUTTON && control < SDL_PAD_STICK_BUTTON + SDL_PAD_BUTTONS) {
        control -= SDL_PAD_STICK_BUTTON;
        if (control == SDL_PAD_LEFT_TRIGGER)
            return "navigation";
        if (control == SDL_PAD_RIGHT_TRIGGER)
            return "autopilot";
        return buttons[control];
    }
    if (control >= SDL_PAD_STICK_HAT && control < SDL_PAD_STICK_HAT + 4)
        return buttons[SDL_PAD_DPAD_UP + control - SDL_PAD_STICK_HAT];
    if (control >= SDL_PAD_STICK_AXIS && control < SDL_PAD_STICK_AXIS + 4)
        control += SDL_PAD_LEFT_STICK_X - SDL_PAD_STICK_AXIS;
    switch (control) {
    case SDL_PAD_LEFT_STICK_X:
        return leftRolls ? "roll" : "yaw";
    case SDL_PAD_LEFT_STICK_Y:
        return "pitch";
    case SDL_PAD_RIGHT_STICK_X:
        return axes == 4 ? "" : leftRolls ? "yaw" : "roll";
    case SDL_PAD_RIGHT_STICK_Y:
        return axes <= 1 ? "speed" : axes <= 4 ? "throttle" : "";
    default:
        return "";
    }
}

#endif
