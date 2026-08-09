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

void DebugOverlayThreadProc(int p);                               /* 0x0041C910 */
char *DirectDrawResultToText(int hr);                             /* 0x004331F0 */
void DispatchPendingEvents(int a);                            /* 0x004359C0 */
void ix_system_shutdown(void);                                   /* 0x004273F3 */
void EndCommSessionWithWingman(void);                                        /* 0x00431470 */
void StopAllSounds(void);                                          
short LoadWingCmdrCfgFile(short argc, char **argv);               /* 0x0042C580 */
void ix_system_delete_all_samples(void);                                  /* 0x00427548 */
void ix_system_delete_all_sounds(void);                                   /* 0x00427610 family */
void PlaySfxWaveFileByNumber(int n, int a, int b);                
void ShowEnemyTargetSelectMenu(void);                             /* 0x00430E90 */
void Streamer_stop(void);                                         /* 0x00442460 */
short UiRoutine03(void);                                          /* 0x00429E30 family */
void _free_dbg(void *p, int blockType);                           /* Function start: 0x401250 */
long _ftol(void);                                                 /* Pointer table of prompt strings indexed by SendCommMenuChoice. */
void auto_pilot_sequence(void);                                   /* named by the leaked header */
int drop_mine(short obj, short weapon, enum ObjectType type,
              short lifetime);                                   /* 0x004212A0 */

#endif /* WC1_EXTERN_H */
