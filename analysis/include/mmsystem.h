#ifndef TEACHER_ANALYSIS_MMSYSTEM_H
#define TEACHER_ANALYSIS_MMSYSTEM_H

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT MMRESULT;
typedef UINT MCIDEVICEID;

typedef struct joyinfo_tag {
    UINT wXpos;
    UINT wYpos;
    UINT wZpos;
    UINT wButtons;
} JOYINFO;

typedef struct joycapsa_tag {
    WORD wMid;
    WORD wPid;
    char szPname[32];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
} JOYCAPSA;

typedef JOYCAPSA JOYCAPS;

unsigned long __stdcall timeGetTime(void);
UINT joyGetNumDevs(void);
MMRESULT joyGetPos(UINT, JOYINFO*);
MMRESULT joyGetDevCapsA(UINT, JOYCAPSA*, UINT);
UINT GetDoubleClickTime(void);

#ifdef __cplusplus
}
#endif

#endif
