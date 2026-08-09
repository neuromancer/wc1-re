/*
 *  Functions referenced by the reconstruction but not yet reimplemented.
 *
 *  Each one still carries an operational Ghidra label because nothing has
 *  been written for it yet; the label describes mechanism, not intent
 *  (docs/LABELS.md).  Entries move to include/wc1funcs.h as they are
 *  implemented, and get a real name at that point.
 */
#ifndef WC1_EXTERN_H
#define WC1_EXTERN_H

void AbortToDesktop(void);                                        /* 0x00402110 */
unsigned int PresentViewport(int *p, char c);                        
long FloatToLongDirect(void);                                       /* 0x00434F20 */
int CheckMcgaVideoMode();                                         /* Function start: 0x42F7E0 */
void CombatRoutine02(short a, int b, unsigned int c, int d, int e);/* 0x00417A00 */
void CombatRoutine03(int a, short b);                             /* 0x00430C00-family */
int DIBcascade(int mode, int *err);                               /* 0x00432410 */
void DIBdestroyDIB(void);                                         /* 0x004328A0 */
void DIBslamReal(void);                                           /* 0x00432970 */
void DIBwholePaletteFromTriplets(unsigned char *palette);         /* 0x00433060 */
void DIBwholePaletteFromWords(void);                              /* 0x00433120 */
void DebugOverlayThreadProc(int p);                               /* 0x0041C910 */
char *DirectDrawResultToText(int hr);                             /* 0x004331F0 */
void StepShipOrderState(short i);                                /* 0x00409F80 */
void DrawWrappedCommText(char *a, unsigned char b);           /* 0x00416260 */
void DispatchPendingEvents(int a);                            /* 0x004359C0 */
void DispatchCommMenuCommand(void *a, int b, short c);          /* 0x004117D0 */
void StepShipOrderWithLeader(short i);                          /* 0x0040A030 */
void StepShipOrderForNav(short a);                          /* 0x0040A360 */
void CopyViewportContents(void *a, void *b);                             
void InterpolateClamped(int a, int b, unsigned int c, int d, int e);   /* 0x00403BA0 */
void SplitGameClockToHms(void *p);                                      /* 0x00413C40 */
void ClearHudMessageSlot(int *p);                                       /* 0x00414180 */
void IsPointInRect(short a, short b, short *p);                   /* 0x00435090 */
void FixedToShortSaturating(int v);                                        /* 0x004184C0 */
void MakeRandomVectorFixed(int a, short b, int *p);                       /* 0x00418780 */
void ShowModalMessage(const char *s);                                /* 0x00428F20 */
void NormaliseAndScaleVector(unsigned int *p, int v);                       /* 0x00419950 */
void ScaleDeltaToRange(int *a, int *b, short c);                      
void SetShipOrder(short a, int b);                              /* 0x00402C70 */
void AimShipAtPointOffset90(short a, void *p);                            /* 0x00419810 */
void DrawStickIndicator(void);                                      /* 0x00417260 */
void FetchDiskPacketRetrying(short a, int b, int c);              /* 0x0041D2E0 */
unsigned int JoystickEdgeHook(void);                             /* 0x00427440 family */
void ix_system_shutdown(void);                                   /* 0x004273F3 */
void ShowHudMessageUnlessDuplicate(int a, unsigned short b, int c);             /* 0x004142B0 */
unsigned int PushMemoryStackFrameTagged(int a, int b);                    /* 0x0040FA90 */
void AimShipAtPoint(short a, void *p);                        /* 0x00419660 */
void EndCommSessionWithWingman(void);                                        /* 0x00431470 */
void StopAllSounds(void);                                          
void RunSimulationTick(void);                                       /* 0x00427C50 */
void ScaleShipVelocity(short i);                               /* 0x0040E820 */
short GetHeadingErrorToPoint(short a, void *p);                     
void FlushPendingScreenText(unsigned char a, int b);                         /* 0x004360F0 */
void DrawHudMessageList(void);                                           /* 0x00417760 */
void RunFrameUpdate(void);                                           /* 0x00409DD0 */
void LogMemoryUsage(void);                                          /* 0x004272F0 */
void ClearShipAiOrders(short ship);                               /* 0x00407460 */
void FatalErrorAndExit(const char *s);                              /* 0x00428FA0-family */
void LoadInstallDat(void);                                        /* 0x0042C660 */
short LoadWingCmdrCfgFile(short argc, char **argv);               /* 0x0042C580 */
void ix_system_delete_all_samples(void);                                  /* 0x00427548 */
void FreeWaveTable(void);                                 /* 0x0042B300 */
void ix_system_delete_all_sounds(void);                                   /* 0x00427610 family */
void ClearEventSlotByAddress(int node);                                   /* 0x00435760 */
void DosStrcpy(void *dst, const char *src);                       /* 0x00435470 */
void WaitForKeyAcknowledge(int mode);                                   /* 0x00428EA0 */
short FindNearestNavPoint(int a);                                       /* 0x0042A120 */
void MonoDebug_print(const char *s);                              /* 0x00403DE0 */
void PlaySfxWaveFileByNumber(int n, int a, int b);                
void PumpMessagesWhileDebugPaused(unsigned int arg);              /* 0x0041CD40 */
void PumpWindowMessages(void);                                    /* 0x00402320 */
void ReleasePacketHandle(int h);                                  /* 0x0042FAE0 */
void ReleaseSceneActors(void);                              /* 0x00424490 */
void DestroyShip(short i);                           /* Function start: 0x401680 */
void EnterNavPoint(short a);                           /* 0x0040BFF0 */
void free_viewport(void *p);                           /* 0x0042F940-family */
void ReportComponentHit(char a, char b, char c);                  /* 0x00414BF0 */
void ReleaseAllSfxSlots(void);                                 /* 0x00401210 */
void ProcessShipCommQueue(short a);                              /* 0x0040AA0 family */
void FindShipCommEntry(short a, int b);                       /* 0x00421100 */
short AnyShipReadyToJump(void);                                /* 0x004154C0 */
void ComputeStickIndicatorFrame(void);                                       /* 0x004171D0 */
void SetMultimediaTimerCallback(int period);                      /* 0x0042AFC0 */
void ResetSeriesProgress(void);                                          /* 0x00424BE0 */
void UpdateAimSolution(short a, int *p);                       /* 0x00419290 */
void PushSeriesStateWord(short i, int v);                        /* 0x004148A0 */
void SetHudMessageText(void *a, unsigned short b, unsigned short c);    /* 0x00436DE0 */
void ShipAiRoutine02(short a, unsigned int b, int c);             /* 0x00406200-family */
void ShipAiRoutine04(short a, short b);                           /* 0x00406310-family */
void ShipAiRoutine07(short a, int b);                             
int ShipAiRoutine18(short s);                                     /* 0x00407060 */
void ShipAiRoutine21(short a, int b);                             
void ShipAiState29(short a, short b);                             /* 0x00406FB0 */
void ShipAiState40(short a, short b);                             /* 0x00406D80 */
void ShowEnemyTargetSelectMenu(void);                             /* 0x00430E90 */
void ShowMemoryStatusDebug(void);                                 /* 0x004273C0 */
void ShowOnScreenMessage(short f, short d, const char *fmt, ...); 
void Streamer_stop(void);                                         /* 0x00442460 */
short UiRoutine03(void);                                          /* 0x00429E30 family */
void ValidateViewportBounds(int *a, int *b, unsigned int *c);     /* 0x00440C00 */
void _free_dbg(void *p, int blockType);                           /* Function start: 0x401250 */
long _ftol(void);                                                 /* Pointer table of prompt strings indexed by ShowCommMenuEntry. */
void auto_pilot_sequence(void);                                   /* named by the leaked header */

#endif /* WC1_EXTERN_H */
