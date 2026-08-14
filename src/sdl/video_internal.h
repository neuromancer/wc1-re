#ifndef WC1_SDL_VIDEO_INTERNAL_H
#define WC1_SDL_VIDEO_INTERNAL_H

#include "wc1sdl.h"

#define WC1_SDL_FRAME_WIDTH 320
#define WC1_SDL_FRAME_HEIGHT 200

typedef enum Wc1SdlVideoBackend {
    WC1_SDL_VIDEO_BACKEND_INDEXED,
    WC1_SDL_VIDEO_BACKEND_GL_SHARP_BILINEAR
} Wc1SdlVideoBackend;

void Wc1SdlSetVideoBackend(Wc1SdlVideoBackend backend);
int Wc1SdlUsingGlRenderer(void);
int Wc1SdlConfigureVideoWindow(Uint32 *windowFlags);
void Wc1SdlCalculateOutputViewport(int width, int height, int *left,
                                   int *bottom, int *viewportWidth,
                                   int *viewportHeight);
int Wc1SdlMapLogicalToWindow(SDL_Window *window, int logicalX, int logicalY,
                             int *windowX, int *windowY);
int Wc1SdlMapWindowToLogical(SDL_Window *window, int windowX, int windowY,
                             int *logicalX, int *logicalY);

int Wc1SdlGlRendererInitialize(SDL_Window *window);
void Wc1SdlGlRendererShutdown(void);
int Wc1SdlGlRendererPresent(const unsigned char *pixels,
                            const unsigned char *palette);
void Wc1SdlGlRendererWaitForVerticalBlank(void);
void Wc1SdlGlRendererBeginSpaceFrame(
    const struct ScreenViewportGeometry *geometry, int viewportMode,
    int fullViewportCopy, unsigned char clearColour);
void Wc1SdlGlRendererCompleteSpaceFrame(void);
void Wc1SdlGlRendererCancelSpaceFrame(void);
int Wc1SdlGlRendererRecordSpaceSprite(const struct Viewport *viewport, short x,
                                      short y, unsigned char *shape,
                                      short frame, short angle, short scale,
                                      short flip);

#endif
