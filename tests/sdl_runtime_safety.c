#include "wc1.h"

int main(void)
{
    short ship = 1;
    short target = 2;

    g_aeShipSide_0059d650[0] = SIDE_IMPERIAL;
    g_aeShipSide_0059d650[1] = SIDE_KILRATHI;
    g_acPlayerComponentDamage_0059bff0[5] = 0;
    g_asObjectScreenX_0059d9b0[1] = 0;
    g_asObjectScreenY_0059d930[1] = 0;
    g_nTargetLockCountdown_0046c064 = 0;
    g_nSelectedReleaseWeaponIndex_0046c058 = -1;
    target_locking(1);
    if (g_nTargetLockCountdown_0046c064 != -1)
        return 1;

    g_acShipTarget_0059ce60[ship] = (signed char)target;
    g_aeShipManeuver_0059dcb0[ship] = MANEUVER_BUZZ_DEBRIS;
    g_acShipSequence_0059d520[ship] = 2;
    g_aeSpecialManeuver_0059c3c0[ship] = SPECIAL_MANEUVER_NONE;
    g_aeObjectClass_0059d100[target] = OBJECT_CLASS_SHIP;
    g_aeShipManeuver_0059dcb0[target] = MANEUVER_NONE;
    g_aShipPosition_0059c490[ship].x = 0;
    g_aShipPosition_0059c490[ship].y = 0;
    g_aShipPosition_0059c490[ship].z = 0;
    g_aShipPosition_0059c490[target].x = 0;
    g_aShipPosition_0059c490[target].y = 0;
    g_aShipPosition_0059c490[target].z = 0x10000;
    g_aShipForwardVector_0059bce0[ship].z = 0x100;
    g_aShipForwardVector_0059bce0[target].z = 0x100;
    perform_maneuver(ship);
    if (g_aeShipManeuver_0059dcb0[ship] != MANEUVER_NONE)
        return 1;

    DAT_005a7cec = 1;
    Wc1SdlStopDosSoundEffects();
    if (DAT_005a7cec != 0)
        return 1;

    return 0;
}
