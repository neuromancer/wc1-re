/*
 *  16-bit DOS C string and memory shims.
 *
 *  Address range 0x435400-0x4355ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: all __stdcall with short-width arguments, each forwarding to one CRT routine.
 */
#include "wc1.h"

#pragma function(strlen)

/* Function start: WC2_UNMAPPED */
unsigned int __stdcall DosFarPtrToNear(void *v)
{
    return (unsigned int)v;
}

/* Function start: 0x462452 */
void *DosNearPtrToFar(unsigned int v)
{
    return (void *)v;
}

/* Function start: 0x462465 */
char *DosStrrchr(char *s, short c)
{
    return strrchr(s, (int)c);
}

/* Function start: 0x462486 */
char *DosStrchr(const char *s, short c)
{
    return strchr(s, (int)c);
}

/* Function start: WC2_UNMAPPED */
char *__stdcall DosStrcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
}

/* Function start: WC2_UNMAPPED */
char *__stdcall CopyFarString(char *destination, const char *source)
{
    return strcpy(destination, source);
}

/* Function start: 0x46250B */
short DosStrlen(const char *s)
{
    return (short)strlen(s);
}

/* Function start: WC2_UNMAPPED */
void __stdcall DosMemcpy(void *dst, const void *src, size_t n)
{
    memmove(dst, src, n);
}

/* Function start: WC2_UNMAPPED */
void __stdcall DosMemset(void *destination, unsigned int count, short value)
{
    memset(destination, value, (unsigned short)count);
}

/* Function start: WC2_UNMAPPED */
unsigned short GetEventManagerStatus(void)
{
    return 0;
}

/* Function start: WC2_UNMAPPED */
void __stdcall RegisterEventManagerShutdown(void (*fn)(void))
{
}

/* Function start: WC2_UNMAPPED */
short __stdcall InitializeEventManager(short period,
                                       unsigned short (*initialize)(void),
                                       void *configuration)
{
    g_nEventManagerActive_0059a850 = 1;
    if (initialize != 0)
        initialize();
    return 1;
}

/* Function start: WC2_UNMAPPED */
void ShutdownEventManager(void)
{
    g_nEventManagerActive_0059a850 = 0;
}

/* Function start: WC2_UNMAPPED */
unsigned short __stdcall ConfigureEventManagerPointer(
    unsigned char *shape, short frame)
{
    return 0;
}

/* Function start: WC2_UNMAPPED */
void __stdcall EventManagerHook(void (*callback)(void))
{
    (void)callback;
}

/* Function start: WC2_UNMAPPED */
void __stdcall SetEventManagerPump(void (*pump)(void))
{
    DAT_0059ab2c = pump;
}
