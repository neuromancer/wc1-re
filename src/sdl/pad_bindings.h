/* The game controller's inputs in WC1's Custom joystick mode, shared by
 * src/sdl/joystick.c and the launcher's Joystick tab (controls.slint).
 *
 * The launcher settings' [pad] section binds each control to a function, one
 * "control=function" line each; a control without a line keeps its default.
 * The sixteen buttons, numbered as SdlJoystickButton numbers them, take a
 * flight key's action (the ids of keymap_actions.h), "cycle-views", or
 * "roll-and-speed", which while held turns what yaws into roll and what
 * pitches into speed, as the original joystick's second button does. The
 * sticks' axes take "pitch", "yaw", "roll", "speed" (faster or slower at a
 * rate) or "throttle" (the speed its position gives); a leading "-" reverses
 * one. The triggers take either kind. An empty function leaves a control
 * unbound. Right, up and a pressed trigger are positive, and pitch pushes the
 * nose down, as the original's stick does. */
#ifndef WC1_SDL_PAD_BINDINGS_H
#define WC1_SDL_PAD_BINDINGS_H

#define SDL_PAD_CONTROLS 20
#define SDL_PAD_BUTTONS 16
#define SDL_PAD_LEFT_TRIGGER 6
#define SDL_PAD_RIGHT_TRIGGER 7
#define SDL_PAD_LEFT_STICK_X 16
#define SDL_PAD_LEFT_STICK_Y 17
#define SDL_PAD_RIGHT_STICK_X 18
#define SDL_PAD_RIGHT_STICK_Y 19

static const char *const g_apszSdlPadControls[SDL_PAD_CONTROLS] = {
    "a", "b", "x", "y", "left-shoulder", "right-shoulder", "left-trigger", "right-trigger",
    "back", "start", "left-stick-click", "right-stick-click",
    "dpad-up", "dpad-down", "dpad-left", "dpad-right",
    "left-stick-x", "left-stick-y", "right-stick-x", "right-stick-y"
};

/* The axis functions, in the order that the launcher numbers them. */
#define SDL_PAD_AXIS_FUNCTIONS 5
static const char *const g_apszSdlPadAxisFunctions[SDL_PAD_AXIS_FUNCTIONS] = {
    "pitch", "yaw", "roll", "speed", "throttle"
};

static inline int SdlPadControlTakesAxis(int control)
{
    return control >= SDL_PAD_BUTTONS || control == SDL_PAD_LEFT_TRIGGER ||
        control == SDL_PAD_RIGHT_TRIGGER;
}

/* The Custom mode's bindings until the player changes them: the WCAT
 * four-button, four-axis mode's, its axes as the axis layout (0-6, as
 * SdlJoystickAxesMode numbers them) sets them. */
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
