/*
 *  MONODEBG.VXD developer console and its printf channels.
 *
 *  Address range 0x403500-0x403fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: MonoDebug_install/MonoDebug_print anchor the module; SoundDebugPrintf feeds it.
 */
#include "wc1.h"

/* Function start: 0x403500 */
void CloseDataFile(unsigned int fd) { DAT_00465460 = (short)_close(fd & 0xffff); }

/* Function start: 0x403DB0 */
void SoundDebugPrintf(const char *fmt, ...)
{
    vsprintf(DAT_005a8760, fmt, (char *)(&fmt + 1));
    MonoDebug_print(DAT_005a8760);
}

/* Function start: 0x403E30 */
void ReadPerformanceCounter(LARGE_INTEGER *p) { QueryPerformanceCounter(p); }

/* Function start: 0x403E40 */
void ResetStringBuilder(int p)
{
    *(unsigned char **)(p + 0x12) = *(unsigned char **)(p + 0xe);
    **(unsigned char **)(p + 0xe) = 0;
}

/* Function start: 0x403EE0 */
unsigned int GetNavSystemId(short i)
{
    if (i != -1 && DAT_0059d400[i] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void SetNavSystemId(short i, short v)
{
    DAT_0059b320[i] = (int)v << 8;
    GetTbl0059b320FnE820(i);
}
