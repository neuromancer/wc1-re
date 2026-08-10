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
void __stdcall SetViewportRect(Viewport *viewport, unsigned short left,
                               unsigned short top, unsigned short right,
                               unsigned short bottom)
{
    viewport->left = (short)left;
    viewport->top = (short)top;
    viewport->right = (short)right;
    viewport->bottom = (short)bottom;
}

/* Function start: 0x439840 */
unsigned int ShowGetReadyScreen(void)
{
    short frame;
    short distance;

    frame = 0;
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    distance = 400;
    g_nCannedSceneMode_00469fac = 1;
    force_view(0, 0);
    DAT_00469fb4 = 1;
    DAT_0059ab58 = 0;
    do {
        if (RefreshCockpitStatus() != 0) {
            DrawCenteredScaledIntroText(
                "Get Ready", g_nViewCenterX_0059a852,
                g_nViewCenterY_0059a854,
                (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (distance > 100)
            distance = (short)(distance - 10);
        if (DAT_0059ab58 == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 40);
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
    clear_view_buffer();
    g_nCannedSceneMode_00469fac = 0;
    ResetSoundState();
    return 0;
}

/* Function start: 0x439910 */
unsigned int ShowVictoryScreen(void)
{
    short frame;
    short distance;

    frame = 0;
    distance = 500;
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    do {
        if (RefreshCockpitStatus() != 0) {
            DrawCenteredScaledIntroText(
                "Victory", g_nViewCenterX_0059a852,
                g_nViewCenterY_0059a854,
                (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (distance > 100)
            distance = (short)(distance - 10);
        if (DAT_0059ab58 == 1)
            break;
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 80);
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
    return 0;
}

/* Function start: 0x439A80 */
unsigned int ShowGameOverScreen(void)
{
    FixedVector cameraOffset;
    short frame;
    short distance;

    frame = 0;
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    g_cViewObject_0046c000 = (signed char)Explosion(0);
    DAT_0046c03c = 4;
    g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] = 100;
    ScaleFixedVector(
        &g_aShipForwardVector_0059bce0[g_cViewObject_0046c000],
        -0x12c00, &cameraOffset);
    AddFixedVectors(
        &g_aShipPosition_0059c490[g_cViewObject_0046c000],
        &cameraOffset, &g_aShipPosition_0059c490[WC1_EYE_OBJECT]);
    g_aShipUpVector_0059b9e0[WC1_EYE_OBJECT] =
        g_aShipUpVector_0059b9e0[g_cViewObject_0046c000];
    g_aShipForwardVector_0059bce0[WC1_EYE_OBJECT] = cameraOffset;
    fix_objects_ijk(WC1_EYE_OBJECT);
    zero_vector(&g_aShipVelocity_0059c010[WC1_EYE_OBJECT]);
    set_eye_direction_and_position();
    distance = 700;
    generate_stars();
    StartMusicTrack(22, 2, 1);
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    do {
        if (RefreshCockpitStatus() != 0) {
            if (frame > 20)
                DrawCenteredScaledIntroText(
                    "Game Over", g_nViewCenterX_0059a852,
                    g_nViewCenterY_0059a854,
                    (short)(0xc800 / (int)distance));
            dump_buffer_to_screen();
        }
        if (DAT_0059ab58 == 1)
            break;
        if (distance > 100)
            distance = (short)(distance - 10);
        frame++;
        DIBslam();
        DIBslamReal();
    } while (frame < 80);
    StopMusicUnlessSuppressed();
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
    return 0;
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
