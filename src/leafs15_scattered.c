/*
 *  Fifteenth leaf tranche.  NOT A REAL COMPILATION UNIT.
 */
#include "wc1.h"

unsigned int  DAT_00468664;
char         *DAT_00469008;

extern unsigned char DAT_0059daca[];
extern unsigned char DAT_005a7dca;
extern unsigned char DAT_0059c310[];
extern unsigned char DAT_0059d7a0[];
extern unsigned char DAT_0059d630[];
extern unsigned int  DAT_004699d8;
extern short         DAT_0046c064;

extern int  GetCombatFn49F0(void);
extern void DoCombatFn14C0(void);
extern void DispatchG004691e2Fn6260(char *a, unsigned char b);
extern void SetCombatFn5FC0(unsigned short v);

/* Function start: 0x40CBC0 */
void GameNoBodyStub(short *p) { *p = (short)((int)*p / (DAT_00468664 * 100)); }

/* Function start: 0x414A20 */
void GetCombatFn4A20(void)
{
    if (GetCombatFn49F0() && (DAT_005a7dca = DAT_005a7dca - 1, DAT_005a7dca < 1))
        DoCombatFn14C0();
}

/* Function start: 0x415050 */
int GetTbl0059dacaFn5050(short i) { return (DAT_0059daca[i * 0x1f] & 4) == 4; }

/* Function start: 0x415070 */
int NavRoutine01(short i) { return (DAT_0059daca[i * 0x1f] & 1) == 1; }

/* Function start: 0x415090 */
int GetTbl0059dacaFn5090(short i) { return (DAT_0059daca[i * 0x1f] & 2) == 2; }

/* Function start: 0x415FF0 */
unsigned int GetCombatFn5FF0(unsigned short v)
{
    if (DAT_0046c064 == -1) {
        SetCombatFn5FC0(v);
        return 1;
    }
    return 0;
}

/* Function start: 0x416480 */
void GetUiFn6480(short v)
{
    if (v != 0)
        DoCombatFn14C0();
    DispatchG004691e2Fn6260(DAT_00469008, (unsigned char)DAT_004699d8);
}

/* Function start: 0x418600 */
void DoLocalFn8600(int *p)
{
    p[0] = -p[0];
    p[1] = -p[1];
    p[2] = -p[2];
}

/* Function start: 0x41A0F0 */
void DoLocalFnA0F0(short *p)
{
    short v = *p;

    if (v < 0) {
        *p = v + 1;
        return;
    }
    if (0 < v)
        *p = v - 1;
}

/* Function start: 0x422830 */
unsigned int ShipAiRoutine19(short ship)
{
    return DAT_0059c310[ship] == 0 && DAT_0059d7a0[ship] == 0 && DAT_0059d630[ship] == 0;
}
