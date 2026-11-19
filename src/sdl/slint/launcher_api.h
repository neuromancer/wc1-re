#ifndef WC1_SDL_SLINT_LAUNCHER_API_H
#define WC1_SDL_SLINT_LAUNCHER_API_H

#include "../joystick_calibration.h"
#include "../pad_bindings.h"

#include <stddef.h>

#define SDL_LAUNCHER_DIRECTORY_CAPACITY 4096
#define SDL_LAUNCHER_KEY_BINDINGS_CAPACITY 4096
#define SDL_LAUNCHER_PAD_BINDINGS_CAPACITY 4096

enum SdlLauncherResult {
    SDL_LAUNCHER_UNAVAILABLE = -2,
    SDL_LAUNCHER_ERROR = -1,
    SDL_LAUNCHER_CANCELLED = 0,
    SDL_LAUNCHER_ACCEPTED = 1
};

enum SdlLauncherJoystickMode {
    SDL_LAUNCHER_JOYSTICK_ORIGINAL = 0,
    SDL_LAUNCHER_JOYSTICK_FOUR_BUTTON_TWO_AXIS = 1,
    SDL_LAUNCHER_JOYSTICK_FOUR_BUTTON_FOUR_AXIS = 2,
    SDL_LAUNCHER_JOYSTICK_CUSTOM = 3,
    SDL_LAUNCHER_JOYSTICK_STEAM_DECK = 4
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
    SDL_LAUNCHER_EXPLICIT_CHEATS = 1 << 7,
    SDL_LAUNCHER_EXPLICIT_FULLSCREEN = 1 << 8,
    SDL_LAUNCHER_EXPLICIT_AUTOSAVE = 1 << 9,
    SDL_LAUNCHER_EXPLICIT_LANGUAGE = 1 << 10,
    SDL_LAUNCHER_EXPLICIT_JOYSTICK_DEAD_ZONE = 1 << 11,
    SDL_LAUNCHER_EXPLICIT_MUSIC = 1 << 12,
    SDL_LAUNCHER_EXPLICIT_MT32_ROMS = 1 << 13,
    SDL_LAUNCHER_EXPLICIT_MT32_MODEL = 1 << 14,
    SDL_LAUNCHER_EXPLICIT_HIRES_HUD = 1 << 15,
    SDL_LAUNCHER_EXPLICIT_INTERPOLATION = 1 << 16,
    SDL_LAUNCHER_EXPLICIT_SOUND = 1 << 17
};

/* The music the game plays (SDL_PORT_MUSIC_* in wc1sdl.h). */
enum SdlLauncherMusic {
    SDL_LAUNCHER_MUSIC_DEFAULT = 0, /* Kilrathi Saga streams, DOS AdLib */
    SDL_LAUNCHER_MUSIC_ADLIB = 1,
    SDL_LAUNCHER_MUSIC_MT32 = 2
};

/* The sound effects the game plays (SDL_PORT_SOUND_* in wc1sdl.h). Unset,
 * as in settings saved before the choice existed, they follow MT-32 music
 * onto the MT-32 and otherwise are the installation's own. */
enum SdlLauncherSound {
    SDL_LAUNCHER_SOUND_UNSET = -1,
    SDL_LAUNCHER_SOUND_DEFAULT = 0, /* Kilrathi Saga digital, DOS AdLib */
    SDL_LAUNCHER_SOUND_ADLIB = 1,
    SDL_LAUNCHER_SOUND_MT32 = 2
};

#define SDL_LAUNCHER_MT32_MODEL_CAPACITY 32

typedef struct SdlLauncherOptions {
    char gameDirectory[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    char voiceCatalog[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    int fullscreen;
    int autosave;
    int language; /* 0: English, 1: Spanish, 2: Japanese (FM Towns), 3: French */
    int enhancedRenderer;
    int egaDither;
    /* Flight HUD text in the larger conversation font, with the enhanced
     * renderer. */
    int hiResHudFonts;
    /* Spaceflight drawn at the display's refresh rate between the game's
     * steps (src/sdl/interpolation.c). */
    int frameInterpolation;
    int cockpitlessView;
    int cheatMode;
    int joystickRumble;
    int joystickMode;
    int joystickSource; /* SdlPadSource, used by Custom. */
    int joystickAxes;
    /* Percent of travel, 0 to SDL_JOYSTICK_DEAD_ZONE_MAXIMUM; the original's
     * is SDL_JOYSTICK_DEAD_ZONE_ORIGINAL. */
    int joystickDeadZone;
    int musicDevice;
    int soundDevice;
    /* The player's MT-32 or CM-32L ROMs, and optionally the libmt32emu
     * machine to use (empty: the best one the folder has). */
    char mt32Roms[SDL_LAUNCHER_DIRECTORY_CAPACITY];
    char mt32Model[SDL_LAUNCHER_MT32_MODEL_CAPACITY];
    int swcDemo;
    int campaign; /* This launch: 0 WC1, 1 SM1, 2 SM2. Not a saved preference. */
    int transferPilot; /* Open the transfer barracks for this launch only. */
    int swcCredits; /* Show only the SWC credits for this launch. */
    /* WC1 flight keys changed in the controls window, one "action=bindings"
     * line each (see src/sdl/keymap.c). */
    char keyBindings[SDL_LAUNCHER_KEY_BINDINGS_CAPACITY];
    /* The Custom joystick mode's bindings, one "control=function" line each
     * (see src/sdl/pad_bindings.h); empty for the defaults. */
    char padBindings[SDL_LAUNCHER_PAD_BINDINGS_CAPACITY];
    /* Command-line values take precedence over saved GUI settings. */
    unsigned int explicitOptions;
} SdlLauncherOptions;

/* Without a Sound choice, as in settings saved before it existed, the
 * sound effects follow MT-32 music onto the MT-32. */
static inline void SdlResolveLauncherSound(SdlLauncherOptions *options)
{
    if (options->soundDevice == SDL_LAUNCHER_SOUND_UNSET)
        options->soundDevice = options->musicDevice == SDL_LAUNCHER_MUSIC_MT32
            ? SDL_LAUNCHER_SOUND_MT32 : SDL_LAUNCHER_SOUND_DEFAULT;
}

#ifdef __cplusplus
extern "C" {
#endif

/* Accepting also sets the working directory to the selected installation. */
int SdlRunLauncherGui(SdlLauncherOptions *options);
/* Game-side check that a directory holds the SWC demo application's
 * resources in any supported form, including a MacBinary application. */
int SdlHasSwcResources(const char *directory);
int SdlFindVoiceDirectory(const char *directory, char *root, unsigned long capacity);
/* German UI strings in a player's patched DOS WC.EXE (0 for any other). */
int SdlCountGermanText(const char *path);
/* 1 when the folder has a playable MT-32 or CM-32L ROM pair, describing it;
 * otherwise 0 and the reason (src/sdl/mt32.c). */
int SdlDescribeMt32Roms(const char *directory, const char *machine,
                        char *text, size_t textSize);

#ifdef __cplusplus
}
#endif

#endif
