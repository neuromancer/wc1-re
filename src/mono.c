/*
 *  Win32 data-file wrappers, scaled interstitial text, canned sequences,
 *  and the MONODEBG.VXD developer console.
 *
 *  Address range 403500-403e4f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MonoDebug_install/MonoDebug_print anchor the final block;
 *  the Mac auto unit starts at visit_the_cinema (403e50).
 */
#define WC1_MONO_SOURCE
#include "wc1.h"

short nPacketError = 0; /* 0x00465460 */
/* File-error buffers and MONODEBG state. CL places these statics by
 * identifier hash, so their spellings reproduce the original order. */
static char szDataFileReadError[0x40]; /* 0x00475d20 */
static char szDataCreateError[0x40]; /* 0x00475d60 */
static char szWriteDataError[0x40]; /* 0x00475da0 */
static char szSeekDataError[0x40]; /* 0x00475de0 */
/* Nothing in the shipped executable references this buffer. */
static unsigned char abUnusedMonoBuffer[0x50]; /* 0x00475e20 */
static int bMonoDebugAvailable; /* 0x00475e70 */
static HANDLE hMonoDebugDriver; /* 0x00475e74 */


/* Linker commons defined here; LINK orders them by identifier hash. */
int nHostMouseMessageY; /* 0x005a8994 */
int bHostPrimaryMouseButton; /* 0x005a8998 */
int bHostSecondaryMouseButton; /* 0x005a899c */

/* Function start: 0x403500 */
void __stdcall CloseDataFile(unsigned short fd)
{
    nPacketError = (short)_close(fd & 0xffff);
}

/* Function start: 0x403520 */
short __stdcall WriteDataFileAtOffset(unsigned short fd, int offset,
                                      unsigned int length, const void *data)
{
    nPacketError = 0;
    if (_lseek(fd, offset, SEEK_SET) == -1) {
        sprintf(szWriteDataError, "!lseek %d\n", offset);
        nPacketError = (short)errno;
        return 0;
    }
    if (_write(fd, data, length) == -1) {
        sprintf(szWriteDataError, "!write %d\n", offset);
        nPacketError = (short)errno;
        return 0;
    }
    return 1;
}

/* Function start: 0x4035C0 */
short __stdcall CreateDataFile(const char *path)
{
    unsigned short fd;

    fd = (unsigned short)_open(path, 0x8101, 0x180);
    if ((unsigned int)fd == (unsigned int)-1) {
        sprintf(szDataCreateError, "!_open '%s'\n", path);
        nPacketError = (short)errno;
        return 0;
    }
    return (short)fd;
}

/* Function start: 0x403610 */
int __stdcall ReadDataFileAtOffset(unsigned short fd, int offset,
                                   unsigned int length, void *data)
{
    nPacketError = 0;
    if (_lseek(fd, offset, SEEK_SET) == -1) {
        sprintf(szDataFileReadError, "!lseek %d\n", offset);
        nPacketError = (short)errno;
        return 0;
    }
    if (_read(fd, data, length) == -1) {
        sprintf(szDataFileReadError, "!lseek %d\n", offset);
        nPacketError = (short)errno;
        return 0;
    }
    return 1;
}

/* Function start: 0x4036B0 */
int __stdcall SeekDataFile(unsigned short fd, int offset,
                           unsigned int origin)
{
    int position;

    position = _lseek(fd, offset, origin & 0xffff);
    if (position == -1) {
        sprintf(szSeekDataError, "!lseek %d\n", offset);
        nPacketError = (short)errno;
    }
    return position;
}

/* Function start: 0x403710 */
int MeasureScaledIntroTextWidth(const char *text, short scale)
{
    short bounds[4];
    short width = 0;

    for (;;) {
        char c = *text++;

        if (c == 0)
            break;

        if (c >= 'A' && c <= 'z') {
            c -= 'A';

            GetTransformedShapeBounds(&stSpaceBufferViewport, 0, 0,
                                      pIntroFont, (short)c, 0,
                                      scale, 0, bounds);
            width = (short)(width + bounds[2] + 1);
            width = (short)(width + ((int)scale * 2 >> 8));
        } else if (c == ' ') {
            width = (short)(width + ((int)scale * 6 >> 8));
        } else if (c == '\n') {
            break;
        }
    }
    return width;
}

/* Function start: 0x4037A0 */
int DrawCenteredScaledIntroText(const char *text, short centreX,
                                short baselineY, short scale)
{
    short bounds[4];
    short x;
    short y;
    char c;

    x = (short)(centreX - MeasureScaledIntroTextWidth(text, scale) / 2);
    y = (short)(baselineY - ((int)scale * 16 >> 9));
    for (;;) {
        c = *text++;

        if (c == 0)
            break;

        if (c >= 'A' && c <= 'z') {
            c -= 'A';

            DrawSpriteScaled(&stSpaceBufferViewport, x, y,
                             pIntroFont, (short)c, 0,
                             scale, 0);
            GetTransformedShapeBounds(&stSpaceBufferViewport, 0, 0,
                                      pIntroFont, (short)c, 0,
                                      scale, 0, bounds);
            x = (short)(x + bounds[2] + 1);
            x = (short)(x + ((int)scale * 2 >> 8));
        } else if (c == ' ') {
            x = (short)(x + ((int)scale * 6 >> 8));
        } else if (c == '\n') {
            break;
        }
    }
    return 0;
}

/* Function start: 0x403890 */
short GetLineLength(const char *text)
{
    short width;
    char c;

    width = 0;
    for (;;) {
        c = *text;
        text++;
        if (c == 0)
            break;
        if (c >= 'A' && c <= 'z') {
            c = (char)(c - 'A');
            width = width + GetShapeFrameExtent(
                0, 0, pIntroFont, c, 2);
            width = width + 2;
        } else if (c == '.') {
            width = width + GetShapeFrameExtent(
                0, 0, pIntroFont, 58, 2);
            width = width + 2;
        } else if (c == ',') {
            width = width + GetShapeFrameExtent(
                0, 0, pIntroFont, 59, 2);
            width = width + 2;
        } else if (c == ' ') {
            width = width + 6;
        } else if (c == '\n') {
            break;
        }
    }
    return width;
}

/* print_subtitle's glyph blitter (gr.c); wc1funcs.h omits it for this unit. */
void DrawSpriteDefault(Viewport *viewport, short x, short y,
                       unsigned char *shape, short frame);

/* Function start: 0x403920 */
int print_subtitle(Viewport *viewport, short colour, const char *text)
{
    char c;
    const char *scan;
    short lines;
    short y;
    short x;

    lines = 1;
    scan = text;
    while ((c = *scan++) != 0) {
        if (c == '\n')
            lines++;
    }
    lines = (short)(lines * 16);
    y = (short)((128 - lines) / 2);
    x = GetLineLength(text);
    x = (short)((320 - x) >> 1);
    scan = text;
    while ((c = *scan++) != 0) {
        if (c >= 'A' && c <= 'z') {
            DrawSpriteDefault(viewport, x, y, pIntroFont, (char)(c - 'A'));
            x = x + GetShapeFrameExtent(
                0, 0, pIntroFont, (char)(c - 'A'), 2);
            x = x + 2;
        } else if (c == ' ') {
            x = x + 6;
        } else if (c == '.') {
            DrawSpriteDefault(viewport, x, y, pIntroFont, 58);
            x = x + GetShapeFrameExtent(
                0, 0, pIntroFont, 58, 2);
            x = x + 2;
        } else if (c == ',') {
            DrawSpriteDefault(viewport, x, y, pIntroFont, 59);
            x = x + GetShapeFrameExtent(
                0, 0, pIntroFont, 59, 2);
            x = x + 2;
        } else if (c == '\n') {
            y = y + 16;
            x = GetLineLength(scan);
            x = (short)((320 - x) >> 1);
        }
    }
    if (viewport->pixels == stScreenView.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x403A80 */
int advance_canned_sequence(short obj)
{
    const short *command;

    command = apCannedSequenceList[obj];
    asCannedCommands[obj] = *command++;
    switch (asCannedCommands[obj]) {
    case 0:
        asActionCounts[obj] = *command++;
        break;
    case 1:
        anYawTarget[obj] = *command++;
        anPitchGoal[obj] = *command++;
        anRollGoal[obj] = *command++;
        anShipSpeed[obj] = (int)*command++ << 8;
        break;
    case 2:
        explode(-1, obj);
        break;
    case 3:
        fire_fixed_projectile_weapon(obj);
        break;
    case 4:
        aeSpecialManeuvers[obj] =
            SPECIAL_MANEUVER_AFTERBURNER;
        break;
    }
    apCannedSequenceList[obj] = command;
    return 0;
}

/* Function start: 0x403B70 */
unsigned int update_canned_sequence(short obj)
{
    switch (asCannedCommands[obj]) {
    case 0:
        asActionCounts[obj]--;
        if (asActionCounts[obj] == 0)
            advance_canned_sequence(obj);
        break;
    case 1:
        if (anYawTarget[obj] == 0 &&
            anPitchGoal[obj] == 0 &&
            anRollGoal[obj] == 0) {
            /* A chained comparison: the speed window test compares the
               0/1 result of the first test with the upper bound. */
            if ((anShipSpeed[obj] - 0x400 <
                 Vector_magnitude(&aShipVelocity[0])) <
                anShipSpeed[obj] + 0x400)
                advance_canned_sequence(obj);
        }
        break;
    case 3:
    case 4:
        advance_canned_sequence(obj);
        break;
    }
    return 0;
}

/* Function start: 0x403C40 */
void __stdcall SplitGameClockTicks(unsigned char *parts)
{
    int ticks = (int)GetGameClockTicks();

    parts[0] = (unsigned char)(ticks % 60);
    ticks = ticks / 60;
    parts[1] = (unsigned char)(ticks % 60);
    ticks = ticks / 60;
    parts[2] = (unsigned char)(ticks % 60);
    ticks = ticks / 60;
    parts[3] = (unsigned char)(ticks % 24);
}

/* Function start: 0x403C90 */
void MonoDebug_install(void)
{
    unsigned int version;

    hMonoDebugDriver =
        CreateFileA("\\\\.\\MONODEBG.VXD", 0, 0, 0, CREATE_ALWAYS,
                    FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (hMonoDebugDriver == INVALID_HANDLE_VALUE)
        return;

    if (!DeviceIoControl(hMonoDebugDriver, 1, 0, 0,
                         &version, sizeof(version), 0, 0)) {
        CloseHandle(hMonoDebugDriver);
        return;
    }
    if (version != 0x20004) {
        CloseHandle(hMonoDebugDriver);
        exit_squadron("MonoDebug__install expecting version");
        return;
    }
    if (!DeviceIoControl(hMonoDebugDriver, 2, 0, 0,
                         0, 0, 0, 0)) {
        CloseHandle(hMonoDebugDriver);
        exit_squadron("MonoDebug__install init failed");
        return;
    }
    bMonoDebugAvailable = 1;
}

/* Function start: 0x403D60 */
void MonoDebug_remove(void)
{
    if (bMonoDebugAvailable != 0) {
        BOOL (WINAPI *closeDriver)(HANDLE handle) = CloseHandle;

        closeDriver(hMonoDebugDriver);
        bMonoDebugAvailable = 0;
    }
}

/* Function start: 0x403D80 */
/* Nothing in the shipped executable calls this. It sends control code 3 to
   MONODEBG.VXD (install uses 1 and 2, print uses 9); what code 3 asks the
   driver to do is unknown. */
void SendMonoDebugCommand3(void)
{
    if (bMonoDebugAvailable != 0) {
        BOOL (WINAPI *controlDevice)(HANDLE device, DWORD code,
                                    LPVOID input, DWORD inputBytes,
                                    LPVOID output, DWORD outputBytes,
                                    DWORD *returnedBytes,
#ifdef SDL_PORT
                                    LPVOID overlap) = DeviceIoControl;
#else
                                    LPOVERLAPPED overlap) = DeviceIoControl;
#endif

        if (!controlDevice(hMonoDebugDriver, 3, 0, 0, 0, 0, 0, 0))
            MonoDebug_remove();
    }
}

/* Function start: 0x403DB0 */
void SoundDebugPrintf(const char *fmt, ...)
{
#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, fmt);
    vsprintf(szSoundDebugMsg, fmt, arguments);
    va_end(arguments);
#else
    vsprintf(szSoundDebugMsg, fmt, (char *)(&fmt + 1));
#endif
    MonoDebug_print(szSoundDebugMsg);
}

/* Function start: 0x403DE0 */
void MonoDebug_print(const char *text)
{
    if (bMonoDebugAvailable != 0) {
        BOOL (WINAPI *controlDevice)(HANDLE device, DWORD code,
                                    LPVOID input, DWORD inputBytes,
                                    LPVOID output, DWORD outputBytes,
                                    DWORD *returnedBytes,
#ifdef SDL_PORT
                                    LPVOID overlap) = DeviceIoControl;
#else
                                    LPOVERLAPPED overlap) = DeviceIoControl;
#endif

        if (!controlDevice(hMonoDebugDriver, 9,
                           (void *)text, 0xfa0, 0, 0, 0, 0)) {
            exit_squadron("MonoDebug::print failed (buffer possibly on stack?!)");
            MonoDebug_remove();
        }
    }
}

/* Function start: 0x403E30 */
void ReadPerformanceCounter(LARGE_INTEGER *p)
{
    BOOL (WINAPI *readCounter)(LARGE_INTEGER *counter) = QueryPerformanceCounter;

    readCounter(p);
}

/* Function start: 0x403E40 */
void __stdcall ResetStringBuilder(TextContext *context)
{
    context->textCursor = context->text;
    *context->text = 0;
}
