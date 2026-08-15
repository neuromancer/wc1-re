#include "video_internal.h"

static Wc1SdlVideoBackend g_videoBackend;

void Wc1SdlSetVideoBackend(Wc1SdlVideoBackend backend)
{
    if (backend == WC1_SDL_VIDEO_BACKEND_GL_SHARP_BILINEAR)
        g_videoBackend = backend;
    else
        g_videoBackend = WC1_SDL_VIDEO_BACKEND_INDEXED;
}

int Wc1SdlUsingGlRenderer(void)
{
    return g_videoBackend == WC1_SDL_VIDEO_BACKEND_GL_SHARP_BILINEAR;
}

int Wc1SdlConfigureVideoWindow(Uint32 *windowFlags)
{
    if (windowFlags == 0)
        return 0;
    if (!Wc1SdlUsingGlRenderer())
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

void Wc1SdlCalculateOutputViewport(int width, int height, int *left,
                                   int *bottom, int *viewportWidth,
                                   int *viewportHeight)
{
    int scale;
    double fractionalScale;

    scale = width / WC1_SDL_FRAME_WIDTH;
    if (height / WC1_SDL_FRAME_HEIGHT < scale)
        scale = height / WC1_SDL_FRAME_HEIGHT;
    if (scale >= 1) {
        *viewportWidth = WC1_SDL_FRAME_WIDTH * scale;
        *viewportHeight = WC1_SDL_FRAME_HEIGHT * scale;
    } else {
        fractionalScale = (double)width / WC1_SDL_FRAME_WIDTH;
        if ((double)height / WC1_SDL_FRAME_HEIGHT < fractionalScale)
            fractionalScale = (double)height / WC1_SDL_FRAME_HEIGHT;
        *viewportWidth = (int)(WC1_SDL_FRAME_WIDTH * fractionalScale);
        *viewportHeight = (int)(WC1_SDL_FRAME_HEIGHT * fractionalScale);
    }
    if (*viewportWidth < 1)
        *viewportWidth = 1;
    if (*viewportHeight < 1)
        *viewportHeight = 1;
    *left = (width - *viewportWidth) / 2;
    *bottom = (height - *viewportHeight) / 2;
}

static int Wc1SdlGetWindowViewport(SDL_Window *window, int *left, int *top,
                                   int *viewportWidth, int *viewportHeight)
{
    int bottom;
    int height;
    int width;

    SDL_GetWindowSize(window, &width, &height);
    if (width < 1 || height < 1)
        return 0;
    *left = 0;
    *top = 0;
    *viewportWidth = width;
    *viewportHeight = height;
    if (!Wc1SdlUsingGlRenderer())
        return 1;
    Wc1SdlCalculateOutputViewport(width, height, left, &bottom,
                                  viewportWidth, viewportHeight);
    *top = height - bottom - *viewportHeight;
    return 1;
}

int Wc1SdlMapLogicalToWindow(SDL_Window *window, int logicalX, int logicalY,
                             int *windowX, int *windowY)
{
    SDL_Renderer *renderer;
    int viewportHeight;
    int viewportLeft;
    int viewportTop;
    int viewportWidth;

    if (window == 0 || windowX == 0 || windowY == 0)
        return 0;
    renderer = SDL_GetRenderer(window);
    if (renderer != 0) {
        SDL_RenderLogicalToWindow(renderer, (float)logicalX, (float)logicalY,
                                  windowX, windowY);
        return 1;
    }
    if (!Wc1SdlGetWindowViewport(window, &viewportLeft, &viewportTop,
                                 &viewportWidth, &viewportHeight))
        return 0;
    *windowX = viewportLeft + logicalX * viewportWidth / WC1_SDL_FRAME_WIDTH;
    *windowY = viewportTop + logicalY * viewportHeight / WC1_SDL_FRAME_HEIGHT;
    return 1;
}

int Wc1SdlMapWindowToLogical(SDL_Window *window, int windowX, int windowY,
                             int *logicalX, int *logicalY)
{
    SDL_Renderer *renderer;
    float mappedX;
    float mappedY;
    int viewportHeight;
    int viewportLeft;
    int viewportTop;
    int viewportWidth;

    if (window == 0 || logicalX == 0 || logicalY == 0)
        return 0;
    renderer = SDL_GetRenderer(window);
    if (renderer != 0) {
        SDL_RenderWindowToLogical(renderer, windowX, windowY, &mappedX,
                                  &mappedY);
        *logicalX = (int)mappedX;
        *logicalY = (int)mappedY;
        return 1;
    }
    if (!Wc1SdlGetWindowViewport(window, &viewportLeft, &viewportTop,
                                 &viewportWidth, &viewportHeight))
        return 0;
    *logicalX = (windowX - viewportLeft) * WC1_SDL_FRAME_WIDTH / viewportWidth;
    *logicalY =
        (windowY - viewportTop) * WC1_SDL_FRAME_HEIGHT / viewportHeight;
    return 1;
}
