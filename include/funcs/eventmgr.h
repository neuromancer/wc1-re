/* eventmgr.c */
void TranslatePolledInputEvent(unsigned short type,
                               unsigned int value);                   /* 0x004355F0 */
void QueueInputEventAtCursor(unsigned short type, short primaryButton,
                             short secondaryButton);                   /* 0x004356A0 */
InputEvent *AllocateInputEvent(void);                                  /* 0x004356E0 */
void ReleaseInputEvent(InputEvent *event);                             /* 0x00435760 */
void QueueInputEvent(int type, int x, int y, int value,
                     int primaryButton, int secondaryButton,
                     unsigned int timestamp);                          /* 0x00435790 */
void ReleaseInputEventQueue(void);                                    /* 0x004358B0 */
void RetainInputEventsOfType(int type);                                /* 0x004358E0 */
void RemoveInputEvent(InputEvent *event);                              /* 0x00435940 */
short __stdcall GetNextInputEvent(InputEventState *event);             /* 0x004359C0 */
short __stdcall PollInputEvent(InputEventState *event, short filter);  /* 0x00435CC0 */
short __stdcall PeekInputEvent(InputEventState *event, short type);    /* 0x00435CE0 */
short __stdcall IsInputEventQueued(short type);                        /* 0x00435D80 */
void FlushInputEvents(void);                                          /* 0x00435DB0 */
short __stdcall ResetAllocationDepth(int x, int y);                                          /* 0x00435DC0 */
void CheckCursor(void);                                                /* 0x00435DE0 */
void CaptureMouseCursorBackground(void);                           /* 0x00435E20 */
void DrawMouseCursor(void);                                        /* 0x00435EF0 */
void RestoreMouseCursorBackground(void);                           /* 0x00435FA0 */
void RefreshMouseCursorDisplay(void);                              /* 0x00436060 */
void EnterAllocationScope(void);                                                 /* 0x004360D0 */
void LeaveAllocationScope(void);                                                 /* 0x004360E0 */
void __stdcall SetMouseCursorShape(struct Shape *shape, short frame); /* 0x004360F0 */
void __stdcall SetMouseHomePosition(short x, short y);                /* 0x00436160 */
void __stdcall ApplyPackedMousePosition(ShortPoint point);             /* 0x00436190 */
void __stdcall GetMouseCursorPosition(ShortPoint *point);              /* 0x004361C0 */
void SetFrameTimerPeriod(short period);                                   /* 0x004361E0 */
void __stdcall SetFrameTimerAndWait(short period);                                       /* 0x004361F0 */
void __stdcall SetFrameTimerPeriodDirect(short p);                    /* 0x00436210 */
void WaitForFrameTick(void);                                          /* 0x00436230 */
int IsFrameTickElapsed(void);                                            /* 0x00436240 */
unsigned short GetSoundHardwareFlag(void);                                    /* 0x00436260 */
void *AllocateUntaggedMemory(unsigned int size);                       /* 0x00436270 */
void ReleaseUntaggedMemory(void *memory);                              /* 0x00436290 */
void TimerResetHook(void);                                           /* 0x004362A0 */
unsigned int GetPacketDecompressionResult(void *);                     /* 0x004362B0 */
unsigned short IdentityWord(unsigned short v);                         /* 0x004362C0 */
void TimerStopHook(void);                                           /* 0x004362D0 */
unsigned int GetFixedOneMillion(void);                                 /* 0x004362E0 */
unsigned int GetFixedOneMillionAlt(void);                              /* 0x004362F0 */
void ClearInputKeyStatePreservingModifiers(void);                     /* 0x004363A0 */
void ClearInputKeyState(void);                                        /* 0x004363E0 */
void SetInputKeyState(int scanCode, unsigned char pressed);            /* 0x00436420 */
void sort_object_depth(void);                                          /* 0x00436460 */
void draw_sorted_objects_to_buffer(void);                              /* 0x00436520 */
void intro_drawbackgroundships(void);                                  /* 0x00436650 */
void set_up_screen_viewport(signed char mode);                         /* 0x00436740 */
void MouseIdleHook(void);                                           /* 0x004368C0 */
unsigned short GetNavRangeSentinel(void);                                   /* 0x004368E0 */
unsigned short GetOriginalFreeMemory(void);                                   /* 0x004368F0 */
void StartupHook(unsigned int (*callback)(unsigned int, short));  /* 0x00436900 */
unsigned int JoystickEdgeHook(int button);                         /* 0x00436910 */
void *AllocateSmallMemoryBlock(short size);                           /* 0x00436940 */
void FreeIfNotNull(void *p);                                            /* 0x00436950 */
unsigned int GetStartupErrorCode(int vector);                       /* 0x00436970 */
void ShutdownHook(int vector, void *handler);                       /* 0x00436980 */
unsigned short __stdcall SelectDiskDriveHook(short drive);             /* 0x004369A0 */
short GetCurrentDiskDriveHook(void);                                    /* 0x004369B0 */
unsigned short __stdcall GetShutdownErrorCode(
    unsigned char *driveState);                                    /* 0x004369C0 */
void FinishPacketDecompressionHook(void);                              /* 0x004369D0 */
void ExitCleanupHook(void);                                           /* 0x004369E0 */
unsigned int IsVectorWithinRange(FixedVector *vector, short range);    /* 0x00436A00 */
unsigned int shrink_vector(FixedVector *vector);                       /* 0x00436A30 */
unsigned int shrink(int *component);                                   /* 0x00436A70 */
void __stdcall FillGraphicSuffix(char *path, short number,
                                 short digits);                        /* 0x00436C70 */
void __stdcall ConvertChar_Int(char *text, short number,
                              short digits);                          /* 0x00436CB0 */
