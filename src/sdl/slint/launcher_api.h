#ifndef WC1_SDL_SLINT_LAUNCHER_API_H
#define WC1_SDL_SLINT_LAUNCHER_API_H

#define SDL_LAUNCHER_DIRECTORY_CAPACITY 4096

enum SdlLauncherResult {
    SDL_LAUNCHER_UNAVAILABLE = -2,
    SDL_LAUNCHER_ERROR = -1,
    SDL_LAUNCHER_CANCELLED = 0,
    SDL_LAUNCHER_ACCEPTED = 1
};

enum SdlLauncherJoystickMode {
    SDL_LAUNCHER_JOYSTICK_ORIGINAL = 0,
    SDL_LAUNCHER_JOYSTICK_FOUR_BUTTON_TWO_AXIS = 1,
    SDL_LAUNCHER_JOYSTICK_FOUR_BUTTON_FOUR_AXIS = 2
};

enum SdlLauncherJoystickAxes {
    SDL_LAUNCHER_AXES_TWIN_STICK_ROLL = 0,
    SDL_LAUNCHER_AXES_TWIN_STICK_YAW = 1,
    SDL_LAUNCHER_AXES_HOTAS_YAW = 2,
    SDL_LAUNCHER_AXES_HOTAS_ROLL = 3,
    SDL_LAUNCHER_AXES_LINEAR_THROTTLE = 4,
    SDL_LAUNCHER_AXES_RUDDER_YAW = 5,
    SDL_LAUNCHER_AXES_RUDDER_ROLL = 6
};

enum SdlLauncherExplicitOption {
    SDL_LAUNCHER_EXPLICIT_ENHANCED = 1 << 1,
    SDL_LAUNCHER_EXPLICIT_EGA = 1 << 2,
    SDL_LAUNCHER_EXPLICIT_COCKPITLESS = 1 << 3,
    SDL_LAUNCHER_EXPLICIT_RUMBLE = 1 << 4,
    SDL_LAUNCHER_EXPLICIT_JOYSTICK_MODE = 1 << 5,
    SDL_LAUNCHER_EXPLICIT_JOYSTICK_AXES = 1 << 6,
    SDL_LAUNCHER_EXPLICIT_CHEATS = 1 << 7
};

typedef struct SdlLauncherOptions {
    char gameDirectory[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    int enhancedRenderer;
    int egaDither;
    int cockpitlessView;
    int cheatMode;
    int joystickRumble;
    int joystickMode;
    int joystickAxes;
    int swcDemo;
    /* Command-line values take precedence over saved GUI settings. */
    unsigned int explicitOptions;
} SdlLauncherOptions;

#ifdef __cplusplus
extern "C" {
#endif

/* Accepting also sets the working directory to the selected installation. */
int SdlRunLauncherGui(SdlLauncherOptions *options);
/* Game-side check that a directory holds the SWC demo application's
 * resources in any supported form, including a MacBinary application. */
int SdlHasSwcResources(const char *directory);

#ifdef __cplusplus
}
#endif

#endif
