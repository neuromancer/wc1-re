/*
 *  Joystick transitions and menu input polling.
 *
 *  Address range 430710-430bbf (see docs/ORDER.md).
 */
#include "wc1.h"

short nInputDoubleClickInterval = 1; /* 0x0046af54 */
short nMenuPointerSpeed = 2; /* 0x0046af58 */


/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned short ausVduModeStack[16]; /* 0x0059d500 */
char acShipSequence[10]; /* 0x0059d520 */
short nTargetFacingValue; /* 0x0059d52a */
FixedVector aShipDestinationsArray[10]; /* 0x0059d530 */
short aasShipShield[10][2]; /* 0x0059d5b0 */
enum ShipTactic aeShipTacticArray[12]; /* 0x0059d5e0 */
signed char abShipExhaustHeatList[10]; /* 0x0059d610 */
short nRenderedSpaceFrame; /* 0x0059d61a */
short nEyePitchTargetValue; /* 0x0059d61c */

/* Function start: 0x430710 */
short __stdcall UpdateInputDeviceTransitions(short raw)
{
    unsigned short previous;
    unsigned short buttons;
    unsigned short changed;
    int now;
    short device;

    device = nCurrentActiveInputDevice;
    if (raw == 0)
        ReadCalibratedJoystick();
    else
        SampleJoystickDevice(&aInputDevSamples[device],
                             device, 0);

    asInputButton2DoubleClick[device] = 0;
    asInputButton1DoubleClicksList[device] = 0;
    asInputButton2ChangedTable[device] = 0;
    asInputBtn1ChangedTable[device] = 0;
    buttons = (unsigned short)aInputDevSamples[device].buttons;
    previous = (unsigned short)asPreviousInputButtons[device];
    if (buttons != previous) {
        changed = buttons ^ previous;
        now = nTickCount60Hz;
        if ((changed & 1) != 0) {
            asInputBtn1ChangedTable[device] = 1;
            if ((buttons & 1) != 0) {
                if ((int)(now -
                    anInputButton1PressTimes[device]) <=
                        nInputDoubleClickInterval *
                        nInputTickScale)
                    asInputButton1DoubleClicksList[device] = 1;
                anInputButton1PressTimes[device] = now;
            }
        }
        if ((changed & 2) != 0) {
            asInputButton2ChangedTable[device] = 1;
            if ((buttons & 2) != 0) {
                if ((int)(now -
                    anInputButton2PressTime[device]) <=
                        nInputDoubleClickInterval *
                        nInputTickScale)
                    asInputButton2DoubleClick[device] = 1;
                anInputButton2PressTime[device] = now;
            }
        }
        asPreviousInputButtons[device] = (short)buttons;
    }
    return 1;
}

/* Function start: 0x430830 */
/* Empty, and nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it sits among the joystick input routines. */
void JoystickInputHook(void)
{
}

/* Function start: 0x430840 */
void PollJoystickButtonEvents(void)
{
    short doubleClick;

    if (nCurrentActiveInputDevice == -1)
        return;
    if (bInputPollingGuard != 0)
        return;
    bInputPollingGuard++;
    UpdateInputDeviceTransitions(1);
    if (asInputBtn1ChangedTable[nCurrentActiveInputDevice] != 0) {
        doubleClick = 0;
        if (asInputButton1DoubleClicksList[nCurrentActiveInputDevice] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            (aInputDevSamples[nCurrentActiveInputDevice].buttons & 1) + 1,
            0, doubleClick);
    }
    if (asInputButton2ChangedTable[nCurrentActiveInputDevice] != 0) {
        doubleClick = 0;
        if (asInputButton2DoubleClick[nCurrentActiveInputDevice] != 0)
            doubleClick = 3;
        QueueInputEventAtCursor(
            ((aInputDevSamples[nCurrentActiveInputDevice].buttons >> 1) &
             1) + 1,
            1, doubleClick);
    }
    stHostMouseState.secondaryButton =
        (unsigned char)aInputDevSamples[nCurrentActiveInputDevice].buttons;
    stHostMouseState.primaryButton = stHostMouseState.secondaryButton;
    bInputPollingGuard--;
}

/* Function start: 0x430920 */
void PollMenuInputDevices(void)
{
    signed char doubleClick;
    unsigned char changes;

    changes = 0;
    if (nCurrentActiveInputDevice == -1)
        return;
    stHostMouseState.x = stMouseCursorState.x;
    stHostMouseState.y = stMouseCursorState.y;
    if (bInputPollingGuard == 0) {
        bInputPollingGuard++;
        UpdateInputDeviceTransitions(0);

        if (asInputBtn1ChangedTable[nCurrentActiveInputDevice] != 0) {
            stHostMouseState.x +=
                aInputDevSamples[nCurrentActiveInputDevice].x *
                nMenuPointerSpeed;
            stHostMouseState.y +=
                aInputDevSamples[nCurrentActiveInputDevice].y *
                nMenuPointerSpeed;
            doubleClick = 0;
            if (asInputButton1DoubleClicksList[nCurrentActiveInputDevice] != 0)
                doubleClick = 3;
            QueueInputEventAtCursor(
                (aInputDevSamples[nCurrentActiveInputDevice].buttons & 1) + 1,
                0, (short)doubleClick);
            changes++;
        }
        if (asInputButton2ChangedTable[nCurrentActiveInputDevice] != 0) {
            stHostMouseState.x +=
                aInputDevSamples[nCurrentActiveInputDevice].x *
                nMenuPointerSpeed;
            stHostMouseState.y +=
                aInputDevSamples[nCurrentActiveInputDevice].y *
                nMenuPointerSpeed;
            doubleClick = 0;
            if (asInputButton1DoubleClicksList[nCurrentActiveInputDevice] != 0)
                doubleClick = 3;
            QueueInputEventAtCursor(
                ((aInputDevSamples[nCurrentActiveInputDevice].buttons >> 1) &
                 1) + 1,
                1, (short)doubleClick);
            changes++;
        }
        if (changes == 0) {
            stHostMouseState.x +=
                aInputDevSamples[nCurrentActiveInputDevice].x *
                nMenuPointerSpeed;
            stHostMouseState.y +=
                aInputDevSamples[nCurrentActiveInputDevice].y *
                nMenuPointerSpeed;
            changes = (unsigned char)(
                aInputDevSamples[nCurrentActiveInputDevice].y |
                aInputDevSamples[nCurrentActiveInputDevice].x);
            if (changes != 0) {
                FlushInputEvents();
                stHostMouseState.x = stHostMouseState.x > 0 ?
                    stHostMouseState.x : 0;
                stHostMouseState.x = stHostMouseState.x < 319 ?
                    stHostMouseState.x : 319;
                stHostMouseState.y = stHostMouseState.y > 0 ?
                    stHostMouseState.y : 0;
                stHostMouseState.y = stHostMouseState.y < 199 ?
                    stHostMouseState.y : 199;
                SetMousePosition(stHostMouseState.x, stHostMouseState.y);
            }
        }

        stHostMouseState.secondaryButton =
            (unsigned char)aInputDevSamples[nCurrentActiveInputDevice].buttons;
        stHostMouseState.primaryButton = stHostMouseState.secondaryButton;
        stHostMouseState.x = stHostMouseState.x > 0 ?
            stHostMouseState.x : 0;
        stHostMouseState.x = stHostMouseState.x < 319 ?
            stHostMouseState.x : 319;
        stHostMouseState.y = stHostMouseState.y > 0 ?
            stHostMouseState.y : 0;
        stHostMouseState.y = stHostMouseState.y < 199 ?
            stHostMouseState.y : 199;
        if (changes != 0) {
            LeaveAllocationScope();
            stMouseCursorState.x = stHostMouseState.x;
            stMouseCursorState.y = stHostMouseState.y;
            stMouseCursorState.primaryButton = stHostMouseState.primaryButton;
            stMouseCursorState.secondaryButton = stHostMouseState.secondaryButton;
            stMouseCursorState.flags = stHostMouseState.flags;
            EnterAllocationScope();
        }
        bInputPollingGuard--;
    }
}

