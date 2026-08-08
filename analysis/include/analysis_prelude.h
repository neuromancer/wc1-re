#ifndef TEACHER_ANALYSIS_PRELUDE_H
#define TEACHER_ANALYSIS_PRELUDE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include "mbctype.h"
#include "mbstring.h"

#ifndef __cdecl
#define __cdecl
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __fastcall
#define __fastcall
#endif

#ifndef ZeroMemory
#define ZeroMemory(ptr, len) memset((ptr), 0, (len))
#endif

#ifndef _stricmp
#define _stricmp strcasecmp
#endif
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strcmpi
#define strcmpi strcasecmp
#endif
#ifndef _strcmpi
#define _strcmpi strcasecmp
#endif
#ifndef _strnicmp
#define _strnicmp strncasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif

#ifndef filelength
#define filelength _filelength
#endif

#if defined(__clang__)
#define WC1_ANALYSIS_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#define WC1_ANALYSIS_NORETURN __attribute__((noreturn))
#else
#define WC1_ANALYSIS_PRINTF(fmt, args)
#define WC1_ANALYSIS_NORETURN
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* WC1 variadic functions the analyzer needs format-checking for. */
void ShowOnScreenMessage(short flags, short duration, const char *fmt, ...)
    WC1_ANALYSIS_PRINTF(3, 4);          /* 0x00428FA0 */
void ix_log_printf(const char *fmt, ...)
    WC1_ANALYSIS_PRINTF(1, 2);          /* 0x004426A0 */
void WriteDebugString(const char *s);   /* 0x004030D0, OutputDebugStringA wrapper */
void DIBerror(const char *tag, long hr);/* 0x00432140 */

#ifdef __cplusplus
}
#endif

#endif
