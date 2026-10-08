/* SDL2 bootstrap for the Mac demo's graphics. This is an asset viewer, not
 * the reconstructed FlyingLoop. It does not use WC1's 320x200 render globals. */
#include "swc.h"

#include <stdio.h>
#include <string.h>

static const char *const shipTypes[] = {
    "SH04", "SH05", "SH06", "SH07", "SH08", "SH14", "SH15",
    "SH16", "SH17", "SH18", "SH19", "SH20", "SH21"
};

static int SwcLoadPreviewFrame(const SwcBuffer *set, uint32_t index,
                                SwcFrame *frame, SwcBuffer *pixels)
{
    SDL_free(pixels->data);
    memset(pixels, 0, sizeof(*pixels));
    if (SwcGetFramePtr(set, index, frame) != 0)
        return -1;
    pixels->size = (size_t)frame->width * frame->height;
    pixels->data = SDL_malloc(pixels->size != 0 ? pixels->size : 1);
    if (pixels->data == NULL)
        return SDL_OutOfMemory();
    return SwcDecodeFrame(frame, pixels->data, pixels->size);
}

static void SwcComposePreview(uint32_t *screen, const SDL_Color colors[256],
                               const SwcFrame *shipFrame,
                               const SwcBuffer *shipPixels,
                               const SwcBuffer *cockpitPixels, int showCockpit)
{
    int x;
    int y;
    int shipX;
    int shipY;
    size_t pixel;
    uint8_t color;

    /* Center each extracted view for inspection. Sprite hotspot projection
     * and flight scaling still belong to the future game renderer. */
    shipX = 160 - shipFrame->width / 2;
    shipY = 80 - shipFrame->height / 2;
    for (y = 0; y < SWC_FRAME_HEIGHT; y++) {
        for (x = 0; x < SWC_FRAME_WIDTH; x++) {
            pixel = (size_t)y * SWC_FRAME_WIDTH + x;
            color = 255;
            if (x >= shipX && x - shipX < shipFrame->width &&
                y >= shipY && y - shipY < shipFrame->height) {
                color = shipPixels->data[(size_t)(y - shipY) *
                                         shipFrame->width + x - shipX];
                if (color == 0)
                    color = 255;
            }
            if (showCockpit && cockpitPixels->data[pixel] != 0)
                color = cockpitPixels->data[pixel];
            screen[pixel] = UINT32_C(0xff000000) |
                            (uint32_t)colors[color].r << 16 |
                            (uint32_t)colors[color].g << 8 | colors[color].b;
        }
    }
}

int main(int argc, char **argv)
{
    SwcCmf cockpitCmf = {0};
    SwcCmf flightCmf = {0};
    SwcBuffer cockpitSet = {0};
    SwcBuffer shipSet = {0};
    SwcBuffer cockpitPixels = {0};
    SwcBuffer shipPixels = {0};
    SwcFrame cockpitFrame;
    SwcFrame shipFrame;
    SDL_Color colors[256];
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Surface *snapshot = NULL;
    SDL_Event event;
    uint32_t *screen = NULL;
    uint32_t frameCount;
    uint32_t frameIndex;
    const char *dataDirectory;
    const char *resourceFork;
    const char *snapshotPath;
    char *path = NULL;
    size_t pathSize;
    char title[192];
    int argument;
    int checkOnly;
    int shipIndex;
    int previousShip;
    int showCockpit;
    int done;
    int dirty;
    int result;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s DEMO_DIRECTORY [--rsrc RAW_RESOURCE_FORK] "
               "[--check] [--snapshot FILE.bmp]\n"
               "Left/right: ship view. Up/down: ship. C: cockpit. Esc: quit.\n"
               "This SDL2 asset preview does not run missions yet.\n", argv[0]);
        return argc < 2 ? 1 : 0;
    }
    dataDirectory = argv[1];
    resourceFork = NULL;
    snapshotPath = NULL;
    checkOnly = 0;
    for (argument = 2; argument < argc; argument++) {
        if (strcmp(argv[argument], "--check") == 0) {
            checkOnly = 1;
        } else if (strcmp(argv[argument], "--rsrc") == 0 && argument + 1 < argc) {
            resourceFork = argv[++argument];
        } else if (strcmp(argv[argument], "--snapshot") == 0 && argument + 1 < argc) {
            snapshotPath = argv[++argument];
            checkOnly = 1;
        } else {
            fprintf(stderr, "Unknown or incomplete option: %s\n", argv[argument]);
            return 1;
        }
    }
    result = 1;
    pathSize = strlen(dataDirectory) + 96;
    path = SDL_malloc(pathSize);
    if (path == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    SDL_snprintf(path, pathSize, "%s/CMFs/PCShipV00.CMF", dataDirectory);
    if (SwcCMOpen(path, &cockpitCmf) != 0)
        goto done;
    SDL_snprintf(path, pathSize, "%s/CMFs/Spaceflight.CMF", dataDirectory);
    if (SwcCMOpen(path, &flightCmf) != 0)
        goto done;
    if (resourceFork == NULL) {
        SDL_snprintf(path, pathSize, "%s/SuperWingCommanderDemo.rsrc", dataDirectory);
        if (SwcReadPalette(path, 251, colors) != 0) {
            SDL_snprintf(path, pathSize,
                         "%s/Super Wing Commander Demo/..namedfork/rsrc", dataDirectory);
            if (SwcReadPalette(path, 251, colors) != 0) {
                SDL_SetError("Cannot load clut/251; use --rsrc with the demo's "
                             "extracted raw resource fork");
                goto done;
            }
        }
    } else if (SwcReadPalette(resourceFork, 251, colors) != 0) {
        goto done;
    }
    if (SwcCMGetChunk(&cockpitCmf, "PC00", 1, &cockpitSet) != 0 ||
        SwcLoadPreviewFrame(&cockpitSet, 0, &cockpitFrame, &cockpitPixels) != 0)
        goto done;
    if (cockpitFrame.width != SWC_FRAME_WIDTH || cockpitFrame.height != SWC_FRAME_HEIGHT) {
        SDL_SetError("Expected the demo's 320x240 cockpit frame");
        goto done;
    }
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        goto done;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    window = SDL_CreateWindow("Super Wing Commander Mac demo - asset preview",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              960, 720, checkOnly ? SDL_WINDOW_HIDDEN : SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        goto done;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL ||
        SDL_RenderSetLogicalSize(renderer, SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT) != 0)
        goto done;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT);
    if (texture == NULL)
        goto done;
    screen = SDL_malloc(SWC_FRAME_WIDTH * SWC_FRAME_HEIGHT * sizeof(*screen));
    if (screen == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    shipIndex = 0;
    previousShip = -1;
    frameIndex = 0;
    frameCount = 0;
    showCockpit = 1;
    done = 0;
    dirty = 1;
    while (!done) {
        if (previousShip != shipIndex) {
            SDL_free(shipSet.data);
            memset(&shipSet, 0, sizeof(shipSet));
            if (SwcCMGetChunk(&flightCmf, shipTypes[shipIndex], 1, &shipSet) != 0 ||
                SwcGetFrameCount(&shipSet, &frameCount) != 0)
                goto done;
            if (frameCount == 0) {
                SDL_SetError("Empty ship frame set");
                goto done;
            }
            frameIndex = 0;
            previousShip = shipIndex;
            dirty = 1;
        }
        if (dirty) {
            if (SwcLoadPreviewFrame(&shipSet, frameIndex, &shipFrame, &shipPixels) != 0)
                goto done;
            SwcComposePreview(screen, colors, &shipFrame, &shipPixels,
                               &cockpitPixels, showCockpit);
            if (SDL_UpdateTexture(texture, NULL, screen, SWC_FRAME_WIDTH * 4) != 0)
                goto done;
            SDL_snprintf(title, sizeof(title),
                         "SWC Mac demo preview | %s view %u/%u | arrows: browse | C: cockpit | Esc: quit",
                         shipTypes[shipIndex], frameIndex + 1, frameCount);
            SDL_SetWindowTitle(window, title);
            dirty = 0;
        }
        if (SDL_RenderClear(renderer) != 0 || SDL_RenderCopy(renderer, texture, NULL, NULL) != 0)
            goto done;
        SDL_RenderPresent(renderer);
        if (checkOnly) {
            if (snapshotPath != NULL) {
                snapshot = SDL_CreateRGBSurfaceWithFormatFrom(screen,
                    SWC_FRAME_WIDTH, SWC_FRAME_HEIGHT, 32, SWC_FRAME_WIDTH * 4,
                    SDL_PIXELFORMAT_ARGB8888);
                if (snapshot == NULL || SDL_SaveBMP(snapshot, snapshotPath) != 0)
                    goto done;
            }
            printf("SWC Mac demo: cockpit PC00/1, clut/251, %s/1 (%u views), 320x240 SDL2: OK\n",
                   shipTypes[shipIndex], frameCount);
            break;
        }
        if (SDL_WaitEventTimeout(&event, 50)) {
            if (event.type == SDL_QUIT) {
                done = 1;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    done = 1;
                    break;
                case SDLK_LEFT:
                    frameIndex = (frameIndex + frameCount - 1) % frameCount;
                    break;
                case SDLK_RIGHT:
                    frameIndex = (frameIndex + 1) % frameCount;
                    break;
                case SDLK_UP:
                    shipIndex = (shipIndex + 1) % SDL_arraysize(shipTypes);
                    break;
                case SDLK_DOWN:
                    shipIndex = (shipIndex + SDL_arraysize(shipTypes) - 1) % SDL_arraysize(shipTypes);
                    break;
                case SDLK_c:
                    showCockpit = !showCockpit;
                    break;
                default:
                    break;
                }
                dirty = 1;
            }
        }
    }
    result = 0;
done:
    if (result != 0)
        fprintf(stderr, "SWC demo: %s\n", SDL_GetError());
    SDL_FreeSurface(snapshot);
    SDL_free(screen);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    SDL_free(path);
    SDL_free(shipPixels.data);
    SDL_free(cockpitPixels.data);
    SDL_free(shipSet.data);
    SDL_free(cockpitSet.data);
    SwcCMClose(&flightCmf);
    SwcCMClose(&cockpitCmf);
    return result;
}
