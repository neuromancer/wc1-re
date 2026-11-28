/* Frame presentation (video.c). */
#ifndef SDL_PORT_VIDEO_H
#define SDL_PORT_VIDEO_H

#include <SDL.h>

struct ScreenViewportGeometry;
struct Viewport;

#ifdef __cplusplus
extern "C" {
#endif

void SdlDrawViewportStatic(struct Viewport *viewport, int effect,
                              unsigned short colour);
int SdlInitializeVideo(SDL_Window *window);
int SdlPresentIndexedFrame(const unsigned char *pixels,
                              const unsigned char *palette);
void SdlBeginSpaceFrame(
    const struct ScreenViewportGeometry *geometry, int viewportMode,
    int fullViewportCopy, unsigned char clearColour);
void SdlCompleteSpaceFrame(void);
void SdlCancelSpaceFrame(void);
int SdlPresentCurrentFrame(const unsigned char *pixels);
struct Shape;
int SdlRecordSpaceSprite(
    const struct Viewport *viewport, float x, float y,
    struct Shape *shape, short frame, short angle, short scale,
    short flip);
void SdlShutdownVideo(void);
void SdlWaitForVerticalBlank(void);

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_VIDEO_H */
