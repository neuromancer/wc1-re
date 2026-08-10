/*
 *  Autopilot travel and interstitial flight sequences.
 *
 *  Address range 0x403e50-0x40609f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac auto unit preserves visit_the_cinema,
 *  player_wingman, set_speed, auto_position, and auto_pilot_sequence in this
 *  exact order; ship AI begins at 0x4060a0 in the Win32 image.
 */
#include "wc1.h"

/* Function start: 0x403E50 */
void visit_the_cinema(int view, int obj, short frames)
{
    int savedOriginUnlock;
    int savedInputFlag;
    int savedKeyboardFlag;

    savedOriginUnlock = g_nOriginDevUnlock_00469ff4;
    savedInputFlag = DAT_00469ffc;
    savedKeyboardFlag = DAT_0046a000;
    g_nOriginDevUnlock_00469ff4 = 1;
    DAT_00469ffc = 0;
    DAT_0046a000 = 0;
    force_view(view, obj);
    while (frames-- > 0) {
        Update_3Space();
        RenderSpaceViewFrame();
        DIBslam();
        DIBslamReal();
    }
    g_nOriginDevUnlock_00469ff4 = savedOriginUnlock;
    DAT_00469ffc = savedInputFlag;
    DAT_0046a000 = savedKeyboardFlag;
}

/* Function start: 0x403EE0 */
unsigned int player_wingman(short obj)
{
    if (obj != -1 && g_asShipWingLeader_0059d400[obj] == 0)
        return 1;
    return 0;
}

/* Function start: 0x403F10 */
void set_speed(short obj, short speed)
{
    g_anShipSpeed_0059b320[obj] = (int)speed << 8;
    fix_velocity(obj);
}

/* Function start: 0x403F40 */
void auto_position(short obj, short *formationSlot)
{
    short ship = obj;
    short lateral;
    short vertical;
    short forward;

    if (player_wingman(ship) != 0) {
        position_relative_ijk(&g_aShipPosition_0059c490[ship], 0,
                              g_aShipFormationOffset_0059b520[ship].x,
                              g_aShipFormationOffset_0059b520[ship].y,
                              g_aShipFormationOffset_0059b520[ship].z);
        return;
    }

    (*formationSlot)++;
    lateral = ((*formationSlot & 1) >= 1) ? 650 : -650;
    if (g_nAutopilotFormationShipCount_00465544 == *formationSlot)
        lateral = 0;
    vertical = 0;
    forward = MaxShort(1, (short)(*formationSlot >> 1));
    forward = (short)(forward * -1800);
    if (g_nYourWingman_0046c04c != -1) {
        int radii;
        int separation;

        radii = g_asObjectCollisionRadius_0059d710[ship] +
                g_asObjectCollisionRadius_0059d710[g_nYourWingman_0046c04c];
        separation = abs(
            g_aShipFormationOffset_0059b520[g_nYourWingman_0046c04c].z -
            forward);
        if (radii > separation)
            vertical = 500;
    }
    position_relative_ijk(&g_aShipPosition_0059c490[ship], 0,
                          lateral, vertical, forward);
}
