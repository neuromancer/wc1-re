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
void __stdcall CloseDataFile(unsigned int fd)
{
    DAT_00465460 = (short)_close(fd & 0xffff);
}

/* Function start: 0x403520 */
short __stdcall WriteDataFileAtOffset(unsigned int fd, int offset,
                                      unsigned int length, const void *data)
{
    DAT_00465460 = 0;
    fd &= 0xffff;
    if (_lseek(fd, offset, SEEK_SET) == -1) {
        sprintf(g_szWriteDataFileError_00475da0, "!lseek %d\n", offset);
        DAT_00465460 = (short)errno;
        return 0;
    }
    if (_write(fd, data, length) == -1) {
        sprintf(g_szWriteDataFileError_00475da0, "!write %d\n", offset);
        DAT_00465460 = (short)errno;
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
        sprintf(g_szCreateDataFileError_00475d60, "!_open '%s'\n", path);
        DAT_00465460 = (short)errno;
        fd = 0;
    }
    return (short)fd;
}

/* Function start: 0x403610 */
int __stdcall ReadDataFileAtOffset(unsigned int fd, int offset,
                                   unsigned int length, void *data)
{
    DAT_00465460 = 0;
    fd &= 0xffff;
    if (_lseek(fd, offset, SEEK_SET) == -1) {
        sprintf(g_szReadDataFileError_00475d20, "!lseek %d\n", offset);
        DAT_00465460 = (short)errno;
        return 0;
    }
    if (_read(fd, data, length) == -1) {
        sprintf(g_szReadDataFileError_00475d20, "!lseek %d\n", offset);
        DAT_00465460 = (short)errno;
        return 0;
    }
    return 1;
}

/* Function start: 0x4036B0 */
int __stdcall SeekDataFile(unsigned int fd, int offset,
                           unsigned int origin)
{
    int position;

    position = _lseek(fd & 0xffff, offset, origin & 0xffff);
    if (position == -1) {
        sprintf(g_szSeekDataFileError_00475de0, "!lseek %d\n", offset);
        DAT_00465460 = (short)errno;
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

            GetTransformedShapeBounds(&DAT_005a7510, 0, 0,
                                      g_pIntroFont_005a8960, (short)c, 0,
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

            DrawSpriteScaled(&DAT_005a7510, x, y,
                             g_pIntroFont_005a8960, (short)c, 0,
                             drawScale, 0);
            GetTransformedShapeBounds(&DAT_005a7510, 0, 0,
                                      g_pIntroFont_005a8960, (short)c, 0,
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
    short width = 0;

    while (*text != 0) {
        unsigned char c = (unsigned char)*text++;
        short frame;

        if (c == '\n')
            break;
        if (c == ' ') {
            width = width + 6;
            continue;
        }
        if (c == '.')
            frame = 58;
        else if (c == ',')
            frame = 59;
        else if (c >= 'A' && c <= 'z')
            frame = (short)(c - 'A');
        else
            continue;
        width = (short)(width + GetShapeFrameExtent(
            0, 0, g_pIntroFont_005a8960, frame, 2) + 2);
    }
    return width;
}

/* Function start: 0x403920 */
void print_subtitle(Viewport *viewport, short colour, const char *text)
{
    const char *scan;
    short lines = 1;
    short x;
    short y;

    (void)colour;
    scan = text;
    while (*scan != 0) {
        if (*scan == '\n')
            lines++;
        scan++;
    }
    y = (short)((128 - lines * 16) / 2);
    x = (short)((320 - GetLineLength(text)) / 2);
    while (*text != 0) {
        unsigned char c = (unsigned char)*text++;
        short frame;

        if (c == '\n') {
            y = y + 16;
            x = (short)((320 - GetLineLength(text)) / 2);
            continue;
        }
        if (c == ' ') {
            x = x + 6;
            continue;
        }
        if (c == '.')
            frame = 58;
        else if (c == ',')
            frame = 59;
        else if (c >= 'A' && c <= 'z')
            frame = (short)(c - 'A');
        else
            continue;
        DrawSpriteDefault(viewport, x, y, g_pIntroFont_005a8960, frame);
        x = (short)(x + GetShapeFrameExtent(
            0, 0, g_pIntroFont_005a8960, frame, 2) + 2);
    }
}

/* Function start: 0x403A80 */
int advance_canned_sequence(short obj)
{
    const short *command;

    command = g_apCannedSequence_0059dce0[obj];
    if (command == 0)
        return 0;
    g_asCannedCommand_0059d4e0[obj] = *command++;
    switch (g_asCannedCommand_0059d4e0[obj]) {
    case 0:
        g_asActionCount_0059c930[obj] = *command++;
        break;
    case 1:
        g_anYawGoal_0059c310[obj] = *command++;
        g_anPitchGoal_0059d7a0[obj] = *command++;
        g_anRollGoal_0059d630[obj] = *command++;
        g_anShipSpeed_0059b320[obj] = (int)*command++ << 8;
        break;
    case 2:
        explode(-1, obj);
        break;
    case 3:
        fire_fixed_projectile_weapon(obj);
        break;
    case 4:
        g_aeSpecialManeuver_0059c3c0[obj] =
            SPECIAL_MANEUVER_AFTERBURNER;
        break;
    }
    g_apCannedSequence_0059dce0[obj] = command;
    return 0;
}

/* Function start: 0x403B70 */
void update_canned_sequence(short obj)
{
    int velocity;
    int requested;

    switch (g_asCannedCommand_0059d4e0[obj]) {
    case 0:
        g_asActionCount_0059c930[obj]--;
        if (g_asActionCount_0059c930[obj] == 0)
            advance_canned_sequence(obj);
        break;
    case 1:
        if (g_anYawGoal_0059c310[obj] == 0 &&
            g_anPitchGoal_0059d7a0[obj] == 0 &&
            g_anRollGoal_0059d630[obj] == 0) {
            requested = g_anShipSpeed_0059b320[obj];
            velocity = (int)ComputeFixedVectorMagnitude(
                &g_aShipVelocity_0059c010[0]);
            if ((velocity > requested - 0x400) < requested + 0x400)
                advance_canned_sequence(obj);
        }
        break;
    case 3:
    case 4:
        advance_canned_sequence(obj);
        break;
    }
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

    g_hMonoDebugDevice_00475e74 =
        CreateFileA("\\\\.\\MONODEBG.VXD", 0, 0, 0, CREATE_ALWAYS,
                    FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (g_hMonoDebugDevice_00475e74 == INVALID_HANDLE_VALUE)
        return;

    if (!DeviceIoControl(g_hMonoDebugDevice_00475e74, 1, 0, 0,
                         &version, sizeof(version), 0, 0)) {
        CloseHandle(g_hMonoDebugDevice_00475e74);
        return;
    }
    if (version != 0x20004) {
        CloseHandle(g_hMonoDebugDevice_00475e74);
        exit_squadron("MonoDebug__install expecting version");
        return;
    }
    if (!DeviceIoControl(g_hMonoDebugDevice_00475e74, 2, 0, 0,
                         0, 0, 0, 0)) {
        CloseHandle(g_hMonoDebugDevice_00475e74);
        exit_squadron("MonoDebug__install init failed");
        return;
    }
    g_bMonoDebugInstalled_00475e70 = 1;
}

/* Function start: 0x403D60 */
void MonoDebug_remove(void)
{
    if (g_bMonoDebugInstalled_00475e70 != 0) {
        CloseHandle(g_hMonoDebugDevice_00475e74);
        g_bMonoDebugInstalled_00475e70 = 0;
    }
}

/* Function start: 0x403DB0 */
void SoundDebugPrintf(const char *fmt, ...)
{
    vsprintf(DAT_005a8760, fmt, (char *)(&fmt + 1));
    MonoDebug_print(DAT_005a8760);
}

/* Function start: 0x403DE0 */
void MonoDebug_print(const char *text)
{
    if (g_bMonoDebugInstalled_00475e70 != 0) {
        if (!DeviceIoControl(g_hMonoDebugDevice_00475e74, 9,
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
