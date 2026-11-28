/* Joysticks and gamepads (joystick.c). */
#ifndef SDL_PORT_JOYSTICK_H
#define SDL_PORT_JOYSTICK_H

#include "platform/win32.h"

#ifdef __cplusplus
extern "C" {
#endif

BOOL SdlReadJoystick(unsigned int device, JOYINFO *information);
BOOL SdlReadJoystickAxisRange(unsigned int device,
                                 unsigned int *xMinimum,
                                 unsigned int *xMaximum,
                                 unsigned int *yMinimum,
                                 unsigned int *yMaximum);
int SdlSetJoystickMode(const char *name);
int SdlSetJoystickSource(int source);
int SdlSetJoystickAxesMode(const char *name);
/* The Custom joystick mode's "control=function" lines (src/port/input/pad_bindings.h),
 * after the axis layout, whose bindings are the defaults. */
void SdlSetPadBindings(const char *bindings);
/* get_player_input: the Custom mode takes the left stick's steps in flight. */
void SdlTakeFlightStickSample(void);
/* The keys the Custom mode's held buttons hold in flight, given the keys that are. */
const Uint8 *SdlPadKeyboardState(const Uint8 *keys);
struct InputDeviceSample;
/* The joystick dead zone, a percentage of travel (src/port/input/joystick_calibration.h). */
int SdlSetJoystickDeadZone(int percent);
short SdlGetJoystickDeadZoneSteps(void);
int SdlAcceptJoystickCalibration(short device,
                                 unsigned short minimumX, unsigned short minimumY,
                                 unsigned short maximumX, unsigned short maximumY,
                                 unsigned short centreX, unsigned short centreY);
void SdlRestoreJoystickCalibration(unsigned short minimumX, unsigned short minimumY,
                                   unsigned short maximumX, unsigned short maximumY,
                                   short horizontalRange, short verticalRange);
void SdlShapeJoystickDeadZone(struct InputDeviceSample *sample);
int SdlJoystickHasEnhancedControls(void);
void SdlSetJoystickRetryMode(int enabled);
int SdlReadSwcJoystickFlightSample(struct InputDeviceSample *sample);
void SdlTranslateSwcJoystickEvent(SDL_Event *event, int flight);
void SdlEnableJoystickDebug(void);
void SdlEnableJoystickRumble(void);
void SdlLogJoystickEvent(const SDL_Event *event);
int SdlGetCommunicationMenuSelection(void);
void SdlQueueJoystickWeaponRumble(int weaponType);
void SdlQueueJoystickDamageRumble(int damage);
void SdlQueueJoystickCollisionRumble(int collisionSpeed);
void SdlApplyJoystickFlightControls(void);
void SdlEndJoystickSpaceflight(void);
/* WC1 is flying and reading its flight controls (not a menu or the map). */
int SdlSpaceflightInputActive(void);
int SdlLiveJoystickSteering(short *pitch, short *yaw);
void SdlHandleJoystickButtonEvent(SDL_JoystickID instanceId,
                                      int button, int pressed,
                                      int controllerEvent);
void SdlHandleJoystickTriggerEvent(SDL_JoystickID instanceId, Uint8 axis,
                                   Sint16 value);
void SdlHandleJoystickHatEvent(SDL_JoystickID instanceId,
                                  Uint8 hat, Uint8 value);
void SdlHandleJoystickDeviceEvent(Uint32 type, Sint32 which);
void SdlShutdownJoysticks(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_JOYSTICK_H */
