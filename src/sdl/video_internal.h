#ifndef SDL_PORT_VIDEO_INTERNAL_H
#define SDL_PORT_VIDEO_INTERNAL_H

#include "wc1sdl.h"

#define SDL_PORT_FRAME_WIDTH 320
#define SDL_PORT_FRAME_HEIGHT 200
#define SDL_PORT_DISPLAY_ASPECT_WIDTH 4
#define SDL_PORT_DISPLAY_ASPECT_HEIGHT 3

typedef enum SdlVideoBackend {
    SDL_PORT_VIDEO_BACKEND_INDEXED,
    SDL_PORT_VIDEO_BACKEND_GL_SHARP_BILINEAR
} SdlVideoBackend;

void SdlSetVideoBackend(SdlVideoBackend backend);
int SdlUsingGlRenderer(void);
void SdlEnableEgaDither(void);
int SdlConfigureVideoWindow(Uint32 *windowFlags);
void SdlCalculateOutputViewport(int width, int height, int *left,
                                   int *bottom, int *viewportWidth,
                                   int *viewportHeight);
int SdlMapLogicalToWindow(SDL_Window *window, int logicalX, int logicalY,
                             int *windowX, int *windowY);
int SdlMapWindowToLogical(SDL_Window *window, int windowX, int windowY,
                             int *logicalX, int *logicalY);

int SdlGlRendererInitialize(SDL_Window *window);
void SdlGlRendererShutdown(void);
int SdlGlRendererPresent(const unsigned char *pixels,
                            const unsigned char *palette);
void SdlGlRendererWaitForVerticalBlank(void);
void SdlGlRendererBeginSpaceFrame(
    const struct ScreenViewportGeometry *geometry, int viewportMode,
    int fullViewportCopy, unsigned char clearColour);
void SdlGlRendererCompleteSpaceFrame(void);
void SdlGlRendererCancelSpaceFrame(void);
int SdlGlRendererRecordSpaceSprite(const struct Viewport *viewport, float x,
                                      float y, struct Shape *shape,
                                      short frame, short angle, short scale,
                                      short flip);

#endif
