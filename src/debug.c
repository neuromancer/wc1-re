/*
 *  Developer overlay message pumping and pause handling.
 *
 *  Address range 0x41c760-0x41cfff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the GDI debug console, keyboard hook and overlay worker
 *  form one contiguous block before the 0x41d000 utility tranche.
 */
#include "wc1.h"

/* Function start: 0x41CD40 */
char __fastcall PumpMessagesWhileDebugPaused(unsigned int state)
{
    RECT clip;
    MSG message;
    int minimized = 0;
    int complete = 0;

    if (state == 0 || *(int *)(state + 0x424) == 0) {
        while (DAT_00469648 == 0) {
            while (complete == 0) {
                if (minimized != 0) {
                    if (GetMessageA(&message, 0, 0, 0) == 0) {
                        ShutdownGameWindow();
                    } else {
                        complete = 1;
                        TranslateMessage(&message);
                        DispatchMessageA(&message);
                    }
                    if (IsIconic(DAT_005a89a0) == 0)
                        minimized = 0;
                    if (minimized == 0) {
                        clip.left = 0;
                        clip.top = 0;
                        clip.right = 320;
                        clip.bottom = 200;
                        ClipCursor(&clip);
                        ShowCursor(0);
                        SetPriorityClass(GetCurrentProcess(),
                                         HIGH_PRIORITY_CLASS);
                        SetActiveWindow(DAT_005a89a0);
                        SetForegroundWindow(DAT_005a89a0);
                        DIBreInstall();
                        DIBslam();
                        DIBslamReal();
                    }
                } else {
                    if (PeekMessageA(&message, 0, 0, 0, PM_NOREMOVE) == 0) {
                        complete = 1;
                    } else if (GetMessageA(&message, 0, 0, 0) == 0) {
                        complete = 0;
                        ShutdownGameWindow();
                    } else {
                        complete = 0;
                        TranslateMessage(&message);
                        DispatchMessageA(&message);
                    }
                }
                if (state == 0 && DAT_005a8964 != 0)
                    DAT_00469648 = DAT_005a8964;
                if (IsIconic(DAT_005a89a0) != 0) {
                    if (minimized == 0) {
                        ClipCursor(0);
                        ShowCursor(1);
                        SetPriorityClass(GetCurrentProcess(),
                                         NORMAL_PRIORITY_CLASS);
                    }
                    minimized = 1;
                }
                if (minimized != 0)
                    complete = 0;
            }
        }
    } else {
        while (DAT_00469648 == 0)
            ;
    }
    complete = (unsigned char)DAT_00469648;
    DAT_00469648 = 0;
    return (char)complete;
}
