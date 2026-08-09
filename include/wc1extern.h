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
unsigned int BlitUiFn1AE0(int *p, char c);                        
long CalcVideoFn4F20(void);                                       /* 0x00434F20 */
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
void DispatchDebugFn9F80(short i);                                /* 0x00409F80 */
void DispatchG004691e2Fn6260(char *a, unsigned char b);           /* 0x00416260 */
void DispatchTbl0046da90Fn59C0(int a);                            /* 0x004359C0 */
void DispatchTbl0059b9e0Fn17D0(void *a, int b, short c);          /* 0x004117D0 */
void DispatchTbl0059d400FnA030(short i);                          /* 0x0040A030 */
void DispatchTbl0059d5e0FnA360(short a);                          /* 0x0040A360 */
void DoLocalFn1A90(void *a, void *b);                             
void DoLocalFn3BA0(int a, int b, unsigned int c, int d, int e);   /* 0x00403BA0 */
void DoLocalFn3C40(void *p);                                      /* 0x00413C40 */
void DoLocalFn4180(int *p);                                       /* 0x00414180 */
void DoLocalFn5090(short a, short b, short *p);                   /* 0x00435090 */
void DoLocalFn84C0(int v);                                        /* 0x004184C0 */
void DoLocalFn8780(int a, short b, int *p);                       /* 0x00418780 */
void DoLocalFn8F20(const char *s);                                /* 0x00428F20 */
void DoLocalFn9950(unsigned int *p, int v);                       /* 0x00419950 */
void DoLocalFn9990(int *a, int *b, short c);                      
void DoShipAiFn2C70(short a, int b);                              /* 0x00402C70 */
void DoShipAiFn9810(short a, void *p);                            /* 0x00419810 */
void DrawCombatFn7260(void);                                      /* 0x00417260 */
void FetchDiskPacketRetrying(short a, int b, int c);              /* 0x0041D2E0 */
unsigned int GameNoBodyStubv20(void);                             /* 0x00427440 family */
void GetAudioVoiceFn73F3(void);                                   /* 0x004273F3 */
void GetCombatFn42B0(int a, unsigned short b, int c);             /* 0x004142B0 */
unsigned int GetG0046ad88FnFA90(int a, int b);                    /* 0x0040FA90 */
void GetG0059c490Fn9660(short a, void *p);                        /* 0x00419660 */
void GetPilotFn1470(void);                                        /* 0x00431470 */
void GetSfxFnB640(void);                                          
void GetShipAiFn7C50(void);                                       /* 0x00427C50 */
void GetTbl0059b320FnE820(short i);                               /* 0x0040E820 */
short GetTbl0059d710Fn9210(short a, void *p);                     
void GetUiFn60F0(unsigned char a, int b);                         /* 0x004360F0 */
void GetUiFn7760(void);                                           /* 0x00417760 */
void GetUiFn9DD0(void);                                           /* 0x00409DD0 */
void Helper4272F0(void);                                          /* 0x004272F0 */
void ClearShipAiOrders(short ship);                               /* 0x00407460 */
void HelperOf428F80A(const char *s);                              /* 0x00428FA0-family */
void LoadInstallDat(void);                                        /* 0x0042C660 */
short LoadWingCmdrCfgFile(short argc, char **argv);               /* 0x0042C580 */
void LoopAudioSoundFn7548(void);                                  /* 0x00427548 */
void LoopAudioStreamFnB300(void);                                 /* 0x0042B300 */
void LoopG0059861cFn7610(void);                                   /* 0x00427610 family */
void LoopInputFn5760(int node);                                   /* 0x00435760 */
void DosStrcpy(void *dst, const char *src);                       /* 0x00435470 */
void LoopLocalFn8EA0(int mode);                                   /* 0x00428EA0 */
short LoopNavFnA120(int a);                                       /* 0x0042A120 */
void MonoDebug_print(const char *s);                              /* 0x00403DE0 */
void PlaySfxWaveFileByNumber(int n, int a, int b);                
void PumpMessagesWhileDebugPaused(unsigned int arg);              /* 0x0041CD40 */
void PumpWindowMessages(void);                                    /* 0x00402320 */
void ReleasePacketHandle(int h);                                  /* 0x0042FAE0 */
void ReleaseTbl00469d50Fn4490(void);                              /* 0x00424490 */
void ReleaseTbl0046c028Fn9BD0(short i);                           /* Function start: 0x401680 */
void ReleaseTbl0046c35fFnBFF0(short a);                           /* 0x0040BFF0 */
void ReleaseTbl005a7f10FnF940(void *p);                           /* 0x0042F940-family */
void ReportComponentHit(char a, char b, char c);                  /* 0x00414BF0 */
void ScanTbl0046c028Fn1210(void);                                 /* 0x00401210 */
void ScanTbl0059cab0Fn0AA0(short a);                              /* 0x0040AA0 family */
void ScanTbl0059cab0Fn1100(short a, int b);                       /* 0x00421100 */
short ScanTbl0059d100Fn54C0(void);                                /* 0x004154C0 */
void SetCombatFn71D0(void);                                       /* 0x004171D0 */
void SetMultimediaTimerCallback(int period);                      /* 0x0042AFC0 */
void SetNavFn4BE0(void);                                          /* 0x00424BE0 */
void SetTbl0059d710Fn9290(short a, int *p);                       /* 0x00419290 */
void SetTbl0059dec0Fn48A0(short i, int v);                        /* 0x004148A0 */
void SetUiFn6DE0(void *a, unsigned short b, unsigned short c);    /* 0x00436DE0 */
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
long _ftol(void);                                                 /* Pointer table of prompt strings indexed by HelperOf430FC0D. */
void auto_pilot_sequence(void);                                   /* named by the leaked header */

#endif /* WC1_EXTERN_H */
