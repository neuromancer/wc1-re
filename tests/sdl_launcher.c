/* Exercise the real startup path with a stand-in for the native dialog. */
#define WC1_STATIC_GUI 1
#define main SdlLauncherMain
#define SdlEnableEgaDither RecordEgaDither
#define SdlEnableJoystickRumble RecordJoystickRumble
#define SdlFindSwcMissionData FindTestSwcMissionData
#define SdlRunSwcMission RunTestSwcMission
#include "../src/sdl/launcher.c"
#undef main

#include <assert.h>

static int dialogResult;
static int dialogCalls;
static int egaEnabled;
static int rumbleEnabled;
static int swcSelected;
static int swcFound;
static int missionCalls;

int FindTestSwcMissionData(char *path, unsigned long capacity)
{
    SDL_strlcpy(path, "CMFs/Data.CMF", capacity);
    return swcFound;
}

int RunTestSwcMission(const char *path, int checkOnly, int cockpitless)
{
    assert(SDL_WasInit(0) == 0);
    assert(strcmp(path, "CMFs/Data.CMF") == 0);
    assert(checkOnly && cockpitless);
    missionCalls++;
    return 0;
}

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
        if (swcSelected) {
            options->swcDemo = 1;
            options->cockpitlessView = 1;
            return dialogResult;
        }
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
    char *swcCheck[] = {"wc1-modern-gui", "--gui", "--check", 0};
    char *missingSwc[] = {"wc1-modern-gui", "--gui", "--check", 0};
    char *swcOption[] = {"wc1-modern-gui", "--swc-demo", "/tmp/SWC Demo", "--check", 0};
    char *badSwcOption[] = {"wc1-modern-gui", "--swc-demo", 0};

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
    argumentCount = 2;
    assert(!SdlParsePortArguments(&argumentCount, badSwcOption, &useGui, &options));
    argumentCount = 4;
    assert(SdlParsePortArguments(&argumentCount, swcOption, &useGui, &options));
    assert(options.swcDemo && strcmp(options.gameDirectory, "/tmp/SWC Demo") == 0);
    assert(argumentCount == 2 && strcmp(swcOption[1], "--check") == 0);

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
    swcSelected = 1;
    swcFound = 1;
    assert(SdlLauncherMain(3, swcCheck) == 0);
    assert(dialogCalls == 4 && missionCalls == 1 && SDL_WasInit(0) == 0);
    swcFound = 0;
    assert(SdlLauncherMain(3, missingSwc) == 1);
    assert(dialogCalls == 5 && missionCalls == 1 && SDL_WasInit(0) == 0);
    puts("Launcher options and startup checks passed.");
    return 0;
}
