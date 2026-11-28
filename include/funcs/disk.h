/* disk.c */
void ReportPacketLoadError(void *packet, short logicalFile,
                           short retry, short section,
                           const char *sourceTag);                      /* 0x0041D120 */
void *LoadPacketIntoBuffer(short logicalFile, short section,
                           void *destination);                         /* 0x0041D200 */
void *LoadPacketAllocated(short logicalFile, short section);           /* 0x0041D250 */
void *FetchDiskPacketRetrying(short logicalFile, short section,
                              short flags);                             /* 0x0041D2E0 */
unsigned int InitializeTextContextFromFont(TextContext *context,
                                           short fontIndex,
                                           unsigned char colour,
                                           signed char background);    /* 0x0041D510 */
unsigned int ReleaseTextFont(short fontIndex);                         /* 0x0041D590 */
unsigned int DrawTextAt(TextContext *context, short x, short y,
                        const char *text,
                        unsigned char alignment);                     /* 0x0041D5F0 */
unsigned int SortSignedByteValuesAscending(signed char *values,
                                           short count);                /* 0x0041D650 */
short OpenDiskDataFile(short logicalFile);                              /* 0x0041D6C0 */
void __stdcall PromptInsertNumberedDisk(short logicalFile);             /* 0x0041D760 */
unsigned int SetDiskFileNumber(char *fileName, short diskNumber);        /* 0x0041D9B0 */
unsigned int GetZeroUnused(void);                                        /* 0x0041DA00 */
short CheckEscaped(void);                                               /* 0x0041DA10 */
short WaitForInputKey(void);                                         /* 0x0041DAA0 */
void WaitForSceneAdvance(short duration, short unused);                /* 0x0041DBA0 */
void MoveMenuPointerFromKeyboard(InputEventState *event);               /* 0x0041DC70 */
void EraseLastTextInputCharacter(void);                              /* 0x0041DDF0 */
short WaitForStreamInputKey(void);                                  /* 0x0041DEB0 */
