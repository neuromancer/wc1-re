/*
 *  Seventh leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned char  DAT_0059c3f0[512];
short          DAT_0059ce10;
short          DAT_0059d52a;
unsigned char  DAT_005a6ba0;
short          DAT_0046c04c;

extern unsigned char DAT_0059d100[];
extern unsigned char DAT_005a7c30[];
extern unsigned char DAT_005a76b0;

extern void ShipAiRoutine07(short a, int b);
extern void GetG0046b1a4Fn4FB0(void);
extern void DoLocalFn1A90(void *a, void *b);           /* 0x00421A90 */
extern void ShowMemoryStatusDebug(void);               /* 0x004273C0 */
extern void DoLocalFn8F20(const char *s);              /* 0x00428F20 */
extern void HelperOf428F80A(const char *s);            /* 0x00428FA0-family */
extern void ShowOnScreenMessage(short f, short d, const char *fmt, ...);
extern void CombatRoutine03(int a, short b);           /* 0x00430C00-family */
extern void exit_squadron(const char *s);              /* 0x00427370 */
extern long _ftol(void);

/* Pointer table of prompt strings indexed by HelperOf430FC0D. */
extern char *PTR_s_Never_mind____0046af90[];

/* Function start: 0x422DF0 */
unsigned int DoLocalFn2DF0(short ship) { ShipAiRoutine07(ship, 0x500); return 0; }

/* Function start: 0x422F60 */
unsigned int ShipAiRoutine24(short ship)
{
    if (DAT_0059ce10 < ship && DAT_0059d52a < -0x32)
        return 1;
    return 0;
}

/* Function start: 0x425FE0 */
int GetTbl005a7c30Fn5FE0(short i) { return DAT_005a7c30[i * 5] != 0xff; }

/* Function start: 0x427C30 */
unsigned int GetUiFn7C30(void)
{
    GetG0046b1a4Fn4FB0();
    DoLocalFn1A90(&DAT_005a76b0, &DAT_005a6ba0);
    ShowMemoryStatusDebug();
    return 0;
}

/* Function start: 0x428F80 */
void ReportOutOfMemoryAndExit(void)
{
    DoLocalFn8F20("ERROR: Out of memory for %s");
    HelperOf428F80A("You do not have enough memory to run Wing Commander.");
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, "WING COMMANDER VER. %s");
}

/* Function start: 0x42F1F0 */
int GetTbl0059c3f0FnF1F0(short i) { return DAT_0059c3f0[i] == 5; }

/* Function start: 0x430D30 */
void HelperOf430FC0D(short i)
{
    CombatRoutine03((int)PTR_s_Never_mind____0046af90[i], i);
}

/* Function start: 0x430E10 */
int GetShipAiFn0E10(void) { return DAT_0046c04c == -1; }

/* Function start: 0x434CD0 */
unsigned short DoLocalFn4CD0(short n) { return (short)(rand() % (int)n); }

/* Function start: 0x434CF0 */
void DoLocalFn4CF0(void) { srand((unsigned int)time(0)); }

/* Function start: 0x434EC0 */
long CalcVideoFn4EC0(void) { return _ftol(); }

/* Function start: 0x435430 */
void ForwardStrrchr(char *s, short c) { strrchr(s, (int)c); }

/* Function start: 0x4408A0 */
void DoLocalFn08A0(int p)
{
    if (*(int *)(p - 8) != 0x6666656a)
        exit_squadron("not jefftep");
}

/* Function start: 0x4408C0 */
unsigned int WcOrphanRoutine4408C0(int p) { return *(unsigned int *)(p - 4); }

/* Function start: 0x440BE0 */
unsigned int HelperOf440C00A(unsigned short v)
{
    if (v < 0xfdc0)
        return v;
    return (int)(short)v;
}
