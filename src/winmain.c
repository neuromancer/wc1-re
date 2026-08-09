/*
 *  Win32 shell: window creation, message loop, exit paths.
 *
 *  Address range 0x401000-0x402dff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: CreateMainWindow/MainWindowProc/AbortToDesktop cluster; string band 0x465048-0x465354.
 */
#include "wc1.h"

/* Function start: 0x401020 */
void RestoreGamePalette(void)
{
    WaitForVerticalBlankThunk();
    DIBwholePaletteFromWords(DAT_005a8a50);
}

/* Function start: 0x4011D0 */
void ReleaseSoundEffectShip(signed char ship)
{
    short index = ship;

    g_aiSoundEffectShipActive_005a66f0[index] = 0;
    remove_object(index);
    g_nActiveSoundEffectShips_00465044 =
        MaxShort(0, g_nActiveSoundEffectShips_00465044 - 1);
}

/* Function start: 0x401210 */
void ReleaseAllSfxSlots(void)
{
    short i = 0;

    do {
        ReleaseSoundEffectShip(g_abSoundEffectShips_0046c028[i]);
        g_abSoundEffectShips_0046c028[i] = -1;
        i = i + 1;
    } while (i < 0x14);
    g_nSoundEffectSlotCount_0059bfe0 = 0;
}

/* Function start: 0x401250 */
unsigned int GetSfxDistanceFromCamera(void)
{
    return abs(0x19 - (int)DAT_00465040);
}

/* Function start: 0x401270 */
void PickRandomTauntDelay(void)
{
    MinShort(0x14, (short)RandomBelowOrEqual(7) + 10);
}

/* Function start: 0x401390 */
/* Divides in place and returns the quotient: *p keeps only the multiple of n. */
short SumShortArray(short *p, short n)
{
    short v = *p;

    *p = v - v % n;
    return v / n;
}

/* Function start: 0x401680 */
void TransformObjectVector(int p, int *q)
{
    IsPointWithinRange((FixedVector *)(p + 4), (FixedVector *)q,
                       *(short *)(p + 0x10) + 0x10cc);
}

/* Function start: 0x4016A0 */
void TransformObjectVectorAlt(int p, int *q)
{
    IsPointWithinRange((FixedVector *)(p + 4), (FixedVector *)q,
                       *(short *)(p + 0x10));
}

/* Function start: 0x401870 */
void PlayEngineRumble(void)
{
    find_ratio(0, 0x14, DAT_00465040, 0x10cc, 0xc1c);
}

/* Function start: 0x401930 */
void ClearShipSlotState4(short i)
{
    if (g_aeObjectClass_0059d100[i] == OBJECT_CLASS_DUST)
        g_aeObjectClass_0059d100[i] = OBJECT_CLASS_NULL;
}

/* Function start: 0x401CE0 */
void WarpMouseTo(short x, short y)
{
    DAT_0059af70 = x;
    DAT_0059af72 = y;
    DAT_0059ab10 = x;
    DAT_0059ab12 = y;
    SetMouseHomePosition(x, y);
}

/* Function start: 0x401D10 */
void CheckLauncherAndConfig(void)
{
    FILE *config;
    char option[100];

    if (ReadCheaterFlagFromRegistry() != 0) {
        g_nOriginDevUnlock_00469ff4 = 1;
        DAT_00469ffc = 0;
        DAT_0046a000 = 0;
    }

    config = fopen("WINGCMDR.CFG", "rt");
    if (config == 0)
        return;

    while (fscanf(config, "%s", option) != EOF) {
        char command = option[0] == '-' ? option[1] : option[0];

        if (memcmp(option, "$#SAGA.EXE", 11) == 0)
            DAT_0046506c = 1;
        switch (command) {
        case 'b':
            DAT_0046a000 = 0;
            break;
        case 'c':
            DAT_0046507c = 0;
            break;
        case 'f':
            DAT_00465070 = 1;
            break;
        case 'k':
            DAT_00469ffc = 0;
            break;
        case 'q':
            DAT_00465074 = 0;
            break;
        }
    }
    fclose(config);
}

/* Function start: 0x401E30 */
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
        CloseHandle(DAT_005a89a4);
        return 0;
    }

    memset(&memoryStatus, 0, sizeof(memoryStatus));
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatus(&memoryStatus);
    if (memoryStatus.dwTotalPhys < 0x800000) {
        MessageBoxA(0,
                    "You must have at leat 8 megs of memory available to play Wing Commander 1 for Windows95",
                    "ATTENTION", MB_ICONERROR);
        return 0;
    }
    if (memoryStatus.dwTotalPageFile < 0x800000) {
        MessageBoxA(0,
                    "You must have at leat 8 megs of virtual memory available to play Wing Commander 1 for Window95",
                    "ATTENTION", MB_ICONERROR);
        return 0;
    }

    if (waveOutGetNumDevs() == 0)
        DAT_00465058 = 0;
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
    srand((unsigned int)time(0));
    InitGameClockEpoch();
    DAT_005a8a44 = (unsigned int)time(0);
    ShowCursor(FALSE);
    DAT_0059ab2c = 0;
    SetRect(&clip, 0, 0, 320, 200);
    ClipCursor(&clip);

    main(0, (char **)"Vj");

    ClipCursor(0);
    ShowCursor(TRUE);
    DAT_005a8a38 = (unsigned int)time(0);
    DestroyWindow(DAT_005a89a0);
    DIBunInstall();
    CloseHandle(DAT_005a89a4);
    return 1;
}

/* Function start: 0x402070 */
void ShutdownGameWindow(void)
{
    DAT_005a8a38 = (unsigned int)time(0);
    ClipCursor(0);
    ShowCursor(TRUE);
    DestroyWindow(DAT_005a89a0);
    DIBunInstall();
    SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
    CloseHandle(DAT_005a89a4);
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
    HANDLE process;

    ClipCursor(0);
    ShowCursor(TRUE);
    process = GetCurrentProcess();
    SetPriorityClass(process, IDLE_PRIORITY_CLASS);
    sprintf(g_szMemoryUsage_005a89b0,
            "Current: %i\nMax    : %i\nTotal : %i\n",
            g_nGuardedAllocationBytes_00465064,
            g_nGuardedAllocationPeakBytes_00465068,
            g_nGuardedAllocationTotalBytes_00465060);
    OutputDebugStringA("Memory Info:\n");
    OutputDebugStringA(g_szMemoryUsage_005a89b0);
    CloseHandle(DAT_005a89a4);
    return 0;
}

/* Function start: 0x402180 */
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
                                   instance, 0);
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

/* Function start: 0x402320 */
unsigned int PumpWindowMessages(void)
{
    MSG message;

    if (DAT_004650a8 != 0)
        return 1;
    DAT_004650a8 = 1;
    if (DAT_0059ab2c != 0)
        DAT_0059ab2c();

    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            DAT_005a8a3c = 0;
            break;
        }
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    DAT_004650a8 = 0;
    DAT_0059ab54 = GetTickCount() * 60 / 1000;
    return DAT_005a8a3c;
}

/* Function start: 0x402520 */
unsigned int GetF1KeyLatch(void)
{
    return DAT_004650ac;
}

/* Function start: 0x402530 */
LRESULT CALLBACK MainWindowProc(HWND window, UINT message,
                                WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paint;

    switch (message) {
    case WM_SETFOCUS:
        return 0;
    case WM_CLOSE:
    case WM_DESTROY:
        DAT_005a8a3c = 0;
        ClipCursor(0);
        ShowCursor(TRUE);
        SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        BeginPaint(window, &paint);
        EndPaint(window, &paint);
        break;
    case WM_KEYDOWN:
        DAT_005a8964 = (unsigned int)wParam;
        if (wParam == VK_F1)
            DAT_004650ac = (lParam & 0x40000000) == 0;
        if (((lParam >> 16) & 0xff) == 1)
            DAT_0059ab58 = 1;
        break;
    case WM_KEYUP:
        DAT_005a8964 = 0;
        if (wParam == VK_F1)
            DAT_004650ac = 0;
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_SCREENSAVE ||
            (wParam & 0xfff0) == SC_MONITORPOWER)
            return 0;
        break;
    }
    return DefWindowProcA(window, message, wParam, lParam);
}

/* Function start: 0x402AC0 */
short GetJoystickButtons(void)
{
    return ((short)DAT_005a898c << 2) | DAT_005a897c;
}

/* Function start: 0x402B80 */
HINSTANCE GetApplicationInstance(void)
{
    return DAT_005a8a40;
}

/* Function start: 0x402BB0 */
void *AllocateGuardedMemory(unsigned int size)
{
    GuardedAllocation *allocation;

    if (g_pGuardedAllocationHead_004650b0 == 0) {
        allocation =
            (GuardedAllocation *)malloc(sizeof(GuardedAllocation));
        g_pGuardedAllocationHead_004650b0 = allocation;
    } else {
        g_pGuardedAllocationTail_005a89ac->next =
            (GuardedAllocation *)malloc(sizeof(GuardedAllocation));
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

/* Function start: 0x402DB0 */
void FreeGuardedAllocation(void *memory)
{
    GuardedAllocation *allocation = g_pGuardedAllocationHead_004650b0;
    GuardedAllocation *previous = 0;
    unsigned int *guard;
    void *block = (unsigned char *)memory - 0x400;
    int corrupt;
    int i;

    guard = (unsigned int *)block;
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
        if (*guard != 0xabababab)
            corrupt = corrupt + 1;
        guard = guard + 1;
        i = i - 1;
    } while (i != 0);
    if (corrupt != 0)
        ReportHeapGuardCorruption(memory, corrupt, 0);

    corrupt = 0;
    guard = (unsigned int *)((unsigned char *)memory + allocation->size);
    i = 0x100;
    do {
        if (*guard != 0xabababab)
            corrupt = corrupt + 1;
        guard = guard + 1;
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
