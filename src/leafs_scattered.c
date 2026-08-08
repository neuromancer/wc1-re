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
void DoLocalFn5BB0(void) {}

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
