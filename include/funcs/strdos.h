/* strdos.c */
unsigned int __stdcall DosFarPtrToNear(void *v);                       /* 0x00435410 */
void *__stdcall DosNearPtrToFar(unsigned int v);                       /* 0x00435420 */
char *__stdcall DosStrrchr(char *s, short c);                       /* 0x00435430 */
char *__stdcall DosStrchr(const char *s, short c);                  /* 0x00435450 */
char *__stdcall DosStrcpy(char *dst, const char *src);               /* 0x00435470 */
char *__stdcall CopyFarString(char *destination,
                              const char *source);                   /* 0x004354A0 */
short __stdcall DosStrlen(const char *s);                                   /* 0x004354D0 */
/* The DOS-era prototype keeps a word copy count (the barracks caller computes
 * it in CX). The Win32 definition takes size_t and forwards the complete
 * dword stack slot to memmove, so CL 4.20 reports C4028 for it. */
#ifdef SDL_PORT
void __stdcall DosMemcpy(void *dst, const void *src, size_t n);
#else
void __stdcall DosMemcpy(void *dst, const void *src, unsigned short n); /* 0x004354F0 */
#endif
void __stdcall DosMemset(void *destination, unsigned int count,
                         short value);                               /* 0x00435510 */
unsigned short GetEventManagerStatus(void);                            /* 0x00435550 */
void __stdcall RegisterEventManagerShutdown(void (*fn)(void));         /* 0x00435560 */
short __stdcall InitializeEventManager(short period,
                                       unsigned short (*initialize)(void),
                                       void *configuration);           /* 0x00435570 */
void ShutdownEventManager(void);                                      /* 0x00435590 */
unsigned short __stdcall ConfigureEventManagerPointer(
    struct Shape *shape, short frame);                               /* 0x004355A0 */
void __stdcall EventManagerHook(void (*callback)(void));               /* 0x004355C0 */
void __stdcall SetEventManagerPump(void (*pump)(void));                /* 0x004355D0 */
void EventManagerControlHook(void);                                    /* 0x004355E0 */
