/*
 *  Prototypes for every reimplemented function, in address order.
 *
 *  Address order is link order, so this list doubles as the reconstruction
 *  inventory: see src/map for the same list with the original addresses.
 */
#ifndef WC1_FUNCS_H
#define WC1_FUNCS_H

void RestoreGamePalette(void);                                          /* 0x00401020 */
unsigned int GetSfxDistanceFromCamera(void);                                        /* 0x00401250 */
void PickRandomTauntDelay(void);                                               /* 0x00401270 */
short SumShortArray(short *p, short n);                                 /* 0x00401390 */
void TransformObjectVector(int p, int *q);                                      /* 0x00401680 */
void TransformObjectVectorAlt(int p, int *q);                                      /* 0x004016A0 */
void PlayEngineRumble(void);                                                /* 0x00401870 */
void ClearShipMode4(short i);                                     /* 0x00401930 */
void WarpMouseTo(short x, short y);                                  /* 0x00401CE0 */
void ShowNoticeMessageBox(const char *text);                            /* 0x004020E0 */
unsigned int GetWindowHandleWord(void);                                     /* 0x00402520 */
short GetJoystickButtons(void);                                                   /* 0x00402AC0 */
unsigned int GetMouseButtonState(void);                                     /* 0x00402B80 */
void SetMousePosition(int x, int y);                                    /* 0x00402E80 */
int GetShiftKeyState(void);                                             /* 0x00403060 */
int GetControlKeyState(void);                                           /* 0x00403070 */
unsigned int GetKeyboardModifiers(void);                                     /* 0x00403080 */
unsigned int GetGameClockTicks(void);                                   /* 0x00403090 */
void InitGameClockEpoch(void);                                    /* 0x004030B0 */
void WriteDebugString(const char *s);                                   /* 0x004030D0 */
void SetMousePositionDuplicate(int x, int y);                                 /* 0x004030E0 */
short OpenDataFileOrDie(const char *path);                                     /* 0x004034D0 */
void CloseDataFile(unsigned int fd);                                       /* 0x00403500 */
void SoundDebugPrintf(const char *fmt, ...);                          /* 0x00403DB0 */
void ReadPerformanceCounter(LARGE_INTEGER *p);                           /* 0x00403E30 */
void ResetStringBuilder(int p);                                               /* 0x00403E40 */
unsigned int GetNavSystemId(short i);                             /* 0x00403EE0 */
void SetNavSystemId(short i, short v);                             /* 0x00403F10 */
void SetShipAiScratchWord(unsigned short v);                                 /* 0x004060A0 */
void SelectNewShipAiBehavior(short ship);                               /* 0x004060B0 */
void ShipAiRoutine01(short ship);                                       /* 0x004061E0 */
void ShipAiState30(short ship, short target);                           /* 0x00407030 */
void ShipAiState31(short ship, short target);                           /* 0x004070D0 */
void ShipAiState24(short ship, unsigned int arg);                       /* 0x00407350 */
void ShipAiState34(short ship, unsigned int arg);                       /* 0x00407370 */
void ShipAiState44(short ship);                                         /* 0x00407560 */
void ShipAiState27(short ship, short target);                           /* 0x00407580 */
void ShipAiState21(short ship);                                         /* 0x004075A0 */
void ShipAiStateNoOp(void);                                               /* 0x004075B0 */
void ShipAiState03(short ship);                                         /* 0x004075C0 */
void ClearShipTarget(short a);                                            /* 0x004098C0 */
void DisableInputCapture(void);                                                  /* 0x00409CE0 */
void EnableInputCapture(void);                                                 /* 0x00409CF0 */
void ClearNavSelection(short i);                                     /* 0x0040A130 */
void UpdateShipAiVector(short ship);                                       /* 0x0040A160 */
void RunShipAiDispatch(short a);                                            /* 0x0040A400 */
void PromoteShipAiState15(short i);                                     /* 0x0040B110 */
unsigned int GetShipMode(short i);                             /* 0x0040B700 */
void ResetNavCursor(void);                                             /* 0x0040BEA0 */
unsigned int GetObjectVisible(void);                                        /* 0x0040C350 */
void SetNavCursorIndex(unsigned short v);                                /* 0x0040CBB0 */
void ObjectDrawHook(short *p);                                          /* 0x0040CBC0 */
void ClearNavHazardFlag(void);                                           /* 0x0040D1D0 */
void ClearNavLegendFlag(void);                                                 /* 0x0040D240 */
void SetScreenClipRect(unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x0040D8C0 */
void FormatNavCoordinates(unsigned char *out);                                 /* 0x0040DE70 */
char *GetNavNameSkippingMarker(short i);                                         /* 0x0040DF50 */
void NudgeObjectX(short i, short dx, short dy);                 /* 0x0040EFE0 */
unsigned short GetPaletteReadyUnused(void);                                    /* 0x0040FA30 */
void ApplyPitchFromFloat(void);                                               /* 0x00410AD0 */
unsigned int ResetCommTarget(void *p, unsigned int arg);                     /* 0x004117B0 */
unsigned short TickShipAiCountdown(short ship);                             /* 0x00412410 */
int HasFreeMessageSlot(void);                                              /* 0x00413D20 */
unsigned int GetSeriesRecordField(char slot, int rec);                       /* 0x00413F70 */
void ClearHudMessageIfMatching(int *p, int v);                                      /* 0x004141B0 */
void ClearHudGunReadouts(void);                                             /* 0x004141D0 */
void ShowHudMessageIfCurrent(int v);                                            /* 0x004142E0 */
void *ClearHudTargetVectors(void);                                            /* 0x00414410 */
unsigned short GetSeriesStateWord(short i);                           /* 0x004147E0 */
int GetSeriesFlag(short i);                                      /* 0x00414890 */
void ClearSeriesFlagEntry(short i);                                          /* 0x004148E0 */
void ClearMessageSlot(short i);                                          /* 0x004149C0 */
void ClearAutopilotFlag(void);                                              /* 0x004149E0 */
int IsAutopilotEngaged(void);                                              /* 0x004149F0 */
unsigned short SetAutopilotFlag(unsigned short v);                        /* 0x00414A10 */
void RefreshAutopilotHud(void);                                             /* 0x00414A20 */
void PlayTargetLockSfx(void);                                           /* 0x00414AD0 */
void PlayShieldHitSfx(void);                            /* 0x00414AE0 */
void InputFilterHook(void);                                            /* 0x00415040 */
int GetNavRecordField50(short i);                                      /* 0x00415050 */
int GetNavRecordField70(short i);                                              /* 0x00415070 */
int GetNavRecordField90(short i);                                      /* 0x00415090 */
void SetNavRecordFlags(short i, unsigned char bits);                 /* 0x004150B0 */
unsigned int GetShipAiScratch(void);                                     /* 0x00415510 */
void ClearWeaponHardpoints(void);                                            /* 0x00415A70 */
void BeginMissileLockWarning(unsigned short v);                                 /* 0x00415FC0 */
unsigned int GetHudMessageSlot(unsigned short v);                         /* 0x00415FF0 */
void EndMissileLockWarning(void);                                             /* 0x00416010 */
void SetRectBounds(int p, unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x00416220 */
short GetRectHeight(int p);                                             /* 0x00416250 */
void ShowHudTextLine(char *s, unsigned char b);                          /* 0x00416460 */
void SetHudTextColour(short v);                                              /* 0x00416480 */
void ReleaseCurrentTargetLock(void);                                                 /* 0x004168A0 */
void DrawHudMessagesIfEnabled(void);                                             /* 0x00416C90 */
void RefreshDamageDisplay(void);                                             /* 0x004173C0 */
void ForceRefreshDamageDisplay(void);                                             /* 0x004173F0 */
void ClearHudTargetBox(void);                                             /* 0x00417610 */
void ShowDamageMessage(short a);                                       /* 0x00417B10 */
void PlayMissileLaunchSfx(void);                            /* 0x00417F00 */
unsigned short GetMusicDriverPresent(void);                                    /* 0x00418130 */
void AddShipAiTimer(short i, short delta);                        /* 0x00418280 */
int MinInt(int a, int b);                                        /* 0x004184E0 */
int MaxInt(int a, int b);                                        /* 0x004184F0 */
int AbsInt(int v);                                               /* 0x00418500 */
unsigned short SignShort(short v);                                  /* 0x00418520 */
unsigned int SignFixed(int v);                                      /* 0x00418540 */
short WrapDegrees(short degrees);                                     /* 0x00418560 */
void ZeroVector(unsigned int *p);                                    /* 0x004185F0 */
void ZeroVectorPtr(int *p);                                             /* 0x00418600 */
void ScaleVectorByZero(short a, int *p);                                    /* 0x004187E0 */
short GetShipVectorComponent(short a, short i);                           /* 0x00419260 */
void StoreShipVector(short a, int *p);                                    /* 0x00419390 */
void SetShipAiOrder(short a, short b);                                   /* 0x004193B0 */
void ReadShipRotationRow(short a, short i);                              /* 0x004196A0 */
void WriteShipRotationRow(short a, short i);                              /* 0x00419850 */
void SetVectorFixedPoint(unsigned int *p, short v);                           /* 0x00419970 */
short FindShipInMode1(void);                                      /* 0x00419B70 */
short FindShipInMode2(void);                                      /* 0x00419BA0 */
void ClampVectorTo30(short *p);                                           /* 0x0041A0F0 */
void ClampTo30(short *p);                                           /* 0x0041A110 */
short MinShort(short a, short b);                                       /* 0x0041D0C0 */
short MaxShort(short a, short b);                                       /* 0x0041D0E0 */
void FreePacketAndClear(int *p);                                        /* 0x0041D100 */
unsigned int GetZeroUnused(void);                                        /* 0x0041DA00 */
void WaitForStreamIdle(void);                                       /* 0x0041DEB0 */
short FindActiveShipSlot(void);                                      /* 0x0041DF40 */
unsigned int ShipAiRoutine08(short ship);                               /* 0x0041E7C0 */
void ShipAiRoutine09(void);                                             /* 0x0041E7F0 */
unsigned int ReportShieldHit(void);                                       /* 0x0041F5D0 */
short RandomlyNegate(short v);                                           /* 0x004208C0 */
unsigned int ShipAiRoutine15(short ship);                               /* 0x004213B0 */
unsigned int FlagShipAiIfPositive(short i);                             /* 0x00421760 */
unsigned int LeaveWaitCursorScope(void);                                         /* 0x00421A40 */
unsigned int GetFxDriverInitResult(void);                                      /* 0x00421FE0 */
unsigned int GetMessagePumpResult(void);                                      /* 0x00421FF0 */
unsigned int GetFxDriverStatus(void);                                      /* 0x00422000 */
int TestShipFlags(short i, unsigned char bits);                  /* 0x00422010 */
void ClearShipFlags(short i, unsigned char bits);                 /* 0x00422030 */
void SetShipFlags(short i, unsigned char bits);                 /* 0x00422050 */
void ClearShipTimer(short i);                                            /* 0x004220D0 */
short RandomCentred(short range);                                       /* 0x004220F0 */
int ShipAiRoutine16(short ship, unsigned int bits);                     /* 0x00422110 */
unsigned int HasSpeechBuffer(void);                                      /* 0x00422130 */
unsigned int SetShipStateBits(short i, unsigned int bits);               /* 0x00422140 */
void ResetShipStateRecord(short i);                                     /* 0x00422160 */
void ClearWingmanSlots(void);                                       /* 0x00422440 */
unsigned int ShipAiRoutine19(short ship);                               /* 0x00422830 */
unsigned int SetShipOrderWord(short i, unsigned int v);             /* 0x00422CD0 */
unsigned int ToggleShipOrderWord(short i, unsigned int v);              /* 0x00422D00 */
void ShipAiRoutine20(short ship, short state);                          /* 0x00422D30 */
unsigned int ClearShipAiThrottle(short ship);                                 /* 0x00422DD0 */
unsigned int SetShipAiThrottleHalf(short ship);                                 /* 0x00422DF0 */
unsigned int ShipAiRoutine22(short ship);                               /* 0x00422E50 */
unsigned int ShipAiRoutine23(short ship);                               /* 0x00422E80 */
unsigned int SetShipAiThrottleFromTable(short ship, short i);                 /* 0x00422EA0 */
unsigned int IsBehindAndFalling(short ship);                               /* 0x00422F60 */
unsigned int GetObjectRecordField(short i);                             /* 0x00423610 */
void IssueShipAiOrder21(short a);                                          /* 0x00423CD0 */
unsigned int GetPilotNameLength(void);                                     /* 0x00424B80 */
unsigned int GetPilotRecordBase(void);                                  /* 0x00424BA0 */
unsigned int ResetSceneFlags(void);                                      /* 0x00424C60 */
unsigned int MouseHide(void);                                      /* 0x00424CE0 */
unsigned int MouseShow(void);                                      /* 0x00424CF0 */
void SceneEnterHook(void);                                            /* 0x00425AF0 */
void GetMessagePumpInterval(void);                                          /* 0x00425B90 */
void SystemDebugPrintf(const char *s);                                      /* 0x00425BB0 */
void PumpMessagesDuringWait(void);                                          /* 0x00425BC0 */
unsigned char TakeDebugStepFlag(void);                                     /* 0x00425BD0 */
void ResetDiskPromptTimer(void);                                             /* 0x00425BE0 */
void DiskPromptDrawHook(void);                                             /* 0x00425C10 */
void ClearDebugPauseFlags(void);                                              /* 0x00425C20 */
unsigned int GetDebugKeyState(unsigned int *p);                        /* 0x00425C30 */
unsigned char *GetHighScoreEntry(short i);                           /* 0x00425DF0 */
unsigned int GetHighScoreValue(short i);                                      /* 0x00425E20 */
void SetHighScoreEntry(short i, unsigned char b, unsigned int v);    /* 0x00425E30 */
void ClearHighScoreTable(short v);                                    /* 0x00425ED0 */
int IsHighScoreSlotUsed(short i);                                      /* 0x00425FE0 */
void LoadSceneBackdrop(char n);                                           /* 0x00426C50 */
void exit_squadron(const char *msg);                                    /* 0x00427370 */
unsigned int GetJoystickButtonEdge(unsigned int a, short b);                    /* 0x004274C0 */
unsigned int GetScreenUpdateFlag(void);                                         /* 0x004279D0 */
unsigned int DrawStatusBarBackdrop(void);                                         /* 0x00427B00 */
unsigned int ResetScreenClipToFullHeight(void);                                         /* 0x00427BA0 */
unsigned int RefreshMemoryStatusOverlay(void);                                         /* 0x00427C30 */
unsigned int SetDefaultCommDelay(void);                                  /* 0x00427C80 */
short MeasureMessageWidth(const char *text);                                /* 0x00428E70 */
void ReportOutOfMemoryAndExit(void);                                    /* 0x00428F80 */
void ShowVersionBanner(void);                                           /* 0x004290D0 */
void ComputeArcadeTimeBonus(void);                                                 /* 0x00429E70 */
void RefreshCockpitStatus(void);                                                 /* 0x0042A0C0 */
unsigned int ReleaseStaleNavTarget(void);                                     /* 0x0042A170 */
void RedrawCommWindow(void);                                       /* 0x0042A670 */
void WaitForDebugStep(void);                                          /* 0x0042AFA0 */
void ServiceAudioStream(void);                                        /* 0x0042B1B0 */
int *FindWaveTableEntry(int key);                                      /* 0x0042B3F0 */
void FxDriverShutdownHook(void);                                            /* 0x0042C410 */
unsigned short GetJoystickPresentUnused(void);                                    /* 0x0042CDA0 */
unsigned int GetVictoryScreenId(void);                                     /* 0x0042D270 */
void CloseDataFileByHandle(unsigned short *p);                                  /* 0x0042D870 */
short GetTargetColourIndex(void);                                                /* 0x0042DB70 */
void LogDisplayMode(void);                                             /* 0x0042E020 */
void FadeMusic(void);                                                  /* 0x0042E320 */
void SetMusicOn(void);                                               /* 0x0042E330 */
void StopMusic(void);                                                   /* 0x0042E350 */
void SetMusBreakpt(void);                                               /* 0x0042E380 */
void PaletteFadeHook(void);                                            /* 0x0042E390 */
void FlushSoundEffect(void);                                               /* 0x0042E3A0 */
void FlushSoundEffects(void);                                               /* 0x0042E3C0 */
void SceneLeaveHook(void);                                            /* 0x0042E3E0 */
void StopMusicUnlessSuppressed(void);                                        /* 0x0042E8B0 */
unsigned short GetMusicMode(void);                                /* 0x0042E8D0 */
void ResetSoundState(void);                                             /* 0x0042EE80 */
void ResetSoundStateForScene(void);                                               /* 0x0042EEA0 */
void ResetSoundStateForFlight(void);                                               /* 0x0042EEB0 */
void EnableMusicForScene(void);                                               /* 0x0042EEE0 */
unsigned int SoundFxTick(void);                                       /* 0x0042EF00 */
void FlushSoundEffectsAndLog(void);                                               /* 0x0042EF10 */
void sound_effect(void);                                                /* 0x0042EF20 */
int IsShipDestroyed(short i);                                      /* 0x0042F1F0 */
unsigned short GetConversationState(void);                                    /* 0x0042F730 */
void InitFullScreenViewport(int *vp, short arg);                                    /* 0x0042F7E0 */
void FrameStartHook(void);                                            /* 0x0042F930 */
unsigned short IsSoundHardwarePresent(void);                                     /* 0x0042F940 */
void MessagePumpHook(void);                                           /* 0x0042F950 */
void GetFixedOneMillionThunk(void);                         /* 0x0042FB20 */
void GetFixedOneMillionThunkAlt(void);                       /* 0x0042FB30 */
short StepMenuSelection(short v, int flag);                                 /* 0x00430BC0 */
int IsCommMenuIdle(void);                                              /* 0x00430CA0 */
void ShowCommMenuEntry(short i);                                          /* 0x00430D30 */
void ShowCommMenuPrompt(unsigned int a, int b);                         /* 0x00430D50 */
int IsEjectPromptActive(void);                                                /* 0x00430D80 */
short GetPendingMenuAction(void);                                            /* 0x00430DA0 */
void SetPendingMenuAction(unsigned char v);                                 /* 0x00430DB0 */
void RequestEjectPrompt(void);                                               /* 0x00430DC0 */
int HasNoLockedTarget(void);                                              /* 0x00430E10 */
int IsWingmanIdle(void);                                              /* 0x00430E30 */
unsigned short IsCommMenuAvailable(void);                                     /* 0x00430E50 */
void RequestCommMenu(unsigned char v);                                       /* 0x00430E70 */
void EndCommMenu(void);                                              /* 0x004314C0 */
void ShowCentredPrompt(char *text, unsigned short arg);                       /* 0x004314F0 */
void ShutdownVideoHook(void);                                               /* 0x004318F0 */
void SetFpsCapEnabled(void);                                             /* 0x004320E0 */
void SetFpsCapDisabled(void);                                             /* 0x00432110 */
void DIBerror(const char *tag, int hr);                                 /* 0x00432140 */
void DIBpositionWindow(void);                                           /* 0x00432230 */
void DIBreInstall(void);                                                /* 0x004322B0 */
void DIBunInstall(void);                                              /* 0x00432680 */
void DIBslam(void);                                               /* 0x00432960 */
unsigned int GetDIBSurfacePitch(void);                                     /* 0x00432DE0 */
unsigned int GetDIBSurfaceBase(void);                                     /* 0x00432E00 */
void SetPaletteEntryFromWords(short i, unsigned short *rgb);                /* 0x00433020 */
void ReleaseDirectDrawPaletteAgain(void);                                              /* 0x004331E0 */
int GetShipIfArmorBelow15(short i);                                      /* 0x004345D0 */
unsigned short RandomBelow(short n);                                  /* 0x00434CD0 */
void SeedRandomFromClock(void);                                               /* 0x00434CF0 */
short __stdcall RandomInRange(short lo, short hi);                      /* 0x00434D20 */
short RandomBelowOrEqual(short n);                                      /* 0x00434D50 */
long FloatToLong(void);                                                /* 0x00434D80 */
long SinFixed(short degrees);                                    /* 0x00434E00 */
long CosFixed(short degrees);                                    /* 0x00434E30 */
long FloatToLongPassThrough(void);                                             /* 0x00434EC0 */
void SetTextCursor(unsigned short a, unsigned short b);             /* 0x00434F70 */
void SetTextContext(unsigned int v);                                       /* 0x00434FA0 */
void ReleaseDirectDrawPalette(void);                                          /* 0x00434FB0 */
unsigned int IdentityHandle(unsigned int v);                             /* 0x00434FC0 */
void __stdcall SetWholePaletteFromTriplets(unsigned char *palette);               /* 0x00434FD0 */
unsigned short ReadWord(unsigned short *p);                        /* 0x00434FE0 */
unsigned short GetFontCharWidth(char i);                                     /* 0x00434FF0 */
void ReleaseVideoResourcesHook(void);                                           /* 0x00435010 */
void SplitPackedPoint(unsigned int packed, short *p);                      /* 0x004350D0 */
void ResetTextCursor(void);                                           /* 0x004353F0 */
unsigned int DosFarPtrToNear(unsigned int v);                             /* 0x00435410 */
unsigned int DosNearPtrToFar(unsigned int v);                             /* 0x00435420 */
void DosStrrchr(char *s, short c);                                  /* 0x00435430 */
short DosStrlen(const char *s);                                   /* 0x004354D0 */
void DosMemcpy(void *dst, const void *src, size_t n);               /* 0x004354F0 */
unsigned short GetFxDriverCount(void);                                    /* 0x00435550 */
void InitExtendedMemoryStub(void);                                             /* 0x00435560 */
unsigned int SetWaitCursorAndRun(unsigned int a, void (*fn)(void));      /* 0x00435570 */
void ClearWaitCursorFlag(void);                                                  /* 0x00435590 */
unsigned short GetExtendedMemoryFree(void);                                 /* 0x004355A0 */
void BeginScreenUpdateHook(void);                                             /* 0x004355C0 */
void SetScreenUpdateMode(unsigned int v);                                /* 0x004355D0 */
void PumpMessagesAndDispatch(int a);                                              /* 0x00435CC0 */
void FreeAllTrackedAllocations(void);                                         /* 0x00435DB0 */
unsigned int ResetAllocationDepth(void);                                          /* 0x00435DC0 */
void EnterAllocationScope(void);                                                 /* 0x004360D0 */
void LeaveAllocationScope(void);                                                 /* 0x004360E0 */
void SetMouseHomePosition(short x, short y);                                  /* 0x00436160 */
void SetFrameTimerPeriod(short period);                                   /* 0x004361E0 */
void SetFrameTimerAndWait(short period);                                       /* 0x004361F0 */
void SetFrameTimerPeriodDirect(short p);                      /* 0x00436210 */
void WaitForFrameTick(void);                                          /* 0x00436230 */
int IsFrameTickElapsed(void);                                            /* 0x00436240 */
unsigned short GetSoundHardwareFlag(void);                                    /* 0x00436260 */
void TimerResetHook(void);                                           /* 0x004362A0 */
unsigned int GetVideoReleaseResult(void);                                      /* 0x004362B0 */
unsigned short IdentityWord(unsigned short v);                         /* 0x004362C0 */
void TimerStopHook(void);                                           /* 0x004362D0 */
unsigned int GetFixedOneMillion(void);                                 /* 0x004362E0 */
unsigned int GetFixedOneMillionAlt(void);                                 /* 0x004362F0 */
void MouseIdleHook(void);                                           /* 0x004368C0 */
unsigned short GetNavRangeSentinel(void);                                   /* 0x004368E0 */
unsigned short GetOriginalFreeMemory(void);                                   /* 0x004368F0 */
void StartupHook(void);                                           /* 0x00436900 */
void FreeIfNotNull(void *p);                                            /* 0x00436950 */
unsigned int GetStartupErrorCode(void);                                     /* 0x00436970 */
void ShutdownHook(void);                                           /* 0x00436980 */
unsigned short GetDiskRetryCount(void);                                   /* 0x004369A0 */
unsigned short GetDiskChangeCount(void);                                   /* 0x004369B0 */
unsigned short GetShutdownErrorCode(void);                                   /* 0x004369C0 */
void VideoReleaseHook(void);                                           /* 0x004369D0 */
void ExitCleanupHook(void);                                           /* 0x004369E0 */
unsigned int BeginBriefingScene(void);                                         /* 0x00438B90 */
void SetViewportRect(int p, unsigned short a, unsigned short b, unsigned short c, unsigned short d);/* 0x00439400 */
void ClearSaveSlotFlag(void);                                            /* 0x0043F690 */
int IsSaveSlotFree(void);                                            /* 0x0043F6A0 */
void ClearLoadSlotFlag(void);                                            /* 0x0043F720 */
void SelectSaveSlot(short i);                                        /* 0x0043F730 */
void CheckHeapBlockSignature(int p);                                              /* 0x004408A0 */
unsigned int GetHeapBlockSize(int p);                              /* 0x004408C0 */
unsigned int SignExtendClipCoord(unsigned short v);                         /* 0x00440BE0 */
void ClipViewportToScreen(int *p);                                           /* 0x00440CF0 */
void RasterLineHook(void);                                               /* 0x00441140 */
void SetPaletteEntry(short a, unsigned short *b);                         /* 0x004413C0 */

#endif /* WC1_FUNCS_H */
