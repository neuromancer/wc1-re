/* Objective arrival, warp transitions, mines, and cockpit cleanup.
 * Mac targ symbols corroborate the 42a8f0-42acff range.
 */
#include "wc1.h"
#ifndef SDL_PORT
#include <process.h>
#endif

/* Linker commons defined here; LINK orders them by identifier hash. */
TextContext stRightVduTxtContext; /* 0x005a7700 */
TextContext stCurrentCockpitReadoutTextCtx; /* 0x005a7720 */
TextContext stDefaultTxtContext; /* 0x005a7740 */
TextContext stCurrentConvTxtCtx; /* 0x005a7760 */
short asDamageFlashColour[3]; /* 0x005a7780 */
short nCurrentDmgDispTicks; /* 0x005a7786 */
ShortPoint stWpnDispOrigin; /* 0x005a7788 */
char szDmgStateText[0x48]; /* 0x005a7790 */
ShortPoint stDamageSpritePos; /* 0x005a77d8 */
signed char cDamageDisplayComponentValue; /* 0x005a77dc */
signed char cDamageDisplayFrame; /* 0x005a77dd */
signed char cDamagedComponentCount; /* 0x005a77de */

/* Function start: 0x42A8F0 */
short find_objective(int type, short index)
{
    short objective;

    objective = 0;
    while (objective < (short)cMissionObjectiveCountValue) {
        if (aMissionObjectivesArray[objective].type == type) {
            if (index == -1)
                return objective;
            if (aMissionObjectivesArray[objective].index == index)
                return objective;
        }
        objective++;
    }
    return -1;
}

/* Function start: 0x42A950 */
void arrive_from_warp(short obj)
{
    short objective = find_objective(0, nCurrentNavigationPoint);

    if (objective != -1) {
        if (aMissionObjectivesArray[
                abFlightPathList[objective]].type != 1)
            flag_objective(objective, 1);
        if (cCurrentObjective == objective)
            set_next_destination();
    }
    approve_xyz(obj, 2000, 5000);
    unwarp(obj);
    anShipSpeed[obj] =
        (int)asShipMaximumSpeedArray[obj] << 8;
    fix_velocity(obj);
    if (aeShipSide[obj] == SIDE_IMPERIAL)
        reset_mission_type(obj, MISSION_TYPE_COME_HOME);
    else
        reset_mission_type(obj, MISSION_TYPE_PATROL);
}

/* Function start: 0x42AA10 */
unsigned int unwarp(short obj)
{
    short effect;

    ClearViewport(&stSpaceBufferViewport, cViewportClearColour);
    bViewportDirty = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
        aShipPositionsArray[effect] = aShipPositionsArray[obj];
        aShipVelocity[effect] = aShipVelocity[obj];
        aeShipManeuversTable[obj] = MANEUVER_NONE;
        asObjectCounterArray[obj] = 6;
        return 0;
    }
    abShipNavPtIdx[obj] =
        (signed char)aeObjectTypeList[obj];
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
    return 0;
}

/* Function start: 0x42AAF0 */
unsigned int warp(short obj)
{
    short effect;

    ClearViewport(&stSpaceBufferViewport, cViewportClearColour);
    bViewportDirty = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                         acObjOwnersTable[obj]);
        aShipPositionsArray[effect] = aShipPositionsArray[obj];
        aShipVelocity[effect] = aShipVelocity[obj];
        aeShipManeuversTable[obj] = MANEUVER_WARPING_OUT;
        asObjectCounterArray[obj] = 6;
        return 0;
    }
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                     acObjOwnersTable[obj]);
    return 0;
}

/* Function start: 0x42ABD0 */
int drop_player_mine(short obj)
{
    enum ObjectType type;
    short weapon;

    for (weapon = 0; weapon < aShipWeaponsTable[obj][0]; weapon++) {
        type = ((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon].type;
        if (aObjectTypeData[type].objectClass == OBJECT_CLASS_MINE &&
            ((ShipWeaponSlot *)&aShipWeaponsTable[obj][1])[weapon].disabled == 0)
            return drop_mine(obj, (signed char)weapon, type, 20);
    }
    return -1;
}

/* Function start: 0x42AC50 */
unsigned int personality_killed(short personality)
{
    if (personality < 8) {
        stCampaignState.personalityDeathMission[personality] =
            (int)stCampaignState.currentMission +
            (int)stCampaignState.currentSeries * 4;
        stCampaignState.promotionScore = MaxShort(
            0, (short)(stCampaignState.promotionScore - 1));
        return 0;
    }
    kill_ace((short)(personality - 9));
    stCampaignState.promotionScore++;
    stCampaignState.missionScore += 25;
    return 0;
}

/* Function start: 0x42ACC0 */
void clean_up_cockpit(void)
{
    acShipTargetTable[0] = -1;
    nTargetLockMode = 0;
    if (nYourWingman != -1) {
        nAutoEngageTimer = -1;
        acShipTargetTable[nYourWingman] = -1;
        reset_objective(nYourWingman, OBJECTIVE_HOLD_FORMATION);
    }
    ClearHudGunReadouts();
}
