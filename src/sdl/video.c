#include "video_internal.h"

static SDL_Renderer *g_pSdlRenderer;
static SDL_Texture *g_pSdlFrameTexture;
static Uint32 g_adwSdlFramePixels[
    WC1_SDL_FRAME_WIDTH * WC1_SDL_FRAME_HEIGHT];

int Wc1SdlInitializeVideo(SDL_Window *window)
{
    Wc1SdlShutdownVideo();
    if (Wc1SdlUsingGlRenderer())
        return Wc1SdlGlRendererInitialize(window);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    g_pSdlRenderer =
        SDL_CreateRenderer(window, -1,
                           SDL_RENDERER_ACCELERATED |
                               SDL_RENDERER_PRESENTVSYNC);
    if (g_pSdlRenderer == 0)
        g_pSdlRenderer =
            SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (g_pSdlRenderer == 0)
        return 0;
    if (SDL_RenderSetLogicalSize(g_pSdlRenderer, WC1_SDL_FRAME_WIDTH,
                                 WC1_SDL_FRAME_HEIGHT) != 0) {
        Wc1SdlShutdownVideo();
        return 0;
    }
    if (SDL_RenderSetIntegerScale(g_pSdlRenderer, SDL_TRUE) != 0) {
        Wc1SdlShutdownVideo();
        return 0;
    }
    g_pSdlFrameTexture =
        SDL_CreateTexture(g_pSdlRenderer, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING,
                          WC1_SDL_FRAME_WIDTH,
                          WC1_SDL_FRAME_HEIGHT);
    if (g_pSdlFrameTexture == 0) {
        Wc1SdlShutdownVideo();
        return 0;
    }
    SDL_SetRenderDrawColor(g_pSdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(g_pSdlRenderer);
    SDL_RenderPresent(g_pSdlRenderer);
    return 1;
}

void Wc1SdlShutdownVideo(void)
{
    Wc1SdlGlRendererShutdown();
    if (g_pSdlFrameTexture != 0) {
        SDL_DestroyTexture(g_pSdlFrameTexture);
        g_pSdlFrameTexture = 0;
    }
    if (g_pSdlRenderer != 0) {
        SDL_DestroyRenderer(g_pSdlRenderer);
        g_pSdlRenderer = 0;
    }
}

int Wc1SdlPresentIndexedFrame(const unsigned char *pixels,
                              const unsigned char *palette)
{
    int pixel;

    if (Wc1SdlUsingGlRenderer())
        return Wc1SdlGlRendererPresent(pixels, palette);
    if (g_pSdlRenderer == 0 || g_pSdlFrameTexture == 0 || pixels == 0 ||
        palette == 0)
        return 0;
    pixel = 0;
    while (pixel < WC1_SDL_FRAME_WIDTH * WC1_SDL_FRAME_HEIGHT) {
        int paletteOffset;
        unsigned char colour;

        colour = pixels[pixel];
        paletteOffset = colour * 4;
        g_adwSdlFramePixels[pixel] =
            0xff000000U |
            (Uint32)palette[paletteOffset + 2] << 16 |
            (Uint32)palette[paletteOffset + 1] << 8 |
            palette[paletteOffset];
        pixel++;
    }
    if (SDL_UpdateTexture(g_pSdlFrameTexture, 0, g_adwSdlFramePixels,
                          WC1_SDL_FRAME_WIDTH *
                              (int)sizeof(Uint32)) != 0)
        return 0;
    if (SDL_RenderClear(g_pSdlRenderer) != 0)
        return 0;
    if (SDL_RenderCopy(g_pSdlRenderer, g_pSdlFrameTexture, 0, 0) != 0)
        return 0;
    SDL_RenderPresent(g_pSdlRenderer);
    return 1;
}

void Wc1SdlWaitForVerticalBlank(void)
{
    if (Wc1SdlUsingGlRenderer()) {
        Wc1SdlGlRendererWaitForVerticalBlank();
        return;
    }
    if (g_pSdlRenderer == 0 || g_pSdlFrameTexture == 0) {
        SDL_Delay(1);
        return;
    }
    SDL_RenderClear(g_pSdlRenderer);
    SDL_RenderCopy(g_pSdlRenderer, g_pSdlFrameTexture, 0, 0);
    SDL_RenderPresent(g_pSdlRenderer);
}

void Wc1SdlBeginSpaceFrame(
    const struct ScreenViewportGeometry *geometry, int viewportMode,
    int fullViewportCopy, unsigned char clearColour)
{
    if (Wc1SdlUsingGlRenderer()) {
        Wc1SdlGlRendererBeginSpaceFrame(
            geometry, viewportMode, fullViewportCopy, clearColour);
    }
}

void Wc1SdlCompleteSpaceFrame(void)
{
    if (Wc1SdlUsingGlRenderer())
        Wc1SdlGlRendererCompleteSpaceFrame();
}

void Wc1SdlCancelSpaceFrame(void)
{
    if (Wc1SdlUsingGlRenderer())
        Wc1SdlGlRendererCancelSpaceFrame();
}

int Wc1SdlRecordSpaceSprite(
    const struct Viewport *viewport, float x, float y,
    unsigned char *shape, short frame, short angle, short scale,
    short flip)
{
    if (!Wc1SdlUsingGlRenderer())
        return 0;
    return Wc1SdlGlRendererRecordSpaceSprite(
        viewport, x, y, shape, frame, angle, scale, flip);
}
