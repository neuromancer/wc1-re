/*
 *  Win32 data-file wrappers, scaled interstitial text, canned sequences,
 *  and the MONODEBG.VXD developer console.
 *
 *  Address range 0x403500-0x403e4f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MonoDebug_install/MonoDebug_print anchor the final block;
 *  the Mac auto unit starts at visit_the_cinema (0x403e50).
 */
#include "wc1.h"

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
        sprintf(szWriteDataFileError, "!lseek %d\n", offset);
        nPacketError = (short)errno;
        return 0;
    }
    if (_write(fd, data, length) == -1) {
        sprintf(szWriteDataFileError, "!write %d\n", offset);
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
        sprintf(szCreateDataFileError, "!_open '%s'\n", path);
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
        sprintf(szReadDataFileError, "!lseek %d\n", offset);
        nPacketError = (short)errno;
        return 0;
    }
    if (_read(fd, data, length) == -1) {
        sprintf(szReadDataFileError, "!lseek %d\n", offset);
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
        sprintf(szSeekDataFileError, "!lseek %d\n", offset);
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

            GetTransformedShapeBounds(&stSpaceBuffer, 0, 0,
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
    short x = centreX;
    short y;
    short drawScale = scale;
    int scaled = drawScale;

    x = (short)(x - MeasureScaledIntroTextWidth(text, drawScale) / 2);
    y = (short)(baselineY - (scaled * 16 >> 9));
    for (;;) {
        char c = *text++;

        if (c == 0)
            break;

        if (c >= 'A' && c <= 'z') {
            c -= 'A';

            DrawSpriteScaled(&stSpaceBuffer, x, y,
                             pIntroFont, (short)c, 0,
                             drawScale, 0);
            GetTransformedShapeBounds(&stSpaceBuffer, 0, 0,
                                      pIntroFont, (short)c, 0,
                                      drawScale, 0, bounds);
            x = (short)(x + bounds[2] + 1);
            x = (short)(x + (scaled * 2 >> 8));
        } else if (c == ' ') {
            x = (short)(x + (scaled * 6 >> 8));
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

/* Function start: 0x403920 */
int print_subtitle(Viewport *viewport, short colour, const char *text)
{
    const char *scan;
    short lines;
    short x;
    short y;
    char c;

    (void)colour;
    lines = 1;
    scan = text;
    c = *scan;
    scan++;
    while (c != 0) {
        if (c == '\n')
            lines++;
        c = *scan;
        scan++;
    }
    lines = (short)(lines * 16);
    y = (short)((128 - lines) / 2);
    x = (short)((320 - GetLineLength(text)) >> 1);
    for (;;) {
        c = *text;
        text++;
        if (c == 0)
            break;
        if (c >= 'A' && c <= 'z') {
            c = (char)(c - 'A');
            DrawSpriteDefault(viewport, x, y, pIntroFont, c);
            x = x + GetShapeFrameExtent(
                0, 0, pIntroFont, c, 2);
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
            x = (short)((320 - GetLineLength(text)) >> 1);
        }
    }
    if (viewport->pixels == stScreen.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x403A80 */
int advance_canned_sequence(short obj)
{
    const short *command;

    command = apCannedSequence[obj];
    if (command == 0)
        return 0;
    asCannedCommand[obj] = *command++;
    switch (asCannedCommand[obj]) {
    case 0:
        asActionCount[obj] = *command++;
        break;
    case 1:
        anYawGoal[obj] = *command++;
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
        aeSpecialManeuver[obj] =
            SPECIAL_MANEUVER_AFTERBURNER;
        break;
    }
    apCannedSequence[obj] = command;
    return 0;
}

/* Function start: 0x403B70 */
unsigned int update_canned_sequence(short obj)
{
    int velocity;
    int requested;

    switch (asCannedCommand[obj]) {
    case 0:
        asActionCount[obj]--;
        if (asActionCount[obj] == 0)
            advance_canned_sequence(obj);
        break;
    case 1:
        if (anYawGoal[obj] == 0 &&
            anPitchGoal[obj] == 0 &&
            anRollGoal[obj] == 0) {
            requested = anShipSpeed[obj];
            velocity = Vector_magnitude(
                &aShipVelocity[0]);
            if ((velocity > requested - 0x400) < requested + 0x400)
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

    hMonoDebugDevice =
        CreateFileA("\\\\.\\MONODEBG.VXD", 0, 0, 0, CREATE_ALWAYS,
                    FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (hMonoDebugDevice == INVALID_HANDLE_VALUE)
        return;

    if (!DeviceIoControl(hMonoDebugDevice, 1, 0, 0,
                         &version, sizeof(version), 0, 0)) {
        CloseHandle(hMonoDebugDevice);
        return;
    }
    if (version != 0x20004) {
        CloseHandle(hMonoDebugDevice);
        exit_squadron("MonoDebug__install expecting version");
        return;
    }
    if (!DeviceIoControl(hMonoDebugDevice, 2, 0, 0,
                         0, 0, 0, 0)) {
        CloseHandle(hMonoDebugDevice);
        exit_squadron("MonoDebug__install init failed");
        return;
    }
    bMonoDebugInstalled = 1;
}

/* Function start: 0x403D60 */
void MonoDebug_remove(void)
{
    if (bMonoDebugInstalled != 0) {
        CloseHandle(hMonoDebugDevice);
        bMonoDebugInstalled = 0;
    }
}

/* Function start: 0x403DB0 */
void SoundDebugPrintf(const char *fmt, ...)
{
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, fmt);
    vsprintf(szSoundDebugMessage, fmt, arguments);
    va_end(arguments);
#else
    vsprintf(szSoundDebugMessage, fmt, (char *)(&fmt + 1));
#endif
    MonoDebug_print(szSoundDebugMessage);
}

/* Function start: 0x403DE0 */
void MonoDebug_print(const char *text)
{
    if (bMonoDebugInstalled != 0) {
        if (!DeviceIoControl(hMonoDebugDevice, 9,
                             (void *)text, 0xfa0, 0, 0, 0, 0)) {
            exit_squadron("MonoDebug::print failed (buffer possibly on stack?!)");
            MonoDebug_remove();
        }
    }
}

/* Function start: 0x403E30 */
void ReadPerformanceCounter(LARGE_INTEGER *p)
{
    QueryPerformanceCounter(p);
}

/* Function start: 0x403E40 */
void __stdcall ResetStringBuilder(TextContext *context)
{
    context->textCursor = context->text;
    *context->text = 0;
}
