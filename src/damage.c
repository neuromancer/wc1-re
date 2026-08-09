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

/* Function start: 0x420040 */
int explode(short attacker, short victim)
{
    (void)attacker;
    if (victim < 0 || g_aeObjectClass_0059d100[victim] == OBJECT_CLASS_NULL)
        return 0;
    if (g_aeSpecialManeuver_0059c3c0[victim] ==
            SPECIAL_MANEUVER_UNKNOWN_9 &&
        g_aeObjectClass_0059d100[victim] >= OBJECT_CLASS_SHIP)
        return 0;
    set_special(victim, SPECIAL_MANEUVER_BLOWING_UP);
    return 1;
}

/* Function start: 0x4202D0 */
void fire_capital_weapon(short obj, short target)
{
    get_facing_range_from_object(obj, target);
    g_acShipTarget_0059ce60[obj] = (signed char)target;
}

/* Function start: 0x4208C0 */
short RandomlyNegate(short v)
{
    if ((short)RandomBelowOrEqual(1) != 0)
        return v;
    return -v;
}

/* Function start: 0x420AA0 */
int fire_turrets(short obj)
{
    if (attacker_in_range(obj, 5000) != 0) {
        g_acShipTarget_0059ce60[obj] =
            (signed char)g_nTargetShip_0059c3b0;
        fire_capital_weapon(obj, g_nTargetShip_0059c3b0);
        return 1;
    }
    return 0;
}
