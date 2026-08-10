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
char *__stdcall DosStrrchr(char *s, short c)
{
    return strrchr(s, (int)c);
}

/* Function start: 0x435450 */
char *__stdcall DosStrchr(char *s, short c)
{
    return strchr(s, (int)c);
}

/* Function start: 0x435470 */
char *__stdcall DosStrcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
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
unsigned short GetEventManagerStatus(void)
{
    return 0;
}

/* Function start: 0x435560 */
void __stdcall RegisterEventManagerShutdown(void (*fn)(void))
{
}

/* Function start: 0x435570 */
short __stdcall InitializeEventManager(short period,
                                       void (*initialize)(void),
                                       void *configuration)
{
    g_nEventManagerActive_0059a850 = 1;
    if (initialize != 0)
        initialize();
    return 1;
}

/* Function start: 0x435590 */
void ShutdownEventManager(void)
{
    g_nEventManagerActive_0059a850 = 0;
}

/* Function start: 0x4355A0 */
unsigned short __stdcall ConfigureEventManagerPointer(
    unsigned char *shape, short frame)
{
    return 0;
}

/* Function start: 0x4355C0 */
void __stdcall EventManagerHook(void (*callback)(void))
{
    (void)callback;
}

/* Function start: 0x4355D0 */
void __stdcall SetEventManagerPump(void (*pump)(void))
{
    DAT_0059ab2c = pump;
}
