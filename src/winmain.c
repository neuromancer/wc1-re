/*
 *  Win32 shell: window creation, message loop, exit paths.
 *
 *  Address range 0x401000-0x402dff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: CreateMainWindow/MainWindowProc/AbortToDesktop cluster; string band 0x465048-0x465354.
 */
#include "wc1.h"

/* Function start: 0x401000 */
void SaveGamePalette(void)
{
    int index;
    unsigned short *entry;

    index = 0;
    entry = awPaletteRgbWords;
    do {
        GetPaletteEntry((short)index, entry);
        entry += 3;
        index++;
    } while (entry < awPaletteRgbWords + 0x300);
}

/* Function start: 0x401020 */
void RestoreGamePalette(void)
{
    WaitForVerticalBlankThunk();
    DIBwholePaletteFromWords(awPaletteRgbWords);
}

/* Function start: 0x401040 */
short easy2see(short obj)
{
    short bounds[4];
    short x;
    short y;
    unsigned char *shape;

    x = asObjectScreenX[obj];
    if (x == (short)0x8001)
        return 0;
    x = (short)(x + nViewCenterX);
    y = asObjectScreenY[obj];
    shape = apObjectShape[obj];
    y = (short)(y + nViewCenterY);
    return GetTransformedShapeBounds(
        &stSpaceBuffer, x, y, shape,
        asObjectViewFrame[obj],
        asObjectScreenAngle[obj],
        asObjectScreenScale[obj],
        asObjectFlip[obj], bounds);
}

/* Function start: 0x4010C0 */
void make_shard(short asteroid, FixedVector direction)
{
    short fragment;
    short speed;

    fragment = find_vacant_3d_object();
    if (fragment == -1)
        return;
    set_objects_data(fragment, OBJECT_TYPE_ROCK_CHUNK, asteroid);
    asObjectCounter[fragment] = 40;
    acObjectOwner[fragment] = (signed char)asteroid;
    SetVectorFixedPoint((unsigned int *)&direction,
                        (short)(asObjectCollisionRadius[asteroid] >> 1));
    AddFixedVectors(&aShipPosition[asteroid], &direction,
                    &aShipPosition[fragment]);
    aShipForwardVector[fragment] = direction;
    fix_objects_ijk(fragment);
    alter_yaw(signed_random(20), fragment);
    alter_pitch(signed_random(20), fragment);
    aShipVelocity[fragment] =
        aShipForwardVector[fragment];
    speed = (short)(real_velocity(asteroid) + RandomInRange(0, 5));
    SetVectorFixedPoint(
        (unsigned int *)&aShipVelocity[fragment], speed);
}

/* Base flight times and pitch windows for the four forward view bands. */
static const signed char acHazardTravelTimeByView[8] = {
    56, 52, 75, 73, 0, 0, 0, 0
};

static const signed char acHazardPitchRange[8] = {
    -10, 4, -8, 8, -12, 8, -8, 8
};

/* Function start: 0x4011D0 */
void remove_hazard(signed char hazard)
{
    aiSoundEffectSourceActive[(short)hazard + 1] = 0;
    remove_object((short)hazard);
    nActiveHazards =
        MaxShort(0, (short)(nActiveHazards - 1));
}

/* Function start: 0x401210 */
void remove_all_hazards(void)
{
    short slot = 0;

    do {
        remove_hazard(abHazardObjects[slot]);
        abHazardObjects[slot] = -1;
        slot++;
    } while (slot < 20);
    pActiveHazardField = 0;
}

/* Function start: 0x401250 */
short difficulty(void)
{
    return (short)(abs(25 - (int)nHazardReferenceSpeed) * 2);
}

/* Function start: 0x401270 */
short asteroid_velocity(void)
{
    return MinShort(20, (short)(RandomBelowOrEqual(7) + 10));
}

/* Function start: 0x401290 */
void skew_randomly(short obj, short allowReverse)
{
    FixedVector saved;

    if (RandomBelow(100) < 50) {
        saved = aShipRightVector[obj];
        aShipRightVector[obj] =
            aShipForwardVector[obj];
        aShipForwardVector[obj] =
            aShipUpVector[obj];
        aShipUpVector[obj] = saved;
    } else {
        saved = aShipUpVector[obj];
        aShipUpVector[obj] =
            aShipForwardVector[obj];
        aShipForwardVector[obj] =
            aShipRightVector[obj];
        aShipRightVector[obj] = saved;
    }
    if (allowReverse != 0 && RandomBelow(100) < 50)
        negate_vector(&aShipForwardVector[obj]);
}

/* Function start: 0x401390 */
void align(short *value, short quantum)
{
    short current;

    current = *value;
    current = (short)(current - current % quantum);
    *value = current;
}

/* Function start: 0x4013B0 */
void init_hazard(short obj, FixedVector position, short moving)
{
    enum ObjectType type;
    FixedVector vector;
    short hazardMoves;
    short travelTime;
    short speed;

    hazardMoves = moving;
    type = OBJECT_TYPE_SPACE_MINE;
    if (pActiveHazardField->type == OBJECT_TYPE_ASTEROID_FIELD)
        type = (enum ObjectType)(OBJECT_TYPE_ASTEROID1 +
                                RandomBelowOrEqual(5));
    set_objects_data(obj, type, -1);
    aShipPosition[obj] = position;

    if (type == OBJECT_TYPE_SPACE_MINE) {
        point_at(obj, aShipPosition[0]);
        speed = 2;
        skew_randomly(obj, 1);
        hazardMoves = 0;
    } else if (hazardMoves != 0) {
        travelTime = 65;
        if (cCockpitView < 4)
            travelTime = acHazardTravelTimeByView[
                cCockpitView];
        travelTime = (short)(travelTime + RandomBelowOrEqual(15));
        if (bIntroSecondaryScene != 0) {
            travelTime = (short)(travelTime -
                                 RandomBelowOrEqual(difficulty()));
            travelTime = MaxShort(45, travelTime);
        } else {
            travelTime = (short)(travelTime -
                                 RandomBelowOrEqual(difficulty()));
            travelTime = MaxShort(7, travelTime);
        }
        ScaleFixedVector(&aShipVelocity[0],
                         (int)travelTime << 8, &vector);
        AddFixedVectors(&aShipPosition[0], &vector, &vector);
        point_at(obj, vector);
        speed = distance_between_points(
            &vector, &aShipPosition[obj]);
        travelTime = MaxShort(3,
            (short)(travelTime - RandomBelow(5)));
        speed = (short)(speed / travelTime);
    } else {
        point_at(obj, pActiveHazardField->center);
        speed = 0;
        skew_randomly(obj, 1);
        if (RandomBelow(100) >= 20)
            speed = asteroid_velocity();
    }
    if (kilrathi_near(0, 16000) != 0)
        speed = 0;
    ScaleFixedVector(&aShipForwardVector[obj],
                     (int)speed << 8, &aShipVelocity[obj]);

    if (hazardMoves == 0) {
        int separation;

        if (type == OBJECT_TYPE_ASTEROID_FIELD)
            separation = 1500;
        else
            separation = RandomBelowOrEqual(1000) << 8;
        ScaleFixedVector(&aShipForwardVector[obj],
                         separation, &vector);
        SubtractFixedVectors(&aShipPosition[obj], &vector,
                             &aShipPosition[obj]);
    }
    if (type == OBJECT_TYPE_SPACE_MINE) {
        align((short *)&aShipPosition[obj].x, 200);
        align((short *)&aShipPosition[obj].y, 200);
        align((short *)&aShipPosition[obj].z, 200);
    }
    nActiveHazards++;
    asObjectCounter[obj] = 0;
    acObjectCollisionGraceTicks[obj] = 0;
}

/* Function start: 0x401680 */
int near_field(const HazardField *field, const FixedVector *point)
{
    return IsPointWithinRange((FixedVector *)&field->center,
                              (FixedVector *)point,
                              (short)(field->innerRadius + 4300));
}

/* Function start: 0x4016A0 */
short within_field(const HazardField *field, const FixedVector *point)
{
    return IsPointWithinRange((FixedVector *)&field->center,
                              (FixedVector *)point, field->innerRadius);
}

/* Function start: 0x4016C0 */
short try_far_spot(FixedVector *spot, short *moving)
{
    short yaw;
    short pitch;
    unsigned short outsideRange;

    copy_frame(0, 63);
    aShipPosition[63] = aShipPosition[0];
    pitch = signed_random(20);
    yaw = signed_random(35);
    if (nCameraViewMode == 0 && cCockpitView <= 3) {
        signed char minimum;
        signed char maximum;

        minimum = acHazardPitchRange[
            cCockpitView * 2];
        if (pitch > minimum &&
            pitch < (maximum = acHazardPitchRange[
                         cCockpitView * 2 + 1]) &&
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
        -15, 15, anObjectPitchRotation[0], -150, 150));
    yaw = (short)(yaw + find_ratio(
        -15, 15, anObjectYawRotation[0], -150, 150));
    rotate_about_j(yaw, &aShipRightVector[63],
                   &aShipForwardVector[63]);
    rotate_about_i(pitch, &aShipUpVector[63],
                   &aShipForwardVector[63]);
    position_relative_ijk(spot, 63, 0, 0, 3050);
    outsideRange = !(unsigned short)IsPointWithinRange(
        &aShipPosition[0], spot, 3000);
    return outsideRange != 0 &&
           within_field(pActiveHazardField, spot) != 0;
}

/* Function start: 0x401870 */
short rear_sphere(void)
{
    return find_ratio(0, 20, (short)nHazardReferenceSpeed,
                      4300, 3100);
}

/* Function start: 0x401890 */
int ok_hazard_spot(short obj)
{
    int range = 4300;

    if (asObjectScreenX[obj] == (short)0x8001)
        range = rear_sphere();
    return IsPointWithinRange(&aShipPosition[0],
                              &aShipPosition[obj],
                              (short)range);
}

/* Function start: 0x4018D0 */
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

/* Function start: 0x401930 */
void extra_hazard(short obj)
{
    if (aeObjectClass[obj] == OBJECT_CLASS_DUST)
        aeObjectClass[obj] = OBJECT_CLASS_NULL;
}

/* Function start: 0x401950 */
void approach(short obj)
{
    FixedVector target;
    FixedVector thrust;

    ScaleFixedVector(&aShipVelocity[0], 20 << 8, &target);
    AddFixedVectors(&aShipPosition[0], &target, &target);
    point_at(obj, target);
    ScaleFixedVector(&aShipForwardVector[obj], 20 << 8,
                     &thrust);
    AddFixedVectors(&aShipVelocity[obj], &thrust,
                    &aShipVelocity[obj]);
}

/* Function start: 0x4019E0 */
void manage_hazard(short obj, short slot)
{
    if (nRenderedSpaceFrame % 20 != slot)
        return;
    if (ok_hazard_spot(obj) == 0) {
        remove_hazard((signed char)obj);
        return;
    }
    if (aeObjectType[obj] == OBJECT_TYPE_SPACE_MINE &&
        asObjectScreenX[obj] != (short)0x8001 &&
        (unsigned short)asObjectDistance[obj] > 1500 &&
        real_velocity(obj) < 20)
        approach(obj);
}

/* Function start: 0x401A60 */
void match_ship_to_eye(void)
{
    aShipPosition[0] = aShipPosition[61];
    nHazardReferenceSpeed = 100;
    aShipRightVector[0] = aShipRightVector[61];
    aShipUpVector[0] = aShipUpVector[61];
    aShipForwardVector[0] =
        aShipForwardVector[61];
    ScaleFixedVector(&aShipForwardVector[0], 100 << 8,
                     &aShipVelocity[0]);
    pActiveHazardField->center = aShipPosition[61];
}

/* Function start: 0x401B30 */
void update_hazards(void)
{
    short slot;
    short emptySlot = -1;

    if (bIntroSecondaryScene != 0)
        match_ship_to_eye();
    else
        nHazardReferenceSpeed = real_velocity(0);
    slot = 0;
    do {
        if (abHazardObjects[slot] != -1)
            manage_hazard((short)abHazardObjects[slot], slot);
        else
            emptySlot = slot;
        slot++;
    } while (slot < 20);
    if (emptySlot != -1 &&
        RandomBelowOrEqual(215) <
            (short)nHazardReferenceSpeed + 30)
        abHazardObjects[emptySlot] = (signed char)make_hazard();
}

/* Function start: 0x401BC0 */
void start_hazard_field(short region)
{
    short slot;

    remove_all_hazards();
    pActiveHazardField = &aHazardFields[region];
    slot = 1;
    do {
        abHazardObjects[slot] = (signed char)make_hazard();
    } while (slot++ < 3);
}

/* Function start: 0x401C00 */
void add_hazard_field(enum ObjectType type, FixedVector center,
                      short radius, short density)
{
    HazardField *field;

    if (nHazardFieldCount >= 7)
        return;
    field = &aHazardFields[nHazardFieldCount];
    field->type = type;
    field->center = center;
    field->outerRadius = radius;
    field->innerRadius = radius;
    field->density = density;
    nHazardFieldCount++;
}

/* Function start: 0x401C60 */
void check_hazards(void)
{
    HazardField *field;
    short region;

    if (bIntroSecondaryScene != 0)
        return;
    if (pActiveHazardField == 0) {
        region = 0;
        field = aHazardFields;
        while (region < nHazardFieldCount) {
            if (field != pActiveHazardField &&
                near_field(field, &aShipPosition[0]) != 0) {
                start_hazard_field(region);
                return;
            }
            region++;
            field++;
        }
    } else if (near_field(pActiveHazardField,
                          &aShipPosition[0]) == 0) {
        remove_all_hazards();
    }
}

/* Function start: 0x401CE0 */
void __stdcall WarpMouseTo(short x, short y)
{
#ifndef WC1_SDL
    __asm cli
#endif
    stHostMouseState.x = x;
    stHostMouseState.y = y;
    stMouseCursorState.x = x;
    stMouseCursorState.y = y;
    SetMouseHomePosition(x, y);
#ifndef WC1_SDL
    __asm sti
#endif
}

/* Function start: 0x401D10 */
void CheckLauncherAndConfig(void)
{
    FILE *config;
    char option[100];
#ifdef WC1_SDL
    char resolvedPath[PATH_MAX];
#endif

    if (ReadCheaterFlagFromRegistry() != 0) {
        *(unsigned char *)&nOriginDevUnlock = 1;
        *(unsigned char *)&bPlayerVulnerable = 0;
        *(unsigned char *)&bPlayerCollisionResponse = 0;
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
                bShowKilrathiSagaCredits = 1;
            command = option[0] == '-' ? option[1] : option[0];
            switch (command) {
            case 'b':
                *(unsigned char *)&bPlayerCollisionResponse = 0;
                break;
            case 'c':
                bCockpitEnabled = 0;
                break;
            case 'f':
                bShowFrameRate = 1;
                break;
            case 'k':
                *(unsigned char *)&bPlayerVulnerable = 0;
                break;
            case 'q':
                bDirectDrawModeCascadeEnabled = 0;
                break;
            }
            if (config == 0)
                return;
        }
        fclose(config);
    }
}

#ifndef WC1_SDL

/* Function start: 0x401E30 */
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous,
                   LPSTR commandLine, int showCommand)
{
    MEMORYSTATUS memoryStatus;
    RECT clip;
    HANDLE process;

    (void)commandLine;
    hSingleInstanceSemaphore =
        CreateSemaphoreA(0, 0, 1, "Wing Commander 1");
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
        bIxAudioEnabled = 0;
    CheckLauncherAndConfig();
    if (!PromptInsertCorrectCd()) {
        CloseHandle(hSingleInstanceSemaphore);
        return 0;
    }
    if (!CreateMainWindow(instance, previous, showCommand))
        return 0;

    bMainWindowMinimized = 0;
    process = GetCurrentProcess();
    MonoDebug_install();
    SetPriorityClass(process, HIGH_PRIORITY_CLASS);
    if (bIxAudioEnabled != 0) {
        InitializeAudioSystem(hMainWindow);
        InitializeAudioStreamer(hMainWindow);
    }
    srand((unsigned int)time(0));
    InitGameClockEpoch();
    CreateDebugOverlayConsole(instance, hMainWindow, 60, 20);
    nSessionStartTime = (unsigned int)time(0);
    ShowCursor(FALSE);
    pEventManagerPump = 0;
    clip.left = 0;
    clip.top = 0;
    clip.right = 320;
    clip.bottom = 200;
    ClipCursor(&clip);

    _onexit((_onexit_t)AbortToDesktop);
    main(0, (char **)"Vj");

    ClipCursor(0);
    ShowCursor(TRUE);
    nSessionEndTime = (unsigned int)time(0);
    DestroyGlobalDebugOverlayConsole();
    ServiceAudioStream();
    DestroyWindow(hMainWindow);
    DIBunInstall();
    Streamer_close();
    CloseHandle(hSingleInstanceSemaphore);
    return 1;
}

#endif

/* Function start: 0x402070 */
void ShutdownGameWindow(void)
{
    nSessionEndTime = (unsigned int)time(0);
#ifdef WC1_SDL
    {
        SDL_Window *window;

        bMainWindowAlive = 0;
        if ((dwStreamerState & 1) != 0)
            ix_streamer_destroy();
        ServiceAudioStream();
        DestroyGlobalDebugOverlayConsole();
        window = (SDL_Window *)hMainWindow;
        DIBunInstall();
        Wc1SdlShutdownJoysticks();
        if (window != 0)
            SDL_DestroyWindow(window);
        hMainWindow = 0;
        SDL_Quit();
    }
#else
    ClipCursor(0);
    ShowCursor(TRUE);
    DestroyGlobalDebugOverlayConsole();
    DestroyWindow(hMainWindow);
    DIBunInstall();
    ClipCursor(0);
    ShowCursor(TRUE);
    {
        HANDLE process;

        process = GetCurrentProcess();
        SetPriorityClass(process, IDLE_PRIORITY_CLASS);
    }
    CloseHandle(hSingleInstanceSemaphore);
#endif
    exit(0);
}

/* Function start: 0x4020E0 */
void ShowNoticeMessageBox(const char *text)
{
    DIBpositionWindow();
    MessageBoxA(0, text, "NOTICE", 0x10);
    exit(0);
}

/* Function start: 0x402110 */
unsigned int AbortToDesktop(void)
{
#ifndef WC1_SDL
    HANDLE process;

    ClipCursor(0);
    ShowCursor(TRUE);
    process = GetCurrentProcess();
    SetPriorityClass(process, IDLE_PRIORITY_CLASS);
#endif
    sprintf(szMemoryUsage,
            "Current: %i\nMax    : %i\nTotal : %i\n",
            nGuardedAllocationBytes,
            nGuardedAllocationPeakBytes,
            nGuardedAllocationTotalBytes);
    OutputDebugStringA("Memory Info:\n");
    OutputDebugStringA(szMemoryUsage);
#ifndef WC1_SDL
    CloseHandle(hSingleInstanceSemaphore);
#endif
    return 0;
}

#ifndef WC1_SDL

/* Function start: 0x402180 */
int CreateMainWindow(HINSTANCE instance, HINSTANCE previous,
                     int showCommand)
{
    WNDCLASSA windowClass;

    hAppInstance = instance;
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

    hMainWindow = CreateWindowExA(0, "Wing Commander",
                                             "Wing Commander",
                                             WS_POPUP, 0, 0, 320, 200,
                                             0, 0,
                                             hAppInstance, 0);
    if (hMainWindow == 0) {
        GetLastError();
        return 0;
    }

    hMainWindowDC = GetDC(hMainWindow);
    SetTextColor(hMainWindowDC, RGB(255, 0, 0));
    SetBkColor(hMainWindowDC, RGB(0, 0, 0));
    if (GetDeviceCaps(hMainWindowDC, BITSPIXEL) < 8) {
        MessageBoxA(0,
                    "You must be running with 256, or more, colors to play Wing Commander",
                    "NOTICE", MB_ICONEXCLAMATION);
        DestroyWindow(hMainWindow);
        return 0;
    }

    hPreviousCursor = SetCursor(0);
    ShowWindow(hMainWindow, showCommand);
    UpdateWindow(hMainWindow);
    PumpWindowMessages();
    PumpWindowMessages();
    PumpWindowMessages();
    DIBinstall(hMainWindow);
    bMainWindowAlive = 1;
    return 1;
}

#endif

/* Function start: 0x402320 */
unsigned int PumpWindowMessages(void)
{
#ifndef WC1_SDL
    RECT clip;
    MSG message;
    int cursorX;
    int cursorY;
    int done;
#endif

    if (bWindowMessagePumpActive != 0)
        return 1;
    bWindowMessagePumpActive = 1;
    if (pEventManagerPump != 0)
        pEventManagerPump();
#ifdef WC1_SDL
    Wc1SdlPumpEvents();
#else
    done = 0;
    do {
        if (bMainWindowMinimized != 0) {
            if (GetMessageA(&message, 0, 0, 0) != 0) {
                done = 1;
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } else {
                LogMemoryUsage();
                ShutdownGameWindow();
            }
            if (IsIconic(hMainWindow) == 0)
                bMainWindowMinimized = 0;
            if (bMainWindowMinimized == 0) {
                clip.left = 0;
                clip.top = 0;
                clip.right = 320;
                clip.bottom = 200;
                ShowCursor(FALSE);
                SetPriorityClass(GetCurrentProcess(),
                                 HIGH_PRIORITY_CLASS);
                SetActiveWindow(hMainWindow);
                SetForegroundWindow(hMainWindow);
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
        if (IsIconic(hMainWindow) != 0) {
            if (bMainWindowMinimized == 0) {
                cursorX = 160;
                cursorY = 100;
                ClipCursor(0);
                ShowCursor(TRUE);
                SetPriorityClass(GetCurrentProcess(),
                                 NORMAL_PRIORITY_CLASS);
            }
            bMainWindowMinimized = 1;
            if (bMainWindowMinimized != 0)
                done = 0;
        }
    } while (done == 0);
#endif
    nTickCount60Hz = GetTickCount() * 60 / 1000;
    bWindowMessagePumpActive = 0;
    return bMainWindowAlive;
}

/* Function start: 0x402520 */
unsigned int GetF1KeyLatch(void)
{
    return bF1KeyLatch;
}

#ifndef WC1_SDL

/* Function start: 0x402530 */
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
        bMainWindowAlive = 0;
        ClipCursor(0);
        ShowCursor(TRUE);
        SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
        PostQuitMessage(0);
        break;
    case WM_QUIT:
        bMainWindowAlive = 0;
        ClipCursor(0);
        ShowCursor(TRUE);
        SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
        break;
    case WM_PAINT:
        BeginPaint(hMainWindow, &paint);
        EndPaint(hMainWindow, &paint);
        break;
    case WM_KEYDOWN:
        if (bKeyEventQueueEnabled != 0)
            QueueInputEvent(3, 0, 0, (unsigned short)wParam,
                            0, 0, 0);
        if (wParam == VK_F1) {
            bF1KeyLatch = 1;
            if ((lParam & 0x40000000) != 0)
                bF1KeyLatch = 0;
        }
        scanCode = ((unsigned long)lParam & 0xff0000) >> 16;
        if (scanCode == 1)
            bEscapePressed = 1;
        QueueInputEvent(3, 0, 0, (unsigned short)scanCode,
                        0, 0, 0);
        SetInputKeyState((int)scanCode, 1);
        break;
    case WM_KEYUP:
        if (bKeyEventQueueEnabled != 0)
            QueueInputEvent(4, 0, 0, (unsigned short)wParam,
                            0, 0, 0);
        if (wParam == VK_F1)
            bF1KeyLatch = 0;
        scanCode = ((unsigned long)lParam & 0xff0000) >> 16;
        QueueInputEvent(4, 0, 0, (unsigned short)scanCode,
                        0, 0, 0);
        SetInputKeyState((int)scanCode, 0);
        break;
    case WM_SYSKEYDOWN:
        nSystemKeyDown = (unsigned int)wParam;
        if (wParam == 'X' &&
            ((unsigned long)lParam & 0x20000000) != 0) {
            PostQuitMessage(0);
            sprintf(szMemoryUsage,
                    "Current: %i\nMax    : %i\nTotal : %i\n",
                    nGuardedAllocationBytes,
                    nGuardedAllocationPeakBytes,
                    nGuardedAllocationTotalBytes);
            OutputDebugStringA("Memory Info:\n");
            OutputDebugStringA(szMemoryUsage);
        } else if (wParam == 'N') {
            ReportSpaceFlightMaxFps(-0.5f);
        } else if (wParam == 'M') {
            ReportSpaceFlightMaxFps(0.5f);
        }
        break;
    case WM_SYSKEYUP:
        nSystemKeyDown = 0;
        break;
    case WM_COMMAND:
        if (((unsigned int)wParam & 0xffff) == 3) {
            bMainWindowAlive = 0;
            PostQuitMessage(0);
        }
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_SCREENSAVE ||
            (wParam & 0xfff0) == SC_MONITORPOWER)
            return 0;
        break;
    case WM_MOUSEMOVE:
        if (bPointerMovedByKeyboard != 0) {
            bPointerMovedByKeyboard = 0;
            break;
        }
        eventType = 13;
        QueueInputEvent(eventType, (unsigned short)mouseX,
                        (unsigned short)mouseY, 0,
                        primaryButton, secondaryButton, 0);
        mouseEvent = 1;
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        eventType = 2;
        QueueInputEvent(eventType, (unsigned short)mouseX,
                        (unsigned short)mouseY, 0,
                        primaryButton, secondaryButton, 0);
        mouseEvent = 1;
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        eventType = 1;
        QueueInputEvent(eventType, (unsigned short)mouseX,
                        (unsigned short)mouseY, 0,
                        primaryButton, secondaryButton, 0);
        mouseEvent = 1;
        break;
    }
    if (mouseEvent != 0) {
        nHostMouseMessageX = mouseX;
        nHostMouseMessageY = mouseY;
        bHostPrimaryMouseButton = primaryButton;
        bHostSecondaryMouseButton = secondaryButton;
    }
    return DefWindowProcA(window, message, wParam, lParam);
}

#endif

/* Function start: 0x402A20 */
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
            device, &aJoystickInfo[infoIndex]) != FALSE) {
#else
    if (joyGetPos(device, &aJoystickInfo[infoIndex]) ==
        JOYERR_NOERROR) {
#endif
        *x = aJoystickInfo[infoIndex].wXpos;
        *y = aJoystickInfo[infoIndex].wYpos;
        buttonState = aJoystickInfo[infoIndex].wButtons;
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

/* Function start: 0x402AC0 */
short GetJoystickButtons(void)
{
    return ((short)aJoystickInfo[1].wButtons << 2) |
           (unsigned short)aJoystickInfo[0].wButtons;
}

/* Function start: 0x402AE0 */
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
        SystemDebugPrintf(szJoystickDevCapsFailure);
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

/* Function start: 0x402B80 */
HINSTANCE GetApplicationInstance(void)
{
    return hAppInstance;
}

/* Function start: 0x402B90 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
HWND GetMainWindowHandle(void)
{
    return hMainWindow;
}

/* Function start: 0x402BA0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
HDC GetMainWindowDeviceContext(void)
{
    return hMainWindowDC;
}

/* Function start: 0x402BB0 */
void *AllocateGuardedMemory(unsigned int size)
{
    GuardedAllocation *allocation;

    if (pGuardedAllocationHead == 0) {
        allocation =
            malloc(sizeof(GuardedAllocation));
        pGuardedAllocationHead = allocation;
    } else {
        pGuardedAllocationTail->next =
            malloc(sizeof(GuardedAllocation));
        allocation = pGuardedAllocationTail->next;
    }
    pGuardedAllocationTail = allocation;
    pGuardedAllocationTail->next = 0;
    pGuardedAllocationTail->size = size;
    pGuardedAllocationTail->block = malloc(size + 0x800);

    memset(pGuardedAllocationTail->block, 0xab, 0x400);
    memset((unsigned char *)pGuardedAllocationTail->block +
               0x400,
           0, size);
    memset((unsigned char *)pGuardedAllocationTail->block +
               0x400 + size,
           0xab, 0x400);
    nGuardedAllocationTotalBytes += size;
    nGuardedAllocationBytes += size;
    if (nGuardedAllocationPeakBytes <
        nGuardedAllocationBytes) {
        nGuardedAllocationPeakBytes =
            nGuardedAllocationBytes;
    }
    return (unsigned char *)pGuardedAllocationTail->block +
           0x400;
}

/* Function start: 0x402CA0 */
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

/* Function start: 0x402D40 */
void CheckAllGuardedAllocations(void)
{
    GuardedAllocation *allocation = pGuardedAllocationHead;
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
}

/* Function start: 0x402DB0 */
void FreeGuardedAllocation(void *memory)
{
    GuardedAllocation *allocation = pGuardedAllocationHead;
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

    nGuardedAllocationBytes =
        nGuardedAllocationBytes - allocation->size;
    free(block);
    if (previous != 0)
        previous->next = allocation->next;
    else
        pGuardedAllocationHead = allocation->next;
    if (pGuardedAllocationTail == allocation) {
        if (previous != 0)
            pGuardedAllocationTail = previous;
        else
            pGuardedAllocationTail = 0;
    }
    free(allocation);
}
