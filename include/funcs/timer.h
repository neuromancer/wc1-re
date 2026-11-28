/* timer.c */
void WaitForDebugStep(void);                                          /* 0x0042AFA0 */
void CALLBACK FrameTimerCallback(UINT timerId, UINT message, DWORD user,
                                 DWORD first, DWORD second);       /* 0x0042AFB0 */
void SetMultimediaTimerCallback(int period);                       /* 0x0042AFC0 */
