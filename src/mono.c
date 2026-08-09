/*
 *  MONODEBG.VXD developer console and its printf channels.
 *
 *  Address range 0x403500-0x403fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MonoDebug_install/MonoDebug_print anchor the module; SoundDebugPrintf feeds it.
 */
#include "wc1.h"

/* Function start: 0x403500 */
void CloseDataFile(unsigned int fd)
{
    DAT_00465460 = (short)_close(fd & 0xffff);
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
void ResetStringBuilder(int p)
{
    *(unsigned char **)(p + 0x12) = *(unsigned char **)(p + 0xe);
    **(unsigned char **)(p + 0xe) = 0;
}

/* Function start: 0x403EE0 */
unsigned int GetNavSystemId(short i)
{
    if (i != -1 && g_asShipWingLeader_0059d400[i] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void SetNavSystemId(short i, short v)
{
    g_anShipSpeed_0059b320[i] = (int)v << 8;
    fix_velocity(i);
}
