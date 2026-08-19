#include "wc1.h"

int main(int argumentCount, char **arguments)
{
    short ship = 1;
    short target = 2;

    aeShipSide[0] = SIDE_IMPERIAL;
    aeShipSide[1] = SIDE_KILRATHI;
    acPlayerComponentDamage[5] = 0;
    asObjectScreenX[1] = 0;
    asObjectScreenY[1] = 0;
    nTargetLockCountdown = 0;
    nSelectedReleaseWeaponIndex = -1;
    target_locking(1);
    if (nTargetLockCountdown != -1)
        return 1;

    acShipTarget[ship] = (signed char)target;
    aeShipManeuver[ship] = MANEUVER_BUZZ_DEBRIS;
    acShipSequence[ship] = 2;
    aeSpecialManeuver[ship] = SPECIAL_MANEUVER_NONE;
    aeObjectClass[target] = OBJECT_CLASS_SHIP;
    aeShipManeuver[target] = MANEUVER_NONE;
    aShipPosition[ship].x = 0;
    aShipPosition[ship].y = 0;
    aShipPosition[ship].z = 0;
    aShipPosition[target].x = 0;
    aShipPosition[target].y = 0;
    aShipPosition[target].z = 0x10000;
    aShipForwardVector[ship].z = 0x100;
    aShipForwardVector[target].z = 0x100;
    perform_maneuver(ship);
    if (aeShipManeuver[ship] != MANEUVER_NONE)
        return 1;

    bAfterburnerSfxActive = 1;
    Wc1SdlStopDosSoundEffects();
    if (bAfterburnerSfxActive != 0)
        return 1;

    return 0;
}
