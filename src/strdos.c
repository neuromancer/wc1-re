/**
 * @file
 * @ingroup data
 * @brief 16-bit DOS C string and memory shims, and the event manager's set-up
 * entries.
 *
 * Address range 435410-4355ef. The shims are all __stdcall with short-width
 * arguments, each forwarding to one CRT routine.
 */
#include "wc1.h"

/* Linker commons defined here; LINK orders them by identifier hash. */
signed char cScreenViewMode; /* 0x0059a9f2 */
const ScreenViewportGeometry *pScreenViewGeometry; /* 0x0059a9f4 */
int anSortedObjects[SPACE_OBJECT_COUNT]; /* 0x0059aa00 */
short nPacketDecompressInputPositionValue; /* 0x0059ab00 */
void *pPktDecompInput; /* 0x0059ab04 */
unsigned short wCurrentInputModifiers; /* 0x0059ab08 */
MouseCursorState stMouseCursorState; /* 0x0059ab10 */
void (*pCurrentEventManagerPump)(void); /* 0x0059ab2c */
int nPacketDecompressResult; /* 0x0059ab30 */
short bCurrentRunningFromHardDiskFlag; /* 0x0059ab34 */

/* Function start: 0x435410 */
unsigned int __stdcall DosFarPtrToNear(void *v)
{
    return (unsigned int)v;
}

/* Function start: 0x435420 */
void *__stdcall DosNearPtrToFar(unsigned int v)
{
    return (void *)v;
}

/* Function start: 0x435430 */
char *__stdcall DosStrrchr(char *s, short c)
{
    return strrchr(s, c);
}

/* Function start: 0x435450 */
char *__stdcall DosStrchr(const char *s, short c)
{
    return strchr(s, c);
}

/* Function start: 0x435470 */
char *__stdcall DosStrcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
}

/* Function start: 0x4354A0 */
char *__stdcall CopyFarString(char *destination, const char *source)
{
    return strcpy(destination, source);
}

/* Function start: 0x4354D0 */
short __stdcall DosStrlen(const char *s)
{
    return strlen(s);
}

/* Function start: 0x4354F0 */
void __stdcall DosMemcpy(void *dst, const void *src, size_t n)
{
    memmove(dst, src, n);
}

/* Function start: 0x435510 */
void __stdcall DosMemset(void *destination, unsigned int count, short value)
{
    memset(destination, value, (unsigned short)count);
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
                                       unsigned short (*initialize)(void),
                                       void *configuration)
{
    nEventManagerActive = 1;
    if (initialize != 0)
        initialize();
    return 1;
}

/* Function start: 0x435590 */
void ShutdownEventManager(void)
{
    nEventManagerActive = 0;
}

/* Function start: 0x4355A0 */
unsigned short __stdcall ConfigureEventManagerPointer(
    struct Shape *shape, short frame)
{
    return 0;
}

/* Function start: 0x4355B0 */
/* Empty in the Win32 build; the hidden kill-board code at 0x4404B0 calls it. */
void ReleaseEventManagerPointerHook(void)
{
}

/* Function start: 0x4355C0 */
void __stdcall EventManagerHook(void (*callback)(void))
{
    (void)callback;
}

/* Function start: 0x4355D0 */
void __stdcall SetEventManagerPump(void (*pump)(void))
{
    pCurrentEventManagerPump = pump;
}

/* Function start: 0x4355E0 */
/* Empty, and nothing in the shipped executable calls it, so its purpose
 * cannot be recovered; it closes the event-manager control entries. */
void EventManagerControlHook(void)
{
}
