/* WC1's joystick calibration and the port's dead zone through the real
 * LoadJoystickCalibrationFile and ReadCalibratedJoystick, on an SDL virtual
 * flight stick. No game assets are needed. */
#include "wc1.h"
#include "../src/sdl/joystick_calibration.h"

#include <unistd.h>

static SDL_Joystick *stick;

static void Require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        exit(1);
    }
}

static void Move(Sint16 x, Sint16 y)
{
    Require(SDL_JoystickSetVirtualAxis(stick, 0, x) == 0 &&
            SDL_JoystickSetVirtualAxis(stick, 1, y) == 0, "move the virtual stick");
    SDL_JoystickUpdate();
}

/* What ReadCalibratedJoystick gives the flight code for one axis. */
static int ReadX(Sint16 x)
{
    Move(x, 0);
    Require(ReadCalibratedJoystick() == 1, "read the calibrated stick");
    return aInputDevSamples[nCurrentActiveInputDevice].x;
}

static int ReadY(Sint16 y)
{
    Move(0, y);
    Require(ReadCalibratedJoystick() == 1, "read the calibrated stick");
    return aInputDevSamples[nCurrentActiveInputDevice].y;
}

static int ReadSwcX(Sint16 x)
{
    InputDeviceSample sample;

    Move(x, 0);
    Require(SdlReadSwcJoystickFlightSample(&sample), "read SWC's stick");
    return sample.x;
}

static void Load(int percent)
{
    Require(SdlSetJoystickDeadZone(percent), "set the dead zone");
    LoadJoystickCalibrationFile(9, 9, SdlGetJoystickDeadZoneSteps(),
                                SdlGetJoystickDeadZoneSteps());
}

static void WriteCalibration(unsigned short device, unsigned short minimumX,
                             unsigned short minimumY, unsigned short maximumX,
                             unsigned short maximumY, unsigned short centreX,
                             unsigned short centreY)
{
    const unsigned short words[SDL_JOYSTICK_CALIBRATION_WORDS] = {
        device, minimumX, minimumY, maximumX, maximumY, centreX, centreY
    };
    FILE *file = fopen("j.cal", "wb");

    Require(file != NULL && fwrite(words, sizeof(words), 1, file) == 1 &&
            fclose(file) == 0, "write j.cal");
}

static int DefaultCalibration(void)
{
    return nCurrentActiveInputDevice == 0 && nJoystickCentreX == 32767 &&
        nJoystickCentreY == 32767 && nCurrentJoystickLeftScale == 3639 &&
        nJoystickRightScale == 3639 && nStickMinimumPositionX == 16 &&
        nStickMaxX == 65518 && nStickMaxY == 65518;
}

int main(void)
{
    char directory[] = "/tmp/wc1-joycal-XXXXXX";
    char previous[4096];
    int device;

    Require(getcwd(previous, sizeof(previous)) != NULL && mkdtemp(directory) != NULL &&
            chdir(directory) == 0, "create an isolated GAMEDAT");
    Require(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) == 0, "initialize SDL joysticks");
    device = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_FLIGHT_STICK, 2, 2, 0);
    Require(device >= 0 && !SDL_IsGameController(device), "attach a virtual flight stick");
    stick = SDL_JoystickOpen(device);
    Require(stick != NULL, "open the virtual flight stick");
    Move(0, 0);

    /* Without j.cal: SDL's range, centre 32767, nine steps of 3639. The
       original's dead zone zeroes the first two steps and jumps to two. */
    Load(22);
    Require(DefaultCalibration() && nJoystickHorizontalDeadZone == 1 &&
            nJoystickVertDeadZoneValue == 1, "the original calibration without j.cal");
    Require(ReadX(0) == 0 && ReadX(7276) == 0 && ReadX(7277) == 2 &&
            ReadX(-7278) == 0 && ReadX(-7279) == -2 &&
            ReadX(32767) == 9 && ReadX(-32768) == -9 &&
            ReadY(7277) == 2 && ReadY(-7279) == -2,
            "22% is the original's two steps exactly");
    Require(ReadSwcX(7276) == 0 && ReadSwcX(7277) == 2 && ReadSwcX(32767) == 9,
            "SWC's stick and the extra axes read as WC1's");

    /* 11% and 33% are whole steps the original takes unchanged. */
    Load(11);
    Require(nJoystickHorizontalDeadZone == 0, "11% is one whole step");
    Require(ReadX(3637) == 0 && ReadX(3638) == 1 && ReadX(-3640) == -1,
            "the original's smallest dead zone");
    Load(33);
    Require(nJoystickHorizontalDeadZone == 2 && ReadX(10915) == 0 && ReadX(10916) == 3,
            "33% is three whole steps");

    /* Between whole steps the edge moves to the percentage: 5% of the
       32751 a side spans is 1637, and step one starts just past it. */
    Load(5);
    Require(nJoystickHorizontalDeadZone == 0, "5% gives the original no whole steps");
    Require(ReadX(0) == 0 && ReadX(1636) == 0 && ReadX(1637) == 1 &&
            ReadX(-1638) == 0 && ReadX(-1639) == -1 &&
            ReadX(16383) == 4 && ReadX(32767) == 9 && ReadX(-32768) == -9,
            "a 5% dead zone, stretched to reach the last step");
    Require(ReadSwcX(1636) == 0 && ReadSwcX(1637) == 1, "SWC follows the dead zone");
    Load(25);
    Require(nJoystickHorizontalDeadZone == 1 && ReadX(8186) == 0 && ReadX(8187) == 2,
            "25% keeps the original's jump to step two");
    Load(0);
    Require(ReadX(0) == 0 && ReadX(-1) == 0 && ReadX(2) == 1,
            "no dead zone still treats SDL's rest position as centred");

    /* j.cal's extremes now set each side, as after the in-game calibration;
       the original kept the centre and made both sides 3639-unit steps. */
    WriteCalibration(0, 12768, 2768, 62768, 62768, 32768, 32768);
    Load(22);
    Require(nCurrentActiveInputDevice == 0 && nJoystickCentreX == 32768 &&
            nCurrentJoystickLeftScale == 2222 && nJoystickRightScale == 3333 &&
            nStickMinimumPositionX == 12770 && nStickMaxX == 62765 &&
            nJoystickUpScale == 3333 && nStickMinimumPositionY == 2771,
            "j.cal's recorded extremes come back");
    Require(ReadX(-19998) == -9 && ReadX(29997) == 9 && ReadX(0) == 0 &&
            ReadX(-4444) == -2 && ReadX(-4443) == 0, "each side reads its own steps");

    /* A j.cal that cannot be SDL's gives the full range, and stays. */
    WriteCalibration(0, 10, 10, 300, 300, 150, 150);
    Load(22);
    Require(DefaultCalibration() && access("j.cal", F_OK) == 0,
            "a gameport-count j.cal is ignored, not deleted");
    WriteCalibration(1, 2768, 2768, 62768, 62768, 32768, 32768);
    Load(22);
    Require(DefaultCalibration(), "a j.cal for an absent joystick 2 is ignored");

    Require(unlink("j.cal") == 0 && chdir(previous) == 0 && rmdir(directory) == 0,
            "remove the isolated GAMEDAT");
    SDL_JoystickClose(stick);
    SDL_JoystickDetachVirtual(device);
    SDL_Quit();
    puts("PASS: original calibration and dead zone, whole and finer dead zones, j.cal extremes, ignored j.cal, SWC axes.");
    return 0;
}
