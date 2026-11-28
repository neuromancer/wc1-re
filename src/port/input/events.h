/* Host events: the event pump, the mouse grab and scene input (events.c). */
#ifndef SDL_PORT_EVENTS_H
#define SDL_PORT_EVENTS_H

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

void SdlPumpEvents(void);
void SdlPumpEventsWhileWaiting(void);
/* Keeps the rate the player sets in flight with [ and ], as half frames a
 * second; the GUI launcher saves it with its settings. */
void SdlSetFlightFpsSaver(void (*saver)(int halves));
void SdlSetMouseGrab(int enabled);
void SdlSuspendMouseGrab(void);
void SdlResumeMouseGrab(void);
/* The endings' frames: Escape ends the scene, P pauses it (events.c). */
void SdlServiceSceneInput(void);
int SdlScenePumpActive(void);
/* Consume host shortcuts/device events; translate controller buttons in place. */
int SdlHandleSwcEvent(SDL_Window *window, SDL_Event *event, int flight);
void SdlStartEventPump(void);
int SdlTranslateScanCode(SDL_Scancode scanCode);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_EVENTS_H */
