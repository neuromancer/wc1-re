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
                   ((char)bMessageSpeed + 1));
}

/* Function start: 0x428EA0 */
void WaitForKeyAcknowledge(int mode)
{
    int acknowledged;
    int key;

#ifdef WC1_SDL
    /* Every modal wait blocks here, including both pause paths: Ctrl+P
     * calls this directly and P reaches it through ShowOnScreenMessage's
     * 9999 duration.  Free the pointer for the duration of the wait. */
    Wc1SdlSuspendMouseGrab();
#endif
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
#ifdef WC1_SDL
        Wc1SdlResumeMouseGrab();
#endif
        return;
    }
    FlushInputEvents();
    ClearDebugPauseFlags();
    do {
        key = PumpMessagesDuringWait();
    } while (key == 0x19 || key == 0x50 || key == 0x0c);
    FlushInputEvents();
#ifdef WC1_SDL
    Wc1SdlResumeMouseGrab();
#endif
}

/* Function start: 0x428F20 */
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
        SetHudTextColour(1);
        DosStrcpy(szHudMessageBuffer, text);
        SetHudMessageText(szHudMessageBuffer,
                          cRedColour, messageDuration);
        if (messageDuration == 9999) {
            ShowHudTextLine(szHudMessageBuffer,
                            (unsigned short)cRedColour);
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
        SetHudMessageText("", cRedColour, 2);
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
                        pGameVersion);
}

/* Function start: 0x4290F0 */
void SetMessageDisplaySpeed(void)
{
    bMessageSpeed =
        (unsigned char)(((signed char)bMessageSpeed + 1) % 5);
    ShowOnScreenMessage(0, 0, "MESSAGES SPEED IS NOW %d.",
                        (signed char)bMessageSpeed + 1);
}

/* Function start: 0x429120 */
void ReportFramesSkipped(short adjustment)
{
    nFrameSkip = MinShort(
        MaxShort((short)(nFrameSkip + adjustment), 1), 5);
    ShowOnScreenMessage(0, 0, "%d FRAMES SKIPPED.",
                        nFrameSkip - 1);
}

/* Function start: 0x429160 */
int HandleSpaceFlightControls(void)
{
    int notRepeated;
    int control;

    player_input();
    players_flight_dynamics();
    notRepeated = (signed char)bCurrentKey !=
                  cPreviousKey;
    control = GetControlKeyState();
    GetKeyboardModifiers();
    HandleFleetOverviewInput();

    if (nTrainSimActive == 0) {
        switch ((signed char)bCurrentKey) {
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
                nCameraViewMode == 0 &&
                (signed char)bCurrentKey >= 2 &&
                (signed char)bCurrentKey <=
                    nCommMenuChoiceCount + 2 &&
                get_mode(1) == 4) {
                Chosen_communicate_option(
                    (short)((signed char)bCurrentKey - 2));
            }
            break;
        case 0x12:
            if (notRepeated &&
                (control != 0 ||
                 (wCurrentInputModifiers & 0x2000) != 0) &&
                acPlayerComponentDamage[7] != 4) {
                if (RandomInRange(
                        0, acPlayerComponentDamage[7]) == 0)
                    nArcadeState = 2;
                else
                    malf_sound();
            }
            break;
        case 0x1e:
            if (notRepeated) {
                bMouseCursorVisible = 0;
                if (get_mode(1) != 5)
                    SelectCockpitVduMode(1, 5);
                if (bCockpitlessView == 0) {
                    auto_pilot_sequence();
                } else {
                    GetScreenUpdateFlag();
                    SetViewportRect(
                        &stSpaceBuffer, 0, 0,
                        (short)(nScreenWidth - 1),
                        (short)(nScreenHeight - 1));
                    initialize_view_buffer();
                    bCockpitlessView = -2;
                    auto_pilot_sequence();
                    bCockpitlessView = 1;
                    GetScreenUpdateFlag();
                    SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
                    initialize_view_buffer();
                }
                FlushInputEvents();
                ClearDebugPauseFlags();
                bMouseCursorVisible = 0;
                bPointerMovedByKeyboard = 1;
            }
            break;
        case 0x1f:
            if (control != 0 && notRepeated) {
                if (nFlightSoundEffectsEnabled == 1)
                    ResetSoundStateForScene();
                else
                    ResetSoundStateForFlight();
            }
            break;
        case 0x23:
            if (notRepeated && nYourWingman != -1 &&
                aeShipObjective[nYourWingman] !=
                    OBJECTIVE_HOLD_FORMATION) {
                request(0, nYourWingman, 9);
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
                bVideoImagesSuppressed =
                    bVideoImagesSuppressed == 0;
                if (bVideoImagesSuppressed != 0)
                    SetHudMessageText("VIDEO IMAGES SUPRESSED",
                                      cRedColour, 20);
                else
                    SetHudMessageText("VIDEO IMAGES ENABLED",
                                      cPrimaryTextColour, 20);
            }
            break;
        case 0x30:
            if (notRepeated && nYourWingman != -1 &&
                aeShipObjective[nYourWingman] ==
                    OBJECTIVE_HOLD_FORMATION &&
                any_enemy(0, 14000) != 0) {
                request(0, nYourWingman, 7);
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
                if (cScreenViewportMode == 0) {
                    bCockpitlessView = bCockpitlessView == 0;
                    GetScreenUpdateFlag();
                    if (bCockpitlessView != 0) {
                        SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
                        initialize_cockpit(
                            cScreenViewportMode++);
                        SetMousePosition(
                            (short)(((int)stSpaceBuffer.right -
                                     (int)stSpaceBuffer.left) / 2 + 1),
                            nViewCenterY);
                    } else {
                        SetViewportRect(
                            &stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
                        initialize_cockpit(
                            cScreenViewportMode++);
                        SetMousePosition(
                            (short)(((int)stSpaceBuffer.right -
                                     (int)stSpaceBuffer.left) / 2 + 1),
                            (short)(((int)stSpaceBuffer.bottom -
                                     (int)stSpaceBuffer.top) / 2));
                    }
                } else {
                    bMouseCursorVisible = 0;
                    if (bCockpitlessView == 0) {
                        new_view(0, 0);
                    } else {
                        GetScreenUpdateFlag();
                        SetViewportRect(
                            &stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
                        initialize_view_buffer();
                        new_view(0, 0);
                        GetScreenUpdateFlag();
                        SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
                        initialize_view_buffer();
                    }
                }
                FlushInputEvents();
                ClearDebugPauseFlags();
                bMouseCursorVisible = 0;
                bPointerMovedByKeyboard = 1;
            }
            break;
        case 0x3c:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(2, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(2, 0);
            goto restore_normal_viewport;
        case 0x3d:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(1, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(1, 0);
            goto restore_normal_viewport;
        case 0x3e:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(3, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(3, 0);
            goto restore_normal_viewport;
        case 0x3f:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(4, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(4, 0);
            goto restore_normal_viewport;
        case 0x40:
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(14, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(14, 0);
            goto restore_normal_viewport;
        case 0x41:
            if (acShipTarget[0] == -1)
                break;
            bMouseCursorVisible = 0;
            if (bCockpitlessView == 0) {
                new_view(7, 0);
                break;
            }
            GetScreenUpdateFlag();
            SetViewportRect(&stSpaceBuffer, 0, 0,
                            (short)(nScreenWidth - 1),
                            (short)(nScreenHeight - 1));
            initialize_view_buffer();
            new_view(7, 0);
            goto restore_normal_viewport;
        case 0x42:
            if (notRepeated) {
                bMouseCursorVisible = 0;
                bMissileCameraEnabled ^= 1;
                if (bMissileCameraEnabled != 0)
                    SetHudMessageText("MISSILE CAMERA ON",
                                      cRedColour, 20);
                else
                    SetHudMessageText("MISSILE CAMERA OFF",
                                      cPrimaryTextColour, 20);
            }
            break;
        case 0x43:
            if (notRepeated) {
                bMouseCursorVisible = 0;
                SelectNextExternalViewObject();
                if (bCockpitlessView == 0) {
                    force_view(4, (short)cViewObject);
                    break;
                }
                GetScreenUpdateFlag();
                SetViewportRect(
                    &stSpaceBuffer, 0, 0,
                    (short)(nScreenWidth - 1),
                    (short)(nScreenHeight - 1));
                initialize_view_buffer();
                force_view(4, (short)cViewObject);
                goto restore_normal_viewport;
            }
            break;
        }
    }
    goto primary_controls_complete;

restore_normal_viewport:
    GetScreenUpdateFlag();
    SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
    initialize_view_buffer();

primary_controls_complete:
    switch ((signed char)bCurrentKey) {
    case 1:
        bEscapePressed = 0;
        if (nTrainSimActive != 0)
            return -1;
        if (get_mode(1) == 4) {
            CloseCommChoiceMenu();
            return 0;
        }
#ifdef WC1_SDL
        /* The original leaves Escape inert in flight, leaving no way to
         * halt a mission.  Pause on it, exactly as the pause key does. */
        ShowGamePausedBanner(1);
        SetFrameTimerPeriodDirect(1);
        return 0;
#endif
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
        anShipSpeed[0] = 0;
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
        if (notRepeated && nSelectedReleaseWeaponIndex != -1) {
            ShipWeaponSlot *slot =
                &((ShipWeaponSlot *)&aShipWeapons[0][1])[
                    nSelectedReleaseWeaponIndex];

            if (slot->type == OBJECT_TYPE_SPACE_MINE) {
                drop_player_mine(0);
                return 0;
            }
            if (nExternalViewShip == -1) {
                nExternalViewShip =
                    (short)fire_missile(0);
                if (bMissileCameraEnabled == 1 &&
                    nExternalViewShip != -1) {
                    new_view(6, nExternalViewShip);
                    return 0;
                }
            }
        }
        break;
    case 0x1f:
        if (control != 0 && notRepeated) {
            nSfxVolumeSetting =
                nSfxVolumeSetting == 0 ? 20 : 0;
            SetSoundEffectsVolume(
                anVolumeLevels[
                    nSfxVolumeSetting / 2]);
            ShowOnScreenMessage(0, 0, "SFX VOLUME: %d.",
                                nSfxVolumeSetting / 2);
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
        nTargetLockMode =
            (short)(nTargetLockMode == 0);
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
            nMusicVolumeSetting =
                nMusicVolumeSetting == 0 ? 20 : 0;
            SetMusicStreamVolume(
                (unsigned short)anVolumeLevels[
                    nMusicVolumeSetting / 2]);
            ShowOnScreenMessage(0, 0, "MUSIC VOLUME: %d.",
                                nMusicVolumeSetting / 2);
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
    nFrameSkipCounter--;
    if (nFrameSkipCounter > 0)
        return 0;
    nFrameSkipCounter = nFrameSkip;
    nRenderedSpaceFrame++;
    transform_objects_to_your_view();
    update_star_field();
    place_exhaust_on_ships();
    reposition_fixed_child_objects();
    sort_object_depth();
#ifdef WC1_SDL
    Wc1SdlBeginSpaceFrame(
        pScreenViewportGeometry,
        (int)cScreenViewportMode,
        bCockpitlessView > 0,
        (unsigned char)cPrimaryViewBufferColour);
#endif
    draw_sorted_objects_to_buffer();
    if (nCameraViewMode == 0)
        overlay_head_up_display();
    return 1;
}

/* Function start: 0x429E30 */
void GetArcadeBonus(void)
{
    nArcadeWaveBonus =
        (nArcadeTimeRemaining *
             (nTrainSimMission + 1) +
         (nTrainSimMission +
          (nArcadeWave * 5 + 5) * 2) * 50) * 2;
}

/* Function start: 0x429E70 */
void FigureArcadeTime(void)
{
    nArcadeTimeRemaining =
        (short)((nArcadeWave + 6) * 400);
}

/* Function start: 0x429E90 */
void DrawArcadeScorePanel(short x, short y)
{
    char score[20];

    sprintf(score, "%0ld", nArcadeScore);
    DrawFormattedText("%X%YScore: %s0 %XTime: %u %X1 UP",
                      x, y, score, x + 0x82,
                      nArcadeTimeRemaining, x + 0xbe);
}

/* Function start: 0x429EE0 */
void UpdateArcadeScoreDisplay(void)
{
    char bonus[20];

    if (nTrainSimActive != 0) {
        SetTextContext(&stHudMessageTextContext);
        DrawArcadeScorePanel(10, 10);
        if (nArcadeBonusCountdown < 1) {
            nArcadeScore++;
            nArcadeTimeRemaining--;
            if (nArcadeTimeRemaining < 1) {
                nArcadeState = 4;
                return;
            }
        } else {
            sprintf(bonus, "%0ld", nArcadeWaveBonus);
            SetTextCursor((unsigned short)stSpaceBuffer.left,
                          (unsigned short)((stSpaceBuffer.top +
                                            stSpaceBuffer.bottom) / 2 - 5));
            if (nCurrentWave != -1) {
                FormatTextBufferFromStart(
                    "Wave %d complete.\n\nBonus Points: %s0%P",
                    nArcadeWave + 1, bonus);
                return;
            }
            FormatTextBufferFromStart(
                "Mission %d complete.\n\nBonus Points: %s0%P",
                nTrainSimMission + 1, bonus);
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
    if (nCameraViewMode == 0)
        RestoreTransientCockpitGraphics();
    if (bCockpitlessView == 0 && nTrainSimActive != 0) {
        DrawFilledViewportRect(&stSpaceBuffer, 10, 10,
                               stSpaceBuffer.right, 0x11,
                               cPrimaryViewBufferColour);
        if (nArcadeBonusCountdown != 0) {
            nArcadeBonusCountdown--;
            if (nArcadeBonusCountdown == 0) {
                if (Vector_magnitude(
                        &aShipPosition[0]) > 0x271000)
                    zero_vector(&aShipPosition[0]);
                nArcadeScore += nArcadeWaveBonus;
                if (nCurrentWave == -1)
                    nArcadeState = 1;
                else
                    nArcadeWave++;
                ClearViewport(&stSpaceBuffer,
                              cPrimaryViewBufferColour);
            }
        }
    }
    ClearViewport(&stSpaceBuffer, cPrimaryViewBufferColour);
    return 1;
}

/* Function start: 0x42A0C0 */
unsigned int RefreshCockpitStatus(void)
{
    Update_3Space();
    if (nFrameSkipCounter <= 1)
        clear_view_buffer();
    return Draw_3Space_Frame();
}

/* Function start: 0x42A0E0 */
short GetShipDistanceToNavPoint(short ship, MissionNavPoint *navPoint)
{
    FixedVector delta;

    ComputeVectorDelta(&aShipPosition[ship],
                       &navPoint->position, &delta);
    return FixedToShortSaturating(Vector_magnitude(&delta));
}

/* Function start: 0x42A120 */
short FindNearestNavPoint(short ship)
{
    short navPointIndex = 0;
    MissionNavPoint *navPoint = aMissionNavPoints;

    do {
        if (navPoint->type == 1 &&
            GetShipDistanceToNavPoint(ship, navPoint) < navPoint->proximityRadius)
            return navPointIndex;
        navPointIndex++;
        navPoint++;
    } while (navPointIndex < WC1_ACTIVE_MISSION_NAV_POINT_COUNT);

    return nCurrentNavPoint;
}

/* Function start: 0x42A170 */
unsigned int ReleaseStaleNavTarget(void)
{
    short v = FindNearestNavPoint(0);

    if (nCurrentNavPoint != v)
        set_up_action_sphere(v);
    return 0;
}

/* Function start: 0x42A190 */
int RunSpaceFlight(short entryNavPoint)
{
    Viewport *savedViewport;
    signed char savedMode;
    unsigned int frameReady;

    bCockpitlessView = 0;
    if (nTrainSimActive == 0 &&
        bCockpitEnabled == 0)
        bCockpitlessView = 1;
    nFrameSkipCounter = 1;
    bInputMode = 1;
#ifdef WC1_SDL
    Wc1SdlSetMouseGrab(1);
#endif
    SetEventManagerPump(get_player_input);
    savedViewport = (Viewport *)stMouseCursorState.viewport;
    stMouseCursorState.viewport = &stSpaceBuffer;
    init_inflight_music();

    if (entryNavPoint == -1)
        entryNavPoint = aMissionShips[
            nPlayerMissionShipIndex].navPoint;
    set_up_action_sphere(entryNavPoint);

    if (bCockpitlessView != 0) {
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBuffer, 0, 0,
                        (unsigned short)(nScreenWidth - 1),
                        (unsigned short)(nScreenHeight - 1));
        initialize_view_buffer();
        new_view(0, 0);
        GetScreenUpdateFlag();
        SetViewportRect(&stSpaceBuffer, 0, 0, 319, 199);
        savedMode = cScreenViewportMode;
        cScreenViewportMode++;
        initialize_cockpit(savedMode);
        SetMousePosition(
            (stSpaceBuffer.right - stSpaceBuffer.left) / 2 + 1,
            nViewCenterY);
        bMouseAfterburnerControl = 0;
        bMouseCursorVisible = 0;
        initialize_view_buffer();
        FlushInputEvents();
    }

    copy_frame(0, 62);
    WarpMouseTo((short)((stSpaceBuffer.left +
                         stSpaceBuffer.right) / 2),
                (short)((stSpaceBuffer.top +
                         stSpaceBuffer.bottom) / 2));
    FlushInputEvents();
    bMouseAfterburnerControl = 0;
    bMouseCursorVisible = 0;
    nArcadeState = 0;
    DIBslam();
    DIBslamReal();
    SetSpaceFlightFrameTiming();
    FlushInputEvents();
    ClearDebugPauseFlags();
    bMouseCursorVisible = 0;
    bPointerMovedByKeyboard = 1;
    frameReady = 1;

    while (nArcadeState == 0) {
        ReadPerformanceCounter(&liFlightFrameStart);
        if (HandleSpaceFlightControls() == -1) {
            nArcadeState = 5;
            break;
        }
        ReadPerformanceCounter(&liFlightAfterInput);
        if (nArcadeState == 0) {
            Update_3Space();
            ReadPerformanceCounter(&liFlightAfterSimulation);
            frameReady = RenderSpaceViewFrame();
            ReadPerformanceCounter(&liFlightAfterRender);
            update_cockpit();
        } else {
            liFlightAfterRender =
                liFlightAfterSimulation =
                    liFlightAfterInput;
        }
        ReadPerformanceCounter(&liFlightAfterCockpit);
        if (frameReady != 0) {
            frameReady = 0;
            DIBslam();
            DIBslamReal();
        }
        ReadPerformanceCounter(&liFlightFrameEnd);
        nFlightPresentTicks =
            (int)(liFlightFrameEnd.LowPart -
                  liFlightAfterCockpit.LowPart);
        nFlightCockpitTicks =
            (int)(liFlightAfterCockpit.LowPart -
                  liFlightAfterRender.LowPart);
        nFlightRenderTicks =
            (int)(liFlightAfterRender.LowPart -
                  liFlightAfterSimulation.LowPart);
        nFlightSimulationTicks =
            (int)(liFlightAfterSimulation.LowPart -
                  liFlightAfterInput.LowPart);
        nFlightFrameTotalTicks =
            (int)(liFlightFrameEnd.LowPart -
                  liFlightFrameStart.LowPart);
        nFlightInputTicks =
            (int)(liFlightAfterInput.LowPart -
                  liFlightFrameStart.LowPart);
        DAT_00598888 = 0;
        DAT_0059888c = 0;
        DAT_00598890 = 0;
    }

#ifdef WC1_SDL
    Wc1SdlSetMouseGrab(0);
    Wc1SdlCancelSpaceFrame();
    if (Wc1SdlUsingDosData())
        Wc1SdlStopDosSoundEffects();
#endif
    SetCinematicFrameTiming();
    SetViewportRect(&stSpaceBuffer, 0, 0,
                    (unsigned short)(nScreenWidth - 1),
                    (unsigned short)(nScreenHeight - 1));
    bCockpitlessView = 0;
    if (nArcadeState == 1)
        flag_objective(find_objective(1, -1), 2);
    bCockpitlessView = 0;
    ResetCockpitPaletteEntries();
    stMouseCursorState.viewport = savedViewport;
    free_inflight_music();
    SetEventManagerPump(0);
    bMouseCursorVisible = 0;
    QueueInputEvent(13, 160, 100, 0, 0, 0, 0);
    SetMouseCursorShape(stMouseCursorState.shape, 0);
    return nArcadeState;
}

/* Function start: 0x42A520 */
int calculate_damage_level(void)
{
    ObjectTypeData *typeData;
    short damage;

    typeData = &aObjectTypeData[aeObjectType[0]];
    damage = (short)(((typeData->armorLeft -
                       aasShipArmor[0][2]) * 4) /
                     typeData->armorLeft);
    damage = (short)(damage +
        ((typeData->armorRear - aasShipArmor[0][1]) * 4) /
            typeData->armorRear);
    damage = (short)(damage +
        ((typeData->armorRight - aasShipArmor[0][3]) * 4) /
            typeData->armorRight);
    damage = (short)(damage +
        ((typeData->armorFront - aasShipArmor[0][0]) * 4) /
            typeData->armorFront);
    damage = (short)(
        (acShipDamage[0] * 30) / typeData->damageCapacity +
        damage * 2);
    damage = (short)(damage +
                     asShipAccumulatedDamage[0] * 5);

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
    mouseX = stMouseCursorState.x;
    mouseY = stMouseCursorState.y;
    region = aTrainSimMissionRegions;
    while (region->frame != -1) {
        state = IsPointInRect(mouseX, mouseY, &region->left);
        if (state != 0)
            frame = region->frame;
        region++;
    }
    SetMouseCursorShape(stMouseCursorState.shape, frame);
}

/* Function start: 0x42A670 */
void ResetMouseCursorFrame(void)
{
    SetMouseCursorShape(stMouseCursorState.shape, 0);
}

/* Function start: 0x42A680 */
void UpdateRoomMenuCursor(void)
{
    short mouseX;
    short mouseY;
    short state;
    short frame;
    TitleMenuRegion *region;
    short index;

    mouseX = stMouseCursorState.x;
    frame = nRoomMenuCursorFrame;
    index = 0;
    region = pRoomMenuRegions;
    mouseY = stMouseCursorState.y;
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
    SetMouseCursorShape(stMouseCursorState.shape, frame);
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
    if (nVideoMode != 0x13)
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
            abPaletteTriplets[indices[index]][0] =
                (unsigned char)currentPalette[index * 3];
            abPaletteTriplets[indices[index]][1] =
                (unsigned char)currentPalette[index * 3 + 1];
            abPaletteTriplets[indices[index]][2] =
                (unsigned char)currentPalette[index * 3 + 2];
        }
        SetWholePaletteFromTriplets(&abPaletteTriplets[0][0]);
    }

    ReleasePacketHandle(targetPalette);
    ReleasePacketHandle(currentPalette);
    ReleasePacketHandle(indices);
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x42A8F0 */
short find_objective(int type, short index)
{
    short objective;

    objective = 0;
    while (objective < (short)cMissionObjectiveCount) {
        if (aMissionObjectives[objective].type == type) {
            if (index == -1)
                return objective;
            if (aMissionObjectives[objective].index == index)
                return objective;
        }
        objective++;
    }
    return -1;
}

/* Function start: 0x42A950 */
void arrive_from_warp(short obj)
{
    short objective = find_objective(0, nCurrentNavPoint);

    if (objective != -1) {
        if (aMissionObjectives[
                abFlightPath[objective]].type != 1)
            flag_objective(objective, 1);
        if (cCurrentObjective == objective)
            set_next_destination();
    }
    approve_xyz(obj, 2000, 5000);
    unwarp(obj);
    anShipSpeed[obj] =
        (int)asShipMaximumSpeed[obj] << 8;
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

    ClearViewport(&stSpaceBuffer, cViewportClearColour);
    bViewportDirty = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
        aShipPosition[effect] = aShipPosition[obj];
        aShipVelocity[effect] = aShipVelocity[obj];
        aeShipManeuver[obj] = MANEUVER_NONE;
        asObjectCounter[obj] = 6;
        return 0;
    }
    abShipNavPointIndex[obj] =
        (signed char)aeObjectType[obj];
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH, obj);
    return 0;
}

/* Function start: 0x42AAF0 */
unsigned int warp(short obj)
{
    short effect;

    ClearViewport(&stSpaceBuffer, cViewportClearColour);
    bViewportDirty = 1;
    effect = find_vacant_3d_object();
    if (effect != -1) {
        set_objects_data(effect, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                         acObjectOwner[obj]);
        aShipPosition[effect] = aShipPosition[obj];
        aShipVelocity[effect] = aShipVelocity[obj];
        aeShipManeuver[obj] = MANEUVER_WARPING_OUT;
        asObjectCounter[obj] = 6;
        return 0;
    }
    set_objects_data(obj, OBJECT_TYPE_HYPERSPACE_JUMP_FLASH,
                     acObjectOwner[obj]);
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
    loadoutOffset = (int)obj * sizeof(aShipWeapons[0]);
    weaponCount = *(signed char *)
        ((unsigned char *)aShipWeapons + loadoutOffset);
    for (; weaponCount > weapon; weapon++) {
        weaponSlot = (ShipWeaponSlot *)
            ((unsigned char *)aShipWeapons + loadoutOffset + 1) +
            weapon;
        type = weaponSlot->type;

        if (aObjectTypeData[type].objectClass ==
                OBJECT_CLASS_MINE &&
            weaponSlot->disabled == 0)
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
    short wingman = nYourWingman;

    acShipTarget[0] = -1;
    nTargetLockMode = 0;
    if (wingman != -1) {
        nAutoEngageTimer = -1;
        acShipTarget[wingman] = -1;
        reset_objective(wingman, OBJECTIVE_HOLD_FORMATION);
    }
    ClearHudGunReadouts();
}

/* Function start: 0x42AD00 */
short find_next_gun(short obj, enum ObjectType currentGun)
{
    unsigned char *loadout = aShipWeapons[obj];
    int foundCurrent = 0;
    short weapon = 0;
    short firstGun = -1;
    short weaponCount = (signed char)loadout[0];
    ShipWeaponSlot *weaponSlot = (ShipWeaponSlot *)&loadout[1];

    if (weaponCount > 0) {
        do {
            enum ObjectType type = weaponSlot->type;

            if (aObjectTypeData[type].objectClass ==
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
    weaponCount = (signed char)aShipWeapons[0][0];
    found = 0;
    weaponSlot = (ShipWeaponSlot *)&aShipWeapons[0][1];
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
    if (get_mode(0) == 1)
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

    weaponCount = (signed char)aShipWeapons[0][0];
    currentWeapon = nSelectedReleaseWeaponIndex;
    weapon = (signed char)(currentWeapon + 1);
    weaponSlots = (ShipWeaponSlot *)&aShipWeapons[0][1];

    if (weaponCount <= weapon)
        weapon = 0;
    if (currentWeapon == -1) {
        if (preferredType != -1) {
            weapon = 0;
            if (weaponCount > 0) {
                for (; weapon <
                           (signed char)aShipWeapons[0][0];
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
    if (get_mode(0) == 1)
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
    bFrameTickPending = 0;
}

/* Function start: 0x42AFC0 */
void SetMultimediaTimerCallback(int period)
{
    int milliseconds = period * 1000 / 60;

    if (period == 0) {
        bFrameTickPending = 0;
        if (nFrameTimerId != 0) {
            timeKillEvent(nFrameTimerId);
            nFrameTimerId = 0;
        }
    } else {
        if (nFrameTimerId != 0) {
            timeKillEvent(nFrameTimerId);
            nFrameTimerId = 0;
        }
        bFrameTickPending = 1;
        nFrameTimerId = timeSetEvent(milliseconds, milliseconds,
                                    FrameTimerCallback, 0, 0);
    }
}
