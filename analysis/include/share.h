#ifndef TEACHER_ANALYSIS_SHARE_H
#define TEACHER_ANALYSIS_SHARE_H

#define _SH_DENYNO 0x40
#define _SH_DENYWR 0x20
#define _SH_DENYRD 0x30
#define _SH_DENYRW 0x10

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

FILE* _fsopen(const char*, const char*, int);

#ifdef __cplusplus
}
#endif

#endif
