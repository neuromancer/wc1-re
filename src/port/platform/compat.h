/* The old C runtime names the original game calls (compat.c). */
#ifndef SDL_PORT_COMPAT_H
#define SDL_PORT_COMPAT_H

#include "platform/win32.h"

#ifdef __cplusplus
extern "C" {
#endif

int SdlChangeDirectory(const char *path);
int SdlResolvePath(const char *path, char *resolved,
                      unsigned long resolvedSize);
#ifndef _WIN32
int SdlOpen(const char *path, int flags, ...);
long SdlFileLength(int file);
char *SdlItoa(int value, char *text, int radix);
char *SdlLtoa(long value, char *text, int radix);
char *SdlUltoa(unsigned long value, char *text, int radix);
char *SdlStrupr(char *text);

#endif
#ifndef _WIN32
#define _open SdlOpen
#define _close close
#define _read read
#define _write write
#define _lseek lseek
#define _filelength SdlFileLength
#define _unlink unlink
#define _chdir SdlChangeDirectory
#define _cprintf printf
#define _itoa SdlItoa
#define _ltoa SdlLtoa
#define _ultoa SdlUltoa
#define _strupr SdlStrupr
#endif

/* The game calls the CRT's old names, which the original executable resolved
 * through OLDNAMES.LIB; route them to the underscore spellings above. */
#define strupr _strupr
#define itoa _itoa
#define ltoa _ltoa
#define ultoa _ultoa
#define filelength _filelength
#define cprintf _cprintf
#define chdir _chdir

/* <dos.h>'s interrupt switches; the host has no interrupt flag to clear. */
#define _disable()
#define _enable()

#ifdef __cplusplus
}
#endif

#endif /* SDL_PORT_COMPAT_H */
