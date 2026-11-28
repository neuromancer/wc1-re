/* debug.cpp */
DWORD WINAPI DebugOverlayWorkerProc(void *parameter);                  /* 0x0041C960 */
LRESULT CALLBACK DebugKeyboardHookProc(int code, WPARAM key,
                                       LPARAM flags);                  /* 0x0041CA60 */
void DebugOverlayPrintf(DebugOverlayConsole *console,
                        const char *format, ...);                      /* 0x0041CAB0 */
