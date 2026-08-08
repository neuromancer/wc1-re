/*
 *  Leaf functions: empty bodies, single-global writes, tiny getters.
 *
 *  NOT A REAL COMPILATION UNIT -- same caveat as accessors_scattered.c.  These
 *  span 0x004060A0 to 0x00435590 and belong to many different original object
 *  files; redistribute once `make order` establishes boundaries.
 *
 *  Several have genuinely empty bodies in the original.  That is not a stub on
 *  our side: the shipped code really does `ret` immediately, most likely from a
 *  compiled-out debug macro.
 */
#include "wc1.h"

unsigned short DAT_00475e78;
unsigned char  DAT_0046c080;
unsigned char  DAT_00468710;
unsigned char  DAT_0046870c;
char           DAT_0059dec0[256];
unsigned char  DAT_005a7dca;
unsigned char  DAT_0059ca94[256];
unsigned int   DAT_004763f0;
unsigned char  DAT_0046964c;
unsigned char  DAT_0046aa34;
unsigned char  DAT_00486518;
unsigned char  DAT_0059a850;

extern unsigned char DAT_0046af6c;

extern void PumpMessagesWhileDebugPaused(unsigned int arg);   /* 0x0041CD40 */
extern void SetShipAiFnEE80(void);                            /* 0x0042EE80 */
extern void GetSfxFnB640(void);                               /* 0x0042B640 */
extern void GetG005a8760Fn3DB0(const char *fmt, ...);         /* 0x00403DB0 */

/* Function start: 0x4060A0 */
void HelperOf4075D0A(unsigned short v) { DAT_00475e78 = v; }

/* Function start: 0x4075B0 */
/* Empty in the original: dispatch-table slots 0 and 1 (no-op / invalid state). */
void ShipAiState00(void) {}

/* Function start: 0x409CE0 */
void DoUiFn9CE0(void) { DAT_0046c080 = 0; }

/* Function start: 0x40D1D0 */
void DoG00468710FnD1D0(void) { DAT_00468710 = 0; }

/* Function start: 0x40D240 */
void DoNavFnD240(void) { DAT_0046870c = 0; }

/* Function start: 0x414890 */
int GetTbl0059dec0Fn4890(short i) { return (char)DAT_0059dec0[i]; }

/* Function start: 0x4149E0 */
void DoCombatFn49E0(void) { DAT_005a7dca = 0; }

/* Function start: 0x415040 */
void GameNoBodyStubv2(void) {}

/* Function start: 0x422050 */
void GetTbl0059ca94Fn2050(short i, unsigned char bits) { DAT_0059ca94[i] |= bits; }

/* Function start: 0x425AF0 */
void GameNoBodyStubv3(void) {}

/* Function start: 0x425BB0 */
/* TODO: takes a string (see exit_squadron call sites); body not yet recovered. */
void DoLocalFn5BB0(const char *s) { (void)s; }

/* Function start: 0x425BC0 */
void GetG004763f0Fn5BC0(void) { PumpMessagesWhileDebugPaused(DAT_004763f0); }

/* Function start: 0x425BD0 */
unsigned char GetDebugFn5BD0(void)
{
    unsigned char v = DAT_0046964c;

    DAT_0046964c = 0;
    return v;
}

/* Function start: 0x425C10 */
void HelperOf41D760B(void) {}

/* Function start: 0x42EEA0 */
void DoSceneFnEEA0(void) { SetShipAiFnEE80(); DAT_0046aa34 = 0; }

/* Function start: 0x42EF10 */
void DoLocalFnEF10(void)
{
    GetSfxFnB640();
    GetG005a8760Fn3DB0("FlushSoundEffects");
}

/* Function start: 0x42F930 */
void GameFlowVoidStub(void) {}

/* Function start: 0x42F950 */
void GameNoBodyStubv10(void) {}

/* Function start: 0x430DA0 */
short GetCombatFn0DA0(void) { return (short)DAT_0046af6c; }

/* Function start: 0x432960 */
void DoVideoFn2960(void) { DAT_00486518 = 1; }

/* Function start: 0x435560 */
void HelperOf421AB0A(void) {}

/* Function start: 0x435590 */
void DoUiFn5590(void) { DAT_0059a850 = 0; }

/* Function start: 0x4355C0 */
void SceneNoBodyStub(void) {}

/* ------------------------------------------------------------------------
 * Second tranche.  Same "not a real compilation unit" caveat applies.
 * ------------------------------------------------------------------------ */

unsigned short DAT_0046c084;
unsigned int   DAT_0059af8c;
short          DAT_0046daa0;
unsigned char  DAT_005a7c31[2048];

extern void GameNoBodyStubv12(void);
extern unsigned short ReturnConst1v5(void);

/* Function start: 0x403E40 */
void UiNoBodyStub(int p)
{
    *(unsigned char **)(p + 0x12) = *(unsigned char **)(p + 0xe);
    **(unsigned char **)(p + 0xe) = 0;
}

/* Function start: 0x409CF0 */
void SetUiFn9CF0(void) { DAT_0046c080 = 1; DAT_0046c084 = 0xffff; }

/* Function start: 0x414A10 */
unsigned short DoCombatFn4A10(unsigned short v) { DAT_005a7dca = (unsigned char)v; return 0; }

/* Function start: 0x416250 */
short DoLocalFn6250(int p) { return *(short *)(p + 0xc) - *(short *)(p + 8); }

/* Function start: 0x425E20 */
unsigned int GetUiFn5E20(short i) { return *(unsigned int *)(&DAT_005a7c31[0] + i * 5); }

/* Function start: 0x42C410 */
void GameNoBodyStubv7(void) {}

/* Function start: 0x42E390 */
void GameNoBodyStubv8(void) {}

/* Function start: 0x42E3E0 */
void GameNoBodyStubv9(void) {}

/* Function start: 0x42F940 */
unsigned short DoLocalFnF940(void) { ReturnConst1v5(); return 1; }

/* Function start: 0x4318F0 */
void DoLocalFn18F0(void) { GameNoBodyStubv12(); }

/* Function start: 0x434FA0 */
void UiRoutine08(unsigned int v) { DAT_0059af8c = v; }

/* Function start: 0x434FC0 */
unsigned int DoLocalFn4FC0(unsigned int v) { return v; }

/* Function start: 0x434FE0 */
unsigned short DoLocalFn4FE0(unsigned short *p) { return *p; }

/* Function start: 0x435010 */
void GameNoBodyStubv12(void) {}

/* Function start: 0x435410 */
unsigned int DoLocalFn5410(unsigned int v) { return v; }

/* Function start: 0x435420 */
unsigned int DoLocalFn5420(unsigned int v) { return v; }

/* Function start: 0x4360D0 */
void GetUiFn60D0(void) { DAT_0046daa0 = DAT_0046daa0 + 1; }

/* Function start: 0x4360E0 */
void GetUiFn60E0(void) { DAT_0046daa0 = DAT_0046daa0 - 1; }

/* Function start: 0x4362A0 */
void GameNoBodyStubv16(void) {}

/* Function start: 0x4362C0 */
unsigned short DoLocalFn62C0(unsigned short v) { return v; }

/* Function start: 0x4362D0 */
void GameNoBodyStubv17(void) {}

/* Function start: 0x4368F0 */
unsigned short HelperOf4273C0A(void) { return 0x8000; }

/* Function start: 0x436900 */
void GameNoBodyStubv19(void) {}

/* Function start: 0x436970 */
unsigned int ReturnConst0v10(void) { return 0; }

/* Function start: 0x4369A0 */
unsigned short HelperOf41D6C0C(void) { return 0; }

/* Function start: 0x4369B0 */
unsigned short HelperOf41D6C0D(void) { return 0; }

/* Function start: 0x4369C0 */
unsigned short ReturnConst0v13(void) { return 0; }

/* Function start: 0x4369D0 */
void GameNoBodyStubv22(void) {}

/* Function start: 0x441140 */
void DoLocalFn1140(void) {}
