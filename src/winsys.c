/**
 * @file
 * @ingroup shell
 * @brief Win32 system services: mouse, keyboard and clock wrappers, font
 * workspaces, CD-ROM location and disc-swap prompts.
 *
 * Address range 402e80-4034ff. It is one unit, not two: PollKeyboardState's
 * statics and LocateStreamsDirOnDisc's szStreamsDir interleave in the
 * original's .bss (475b68-475d1f), and statics are private to one object.
 * Its string literals occupy 46535c-4653fc.
 */
#include "wc1.h"

/* Polled key states and the CD streams directory. CL places these statics
 * by identifier hash, so their spellings reproduce the original order; the
 * SDL flight adapter also reads the key states. */
#ifndef SDL_PORT
static
#endif
int nInsertKeyDown; /* 0x00475b68 */
static int nClearedKeyboard; /* 0x00475b6c */
/* Nothing in the shipped executable references this buffer. */
static unsigned char abUnusedPollArea[0x80]; /* 0x00475b70 */
#ifndef SDL_PORT
static
#endif
int nOemPeriodKeyStatus; /* 0x00475bf0 */
#ifndef SDL_PORT
static
#endif
int nHomeState; /* 0x00475bf4 */
#ifndef SDL_PORT
static
#endif
int nPageDownKeyHeld; /* 0x00475bf8 */
#ifndef SDL_PORT
static
#endif
int nOemCommaKeyPressed; /* 0x00475bfc */
#ifndef SDL_PORT
static
#endif
int nRightArrowKeyHeld; /* 0x00475c00 */
#ifndef SDL_PORT
static
#endif
int nEndKeyDown; /* 0x00475c04 */
static int nClearKeyState; /* 0x00475c08 */
#ifndef SDL_PORT
static
#endif
int nDeleteKeyDown; /* 0x00475c0c */
#ifndef SDL_PORT
static
#endif
int nUpArrowKeyState; /* 0x00475c10 */
#ifndef SDL_PORT
static
#endif
int nDownArrowKeyState; /* 0x00475c14 */
static char szStreamsDir[0x100]; /* 0x00475c18 */
#ifndef SDL_PORT
static
#endif
int nPageUpKeyState; /* 0x00475d18 */
#ifndef SDL_PORT
static
#endif
int nLeftArrowKeyState; /* 0x00475d1c */


/* Linker commons defined here; LINK orders them by identifier hash. */
HWND hMainWindow; /* 0x005a89a0 */
HANDLE hSingleInstanceSemaphore; /* 0x005a89a4 */
unsigned long dwGameClockBaseValue; /* 0x005a89a8 */
struct GuardedAllocation *pGuardedAllocationTail; /* 0x005a89ac */
char szMemoryUsage[0x80]; /* 0x005a89b0 */

/* Function start: 0x402E80 */
void SetMousePosition(int x, int y)
{
    SetCursorPos(x, y);
}

/* Function start: 0x402EA0 */
unsigned int PollKeyboardState(void)
{
    nClearedKeyboard = 0;
    nHomeState = GetAsyncKeyState(VK_HOME);
    nUpArrowKeyState = GetAsyncKeyState(VK_UP);
    nPageUpKeyState = GetAsyncKeyState(VK_PRIOR);
    nLeftArrowKeyState = GetAsyncKeyState(VK_LEFT);
    nRightArrowKeyHeld = GetAsyncKeyState(VK_RIGHT);
    nEndKeyDown = GetAsyncKeyState(VK_END);
    nDownArrowKeyState = GetAsyncKeyState(VK_DOWN);
    nPageDownKeyHeld = GetAsyncKeyState(VK_NEXT);
    nClearKeyState = GetAsyncKeyState(VK_CLEAR);
    nOemPeriodKeyStatus = GetAsyncKeyState(VK_OEM_PERIOD);
    nOemCommaKeyPressed = GetAsyncKeyState(VK_OEM_COMMA);
    nInsertKeyDown = GetAsyncKeyState(VK_INSERT);
    nDeleteKeyDown = GetAsyncKeyState(VK_DELETE);

    if (nHomeState != 0)
        return SCAN_HOME;
    if (nPageUpKeyState != 0)
        return SCAN_PAGEUP;
    if (nEndKeyDown != 0)
        return SCAN_END;
    if (nPageDownKeyHeld != 0)
        return SCAN_PAGEDOWN;
    if (nInsertKeyDown != 0 ||
        nOemCommaKeyPressed != 0)
        return SCAN_INSERT;
    if (nDeleteKeyDown != 0 ||
        nOemPeriodKeyStatus != 0)
        return SCAN_DELETE;
    if (nClearKeyState != 0)
        return SCAN_KP_5;
    if (nUpArrowKeyState != 0) {
        if (nLeftArrowKeyState != 0)
            return SCAN_HOME;
        return nRightArrowKeyHeld != 0 ? SCAN_PAGEUP : SCAN_UP;
    }
    if (nDownArrowKeyState != 0) {
        if (nLeftArrowKeyState != 0)
            return SCAN_END;
        return nRightArrowKeyHeld != 0 ? SCAN_PAGEDOWN : SCAN_DOWN;
    }
    if (nLeftArrowKeyState != 0)
        return SCAN_LEFT;
    return nRightArrowKeyHeld != 0 ? SCAN_RIGHT : SCAN_NONE;
}

/* Function start: 0x403060 */
int GetShiftKeyState(void)
{
    return GetAsyncKeyState(VK_SHIFT);
}

/* Function start: 0x403070 */
int GetControlKeyState(void)
{
    return GetAsyncKeyState(VK_CONTROL);
}

/* Function start: 0x403080 */
unsigned int GetKeyboardModifiers(void)
{
    return nSystemKeyDown;
}

/* Function start: 0x403090 */
unsigned int GetGameClockTicks(void)
{
    return (GetTickCount() - dwGameClockBaseValue) * 60 / 1000;
}

/* Function start: 0x4030B0 */
void InitGameClockEpoch(void)
{
    dwGameClockBaseValue = GetTickCount() + (rand() & 3600000);
}

/* Function start: 0x4030D0 */
void WriteDebugString(const char *s)
{
    OutputDebugString(s);
}

/* Function start: 0x4030E0 */
/* Byte-identical duplicate of SetMousePosition (0x00402E80); both exist because
 * VC++ 4.2's linker did no COMDAT folding. */
void SetMousePositionDuplicate(int x, int y)
{
    SetCursorPos(x, y);
}

/* Function start: 0x403100 */
FontWorkspace **AllocateFontWorkspace(short fontIndex)
{
    FontWorkspace **workspace;
    int index;

    workspace = malloc(sizeof(*workspace));
    *workspace = malloc(sizeof(**workspace));
    for (index = 0; index < 1; index++) {
        (*workspace)[index].width = 5;
        (*workspace)[index].height = 5;
        (*workspace)[index].pixels =
            malloc((*workspace)->width * (*workspace)->height);
        memset((*workspace)[index].pixels, 5,
               (*workspace)->width * (*workspace)->height);
    }
    return workspace;
}

/* Function start: 0x403180 */
void FreeFontWorkspace(FontWorkspace **workspace)
{
    int offset;

    offset = 0;
    do {
        free(((FontWorkspace *)((unsigned char *)*workspace + offset))->pixels);
        offset += sizeof(FontWorkspace);
    } while (offset < (int)sizeof(FontWorkspace));
    free(*workspace);
    free(workspace);
}

/* Function start: 0x4031C0 */
char *LocateStreamsDirOnDisc(void)
{
    char current[256];
    char *result;
    char drive;

    result = szStreamsDir;
    GetCurrentDirectoryA(0xff, current);
    drive = FindCdRomDriveByVolumeLabel("<anydisc>", "\\wc1\\streams\\");
    if (drive != 0) {
        sprintf(result, "%c:%s", drive,
                "\\wc1\\streams\\");
        return result;
    }

#ifdef SDL_PORT
    if (strstr(current, "gamedat") != 0 ||
        strstr(current, "GAMEDAT") != 0)
#else
    if (strstr(current, "gamedat") != 0)
#endif
        sprintf(result, "%s\\..\\%s", current, "streams\\");
    else
        sprintf(result, "%s\\%s", current, "streams\\");
    GetCurrentDirectoryA(0xff, current);
    if (!SetCurrentDirectoryA(result))
        result = 0;
    SetCurrentDirectoryA(current);
    return result;
}

/* Function start: 0x403290 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory)
{
    char drives[26];
    char scanRoot[12];
    char root[12];
    char volume[256];
    char filesystem[64];
    DWORD maximumComponentLength;
    DWORD flags;
    int driveCount = 0;
    int i;
    int scanDrive;
    char drive;
    char result;

    for (scanDrive = 'a'; scanDrive <= 'z'; scanDrive++) {
        sprintf(scanRoot, "%c:\\", (char)scanDrive);
        if (GetDriveTypeA(scanRoot) == DRIVE_CDROM) {
            drives[driveCount] = scanDrive;
            driveCount++;
        }
    }

    result = 0;
    for (i = 0; i < driveCount; i++) {
        drive = drives[i];
        sprintf(root, "%c:\\", drive);
        GetVolumeInformationA(root, volume, 0xff, 0,
                              &maximumComponentLength, &flags,
                              filesystem, sizeof(filesystem));
        if (memcmp(label, "<anydisc>", 10) == 0) {
            if (SetCurrentDirOnDrive(drive, directory) != 0) {
                result = drive;
                break;
            }
        } else if (strcmp(volume, label) == 0) {
            if (SetCurrentDirOnDrive(drive, directory) != 0) {
                result = drives[i];
                break;
            }
        }
    }
    return result;
}

/* Function start: 0x4033E0 */
int SetCurrentDirOnDrive(char drive, const char *directory)
{
    char path[256];
    char current[256];
    int result;

    result = 0;
    GetCurrentDirectoryA(0xff, current);
    sprintf(path, "%c:%s", drive, directory);
    if (SetCurrentDirectoryA(path) != 0)
        result = 1;
    SetCurrentDirectoryA(current);
    return result;
}

/* Function start: 0x403450 */
int PromptInsertCorrectCd(void)
{
    char title[18];
    char message[1024];

    strcpy(title, "Insert Correct CD");
    sprintf(message,
            "Please place The Kilrathi Saga disc %d into your CD-ROM drive and click OK\n"
            "or click cancel to quit",
            1);
    do {
        if (LocateStreamsDirOnDisc() != 0)
            return 1;
    } while (MessageBoxA(0, message, title,
                         MB_OKCANCEL | MB_ICONEXCLAMATION) != IDCANCEL);
    return 0;
}

/* Function start: 0x4034D0 */
short __stdcall OpenDataFileOrDie(const char *path)
{
#ifdef SDL_PORT
    short fd = _open(SdlTownsFilename(path), 0x8002);
#else
    short fd = _open(path, 0x8002);
#endif

    nPacketError = errno;
    return fd;
}
