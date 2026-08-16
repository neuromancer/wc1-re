/*
 *  Win32 data-file wrappers, scaled interstitial text, canned sequences,
 *  and the MONODEBG.VXD developer console.
 *
 *  Address range 0x403500-0x403e4f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MonoDebug_install/MonoDebug_print anchor the final block;
 *  the Mac auto unit starts at visit_the_cinema (0x403e50).
 */
#include "wc1.h"

/* Function start: 0x45A04A */
void __stdcall CloseDataFile(unsigned short fd)
{
    g_nPacketError_00465460 = (short)_close(fd & 0xffff);
}

/* Function start: 0x45A06C */
short __stdcall WriteDataFileAtOffset(unsigned short fd, int offset,
                                      unsigned int length, const void *data)
{
    g_nPacketError_00465460 = 0;
    if (_lseek(fd, offset, SEEK_SET) == -1) {
        sprintf(g_szWriteDataFileError_00475da0, "!lseek %d\n", offset);
        g_nPacketError_00465460 = (short)errno;
        return 0;
    }
    if (_write(fd, data, length) == -1) {
        sprintf(g_szWriteDataFileError_00475da0, "!write %d\n", offset);
        g_nPacketError_00465460 = (short)errno;
        return 0;
    }
    return 1;
}

/* Function start: 0x45A12D */
short __stdcall CreateDataFile(const char *path)
{
    unsigned short fd;

    fd = (unsigned short)_open(path, 0x8101, 0x180);
    if ((unsigned int)fd == (unsigned int)-1) {
        sprintf(g_szCreateDataFileError_00475d60, "!_open '%s'\n", path);
        g_nPacketError_00465460 = (short)errno;
        return 0;
    }
    return (short)fd;
}

/* Function start: 0x45A19B */
int __stdcall ReadDataFileAtOffset(unsigned short fd, int offset,
                                   unsigned int length, void *data)
{
    g_nPacketError_00465460 = 0;
    if (_lseek(fd, offset, SEEK_SET) == -1) {
        sprintf(g_szReadDataFileError_00475d20, "!lseek %d\n", offset);
        g_nPacketError_00465460 = (short)errno;
        return 0;
    }
    if (_read(fd, data, length) == -1) {
        sprintf(g_szReadDataFileError_00475d20, "!lseek %d\n", offset);
        g_nPacketError_00465460 = (short)errno;
        return 0;
    }
    return 1;
}

/* Function start: 0x45A259 */
int __stdcall SeekDataFile(unsigned short fd, int offset,
                           unsigned int origin)
{
    int position;

    position = _lseek(fd, offset, origin & 0xffff);
    if (position == -1) {
        sprintf(g_szSeekDataFileError_00475de0, "!lseek %d\n", offset);
        g_nPacketError_00465460 = (short)errno;
    }
    return position;
}

/* Function start: 0x41687F */
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

/* Function start: 0x4087FF */
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

/* Function start: 0x42C0A2 */
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
                0, 0, g_pIntroFont_005a8960, c, 2);
            width = width + 2;
        } else if (c == '.') {
            width = width + GetShapeFrameExtent(
                0, 0, g_pIntroFont_005a8960, 58, 2);
            width = width + 2;
        } else if (c == ',') {
            width = width + GetShapeFrameExtent(
                0, 0, g_pIntroFont_005a8960, 59, 2);
            width = width + 2;
        } else if (c == ' ') {
            width = width + 6;
        } else if (c == '\n') {
            break;
        }
    }
    return width;
}

/* Function start: 0x42C0F1 */
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
            DrawSpriteDefault(viewport, x, y, g_pIntroFont_005a8960, c);
            x = x + GetShapeFrameExtent(
                0, 0, g_pIntroFont_005a8960, c, 2);
            x = x + 2;
        } else if (c == ' ') {
            x = x + 6;
        } else if (c == '.') {
            DrawSpriteDefault(viewport, x, y, g_pIntroFont_005a8960, 58);
            x = x + GetShapeFrameExtent(
                0, 0, g_pIntroFont_005a8960, 58, 2);
            x = x + 2;
        } else if (c == ',') {
            DrawSpriteDefault(viewport, x, y, g_pIntroFont_005a8960, 59);
            x = x + GetShapeFrameExtent(
                0, 0, g_pIntroFont_005a8960, 59, 2);
            x = x + 2;
        } else if (c == '\n') {
            y = y + 16;
            x = (short)((320 - GetLineLength(text)) >> 1);
        }
    }
    if (viewport->pixels == DAT_005a6ba0.pixels)
        DIBslam();
    return 0;
}

/* Function start: 0x4207B0 */
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

/* Function start: 0x438711 */
unsigned int update_canned_sequence(short obj)
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
            velocity = Vector_magnitude(
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
    return 0;
}

/* Function start: 0x437760 */
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

/* Function start: 0x4377F0 */
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

/* Function start: 0x4378D9 */
void MonoDebug_remove(void)
{
    if (g_bMonoDebugInstalled_00475e70 != 0) {
        CloseHandle(g_hMonoDebugDevice_00475e74);
        g_bMonoDebugInstalled_00475e70 = 0;
    }
}

/* Function start: 0x42F0B3 */
void SoundDebugPrintf(const char *fmt, ...)
{
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, fmt);
    vsprintf(DAT_005a8760, fmt, arguments);
    va_end(arguments);
#else
    vsprintf(DAT_005a8760, fmt, (char *)(&fmt + 1));
#endif
    MonoDebug_print(DAT_005a8760);
}

/* Function start: 0x437983 */
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

/* Function start: 0x4379D4 */
void ReadPerformanceCounter(LARGE_INTEGER *p)
{
    QueryPerformanceCounter(p);
}

/* Function start: 0x40A27A */
void __stdcall ResetStringBuilder(TextContext *context)
{
    context->textCursor = context->text;
    *context->text = 0;
}
