/*
 *  16-bit DOS C string and memory shims.
 *
 *  Address range 0x435400-0x4355ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: all __stdcall with short-width arguments, each forwarding to one CRT routine.
 */
#include "wc1.h"

/* Function start: 0x435410 */
unsigned int DosFarPtrToNear(unsigned int v)
{
    return v;
}

/* Function start: 0x435420 */
unsigned int DosNearPtrToFar(unsigned int v)
{
    return v;
}

/* Function start: 0x435430 */
void DosStrrchr(char *s, short c)
{
    strrchr(s, (int)c);
}

/* Function start: 0x4354D0 */
short DosStrlen(const char *s)
{
    return (short)strlen(s);
}

/* Function start: 0x4354F0 */
void DosMemcpy(void *dst, const void *src, size_t n)
{
    memcpy(dst, src, n);
}

/* Function start: 0x435550 */
unsigned short GetFxDriverCount(void)
{
    return 0;
}

/* Function start: 0x435560 */
void InitExtendedMemoryStub(void)
{
}

/* Function start: 0x435570 */
unsigned int SetWaitCursorAndRun(unsigned int a, void (*fn)(void))
{
    (void)a;
    DAT_0059a850 = 1;
    if (fn != 0)
        (*fn)();
    return 0;
}

/* Function start: 0x435590 */
void ClearWaitCursorFlag(void)
{
    DAT_0059a850 = 0;
}

/* Function start: 0x4355A0 */
unsigned short GetExtendedMemoryFree(void)
{
    return 0;
}

/* Function start: 0x4355C0 */
void BeginScreenUpdateHook(void)
{
}

/* Function start: 0x4355D0 */
void SetScreenUpdateMode(unsigned int v)
{
    DAT_0059ab2c = v;
}
