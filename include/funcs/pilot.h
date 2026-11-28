/* pilot.cpp */
unsigned int WaitForKeyExceptXOrF12(void);                            /* 0x00425730 */
void ShowMeanwhileTransition(short scene, short variant);             /* 0x00425770 */
void ApplyAnswerTextCipher(char *text, signed char direction);        /* 0x004258D0 */
void LoadAnswerPromptAndResponse(short entry, char *prompt,
                                 char *response);                     /* 0x00425910 */
short PromptForAnswerText(short entry);                              /* 0x004259B0 */
void SceneEnterHook(void);                                            /* 0x00425AF0 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows);             /* 0x00425B00 */
void DestroyGlobalDebugOverlayConsole(void);                           /* 0x00425B90 */
void SystemDebugPrintf(const char *format, ...);                           /* 0x00425BB0 */
char PumpMessagesDuringWait(void);                                        /* 0x00425BC0 */
unsigned char TakeDebugStepFlag(void);                                     /* 0x00425BD0 */
void ResetDiskPromptTimerHook(int mode, int interval);                       /* 0x00425BE0 */
void SetConsoleTextColourHook(void);                                  /* 0x00425BF0 */
void SetConsoleBackgroundColourHook(void);                            /* 0x00425C00 */
void DiskPromptDrawHook(void);                                             /* 0x00425C10 */
void ClearDebugPauseFlags(void);                                              /* 0x00425C20 */
