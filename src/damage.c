/*
 *  Ship damage and component repair reporting.
 *
 *  Address range 0x41f000-0x420fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ReportShipSystemDamage/ReportComponentRepaired; string band 0x469960-0x469984.
 */
#include "wc1.h"

/* Function start: 0x41F5D0 */
unsigned int ReportShieldHit(void)
{
    ReportComponentHit(3, 1, 3);
    return 0;
}

/* Function start: 0x4208C0 */
short RandomlyNegate(short v)
{
    if ((short)RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}
