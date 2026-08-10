/*
 *  On-screen message banners and the debug cheat keys.
 *
 *  Address range 0x428000-0x42afff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ShowOnScreenMessage and its six callers; string band 0x46A24C-0x46A378.
 *  The Mac CODE 4 `targ` symbols prove the nested 0x42A8F0-0x42ACFF unit.
 *  The Mac CODE 15 `select` symbols prove the nested 0x42AD00-0x42AF9F unit.
 */
#include "wc1.h"

/* Function start: 0x428E70 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short MeasureMessageWidth(const char *text)
{
    short n = MinShort(5, DosStrlen(text) >> 1);

    return (n + 5) * ((char)g_bMessageSpeed_0046af68 + 1);
}

/* Function start: 0x428EA0 */
void WaitForKeyAcknowledge(int mode)
{
    short acknowledged;
    char key;

    if (mode != 0) {
        acknowledged = 0;
        do {
            PumpWindowMessages();
            if (IsInputEventQueued(4) != 0)
                acknowledged = 1;
        } while (acknowledged == 0);
        acknowledged = 0;
        FlushInputEvents();
        ClearDebugPauseFlags();
        do {
            PumpWindowMessages();
            if (IsInputEventQueued(3) != 0)
                acknowledged = 1;
        } while (acknowledged == 0);
        FlushInputEvents();
        ClearDebugPauseFlags();
        return;
    }
    FlushInputEvents();
    ClearDebugPauseFlags();
    do {
        key = PumpMessagesDuringWait();
    } while (key == 0x19 || key == 0x50 || key == 0x0c);
    FlushInputEvents();
}

/* Function start: 0x428F20 */
void ShowModalMessage(const char *format, ...)
{
    char text[52];

    vsprintf(text, format, (char *)(&format + 1));
    if (ShowModalTextPanel(1, text) != 0) {
        WaitForKeyAcknowledge(0);
        ReleaseModalTextPanel();
        return;
    }
    SystemDebugPrintf(text);
    WaitForKeyAcknowledge(0);
}

/* Function start: 0x428F80 */
void ReportOutOfMemoryAndExit(void)
{
    ShowModalMessage("ERROR: Out of memory for %s");
    FatalErrorAndExit("You do not have enough memory to run Wing Commander.");
}

/* Function start: 0x428FA0 */
void ShowOnScreenMessage(int flags, short duration,
                         const char *format, ...)
{
    short messageDuration;
    short modalShown = 0;
    char text[52];

    vsprintf(text, format, (char *)(&format + 1));
    FlushInputEvents();
    messageDuration = duration;
    if (messageDuration == 9999)
        modalShown = ShowModalTextPanel(1, text);
    if (modalShown == 0) {
        if (messageDuration == 0)
            messageDuration = MeasureMessageWidth(text);
        SetHudTextColour(1);
        DosStrcpy(DAT_0059e1c0, text);
        SetHudMessageText(DAT_0059e1c0,
                          DAT_004699ac, messageDuration);
        if (messageDuration == 9999) {
            ShowHudTextLine(DAT_0059e1c0,
                            (unsigned short)DAT_004699ac);
            dump_buffer_to_screen();
        }
    }
    if (messageDuration == 9999) {
        if (flags != 0)
            WaitForKeyAcknowledge(1);
        else
            WaitForKeyAcknowledge(0);
    }
    if (modalShown != 0) {
        ReleaseModalTextPanel();
        return;
    }
    if (messageDuration == 9999)
        SetHudMessageText("", DAT_004699ac, 2);
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, "WING COMMANDER VER. %s");
}

/* Function start: 0x429160 */
int HandleDebugCheatKeys(void)
{
    unsigned char key;
    int repeated;

    player_input();
    players_flight_dynamics();
    key = g_bCurrentKey_0046c014;
    repeated = (signed char)key == g_cPreviousKey_0046c018;

    switch (key) {
    case 1:
        DAT_0059ab58 = 0;
        if (g_nTrainSimActive_00469e2c != 0)
            return -1;
        break;
    case 0x0c:
    case 0x4a:
        celerate(0, -0x100);
        break;
    case 0x0d:
    case 0x4e:
        celerate(0, 0x100);
        break;
    case 0x0e:
        g_anShipSpeed_0059b320[0] = 0;
        break;
    case 0x0f:
    case 0x37:
        if (g_anShipFuel_0059b470[0] > 0)
            fire_afterburner(0, 8);
        break;
    case 0x1c:
        if (!repeated && g_nSelectedReleaseWeaponIndex_0046c058 != -1)
            fire_weapon(0, (short)g_nSelectedReleaseWeaponIndex_0046c058);
        break;
    case 0x2b:
        g_anShipSpeed_0059b320[0] =
            (int)g_asShipMaximumSpeed_0059c440[0] << 8;
        break;
    case 0x39:
        fire_players_lasers();
        break;
    }
    return 0;
}

/* Function start: 0x429DD0 */
unsigned int Draw_3Space_Frame(void)
{
    UpdateSpacePaletteFade();
    DAT_00469fb4--;
    if (DAT_00469fb4 > 0)
        return 0;
    DAT_00469fb4 = g_nFrameSkip_00469fb8;
    g_nRenderedSpaceFrame_0059d61a++;
    transform_objects_to_your_view();
    update_star_field();
    place_exhaust_on_ships();
    reposition_fixed_child_objects();
    sort_object_depth();
    draw_sorted_objects_to_buffer();
    return 1;
}

/* Function start: 0x429E30 */
void GetArcadeBonus(void)
{
    g_nArcadeWaveBonus_005a7c50 =
        (g_nArcadeTimeRemaining_005a7c2c *
             (g_nTrainSimMission_00469e30 + 1) +
         (g_nTrainSimMission_00469e30 +
          (g_nArcadeWave_00469e34 * 5 + 5) * 2) * 50) * 2;
}

/* Function start: 0x429E70 */
void FigureArcadeTime(void)
{
    g_nArcadeTimeRemaining_005a7c2c =
        (short)((g_nArcadeWave_00469e34 + 6) * 400);
}

/* Function start: 0x429E90 */
void DrawArcadeScorePanel(short x, short y)
{
    char score[20];

    sprintf(score, "%0ld", g_nArcadeScore_005a7bc4);
    DrawFormattedText("%X%YScore: %s0 %XTime: %u %X1 UP",
                      x, y, score, x + 0x82,
                      g_nArcadeTimeRemaining_005a7c2c, x + 0xbe);
}

/* Function start: 0x429EE0 */
void UpdateArcadeScoreDisplay(void)
{
    char bonus[20];

    if (g_nTrainSimActive_00469e2c != 0) {
        SetTextContext(&DAT_005a6bc0);
        DrawArcadeScorePanel(10, 10);
        if (g_nArcadeBonusCountdown_0046a014 < 1) {
            g_nArcadeScore_005a7bc4++;
            g_nArcadeTimeRemaining_005a7c2c--;
            if (g_nArcadeTimeRemaining_005a7c2c < 1) {
                g_nArcadeState_00469fb0 = 4;
                return;
            }
        } else {
            sprintf(bonus, "%0ld", g_nArcadeWaveBonus_005a7c50);
            SetTextCursor((unsigned short)DAT_005a7510.left,
                          (unsigned short)((DAT_005a7510.top +
                                            DAT_005a7510.bottom) / 2 - 5));
            if (g_nCurrentWave_0046c01c != -1) {
                FormatTextBufferFromStart(
                    "Wave %d complete.\n\nBonus Points: %s0%P",
                    g_nArcadeWave_00469e34 + 1, bonus);
                return;
            }
            FormatTextBufferFromStart(
                "Mission %d complete.\n\nBonus Points: %s0%P",
                g_nTrainSimMission_00469e30 + 1, bonus);
        }
    }
}

/* Function start: 0x429FC0 */
unsigned int RenderSpaceViewFrame(void)
{
    if (Draw_3Space_Frame() == 0)
        return 0;
    RefreshAutopilotHud();
    UpdateArcadeScoreDisplay();
    RestoreCockpitExplosionIfVisible();
    dump_buffer_to_screen();
    if (DAT_0046c03c == 0)
        RestoreTransientCockpitGraphics();
    if (DAT_0046a008 == 0 && g_nTrainSimActive_00469e2c != 0) {
        DrawFilledViewportRect(&DAT_005a7510, 10, 10,
                               DAT_005a7510.right, 0x11,
                               DAT_004699d8);
        if (g_nArcadeBonusCountdown_0046a014 != 0) {
            g_nArcadeBonusCountdown_0046a014--;
            if (g_nArcadeBonusCountdown_0046a014 == 0) {
                if (Vector_magnitude(
                        &g_aShipPosition_0059c490[0]) > 0x271000)
                    zero_vector(&g_aShipPosition_0059c490[0]);
                g_nArcadeScore_005a7bc4 += g_nArcadeWaveBonus_005a7c50;
                if (g_nCurrentWave_0046c01c == -1)
                    g_nArcadeState_00469fb0 = 1;
                else
                    g_nArcadeWave_00469e34++;
                ClearViewport(&DAT_005a7510, DAT_004699d8);
            }
        }
    }
    ClearViewport(&DAT_005a7510, DAT_004699d8);
    return 1;
}

/* Function start: 0x42A0C0 */
unsigned int RefreshCockpitStatus(void)
{
    Update_3Space();
    if (DAT_00469fb4 < 2)
        clear_view_buffer();
    return Draw_3Space_Frame();
}

/* Function start: 0x42A0E0 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint)
{
    FixedVector delta;

    ComputeVectorDelta(&g_aShipPosition_0059c490[ship],
                       &navPoint->position, &delta);
    return FixedToShortSaturating((int)Vector_magnitude(&delta));
}

/* Function start: 0x42A120 */
short FindNearestNavPoint(short ship)
{
    short navPointIndex = 0;
    MissionNavPoint *navPoint = g_aMissionNavPoints_0046c2f0;

    do {
        if (navPoint->type == 1 &&
            GetShipDistanceToNavPoint(ship, navPoint) < navPoint->proximityRadius)
            return navPointIndex;
        navPointIndex++;
        navPoint++;
    } while (navPointIndex < WC1_ACTIVE_MISSION_NAV_POINT_COUNT);

    return g_nCurrentNavPoint_0059df60;
}

/* Function start: 0x42A170 */
unsigned int ReleaseStaleNavTarget(void)
{
    short v = FindNearestNavPoint(0);

    if (g_nCurrentNavPoint_0059df60 != v)
        set_up_action_sphere(v);
    return 0;
}

/* Function start: 0x42A190 */
int RunSpaceFlight(short entryNavPoint)
{
    Viewport *savedViewport;
    signed char savedMode;
    unsigned int frameReady;

    DAT_0046a008 = 0;
    if (g_nTrainSimActive_00469e2c == 0 && DAT_0046507c == 0)
        DAT_0046a008 = 1;
    DAT_00469fb4 = 1;
    g_bInputMode_0059a848 = 1;
    SetEventManagerPump(PollSpaceFlightInput);
    savedViewport = (Viewport *)DAT_0059ab23;
    DAT_0059ab23 = &DAT_005a7510;
    ResetSceneFlags();

    if (entryNavPoint == -1)
        entryNavPoint = g_aMissionShips_0046c948[
            g_nPlayerMissionShipIndex_005a8694].navPoint;
    set_up_action_sphere(entryNavPoint);

    if (DAT_0046a008 != 0) {
        GetScreenUpdateFlag();
        SetViewportRect(&DAT_005a7510, 0, 0,
                        (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                        (unsigned short)(g_nScreenHeight_0046daa8 - 1));
        initialize_view_buffer();
        new_view(0, 0);
        GetScreenUpdateFlag();
        SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
        savedMode = g_cScreenViewportMode_0059a9f2;
        g_cScreenViewportMode_0059a9f2++;
        initialize_cockpit(savedMode);
        SetMousePosition(
            (DAT_005a7510.right - DAT_005a7510.left) / 2 + 1,
            g_nViewCenterY_0059a854);
        DAT_0046a02c = 0;
        g_bMouseCursorVisible_0046a018 = 0;
        initialize_view_buffer();
        FlushInputEvents();
    }

    copy_frame(0, 62);
    WarpMouseTo((short)((DAT_005a7510.left + DAT_005a7510.right) / 2),
                (short)((DAT_005a7510.top + DAT_005a7510.bottom) / 2));
    FlushInputEvents();
    DAT_0046a02c = 0;
    g_bMouseCursorVisible_0046a018 = 0;
    g_nArcadeState_00469fb0 = 0;
    DIBslam();
    DIBslamReal();
    SetSpaceFlightFrameTiming();
    FlushInputEvents();
    ClearDebugPauseFlags();
    g_bMouseCursorVisible_0046a018 = 0;
    g_bPointerMovedByKeyboard_005a7d54 = 1;
    frameReady = 1;

    while (g_nArcadeState_00469fb0 == 0) {
        ReadPerformanceCounter(&g_liFlightFrameStart_00476518);
        if (HandleDebugCheatKeys() == -1) {
            g_nArcadeState_00469fb0 = 5;
            break;
        }
        ReadPerformanceCounter(&g_liFlightAfterInput_00476500);
        if (g_nArcadeState_00469fb0 == 0) {
            Update_3Space();
            ReadPerformanceCounter(&g_liFlightAfterSimulation_00476520);
            frameReady = RenderSpaceViewFrame();
            ReadPerformanceCounter(&g_liFlightAfterRender_00476540);
            update_cockpit();
        } else {
            g_liFlightAfterRender_00476540 =
                g_liFlightAfterSimulation_00476520 =
                    g_liFlightAfterInput_00476500;
        }
        ReadPerformanceCounter(&g_liFlightAfterCockpit_00476530);
        if (frameReady != 0) {
            frameReady = 0;
            DIBslam();
            DIBslamReal();
        }
        ReadPerformanceCounter(&g_liFlightFrameEnd_00476508);
        g_nFlightPresentTicks_00476510 =
            (int)(g_liFlightFrameEnd_00476508.LowPart -
                  g_liFlightAfterCockpit_00476530.LowPart);
        g_nFlightCockpitTicks_004764fc =
            (int)(g_liFlightAfterCockpit_00476530.LowPart -
                  g_liFlightAfterRender_00476540.LowPart);
        g_nFlightRenderTicks_00476548 =
            (int)(g_liFlightAfterRender_00476540.LowPart -
                  g_liFlightAfterSimulation_00476520.LowPart);
        g_nFlightSimulationTicks_00476528 =
            (int)(g_liFlightAfterSimulation_00476520.LowPart -
                  g_liFlightAfterInput_00476500.LowPart);
        g_nFlightFrameTotalTicks_004764f8 =
            (int)(g_liFlightFrameEnd_00476508.LowPart -
                  g_liFlightFrameStart_00476518.LowPart);
        g_nFlightInputTicks_00476538 =
            (int)(g_liFlightAfterInput_00476500.LowPart -
                  g_liFlightFrameStart_00476518.LowPart);
        DAT_00598888 = 0;
        DAT_0059888c = 0;
        DAT_00598890 = 0;
    }

    SetCinematicFrameTiming();
    SetViewportRect(&DAT_005a7510, 0, 0,
                    (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                    (unsigned short)(g_nScreenHeight_0046daa8 - 1));
    DAT_0046a008 = 0;
    if (g_nArcadeState_00469fb0 == 1)
        flag_objective(find_objective(1, -1), 2);
    DAT_0046a008 = 0;
    ResetCockpitPaletteEntries();
    DAT_0059ab23 = savedViewport;
    ReleaseSceneFlags();
    SetEventManagerPump(0);
    g_bMouseCursorVisible_0046a018 = 0;
    QueueInputEvent(13, 160, 100, 0, 0, 0, 0);
    SetMouseCursorShape(DAT_0059ab19, 0);
    return g_nArcadeState_00469fb0;
}

/* Function start: 0x42A670 */
void RedrawCommWindow(void)
{
    SetMouseCursorShape(DAT_0059ab19, 0);
}

/* Function start: 0x42A700 */
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled)
{
    unsigned short source[256][3];
    unsigned short palette[256][3];
    unsigned short target[3];
    short index;
    short component;
    short step;

    (void)viewport;
    (void)enabled;
    if (DAT_0046b168 != 0x13)
        return;

    GetPaletteEntry((short)colour, target);
    index = 0;
    do {
        GetPaletteEntry(index, source[index]);
        index++;
    } while (index < 256);

    step = 1;
    do {
        index = 0;
        do {
            component = 0;
            do {
                palette[index][component] = (unsigned short)(
                    source[index][component] +
                    ((int)target[component] - source[index][component]) *
                        step / 4);
                component++;
            } while (component < 3);
            index++;
        } while (index < 256);
        DIBwholePaletteFromWords(&palette[0][0]);
        DIBslam();
        DIBslamReal();
        step++;
    } while (step <= 4);
}

/* Function start: 0x42A8F0 */
short find_objective(int type, short index)
{
    short objective;

    objective = 0;
    while (objective < (short)g_cMissionObjectiveCount_0059c46a) {
        if (g_aMissionObjectives_0059dac5[objective].type == type) {
            if (index == -1)
                return objective;
            if (g_aMissionObjectives_0059dac5[objective].index == index)
                return objective;
        }
        objective++;
    }
    return -1;
}

/* Function start: 0x42A950 */
void arrive_from_warp(short obj)
{
    short objective = find_objective(0, g_nCurrentNavPoint_0059df60);

    if (objective != -1) {
        if (g_aMissionObjectives_0059dac5[
                g_abFlightPath_0059c000[objective]].type != 1)
            flag_objective(objective, 1);
        if (g_cCurrentObjective_0046c020 == objective)
            set_next_destination();
    }
    approve_xyz(obj, 2000, 5000);
    unwarp(obj);
    g_anShipSpeed_0059b320[obj] =
        (int)g_asShipMaximumSpeed_0059c440[obj] << 8;
    fix_velocity(obj);
    if (g_aeShipSide_0059d650[obj] == SIDE_IMPERIAL)
        reset_mission_type(obj, MISSION_TYPE_COME_HOME);
    else
        reset_mission_type(obj, MISSION_TYPE_PATROL);
}

/* Function start: 0x42AA10 */
unsigned int unwarp(short obj)
{
    short effect;

    ClearViewport(&DAT_005a7510, g_cViewportClearColour_004699a0);
    g_bViewportDirty_00469fc4 = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
        g_aShipPosition_0059c490[effect] = g_aShipPosition_0059c490[obj];
        g_aShipVelocity_0059c010[effect] = g_aShipVelocity_0059c010[obj];
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_NONE;
        g_asObjectCounter_0059c330[obj] = 6;
        return 0;
    }
    g_abShipNavPointIndex_0059d7c0[obj] =
        (signed char)g_aeObjectType_0059b560[obj];
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
    return 0;
}

/* Function start: 0x42AAF0 */
unsigned int warp(short obj)
{
    short effect;

    ClearViewport(&DAT_005a7510, g_cViewportClearColour_004699a0);
    g_bViewportDirty_00469fc4 = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                         g_acObjectOwner_0059ce20[obj]);
        g_aShipPosition_0059c490[effect] = g_aShipPosition_0059c490[obj];
        g_aShipVelocity_0059c010[effect] = g_aShipVelocity_0059c010[obj];
        g_aeShipManeuver_0059dcb0[obj] = MANEUVER_WARPING_OUT;
        g_asObjectCounter_0059c330[obj] = 6;
        return 0;
    }
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                     g_acObjectOwner_0059ce20[obj]);
    return 0;
}

/* Function start: 0x42ABD0 */
int drop_player_mine(short obj)
{
    short weapon;

    weapon = 0;
    while (weapon < (signed char)g_aShipWeapons_0059cab0[obj][0]) {
        ShipWeaponSlot *weaponSlot =
            &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[obj][1])[weapon];
        enum ObjectType type = weaponSlot->type;

        if (g_aObjectTypeData_00466458[type].objectClass ==
                OBJECT_CLASS_MINE &&
            weaponSlot->disabled == 0)
            return drop_mine(obj, weapon, type, 20);
        weapon++;
    }
    return -1;
}

/* Function start: 0x42AC50 */
unsigned int personality_killed(short personality)
{
    if (personality < 8) {
        g_stCampaignState_0059ca50.personalityDeathMission[personality] =
            (int)g_stCampaignState_0059ca50.currentMission +
            (int)g_stCampaignState_0059ca50.currentSeries * 4;
        g_stCampaignState_0059ca50.promotionScore = MaxShort(
            0, g_stCampaignState_0059ca50.promotionScore - 1);
        return 0;
    }
    kill_ace(personality - 9);
    g_stCampaignState_0059ca50.promotionScore++;
    g_stCampaignState_0059ca50.missionScore += 25;
    return 0;
}

/* Function start: 0x42ACC0 */
void clean_up_cockpit(void)
{
    short wingman = g_nYourWingman_0046c04c;

    g_acShipTarget_0059ce60[0] = -1;
    g_nTargetLockMode_0046c078 = 0;
    if (wingman != -1) {
        g_nAutoEngageTimer_0046c084 = -1;
        g_acShipTarget_0059ce60[wingman] = -1;
        reset_objective(wingman, OBJECTIVE_HOLD_FORMATION);
    }
    ClearHudGunReadouts();
}

/* Function start: 0x42AD00 */
short find_next_gun(short obj, enum ObjectType currentGun)
{
    unsigned char *loadout = g_aShipWeapons_0059cab0[obj];
    int foundCurrent = 0;
    short weapon = 0;
    short firstGun = -1;
    short weaponCount = (signed char)loadout[0];
    ShipWeaponSlot *weaponSlot = (ShipWeaponSlot *)&loadout[1];

    if (weaponCount > 0) {
        do {
            enum ObjectType type = weaponSlot->type;

            if (g_aObjectTypeData_00466458[type].objectClass ==
                    OBJECT_CLASS_PROJECTILE) {
                if (firstGun == -1)
                    firstGun = (short)type;
                if (foundCurrent == 0) {
                    if (currentGun == type)
                        foundCurrent = 1;
                } else if (currentGun != type) {
                    return (short)type;
                }
            }
            weapon++;
            weaponSlot++;
        } while (weapon < weaponCount);
    }
    if (foundCurrent != 0 && firstGun != currentGun)
        firstGun = 0x80;
    return firstGun;
}

/* Function start: 0x42ADA0 */
int select_guns(short obj, short selectedGun)
{
    short weaponCount = (signed char)g_aShipWeapons_0059cab0[0][0];
    int found = 0;
    ShipWeaponSlot *weaponSlot =
        (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];

    (void)obj;
    if (weaponCount > 0) {
        do {
            if (g_aObjectTypeData_00466458[weaponSlot->type].objectClass ==
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
    g_eSelectedGunType_0046c054 = (enum ObjectType)select_guns(
        0, find_next_gun(0, g_eSelectedGunType_0046c054));
    if ((short)get_mode(0) == 1)
        ClearMessageSlot(0);
    return 0;
}

/* Function start: 0x42AE50 */
unsigned int select_new_release_weapon(enum ObjectType preferredType)
{
    signed char weaponCount;
    int currentWeapon;
    signed char firstWeapon;
    signed char weapon;
    ShipWeaponSlot *weaponSlots;

    weaponCount = (signed char)g_aShipWeapons_0059cab0[0][0];
    currentWeapon = g_nSelectedReleaseWeaponIndex_0046c058;
    weapon = (signed char)(currentWeapon + 1);
    weaponSlots = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];

    if (weaponCount <= weapon)
        weapon = 0;
    if (currentWeapon == -1) {
        if (preferredType != -1) {
            weapon = 0;
            if (weaponCount > 0) {
                do {
                    if (weaponSlots[weapon].type == preferredType) {
                        currentWeapon = weapon;
                        weaponSlots[currentWeapon].disabled = 0;
                        break;
                    }
                    weapon++;
                } while (weapon <
                         (signed char)g_aShipWeapons_0059cab0[0][0]);
            }
        }
        g_nSelectedReleaseWeaponIndex_0046c058 = currentWeapon;
        if (currentWeapon == -1) {
            weapon = 0;
            if (weaponCount > 0) {
                do {
                    if (g_aObjectTypeData_00466458[
                            weaponSlots[weapon].type].objectClass !=
                            OBJECT_CLASS_PROJECTILE) {
                        currentWeapon = weapon;
                        g_nSelectedReleaseWeaponIndex_0046c058 = currentWeapon;
                        weaponSlots[currentWeapon].disabled = 0;
                        break;
                    }
                    weapon++;
                } while (weapon < weaponCount);
            }
        }
    } else {
        firstWeapon = weapon;
        do {
            enum ObjectType type;

            if (currentWeapon == weapon)
                break;
            type = weaponSlots[weapon].type;
            if (g_aObjectTypeData_00466458[type].objectClass !=
                    OBJECT_CLASS_PROJECTILE &&
                weaponSlots[currentWeapon].type != type) {
                weaponSlots[currentWeapon].disabled = 1;
                currentWeapon = weapon;
                g_nSelectedReleaseWeaponIndex_0046c058 = currentWeapon;
                weaponSlots[currentWeapon].disabled = 0;
                break;
            }
            weapon++;
            if (weaponCount <= weapon)
                weapon = 0;
        } while (weapon != firstWeapon);
    }
    if ((short)get_mode(0) == 1)
        ClearMessageSlot(0);
    return 0;
}

/* Function start: 0x42AFA0 */
void WaitForDebugStep(void)
{
    while (TakeDebugStepFlag() == 0) ;
}

/* Function start: 0x42AFB0 */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message,
                                 DWORD user, DWORD first, DWORD second)
{
    (void)timerId;
    (void)message;
    (void)user;
    (void)first;
    (void)second;
    DAT_0059ab3c = 0;
}

/* Function start: 0x42AFC0 */
void SetMultimediaTimerCallback(int period)
{
    int milliseconds = period * 1000 / 60;

    if (period == 0) {
        DAT_0059ab3c = 0;
        if (DAT_005a77ec != 0) {
            timeKillEvent(DAT_005a77ec);
            DAT_005a77ec = 0;
        }
    } else {
        if (DAT_005a77ec != 0) {
            timeKillEvent(DAT_005a77ec);
            DAT_005a77ec = 0;
        }
        DAT_0059ab3c = 1;
        DAT_005a77ec = timeSetEvent(milliseconds, milliseconds,
                                    FrameTimerCallback, 0, 0);
    }
}
