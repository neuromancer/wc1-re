/* video.c */
void __stdcall ShutdownVideoHook(short mode);                           /* 0x004318F0 */
short __stdcall ReserveContiguousPaletteEntries(short entryCount);    /* 0x00431900 */
void __stdcall ReleaseContiguousPaletteEntries(short firstEntry);     /* 0x00431970 */
void PrintPaletteAllocationMap(void);                                 /* 0x004319B0 */
