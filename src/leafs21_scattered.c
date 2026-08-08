/*
 *  Twenty-first leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"
#include <io.h>
#include <fcntl.h>
#include <errno.h>

extern short         DAT_00465460;
extern short         DAT_0059c330[];
extern unsigned char DAT_0059d630[];
extern unsigned char DAT_005a76b0;

extern void ShipAiState29(short a, short b);
extern void DoLocalFn6220(int p, unsigned short a, unsigned short b,
                          unsigned short c, unsigned short d);

/* Function start: 0x4034D0 */
short DoUiFn34D0(const char *path)
{
    int fd = _open(path, 0x8002);

    DAT_00465460 = (short)errno;
    return (short)fd;
}

/* Function start: 0x4070D0 */
void ShipAiState31(short ship, short target)
{
    if (0 < DAT_0059c330[ship]) {
        DAT_0059d630[ship] = 0x2d;
        return;
    }
    ShipAiState29(ship, target);
}

/* Function start: 0x40D8C0 */
void DoUiFnD8C0(unsigned short a, unsigned short b, unsigned short c, unsigned short d)
{
    DoLocalFn6220((int)&DAT_005a76b0, a, b, c, d);
}

/* Function start: 0x439400 */
void DebugNoBodyStubv1(int p, unsigned short a, unsigned short b,
                       unsigned short c, unsigned short d)
{
    *(unsigned short *)(p + 8) = a;
    *(unsigned short *)(p + 10) = b;
    *(unsigned short *)(p + 0xc) = c;
    *(unsigned short *)(p + 0xe) = d;
}
