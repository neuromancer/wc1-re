/* Exercise the real startup path with a stand-in for the native dialog. */
#define WC1_STATIC_GUI 1
#define main SdlLauncherMain
#define SdlEnableEgaDither RecordEgaDither
#define SdlEnableJoystickRumble RecordJoystickRumble
#include "../src/sdl/launcher.c"
#undef main

#include <assert.h>

static int dialogResult;
static int dialogCalls;
static int egaEnabled;
static int rumbleEnabled;

void RecordEgaDither(void)
{
    egaEnabled++;
}

void RecordJoystickRumble(void)
{
    rumbleEnabled++;
}

int SdlRunLauncherGui(SdlLauncherOptions *options)
{
    assert(SDL_WasInit(0) == 0);
    dialogCalls++;
    if (dialogResult == SDL_LAUNCHER_ACCEPTED) {
        assert(options->enhancedRenderer);
        assert(options->egaDither);
        assert(options->joystickRumble);
        options->enhancedRenderer = 0;
        options->egaDither = 0;
        options->joystickRumble = 0;
    }
    return dialogResult;
}

int main(void)
{
    SdlLauncherOptions options;
    int useGui;
    int argumentCount;
    char *defaults[] = {"wc1-modern-gui", 0};
    char *arguments[] = {
        "wc1-modern-gui", "--gui", "--enhanced", "--ega", "--joystick-rumble",
        "--joystick-mode=4button-4axis", "--joystick-axes=hotas-yaw",
        "Origin", "s1", "m0", "l", "-c", "ignored", 0
    };
    char *badMode[] = {"wc1-modern-gui", "--joystick-mode=unknown", 0};
    char *badAxes[] = {"wc1-modern-gui", "--joystick-axes=unknown", 0};
    char *check[] = {
        "wc1-modern-gui", "--check", "--gui", "--enhanced", "--ega",
        "--joystick-rumble", 0
    };
    char *directCheck[] = {"wc1-modern-gui", "--check", 0};

    argumentCount = 13;
    assert(SdlParsePortArguments(&argumentCount, arguments, &useGui, &options));
    assert(useGui && options.enhancedRenderer && options.egaDither);
    assert(options.joystickRumble && options.cockpitlessView);
    assert(options.joystickMode == SDL_LAUNCHER_JOYSTICK_FOUR_BUTTON_FOUR_AXIS);
    assert(options.joystickAxes == SDL_LAUNCHER_AXES_HOTAS_YAW);
    assert(argumentCount == 7 && arguments[7] == 0);
    assert(strcmp(arguments[1], "Origin") == 0);
    assert(strcmp(arguments[5], "-c") == 0);
    assert(egaEnabled == 0 && rumbleEnabled == 0);
    assert(SdlApplyLauncherOptions(&options));
    assert(egaEnabled == 1 && rumbleEnabled == 1);
    egaEnabled = 0;
    rumbleEnabled = 0;
    options.joystickAxes = 99;
    assert(!SdlApplyLauncherOptions(&options));
    argumentCount = 2;
    assert(!SdlParsePortArguments(&argumentCount, badMode, &useGui, &options));
    argumentCount = 2;
    assert(!SdlParsePortArguments(&argumentCount, badAxes, &useGui, &options));

    dialogResult = SDL_LAUNCHER_CANCELLED;
    assert(SdlLauncherMain(1, defaults) == 0);
    assert(dialogCalls == 1 && SDL_WasInit(0) == 0);
    dialogResult = SDL_LAUNCHER_ERROR;
    assert(SdlLauncherMain(1, defaults) == 1);
    assert(dialogCalls == 2 && SDL_WasInit(0) == 0);
    dialogResult = SDL_LAUNCHER_ACCEPTED;
    assert(SdlLauncherMain(6, check) == 0);
    assert(dialogCalls == 3);
    assert(egaEnabled == 0 && rumbleEnabled == 0);
    assert(SdlLauncherMain(2, directCheck) == 0);
    assert(dialogCalls == 3);
    puts("Launcher options and startup checks passed.");
    return 0;
}
