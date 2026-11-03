/* SWC CODE_01 software texture mapping, presented through an SDL texture.
 * The original Q8 edge/span arithmetic decides coverage and texels. SDL
 * only copies the completed pixels at 1:1 logical resolution. The optional
 * OpenGL path uses the same Q8 corners with output-resolution triangles. */
#include "swc.h"
#include "video_internal.h"

typedef struct SwcTextureVertex {
    int x;
    int y;
    int u;
    int v;
} SwcTextureVertex;

typedef struct SwcTextureEdge {
    int x;
    int u;
    int v;
} SwcTextureEdge;

SDL_Renderer *SdlCreateSwcRenderer(SDL_Window *window)
{
    SDL_RendererInfo driver;
    int index;

    if (!SdlUsingGlRenderer())
        return SDL_CreateRenderer(window, -1, 0);
    /* SWC scenes use SDL textures and render targets. SDL's OpenGL backend
     * owns their context; WC1's indexed-frame GL context is not involved. */
    for (index = 0; index < SDL_GetNumRenderDrivers(); index++) {
        if (SDL_GetRenderDriverInfo(index, &driver) == 0 &&
            SDL_strcmp(driver.name, "opengl") == 0)
            return SDL_CreateRenderer(window, index,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    }
    SDL_SetError("The SDL OpenGL renderer is unavailable");
    return NULL;
}

int SdlLoadSwcRasterImage(const SwcBuffer *set, uint32_t index, SwcRasterImage *image)
{
    SwcFrame frame;

    if (SwcGetFramePtr(set, index, &frame) != 0)
        return -1;
    image->width = frame.width;
    image->height = frame.height;
    if (frame.width == 0 || frame.height == 0)
        return 0;
    image->pixels = SDL_malloc((size_t)frame.width * frame.height);
    if (image->pixels == NULL)
        return SDL_OutOfMemory();
    if (SwcDecodeFrame(&frame, image->pixels, (size_t)frame.width * frame.height) != 0) {
        SDL_free(image->pixels);
        image->pixels = NULL;
        return -1;
    }
    return 0;
}

void SdlFreeSwcRasterImage(SwcRasterImage *image)
{
    SDL_DestroyTexture(image->texture);
    SDL_free(image->pixels);
    SDL_zero(*image);
}

static int SwcDrawEnhancedPolygon(SDL_Renderer *renderer, SwcRasterImage *image,
                                    const SDL_Color colors[256], const SDL_Point corners[4])
{
    static const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_Vertex vertices[4];
    SDL_Surface *surface;
    uint32_t *row;
    SDL_Color color;
    uint8_t ink;
    int x;
    int y;
    int corner;

    if (image->texture == NULL) {
        surface = SDL_CreateRGBSurfaceWithFormat(0, image->width, image->height,
                                                 32, SDL_PIXELFORMAT_ARGB8888);
        if (surface == NULL)
            return -1;
        for (y = 0; y < image->height; y++) {
            row = (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch);
            for (x = 0; x < image->width; x++) {
                ink = image->pixels[(size_t)y * image->width + x];
                color = colors[ink];
                row[x] = ink == 0 ? 0 : UINT32_C(0xff000000) |
                    (uint32_t)color.r << 16 | (uint32_t)color.g << 8 | color.b;
            }
        }
        /* Each image uses the fixed flight palette for its lifetime. Keep
         * nearest sampling so transparent edges retain the original colours. */
        image->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (image->texture == NULL)
            return -1;
        if (SDL_SetTextureBlendMode(image->texture, SDL_BLENDMODE_BLEND) != 0 ||
            SDL_SetTextureScaleMode(image->texture, SDL_ScaleModeNearest) != 0) {
            SDL_DestroyTexture(image->texture);
            image->texture = NULL;
            return -1;
        }
    }
    for (corner = 0; corner < 4; corner++) {
        vertices[corner].position = (SDL_FPoint){corners[corner].x / 256.0f,
                                                corners[corner].y / 256.0f};
        vertices[corner].color = (SDL_Color){255, 255, 255, 255};
        vertices[corner].tex_coord = (SDL_FPoint){
            corner == 1 || corner == 2 ? 1.0f : 0.0f,
            corner >= 2 ? 1.0f : 0.0f};
    }
    return SDL_RenderGeometry(renderer, image->texture, vertices, 4, indices, 6);
}

/* CODE_01 adjust +0x5a28. Arithmetic shifts retain negative-coordinate
 * rounding; do not substitute truncating division by 256. */
static int SwcRoundCoordinate(int value)
{
    if ((value & 255) == 128)
        value++;
    return (value + 128) >> 8;
}

/* CODE_01 edge +0x5d00. End rows are excluded and the increments use
 * max(2, rows-1), including the original behavior of very small quads. */
static void SwcRasterEdge(SwcTextureEdge rows[SWC_FRAME_HEIGHT],
                            const SwcTextureVertex *start, const SwcTextureVertex *end)
{
    const SwcTextureVertex *swap;
    int first;
    int last;
    int steps;
    int dx;
    int du;
    int dv;
    int y;

    if (end->y < start->y) {
        swap = start;
        start = end;
        end = swap;
    }
    first = SwcRoundCoordinate(start->y);
    last = SwcRoundCoordinate(end->y);
    if (first == last)
        return;
    steps = SDL_max(2, last - first - 1);
    dx = (end->x - start->x) / steps;
    du = (end->u - start->u) / steps;
    dv = (end->v - start->v) / steps;
    /* Skipping offscreen rows is equivalent to advancing the original
     * accumulators, without walking a potentially enormous offscreen edge. */
    for (y = SDL_max(0, first); y < SDL_min(last, SWC_FRAME_HEIGHT); y++) {
        rows[y].x = start->x + (int)((int64_t)dx * (y - first));
        rows[y].u = start->u + (int)((int64_t)du * (y - first));
        rows[y].v = start->v + (int)((int64_t)dv * (y - first));
    }
}

/* CODE_01 span +0x5a5a and CODE_09 texture loop +0x38d8. Clip UV at
 * the unrounded edges, then sample integer texels and skip palette zero.
 * The loop draws max(2, span-1) pixels, not the rounded span width. */
static void SwcRasterSpan(uint32_t *pixels, const SwcRasterImage *image,
                            const uint32_t palette[256], const SwcTextureEdge *left,
                            const SwcTextureEdge *right)
{
    const SwcTextureEdge *swap;
    int x;
    int end;
    int count;
    int width;
    int u;
    int v;
    int endU;
    int endV;
    int du;
    int dv;
    int column;
    int row;
    uint8_t ink;

    if (right->x < left->x) {
        swap = left;
        left = right;
        right = swap;
    }
    x = SwcRoundCoordinate(left->x);
    end = SwcRoundCoordinate(right->x);
    if (end < 0 || x >= SWC_FRAME_WIDTH || x == end)
        return;
    width = right->x - left->x;
    if (width == 0)
        return;
    u = left->u;
    v = left->v;
    endU = right->u;
    endV = right->v;
    if (left->x < 0) {
        u += (int)((int64_t)(right->u - left->u) * -left->x / width);
        v += (int)((int64_t)(right->v - left->v) * -left->x / width);
        x = 0;
        if (end == 0)
            return;
    }
    if (right->x >= SWC_FRAME_WIDTH * 256) {
        endU -= (int)((int64_t)(right->u - left->u) *
            (right->x - SWC_FRAME_WIDTH * 256) / width);
        endV -= (int)((int64_t)(right->v - left->v) *
            (right->x - SWC_FRAME_WIDTH * 256) / width);
        end = SWC_FRAME_WIDTH - 1;
        if (x == end)
            return;
    }
    count = SDL_max(2, end - x - 1);
    du = (endU - u) / count;
    dv = (endV - v) / count;
    for (; count > 0 && x < SWC_FRAME_WIDTH; count--, x++) {
        column = u >> 8;
        row = v >> 8;
        /* Tiny or clipped polygons can extrapolate outside the source.
         * Never reproduce the original renderer's out-of-buffer reads. */
        if (column >= 0 && column < image->width && row >= 0 && row < image->height) {
            ink = image->pixels[(size_t)row * image->width + column];
            if (ink != 0)
                pixels[x] = palette[ink];
        }
        u += du;
        v += dv;
    }
}

/* CODE_01 poly +0x5e3a and MacDrawcel2 +0x14a4. The first extrema
 * select the two edge chains. The bottom clip is 239, exclusive. */
int SdlDrawSwcPolygon(SDL_Renderer *renderer, SDL_Texture *canvas,
                        SwcRasterImage *image, const SDL_Color colors[256],
                        const SDL_Point corners[4])
{
    SwcTextureVertex vertices[4];
    SwcTextureEdge edges[2][SWC_FRAME_HEIGHT];
    uint32_t pixels[SWC_FRAME_HEIGHT][SWC_FRAME_WIDTH] = {{0}};
    uint32_t palette[256];
    int top = 0;
    int bottom = 0;
    int first;
    int last;
    int vertex;
    int next;
    int y;
    SDL_Rect region;

    if (image->pixels == NULL)
        return 0;
    if (SdlUsingGlRenderer())
        return SwcDrawEnhancedPolygon(renderer, image, colors, corners);
    for (vertex = 0; vertex < 4; vertex++) {
        vertices[vertex] = (SwcTextureVertex){corners[vertex].x, corners[vertex].y,
            (vertex == 1 || vertex == 2) ? (image->width - 1) * 256 : 0,
            vertex >= 2 ? (image->height - 1) * 256 : 0};
        if (corners[vertex].y < corners[top].y)
            top = vertex;
        if (corners[vertex].y > corners[bottom].y)
            bottom = vertex;
    }
    first = SwcRoundCoordinate(corners[top].y);
    last = SwcRoundCoordinate(corners[bottom].y);
    if (first > SWC_FRAME_HEIGHT || last < 0 || first == last)
        return 0;
    vertex = top;
    do {
        next = (vertex + 1) % 4;
        SwcRasterEdge(edges[0], &vertices[vertex], &vertices[next]);
        vertex = next;
    } while (vertex != bottom);
    do {
        next = (vertex + 1) % 4;
        SwcRasterEdge(edges[1], &vertices[vertex], &vertices[next]);
        vertex = next;
    } while (vertex != top);
    first = SDL_max(0, first);
    last = SDL_min(SWC_FRAME_HEIGHT - 1, last);
    if (first >= last)
        return 0;
    for (vertex = 0; vertex < 256; vertex++)
        palette[vertex] = UINT32_C(0xff000000) | (uint32_t)colors[vertex].r << 16 |
            (uint32_t)colors[vertex].g << 8 | colors[vertex].b;
    for (y = first; y < last; y++)
        SwcRasterSpan(pixels[y], image, palette, &edges[0][y], &edges[1][y]);
    region = (SDL_Rect){0, first, SWC_FRAME_WIDTH, last - first};
    if (SDL_UpdateTexture(canvas, &region, pixels[first], sizeof(pixels[0])) != 0)
        return -1;
    return SDL_RenderCopy(renderer, canvas, &region, &region);
}

/* CODE_02 Cos +0x557c, Sin +0x5608 and DATA/0 +0x4550. These short
 * MacsBug names are absent from the older exported symbol list. */
static int SwcCos(int angle)
{
    static const unsigned short quadrant[91] = {
        256, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250,
        249, 248, 247, 246, 244, 243, 242, 240, 238, 237, 235, 233, 232,
        230, 228, 226, 223, 221, 219, 217, 214, 212, 209, 207, 204, 201,
        198, 196, 193, 190, 187, 184, 181, 177, 174, 171, 167, 164, 161,
        157, 154, 150, 146, 143, 139, 135, 131, 128, 124, 120, 116, 112,
        108, 104, 100, 95, 91, 87, 83, 79, 74, 70, 66, 61, 57, 53,
        48, 44, 40, 35, 31, 26, 22, 17, 13, 8, 4, 0
    };

    angle %= 360;
    if (angle < 0)
        angle += 360;
    if (angle <= 90)
        return quadrant[angle];
    if (angle <= 180)
        return -quadrant[180 - angle];
    if (angle <= 270)
        return -quadrant[angle - 180];
    return quadrant[360 - angle];
}

/* CODE_01 get_Mac_boundaries +0x0ae2 uses Q16 corners, retaining the
 * low scale bit that rotate_Mac_shape2 discards. All four extrema are
 * floored; the right and bottom coordinates are not rounded outward. */
static SDL_Rect SwcMacSpriteBounds(const SwcRasterImage *image, int x, int y,
                                    int cosine, int sine, int scale)
{
    SDL_Rect bounds;
    int64_t halfWidth = (int64_t)image->width * scale * 128;
    int64_t halfHeight = (int64_t)image->height * scale * 128;
    int64_t dx;
    int64_t dy;
    int px;
    int py;
    int right = 0;
    int bottom = 0;
    int corner;

    bounds.x = bounds.y = 0;
    for (corner = 0; corner < 4; corner++) {
        dx = corner == 1 || corner == 2 ? halfWidth : -halfWidth;
        dy = corner >= 2 ? halfHeight : -halfHeight;
        px = (((int64_t)x * 65536 + ((dx * cosine) >> 8) +
            ((dy * sine) >> 8)) >> 16);
        py = (((int64_t)y * 65536 + ((dy * cosine) >> 8) +
            ((dx * -sine) >> 8)) >> 16);
        if (corner == 0) {
            bounds.x = right = px;
            bounds.y = bottom = py;
        } else {
            bounds.x = SDL_min(bounds.x, px);
            bounds.y = SDL_min(bounds.y, py);
            right = SDL_max(right, px);
            bottom = SDL_max(bottom, py);
        }
    }
    bounds.w = right - bounds.x;
    bounds.h = bottom - bounds.y;
    return bounds;
}

/* CODE_01 rotate_Mac_shape2 +0x10ae: center the frame, invert angle,
 * truncate scale before multiplying dimensions, and swap XY for flips.
 * UV endpoints are width/height minus one, not normalized SDL corners. */
int SdlDrawSwcRotatedImage(SDL_Renderer *renderer, SDL_Texture *canvas,
                            SwcRasterImage *image, const SDL_Color colors[256],
                            int x, int y, int angle, int scale, int flip, SDL_Rect *bounds)
{
    SDL_Point corners[4];
    int cosine = SwcCos(360 - angle);
    int sine = SwcCos(270 - angle);
    int halfWidth = image->width * (scale >> 1);
    int halfHeight = image->height * (scale >> 1);
    int dx;
    int dy;
    int corner;
    int destination;

    for (corner = 0; corner < 4; corner++) {
        dx = corner == 1 || corner == 2 ? halfWidth : -halfWidth;
        dy = corner >= 2 ? halfHeight : -halfHeight;
        destination = corner;
        if (flip & 0x10)
            destination ^= 1;
        if (flip & 0x20)
            destination = 3 - destination;
        corners[destination].x = x * 256 + (int)(((int64_t)dx * cosine) >> 8) +
            (int)(((int64_t)dy * sine) >> 8);
        corners[destination].y = y * 256 + (int)(((int64_t)dy * cosine) >> 8) +
            (int)(((int64_t)dx * -sine) >> 8);
    }
    if (bounds != NULL)
        *bounds = SwcMacSpriteBounds(image, x, y, cosine, sine, scale);
    return SdlDrawSwcPolygon(renderer, canvas, image, colors, corners);
}
