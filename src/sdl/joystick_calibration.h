/* WC1's joystick calibration and dead zone, shared by the game
 * (src/sdl/joystick.c) and the launcher's calibration window so that both
 * read the stick alike.
 *
 * Positions are the game's JOYINFO values: SDL's axis plus 32768, 0-65535.
 * j.cal holds seven 16-bit words as CalibrateJoystickInteractive (0x4102b0)
 * writes them: the joystick, the upper-left X and Y, the lower-right X and Y
 * and the centre X and Y.
 *
 * ReadCalibratedJoystick (0x431d20) cuts each side of an axis into nine steps
 * and zeroes those up to the dead zone that LoadJoystickCalibrationFile
 * (0x431a10) takes in whole steps. The game passes one, and the division
 * truncates, so a stick does nothing for two steps (22% of its travel) and
 * then jumps to step two, a quarter of the turn rate (players_flight_dynamics
 * divides by 8). The port's setting is a percentage of
 * travel: 11, 22 and 33 are the original's whole steps; any other value gives
 * the original its whole steps and moves the edge to that percentage. */
#ifndef WC1_SDL_JOYSTICK_CALIBRATION_H
#define WC1_SDL_JOYSTICK_CALIBRATION_H

#define SDL_JOYSTICK_CALIBRATION_FILE "j.cal"
#define SDL_JOYSTICK_CALIBRATION_WORDS 7
/* LoadJoystickCalibrationFile's range: the steps on each side. */
#define SDL_JOYSTICK_STEPS 9
#define SDL_JOYSTICK_DEAD_ZONE_ORIGINAL 22
#define SDL_JOYSTICK_DEAD_ZONE_MAXIMUM 33
/* Recorded extremes closer to the centre than this, an eighth of a side's
 * travel, are not SDL's: the DOS game's gameport counts, or a corner that
 * was never reached. */
#define SDL_JOYSTICK_CALIBRATION_MINIMUM_SPAN 4096

/* One axis as ReadCalibratedJoystick reads it: the centre, a step's size
 * below and above it, and the bounds positions are clamped to. */
typedef struct SdlJoystickAxis {
    int centre;
    int lowScale;
    int highScale;
    int minimum;
    int maximum;
} SdlJoystickAxis;

/* LoadJoystickCalibrationFile without j.cal: the centre of SDL's range and
 * nine steps of (centre - 10) / 9 on both sides. */
static inline void SdlDefaultJoystickAxis(SdlJoystickAxis *axis)
{
    axis->centre = 0xffff / 2;
    axis->lowScale = (axis->centre - 10) / SDL_JOYSTICK_STEPS;
    axis->highScale = axis->lowScale;
    axis->minimum = axis->centre - axis->lowScale * SDL_JOYSTICK_STEPS;
    axis->maximum = axis->centre + axis->highScale * SDL_JOYSTICK_STEPS;
}

/* CalibrateJoystickInteractive's scales and bounds from one axis's recorded
 * extremes and centre. */
static inline void SdlCalibrateJoystickAxis(SdlJoystickAxis *axis, int minimum,
                                            int centre, int maximum, int steps)
{
    axis->centre = centre;
    axis->lowScale = (centre - minimum) / steps;
    if (axis->lowScale == 0)
        axis->lowScale++;
    axis->highScale = (maximum - centre) / steps;
    if (axis->highScale == 0)
        axis->highScale++;
    axis->minimum = centre - axis->lowScale * steps;
    axis->maximum = centre + axis->highScale * steps;
}

static inline int SdlJoystickAxisCalibrationValid(int minimum, int centre,
                                                  int maximum)
{
    return centre - minimum >= SDL_JOYSTICK_CALIBRATION_MINIMUM_SPAN &&
           maximum - centre >= SDL_JOYSTICK_CALIBRATION_MINIMUM_SPAN;
}

static inline int SdlJoystickDeadZoneIsWholeSteps(int percent)
{
    return percent == 11 || percent == 22 || percent == 33;
}

/* LoadJoystickCalibrationFile's dead zone argument: the whole steps inside
 * the dead zone, less the one that the division already zeroes. */
static inline int SdlJoystickDeadZoneSteps(int percent)
{
    int steps;

    if (SdlJoystickDeadZoneIsWholeSteps(percent))
        return percent / 11 - 1;
    steps = percent * SDL_JOYSTICK_STEPS / 100;
    return steps > 1 ? steps - 1 : 0;
}

/* The offset from the centre where a dead zone ends on one side. */
static inline int SdlJoystickDeadZoneEdge(int span, int scale, int percent)
{
    int edge;

    if (SdlJoystickDeadZoneIsWholeSteps(percent))
        return (SdlJoystickDeadZoneSteps(percent) + 1) * scale;
    edge = (int)((long long)span * percent / 100);
    /* SDL's rest position, 32768, lies one past the default centre. */
    return edge > 1 ? edge : 1;
}

/* Moves a position so that ReadCalibratedJoystick, holding the dead zone's
 * whole steps, leaves zero just past the dead zone's edge, and stretches the
 * rest of the travel so that the bound still reaches the last step. */
static inline int SdlShapeJoystickPosition(const SdlJoystickAxis *axis,
                                           int position, int percent)
{
    long long offset;
    long long span;
    long long edge;
    long long start;
    int scale;
    int sign;

    if (SdlJoystickDeadZoneIsWholeSteps(percent))
        return position;
    if (position >= axis->centre) {
        offset = position - axis->centre;
        span = axis->maximum - axis->centre;
        scale = axis->highScale;
        sign = 1;
    } else {
        offset = axis->centre - position;
        span = axis->centre - axis->minimum;
        scale = axis->lowScale;
        sign = -1;
    }
    edge = SdlJoystickDeadZoneEdge((int)span, scale, percent);
    if (offset <= edge)
        return axis->centre;
    start = (long long)(SdlJoystickDeadZoneSteps(percent) + 1) * scale;
    if (offset >= span || start >= span || edge >= span)
        return position;
    offset = start + (offset - edge) * (span - start) / (span - edge);
    return axis->centre + sign * (int)offset;
}

/* ReadCalibratedJoystick's step for one axis, -9 to 9. */
static inline int SdlJoystickAxisStep(const SdlJoystickAxis *axis, int position,
                                      int deadZoneSteps)
{
    int step;

    if (position < axis->minimum)
        position = axis->minimum;
    if (position > axis->maximum)
        position = axis->maximum;
    if (position < axis->centre) {
        step = (axis->centre - position) / axis->lowScale;
        return deadZoneSteps < step ? -step : 0;
    }
    if (position > axis->centre) {
        step = (position - axis->centre) / axis->highScale;
        return step <= deadZoneSteps ? 0 : step;
    }
    return 0;
}

#endif
