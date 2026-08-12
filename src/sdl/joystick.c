#include "wc1.h"

static SDL_Joystick *g_apSdlJoysticks[2];

static SDL_Joystick *Wc1SdlOpenJoystick(unsigned int deviceIndex)
{
    SDL_Joystick *device;

    device = g_apSdlJoysticks[deviceIndex];
    if (device == 0 && SDL_NumJoysticks() > (int)deviceIndex) {
        device = SDL_JoystickOpen((int)deviceIndex);
        g_apSdlJoysticks[deviceIndex] = device;
    }
    if (device == 0 || SDL_JoystickGetAttached(device) == SDL_FALSE)
        return 0;
    return device;
}

BOOL Wc1SdlReadJoystick(unsigned int deviceIndex, JOYINFO *information)
{
    SDL_Joystick *device;
    unsigned int buttonState;
    int buttonIndex;

    if (deviceIndex >= 2)
        return FALSE;
    device = Wc1SdlOpenJoystick(deviceIndex);
    if (device == 0)
        return FALSE;

    SDL_JoystickUpdate();
    information->wXpos =
        (unsigned int)((int)SDL_JoystickGetAxis(device, 0) + 32768);
    information->wYpos =
        (unsigned int)((int)SDL_JoystickGetAxis(device, 1) + 32768);
    information->wZpos = 0;
    buttonState = 0;
    buttonIndex = 0;
    while (buttonIndex < SDL_JoystickNumButtons(device) &&
           buttonIndex < 2) {
        if (SDL_JoystickGetButton(device, buttonIndex) != 0)
            buttonState |= 1U << buttonIndex;
        buttonIndex++;
    }
    information->wButtons = buttonState;
    return TRUE;
}

BOOL Wc1SdlReadJoystickAxisRange(unsigned int deviceIndex,
                                 unsigned int *xMinimum,
                                 unsigned int *xMaximum,
                                 unsigned int *yMinimum,
                                 unsigned int *yMaximum)
{
    if (deviceIndex >= 2 || Wc1SdlOpenJoystick(deviceIndex) == 0)
        return FALSE;
    *xMinimum = 0;
    *xMaximum = 0xffff;
    *yMinimum = 0;
    *yMaximum = 0xffff;
    return TRUE;
}

void Wc1SdlShutdownJoysticks(void)
{
    int index;

    index = 0;
    while (index < 2) {
        if (g_apSdlJoysticks[index] != 0) {
            SDL_JoystickClose(g_apSdlJoysticks[index]);
            g_apSdlJoysticks[index] = 0;
        }
        index++;
    }
}
