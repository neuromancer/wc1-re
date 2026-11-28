/* screen.c */
void cleanup_objectives(void);                                        /* 0x0042EFC0 */
int too_busy(short ship);                                             /* 0x0042F1F0 */
void reply(short ship, short accepted);                               /* 0x0042F210 */
int disobey_formation(short ship);                                    /* 0x0042F240 */
int bad_target(short ship, short target);                             /* 0x0042F270 */
short can_land(void);                                                 /* 0x0042F2B0 */
short i_wanna_rout(short ship, int pilot);                            /* 0x0042F350 */
void request(short requester, short ship, short command);             /* 0x0042F3F0 */
unsigned short __stdcall ShouldSuspendCursorForRect(
    const ShortRect *bounds);                                           /* 0x0042F730 */
unsigned short __stdcall InitializeDIBScreenViewport(
    Viewport *viewport, unsigned short colour);                       /* 0x0042F740 */
void __stdcall InitFullScreenViewport(Viewport *vp, short arg);                          /* 0x0042F7E0 */
unsigned int __stdcall GetPacketSize(const char *filename,
                                     short section);                  /* 0x0042F810 */
int GetFreeNearHeapBytes(void);                                       /* 0x0042F890 */
void FrameStartHook(int mode);                                         /* 0x0042F930 */
unsigned short IsSoundHardwarePresent(short device);                           /* 0x0042F940 */
void MessagePumpHook(int mode);                                        /* 0x0042F950 */
void *PushMemoryStackFrame(void *memory, int offset);                   /* 0x0042F960 */
int IsPushedPacketHandle(void *handle);                                /* 0x0042F9E0 */
void *MapPacketHandleToBlock(void *handle);                            /* 0x0042FA20 */
