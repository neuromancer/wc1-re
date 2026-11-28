/* winsys.c */
void SetMousePosition(int x, int y);                                    /* 0x00402E80 */
unsigned int PollKeyboardState(void);                                 /* 0x00402EA0 */
int GetShiftKeyState(void);                                             /* 0x00403060 */
int GetControlKeyState(void);                                           /* 0x00403070 */
unsigned int GetKeyboardModifiers(void);                                     /* 0x00403080 */
unsigned int GetGameClockTicks(void);                                   /* 0x00403090 */
void InitGameClockEpoch(void);                                    /* 0x004030B0 */
void WriteDebugString(const char *s);                                   /* 0x004030D0 */
void SetMousePositionDuplicate(int x, int y);                                 /* 0x004030E0 */
FontWorkspace **AllocateFontWorkspace(short fontIndex);                /* 0x00403100 */
void FreeFontWorkspace(FontWorkspace **workspace);                    /* 0x00403180 */
char *LocateStreamsDirOnDisc(void);                                  /* 0x004031C0 */
char FindCdRomDriveByVolumeLabel(const char *label,
                                 const char *directory);              /* 0x00403290 */
int SetCurrentDirOnDrive(char drive, const char *directory);          /* 0x004033E0 */
int PromptInsertCorrectCd(void);                                      /* 0x00403450 */
short __stdcall OpenDataFileOrDie(const char *path);                           /* 0x004034D0 */
