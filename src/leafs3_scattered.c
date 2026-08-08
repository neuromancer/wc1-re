/*
 *  Third leaf tranche -- single-call forwarders, flag tests, small loops.
 *  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

unsigned int   DAT_005a898c;
unsigned short DAT_005a897c;
unsigned char  DAT_0059dcb0[512];
char           DAT_0059d520[512];
unsigned char  DAT_00469068;
unsigned int   DAT_0059ce18[256];
unsigned char  DAT_00469648;
unsigned char  DAT_00469fb4;
unsigned char  DAT_0059ab19;
unsigned char  DAT_0046af60;
extern int *DAT_0046b1a4;
int            DAT_0046da90;
int            DAT_0046da94;
unsigned char  DAT_0059ab3c;
unsigned char  DAT_00598aba;

extern unsigned char DAT_0046964c;

extern void DIBwholePaletteFromWords(void);            /* 0x00433120 */
extern void HelperOf4075A0A(short ship);               /* 0x00407460 */
extern void SelectNewShipAiBehavior(short ship);       /* 0x004060B0 */
extern void DoShipAiFn2C70(short a, int b);            /* 0x00402C70 */
extern void DispatchTbl0059d5e0FnA360(short a);        /* 0x0040A360 */
extern void ScanTbl0059cab0Fn1100(short a, int b);     /* 0x00421100 */
extern void GetShipAiFn7C50(void);                     /* 0x00427C50 */
extern unsigned int UiRoutine05(void);                 /* 0x00427B00 */
extern void GetUiFn9DD0(void);                         /* 0x00409DD0 */
extern void GetUiFn60F0(unsigned char a, int b);       /* 0x004360F0 */
extern void DoUiFn3500(unsigned int v);                /* 0x00403500 */
extern void LoopInputFn5760(int node);                 /* 0x00435760 */
extern void WriteDebugString(const char *s);           /* 0x004030D0 */
extern void GetG005a8760Fn3DB0(const char *fmt, ...);  /* 0x00403DB0 */

void GetG0046b1a4Fn4FB0(void);   /* 0x00434FB0, defined below */

/* Function start: 0x401020 */
void PaletteVideoFn1020(void) { GetG0046b1a4Fn4FB0(); DIBwholePaletteFromWords(); }

/* Function start: 0x402AC0 */
short GetInput(void) { return ((short)DAT_005a898c << 2) | DAT_005a897c; }

/* Function start: 0x403E30 */
void QueryPerfCounterValue(LARGE_INTEGER *p) { QueryPerformanceCounter(p); }

/* Function start: 0x4061E0 */
void ShipAiRoutine01(short ship)
{
    if (DAT_0059dcb0[ship] != 0xff)
        DAT_0059d520[ship] = DAT_0059d520[ship] + 1;
}

/* Function start: 0x4075A0 */
void ShipAiState21(short ship) { HelperOf4075A0A(ship); }

/* Function start: 0x4075C0 */
void ShipAiState03(short ship) { SelectNewShipAiBehavior(ship); }

/* Function start: 0x4098C0 */
void DoLocalFn98C0(short a) { DoShipAiFn2C70(a, -1); }

/* Function start: 0x40A400 */
void DoLocalFnA400(short a) { DispatchTbl0059d5e0FnA360(a); }

/* Function start: 0x413D20 */
int GetCombatFn3D20(void) { return DAT_00469068 < 8; }

/* Function start: 0x4149C0 */
void DebugNoBodyStub(short i) { DAT_0059ce18[i] = 0; }

/* Function start: 0x423CD0 */
void HelperOf4063B0A(short a) { ScanTbl0059cab0Fn1100(a, 0x21); }

/* Function start: 0x425C20 */
void SetDebugFn5C20(void) { DAT_0046964c = 0; DAT_00469648 = 0; }

/* Function start: 0x42A0C0 */
void UiRoutine06(void)
{
    GetShipAiFn7C50();
    if (DAT_00469fb4 < 2)
        UiRoutine05();
    GetUiFn9DD0();
}

/* Function start: 0x42A670 */
void WcOrphanRoutine42A670(void) { GetUiFn60F0(DAT_0059ab19, 0); }

/* Function start: 0x42D870 */
void DoLocalFnD870(unsigned short *p) { DoUiFn3500(*p); }

/* Function start: 0x42E320 */
void DoUiFnE320(void) { GetG005a8760Fn3DB0("FadeMusic"); }

/* Function start: 0x42E380 */
void DoMusicFnE380(void) { GetG005a8760Fn3DB0("SetMusBreakpt"); }

/* Function start: 0x42EF00 */
unsigned int DoLocalFnEF00(void) { WriteDebugString("soundFX"); return 0; }

/* Function start: 0x42EF20 */
void sound_effect(void) { WriteDebugString("sound_effect"); }

/* Function start: 0x430CA0 */
int HelperOf430FC0C(void) { return DAT_0046af60 == 0; }

/* Function start: 0x434FB0 */
void GetG0046b1a4Fn4FB0(void)
{
    (**(void (**)(void *, int, int))(*DAT_0046b1a4 + 0x58))(DAT_0046b1a4, 1, 0);
}

/* Function start: 0x435DB0 */
void LoopG0046da90Fn5DB0(void)
{
    int node = DAT_0046da90;

    while (node != 0) {
        int next = *(int *)(node + 0x14);
        LoopInputFn5760(node);
        node = next;
    }
    DAT_0046da94 = 0;
    DAT_0046da90 = 0;
}

/* Function start: 0x436230 */
void LoopGameFlowFn6230(void) { while (DAT_0059ab3c != 0) ; }

/* Function start: 0x43F690 */
void DoSaveLoadFnF690(void) { DAT_00598aba = 0; }

/* Function start: 0x43F6A0 */
int GetSaveLoadFnF6A0(void) { return DAT_00598aba == 0; }
