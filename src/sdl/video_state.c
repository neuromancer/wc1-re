#include "video_internal.h"
#include "wcdata.h"

static SdlVideoBackend g_videoBackend;
static float g_afThrusterScreenX[SPACE_OBJECT_COUNT];
static float g_afThrusterScreenY[SPACE_OBJECT_COUNT];

void SdlSetThrusterScreenPosition(short object, float x, float y)
{
    g_afThrusterScreenX[object] = x;
    g_afThrusterScreenY[object] = y;
}

void SdlGetThrusterScreenPosition(short object, float *x, float *y)
{
    *x = g_afThrusterScreenX[object];
    *y = g_afThrusterScreenY[object];
}

void SdlSetVideoBackend(SdlVideoBackend backend)
{
    if (backend == SDL_PORT_VIDEO_BACKEND_GL_SHARP_BILINEAR)
        g_videoBackend = backend;
    else
        g_videoBackend = SDL_PORT_VIDEO_BACKEND_INDEXED;
}

int SdlUsingGlRenderer(void)
{
    return g_videoBackend == SDL_PORT_VIDEO_BACKEND_GL_SHARP_BILINEAR;
}

int SdlConfigureVideoWindow(Uint32 *windowFlags)
{
    if (windowFlags == 0)
        return 0;
    if (!SdlUsingGlRenderer())
        return 1;
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0 ||
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2) != 0 ||
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE) != 0 ||
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)
        return 0;
#ifdef __APPLE__
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                            SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) != 0)
        return 0;
#endif
    *windowFlags |= SDL_WINDOW_OPENGL;
    return 1;
}

void SdlCalculateOutputViewport(int width, int height, int *left,
                                   int *bottom, int *viewportWidth,
                                   int *viewportHeight)
{
    if (width < 1 || height < 1) {
        *left = 0;
        *bottom = 0;
        *viewportWidth = 1;
        *viewportHeight = 1;
        return;
    }

    /* Mode 13h artwork is composed for a 4:3 display. */
    if ((Sint64)width * SDL_PORT_DISPLAY_ASPECT_HEIGHT >
        (Sint64)height * SDL_PORT_DISPLAY_ASPECT_WIDTH) {
        *viewportHeight = height;
        *viewportWidth =
            ((Sint64)height * SDL_PORT_DISPLAY_ASPECT_WIDTH /
                  SDL_PORT_DISPLAY_ASPECT_HEIGHT);
    } else {
        *viewportWidth = width;
        *viewportHeight =
            ((Sint64)width * SDL_PORT_DISPLAY_ASPECT_HEIGHT /
                  SDL_PORT_DISPLAY_ASPECT_WIDTH);
    }
    if (*viewportWidth < 1)
        *viewportWidth = 1;
    if (*viewportHeight < 1)
        *viewportHeight = 1;
    *left = (width - *viewportWidth) / 2;
    *bottom = (height - *viewportHeight) / 2;
}

static int SdlGetWindowViewport(SDL_Window *window, int *left, int *top,
                                   int *viewportWidth, int *viewportHeight)
{
    int bottom;
    int height;
    int width;

    SDL_GetWindowSize(window, &width, &height);
    if (width < 1 || height < 1)
        return 0;
    SdlCalculateOutputViewport(width, height, left, &bottom,
                                  viewportWidth, viewportHeight);
    *top = height - bottom - *viewportHeight;
    return 1;
}

static int SdlScaleCoordinate(int coordinate, int sourceExtent,
                                 int destinationExtent)
{
    Sint64 numerator;

    numerator = (Sint64)coordinate * destinationExtent;
    if (numerator < 0)
        numerator -= sourceExtent / 2;
    else
        numerator += sourceExtent / 2;
    return (numerator / sourceExtent);
}

int SdlMapLogicalToWindow(SDL_Window *window, int logicalX, int logicalY,
                             int *windowX, int *windowY)
{
    int viewportHeight;
    int viewportLeft;
    int viewportTop;
    int viewportWidth;

    if (window == 0 || windowX == 0 || windowY == 0)
        return 0;
    if (!SdlGetWindowViewport(window, &viewportLeft, &viewportTop,
                                 &viewportWidth, &viewportHeight))
        return 0;
    *windowX = viewportLeft +
               SdlScaleCoordinate(logicalX, SDL_PORT_FRAME_WIDTH,
                                      viewportWidth);
    *windowY = viewportTop +
               SdlScaleCoordinate(logicalY, SDL_PORT_FRAME_HEIGHT,
                                      viewportHeight);
    return 1;
}

int SdlMapWindowToLogical(SDL_Window *window, int windowX, int windowY,
                             int *logicalX, int *logicalY)
{
    int viewportHeight;
    int viewportLeft;
    int viewportTop;
    int viewportWidth;

    if (window == 0 || logicalX == 0 || logicalY == 0)
        return 0;
    if (!SdlGetWindowViewport(window, &viewportLeft, &viewportTop,
                                 &viewportWidth, &viewportHeight))
        return 0;
    *logicalX = SdlScaleCoordinate(windowX - viewportLeft, viewportWidth,
                                      SDL_PORT_FRAME_WIDTH);
    *logicalY = SdlScaleCoordinate(windowY - viewportTop, viewportHeight,
                                      SDL_PORT_FRAME_HEIGHT);
    return 1;
}
