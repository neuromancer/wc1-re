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

void CombatRoutine02(short a, int b, unsigned int c, int d, int e);/* 0x00417A00 */
void DebugOverlayThreadProc(int p);                               /* 0x0041C910 */
char *DirectDrawResultToText(int hr);                             /* 0x004331F0 */
void DrawWrappedCommText(char *a, unsigned char b);           /* 0x00416260 */
void DispatchPendingEvents(int a);                            /* 0x004359C0 */
void DispatchCommMenuCommand(void *a, int b, short c);          /* 0x004117D0 */
void ClearHudMessageSlot(int *p);                                       /* 0x00414180 */
void ShowModalMessage(const char *s);                                /* 0x00428F20 */
void DrawStickIndicator(void);                                      /* 0x00417260 */
void ix_system_shutdown(void);                                   /* 0x004273F3 */
void ShowHudMessageUnlessDuplicate(int a, unsigned short b, int c);             /* 0x004142B0 */
void EndCommSessionWithWingman(void);                                        /* 0x00431470 */
void StopAllSounds(void);                                          
void RunSimulationTick(void);                                       /* 0x00427C50 */
void DrawHudMessageList(void);                                           /* 0x00417760 */
void RunFrameUpdate(void);                                           /* 0x00409DD0 */
void LogMemoryUsage(void);                                          /* 0x004272F0 */
void ClearShipAiOrders(short ship);                               /* 0x00407460 */
short LoadWingCmdrCfgFile(short argc, char **argv);               /* 0x0042C580 */
void ix_system_delete_all_samples(void);                                  /* 0x00427548 */
void ix_system_delete_all_sounds(void);                                   /* 0x00427610 family */
void WaitForKeyAcknowledge(int mode);                                   /* 0x00428EA0 */
void PlaySfxWaveFileByNumber(int n, int a, int b);                
void PumpMessagesWhileDebugPaused(unsigned int arg);              /* 0x0041CD40 */
void ReleaseSceneActors(void);                              /* 0x00424490 */
void EnterNavPoint(short a);                           /* 0x0040BFF0 */
void ReportComponentHit(char a, char b, char c);                  /* 0x00414BF0 */
void ResetSeriesProgress(void);                                          /* 0x00424BE0 */
void SetHudMessageText(void *a, unsigned short b, unsigned short c);    /* 0x00436DE0 */
void ShipAiRoutine02(short a, unsigned int b, int c);             /* 0x00406200-family */
void ShipAiState29(short a, short b);                             /* 0x00406FB0 */
void ShipAiState40(short a, short b);                             /* 0x00406D80 */
void ShowEnemyTargetSelectMenu(void);                             /* 0x00430E90 */
void ShowMemoryStatusDebug(void);                                 /* 0x004273C0 */
void ShowOnScreenMessage(short f, short d, const char *fmt, ...); 
void Streamer_stop(void);                                         /* 0x00442460 */
short UiRoutine03(void);                                          /* 0x00429E30 family */
void _free_dbg(void *p, int blockType);                           /* Function start: 0x401250 */
long _ftol(void);                                                 /* Pointer table of prompt strings indexed by SendCommMenuChoice. */
void auto_pilot_sequence(void);                                   /* named by the leaked header */
void initialize_object(short obj, enum ObjectType type,
                       short owner);                              /* 0x0041E120 */
int drop_mine(short obj, short weapon, enum ObjectType type,
              short lifetime);                                   /* 0x004212A0 */

#endif /* WC1_EXTERN_H */
