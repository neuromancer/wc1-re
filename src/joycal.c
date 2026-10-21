/*
 *  Joystick calibration and graphics setup data.
 *
 *  Address range 431a10-431eff (see docs/ORDER.md).
 *  The adjacent rectangle helper remains in this provisional unit.
 */
#include "wc1.h"

/* 0x0046b178 */
signed char acGraphicsModeByAnimationSpeed[8] = {
    0x13, 0x0d, 4, 9, 7, 0, 0, 0
};
/* Unreferenced driver names retained in the original initialized data. */
/* 0x0046b180 */
static char szTMDriverName[] = "TM.DRV";
/* 0x0046b188 */
static char szSuperTMDriverName[] = "SUPERTM.DRV";


/* Linker commons defined here; LINK orders them by identifier hash. */
signed char acShipRating[16]; /* 0x0059cd80 */
short asObjScreenAngles[SPACE_OBJECT_COUNT]; /* 0x0059cd90 */
short nTgtRange; /* 0x0059ce10 */
unsigned int anVduModeCache[2]; /* 0x0059ce18 */
signed char acObjOwnersTable[64]; /* 0x0059ce20 */
signed char acShipTargetTable[16]; /* 0x0059ce60 */
short nPrevPitchInputValue; /* 0x0059ce70 */
short nPrevYawInput; /* 0x0059ce72 */
short nPreviousRollInputValue; /* 0x0059ce74 */
short anObjectYawRotation[SPACE_OBJECT_COUNT]; /* 0x0059ce80 */
signed char acPilotHitPointsArray[16]; /* 0x0059cf00 */
signed char acTurnRegulatorsArray[16]; /* 0x0059cf10 */
unsigned char abCollisionAlertTargets[0x10]; /* 0x0059cf20 */

/* Function start: 0x431A10 */
void LoadJoystickCalibrationFile(short horizontalRange,
                                 short verticalRange,
                                 short horizontalDeadZone,
                                 short verticalDeadZone)
{
    unsigned short storedCentreX;
    unsigned short storedCentreY;
    int centreX;
    unsigned short maximumX;
    int centreY;
    int rangeY;
    unsigned short minimumY;
    short activeDevice;
    InputDeviceSample samples[2];
    int file;
    int failed;
    unsigned short minimumX;
    unsigned short maximumY;
    int rangeX;
    int leftScale;
    int upScale;

    activeDevice = -1;
    failed = 1;
    if (horizontalRange == 0)
        horizontalRange += (short)failed;
    if (verticalRange == 0)
        verticalRange++;
    nCurrentJoystickFailureValue = -1;
    SampleBothJoysticks(samples, -1);
    if (samples[0].x != -1 && samples[0].y != -1)
        activeDevice = 0;
    else if (samples[1].x != -1 && samples[1].y != -1)
        activeDevice = 1;

    if (activeDevice != -1) {
        file = _open("j.cal", 0x8000);
        if (file != -1) {
            failed = _read(file, &activeDevice, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &minimumX, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &minimumY, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &maximumX, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &maximumY, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &storedCentreX, 2) <= 0;
            if (failed == 0)
                failed = _read(file, &storedCentreY, 2) <= 0;
            nJoystickCentreX = (unsigned int)storedCentreX;
            nJoystickCentreY = (unsigned int)storedCentreY;
            _close(file);
            if (failed != 0)
                _unlink("j.cal");
        }

        if (failed != 0) {
            GetJoystickDevCaps(activeDevice,
                               (short *)&minimumX,
                               (short *)&maximumX,
                               (short *)&minimumY,
                               (short *)&maximumY);
            centreX = ((int)minimumX + (int)maximumX) / 2;
            centreY = ((int)maximumY + (int)minimumY) / 2;
        } else {
            centreX = nJoystickCentreX;
            centreY = nJoystickCentreY;
        }

        if (centreX > 10) {
            nJoystickCalibrationMinimumX = centreX - 10;
            nJoystickCalibrationMaximumX = centreX + 10;
        } else {
            nJoystickCalibrationMinimumX = centreX;
            nJoystickCalibrationMaximumX = centreX;
        }
        if (centreY > 10) {
            nJoystickCalibrationMinimumY = centreY - 10;
            nJoystickCalibrationMaxY = centreY + 10;
        } else {
            nJoystickCalibrationMaxY = centreY;
            nJoystickCalibrationMinimumY = centreY;
        }

        rangeX = horizontalRange;
        rangeY = verticalRange;
        leftScale = nJoystickCalibrationMinimumX / rangeX;
        upScale = nJoystickCalibrationMinimumY / rangeY;
        nCurrentJoystickLeftScale = leftScale;
        nJoystickUpScale = upScale;
        if (nCurrentJoystickLeftScale == 0)
            nCurrentJoystickLeftScale++;
        if (nJoystickUpScale == 0)
            nJoystickUpScale++;
        nJoystickRightScale = leftScale;
        if (nJoystickRightScale == 0)
            nJoystickRightScale++;
        nJoystickDownScaleValue = upScale;
        if (nJoystickDownScaleValue == 0)
            nJoystickDownScaleValue++;

        nJoystickCentreX = centreX;
        rangeY *= nJoystickUpScale;
        rangeX *= nCurrentJoystickLeftScale;
        nJoystickCentreY = centreY;
        nStickMinimumPositionX = centreX - rangeX;
        nStickMinimumPositionY = centreY - rangeY;
        nStickMaxX = rangeX + centreX;
        nStickMaxY = rangeY + centreY;
        nJoystickHorizontalDeadZone = horizontalDeadZone;
        nJoystickVertDeadZoneValue = verticalDeadZone;
    }
    nCurrentActiveInputDevice = activeDevice;
}

/* Function start: 0x431D20 */
short ReadCalibratedJoystick(void)
{
    InputDeviceSample *sample;
    int sampleResult;
    int failureValue;
    int rawY;
    int rawX;
    int minimumX;
    int centre;
    short normalizedX;
    short normalizedY;
    short device;

    device = *(volatile short *)&nCurrentActiveInputDevice;
    if (device == -1)
        return 0;
    sample = &aInputDevSamples[device];
    sampleResult = SampleJoystickDevice(
        sample, device,
        nCurrentJoystickFailureValue);
    rawX = sample->x;
    failureValue = *(volatile int *)&nCurrentJoystickFailureValue;
    if (rawX == failureValue ||
        (rawY = aInputDevSamples[device].y) == failureValue ||
        sampleResult != 0) {
        nCurrentActiveInputDevice = -1;
        sample->x = aInputDevSamples[device].y =
            aInputDevSamples[device].buttons = 0;
        return 0;
    }

    minimumX = *(volatile int *)&nStickMinimumPositionX;
    nCurrentJoystickRawCoordX = rawX;
    nCurrentJoystickRawCoordY = rawY;
    if (minimumX > rawX)
        sample->x = minimumX;
    if (sample->x > nStickMaxX)
        sample->x = nStickMaxX;
    if (aInputDevSamples[device].y <
        nStickMinimumPositionY)
        aInputDevSamples[device].y =
            nStickMinimumPositionY;
    if (aInputDevSamples[device].y >
        nStickMaxY)
        aInputDevSamples[device].y =
            nStickMaxY;

    normalizedY = 0;
    normalizedX = 0;
    rawX = sample->x;
    centre = *(volatile int *)&nJoystickCentreX;
    if (centre > rawX) {
        rawX = centre - rawX;
        rawX /= nCurrentJoystickLeftScale;
        normalizedX = (short)rawX;
        if (nJoystickHorizontalDeadZone < normalizedX) {
            if (normalizedX != 0)
                normalizedX = (short)-normalizedX;
        } else {
            normalizedX = 0;
        }
    } else if (centre < rawX) {
        rawX = rawX - centre;
        rawX /= nJoystickRightScale;
        normalizedX = (short)rawX;
        if (normalizedX <= nJoystickHorizontalDeadZone)
            normalizedX = 0;
    }

    rawY = aInputDevSamples[device].y;
    centre = *(volatile int *)&nJoystickCentreY;
    if (centre > rawY) {
        rawY = centre - rawY;
        rawY /= nJoystickUpScale;
        normalizedY = (short)rawY;
        if (nJoystickVertDeadZoneValue < normalizedY) {
            if (normalizedY != 0)
                normalizedY = (short)-normalizedY;
        } else {
            normalizedY = 0;
        }
    } else if (centre < rawY) {
        rawY = rawY - centre;
        rawY /= nJoystickDownScaleValue;
        normalizedY = (short)rawY;
        if (normalizedY <= nJoystickVertDeadZoneValue)
            normalizedY = 0;
    }
    sample->x = (int)normalizedX;
    aInputDevSamples[device].y = (int)normalizedY;
    return 1;
}

/* Function start: 0x431EA0 */
void __stdcall UnionRectBounds(ShortRect *destination,
                               const ShortRect *first,
                               const ShortRect *second)
{
    destination->left = first->left < second->left ?
        first->left : second->left;
    destination->top = first->top < second->top ?
        first->top : second->top;
    destination->right = first->right > second->right ?
        first->right : second->right;
    destination->bottom = first->bottom > second->bottom ?
        first->bottom : second->bottom;
}
