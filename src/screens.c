/*
 *  Full-screen presentation screens.
 *
 *  Address range 0x437000-0x43bfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: unbroken run of Blit* screens plus ShowGetReady/Victory/GameOver.
 */
#include "wc1.h"

/* Function start: 0x438B90 */
unsigned int BeginBriefingScene(void)
{
    DAT_00598c18 = (unsigned int)AllocateTaggedMemory(0x140, 0);
    DAT_00598af4 = (unsigned int)AllocateTaggedMemory(0x140, 0);
    return 0;
}

/* Function start: 0x439400 */
void SetViewportRect(int p, unsigned short a, unsigned short b,
                       unsigned short c, unsigned short d)
{
    *(unsigned short *)(p + 8) = a;
    *(unsigned short *)(p + 10) = b;
    *(unsigned short *)(p + 0xc) = c;
    *(unsigned short *)(p + 0xe) = d;
}

/* Function start: 0x439E39 */
unsigned int DrawClippedLine(RasterClip *clip, int x1, int y1, int x2, int y2,
                             int mode, int colour)
{
    RasterSurface *surface;
    unsigned char *pixel;
    int stride;
    int clipLeft;
    int clipTop;
    int clipRight;
    int clipBottom;
    int deltaX;
    int deltaY;
    int stepX;
    int stepY;
    int error;
    int doubled;

    surface = clip->surface;
    if (surface == 0 || surface->pixels == 0 ||
        surface->maximumX < 0 || surface->maximumY < 0)
        return 0xffffffff;
    clipLeft = clip->left < 0 ? 0 : clip->left;
    clipTop = clip->top < 0 ? 0 : clip->top;
    clipRight = clip->right < surface->maximumX ?
        clip->right : surface->maximumX;
    clipBottom = clip->bottom < surface->maximumY ?
        clip->bottom : surface->maximumY;
    if (clipRight < clipLeft || clipBottom < clipTop)
        return 0xfffffffe;
    x1 += clip->left;
    x2 += clip->left;
    y1 += clip->top;
    y2 += clip->top;
    stride = surface->maximumX + 1;
    deltaX = AbsInt(x2 - x1);
    deltaY = AbsInt(y2 - y1);
    stepX = x1 < x2 ? 1 : -1;
    stepY = y1 < y2 ? 1 : -1;
    error = deltaX - deltaY;
    for (;;) {
        if (clipLeft <= x1 && x1 <= clipRight &&
            clipTop <= y1 && y1 <= clipBottom) {
            pixel = surface->pixels + y1 * stride + x1;
            if (mode == 0)
                *pixel = (unsigned char)colour;
            else if (mode == 1)
                *pixel = *(unsigned char *)(colour + *pixel);
        }
        if (x1 == x2 && y1 == y2)
            break;
        doubled = error * 2;
        if (doubled > -deltaY) {
            error -= deltaY;
            x1 += stepX;
        }
        if (doubled < deltaX) {
            error += deltaX;
            y1 += stepY;
        }
    }
    return 1;
}

/* Function start: 0x43AE3F */
/* The segment-register setup and fixed-size REP copy identify this as one of
 * the original hand-written raster support routines. */
__declspec(naked) void SetPaletteTranslationTable(
    const unsigned char *translation)
{
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov edi, offset g_abRasterPaletteTranslation_0046ff2c
        mov ecx, 0x40
        rep movsd
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
