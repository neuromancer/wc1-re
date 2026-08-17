/*
 *  Win32 shell: window creation, message loop, exit paths.
 *
 *  Address range 0x401000-0x402dff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: CreateMainWindow/MainWindowProc/AbortToDesktop cluster; string band 0x465048-0x465354.
 */
#include "wc1.h"

/* Function start: 0x417550 */
void SaveGamePalette(void)
{
#if 0
    int index;
    unsigned short *entry;

    index = 0;
    entry = DAT_005a8a50;
    do {
        GetPaletteEntry((short)index, entry);
        entry += 3;
        index++;
    } while (entry < DAT_005a8a50 + 0x300);
#else
    unsigned short index;

    for (index = 0; (short)index < 256; index++) {
        if ((index & 15) == 0)
            WaitForVerticalBlankThunk();
        GetPaletteEntry(index, g_ausPaletteWords_005d3220[index]);
    }
#endif
}

/* Function start: WC2_UNMAPPED */
void RestoreGamePalette(void)
{
    WaitForVerticalBlankThunk();
    DIBwholePaletteFromWords(DAT_005a8a50);
}

/* Function start: WC2_UNMAPPED */
short easy2see(short obj)
{
    short bounds[4];
    short x;
    short y;
    unsigned char *shape;

    x = g_asObjectScreenX_00493598[obj];
    if (x == (short)0x8001)
        return 0;
    x = (short)(x + g_nViewCenterX_0059a852);
    y = g_asObjectScreenY_00493628[obj];
    shape = g_apObjectShape_0059d2f0[obj];
    y = (short)(y + g_nViewCenterY_0059a854);
    return GetTransformedShapeBounds(
        &g_stViewBuffer_005d2b00, x, y, shape,
        g_asObjectViewFrame_0059d230[obj],
        g_asObjectScreenAngle_0059cd90[obj],
        g_asObjectScreenScale_0059c950[obj],
        g_asObjectFlip_0059c870[obj], bounds);
}

/* Function start: 0x4176D2 */
void make_shard(short asteroid, FixedVector direction)
{
    short fragment;
    short speed;

    fragment = find_vacant_3d_object();
    if (fragment == -1)
        return;
    set_objects_data(fragment, OBJECT_TYPE_ROCK_CHUNK, asteroid);
    g_asObjectCounter_00494be0[fragment] = 40;
    g_acObjectOwner_00495208[fragment] = (signed char)asteroid;
    SetVectorFixedPoint((unsigned int *)&direction,
                        (short)(g_asObjectCollisionRadius_0059d710[asteroid] >> 1));
    AddFixedVectors(&g_aShipPosition_00494550[asteroid], &direction,
                    &g_aShipPosition_00494550[fragment]);
    g_aShipForwardVector_00494208[fragment] = direction;
    fix_objects_ijk(fragment);
    alter_yaw(signed_random(20), fragment);
    alter_pitch(signed_random(20), fragment);
    g_aShipVelocity_0059c010[fragment] =
        g_aShipForwardVector_00494208[fragment];
    speed = (short)(real_velocity(asteroid) + RandomInRange(0, 5));
    SetVectorFixedPoint(
        (unsigned int *)&g_aShipVelocity_0059c010[fragment], speed);
}

/* Base flight times and pitch windows for the four forward view bands. */
static const signed char g_acHazardTravelTimeByView_00465048[8] = {
    56, 52, 75, 73, 0, 0, 0, 0
};

static const signed char g_acHazardPitchRange_00465050[8] = {
    -10, 4, -8, 8, -12, 8, -8, 8
};

/* Function start: 0x417838 */
void remove_hazard(signed char hazard)
{
#if 0
    g_aiSoundEffectSourceActive_005a66ec[(short)hazard + 1] = 0;
    remove_object((short)hazard);
    g_nActiveHazards_00492e5c =
        MaxShort(0, (short)(g_nActiveHazards_00492e5c - 1));
#else
    remove_object((short)hazard);
    g_nActiveHazards_00492e5c =
        MaxShort(0, (short)(g_nActiveHazards_00492e5c - 1));
#endif
}

/* Function start: 0x41787A */
void remove_all_hazards(void)
{
#if 0
    short slot = 0;

    do {
        remove_hazard(g_abHazardObjects_00493280[slot]);
        g_abHazardObjects_00493280[slot] = -1;
        slot++;
    } while (slot < 20);
    g_pActiveHazardField_00493278 = 0;
#else
    short slot;

    for (slot = 0; slot < 20; slot++) {
        if (g_abHazardObjects_00493280[slot] != -1) {
            remove_hazard(g_abHazardObjects_00493280[slot]);
            g_abHazardObjects_00493280[slot] = -1;
        }
    }
    g_pActiveHazardField_00493278 = 0;
#endif
}

/* Function start: 0x4178E5 */
short difficulty(void)
{
    return (short)(abs(25 - (int)g_nHazardReferenceSpeed_00465040) * 2);
}

/* Function start: 0x417916 */
short asteroid_velocity(void)
{
    return MinShort(20, (short)(RandomBelowOrEqual(7) + 10));
}

/* Function start: 0x417941 */
void skew_randomly(short obj, short allowReverse)
{
    FixedVector saved;

    if (RandomBelow(100) < 50) {
        saved = g_aShipRightVector_0059b6e0[obj];
        g_aShipRightVector_0059b6e0[obj] =
            g_aShipForwardVector_00494208[obj];
        g_aShipForwardVector_00494208[obj] =
            g_aShipUpVector_0059b9e0[obj];
        g_aShipUpVector_0059b9e0[obj] = saved;
    } else {
        saved = g_aShipUpVector_0059b9e0[obj];
        g_aShipUpVector_0059b9e0[obj] =
            g_aShipForwardVector_00494208[obj];
        g_aShipForwardVector_00494208[obj] =
            g_aShipRightVector_0059b6e0[obj];
        g_aShipRightVector_0059b6e0[obj] = saved;
    }
    if (allowReverse != 0 && RandomBelow(100) < 50)
        negate_vector(&g_aShipForwardVector_00494208[obj]);
}

/* Function start: 0x417AD7 */
void align(int *value, short quantum)
{
#if 0
    short current;

    current = *(short *)value;
    current = (short)(current - current % quantum);
    *(short *)value = current;
#else
    *value -= *value % quantum;
#endif
}

/* Function start: 0x417AF9 */
void init_hazard(short obj, FixedVector position, short moving)
{
    enum ObjectType type;
    FixedVector vector;
    short hazardMoves;
    short travelTime;
    short speed;

    hazardMoves = moving;
    type = OBJECT_TYPE_SPACE_MINE;
    if (g_pActiveHazardField_00493278->type == OBJECT_TYPE_ASTEROID_FIELD)
        type = (enum ObjectType)(OBJECT_TYPE_ASTEROID1 +
                                RandomBelowOrEqual(5));
    set_objects_data(obj, type, -1);
    g_aShipPosition_00494550[obj] = position;

    if (type == OBJECT_TYPE_SPACE_MINE) {
        point_at(obj, g_aShipPosition_00494550[0]);
        speed = 2;
        skew_randomly(obj, 1);
        hazardMoves = 0;
    } else if (hazardMoves != 0) {
        travelTime = 65;
        if (g_cCockpitView_0059dab0 < 4)
            travelTime = g_acHazardTravelTimeByView_00465048[
                g_cCockpitView_0059dab0];
        travelTime = (short)(travelTime + RandomBelowOrEqual(15));
        if (g_bIntroSecondaryScene_0046c024 != 0) {
            travelTime = (short)(travelTime -
                                 RandomBelowOrEqual(difficulty()));
            travelTime = MaxShort(45, travelTime);
        } else {
            travelTime = (short)(travelTime -
                                 RandomBelowOrEqual(difficulty()));
            travelTime = MaxShort(7, travelTime);
        }
        ScaleFixedVector(&g_aShipVelocity_0059c010[0],
                         (int)travelTime << 8, &vector);
        AddFixedVectors(&g_aShipPosition_00494550[0], &vector, &vector);
        point_at(obj, vector);
        speed = distance_between_points(
            &vector, &g_aShipPosition_00494550[obj]);
        travelTime = MaxShort(3,
            (short)(travelTime - RandomBelow(5)));
        speed = (short)(speed / travelTime);
    } else {
        point_at(obj, g_pActiveHazardField_00493278->center);
        speed = 0;
        skew_randomly(obj, 1);
        if (RandomBelow(100) >= 20)
            speed = asteroid_velocity();
    }
    if (kilrathi_near(0, 16000) != 0)
        speed = 0;
    ScaleFixedVector(&g_aShipForwardVector_00494208[obj],
                     (int)speed << 8, &g_aShipVelocity_0059c010[obj]);

    if (hazardMoves == 0) {
        int separation;

        if (type == OBJECT_TYPE_ASTEROID_FIELD)
            separation = 1500;
        else
            separation = RandomBelowOrEqual(1000) << 8;
        ScaleFixedVector(&g_aShipForwardVector_00494208[obj],
                         separation, &vector);
        SubtractFixedVectors(&g_aShipPosition_00494550[obj], &vector,
                             &g_aShipPosition_00494550[obj]);
    }
    if (type == OBJECT_TYPE_SPACE_MINE) {
        align(&g_aShipPosition_00494550[obj].x, 200);
        align(&g_aShipPosition_00494550[obj].y, 200);
        align(&g_aShipPosition_00494550[obj].z, 200);
    }
    g_nActiveHazards_00492e5c++;
    g_asObjectCounter_00494be0[obj] = 0;
    g_acObjectCollisionGraceTicks_0059ddb0[obj] = 0;
}

/* Function start: 0x417EA4 */
int near_field(const HazardField *field, const FixedVector *point)
{
    return IsPointWithinRange((FixedVector *)&field->center,
                              (FixedVector *)point,
                              (short)(field->innerRadius + 4300));
}

/* Function start: 0x417ED4 */
short within_field(const HazardField *field, const FixedVector *point)
{
    return IsPointWithinRange((FixedVector *)&field->center,
                              (FixedVector *)point, field->innerRadius);
}

/* Function start: 0x417EFF */
short try_far_spot(FixedVector *spot, short *moving)
{
    short yaw;
    short pitch;
    unsigned short outsideRange;

    copy_frame(0, 63);
    g_aShipPosition_00494550[63] = g_aShipPosition_00494550[0];
    pitch = signed_random(20);
    yaw = signed_random(35);
    if (g_nCurrentView_00492fa8 == 0 && g_cCockpitView_0059dab0 <= 3) {
        signed char minimum;
        signed char maximum;

        minimum = g_acHazardPitchRange_00465050[
            g_cCockpitView_0059dab0 * 2];
        if (pitch > minimum &&
            pitch < (maximum = g_acHazardPitchRange_00465050[
                         g_cCockpitView_0059dab0 * 2 + 1]) &&
            abs(yaw) < 19 &&
            RandomBelow(100) < 60)
            *moving = 1;
        else
            *moving = 0;
    } else {
        if (abs(pitch) > 5 && abs(pitch) < 20 &&
            abs(yaw) > 5 && abs(yaw) < 20 && RandomBelow(100) < 30)
            *moving = 1;
        else
            *moving = 0;
    }
    pitch = (short)(pitch + find_ratio(
        -15, 15, g_anObjectPitchRotation_0059b2a0[0], -150, 150));
    yaw = (short)(yaw + find_ratio(
        -15, 15, g_anObjectYawRotation_0059ce80[0], -150, 150));
    rotate_about_j(yaw, &g_aShipRightVector_0059b6e0[63],
                   &g_aShipForwardVector_00494208[63]);
    rotate_about_i(pitch, &g_aShipUpVector_0059b9e0[63],
                   &g_aShipForwardVector_00494208[63]);
    position_relative_ijk(spot, 63, 0, 0, 3050);
    outsideRange = !(unsigned short)IsPointWithinRange(
        &g_aShipPosition_00494550[0], spot, 3000);
    return outsideRange != 0 &&
           within_field(g_pActiveHazardField_00493278, spot) != 0;
}

/* Function start: 0x418175 */
short rear_sphere(void)
{
    return find_ratio(0, 20, (short)g_nHazardReferenceSpeed_00465040,
                      4300, 3100);
}

/* Function start: 0x4181A2 */
int ok_hazard_spot(short obj)
{
    int range = 4300;

    if (g_asObjectScreenX_00493598[obj] == (short)0x8001)
        range = rear_sphere();
    return IsPointWithinRange(&g_aShipPosition_00494550[0],
                              &g_aShipPosition_00494550[obj],
                              (short)range);
}

/* Function start: 0x41820B */
short make_hazard(void)
{
    FixedVector spot;
    short moving;
    short obj = find_vacant_3d_object();

    if (obj != -1 && try_far_spot(&spot, &moving) != 0) {
        init_hazard(obj, spot, moving);
    } else
        obj = -1;
    return obj;
}

/* Function start: 0x418288 */
void extra_hazard(short obj)
{
    if (g_aeObjectClass_00495328[obj] == OBJECT_CLASS_DUST)
        g_aeObjectClass_00495328[obj] = OBJECT_CLASS_NULL;
}

/* Function start: 0x4182B6 */
void approach(short obj)
{
    FixedVector target;
    FixedVector thrust;

    ScaleFixedVector(&g_aShipVelocity_0059c010[0], 20 << 8, &target);
    AddFixedVectors(&g_aShipPosition_00494550[0], &target, &target);
    point_at(obj, target);
    ScaleFixedVector(&g_aShipForwardVector_00494208[obj], 20 << 8,
                     &thrust);
    AddFixedVectors(&g_aShipVelocity_0059c010[obj], &thrust,
                    &g_aShipVelocity_0059c010[obj]);
}

/* Function start: 0x41836E */
void manage_hazard(short obj, short slot)
{
    if (g_nRenderedSpaceFrame_00493138 % 20 != slot)
        return;
    if (ok_hazard_spot(obj) == 0) {
        remove_hazard((signed char)obj);
        return;
    }
    if (g_acObjectType_00493980[obj] == OBJECT_TYPE_SPACE_MINE &&
        g_asObjectScreenX_00493598[obj] != (short)0x8001 &&
        (unsigned short)g_asObjectDistance_0059b4a0[obj] > 1500 &&
        real_velocity(obj) < 20)
        approach(obj);
}

/* Function start: 0x418426 */
void match_ship_to_eye(void)
{
    g_aShipPosition_00494550[0] = g_aShipPosition_00494550[61];
    g_nHazardReferenceSpeed_00465040 = 100;
    g_aShipRightVector_0059b6e0[0] = g_aShipRightVector_0059b6e0[61];
    g_aShipUpVector_0059b9e0[0] = g_aShipUpVector_0059b9e0[61];
    g_aShipForwardVector_00494208[0] =
        g_aShipForwardVector_00494208[61];
    ScaleFixedVector(&g_aShipForwardVector_00494208[0], 100 << 8,
                     &g_aShipVelocity_0059c010[0]);
    g_pActiveHazardField_00493278->center = g_aShipPosition_00494550[61];
}

/* Function start: 0x4184F6 */
void update_hazards(void)
{
    short slot;
    short emptySlot = -1;

    if (g_bIntroSecondaryScene_0046c024 != 0)
        match_ship_to_eye();
    else
        g_nHazardReferenceSpeed_00465040 = real_velocity(0);
    slot = 0;
    do {
        if (g_abHazardObjects_00493280[slot] != -1)
            manage_hazard((short)g_abHazardObjects_00493280[slot], slot);
        else
            emptySlot = slot;
        slot++;
    } while (slot < 20);
    if (emptySlot != -1 &&
        RandomBelowOrEqual(215) <
            (short)g_nHazardReferenceSpeed_00465040 + 30)
        g_abHazardObjects_00493280[emptySlot] = (signed char)make_hazard();
}

/* Function start: 0x4185CC */
void start_hazard_field(short region)
{
#if 0
    short slot;

    remove_all_hazards();
    g_pActiveHazardField_00493278 = &g_aHazardFields_004931d8[region];
    slot = 1;
    do {
        g_abHazardObjects_00493280[slot] = (signed char)make_hazard();
    } while (slot++ < 3);
#else
    short slot;

    slot = 0;
    remove_all_hazards();
    g_pActiveHazardField_00493278 = &g_aHazardFields_004931d8[region];
    while (slot++ < 3)
        g_abHazardObjects_00493280[slot] = (signed char)make_hazard();
#endif
}

/* Function start: 0x418626 */
void add_hazard_field(enum ObjectType type, FixedVector center,
                      short radius, short density)
{
    HazardField *field;

    if (g_nHazardFieldCount_0059c90c >= 7)
        return;
    field = &g_aHazardFields_004931d8[g_nHazardFieldCount_0059c90c];
    field->type = type;
    field->center = center;
    field->outerRadius = radius;
    field->innerRadius = radius;
    field->density = density;
    g_nHazardFieldCount_0059c90c++;
}

/* Function start: 0x4186A4 */
void check_hazards(void)
{
    HazardField *field;
    short region;

    if (g_bIntroSecondaryScene_0046c024 != 0)
        return;
    if (g_pActiveHazardField_00493278 == 0) {
        region = 0;
        field = g_aHazardFields_004931d8;
        while (region < g_nHazardFieldCount_0059c90c) {
            if (field != g_pActiveHazardField_00493278 &&
                near_field(field, &g_aShipPosition_00494550[0]) != 0) {
                start_hazard_field(region);
                return;
            }
            region++;
            field++;
        }
    } else if (near_field(g_pActiveHazardField_00493278,
                          &g_aShipPosition_00494550[0]) == 0) {
        remove_all_hazards();
    }
}

/* Function start: 0x4199F0 */
void __stdcall WarpMouseTo(short x, short y)
{
#ifndef WC1_SDL
    __asm cli
#endif
    g_stHostMouseState_0059af70.x = x;
    g_stHostMouseState_0059af70.y = y;
    g_stMouseCursorState_0059ab10.x = x;
    g_stMouseCursorState_0059ab10.y = y;
    SetMouseHomePosition(x, y);
#ifndef WC1_SDL
    __asm sti
#endif
}

/* Function start: 0x453C95 */
void CheckLauncherAndConfig(void)
{
    FILE *config;
    char option[100];
#ifdef WC1_SDL
    char resolvedPath[PATH_MAX];
#endif

    if (ReadCheaterFlagFromRegistry() != 0) {
        *(unsigned char *)&g_nOriginDevUnlock_0049d774 = 1;
        *(unsigned char *)&DAT_00469ffc = 0;
        *(unsigned char *)&DAT_0046a000 = 0;
    }

#ifdef WC1_SDL
    if (Wc1SdlResolvePath("WINGCMDR.CFG", resolvedPath,
                          sizeof(resolvedPath)))
        config = fopen(resolvedPath, "rt");
    else
        config = 0;
#else
    config = fopen("WINGCMDR.CFG", "rt");
#endif
    if (config != 0) {
        while (fscanf(config, "%s", option) != EOF) {
            char command;

            if (memcmp(option, "$#SAGA.EXE", 11) == 0)
                DAT_0046506c = 1;
            command = option[0] == '-' ? option[1] : option[0];
            switch (command) {
            case 'b':
                *(unsigned char *)&DAT_0046a000 = 0;
                break;
            case 'c':
                DAT_0046507c = 0;
                break;
            case 'f':
                DAT_00465070 = 1;
                break;
            case 'k':
                *(unsigned char *)&DAT_00469ffc = 0;
                break;
            case 'q':
                DAT_00465074 = 0;
                break;
            }
            if (config == 0)
                return;
        }
        fclose(config);
    }
}

#ifndef WC1_SDL

/* Function start: 0x453E60 */
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR commandLine, int showCommand)
{
    MEMORYSTATUS memoryStatus;
    RECT clip;
    HANDLE process;

    (void)commandLine;
    DAT_005a89a4 = CreateSemaphoreA(0, 0, 1, "Wing Commander 1");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxA(0,
                    "Only one instance of Wing Commander 1 for Windows95 may be running at a time",
                    "ATTENTION", MB_ICONERROR);
        exit(0);
    }

    memset(&memoryStatus, 0, sizeof(memoryStatus));
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatus(&memoryStatus);
    if (memoryStatus.dwTotalPhys < 0x800000) {
        MessageBoxA(0,
                    "You must have at leat 8 megs of memory available to play Wing Commander 1 for Windows95",
                    "ATTENTION", MB_ICONERROR);
        exit(0);
    }
    if (memoryStatus.dwTotalPageFile < 0x800000) {
        MessageBoxA(0,
                    "You must have at leat 8 megs of virtual memory available to play Wing Commander 1 for Window95",
                    "ATTENTION", MB_ICONERROR);
        exit(0);
    }

    if (waveOutGetNumDevs() == 0)
        g_nAudioEnabled_0049c244 = 0;
    CheckLauncherAndConfig();
    if (!PromptInsertCorrectCd()) {
        CloseHandle(DAT_005a89a4);
        return 0;
    }
    if (!CreateMainWindow(instance, previous, showCommand))
        return 0;

    DAT_00465080 = 0;
    process = GetCurrentProcess();
    MonoDebug_install();
    SetPriorityClass(process, HIGH_PRIORITY_CLASS);
    if (g_nAudioEnabled_0049c244 != 0) {
        InitializeAudioSystem(DAT_005a89a0);
        InitializeAudioStreamer(DAT_005a89a0);
    }
    srand((unsigned int)time(0));
    InitGameClockEpoch();
    CreateDebugOverlayConsole(instance, DAT_005a89a0, 60, 20);
    DAT_005a8a44 = (unsigned int)time(0);
    ShowCursor(FALSE);
    DAT_0059ab2c = 0;
    clip.left = 0;
    clip.top = 0;
    clip.right = 320;
    clip.bottom = 200;
    ClipCursor(&clip);

    _onexit((_onexit_t)AbortToDesktop);
    main(0, (char **)"Vj");

    ClipCursor(0);
    ShowCursor(TRUE);
    DAT_005a8a38 = (unsigned int)time(0);
    DestroyGlobalDebugOverlayConsole();
    ServiceAudioStream();
    DestroyWindow(DAT_005a89a0);
    DIBunInstall();
    Streamer_close();
    CloseHandle(DAT_005a89a4);
    return 1;
}

#endif

/* Function start: 0x454103 */
void ShutdownGameWindow(void)
{
    DAT_005a8a38 = (unsigned int)time(0);
#ifdef WC1_SDL
    {
        SDL_Window *window;

        DAT_005a8a3c = 0;
        if ((g_dwStreamerState_005c4c38 & 1) != 0)
            ix_streamer_destroy();
        ServiceAudioStream();
        DestroyGlobalDebugOverlayConsole();
        window = (SDL_Window *)DAT_005a89a0;
        DIBunInstall();
        Wc1SdlShutdownJoysticks();
        if (window != 0)
            SDL_DestroyWindow(window);
        DAT_005a89a0 = 0;
        SDL_Quit();
    }
#else
    ClipCursor(0);
    ShowCursor(TRUE);
    DestroyGlobalDebugOverlayConsole();
    DestroyWindow(DAT_005a89a0);
    DIBunInstall();
    ClipCursor(0);
    ShowCursor(TRUE);
    {
        HANDLE process;

        process = GetCurrentProcess();
        SetPriorityClass(process, IDLE_PRIORITY_CLASS);
    }
    CloseHandle(DAT_005a89a4);
#endif
    exit(0);
}

/* Function start: 0x45418B */
void ShowNoticeMessageBox(const char *text)
{
    DIBpositionWindow();
    MessageBoxA(0, text, "NOTICE", 0x10);
    exit(0);
}

/* Function start: 0x45422D */
unsigned int AbortToDesktop(void)
{
#ifndef WC1_SDL
    HANDLE process;

    ClipCursor(0);
    ShowCursor(TRUE);
    process = GetCurrentProcess();
    SetPriorityClass(process, IDLE_PRIORITY_CLASS);
#endif
    sprintf(g_szMemoryUsage_005a89b0,
            "Current: %i\nMax    : %i\nTotal : %i\n",
            g_nGuardedAllocationBytes_00465064,
            g_nGuardedAllocationPeakBytes_00465068,
            g_nGuardedAllocationTotalBytes_00465060);
    OutputDebugStringA("Memory Info:\n");
    OutputDebugStringA(g_szMemoryUsage_005a89b0);
#ifndef WC1_SDL
    CloseHandle(DAT_005a89a4);
#endif
    return 0;
}

#ifndef WC1_SDL

/* Function start: 0x4542B7 */
int CreateMainWindow(HINSTANCE instance, HINSTANCE previous,
                     int showCommand)
{
    WNDCLASSA windowClass;

    DAT_005a8a40 = instance;
    if (previous != 0)
        return 0;

    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = MainWindowProc;
    windowClass.hInstance = instance;
    windowClass.hIcon = LoadIconA(0, IDI_APPLICATION);
    windowClass.hCursor = 0;
    windowClass.hbrBackground = GetStockObject(BLACK_BRUSH);
    windowClass.lpszMenuName = "Wing Commander";
    windowClass.lpszClassName = "Wing Commander";
    if (RegisterClassA(&windowClass) == 0)
        return 0;

    DAT_005a89a0 = CreateWindowExA(0, "Wing Commander", "Wing Commander",
                                   WS_POPUP, 0, 0, 320, 200, 0, 0,
                                   DAT_005a8a40, 0);
    if (DAT_005a89a0 == 0) {
        GetLastError();
        return 0;
    }

    DAT_005a8a30 = GetDC(DAT_005a89a0);
    SetTextColor(DAT_005a8a30, RGB(255, 0, 0));
    SetBkColor(DAT_005a8a30, RGB(0, 0, 0));
    if (GetDeviceCaps(DAT_005a8a30, BITSPIXEL) < 8) {
        MessageBoxA(0,
                    "You must be running with 256, or more, colors to play Wing Commander",
                    "NOTICE", MB_ICONEXCLAMATION);
        DestroyWindow(DAT_005a89a0);
        return 0;
    }

    DAT_005a8a34 = SetCursor(0);
    ShowWindow(DAT_005a89a0, showCommand);
    UpdateWindow(DAT_005a89a0);
    PumpWindowMessages();
    PumpWindowMessages();
    PumpWindowMessages();
    DIBinstall(DAT_005a89a0);
    DAT_005a8a3c = 1;
    return 1;
}

#endif

/* Function start: 0x45445A */
unsigned int PumpWindowMessages(void)
{
#ifndef WC1_SDL
    RECT clip;
    MSG message;
    int cursorX;
    int cursorY;
    int done;
#endif

    if (DAT_004650a8 != 0)
        return 1;
    DAT_004650a8 = 1;
    if (DAT_0059ab2c != 0)
        DAT_0059ab2c();
#ifdef WC1_SDL
    Wc1SdlPumpEvents();
#else
    done = 0;
    do {
        if (DAT_00465080 != 0) {
            if (GetMessageA(&message, 0, 0, 0) != 0) {
                done = 1;
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } else {
                LogMemoryUsage();
                ShutdownGameWindow();
            }
            if (IsIconic(DAT_005a89a0) == 0)
                DAT_00465080 = 0;
            if (DAT_00465080 == 0) {
                clip.left = 0;
                clip.top = 0;
                clip.right = 320;
                clip.bottom = 200;
                ShowCursor(FALSE);
                SetPriorityClass(GetCurrentProcess(),
                                 HIGH_PRIORITY_CLASS);
                SetActiveWindow(DAT_005a89a0);
                SetForegroundWindow(DAT_005a89a0);
                DIBreInstall();
                DIBslam();
                DIBslamReal();
                ClipCursor(&clip);
                SetCursorPos(cursorX, cursorY);
                init_player_input();
            }
        } else {
            if (PeekMessageA(&message, 0, 0, 0, PM_NOREMOVE) != 0) {
                if (GetMessageA(&message, 0, 0, 0) != 0) {
                    done = 0;
                    TranslateMessage(&message);
                    DispatchMessageA(&message);
                } else {
                    done = 0;
                    LogMemoryUsage();
                    ShutdownGameWindow();
                }
            } else {
                done = 1;
            }
        }
        if (IsIconic(DAT_005a89a0) != 0) {
            if (DAT_00465080 == 0) {
                cursorX = 160;
                cursorY = 100;
                ClipCursor(0);
                ShowCursor(TRUE);
                SetPriorityClass(GetCurrentProcess(),
                                 NORMAL_PRIORITY_CLASS);
            }
            DAT_00465080 = 1;
            if (DAT_00465080 != 0)
                done = 0;
        }
    } while (done == 0);
#endif
    DAT_0059ab54 = GetTickCount() * 60 / 1000;
    DAT_004650a8 = 0;
    return DAT_005a8a3c;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetF1KeyLatch(void)
{
    return DAT_004650ac;
}

#ifndef WC1_SDL

/* Function start: 0x454625 */
LRESULT CALLBACK MainWindowProc(HWND window, UINT message,
                                WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;
    int mouseEvent;
    unsigned int scanCode;
    unsigned int primaryButton;
    unsigned int secondaryButton;
    unsigned int mouseX;
    unsigned int mouseY;
    unsigned short eventType;

    mouseEvent = 0;
    primaryButton = (unsigned int)wParam & 1;
    secondaryButton = ((unsigned int)wParam & 2) >> 1;
    mouseX = (unsigned short)lParam;
    mouseY = (unsigned short)((unsigned long)lParam >> 16);

    switch (message) {
    case WM_SETFOCUS:
        SignalAudioMixerWakeEvent();
        return 0;
    case WM_CLOSE:
    case WM_DESTROY:
        DAT_005a8a3c = 0;
        ClipCursor(0);
        ShowCursor(TRUE);
        SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
        PostQuitMessage(0);
        break;
    case WM_QUIT:
        DAT_005a8a3c = 0;
        ClipCursor(0);
        ShowCursor(TRUE);
        SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
        break;
    case WM_PAINT:
        BeginPaint(DAT_005a89a0, &paint);
        EndPaint(DAT_005a89a0, &paint);
        break;
    case WM_KEYDOWN:
        if (DAT_0046505c != 0)
            QueueInputEvent(3, 0, 0, (unsigned short)wParam,
                            0, 0, 0, 0, 0);
        if (wParam == VK_F1) {
            DAT_004650ac = 1;
            if ((lParam & 0x40000000) != 0)
                DAT_004650ac = 0;
        }
        scanCode = ((unsigned long)lParam & 0xff0000) >> 16;
        if (scanCode == 1)
            g_bSceneEscapeRequested_0049d4b0 = 1;
        QueueInputEvent(3, 0, 0, (unsigned short)scanCode,
                        0, 0, 0, 0, 0);
        SetInputKeyState((int)scanCode, 1);
        break;
    case WM_KEYUP:
        if (DAT_0046505c != 0)
            QueueInputEvent(4, 0, 0, (unsigned short)wParam,
                            0, 0, 0, 0, 0);
        if (wParam == VK_F1)
            DAT_004650ac = 0;
        scanCode = ((unsigned long)lParam & 0xff0000) >> 16;
        QueueInputEvent(4, 0, 0, (unsigned short)scanCode,
                        0, 0, 0, 0, 0);
        SetInputKeyState((int)scanCode, 0);
        break;
    case WM_SYSKEYDOWN:
        DAT_005a8964 = (unsigned int)wParam;
        if (wParam == 'X' &&
            ((unsigned long)lParam & 0x20000000) != 0) {
            PostQuitMessage(0);
            sprintf(g_szMemoryUsage_005a89b0,
                    "Current: %i\nMax    : %i\nTotal : %i\n",
                    g_nGuardedAllocationBytes_00465064,
                    g_nGuardedAllocationPeakBytes_00465068,
                    g_nGuardedAllocationTotalBytes_00465060);
            OutputDebugStringA("Memory Info:\n");
            OutputDebugStringA(g_szMemoryUsage_005a89b0);
        } else if (wParam == 'N') {
            ReportSpaceFlightMaxFps(-0.5f);
        } else if (wParam == 'M') {
            ReportSpaceFlightMaxFps(0.5f);
        }
        break;
    case WM_SYSKEYUP:
        DAT_005a8964 = 0;
        break;
    case WM_COMMAND:
        if (((unsigned int)wParam & 0xffff) == 3) {
            DAT_005a8a3c = 0;
            PostQuitMessage(0);
        }
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_SCREENSAVE ||
            (wParam & 0xfff0) == SC_MONITORPOWER)
            return 0;
        break;
    case WM_MOUSEMOVE:
        if (g_bPointerMovedByKeyboard_005a7d54 != 0) {
            g_bPointerMovedByKeyboard_005a7d54 = 0;
            break;
        }
        eventType = 13;
        QueueInputEvent(eventType, (unsigned short)mouseX,
                        (unsigned short)mouseY, 0,
                        primaryButton, secondaryButton, 0, 0, 0);
        mouseEvent = 1;
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        eventType = 2;
        QueueInputEvent(eventType, (unsigned short)mouseX,
                        (unsigned short)mouseY, 0,
                        primaryButton, secondaryButton, 0, 0, 0);
        mouseEvent = 1;
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        eventType = 1;
        QueueInputEvent(eventType, (unsigned short)mouseX,
                        (unsigned short)mouseY, 0,
                        primaryButton, secondaryButton, 0, 0, 0);
        mouseEvent = 1;
        break;
    }
    if (mouseEvent != 0) {
        g_nHostMouseMessageX_005a8990 = mouseX;
        g_nHostMouseMessageY_005a8994 = mouseY;
        g_bHostPrimaryMouseButton_005a8998 = primaryButton;
        g_bHostSecondaryMouseButton_005a899c = secondaryButton;
    }
    return DefWindowProcA(window, message, wParam, lParam);
}

#endif

/* Function start: 0x4551E7 */
int __stdcall GetJoystickPosition(unsigned int *x, unsigned int *y,
                                  unsigned int *buttons, short joystick,
                                  unsigned int fallback)
{
    unsigned int device;
    unsigned int infoIndex;
    unsigned int buttonState;

    if (joystick != 0) {
        device = 1;
        infoIndex = 1;
    } else {
        device = 0;
        infoIndex = 0;
    }
#ifdef WC1_SDL
    if (Wc1SdlReadJoystick(
            device, &g_aJoystickInfo_005d10b0[infoIndex]) != FALSE) {
#else
    if (joyGetPos(device, &g_aJoystickInfo_005d10b0[infoIndex]) ==
        JOYERR_NOERROR) {
#endif
        *x = g_aJoystickInfo_005d10b0[infoIndex].wXpos;
        *y = g_aJoystickInfo_005d10b0[infoIndex].wYpos;
        buttonState = g_aJoystickInfo_005d10b0[infoIndex].wButtons;
        *buttons = buttonState;
        if (joystick != 0)
            *buttons = buttonState >> 2;
        else
            *buttons = buttonState & 3;
        return 0;
    }

#ifdef WC1_SDL
    if ((fallback & 0xffff) == 0xffff)
        fallback = (unsigned int)-1;
    else
        fallback &= 0xffff;
#else
    fallback &= 0xffff;
#endif
    *x = fallback;
    *y = fallback;
    *buttons = fallback;
    return 1;
}

/* Function start: 0x455346 */
short GetJoystickButtons(void)
{
#if 0
    return ((short)g_aJoystickInfo_005d10b0[1].wButtons << 2) |
           (unsigned short)g_aJoystickInfo_005d10b0[0].wButtons;
#else
    short buttons;

    buttons = (g_aJoystickInfo_005d10b0[1].wButtons << 2) |
              g_aJoystickInfo_005d10b0[0].wButtons;
    return buttons;
#endif
}

/* Function start: 0x45536F */
void GetJoystickDevCaps(short joystick, short *xMin, short *xMax,
                        short *yMin, short *yMax)
{
#ifdef WC1_SDL
    unsigned int hostXMin;
    unsigned int hostXMax;
    unsigned int hostYMin;
    unsigned int hostYMax;
#else
    JOYCAPSA caps;
#endif
    unsigned int device = joystick != 0;

    *xMin = *xMax = *yMin = *yMax = 0;
#ifdef WC1_SDL
    if (Wc1SdlReadJoystickAxisRange(
            device, &hostXMin, &hostXMax, &hostYMin, &hostYMax) == FALSE) {
#else
    if (joyGetDevCapsA(device, &caps, sizeof(caps)) != JOYERR_NOERROR) {
#endif
        SystemDebugPrintf(g_szJoystickDevCapsFailure_004652dc);
        return;
    }

#ifdef WC1_SDL
    *xMin = (short)hostXMin;
    *xMax = (short)hostXMax;
    *yMin = (short)hostYMin;
    *yMax = (short)hostYMax;
#else
    *xMin = (short)caps.wXmin;
    *xMax = (short)caps.wXmax;
    *yMin = (short)caps.wYmin;
    *yMax = (short)caps.wYmax;
#endif
}

/* Function start: 0x455427 */
HINSTANCE GetApplicationInstance(void)
{
    return DAT_005a8a40;
}

/* Function start: 0x45543C */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
HWND GetMainWindowHandle(void)
{
    return DAT_005a89a0;
}

/* Function start: 0x455451 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
HDC GetMainWindowDeviceContext(void)
{
    return DAT_005a8a30;
}

/* Function start: 0x455466 */
void *AllocateGuardedMemory(unsigned int size)
{
    GuardedAllocation *allocation;

    if (g_pGuardedAllocationHead_004650b0 == 0) {
        allocation =
            malloc(sizeof(GuardedAllocation));
        g_pGuardedAllocationHead_004650b0 = allocation;
    } else {
        g_pGuardedAllocationTail_005a89ac->next =
            malloc(sizeof(GuardedAllocation));
        allocation = g_pGuardedAllocationTail_005a89ac->next;
    }
    g_pGuardedAllocationTail_005a89ac = allocation;
    g_pGuardedAllocationTail_005a89ac->next = 0;
    g_pGuardedAllocationTail_005a89ac->size = size;
    g_pGuardedAllocationTail_005a89ac->block = malloc(size + 0x800);

    memset(g_pGuardedAllocationTail_005a89ac->block, 0xab, 0x400);
    memset((unsigned char *)g_pGuardedAllocationTail_005a89ac->block +
               0x400,
           0, size);
    memset((unsigned char *)g_pGuardedAllocationTail_005a89ac->block +
               0x400 + size,
           0xab, 0x400);
    g_nGuardedAllocationTotalBytes_00465060 += size;
    g_nGuardedAllocationBytes_00465064 += size;
    if (g_nGuardedAllocationPeakBytes_00465068 <
        g_nGuardedAllocationBytes_00465064) {
        g_nGuardedAllocationPeakBytes_00465068 =
            g_nGuardedAllocationBytes_00465064;
    }
    return (unsigned char *)g_pGuardedAllocationTail_005a89ac->block +
           0x400;
}

/* Function start: 0x455565 */
void ReportHeapGuardCorruption(void *memory, int count, int overrun)
{
    char text[0x80];

    DIBpositionWindow();
    sprintf(text, "Memory at %p %swritten. Corruption : (%i/%i) %s",
            memory, overrun ? "over" : "under", count, 0x100,
            count > 0x80
                ? (count > 0xc0 ? "EXTREME" : "SEVERE")
                : (count > 0x40 ? "BAD" : "NAUGHTY"));
    MessageBoxA(0, text, "NOTICE", 0x10);
    exit(0);
}

/* Function start: 0x455624 */
void CheckAllGuardedAllocations(void)
{
#if 0
    GuardedAllocation *allocation = g_pGuardedAllocationHead_004650b0;
#ifdef WC1_SDL
    unsigned char *guard;
    unsigned int guardValue;
#else
    unsigned int *guard;
#endif
    int prefixCorrupt;
    int i;
    int suffixCorrupt;

    while (allocation != 0) {
#ifdef WC1_SDL
        guard = (unsigned char *)allocation->block;
#else
        guard = (unsigned int *)allocation->block;
#endif
        prefixCorrupt = 0;
        i = 0x100;
        do {
#ifdef WC1_SDL
            memcpy(&guardValue, guard, sizeof(guardValue));
            if (guardValue != 0xabababab)
                prefixCorrupt = prefixCorrupt + 1;
            guard += sizeof(guardValue);
#else
            if (*guard != 0xabababab)
                prefixCorrupt = prefixCorrupt + 1;
            guard = guard + 1;
#endif
            i = i - 1;
        } while (i != 0);
        if (prefixCorrupt != 0)
            ReportHeapGuardCorruption(allocation->block, prefixCorrupt, 0);

#ifdef WC1_SDL
        guard += allocation->size;
#else
        guard = (unsigned int *)((unsigned char *)guard + allocation->size);
#endif
        suffixCorrupt = 0;
        i = 0x100;
        do {
#ifdef WC1_SDL
            memcpy(&guardValue, guard, sizeof(guardValue));
            if (guardValue != 0xabababab)
                suffixCorrupt = suffixCorrupt + 1;
            guard += sizeof(guardValue);
#else
            if (*guard != 0xabababab)
                suffixCorrupt = suffixCorrupt + 1;
            guard = guard + 1;
#endif
            i = i - 1;
        } while (i != 0);
        if (suffixCorrupt != 0)
            ReportHeapGuardCorruption(allocation->block, suffixCorrupt, 1);

        allocation = allocation->next;
    }
#endif
}

/* Function start: 0x45562F */
int IsFreedHeapBlockTracked(void *memory)
{
    FreedHeapBlock *block;

    block = g_pFreedHeapBlockHead_0049c304;
    while (block != 0) {
        if (block->block == memory)
            return 1;
        if (block->block == (unsigned char *)memory - 8)
            return 1;
        block = block->next;
    }
    return 0;
}

/* Function start: 0x455715 */
void TrackFreedHeapBlock(void *memory)
{
    if (g_pFreedHeapBlockHead_0049c304 == 0) {
        g_pFreedHeapBlockHead_0049c304 = malloc(sizeof(FreedHeapBlock));
        g_pFreedHeapBlockTail_0049c308 = g_pFreedHeapBlockHead_0049c304;
    } else {
        g_pFreedHeapBlockTail_0049c308->next = malloc(sizeof(FreedHeapBlock));
        g_pFreedHeapBlockTail_0049c308 = g_pFreedHeapBlockTail_0049c308->next;
    }
    g_pFreedHeapBlockTail_0049c308->next = 0;
    g_pFreedHeapBlockTail_0049c308->block = memory;
}

/* Function start: 0x4138A8 */
void FreeGuardedAllocation(void *memory)
{
    GuardedAllocation *allocation = g_pGuardedAllocationHead_004650b0;
    GuardedAllocation *previous = 0;
#ifdef WC1_SDL
    unsigned char *guard;
    unsigned int guardValue;
#else
    unsigned int *guard;
#endif
    void *block = (unsigned char *)memory - 0x400;
    int corrupt;
    int i;

#ifdef WC1_SDL
    guard = (unsigned char *)block;
#else
    guard = (unsigned int *)block;
#endif
    if (allocation == 0)
        return;
    while (allocation->block != block) {
        previous = allocation;
        allocation = allocation->next;
        if (allocation == 0)
            return;
    }

    corrupt = 0;
    i = 0x100;
    do {
#ifdef WC1_SDL
        memcpy(&guardValue, guard, sizeof(guardValue));
        if (guardValue != 0xabababab)
            corrupt = corrupt + 1;
        guard += sizeof(guardValue);
#else
        if (*guard != 0xabababab)
            corrupt = corrupt + 1;
        guard = guard + 1;
#endif
        i = i - 1;
    } while (i != 0);
    if (corrupt != 0)
        ReportHeapGuardCorruption(memory, corrupt, 0);

    corrupt = 0;
#ifdef WC1_SDL
    guard = (unsigned char *)memory + allocation->size;
#else
    guard = (unsigned int *)((unsigned char *)memory + allocation->size);
#endif
    i = 0x100;
    do {
#ifdef WC1_SDL
        memcpy(&guardValue, guard, sizeof(guardValue));
        if (guardValue != 0xabababab)
            corrupt = corrupt + 1;
        guard += sizeof(guardValue);
#else
        if (*guard != 0xabababab)
            corrupt = corrupt + 1;
        guard = guard + 1;
#endif
        i = i - 1;
    } while (i != 0);
    if (corrupt != 0)
        ReportHeapGuardCorruption(memory, corrupt, 1);

    g_nGuardedAllocationBytes_00465064 =
        g_nGuardedAllocationBytes_00465064 - allocation->size;
    free(block);
    if (previous != 0)
        previous->next = allocation->next;
    else
        g_pGuardedAllocationHead_004650b0 = allocation->next;
    if (g_pGuardedAllocationTail_005a89ac == allocation) {
        if (previous != 0)
            g_pGuardedAllocationTail_005a89ac = previous;
        else
            g_pGuardedAllocationTail_005a89ac = 0;
    }
    free(allocation);
}
