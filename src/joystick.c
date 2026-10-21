/*
 * Interactive joystick calibration and button waits.
 * Placement and boundary evidence are recorded in docs/ORDER.md.
 */
#define WC1_JOYSTICK_SOURCE
#include "wc1.h"

/* Function start: 0x4102B0 */
void CalibrateJoystickInteractive()
{
    InputDeviceSample samples[2];
    short calibration[6];
    short shown;
    int file;
    int failed;
    int fontLoaded;
    int horizontalRange;
    int upScale;
    int leftScale;
    int rightScale;
    int verticalRange;
    int downScale;

    fontLoaded = 0;
    if (nCurrentActiveInputDevice == -1)
        return;
    stDefaultTxtContext.alignment = 2;
    if (apTextFonts[1] != 0)
        fontLoaded = 1;

    InitializeTextContextFromFont(&stDefaultTxtContext, 1,
        cViewportClearColour, (signed char)cBlackColour);
    shown = ShowModalTextPanel(1,
        "Turn AUTO FIRE off if present, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Move stick to the UPPER LEFT, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, nCurrentActiveInputDevice, 0x7fff);
        calibration[0] = (short)(nStickMinimumPositionX =
            samples[nCurrentActiveInputDevice].x);
        calibration[1] = (short)(nStickMinimumPositionY =
            samples[nCurrentActiveInputDevice].y);
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Move stick to the LOWER RIGHT, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, nCurrentActiveInputDevice, 0x7fff);
        calibration[2] = (short)(nStickMaxX =
            samples[nCurrentActiveInputDevice].x);
        calibration[3] = (short)(nStickMaxY =
            samples[nCurrentActiveInputDevice].y);
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    shown = ShowModalTextPanel(1,
        "Center Joystick, press a button");
    if (shown != 0) {
        DIBslamReal();
        WaitForJoystickButtonPress();
        SampleJoystickDevice(samples, nCurrentActiveInputDevice, 0x7fff);
        calibration[4] = (short)(nJoystickCentreX =
            samples[nCurrentActiveInputDevice].x);
        calibration[5] = (short)(nJoystickCentreY =
            samples[nCurrentActiveInputDevice].y);
        WaitForJoystickButtonRelease();
        ReleaseModalTextPanel();
    }

    if (fontLoaded == 0)
        ReleaseTextFont(1);

    horizontalRange = nJoystickHorizontalRangeValue;
    if (horizontalRange == 0)
        horizontalRange = 9;
    verticalRange = nJoystickVerticalRange;
    if (verticalRange == 0)
        verticalRange = 9;
    leftScale =
        (nJoystickCentreX - nStickMinimumPositionX) /
        horizontalRange;
    rightScale =
        (nStickMaxX - nJoystickCentreX) /
        horizontalRange;
    upScale =
        (nJoystickCentreY - nStickMinimumPositionY) /
        verticalRange;
    downScale =
        (nStickMaxY - nJoystickCentreY) /
        verticalRange;
    if (leftScale == 0)
        leftScale++;
    if (upScale == 0)
        upScale++;
    if (rightScale == 0)
        rightScale++;
    if (downScale == 0)
        downScale++;
    nCurrentJoystickLeftScale = leftScale;
    nJoystickUpScale = upScale;
    nJoystickRightScale = rightScale;
    nJoystickDownScaleValue = downScale;
    nJoystickHorizontalRangeValue = horizontalRange;
    nJoystickVerticalRange = verticalRange;

    leftScale *= horizontalRange;
    upScale *= verticalRange;
    rightScale *= horizontalRange;
    downScale *= verticalRange;
    nStickMinimumPositionX = nJoystickCentreX - leftScale;
    nStickMinimumPositionY = nJoystickCentreY - upScale;
    nStickMaxX = nJoystickCentreX + rightScale;
    nStickMaxY = nJoystickCentreY + downScale;
    nCurrentJoystickFailureValue = nStickMaxX * 2;

    if (nStickMaxX <= nStickMinimumPositionX ||
        nStickMaxY <= nStickMinimumPositionY) {
        shown = ShowModalTextPanel(1,
            "FAILED! Center Joystick, press a button");
        if (shown != 0) {
            DIBslamReal();
            WaitForJoystickButtonPress();
            WaitForJoystickButtonRelease();
            LoadJoystickCalibrationFile(8, 8, 1, 1);
            ReleaseModalTextPanel();
        }
        _unlink("j.cal");
        return;
    }

    file = _open("j.cal", 0x8301, 0x180);
    if (file == -1)
        return;
    failed = _write(file, &nCurrentActiveInputDevice, 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[0], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[1], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[2], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[3], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[4], 2) == -1;
    if (failed == 0)
        failed = _write(file, &calibration[5], 2) == -1;
    _close(file);
    if (failed != 0)
        _unlink("j.cal");
}

/* Function start: 0x4106C0 */
void WaitForJoystickButtonRelease(void)
{
    do {
        SampleJoystickDevice(
            &aInputDevSamples[
                nCurrentActiveInputDevice],
            nCurrentActiveInputDevice, 0);
    } while (aInputDevSamples[
                 nCurrentActiveInputDevice].buttons != 0);
}

/* Function start: 0x410700 */
void WaitForJoystickButtonPress(void)
{
    do {
        SampleJoystickDevice(
            &aInputDevSamples[
                nCurrentActiveInputDevice],
            nCurrentActiveInputDevice, 0);
    } while (aInputDevSamples[
                 nCurrentActiveInputDevice].buttons == 0);
}
