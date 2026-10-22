/*
 *  Inter-scene transitions, answer prompts, and ownership of
 *  the Win32 developer overlay console.
 *
 *  Address range 425730-425c2f (see docs/ORDER.md).
 *  The remaining internal boundary between the C scene routines and
 *  Win32 console wrappers is provisional.
 */
#include "wc1.h"

DebugOverlayConsole *pDebugOverlay; /* 0x004763f0 */
/* Nothing in the shipped executable references this buffer. A C++ global
 * stays in this object's .bss in declaration order; an unreferenced C static
 * in the following unit would not be emitted at all. */
unsigned char abUnusedOverlayBuffer[0x100]; /* 0x004763f8 */

/* Function start: 0x425730 */
unsigned int WaitForKeyExceptXOrF12(void)
{
    int key;

    bFilteredKeyDelayStartedState =
        bFilteredKeyWaitActive = 1;
    do {
        key = PumpMessagesDuringWait();
    } while (key == 'X' || key == VK_F12);
    bFilteredKeyWaitActive = 0;
    FlushInputEvents();
    ClearInputKeyState();
    return 0;
}

/* Function start: 0x425770 */
void ShowMeanwhileTransition(short scene, short variant)
{
    int track;

    track = variant + 0x21;
    PreloadMusicTrackHook((short)track);
    spacetrack(track, 2, 1);
    InitializeConversationViewport();
    InitializeConversationText();
    LoadSceneAnimationResources(scene, variant);
    ClearViewport(stCurrentConvTxtCtx.viewport,
                  cBlackColour);
    SetTextContext(&stCurrentConvTxtCtx);
    pIntroFont =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    print_subtitle(&stSceneBuffer, 0x40, "Meanwhile...");
    PanToScreen(&stSceneBuffer, &stScreenView);
    FreePacketAndClear((int *)&pIntroFont, 0);
    WaitForSceneAdvance(100, 0);
    ClearViewport(&stModalSrcViewport, cBlackColour);
    DIBslam();
    DIBslamReal();
    SceneDirector(6, pSceneAnimationSceneDataPtr,
                  pSceneAnimationTextData);
    SaveGamePalette();
    StopMusic(30);
    FadeViewportPaletteToColour(&stModalSrcViewport,
                                cBlackColour, 1);
    ClearViewport(&stModalSrcViewport, cBlackColour);
    DIBslam();
    DIBslamReal();
    RestoreGamePalette();
    DIBslam();
    DIBslamReal();
    ReleaseTextFont(0);
    ResetScreenClipToFullHeight();
    ReleaseSceneAnimationResources();
    StopMusicUnlessSuppressed();
    ReleaseMusicTrackHook((short)track);
}

/* Function start: 0x4258D0 */
void ApplyAnswerTextCipher(char *text, signed char direction)
{
    short position;

    position = 0;
    while (*text != '\0') {
        position++;
        *text += (signed char)((position % 30) - 38) * direction;
        text++;
    }
}

/* Function start: 0x425910 */
void LoadAnswerPromptAndResponse(short entry, char *prompt,
                                 char *response)
{
    short line;
    char *promptLine;
    char *scan;
    char *responseLine;
    char *volatile packet;

    packet = 0;
    PromptInsertNumberedDisk(0x3c);
    packet = scan = (char *)FetchDiskPacketRetrying(0x3c, 0, 0);
    responseLine = packet;
    promptLine = packet;
    for (line = 0; line < 20; line++, scan++) {
        if (entry * 2 - line == 0)
            promptLine = scan;
        if (entry * 2 - line == -1)
            responseLine = scan;
        while (*scan != '\n')
            scan++;
        scan[-1] = '\0';
        *scan = '\0';
    }
    DosStrcpy(prompt, promptLine);
    DosStrcpy(response, responseLine);
    FreePacketAndClear((int *)&packet, 0);
}

/* Function start: 0x4259B0 */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
short PromptForAnswerText(short entry)
{
    ShortRect bounds;
    ModalTextPanel panel;
    short matches;

    bounds.left = 50;
    bounds.top = 50;
    bounds.right = 269;
    bounds.bottom = 149;
    ClearViewport(&stScreenView, cBlackColour);
    LoadAnswerPromptAndResponse(entry, szHudMessageBuf,
                                szComponentHitMessage);
    ApplyAnswerTextCipher(szHudMessageBuf, 1);
    InitializeModalTextPanel(&panel, 0, bounds,
                             cViewportClearColour,
                             cBlackColour, cRedColour);
    DrawModalTextPanel(&panel, 6, 10, 2,
                       szHudMessageBuf);
    szComponentHitMessage[20] =
        ""[0];
    PromptForTextInput(65, 125, (char *)"Answer: ",
                       &szComponentHitMessage[20], 10, 2);
    RestoreModalTextPanel(&panel);
    ApplyAnswerTextCipher(&szComponentHitMessage[20], -1);
    matches = (short)(strcmp(&szComponentHitMessage[20],
                             szComponentHitMessage) == 0);
    ReleaseTextFont(0);
    return matches;
}

/* Function start: 0x425AF0 */
void SceneEnterHook(void)
{
}

/* Function start: 0x425B00 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows)
{
    pDebugOverlay =
        new DebugOverlayConsole(module, window, columns, rows, 0);
}

/* Function start: 0x425B90 */
void DestroyGlobalDebugOverlayConsole(void)
{
    delete pDebugOverlay;
#ifdef SDL_PORT
    pDebugOverlay = 0;
#endif
}

/* Function start: 0x425BB0 */
void SystemDebugPrintf(const char *format, ...)
{
#if defined(SDL_PORT) && defined(SDL_PORT_LEGACY_DEBUG_OUTPUT)
    va_list arguments;

    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
#else
    (void)format;
#endif
}

/* Function start: 0x425BC0 */
char PumpMessagesDuringWait(void)
{
    return pDebugOverlay->WaitForKey();
}

/* Function start: 0x425BD0 */
unsigned char TakeDebugStepFlag(void)
{
    unsigned char value = (unsigned char)dwDebugOverlayKeyLatch;

    dwDebugOverlayKeyLatch = 0;
    return value;
}

/* Function start: 0x425BE0 */
void ResetDiskPromptTimerHook(int mode, int interval)
{
    (void)mode;
    (void)interval;
}

/* Function start: 0x425BF0 */
/* No inbound reference is known in the shipped executable; this compiled-out
 * console compatibility routine is believed unreachable. */
void SetConsoleTextColourHook(void)
{
}

/* Function start: 0x425C00 */
/* No inbound reference is known in the shipped executable; this compiled-out
 * console compatibility routine is believed unreachable. */
void SetConsoleBackgroundColourHook(void)
{
}

/* Function start: 0x425C10 */
void DiskPromptDrawHook(void)
{
}

/* Function start: 0x425C20 */
void ClearDebugPauseFlags(void)
{
    dwDebugOverlayKeyLatch = 0;
    dwDebugOverlayKey = 0;
}
