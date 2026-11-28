/* mono.c */
void __stdcall CloseDataFile(unsigned short fd);                           /* 0x00403500 */
short __stdcall WriteDataFileAtOffset(unsigned short fd, int offset,
                                      unsigned int length,
                                      const void *data);                    /* 0x00403520 */
short __stdcall CreateDataFile(const char *path);                          /* 0x004035C0 */
int __stdcall ReadDataFileAtOffset(unsigned short fd, int offset,
                                   unsigned int length, void *data);        /* 0x00403610 */
int __stdcall SeekDataFile(unsigned short fd, int offset,
                           unsigned int origin);                            /* 0x004036B0 */
int MeasureScaledIntroTextWidth(const char *text, short scale);            /* 0x00403710 */
int DrawCenteredScaledIntroText(const char *text, short centreX,
                                short baselineY, short scale);             /* 0x004037A0 */
short GetLineLength(const char *text);                                  /* 0x00403890 */
int print_subtitle(Viewport *viewport, short colour,
                    const char *text);                                  /* 0x00403920 */
int advance_canned_sequence(short obj);                                 /* 0x00403A80 */
unsigned int update_canned_sequence(short obj);                        /* 0x00403B70 */
void __stdcall SplitGameClockTicks(unsigned char *parts);                  /* 0x00403C40 */
void MonoDebug_install(void);                                           /* 0x00403C90 */
void MonoDebug_remove(void);                                            /* 0x00403D60 */
void SoundDebugPrintf(const char *fmt, ...);                          /* 0x00403DB0 */
void MonoDebug_print(const char *text);                                  /* 0x00403DE0 */
void ReadPerformanceCounter(LARGE_INTEGER *p);                           /* 0x00403E30 */
char *__stdcall ResetStringBuilder(TextContext *context);              /* 0x00403E40 */
