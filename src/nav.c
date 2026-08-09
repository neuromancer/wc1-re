/*
 *  Nav map, location readouts and the virtual screen.
 *
 *  Address range 0x40d000-0x40ffff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: DrawNav* family; string band 0x4687AC-0x4688F4.
 */
#include "wc1.h"

/* Function start: 0x40D1D0 */
void ClearNavHazardFlag(void)
{
    DAT_00468710 = 0;
}

/* Function start: 0x40D240 */
void ClearNavLegendFlag(void)
{
    DAT_0046870c = 0;
}

/* Function start: 0x40D8C0 */
void SetScreenClipRect(unsigned short a, unsigned short b,
                       unsigned short c, unsigned short d)
{
    SetRectBounds((int)&DAT_005a76b0, a, b, c, d);
}

/* Function start: 0x40DE70 */
void FormatNavCoordinates(unsigned char *out)
{
    unsigned char tmp[4];

    SplitGameClockTicks(tmp);
    out[1] = tmp[2];
    out[0] = tmp[3];
}

/* Function start: 0x40DF50 */
char *GetNavNameSkippingMarker(short i)
{
    char *p = g_aMissionObjectives_0059dac5[i].name;

    if (*p == '?')
        p = p + 1;
    return p;
}

/* Function start: 0x40EFE0 */
void NudgeObjectX(short i, short dx, short dy)
{
    short *p = (short *)(DAT_00598a30[i] + 0x20);

    (void)dy;
    *p = *p + dx;
}

/* Function start: 0x40F940 */
void __stdcall free_viewport(Viewport *viewport)
{
    unsigned char *allocation;
    int i;
    int oldCount;

    g_nFreeViewportCalls_005a8110++;
    allocation = viewport->allocation;
    if (allocation != 0) {
        i = 0;
        while (i < g_nViewportAllocationCount_005a7f0c) {
            if (g_apViewportAllocations_005a7f10[i] == allocation) {
                oldCount = g_nViewportAllocationCount_005a7f0c;
                g_nViewportAllocationCount_005a7f0c--;
                g_apViewportAllocations_005a7f10[i] =
                    g_apViewportAllocations_005a7f10[oldCount];
                break;
            }
            i++;
        }
        if (viewport->rowOffsets != 0) {
            ReleasePacketHandle((int)viewport->rowOffsets);
            viewport->rowOffsets = 0;
        }
        if (DAT_0046b168 != 0x13)
            printf("free_viewport not mcga\n");
        ReleasePacketHandle((int)allocation);
        viewport->pixels = 0;
        viewport->allocation = 0;
        if (DAT_005a6ba0.pixels == allocation)
            DAT_005a6ba0.pixels = 0;
        if (DAT_005a6ba0.allocation == allocation)
            DAT_005a6ba0.allocation = 0;
        if (DAT_005a76b0.pixels == allocation)
            DAT_005a76b0.pixels = 0;
        if (DAT_005a76b0.allocation == allocation)
            DAT_005a76b0.allocation = 0;
    }
}

/* Function start: 0x40FA30 */
unsigned short GetPaletteReadyUnused(void)
{
    return 1;
}

/* Function start: 0x40FA40 */
void DrawTitleLogo(short distance, short y)
{
    short bounds[4];
    short scale;
    short centre;

    if (distance <= 10 || g_pTitleShape_005a7f08 == 0)
        return;
    scale = (short)(0x1000 / distance);
    centre = (short)((DAT_005a7510.left + DAT_005a7510.right + 1) / 2);
    GetTransformedShapeBounds(&DAT_005a7510, centre, y,
                              g_pTitleShape_005a7f08, 1, 0, scale, 0,
                              bounds);
    DrawSpriteTransformed(&DAT_005a7510, (short)(bounds[0] - 1), y,
                          g_pTitleShape_005a7f08, 0, 0,
                          scale, scale, 0, 0);
    DrawSpriteTransformed(&DAT_005a7510, centre, y,
                          g_pTitleShape_005a7f08, 1, 0,
                          scale, scale, 0, 0);
    DrawSpriteTransformed(&DAT_005a7510, bounds[2], y,
                          g_pTitleShape_005a7f08, 2, 0,
                          scale, scale, 0, 0);
}

/* Function start: 0x40FB70 */
int Title_Sequence(void)
{
    short frame;
    short credit;
    short titleDistance;
    short missionShip;
    int interrupted;

    interrupted = 0;
    DAT_0059ab58 = 0;
    DAT_005a8964 = 0;
    MouseHide();
    init_3Space_objects(0);
    initialize_view_buffer();
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    g_nCannedSceneMode_00469fac = 2;

    while (interrupted == 0) {
        missionShip = 32;
        do {
            g_aMissionShips_0046c948[missionShip].state = 0;
            missionShip++;
        } while (missionShip < 46);
        titleDistance = 200;
        remove_all_hazards();
        g_bIntroSecondaryScene_0046c024 = 0;
        EnterNavPoint(16);
        g_pTitleShape_005a7f08 =
            (unsigned char *)FetchDiskPacketRetrying(9, 0, 0);
        StartMusicTrack(0x17, 2);
        initialize_scripted_view(g_asIntroCameraSequence_0046c090);
        DAT_00469fb4 = 1;

        frame = 0;
        do {
            Update_3Space();
            if (Draw_3Space_Frame() != 0) {
                print_subtitle(&DAT_005a7510, 0x32,
                    "In the distant future,\nmankind is locked in a deadly war...");
                dump_buffer_to_screen();
                DIBslamReal();
                clear_view_buffer();
            }
            if (PumpWindowMessages() == 0 || DAT_0059ab58 != 0 ||
                DAT_005a8964 != 0) {
                interrupted = 1;
                break;
            }
            frame++;
        } while (frame < 25);
        if (interrupted != 0)
            break;

        frame = 0;
        do {
            Update_3Space();
            if (Draw_3Space_Frame() != 0) {
                dump_buffer_to_screen();
                DIBslamReal();
                clear_view_buffer();
            }
            if (PumpWindowMessages() == 0 || DAT_0059ab58 != 0 ||
                DAT_005a8964 != 0) {
                interrupted = 1;
                break;
            }
            frame++;
        } while (frame < 110);
        if (interrupted != 0)
            break;

        frame = 0;
        do {
            Update_3Space();
            if (Draw_3Space_Frame() != 0) {
                DrawTitleLogo(titleDistance,
                              (short)((DAT_005a7510.top +
                                       DAT_005a7510.bottom + 1) / 2 - 6));
                dump_buffer_to_screen();
                DIBslamReal();
                clear_view_buffer();
            }
            if (titleDistance > 16)
                titleDistance = titleDistance - 4;
            if (PumpWindowMessages() == 0 || DAT_0059ab58 != 0 ||
                DAT_005a8964 != 0) {
                interrupted = 1;
                break;
            }
            frame++;
        } while (frame < 100);
        FreePacketAndClear((int *)&g_pTitleShape_005a7f08);
        if (interrupted != 0)
            break;

        ScaleFixedVector(&g_aShipForwardVector_0059bce0[61], 0x9600,
                         &g_aShipVelocity_0059c010[61]);
        EnterNavPoint(17);
        g_bIntroSecondaryScene_0046c024 = 1;
        g_anObjectPitchRotation_0059b2a0[0] = 0;
        g_anObjectYawRotation_0059ce80[0] = 0;
        g_anObjectRollRotation_0059d7e0[0] = 0;
        start_hazard_field(0);

        credit = 0;
        while (credit < g_nIntroCreditCount_00468a30 && interrupted == 0) {
            frame = 0;
            do {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    print_subtitle(&DAT_005a7510, 0x32,
                                   g_apszIntroCredits_00468a38[credit]);
                    dump_buffer_to_screen();
                    DIBslamReal();
                    clear_view_buffer();
                }
                if (PumpWindowMessages() == 0 || DAT_0059ab58 != 0 ||
                    DAT_005a8964 != 0) {
                    interrupted = 1;
                    break;
                }
                frame++;
            } while (frame < 70);
            frame = 0;
            while (frame < 40 && interrupted == 0) {
                Update_3Space();
                if (Draw_3Space_Frame() != 0) {
                    dump_buffer_to_screen();
                    DIBslamReal();
                    clear_view_buffer();
                }
                if (PumpWindowMessages() == 0 || DAT_0059ab58 != 0 ||
                    DAT_005a8964 != 0)
                    interrupted = 1;
                frame++;
            }
            credit++;
        }
        frame = 0;
        while (frame < 150 && interrupted == 0) {
            Update_3Space();
            if (Draw_3Space_Frame() != 0) {
                dump_buffer_to_screen();
                DIBslamReal();
                clear_view_buffer();
            }
            if (PumpWindowMessages() == 0 || DAT_0059ab58 != 0 ||
                DAT_005a8964 != 0)
                interrupted = 1;
            frame++;
        }
    }

    StopMusicUnlessSuppressed();
    ResetSoundStateForScene();
    FreePacketAndClear((int *)&g_pTitleShape_005a7f08);
    FreePacketAndClear((int *)&g_pIntroFont_005a8960);
    remove_all_hazards();
    free_nav_object_resources();
    free_3Space();
    g_bIntroSecondaryScene_0046c024 = 0;
    g_nCannedSceneMode_00469fac = 0;
    g_bScriptedView_0046a8d4 = 0;
    clear_view_buffer();
    dump_buffer_to_screen();
    DIBslamReal();
    MouseShow();
    return 0;
}
