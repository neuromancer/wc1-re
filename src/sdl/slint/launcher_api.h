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

typedef struct SdlLauncherOptions {
    char gameDirectory[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    int enhancedRenderer;
    int egaDither;
    int cockpitlessView;
    int joystickRumble;
    int joystickMode;
    int joystickAxes;
} SdlLauncherOptions;

#ifdef __cplusplus
extern "C" {
#endif

/* Accepting also sets the working directory to the selected installation. */
int SdlRunLauncherGui(SdlLauncherOptions *options);

#ifdef __cplusplus
}
#endif

#endif
