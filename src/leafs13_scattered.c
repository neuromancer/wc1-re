/*
 *  Thirteenth leaf tranche.  NOT A REAL COMPILATION UNIT (see accessors_scattered.c).
 */
#include "wc1.h"

unsigned char DAT_0059c310[512];
unsigned char DAT_0059d7a0[512];
unsigned char DAT_005a86b0;
short         DAT_0059df60;

extern unsigned char DAT_0059d620[];
extern unsigned char DAT_005a7c30[];
extern unsigned int  DAT_0046505c;
extern char         *DAT_00469004;

extern short UiRoutine03(void);
extern void  FetchDiskPacketRetrying(short a, int b, int c);   /* 0x0041D2E0 */
extern short LoopNavFnA120(int a);                             /* 0x0042A120 */
extern void  ReleaseTbl0046c35fFnBFF0(short a);                /* 0x0040BFF0 */
extern int   GetShipAiFn0E30(void);
extern void  DoCombatFn49E0(void);
extern unsigned short GetTbl0059dec0Fn47E0(short i);
extern void  GetPilotFn1470(void);                             /* 0x00431470 */

/* Function start: 0x41DEB0 */
void LoopAudioStreamFnDEB0(void)
{
    unsigned int saved = DAT_0046505c;

    DAT_0046505c = 1;
    while (UiRoutine03() == 0)
        ;
    DAT_0046505c = saved;
}

/* Function start: 0x426C50 */
void LoadSceneFn6C50(char n)
{
    DAT_005a86b0 = n + 0x16;
    FetchDiskPacketRetrying((short)DAT_005a86b0, 1, 0);
}

/* Function start: 0x42A170 */
unsigned int GetShipAiFnA170(void)
{
    short v = LoopNavFnA120(0);

    if (DAT_0059df60 != v)
        ReleaseTbl0046c35fFnBFF0(v);
    return 0;
}

/* Function start: 0x4314C0 */
void DoCombatFn14C0(void)
{
    DoCombatFn49E0();
    if ((short)GetTbl0059dec0Fn47E0(1) == 6)
        GetPilotFn1470();
    DAT_00469004 = 0;
}

/* Function start: 0x4345D0 */
int GetTbl0059d620Fn45D0(short i)
{
    if ((char)DAT_0059d620[i] < 0x0f)
        return 0;
    return i;
}
