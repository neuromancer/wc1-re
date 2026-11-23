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
/* The launcher's hi-res HUD fonts option (issue 29). */
void SdlSetHiResHudFonts(int enabled);

/* Hi-res HUD text (src/sdl/hires_hud.c): a layer at twice the frame's size,
   built on each present and drawn by the GL renderer over the frame. */
#define SDL_PORT_HIRES_SCALE 2
#define SDL_PORT_HIRES_WIDTH (SDL_PORT_FRAME_WIDTH * SDL_PORT_HIRES_SCALE)
#define SDL_PORT_HIRES_HEIGHT (SDL_PORT_FRAME_HEIGHT * SDL_PORT_HIRES_SCALE)
void SdlBuildHiResHudLayer(const unsigned char *pixels, unsigned char *frame);
const unsigned char *SdlTakeHiResHudLayer(int *changed);
/* From the GL renderer: whether it can draw the layer, and the frame as the
   game drew it without the menus' mouse cursor. */
int SdlGlOverlayReady(void);
void SdlCopyFrameWithoutMouse(unsigned char *destination,
                              const unsigned char *pixels);

#endif
