/* dib.c */
void ThrottleFrameAndDrawFps(HDC dc);                                       /* 0x00431F00 */
void ReportSpaceFlightMaxFps(float adjustment);                      /* 0x00432050 */
void SetSpaceFlightFrameTiming(void);                                  /* 0x004320E0 */
void SetCinematicFrameTiming(void);                                    /* 0x00432110 */
void DIBerror(const char *tag, int hr);                                 /* 0x00432140 */
void DIBpositionWindow(void);                                           /* 0x00432230 */
void DIBreInstall(void);                                                /* 0x004322B0 */
void DIBinstall(HWND window);                                           /* 0x00432310 */
int DIBcascade(int mode, int *reportedResult);                          /* 0x00432410 */
void DIBunInstall(void);                                              /* 0x00432680 */
void DIBmakeDIB(void);                                                  /* 0x004326E0 */
void DIBdestroyDIB(void);                                             /* 0x004328A0 */
void DIBslam(void);                                               /* 0x00432960 */
void DIBslamReal(void);                                           /* 0x00432970 */
void DIBupdate(int left, int top, int right, int bottom);               /* 0x00432C60 */
unsigned char *GetDIBPixelBuffer(void);                                  /* 0x00432DE0 */
HWND GetDIBWindowHandle(void);                                           /* 0x00432E00 */
void CachePaletteEntryFromWords(short index, short *rgb);                 /* 0x00432E30 */
void DIBramPalette(void);                                                 /* 0x00432EA0 */
void DIBsetPalette(short index, short *rgb);                              /* 0x00432F10 */
void GetPaletteEntryAsWords(short i, short *rgb);                         /* 0x00433020 */
void DIBwholePaletteFromTriplets(unsigned char *palette);             /* 0x00433060 */
void DIBwholePaletteFromWords(short *palette);                         /* 0x00433120 */
void DIBwaitForVerticalBlank(void);                                      /* 0x004331E0 */
char *DirectDrawResultToText(int result);                              /* 0x004331F0 */
