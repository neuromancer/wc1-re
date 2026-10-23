/* Gun and release-weapon selection.
 * Mac select symbols corroborate the 42ad00-42af9f range.
 */
#define WC1_SELECT_SOURCE
#include <assert.h>
#include "wc1.h"
#ifndef SDL_PORT
#include <process.h>
#endif

/* Linker commons defined here; LINK orders them by identifier hash. */
unsigned char *pCommMenuPointerShape; /* 0x005a7660 */
Viewport stModalSrcViewport; /* 0x005a7670 */
unsigned char *pCurrentPilotHandShapePtr; /* 0x005a7684 */
Viewport stPilotHandComposite; /* 0x005a7690 */
Viewport stSceneBuffer; /* 0x005a76b0 */
short aPaletteFadeEntries[6][3]; /* 0x005a76d0 */
unsigned char *pCockpitDamageShape; /* 0x005a76f4 */

/* Function start: 0x42AD00 */
short find_next_gun(short obj, enum ObjectType currentGun)
{
    short firstGun;
    unsigned char *loadout;
    short weapon;
    short weaponCount;
    ShipWeaponSlot *weaponSlot;
    int foundCurrent;

    loadout = aShipWeaponsTable[obj];
    foundCurrent = 0;
    firstGun = -1;
    weaponCount = (signed char)loadout[0];
    weaponSlot = (ShipWeaponSlot *)&loadout[1];
    for (weapon = 0; weapon < weaponCount; weapon++, weaponSlot++) {
        if (aObjectTypeData[weaponSlot->type].objectClass ==
                OBJECT_CLASS_PROJECTILE) {
            if (firstGun == -1)
                firstGun = (short)weaponSlot->type;
            if (foundCurrent == 0) {
                if (weaponSlot->type == currentGun)
                    foundCurrent = 1;
            } else if (weaponSlot->type != currentGun) {
                return (short)weaponSlot->type;
            }
        }
    }
    if (foundCurrent != 0 && firstGun != currentGun)
        firstGun = 0x80;
    return firstGun;
}

/* Function start: 0x42ADA0 */
int select_guns(short obj, short selectedGun)
{
    short weaponCount;
    ShipWeaponSlot *weaponSlot;
    int found;

    (void)obj;
    weaponCount = (signed char)aShipWeaponsTable[0][0];
    found = 0;
    weaponSlot = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    if (weaponCount > 0) {
        do {
            if (aObjectTypeData[weaponSlot->type].objectClass ==
                    OBJECT_CLASS_PROJECTILE) {
                if (selectedGun == weaponSlot->type || selectedGun == 0x80) {
                    weaponSlot->disabled = 0;
                    found = 1;
                } else {
                    weaponSlot->disabled = 1;
                }
            }
            weaponSlot++;
            weaponCount--;
        } while (weaponCount != 0);
    }
    if (found != 0)
        return selectedGun;
    return -1;
}

/* Function start: 0x42AE10 */
unsigned int select_new_gun(void)
{
    eSelectedGunType = (enum ObjectType)select_guns(
        0, find_next_gun(0, eSelectedGunType));
#ifdef SDL_PORT
    /* SWC CODE_15 +0x0222: fifteen status-text updates per selection. */
    if (SdlSwcFlightActive())
        anSwcWeaponNoticeTicks[0] = 15;
#endif
    if (get_mode(0) == 1)
        InvalidateVduMode(0);
    return 0;
}

/* Function start: 0x42AE50 */
unsigned int select_new_release_weapon(enum ObjectType preferredType)
{
    signed char weapon;
    signed char weaponCount;
    int currentWeapon;
    short firstWeapon;
    ShipWeaponSlot *weaponSlots;

    weaponCount = (signed char)aShipWeaponsTable[0][0];
    currentWeapon = *(volatile int *)&nSelectedReleaseWeaponIndex;
    weapon = (signed char)(currentWeapon + 1);
    weaponSlots = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];

    if (weaponCount <= weapon)
        weapon = 0;
    if (currentWeapon == -1) {
        if (preferredType != -1) {
            weapon = 0;
            if (*(volatile signed char *)&aShipWeaponsTable[0][0] > weapon) {
                for (; weapon <
                           *(volatile signed char *)&aShipWeaponsTable[0][0];
                     weapon++) {
                    if (weaponSlots[weapon].type == preferredType) {
                        currentWeapon = weapon;
                        weaponSlots[currentWeapon].disabled = 0;
                        break;
                    }
                }
            }
        }
        nSelectedReleaseWeaponIndex = currentWeapon;
        if (currentWeapon == -1) {
            weapon = 0;
            if (weaponCount > 0) {
                for (; weapon < weaponCount; weapon++) {
                    if (aObjectTypeData[
                            weaponSlots[weapon].type].objectClass !=
                            OBJECT_CLASS_PROJECTILE) {
                        currentWeapon = weapon;
                        nSelectedReleaseWeaponIndex = currentWeapon;
                        weaponSlots[currentWeapon].disabled = 0;
                        break;
                    }
                }
            }
        }
    } else {
        firstWeapon = weapon;
        do {
            enum ObjectType type;

            if (currentWeapon == weapon)
                break;
            type = weaponSlots[weapon].type;
            if (aObjectTypeData[type].objectClass !=
                    OBJECT_CLASS_PROJECTILE &&
                weaponSlots[currentWeapon].type != type) {
                weaponSlots[currentWeapon].disabled = 1;
                currentWeapon = weapon;
                nSelectedReleaseWeaponIndex = currentWeapon;
                weaponSlots[currentWeapon].disabled = 0;
                break;
            }
            weapon++;
            if (weaponCount <= weapon)
                weapon = 0;
        } while (weapon != firstWeapon);
    }
#ifdef SDL_PORT
    /* SWC CODE_15 +0x0398, including automatic selection after firing. */
    if (SdlSwcFlightActive())
        anSwcWeaponNoticeTicks[1] = 15;
#endif
    if (get_mode(0) == 1)
        InvalidateVduMode(0);
    return 0;
}
