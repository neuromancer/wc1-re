/*
 *  On-screen message banners and the debug cheat keys.
 *
 *  Address range 0x428000-0x42afff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: ShowOnScreenMessage and its six callers; string band 0x46A24C-0x46A378.
 *  The Mac CODE 4 `targ` symbols prove the nested 0x42A8F0-0x42ACFF unit.
 *  The Mac CODE 15 `select` symbols prove the nested 0x42AD00-0x42AF9F unit.
 */
#include "wc1.h"

/* Function start: 0x4672C5 */
/* Message dwell time: grows with text length, scaled by the speed setting. */
short MeasureMessageWidth(const char *text)
{
    return (short)((MinShort(5, (short)(DosStrlen(text) >> 1)) + 5) *
                   ((char)g_cMessageSpeed_0049b778 + 1));
}

/* Function start: 0x418ECD */
void WaitForKeyAcknowledge(int mode)
{
    int acknowledged;
    int key;

    if (mode != 0) {
        acknowledged = 0;
        do {
            PumpWindowMessages();
            if (FindQueuedInputEvent(4) != 0)
                acknowledged = 1;
        } while (acknowledged == 0);
        acknowledged = 0;
        FlushInputEvents();
        ClearDebugPauseFlags();
        do {
            PumpWindowMessages();
            if (FindQueuedInputEvent(3) != 0)
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

/* Function start: 0x437C2E */
void ShowModalMessage(const char *format, ...)
{
    char text[52];

#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    if (ShowModalTextPanel(1, text) != 0) {
        WaitForKeyAcknowledge(0);
        ReleaseModalTextPanel();
        return;
    }
    SystemDebugPrintf(text);
    WaitForKeyAcknowledge(0);
}

/* Function start: 0x437C96 */
void ReportOutOfMemoryAndExit(const char *resource)
{
    ShowModalMessage("ERROR: Out of memory for %s", resource);
    FatalErrorAndExit("You do not have enough memory to run Wing Commander.");
}

/* Function start: 0x437DFA */
void ShowOnScreenMessage(short duration, const char *format, ...)
{
#if 0
    short messageDuration;
    short modalShown = 0;
    char text[52];

#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    FlushInputEvents();
    messageDuration = duration;
    if (messageDuration == 9999)
        modalShown = ShowModalTextPanel(1, text);
    if (modalShown == 0) {
        if (messageDuration == 0)
            messageDuration = MeasureMessageWidth(text);
        ClearHudMessageDisplay(1);
        DosStrcpy(g_szHudMessageBuffer_0059e1c0, text);
        SetHudMessageText(g_szHudMessageBuffer_0059e1c0,
                          DAT_004699ac, messageDuration);
        if (messageDuration == 9999) {
            SetHudTextColour(g_szHudMessageBuffer_0059e1c0,
                             (unsigned char)DAT_004699ac);
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
#else
    char text[52];
    short modalShown;
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
    modalShown = 0;
    if (duration == 9999)
        modalShown = (short)ShowModalTextPanel(1, text);
    if (modalShown == 0) {
        if (duration == 0)
            duration = MeasureMessageWidth(text);
        ClearHudMessageDisplay(1);
        DosStrcpy(g_szOnScreenMessageBuffer_005d1890, text);
        SetHudMessageText(g_szOnScreenMessageBuffer_005d1890,
                          g_ucSpaceClearColour_0049cb5c, duration);
    }
    if (g_bPauseInputActive_0049ac9c != 0) {
        FlushPendingInputPresses();
        FlushInputEvents();
        ClearDebugPauseFlags();
    }
    if (duration == 9999) {
        while (WaitForInputKey() == 0)
            ServiceSoundSystem();
    }
    if (modalShown == 0) {
        if (duration == 9999)
            SetHudMessageText("", g_ucSpaceClearColour_0049cb5c, 2);
    } else {
        ReleaseModalTextPanel();
    }
#endif
}

/* Function start: 0x437F2F */
void ShowGamePausedBanner(short showBanner)
{
#if 0
    if (showBanner != 0) {
        ShowOnScreenMessage(9999, "GAME PAUSED");
        return;
    }
    WaitForKeyAcknowledge(1);
#else
    if (showBanner != 0) {
        ShowOnScreenMessage(9999, "GAME PAUSED");
    } else {
        while (WaitForInputKey() == 0)
            ServiceSoundSystem();
    }
#endif
}

/* Function start: 0x437F77 */
void ShowVersionBanner(void)
{
    ShowOnScreenMessage(9999, "WING COMMANDER VER. %s",
                        g_pszGameVersion_0049b528);
}

/* Function start: 0x437F9A */
void SetMessageDisplaySpeed(void)
{
    g_cMessageSpeed_0049b778 =
        (unsigned char)(((signed char)g_cMessageSpeed_0049b778 + 1) % 5);
    ShowOnScreenMessage(0, "MESSAGES SPEED IS NOW %d.",
                        (signed char)g_cMessageSpeed_0049b778 + 1);
}

/* Function start: 0x437FD3 */
void ReportFramesSkipped(short adjustment)
{
    g_nFrameSkip_0049d764 = MinShort(
        MaxShort((short)(g_nFrameSkip_0049d764 + adjustment), 1), 5);
    ShowOnScreenMessage(0, "%d FRAMES SKIPPED.",
                        g_nFrameSkip_0049d764 - 1);
}

/* Function start: 0x46733D */
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
            if (notRepeated && get_mode(1) == 4 &&
                g_nCurrentView_00492fa8 == 0 && (signed char)g_bCurrentKey_0046c014 >= 2 &&
                (signed char)g_bCurrentKey_0046c014 <=
                    g_nCommMenuChoiceCount_0049b770 + 2 &&
                get_mode(1) == 4) {
                Chosen_communicate_option(
                    (short)((signed char)g_bCurrentKey_0046c014 - 2));
            }
            break;
        case 0x12:
            if (notRepeated &&
                (control != 0 ||
                 (g_wCurrentInputModifiers_0059ab08 & 0x2000) != 0) &&
                g_acPlayerComponentDamage_00493470[7] != 4) {
                if (RandomInRange(
                        0, g_acPlayerComponentDamage_00493470[7]) == 0)
                    g_nArcadeState_0049d75c = 2;
                else
                    malf_sound();
            }
            break;
        case 0x1e:
            if (notRepeated) {
                g_bMouseCursorVisible_0046a018 = 0;
                if (get_mode(1) != 5)
                    SelectCockpitVduMode(1, 5);
                if (g_nCockpitDisplayMode_0049d71c == 0) {
                    auto_pilot_sequence();
                } else {
                    free_view_buffer();
                    SetViewportRect(
                        &g_stViewBuffer_005d2b00, 0, 0,
                        (short)(g_nScreenWidth_0046daa4 - 1),
                        (short)(g_nScreenHeight_0046daa8 - 1));
                    initialize_view_buffer();
                    g_nCockpitDisplayMode_0049d71c = -2;
                    auto_pilot_sequence();
                    g_nCockpitDisplayMode_0049d71c = 1;
                    free_view_buffer();
                    SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0, 319, 199);
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
            if (notRepeated && g_nYourWingman_0049346c != -1 &&
                g_aeShipObjective_00495f08[g_nYourWingman_0049346c] !=
                    OBJECTIVE_HOLD_FORMATION) {
                request(0, g_nYourWingman_0049346c, 9);
            }
            break;
        case 0x2e:
            if (notRepeated) {
                if (message_showing() == 0) {
                    if (get_mode(1) == 4)
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
                                      g_ucPrimaryTextColour_0049cb64, 20);
            }
            break;
        case 0x30:
            if (notRepeated && g_nYourWingman_0049346c != -1 &&
                g_aeShipObjective_00495f08[g_nYourWingman_0049346c] ==
                    OBJECTIVE_HOLD_FORMATION &&
                any_enemy(0, 14000) != 0) {
                request(0, g_nYourWingman_0049346c, 7);
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
                if (g_cScreenViewportMode_005c82a6 == 0) {
                    g_nCockpitDisplayMode_0049d71c = g_nCockpitDisplayMode_0049d71c == 0;
                    free_view_buffer();
                    if (g_nCockpitDisplayMode_0049d71c != 0) {
                        SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0, 319, 199);
                        initialize_cockpit(
                            g_cScreenViewportMode_005c82a6++);
                        SetMousePosition(
                            (short)(((int)g_stViewBuffer_005d2b00.right -
                                     (int)g_stViewBuffer_005d2b00.left) / 2 + 1),
                            g_nViewCenterY_0059a854);
                    } else {
                        SetViewportRect(
                            &g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
                        initialize_cockpit(
                            g_cScreenViewportMode_005c82a6++);
                        SetMousePosition(
                            (short)(((int)g_stViewBuffer_005d2b00.right -
                                     (int)g_stViewBuffer_005d2b00.left) / 2 + 1),
                            (short)(((int)g_stViewBuffer_005d2b00.bottom -
                                     (int)g_stViewBuffer_005d2b00.top) / 2));
                    }
                } else {
                    g_bMouseCursorVisible_0046a018 = 0;
                    if (g_nCockpitDisplayMode_0049d71c == 0) {
                        new_view(0, 0);
                    } else {
                        free_view_buffer();
                        SetViewportRect(
                            &g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
                        initialize_view_buffer();
                        new_view(0, 0);
                        free_view_buffer();
                        SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0, 319, 199);
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
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                new_view(2, 0);
                break;
            }
            free_view_buffer();
            SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(2, 0);
            goto restore_normal_viewport;
        case 0x3d:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                new_view(1, 0);
                break;
            }
            free_view_buffer();
            SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(1, 0);
            goto restore_normal_viewport;
        case 0x3e:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                new_view(3, 0);
                break;
            }
            free_view_buffer();
            SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(3, 0);
            goto restore_normal_viewport;
        case 0x3f:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                new_view(4, 0);
                break;
            }
            free_view_buffer();
            SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(4, 0);
            goto restore_normal_viewport;
        case 0x40:
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                new_view(14, 0);
                break;
            }
            free_view_buffer();
            SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(14, 0);
            goto restore_normal_viewport;
        case 0x41:
            if (g_acShipTarget_00495f20[0] == -1)
                break;
            g_bMouseCursorVisible_0046a018 = 0;
            if (g_nCockpitDisplayMode_0049d71c == 0) {
                new_view(7, 0);
                break;
            }
            free_view_buffer();
            SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                            (short)(g_nScreenWidth_0046daa4 - 1),
                            (short)(g_nScreenHeight_0046daa8 - 1));
            initialize_view_buffer();
            new_view(7, 0);
            goto restore_normal_viewport;
        case 0x42:
            if (notRepeated) {
                g_bMouseCursorVisible_0046a018 = 0;
                g_bMissileCameraEnabled_00493504 ^= 1;
                if (g_bMissileCameraEnabled_00493504 != 0)
                    SetHudMessageText("MISSILE CAMERA ON",
                                      DAT_004699ac, 20);
                else
                    SetHudMessageText("MISSILE CAMERA OFF",
                                      g_ucPrimaryTextColour_0049cb64, 20);
            }
            break;
        case 0x43:
            if (notRepeated) {
                g_bMouseCursorVisible_0046a018 = 0;
                SelectNextExternalViewObject();
                if (g_nCockpitDisplayMode_0049d71c == 0) {
                    force_view(4, (short)g_cViewObject_0049313c);
                    break;
                }
                free_view_buffer();
                SetViewportRect(
                    &g_stViewBuffer_005d2b00, 0, 0,
                    (short)(g_nScreenWidth_0046daa4 - 1),
                    (short)(g_nScreenHeight_0046daa8 - 1));
                initialize_view_buffer();
                force_view(4, (short)g_cViewObject_0049313c);
                goto restore_normal_viewport;
            }
            break;
        }
    }
    goto primary_controls_complete;

restore_normal_viewport:
    free_view_buffer();
    SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0, 319, 199);
    initialize_view_buffer();

primary_controls_complete:
    switch ((signed char)g_bCurrentKey_0046c014) {
    case 1:
        g_bSceneEscapeRequested_0049d4b0 = 0;
        if (g_nTrainSimActive_00469e2c != 0)
            return -1;
        if (get_mode(1) == 4) {
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
        g_bPauseInputActive_0049ac9c = 1;
        ShowGamePausedBanner((short)(control == 0));
        g_bPauseInputActive_0049ac9c = 0;
        SetFrameTimerPeriodDirect(1);
        return 0;
    case 0x1c:
        if (notRepeated && g_nSelectedReleaseWeaponIndex_004934e0 != -1) {
            ShipWeaponSlot *slot =
                &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1])[
                    g_nSelectedReleaseWeaponIndex_004934e0];

            if (slot->type == OBJECT_TYPE_SPACE_MINE) {
                drop_player_mine(0);
                return 0;
            }
            if (g_nExternalViewShip_00493468 == -1) {
                g_nExternalViewShip_00493468 =
                    (short)fire_missile(0);
                if (g_bMissileCameraEnabled_00493504 == 1 &&
                    g_nExternalViewShip_00493468 != -1) {
                    new_view(6, g_nExternalViewShip_00493468);
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
            ShowOnScreenMessage(0, "SFX VOLUME: %d.",
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
        if (get_mode(1) == 3) {
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
            ShowOnScreenMessage(0, "MUSIC VOLUME: %d.",
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

/* Function start: 0x468E7A */
unsigned int DrawSpaceSceneFrame(void)
{
    short paletteCycleActive;
    short hidePlayer;

    paletteCycleActive = 0;
    hidePlayer = 0;
    if (g_nSpaceExplosionFlashActive_00492fb4 != 0) {
        if ((short)g_nSpacePaletteFadeMode_004901e8 == 0x13) {
            g_nSpaceExplosionFlashStep_005c5872++;
            if (g_nSpaceExplosionFlashStep_005c5872 < 4)
                paletteCycleActive++;
            if (g_aSpaceExplosionFlashPalette_0049d808[
                    g_nSpaceExplosionFlashStep_005c5872][0] != 0) {
                g_asSpacePaletteFade_005d2d60[0] =
                    g_aSpaceExplosionFlashPalette_0049d808[
                        g_nSpaceExplosionFlashStep_005c5872][0];
                g_asSpacePaletteFade_005d2d60[1] =
                    g_aSpaceExplosionFlashPalette_0049d808[
                        g_nSpaceExplosionFlashStep_005c5872][1];
                g_asSpacePaletteFade_005d2d60[2] =
                    g_aSpaceExplosionFlashPalette_0049d808[
                        g_nSpaceExplosionFlashStep_005c5872][2];
            } else {
                g_asSpacePaletteFade_005d2d60[0] = 0;
                g_asSpacePaletteFade_005d2d60[1] = 0;
                g_asSpacePaletteFade_005d2d60[2] = 0x20;
                g_nSpaceExplosionFlashActive_00492fb4 = 0;
            }
            SetPaletteEntry((short)g_cPrimaryViewBufferColour_0049cb88,
                            g_asSpacePaletteFade_005d2d60);
        }
    } else {
        UpdateSpacePaletteFade();
    }
    g_nFrameSkipCountdown_0049d760--;
    if (g_nFrameSkipCountdown_0049d760 > 0)
        return 0;
    g_nFrameSkipCountdown_0049d760 = g_nFrameSkip_0049d764;
    g_nRenderedSpaceFrame_00493138++;
    transform_objects_to_your_view();
    update_star_field();
    place_exhaust_on_ships();
    reposition_fixed_child_objects();
    BuildObjectDepthOrder();
    if (g_bJumpSequenceActive_004962f0 != 0 &&
        g_asShipManeuver_00495f48[0] == MANEUVER_WARPING_OUT) {
        hidePlayer = 1;
        g_aeObjectClass_00495328[0] = OBJECT_CLASS_NULL;
    }
    if (paletteCycleActive == 0)
        draw_sorted_objects_to_buffer();
    if (hidePlayer != 0)
        g_aeObjectClass_00495328[0] = OBJECT_CLASS_SHIP;
    if (g_nCurrentView_00492fa8 == 0 || g_nCurrentView_00492fa8 == 4)
        overlay_head_up_display();
    return 1;
}

/* Function start: 0x4690FF */
unsigned short Draw_3Space_Frame(void)
{
#if 0
    UpdateSpacePaletteFade();
    g_nFrameSkipCountdown_0049d760--;
    if (g_nFrameSkipCountdown_0049d760 > 0)
        return 0;
    g_nFrameSkipCountdown_0049d760 = g_nFrameSkip_0049d764;
    g_nRenderedSpaceFrame_00493138++;
    transform_objects_to_your_view();
    update_star_field();
    place_exhaust_on_ships();
    reposition_fixed_child_objects();
    BuildObjectDepthOrder();
#ifdef WC1_SDL
    Wc1SdlBeginSpaceFrame(
        g_pScreenViewportGeometry_0059a9f4,
        (int)g_cScreenViewportMode_005c82a6,
        g_nCockpitDisplayMode_0049d71c > 0,
        (unsigned char)g_cPrimaryViewBufferColour_0049cb88);
#endif
    draw_sorted_objects_to_buffer();
    if (g_nCurrentView_00492fa8 == 0)
        overlay_head_up_display();
    return 1;
#else
    Update_3Space();
    if (g_nArcadeState_0049d75c != 0)
        return 0;
    if (g_nFrameSkipCountdown_0049d760 <= 1)
        clear_view_buffer();
    return (unsigned short)DrawSpaceSceneFrame();
#endif
}

/* Function start: WC2_UNMAPPED */
void GetArcadeBonus(void)
{
    g_nArcadeWaveBonus_005a7c50 =
        (g_nArcadeTimeRemaining_005a7c2c *
             (g_nTrainSimMission_00469e30 + 1) +
         (g_nTrainSimMission_00469e30 +
          (g_nArcadeWave_00469e34 * 5 + 5) * 2) * 50) * 2;
}

/* Function start: WC2_UNMAPPED */
void FigureArcadeTime(void)
{
    g_nArcadeTimeRemaining_005a7c2c =
        (short)((g_nArcadeWave_00469e34 + 6) * 400);
}

/* Function start: WC2_UNMAPPED */
void DrawWc1ArcadeScorePanel(short x, short y)
{
    char score[20];

    sprintf(score, "%0ld", g_nArcadeScore_005a7bc4);
    DrawFormattedText("%X%YScore: %s0 %XTime: %u %X1 UP",
                      x, y, score, x + 0x82,
                      g_nArcadeTimeRemaining_005a7c2c, x + 0xbe);
}

/* Function start: WC2_UNMAPPED */
void UpdateArcadeScoreDisplay(void)
{
    char bonus[20];

    if (g_nTrainSimActive_00469e2c != 0) {
        SetTextContext(&g_stSpaceTextContext_005d21c0);
        DrawWc1ArcadeScorePanel(10, 10);
        if (g_nArcadeBonusCountdown_0046a014 < 1) {
            g_nArcadeScore_005a7bc4++;
            g_nArcadeTimeRemaining_005a7c2c--;
            if (g_nArcadeTimeRemaining_005a7c2c < 1) {
                g_nArcadeState_0049d75c = 4;
                return;
            }
        } else {
            sprintf(bonus, "%0ld", g_nArcadeWaveBonus_005a7c50);
            SetTextCursor((unsigned short)g_stViewBuffer_005d2b00.left,
                          (unsigned short)((g_stViewBuffer_005d2b00.top +
                                            g_stViewBuffer_005d2b00.bottom) / 2 - 5));
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

/* Function start: 0x46903F */
void RenderSpaceViewFrame(void)
{
#if 0
    if (Draw_3Space_Frame() == 0)
        return 0;
    check_message();
    UpdateArcadeScoreDisplay();
    RestoreCockpitExplosionIfVisible();
    dump_buffer_to_screen();
    if (g_nCurrentView_00492fa8 == 0)
        RestoreTransientCockpitGraphics();
    if (g_nCockpitDisplayMode_0049d71c == 0 && g_nTrainSimActive_00469e2c != 0) {
        DrawFilledViewportRect(&g_stViewBuffer_005d2b00, 10, 10,
                               g_stViewBuffer_005d2b00.right, 0x11,
                               g_cPrimaryViewBufferColour_0049cb88);
        if (g_nArcadeBonusCountdown_0046a014 != 0) {
            g_nArcadeBonusCountdown_0046a014--;
            if (g_nArcadeBonusCountdown_0046a014 == 0) {
                if (Vector_magnitude(
                        &g_aShipPosition_00494550[0]) > 0x271000)
                    zero_vector(&g_aShipPosition_00494550[0]);
                g_nArcadeScore_005a7bc4 += g_nArcadeWaveBonus_005a7c50;
                if (g_nCurrentWave_0046c01c == -1)
                    g_nArcadeState_0049d75c = 1;
                else
                    g_nArcadeWave_00469e34++;
                ClearViewport(&g_stViewBuffer_005d2b00, g_cPrimaryViewBufferColour_0049cb88);
            }
        }
    }
    ClearViewport(&g_stViewBuffer_005d2b00, g_cPrimaryViewBufferColour_0049cb88);
    return 1;
#else
    int displayMode;

    if (DrawSpaceSceneFrame() == 0)
        return;
    check_message();
    RestoreCockpitExplosionIfVisible();
    if (g_pszPendingHudMessage_0049afec !=
            g_pszDisplayedHudMessage_0049aff0 &&
        g_pszDisplayedHudMessage_0049aff0 != 0) {
        ClearHudMessageDisplay(0);
    }
    if (g_nCockpitDisplayMode_0049d71c > 0) {
        displayMode = g_nCockpitDisplayMode_0049d71c;
        g_nCockpitDisplayMode_0049d71c = 9;
    }
    dump_buffer_to_screen();
    if (g_nCockpitDisplayMode_0049d71c == 9)
        g_nCockpitDisplayMode_0049d71c = displayMode;
    if (g_nCurrentView_00492fa8 == 0 || g_nCurrentView_00492fa8 == 4)
        RestoreTransientCockpitGraphics();
    ClearViewport(&g_stViewBuffer_005d2b00,
                  g_cPrimaryViewBufferColour_0049cb88);
#endif
}

/* Function start: WC2_UNMAPPED */
unsigned int RefreshCockpitStatus(void)
{
    Update_3Space();
    if (g_nFrameSkipCountdown_0049d760 <= 1)
        clear_view_buffer();
    return Draw_3Space_Frame();
}

/* Function start: 0x469143 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint)
{
    FixedVector delta;

    ComputeVectorDelta(&g_aShipPosition_00494550[ship],
                       &navPoint->position, &delta);
    return FixedToShortSaturating(Vector_magnitude(&delta));
}

/* Function start: 0x46918D */
short FindNearestNavPoint(short ship)
{
    short navPointIndex = 0;
    MissionNavPoint *navPoint = g_aMissionNavPoints_00491e98;

#if 0
    do {
        if (navPoint->type == 1 &&
            GetShipDistanceToNavPoint(ship, navPoint) < navPoint->proximityRadius)
            return navPointIndex;
        navPointIndex++;
        navPoint++;
    } while (navPointIndex < WC1_ACTIVE_MISSION_NAV_POINT_COUNT);

    return g_nCurrentNavPoint_004931bc;
#else
    for (; navPointIndex < 10; navPointIndex++, navPoint++) {
        if (navPoint->type == 1 &&
            navPoint->systemIndex == g_nCurrentStarSystem_005d169c &&
            GetShipDistanceToNavPoint(ship, navPoint) <
                navPoint->proximityRadius)
            return navPointIndex;
    }
    return g_nCurrentNavPoint_004931bc;
#endif
}

/* Function start: 0x469223 */
unsigned int ReleaseStaleNavTarget(void)
{
    short v = FindNearestNavPoint(0);

    if (g_nCurrentNavPoint_004931bc != v)
        set_up_action_sphere(v);
    return 0;
}

/* Function start: 0x4379F0 */
void InitializeSpaceFlightInput(void)
{
    ClearInputPump();
    InitializeInputDriverHook();
    g_bInputCursorEnabled_005c80e6 = 1;
    g_nInputRepeatDelay_005c80d6 = g_bInputCursorEnabled_005c80e6;
    g_nFlightInputCommand_005d1798 = 0;
    ConfigureInputPump(1, get_player_input);
    SetInputViewport(&g_stViewBuffer_005d2b00);
    g_nUiInputMode_005c8d3c = 0;
}

/* Function start: 0x42D4C1 */
void ReleaseSceneMusicPacket(void)
{
    if (g_pSceneMusicPacket_00499c08 != 0) {
        StopMusic(0);
        g_dwRestoredSceneMusicState_005d14e8 =
            g_dwSceneMusicState_005d2dc4;
        ReleasePacketSlot(&g_pSceneMusicPacket_00499c08);
    }
}

/* Function start: 0x446FC8 */
void ProcessCannedSceneInput(void)
{
    InputEventState event;
    short changed;
    short eventType;

    g_cPreviousCannedSceneKey_0049312c = g_cCannedSceneKey_00493128;
    ServiceInputDevices(15);
    eventType = GetNextInputEvent(&event);
    if (eventType == 4) {
        g_cCannedSceneKey_00493128 = (signed char)event.status;
        if (g_cPreviousCannedSceneKey_0049312c !=
            g_cCannedSceneKey_00493128)
            changed = 1;
        else
            changed = 0;
        switch (g_cCannedSceneKey_00493128) {
        case 0x33:
        case 0x34:
            break;
        case 0x3b:
            new_view(0, 0);
            break;
        case 0x3c:
            new_view(2, 0);
            break;
        case 0x3d:
            new_view(1, 0);
            break;
        case 0x3e:
            new_view(3, 0);
            break;
        case 0x3f:
            new_view(5, 0);
            break;
        case 0x40:
            new_view(15, 0);
            break;
        case 0x41:
            if (g_acShipTarget_00495f20[0] != -1)
                new_view(8, 0);
            break;
        case 0x43:
            if (changed != 0) {
                SelectNextExternalViewObject();
                force_view(5, (short)g_cCannedSceneViewObject_0049313c);
            }
            break;
        }
    }
}

#pragma function(memcpy)
/* Function start: 0x44698F */
void FinishCannedScenePlayback(void)
{
    unsigned int stateBytes;

    if (g_bHighMemoryBuffersReady_005d2ad8 != 0) {
        g_pCannedSceneStateBlock_005d3fb0 =
            (void *)IdentityDword((unsigned int)g_pHighMemoryBlockB_00490200);
        stateBytes = (unsigned int)(
            g_abCannedSceneInitialState_00493130 +
            sizeof(g_abCannedSceneInitialState_00493130) -
            g_abCannedSceneInitialState_00493130);
        memcpy(g_abCannedSceneInitialState_00493130,
               g_pCannedSceneStateBlock_005d3fb0, stateBytes);
        if (g_nArcadeState_0049d75c != 4) {
            force_view(0, 0);
            SetHudMessageText(g_szEndInflightReplay_0049b738,
                              (unsigned short)g_ucSpaceClearColour_0049cb5c,
                              20);
        }
        _unlink(g_szCannedSceneTapeFile_00490208);
    }
}
#pragma intrinsic(memcpy)

/* Function start: 0x40230E */
void WriteTapeInitialState(void)
{
    short file;
    unsigned int stateBytes;

    file = CreateDataFile(g_szCannedSceneTapeFile_00490208);
    if (file == -1)
        ReportFatalErrorCode(g_szCannedSceneCreateError_00490270);
    stateBytes = (unsigned int)(
        g_abCannedSceneInitialState_00493130 +
        sizeof(g_abCannedSceneInitialState_00493130) -
        g_abCannedSceneInitialState_00493130);
    WriteDataFileAtOffset((unsigned short)file,
                          g_nCannedSceneFileOffset_005d3fac,
                          stateBytes,
                          g_abCannedSceneInitialState_00493130);
    g_nCannedSceneFileOffset_005d3fac += stateBytes;
    CloseDataFile((unsigned short)file);
}

/* Function start: 0x401A62 */
void InitializeCannedSceneFrameIndex(void)
{
    CannedSceneBufferHeader *header;
    short index;

    for (index = 0; index < 70; index++)
        g_asCannedSceneFrameOffsets_005d3fc0[index] = -1;
    g_nCannedSceneWriteIndex_005d3fa8 = 0;
    header = (CannedSceneBufferHeader *)IdentityDword(
        (unsigned int)g_pHighMemoryBlockA_004901f8);
    header->nextFrame = -1;
    header->byteCount = 0x29a;
}

/* Function start: 0x401A10 */
void ResetCannedSceneRecording(void)
{
    if (g_bHighMemoryBuffersReady_005d2ad8 != 0) {
        _unlink(g_szCannedSceneTapeFile_00490208);
        g_nCannedSceneFrameCount_005d404c = 0;
        g_nCannedSceneFileOffset_005d3fac = 0;
        g_nCannedSceneMode_0049021c = 0;
        WriteTapeInitialState();
        InitializeCannedSceneFrameIndex();
    }
}

/* Function start: WC2_UNMAPPED */
int RunWc1SpaceFlight(short entryNavPoint)
{
    Viewport *savedViewport;
    signed char savedMode;
    unsigned int frameReady;

    g_nCockpitDisplayMode_0049d71c = 0;
    if (g_nTrainSimActive_00469e2c == 0 && DAT_0046507c == 0)
        g_nCockpitDisplayMode_0049d71c = 1;
    g_nFrameSkipCountdown_0049d760 = 1;
    g_bInputMode_0059a848 = 1;
    SetEventManagerPump(get_player_input);
    savedViewport = (Viewport *)g_stMouseCursorState_0059ab10.viewport;
    g_stMouseCursorState_0059ab10.viewport = &g_stViewBuffer_005d2b00;
    init_inflight_music();

    if (entryNavPoint == -1)
        entryNavPoint = g_aMissionShips_00492290[
            g_stMissionHeader_005d3e70.playerMissionShip].navPoint;
    set_up_action_sphere(entryNavPoint);

    if (g_nCockpitDisplayMode_0049d71c != 0) {
        free_view_buffer();
        SetWc1ViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                           (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                           (unsigned short)(g_nScreenHeight_0046daa8 - 1));
        initialize_view_buffer();
        new_view(0, 0);
        free_view_buffer();
        SetWc1ViewportRect(&g_stViewBuffer_005d2b00, 0, 0, 319, 199);
        savedMode = g_cScreenViewportMode_005c82a6;
        g_cScreenViewportMode_005c82a6++;
        initialize_cockpit(savedMode);
        SetMousePosition(
            (g_stViewBuffer_005d2b00.right - g_stViewBuffer_005d2b00.left) / 2 + 1,
            g_nViewCenterY_0059a854);
        g_bMouseAfterburnerControl_0046a02c = 0;
        g_bMouseCursorVisible_0046a018 = 0;
        initialize_view_buffer();
        FlushInputEvents();
    }

    copy_frame(0, 62);
    WarpWc1MouseTo((short)((g_stViewBuffer_005d2b00.left + g_stViewBuffer_005d2b00.right) / 2),
                (short)((g_stViewBuffer_005d2b00.top + g_stViewBuffer_005d2b00.bottom) / 2));
    FlushInputEvents();
    g_bMouseAfterburnerControl_0046a02c = 0;
    g_bMouseCursorVisible_0046a018 = 0;
    g_nArcadeState_0049d75c = 0;
    DIBslam();
    DIBslamReal();
    SetWc1SpaceFlightFrameTiming();
    FlushInputEvents();
    ClearDebugPauseFlags();
    g_bMouseCursorVisible_0046a018 = 0;
    g_bPointerMovedByKeyboard_005a7d54 = 1;
    frameReady = 1;

    while (g_nArcadeState_0049d75c == 0) {
        ReadPerformanceCounter(&g_liFlightFrameStart_00476518);
        if (HandleSpaceFlightControls() == -1) {
            g_nArcadeState_0049d75c = 5;
            break;
        }
        ReadPerformanceCounter(&g_liFlightAfterInput_00476500);
        if (g_nArcadeState_0049d75c == 0) {
            Update_3Space();
            ReadPerformanceCounter(&g_liFlightAfterSimulation_00476520);
            RenderSpaceViewFrame();
            frameReady = 1;
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

#ifdef WC1_SDL
    Wc1SdlCancelSpaceFrame();
    if (Wc1SdlUsingDosData())
        Wc1SdlStopDosSoundEffects();
#endif
    SetCinematicFrameTiming(20.0f);
    SetWc1ViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                       (unsigned short)(g_nScreenWidth_0046daa4 - 1),
                       (unsigned short)(g_nScreenHeight_0046daa8 - 1));
    g_nCockpitDisplayMode_0049d71c = 0;
    if (g_nArcadeState_0049d75c == 1)
        flag_objective(find_objective(1, -1), 2);
    g_nCockpitDisplayMode_0049d71c = 0;
    ResetCockpitPaletteEntries();
    g_stMouseCursorState_0059ab10.viewport = savedViewport;
    free_inflight_music();
    SetEventManagerPump(0);
    g_bMouseCursorVisible_0046a018 = 0;
    QueueInputEvent(13, 160, 100, 0, 0, 0, 0, 0, 0);
    SetMouseCursorShape(g_stMouseCursorState_0059ab10.shape, 0);
    return g_nArcadeState_0049d75c;
}

/* Function start: 0x46925E */
int RunSpaceFlight(short entryNavPoint)
{
    InputPumpContext *savedInputContext;
    unsigned char savedInputDelay;
    short savedUiInputMode;
    short inputResult;
    short index;

    savedUiInputMode = g_nUiInputMode_005c8d3c;
    g_bSpaceFlightActive_005c586c = 1;
    g_nCockpitDisplayMode_0049d71c = 0;
    if (g_bFullScreenSpaceView_0049d718 == 0)
        g_nCockpitDisplayMode_0049d71c = 1;
    for (index = 0; index < 7; index++)
        g_abMissionShipStatusFlags_005d2fb0[index] = 0;
    g_bMissionDeathSequencePending_0049b720 = 0;
    g_bMissionWingmanFlag_005c8dbe = 0;
    g_nFrameSkipCountdown_0049d760 = 1;
    InitializeSpaceFlightInput();
    g_nSpaceExplosionFlashActive_00492fb4 = 0;
    savedInputDelay = g_nInputRepeatDelay_005c80d6;
    savedInputContext = g_pActiveInputContext_005c8487;
    ReleaseSceneMusicPacket();
    init_inflight_music();

    if (entryNavPoint == -1)
        entryNavPoint = g_aMissionShips_00492290[
            g_stMissionHeader_005d3e70.playerMissionShip].navPoint;
    set_up_action_sphere(entryNavPoint);
    force_view(0, 0);
    SetSpaceFlightFrameTiming();

    if (g_nCockpitDisplayMode_0049d71c != 0) {
        free_view_buffer();
        SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                        (unsigned short)(g_nScreenWidth_0049d4d8 - 1),
                        (unsigned short)(g_nScreenHeight_0049d4dc - 1));
        initialize_view_buffer();
        new_view(0, 0);
        free_view_buffer();
        SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0, 319, 199);
        initialize_cockpit(g_cScreenViewportMode_005c82a6++);
        g_nCockpitControlState_0049d7ac =
            g_nCockpitControlGoal_0049d7d0 = 0;
        initialize_view_buffer();
        FlushInputEvents();
    }

    copy_frame(0, 0x44);
    SetPersonnelMousePosition(
        (short)((g_stViewBuffer_005d2b00.left +
                 g_stViewBuffer_005d2b00.right) / 2),
        (short)((g_stViewBuffer_005d2b00.top +
                 g_stViewBuffer_005d2b00.bottom) / 2));
    g_nCockpitControlState_0049d7ac = 0;
    g_nArcadeState_0049d75c = 0;

    while (g_nArcadeState_0049d75c == 0) {
        SetFrameTimerPeriodDirect(g_nSpaceFramePeriod_0049d768);
        if (g_nCannedSceneMode_0049021c != 0) {
            ProcessCannedSceneInput();
            inputResult = 0;
            if (g_nCannedSceneRecordedFrameCount_005d3faa ==
                g_nCannedSceneFrameCount_005d404c) {
                FinishCannedScenePlayback();
                ResetCannedSceneRecording();
            }
        }
        if (g_nCannedSceneMode_0049021c == 0 &&
            g_bJumpSequenceActive_004962f0 == 0)
            inputResult = (short)HandleSpaceFlightControls();
        if (inputResult == -1) {
            g_nArcadeState_0049d75c = 5;
        } else {
            Update_3Space();
            if (g_nArcadeState_0049d75c != 0)
                break;
            g_bRenderingSpaceFrame_0049d858 = 1;
            RenderSpaceViewFrame();
            g_bRenderingSpaceFrame_0049d858 = 0;
            update_cockpit();
            if (g_pszPendingHudMessage_0049afec != 0)
                SetHudTextColour(g_pszPendingHudMessage_0049afec,
                                  g_nHudMessageColour_005d1ef0);
        }
        MarkDibDirty();
        DIBslamReal();
        WaitForFrameTick();
    }

    SetViewportRect(&g_stViewBuffer_005d2b00, 0, 0,
                    (unsigned short)(g_nScreenWidth_0049d4d8 - 1),
                    (unsigned short)(g_nScreenHeight_0049d4dc - 1));
    g_nCockpitDisplayMode_0049d71c = 0;
    if (g_nArcadeState_0049d75c == 1)
        flag_objective(find_objective(1, -1), 2);
    ResetCockpitPaletteEntries();
    SetDefaultFrameTiming();
    if (savedInputContext != 0)
        SetInputViewport(savedInputContext->viewport);
    free_inflight_music();
    ConfigureInputPump(1, 0);
    g_nCockpitControlState_0049d7ac = 0;
    g_nUiInputMode_005c8d3c = savedUiInputMode;
    g_nInputRepeatDelay_005c80d6 = savedInputDelay;
    g_bSpaceFlightActive_005c586c = 0;
    return g_nArcadeState_0049d75c;
}

/* Function start: 0x4695FD */
int calculate_damage_level(void)
{
    ObjectTypeData *typeData;
    short damage;

    typeData = &g_aObjectTypeData_00496d30[g_acObjectType_00493980[0]];
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

/* Function start: 0x419A40 */
void UpdateTrainSimMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short state;
    short frame;
    TitleMenuRegion *region;

    frame = 0;
    mouseX = g_stMouseCursorState_0059ab10.x;
    mouseY = g_stMouseCursorState_0059ab10.y;
    region = g_aTrainSimMissionRegions_00469df8;
    while (region->frame != -1) {
        state = IsPointInRect(mouseX, mouseY, &region->left);
        if (state != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(g_stMouseCursorState_0059ab10.shape, frame);
}

/* Function start: WC2_UNMAPPED */
void ResetMouseCursorFrame(void)
{
    SetMouseCursorShape(g_stMouseCursorState_0059ab10.shape, 0);
}

/* Function start: WC2_UNMAPPED */
void UpdateRoomMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short state;
    short frame;
    TitleMenuRegion *region;
    short index;

    mouseX = g_stMouseCursorState_0059ab10.x;
    frame = g_nRoomMenuCursorFrame_00598ab0;
    index = 0;
    region = g_pRoomMenuRegions_00598ab2;
    mouseY = g_stMouseCursorState_0059ab10.y;
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
    SetMouseCursorShape(g_stMouseCursorState_0059ab10.shape, frame);
}

/* Function start: 0x453820 */
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
    if (g_nSpacePaletteFadeMode_004901e8 != 0x13)
        return;

    indices = AllocateTaggedMemory(256, 0);
    if (indices == 0)
        return;
    memset(indices, 0, 256);
    activeCount = CollectActivePaletteIndices(viewport, indices, 256);
    paletteBytes = (unsigned int)(activeCount * 6);
    currentPalette = AllocateTaggedMemory(paletteBytes, 0);
    if (currentPalette == 0) {
        ReleasePacketHandle(indices);
        return;
    }
    memset(currentPalette, 0, paletteBytes);
    targetPalette = AllocateTaggedMemory(paletteBytes, 0);
    if (targetPalette == 0) {
        ReleasePacketHandle(indices);
        ReleasePacketHandle(currentPalette);
        return;
    }
    memset(targetPalette, 0, paletteBytes);

    GetPaletteEntry((short)colour, target);
    for (index = 0; index < activeCount; index++) {
        GetPaletteEntry((short)indices[index],
                        (unsigned short *)&currentPalette[index * 3]);
        memcpy(&targetPalette[index * 3], target, 6);
    }

    while (StepPaletteTransition(
               currentPalette, targetPalette,
               (short)(activeCount * 3)) != 0) {
        for (index = 0; index < activeCount; index++) {
            g_abPaletteTriplets_005a77f0[indices[index]][0] =
                (unsigned char)currentPalette[index * 3];
            g_abPaletteTriplets_005a77f0[indices[index]][1] =
                (unsigned char)currentPalette[index * 3 + 1];
            g_abPaletteTriplets_005a77f0[indices[index]][2] =
                (unsigned char)currentPalette[index * 3 + 2];
        }
        SetWholePaletteFromTriplets(&g_abPaletteTriplets_005a77f0[0][0]);
    }

    ReleasePacketHandle(targetPalette);
    ReleasePacketHandle(currentPalette);
    ReleasePacketHandle(indices);
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x424A60 */
short find_objective(int type, short index)
{
    short objective;

    objective = 0;
    while (objective < (short)g_cMissionObjectiveCount_00493294) {
        if (g_aMissionObjectives_004932a8[objective].type == type) {
            if (index == -1)
                return objective;
            if (g_aMissionObjectives_004932a8[objective].index == index)
                return objective;
        }
        objective++;
    }
    return -1;
}

/* Function start: 0x424AEE */
void arrive_from_warp(short obj)
{
    short objective = find_objective(0, g_nCurrentNavPoint_004931bc);

    if (objective != -1) {
        if (g_aMissionObjectives_004932a8[
                g_abFlightPath_004932a0[objective]].type != 1)
            flag_objective(objective, 1);
        if (g_cCurrentObjective_004931cc == objective)
            set_next_destination();
    }
    place_ship_near_player_until_valid(obj, 2000, 5000);
    unwarp(obj);
    g_anShipSpeed_0059b320[obj] =
        (int)g_asShipMaximumSpeed_0059c440[obj] << 8;
    fix_velocity(obj);
    if (g_asShipSide_004955d0[obj] == SIDE_IMPERIAL)
        reset_mission_type(obj, MISSION_TYPE_COME_HOME);
    else
        reset_mission_type(obj, MISSION_TYPE_PATROL);
}

/* Function start: 0x424C05 */
unsigned int unwarp(short obj)
{
    short effect;

    ClearViewport(&g_stViewBuffer_005d2b00, g_cViewportClearColour_004699a0);
    g_bViewportDirty_0049d76c = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
        g_aShipPosition_00494550[effect] = g_aShipPosition_00494550[obj];
        g_aShipVelocity_0059c010[effect] = g_aShipVelocity_0059c010[obj];
        g_asShipManeuver_00495f48[obj] = MANEUVER_NONE;
        g_asObjectCounter_00494be0[obj] = 6;
        return 0;
    }
    g_abShipNavPointIndex_00495f60[obj] =
        (signed char)g_acObjectType_00493980[obj];
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
    return 0;
}

/* Function start: WC2_UNMAPPED */
unsigned int warp(short obj)
{
    short effect;

    ClearViewport(&g_stViewBuffer_005d2b00, g_cViewportClearColour_004699a0);
    g_bViewportDirty_0049d76c = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                         g_acObjectOwner_00495208[obj]);
        g_aShipPosition_00494550[effect] = g_aShipPosition_00494550[obj];
        g_aShipVelocity_0059c010[effect] = g_aShipVelocity_0059c010[obj];
        g_asShipManeuver_00495f48[obj] = MANEUVER_WARPING_OUT;
        g_asObjectCounter_00494be0[obj] = 6;
        return 0;
    }
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                     g_acObjectOwner_00495208[obj]);
    return 0;
}

/* Function start: WC2_UNMAPPED */
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
    for (; weaponCount > weapon; weapon++) {
        weaponSlot = (ShipWeaponSlot *)
            ((unsigned char *)g_aShipWeapons_0059cab0 + loadoutOffset + 1) +
            weapon;
        type = weaponSlot->type;

        if (g_aObjectTypeData_00496d30[type].objectClass ==
                OBJECT_CLASS_MINE &&
            weaponSlot->disabled == 0)
            return drop_mine(obj, (signed char)weapon, type, 20);
    }
    return -1;
}

/* Function start: WC2_UNMAPPED */
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
    g_nMissionScore_00493462 += 25;
    return 0;
}

/* Function start: WC2_UNMAPPED */
void clean_up_cockpit(void)
{
    short wingman = g_nYourWingman_0049346c;

    g_acShipTarget_00495f20[0] = -1;
    g_nTargetLockMode_0046c078 = 0;
    if (wingman != -1) {
        g_nAutoEngageTimer_00496130 = -1;
        g_acShipTarget_00495f20[wingman] = -1;
        reset_objective(wingman, OBJECTIVE_HOLD_FORMATION);
    }
    ClearHudGunReadouts();
}

/* Function start: WC2_UNMAPPED */
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

            if (g_aObjectTypeData_00496d30[type].objectClass ==
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

/* Function start: WC2_UNMAPPED */
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
            if (g_aObjectTypeData_00496d30[weaponSlot->type].objectClass ==
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

/* Function start: 0x46166D */
unsigned int select_new_gun(void)
{
    g_eSelectedGunType_0046c054 = (enum ObjectType)select_guns(
        0, find_next_gun(0, g_eSelectedGunType_0046c054));
    if (get_mode(0) == 1)
        InvalidateVduMode(0);
    return 0;
}

/* Function start: 0x4616B8 */
unsigned int select_new_release_weapon(enum ObjectType preferredType)
{
    signed char weaponCount;
    int currentWeapon;
    signed char firstWeapon;
    signed char weapon;
    ShipWeaponSlot *weaponSlots;

    weaponCount = (signed char)g_aShipWeapons_0059cab0[0][0];
    currentWeapon = g_nSelectedReleaseWeaponIndex_004934e0;
    weapon = (signed char)(currentWeapon + 1);
    weaponSlots = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];

    if (weaponCount <= weapon)
        weapon = 0;
    if (currentWeapon == -1) {
        if (preferredType != -1) {
            weapon = 0;
            if (weaponCount > 0) {
                for (; weapon <
                           (signed char)g_aShipWeapons_0059cab0[0][0];
                     weapon++) {
                    if (weaponSlots[weapon].type == preferredType) {
                        currentWeapon = weapon;
                        weaponSlots[currentWeapon].disabled = 0;
                        break;
                    }
                }
            }
        }
        g_nSelectedReleaseWeaponIndex_004934e0 = currentWeapon;
        if (currentWeapon == -1) {
            weapon = 0;
            if (weaponCount > 0) {
                for (; weapon < weaponCount; weapon++) {
                    if (g_aObjectTypeData_00496d30[
                            weaponSlots[weapon].type].objectClass !=
                            OBJECT_CLASS_PROJECTILE) {
                        currentWeapon = weapon;
                        g_nSelectedReleaseWeaponIndex_004934e0 = currentWeapon;
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
            if (g_aObjectTypeData_00496d30[type].objectClass !=
                    OBJECT_CLASS_PROJECTILE &&
                weaponSlots[currentWeapon].type != type) {
                weaponSlots[currentWeapon].disabled = 1;
                currentWeapon = weapon;
                g_nSelectedReleaseWeaponIndex_004934e0 = currentWeapon;
                weaponSlots[currentWeapon].disabled = 0;
                break;
            }
            weapon++;
            if (weaponCount <= weapon)
                weapon = 0;
        } while (weapon != firstWeapon);
    }
    if (get_mode(0) == 1)
        InvalidateVduMode(0);
    return 0;
}

/* Function start: WC2_UNMAPPED */
void WaitForDebugStep(void)
{
    while (TakeDebugStepFlag() == 0) ;
}

/* Function start: 0x40A2D0 */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message,
                                 DWORD user, DWORD first, DWORD second)
{
    (void)timerId;
    (void)message;
    (void)user;
    (void)first;
    (void)second;
    g_nFrameTimerPending_005c844c = 0;
}

/* Function start: 0x40A2E7 */
void SetMultimediaTimerCallback(int period)
{
    int milliseconds = period * 1000 / 60;

    if (period == 0) {
        g_nFrameTimerPending_005c844c = 0;
        if (g_uFrameTimerEventId_005d3bec != 0) {
            timeKillEvent(g_uFrameTimerEventId_005d3bec);
            g_uFrameTimerEventId_005d3bec = 0;
        }
    } else {
        if (g_uFrameTimerEventId_005d3bec != 0) {
            timeKillEvent(g_uFrameTimerEventId_005d3bec);
            g_uFrameTimerEventId_005d3bec = 0;
        }
        g_nFrameTimerPending_005c844c = 1;
        g_uFrameTimerEventId_005d3bec = timeSetEvent(milliseconds, milliseconds,
                                    FrameTimerCallback, 0, 0);
    }
}
