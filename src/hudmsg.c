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
    return (short)((MinShort(5, (short)(DosStrlen(text) >> 1)) + 5) *
                   ((char)g_bMessageSpeed_0046af68 + 1));
}

/* Function start: 0x428EA0 */
void WaitForKeyAcknowledge(int mode)
{
    int acknowledged;
    int key;

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
void ReportOutOfMemoryAndExit(const char *resource)
{
    ShowModalMessage("ERROR: Out of memory for %s", resource);
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
        DosStrcpy(g_szHudMessageBuffer_0059e1c0, text);
        SetHudMessageText(g_szHudMessageBuffer_0059e1c0,
                          DAT_004699ac, messageDuration);
        if (messageDuration == 9999) {
            ShowHudTextLine(g_szHudMessageBuffer_0059e1c0,
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

/* Function start: 0x4290A0 */
void ShowGamePausedBanner(short showBanner)
{
    if (showBanner != 0) {
        ShowOnScreenMessage(1, 9999, "GAME PAUSED");
        return;
    }
    WaitForKeyAcknowledge(1);
}

/* Function start: 0x4290D0 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(1, 9999, "WING COMMANDER VER. %s",
                        g_pGameVersion_004693b4);
}

/* Function start: 0x4290F0 */
void SetMessageDisplaySpeed(void)
{
    g_bMessageSpeed_0046af68 =
        (unsigned char)(((signed char)g_bMessageSpeed_0046af68 + 1) % 5);
    ShowOnScreenMessage(0, 0, "MESSAGES SPEED IS NOW %d.",
                        (signed char)g_bMessageSpeed_0046af68 + 1);
}

/* Function start: 0x429120 */
void ReportFramesSkipped(short adjustment)
{
    g_nFrameSkip_00469fb8 = MinShort(
        MaxShort((short)(g_nFrameSkip_00469fb8 + adjustment), 1), 5);
    ShowOnScreenMessage(0, 0, "%d FRAMES SKIPPED.",
                        g_nFrameSkip_00469fb8 - 1);
}

/* Function start: 0x429160 */
int HandleSpaceFlightControls(void)
{
    int notRepeated;
    int control;

    player_input();
    players_flight_dynamics();
    notRepeated = (signed char)g_bCurrentKey_0046c014 !=
                  g_cPreviousKey_0046c018;
    control = GetControlKeyState();
    GetKeyboardModifiers();
    HandleFleetOverviewInput();

    if (g_nTrainSimActive_00469e2c == 0) {
        switch ((signed char)g_bCurrentKey_0046c014) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (notRepeated && (short)get_mode(1) == 4 &&
                DAT_0046c03c == 0 && (signed char)g_bCurrentKey_0046c014 >= 2 &&
                (signed char)g_bCurrentKey_0046c014 <=
                    g_nCommMenuChoiceCount_0046af60 + 2 &&
                (short)get_mode(1) == 4) {
                Chosen_communicate_option(
                    (short)((signed char)g_bCurrentKey_0046c014 - 2));
            }
            break;
        case 0x12:
            if (notRepeated &&
                (control != 0 ||
                 (g_wCurrentInputModifiers_0059ab08 & 0x2000) != 0) &&
                g_cPlayerEjectionDamage_0059bff7 != 4) {
                if ((short)RandomInRange(
                        0, g_cPlayerEjectionDamage_0059bff7) == 0)
                    g_nArcadeState_00469fb0 = 2;
                else
                    malf_sound();
            }
            break;
        case 0x1e:
            if (notRepeated) {
                g_bMouseCursorVisible_0046a018 = 0;
                if ((short)get_mode(1) != 5)
                    SelectCockpitVduMode(1, 5);
                if (DAT_0046a008 == 0) {
                    auto_pilot_sequence();
                } else {
                    GetScreenUpdateFlag();
                    SetViewportRect(
                        &DAT_005a7510, 0, 0,
                        (short)(g_nScreenWidth_0046daa4 - 1),
                        (short)(g_nScreenHeight_0046daa8 - 1));
                    initialize_view_buffer();
                    DAT_0046a008 = -2;
                    auto_pilot_sequence();
                    DAT_0046a008 = 1;
                    GetScreenUpdateFlag();
                    SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
                    initialize_view_buffer();
                }
                FlushInputEvents();
                ClearDebugPauseFlags();
                g_bMouseCursorVisible_0046a018 = 0;
                g_bPointerMovedByKeyboard_005a7d54 = 1;
            }
            break;
        case 0x1f:
            if (control != 0 && notRepeated) {
                if (g_nFlightSoundEffectsEnabled_0046aa34 == 1)
                    ResetSoundStateForScene();
                else
                    ResetSoundStateForFlight();
            }
            break;
        case 0x23:
            if (notRepeated && g_nYourWingman_0046c04c != -1 &&
                g_aeShipObjective_0059d200[g_nYourWingman_0046c04c] !=
                    OBJECTIVE_HOLD_FORMATION) {
                request(0, g_nYourWingman_0046c04c, 9);
            }
            break;
        case 0x2e:
            if (notRepeated) {
                if ((short)message_showing() == 0) {
                    if ((short)get_mode(1) == 4)
                        CloseCommChoiceMenu();
                    else
                        SelectCockpitVduMode(1, 4);
                } else {
                    EndCommMenu();
                }
            }
            break;
        case 0x2f:
            if (notRepeated && control == 0) {
                g_bVideoImagesSuppressed_0046af74 =
                    g_bVideoImagesSuppressed_0046af74 == 0;
                if (g_bVideoImagesSuppressed_0046af74 != 0)
                    SetHudMessageText("VIDEO IMAGES SUPRESSED",
                                      DAT_004699ac, 20);
                else
                    SetHudMessageText("VIDEO IMAGES ENABLED",
                                      DAT_004699b4, 20);
            }
            break;
        case 0x30:
            if (notRepeated && g_nYourWingman_0046c04c != -1 &&
                g_aeShipObjective_0059d200[g_nYourWingman_0046c04c] ==
                    OBJECTIVE_HOLD_FORMATION &&
                any_enemy(0, 14000) != 0) {
                request(0, g_nYourWingman_0046c04c, 7);
            }
            break;
        case 0x31:
            if (notRepeated)
                SelectCockpitVduMode(1, 5);
            init_player_input();
            break;
        case 0x32:
            if (notRepeated && control == 0)
                SetMessageDisplaySpeed();
            break;
        case 0x3b:
            if (GetF1KeyLatch() != 0) {
                if (g_cScreenViewportMode_0059a9f2 == 0) {
                    DAT_0046a008 = DAT_0046a008 == 0;
                    GetScreenUpdateFlag();
                    if (DAT_0046a008 != 0) {
                        SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
                        initialize_cockpit(
                            g_cScreenViewportMode_0059a9f2++);
                        SetMousePosition(
                            (short)(((int)DAT_005a7510.right -
                                     (int)DAT_005a7510.left) / 2 + 1),
                            g_nViewCenterY_0059a854);
                    } else {
                        SetViewportRect(
                            &DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
                        initialize_cockpit(
                            g_cScreenViewportMode_0059a9f2++);
                        SetMousePosition(
                            (short)(((int)DAT_005a7510.right -
                                     (int)DAT_005a7510.left) / 2 + 1),
                            (short)(((int)DAT_005a7510.bottom -
                                     (int)DAT_005a7510.top) / 2));
                    }
                } else {
                    g_bMouseCursorVisible_0046a018 = 0;
                    if (DAT_0046a008 == 0) {
                        new_view(0, 0);
                    } else {
                        GetScreenUpdateFlag();
                        SetViewportRect(
                            &DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
                        initialize_view_buffer();
                        new_view(0, 0);
                        GetScreenUpdateFlag();
                        SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
                        initialize_view_buffer();
                    }
                }
                FlushInputEvents();
                ClearDebugPauseFlags();
                g_bMouseCursorVisible_0046a018 = 0;
                g_bPointerMovedByKeyboard_005a7d54 = 1;
            }
            break;
        case 0x3c:
            g_bMouseCursorVisible_0046a018 = 0;
            if (DAT_0046a008 == 0) {
                new_view(2, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(2, 0);
            goto restore_normal_viewport;
        case 0x3d:
            g_bMouseCursorVisible_0046a018 = 0;
            if (DAT_0046a008 == 0) {
                new_view(1, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(1, 0);
            goto restore_normal_viewport;
        case 0x3e:
            g_bMouseCursorVisible_0046a018 = 0;
            if (DAT_0046a008 == 0) {
                new_view(3, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(3, 0);
            goto restore_normal_viewport;
        case 0x3f:
            g_bMouseCursorVisible_0046a018 = 0;
            if (DAT_0046a008 == 0) {
                new_view(4, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(4, 0);
            goto restore_normal_viewport;
        case 0x40:
            g_bMouseCursorVisible_0046a018 = 0;
            if (DAT_0046a008 == 0) {
                new_view(14, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(14, 0);
            goto restore_normal_viewport;
        case 0x41:
            if (g_acShipTarget_0059ce60[0] == -1)
                break;
            g_bMouseCursorVisible_0046a018 = 0;
            if (DAT_0046a008 == 0) {
                new_view(7, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&DAT_005a7510, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(7, 0);
            goto restore_normal_viewport;
        case 0x42:
            if (notRepeated) {
                g_bMouseCursorVisible_0046a018 = 0;
                g_bMissileCameraEnabled_0046c07c ^= 1;
                if (g_bMissileCameraEnabled_0046c07c != 0)
                    SetHudMessageText("MISSILE CAMERA ON",
                                      DAT_004699ac, 20);
                else
                    SetHudMessageText("MISSILE CAMERA OFF",
                                      DAT_004699b4, 20);
            }
            break;
        case 0x43:
            if (notRepeated) {
                g_bMouseCursorVisible_0046a018 = 0;
                SelectNextExternalViewObject();
                if (DAT_0046a008 == 0) {
                    force_view(4, (short)g_cViewObject_0046c000);
                    break;
                }
                GetScreenUpdateFlag();
                SetViewportRect(
                    &DAT_005a7510, 0, 0,
                    (short)(g_nScreenWidth_0046daa4 - 1),
                    (short)(g_nScreenHeight_0046daa8 - 1));
                initialize_view_buffer();
                force_view(4, (short)g_cViewObject_0046c000);
                goto restore_normal_viewport;
            }
            break;
        }
    }
    goto primary_controls_complete;

restore_normal_viewport:
    GetScreenUpdateFlag();
    SetViewportRect(&DAT_005a7510, 0, 0, 319, 199);
    initialize_view_buffer();

primary_controls_complete:
    switch ((signed char)g_bCurrentKey_0046c014) {
    case 1:
        DAT_0059ab58 = 0;
        if (g_nTrainSimActive_00469e2c != 0)
            return -1;
        if ((short)get_mode(1) == 4) {
            CloseCommChoiceMenu();
            return 0;
        }
        break;
    case 0x0c:
    case 0x4a:
        if (control != 0) {
            ReportFramesSkipped(-1);
            return 0;
        }
        accelerate(-1);
        return 0;
    case 0x0d:
    case 0x4e:
        if (control != 0) {
            ReportFramesSkipped(1);
            return 0;
        }
        accelerate(1);
        return 0;
    case 0x0e:
        g_anShipSpeed_0059b320[0] = 0;
        return 0;
    case 0x0f:
    case 0x37:
        your_afterburner();
        return 0;
    case 0x11:
        if (notRepeated) {
            SelectCockpitVduMode(0, 1);
            return 0;
        }
        break;
    case 0x14:
        if (notRepeated) {
            SelectCockpitVduMode(1, 3);
            return 0;
        }
        break;
    case 0x19:
        ShowGamePausedBanner((short)(control == 0));
        SetFrameTimerPeriodDirect(1);
        return 0;
    case 0x1c:
        if (notRepeated && g_nSelectedReleaseWeaponIndex_0046c058 != -1) {
            ShipWeaponSlot *slot =
                &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1])[
                    g_nSelectedReleaseWeaponIndex_0046c058];

            if (slot->type == OBJECT_TYPE_SPACE_MINE) {
                drop_player_mine(0);
                return 0;
            }
            if (g_nExternalViewShip_0046c040 == -1) {
                g_nExternalViewShip_0046c040 =
                    (short)fire_missile(0);
                if (g_bMissileCameraEnabled_0046c07c == 1 &&
                    g_nExternalViewShip_0046c040 != -1) {
                    new_view(6, g_nExternalViewShip_0046c040);
                    return 0;
                }
            }
        }
        break;
    case 0x1f:
        if (control != 0 && notRepeated) {
            g_nSfxVolumeSetting_00469fbc =
                g_nSfxVolumeSetting_00469fbc == 0 ? 20 : 0;
            SetSoundEffectsVolume(
                g_anVolumeLevels_00469fc8[
                    g_nSfxVolumeSetting_00469fbc / 2]);
            ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                g_nSfxVolumeSetting_00469fbc / 2);
            return 0;
        }
        break;
    case 0x20:
        if (notRepeated) {
            SelectCockpitVduMode(0, 2);
            return 0;
        }
        break;
    case 0x22:
        if (notRepeated) {
            SelectCockpitVduMode(0, 1);
            return 0;
        }
        break;
    case 0x24:
        if (control != 0) {
            CalibrateJoystickInteractive();
            return 0;
        }
        break;
    case 0x26:
        if (notRepeated == 0) {
            init_player_input();
            return 0;
        }
        g_nTargetLockMode_0046c078 =
            (short)(g_nTargetLockMode_0046c078 == 0);
        PlaySfxWaveFileByNumber(0x19, -1, 0);
        if ((short)get_mode(1) == 3) {
            InvalidateVduMode(1);
            return 0;
        }
        break;
    case 0x2b:
        accelerate(9000);
        return 0;
    case 0x2f:
        if (notRepeated && control != 0) {
            ShowVersionBanner();
            return 0;
        }
        break;
    case 0x32:
        if (notRepeated && control != 0) {
            g_nMusicVolumeSetting_00469fc0 =
                g_nMusicVolumeSetting_00469fc0 == 0 ? 20 : 0;
            SetMusicStreamVolume(
                (unsigned short)g_anVolumeLevels_00469fc8[
                    g_nMusicVolumeSetting_00469fc0 / 2]);
            ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
                                g_nMusicVolumeSetting_00469fc0 / 2);
            return 0;
        }
        break;
    case 0x39:
        fire_players_lasers();
        return 0;
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
    if (DAT_0046c03c == 0)
        overlay_head_up_display();
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
    check_message();
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
    if (DAT_00469fb4 <= 1)
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
    SetEventManagerPump(get_player_input);
    savedViewport = (Viewport *)DAT_0059ab23;
    DAT_0059ab23 = &DAT_005a7510;
    init_inflight_music();

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
        g_bMouseAfterburnerControl_0046a02c = 0;
        g_bMouseCursorVisible_0046a018 = 0;
        initialize_view_buffer();
        FlushInputEvents();
    }

    copy_frame(0, 62);
    WarpMouseTo((short)((DAT_005a7510.left + DAT_005a7510.right) / 2),
                (short)((DAT_005a7510.top + DAT_005a7510.bottom) / 2));
    FlushInputEvents();
    g_bMouseAfterburnerControl_0046a02c = 0;
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
        if (HandleSpaceFlightControls() == -1) {
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
    free_inflight_music();
    SetEventManagerPump(0);
    g_bMouseCursorVisible_0046a018 = 0;
    QueueInputEvent(13, 160, 100, 0, 0, 0, 0);
    SetMouseCursorShape(DAT_0059ab19, 0);
    return g_nArcadeState_00469fb0;
}

/* Function start: 0x42A520 */
int calculate_damage_level(void)
{
    ObjectTypeData *typeData;
    short damage;

    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[0]];
    damage = (short)(((typeData->armorLeft -
                       g_aasShipArmor_0059d420[0][2]) * 4) /
                     typeData->armorLeft);
    damage = (short)(damage +
        ((typeData->armorRear - g_aasShipArmor_0059d420[0][1]) * 4) /
            typeData->armorRear);
    damage = (short)(damage +
        ((typeData->armorRight - g_aasShipArmor_0059d420[0][3]) * 4) /
            typeData->armorRight);
    damage = (short)(damage +
        ((typeData->armorFront - g_aasShipArmor_0059d420[0][0]) * 4) /
            typeData->armorFront);
    damage = (short)(
        (g_acShipDamage_0059c460[0] * 30) / typeData->damageCapacity +
        damage * 2);
    damage = (short)(damage +
                     g_asShipAccumulatedDamage_0059dee0[0] * 5);

    if (damage < 5)
        return 0;
    if (damage < 40)
        return 1;
    if (damage < 70)
        return 2;
    return 3;
}

/* Function start: 0x42A610 */
void UpdateTrainSimMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short state;
    short frame;
    TitleMenuRegion *region;

    frame = 0;
    mouseX = g_nMouseX_0059ab10;
    mouseY = g_nMouseY_0059ab12;
    region = g_aTrainSimMissionRegions_00469df8;
    while (region->frame != -1) {
        state = IsPointInRect(mouseX, mouseY, &region->left);
        if (state != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(DAT_0059ab19, frame);
}

/* Function start: 0x42A670 */
void ResetMouseCursorFrame(void)
{
    SetMouseCursorShape(DAT_0059ab19, 0);
}

/* Function start: 0x42A680 */
void UpdateRoomMenuCursor(void)
{
    short mouseY;
    short mouseX;
    short state;
    short frame;
    TitleMenuRegion *region;
    short index;

    mouseY = g_nMouseY_0059ab12;
    mouseX = g_nMouseX_0059ab10;
    region = g_pRoomMenuRegions_00598ab2;
    frame = g_nRoomMenuCursorFrame_00598ab0;
    index = 0;
    ClearRoomMenuLabel();
    while (region->frame != -1) {
        state = IsPointInRect(mouseX, mouseY, &region->left);
        if (state != 0) {
            frame = region->frame;
            if (index >= 20)
                return;
            if (index < 0)
                return;
            SelectRoomMenuLabel(index);
        }
        index++;
        region++;
    }
    SetMouseCursorShape(DAT_0059ab19, frame);
}

/* Function start: 0x42A700 */
void __stdcall FadeViewportPaletteToColour(Viewport *viewport,
                                           unsigned short colour,
                                           short enabled)
{
    unsigned char *indices;
    unsigned short target[3];
    short *currentPalette;
    short *targetPalette;
    unsigned int paletteBytes;
    short activeCount;
    short index;

    (void)enabled;
    if (DAT_0046b168 != 0x13)
        return;

    indices = (unsigned char *)AllocateTaggedMemory(256, 0);
    if (indices == 0)
        return;
    memset(indices, 0, 256);
    activeCount = CollectActivePaletteIndices(viewport, indices, 256);
    paletteBytes = (unsigned int)(activeCount * 6);
    currentPalette = (short *)AllocateTaggedMemory(paletteBytes, 0);
    if (currentPalette == 0) {
        ReleasePacketHandle((int)indices);
        return;
    }
    memset(currentPalette, 0, paletteBytes);
    targetPalette = (short *)AllocateTaggedMemory(paletteBytes, 0);
    if (targetPalette == 0) {
        ReleasePacketHandle((int)indices);
        ReleasePacketHandle((int)currentPalette);
        return;
    }
    memset(targetPalette, 0, paletteBytes);

    GetPaletteEntry((short)colour, target);
    index = 0;
    while (index < activeCount) {
        GetPaletteEntry((short)indices[index],
                        (unsigned short *)&currentPalette[index * 3]);
        memcpy(&targetPalette[index * 3], target, 6);
        index++;
    }

    while (StepPaletteTransition(
               currentPalette, targetPalette,
               (short)(activeCount * 3)) != 0) {
        index = 0;
        while (index < activeCount) {
            g_abPaletteTriplets_005a77f0[indices[index]][0] =
                (unsigned char)currentPalette[index * 3];
            g_abPaletteTriplets_005a77f0[indices[index]][1] =
                (unsigned char)currentPalette[index * 3 + 1];
            g_abPaletteTriplets_005a77f0[indices[index]][2] =
                (unsigned char)currentPalette[index * 3 + 2];
            index++;
        }
        SetWholePaletteFromTriplets(&g_abPaletteTriplets_005a77f0[0][0]);
    }

    ReleasePacketHandle((int)targetPalette);
    ReleasePacketHandle((int)currentPalette);
    ReleasePacketHandle((int)indices);
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x42A8F0 */
short find_objective(int type, short index)
{
    short objective;

    objective = 0;
    while (objective < (short)g_cMissionObjectiveCount_0059c46a) {
        if (g_aMissionObjectives_0059dac0[objective].type == type) {
            if (index == -1)
                return objective;
            if (g_aMissionObjectives_0059dac0[objective].index == index)
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
        if (g_aMissionObjectives_0059dac0[
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
    signed char weaponCount;
    int loadoutOffset;
    ShipWeaponSlot *weaponSlot;
    enum ObjectType type;

    weapon = 0;
    loadoutOffset = (int)obj * sizeof(g_aShipWeapons_0059cab0[0]);
    weaponCount = *(signed char *)
        ((unsigned char *)g_aShipWeapons_0059cab0 + loadoutOffset);
    while (weaponCount > weapon) {
        weaponSlot = (ShipWeaponSlot *)
            ((unsigned char *)g_aShipWeapons_0059cab0 + loadoutOffset + 1) +
            weapon;
        type = weaponSlot->type;

        if (g_aObjectTypeData_00466458[type].objectClass ==
                OBJECT_CLASS_MINE &&
            weaponSlot->disabled == 0)
            return drop_mine(obj, (signed char)weapon, type, 20);
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
            0, (short)(g_stCampaignState_0059ca50.promotionScore - 1));
        return 0;
    }
    kill_ace((short)(personality - 9));
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
    short weaponCount;
    int found;
    ShipWeaponSlot *weaponSlot;

    (void)obj;
    weaponCount = (signed char)g_aShipWeapons_0059cab0[0][0];
    found = 0;
    weaponSlot = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];
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
        InvalidateVduMode(0);
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
        InvalidateVduMode(0);
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
