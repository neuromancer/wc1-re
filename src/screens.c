/*
 *  Full-screen presentation screens.
 *
 *  Address range 0x437000-0x43cfff (provisional -- see docs/ORDER.md).
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
    short distance;
    short emptyCount;
    short frame;
    short index;

    emptyCount = 0;
    InitializeFireworks();
    g_pFireworkShape_005a6a68 =
        (unsigned char *)FetchDiskPacketRetrying(9, 17, 0);
    distance = 500;
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    frame = 0;
    DAT_0059ab58 = 0;
    DAT_00469fb4 = 1;
    do {
        if (RandomBelowOrEqual(7) == 0 && emptyCount != 0) {
            index = 0;
            do {
                if (g_aFireworks_005a6900[index].frame == -1) {
                    g_aFireworks_005a6900[index].frame = 0;
                    g_aFireworks_005a6900[index].x =
                        RandomInRange(0, DAT_005a7510.right);
                    g_aFireworks_005a6900[index].y =
                        RandomInRange(0, DAT_005a7510.bottom);
                    g_aFireworks_005a6900[index].variant =
                        RandomInRange(0, 2);
                    break;
                }
                index++;
            } while (index < 30);
        }
        if (RefreshCockpitStatus() != 0) {
            emptyCount = TheEndFireWorks(&DAT_005a7510, 30);
            DrawCenteredScaledIntroText(
                "Victory", g_nViewCenterX_0059a852,
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
    DAT_0059ab58 = 0;
    ReleasePacketHandle((int)g_pIntroFont_005a8960);
    ReleasePacketHandle((int)g_pFireworkShape_005a6a68);
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

/* Function start: 0x43C808 */
/* The explicit ES setup and REP stores identify the original as hand-written
 * raster assembly, so preserve that implementation rather than asking the C
 * compiler to synthesize a materially different fill loop. */
__declspec(naked) int FillRasterClip(RasterClip *clip, int colour)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -20h
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 18h], eax
        jle invalid_surface
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle invalid_surface
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 1ch], eax
        cmp eax, 0
        jg left_clipped
        mov eax, 0
left_clipped:
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 20h], eax
        cmp eax, 0
        jg top_clipped
        mov eax, 0
top_clipped:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 18h]
        dec edx
        cmp eax, edx
        jl right_clipped
        mov eax, edx
right_clipped:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl bottom_clipped
        mov eax, edx
bottom_clipped:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [ebp - 0ch]
        cmp eax, dword ptr [ebp - 4]
        jl empty_clip
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl empty_clip
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 14h], eax
        jmp begin_fill
invalid_surface:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
empty_clip:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
begin_fill:
        mov eax, dword ptr [ebp - 8]
        imul dword ptr [ebp - 18h]
        add eax, dword ptr [ebp - 14h]
        add eax, dword ptr [ebp - 4]
        mov edi, eax
        mov ebx, dword ptr [ebp - 0ch]
        inc ebx
        sub ebx, dword ptr [ebp - 4]
        mov esi, dword ptr [ebp - 18h]
        sub esi, ebx
        mov al, byte ptr [ebp + 0ch]
        mov ah, al
        shl eax, 10h
        mov al, byte ptr [ebp + 0ch]
        mov ah, al
        mov edx, dword ptr [ebp - 8]
        jmp test_row
fill_row:
        mov ecx, ebx
        and ecx, 3
        rep stosb
        mov ecx, ebx
        shr ecx, 2
        rep stosd
        add edi, esi
        inc edx
test_row:
        cmp edx, dword ptr [ebp - 10h]
        jle fill_row
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}

/* Function start: 0x43C8E7 */
/* This is the companion hand-written clipped blitter.  Direction-flag and
 * segment-register handling are part of its overlap-safe behavior. */
__declspec(naked) int BlitRasterClip(
    RasterClip *source, int sourceX, int sourceY,
    RasterClip *destination, int destinationX,
    int destinationY, unsigned int colour)
{
    __asm {
        push ebp
        mov ebp, esp
        add esp, -9ch
        push ebx
        push esi
        push edi
        push es
        cld
        push ds
        pop es
        mov esi, dword ptr [ebp + 8]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 70h], eax
        jle blit_invalid_source
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle blit_invalid_source
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 78h], eax
        cmp eax, 0
        jg blit_source_left
        mov eax, 0
blit_source_left:
        mov dword ptr [ebp - 60h], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 7ch], eax
        cmp eax, 0
        jg blit_source_top
        mov eax, 0
blit_source_top:
        mov dword ptr [ebp - 64h], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 70h]
        dec edx
        cmp eax, edx
        jl blit_source_right
        mov eax, edx
blit_source_right:
        mov dword ptr [ebp - 68h], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl blit_source_bottom
        mov eax, edx
blit_source_bottom:
        mov dword ptr [ebp - 6ch], eax
        mov eax, dword ptr [ebp - 68h]
        cmp eax, dword ptr [ebp - 60h]
        jl blit_empty_source
        mov eax, dword ptr [ebp - 6ch]
        cmp eax, dword ptr [ebp - 64h]
        jl blit_empty_source
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 74h], eax
        jmp blit_source_ready
blit_invalid_source:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_empty_source:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_source_ready:
        mov eax, dword ptr [ebp - 60h]
        mov dword ptr [ebp - 2ch], eax
        mov eax, dword ptr [ebp - 64h]
        mov dword ptr [ebp - 30h], eax
        mov eax, dword ptr [ebp - 68h]
        mov dword ptr [ebp - 34h], eax
        mov eax, dword ptr [ebp - 6ch]
        mov dword ptr [ebp - 38h], eax
        mov eax, dword ptr [ebp - 78h]
        sub dword ptr [ebp - 2ch], eax
        sub dword ptr [ebp - 34h], eax
        mov eax, dword ptr [ebp - 7ch]
        sub dword ptr [ebp - 30h], eax
        sub dword ptr [ebp - 38h], eax
        mov esi, dword ptr [ebp + 14h]
        mov ebx, dword ptr [esi]
        mov eax, dword ptr [ebx + 4]
        inc eax
        mov dword ptr [ebp - 94h], eax
        jle blit_invalid_destination
        mov eax, dword ptr [ebx + 8]
        inc eax
        mov ecx, eax
        jle blit_invalid_destination
        mov eax, dword ptr [esi + 4]
        mov dword ptr [ebp - 98h], eax
        cmp eax, 0
        jg blit_destination_left
        mov eax, 0
blit_destination_left:
        mov dword ptr [ebp - 80h], eax
        mov eax, dword ptr [esi + 8]
        mov dword ptr [ebp - 9ch], eax
        cmp eax, 0
        jg blit_destination_top
        mov eax, 0
blit_destination_top:
        mov dword ptr [ebp - 84h], eax
        mov eax, dword ptr [esi + 0ch]
        mov edx, dword ptr [ebp - 94h]
        dec edx
        cmp eax, edx
        jl blit_destination_right
        mov eax, edx
blit_destination_right:
        mov dword ptr [ebp - 88h], eax
        mov eax, dword ptr [esi + 10h]
        mov edx, ecx
        dec edx
        cmp eax, edx
        jl blit_destination_bottom
        mov eax, edx
blit_destination_bottom:
        mov dword ptr [ebp - 8ch], eax
        mov eax, dword ptr [ebp - 88h]
        cmp eax, dword ptr [ebp - 80h]
        jl blit_empty_destination
        mov eax, dword ptr [ebp - 8ch]
        cmp eax, dword ptr [ebp - 84h]
        jl blit_empty_destination
        mov eax, dword ptr [ebx]
        mov dword ptr [ebp - 90h], eax
        jmp blit_destination_ready
blit_invalid_destination:
        mov eax, -1
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_empty_destination:
        mov eax, -2
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_destination_ready:
        mov eax, dword ptr [ebp - 80h]
        mov dword ptr [ebp - 40h], eax
        mov eax, dword ptr [ebp - 84h]
        mov dword ptr [ebp - 44h], eax
        mov eax, dword ptr [ebp - 88h]
        mov dword ptr [ebp - 48h], eax
        mov eax, dword ptr [ebp - 8ch]
        mov dword ptr [ebp - 4ch], eax
        mov eax, dword ptr [ebp - 98h]
        sub dword ptr [ebp - 40h], eax
        sub dword ptr [ebp - 48h], eax
        mov eax, dword ptr [ebp - 9ch]
        sub dword ptr [ebp - 44h], eax
        sub dword ptr [ebp - 4ch], eax
        mov eax, dword ptr [ebp + 0ch]
        sub eax, dword ptr [ebp + 18h]
        mov dword ptr [ebp - 24h], eax
        mov eax, dword ptr [ebp + 10h]
        sub eax, dword ptr [ebp + 1ch]
        mov dword ptr [ebp - 28h], eax
        mov eax, dword ptr [ebp - 2ch]
        mov edx, dword ptr [ebp - 40h]
        add edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jg blit_copy_left
        mov eax, edx
blit_copy_left:
        mov dword ptr [ebp - 4], eax
        mov eax, dword ptr [ebp - 30h]
        mov edx, dword ptr [ebp - 44h]
        add edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jg blit_copy_top
        mov eax, edx
blit_copy_top:
        mov dword ptr [ebp - 8], eax
        mov eax, dword ptr [ebp - 34h]
        mov edx, dword ptr [ebp - 48h]
        add edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jl blit_copy_right
        mov eax, edx
blit_copy_right:
        mov dword ptr [ebp - 0ch], eax
        mov eax, dword ptr [ebp - 38h]
        mov edx, dword ptr [ebp - 4ch]
        add edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jl blit_copy_bottom
        mov eax, edx
blit_copy_bottom:
        mov dword ptr [ebp - 10h], eax
        mov eax, dword ptr [ebp - 0ch]
        cmp eax, dword ptr [ebp - 4]
        jl blit_no_overlap
        mov eax, dword ptr [ebp - 10h]
        cmp eax, dword ptr [ebp - 8]
        jl blit_no_overlap
        mov eax, dword ptr [ebp - 40h]
        mov edx, dword ptr [ebp - 2ch]
        sub edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jg blit_destination_copy_left
        mov eax, edx
blit_destination_copy_left:
        mov dword ptr [ebp - 14h], eax
        mov eax, dword ptr [ebp - 44h]
        mov edx, dword ptr [ebp - 30h]
        sub edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jg blit_destination_copy_top
        mov eax, edx
blit_destination_copy_top:
        mov dword ptr [ebp - 18h], eax
        mov eax, dword ptr [ebp - 48h]
        mov edx, dword ptr [ebp - 34h]
        sub edx, dword ptr [ebp - 24h]
        cmp eax, edx
        jl blit_destination_copy_right
        mov eax, edx
blit_destination_copy_right:
        mov dword ptr [ebp - 1ch], eax
        mov eax, dword ptr [ebp - 4ch]
        mov edx, dword ptr [ebp - 38h]
        sub edx, dword ptr [ebp - 28h]
        cmp eax, edx
        jl blit_destination_copy_bottom
        mov eax, edx
blit_destination_copy_bottom:
        mov dword ptr [ebp - 20h], eax
        mov eax, dword ptr [ebp - 0ch]
        inc eax
        sub eax, dword ptr [ebp - 4]
        mov dword ptr [ebp - 58h], eax
        mov eax, dword ptr [ebp - 10h]
        inc eax
        sub eax, dword ptr [ebp - 8]
        mov dword ptr [ebp - 54h], eax
        mov eax, dword ptr [ebp - 7ch]
        imul dword ptr [ebp - 70h]
        add eax, dword ptr [ebp - 74h]
        add eax, dword ptr [ebp - 78h]
        mov esi, eax
        mov eax, dword ptr [ebp - 9ch]
        imul dword ptr [ebp - 94h]
        add eax, dword ptr [ebp - 90h]
        add eax, dword ptr [ebp - 98h]
        mov edi, eax
        mov eax, dword ptr [ebp - 8]
        mov ebx, dword ptr [ebp - 18h]
        cmp eax, ebx
        jle blit_bottom_up
        mul dword ptr [ebp - 70h]
        add esi, eax
        mov eax, ebx
        mul dword ptr [ebp - 94h]
        add edi, eax
        mov eax, dword ptr [ebp - 70h]
        mov dword ptr [ebp - 3ch], eax
        mov eax, dword ptr [ebp - 94h]
        mov dword ptr [ebp - 50h], eax
        jmp blit_vertical_ready
blit_bottom_up:
        mov eax, dword ptr [ebp - 10h]
        mul dword ptr [ebp - 70h]
        add esi, eax
        mov eax, dword ptr [ebp - 20h]
        mul dword ptr [ebp - 94h]
        add edi, eax
        mov eax, dword ptr [ebp - 70h]
        neg eax
        mov dword ptr [ebp - 3ch], eax
        mov eax, dword ptr [ebp - 94h]
        neg eax
        mov dword ptr [ebp - 50h], eax
blit_vertical_ready:
        mov ecx, dword ptr [ebp - 58h]
        mov eax, dword ptr [ebp - 4]
        mov ebx, dword ptr [ebp - 14h]
        cmp eax, ebx
        jle blit_right_to_left
        add esi, eax
        add edi, ebx
        sub dword ptr [ebp - 3ch], ecx
        sub dword ptr [ebp - 50h], ecx
        cld
        mov dword ptr [ebp - 5ch], 0
        jmp blit_direction_ready
blit_right_to_left:
        add esi, dword ptr [ebp - 0ch]
        add edi, dword ptr [ebp - 1ch]
        add dword ptr [ebp - 3ch], ecx
        add dword ptr [ebp - 50h], ecx
        std
        mov dword ptr [ebp - 5ch], 3
blit_direction_ready:
        mov eax, dword ptr [ebp + 20h]
        test eax, 0ffffff00h
        jz blit_fill
        mov edx, dword ptr [ebp - 54h]
        mov eax, dword ptr [ebp - 3ch]
        mov ebx, dword ptr [ebp - 50h]
blit_copy_row:
        mov ecx, dword ptr [ebp - 58h]
        and ecx, 3
        rep movsb
        mov ecx, dword ptr [ebp - 58h]
        shr ecx, 2
        sub esi, dword ptr [ebp - 5ch]
        sub edi, dword ptr [ebp - 5ch]
        rep movsd
        add esi, dword ptr [ebp - 5ch]
        add edi, dword ptr [ebp - 5ch]
        add esi, eax
        add edi, ebx
        dec edx
        jnz blit_copy_row
        cld
        jmp blit_success
blit_fill:
        mov dl, al
        mov ah, al
        shl eax, 10h
        mov al, dl
        mov ah, al
        mov edx, dword ptr [ebp - 54h]
        mov ebx, dword ptr [ebp - 50h]
blit_fill_row:
        mov ecx, dword ptr [ebp - 58h]
        and ecx, 3
        rep stosb
        mov ecx, dword ptr [ebp - 58h]
        shr ecx, 2
        sub edi, dword ptr [ebp - 5ch]
        rep stosd
        add edi, dword ptr [ebp - 5ch]
        add edi, ebx
        dec edx
        jnz blit_fill_row
        cld
blit_success:
        xor eax, eax
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
blit_no_overlap:
        mov eax, -3
        pop es
        pop edi
        pop esi
        pop ebx
        _emit 0xc9
        ret
    }
}
