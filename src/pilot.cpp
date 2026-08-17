/*
 *  Pilot name entry, high scores, inter-scene transitions and ownership of
 *  the Win32 developer overlay console.
 *
 *  Address range 0x425000-0x426fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: EnterPilotNameAndCallsign/ShowTrainSimHighScores; string band 0x469D74-0x469F98.
 */
#include "wc1.h"

/* Function start: WC2_UNMAPPED */
unsigned int WaitForKeyExceptXOrF12(void)
{
    int key;

    g_bFilteredKeyWaitStarted_00598c34 =
        g_bFilteredKeyWaitActive_0059a849 = 1;
    do {
        key = PumpMessagesDuringWait();
    } while (key == 'X' || key == VK_F12);
    g_bFilteredKeyWaitActive_0059a849 = 0;
    FlushInputEvents();
    ClearInputKeyState();
    return 0;
}

/* Function start: WC2_UNMAPPED */
void ShowMeanwhileTransition(short scene, short variant)
{
    int track;

    track = variant + 0x21;
    PreloadMusicTrackHook((short)track);
    spacetrack(track, 2, 1);
    InitializeConversationViewport();
    InitializeConversationText();
    LoadSceneAnimationResources(scene, variant);
    ClearViewport(g_stConversationTextContext_005a7760.viewport,
                  g_cSecondaryViewBufferColour_0049cb4c);
    SetTextContext(&g_stConversationTextContext_005a7760);
    g_pIntroFont_005a8960 =
        (unsigned char *)FetchDiskPacketRetrying(
            reinterpret_cast<char *>(static_cast<unsigned int>(9)), 1, 0);
    print_subtitle(&g_stSecondaryViewBuffer_005d2c90, 0x40, g_szMeanwhile_00469d80);
    PanToScreen(&g_stSecondaryViewBuffer_005d2c90, &g_stScreenViewport_005d21a0);
    FreePacketAndClear((int *)&g_pIntroFont_005a8960, 0);
    WaitForSceneAdvance(100, 0);
    ClearViewport(&g_stModalSourceViewport_005d2c50, g_cSecondaryViewBufferColour_0049cb4c);
    DIBslam();
    DIBslamReal();
    SceneDirector(6, g_pSceneAnimationSceneData_005a7c54,
                  g_pSceneAnimationTextData_005a7c5c);
    SaveGamePalette();
    StopMusic(0);
    FadeViewportPaletteToColour(&g_stModalSourceViewport_005d2c50,
                                g_cSecondaryViewBufferColour_0049cb4c, 1);
    ClearViewport(&g_stModalSourceViewport_005d2c50, g_cSecondaryViewBufferColour_0049cb4c);
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

/* Function start: WC2_UNMAPPED */
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

/* Function start: WC2_UNMAPPED */
void LoadAnswerPromptAndResponse(short entry, char *prompt,
                                 char *response)
{
    char *promptLine;
    char *scan;
    char *responseLine;
    short line;
    char *packet;

    packet = 0;
    PromptInsertNumberedDisk(0x3c);
    line = 0;
    packet = (char *)FetchDiskPacketRetrying(
        reinterpret_cast<char *>(static_cast<unsigned int>(0x3c)), 0, 0);
    promptLine = packet;
    scan = packet;
    responseLine = packet;
    do {
        if (entry * 2 - line == 0)
            promptLine = scan;
        if (entry * 2 - line == -1)
            responseLine = scan;
        while (*scan != '\n')
            scan++;
        scan[-1] = '\0';
        *scan = '\0';
        line++;
        scan++;
    } while (line < 20);
    DosStrcpy(prompt, promptLine);
    DosStrcpy(response, responseLine);
    FreePacketAndClear((int *)&packet, 0);
}

/* Function start: WC2_UNMAPPED */
/* No inbound reference is known in the shipped executable; this routine is
 * believed unreachable. */
short PromptForAnswerText(short entry)
{
    unsigned int bounds[2];
    ModalTextPanel panel;
    short matches;

    ((short *)&bounds[0])[0] = 50;
    ((short *)&bounds[0])[1] = 50;
    ((short *)&bounds[1])[0] = 269;
    ((short *)&bounds[1])[1] = 149;
    ClearViewport(&g_stScreenViewport_005d21a0, g_cSecondaryViewBufferColour_0049cb4c);
    LoadAnswerPromptAndResponse(entry, g_szHudMessageBuffer_0059e1c0,
                                g_szComponentHitMessage_005a7e00);
    ApplyAnswerTextCipher(g_szHudMessageBuffer_0059e1c0, 1);
    InitializeModalTextPanel(&panel, 0, bounds[0], bounds[1],
                             g_cViewportClearColour_004699a0,
                             g_cSecondaryViewBufferColour_0049cb4c, DAT_004699ac);
    DrawModalTextPanel(&panel, 6, 10, 2,
                       g_szHudMessageBuffer_0059e1c0);
    g_szComponentHitMessage_005a7e00[20] =
        g_szEmptyAnswerInput_00469d90[0];
    PromptForTextInput(65, 125, (char *)g_szAnswerLabel_00469d94,
                       &g_szComponentHitMessage_005a7e00[20], 10, 2);
    RestoreModalTextPanel(&panel);
    ApplyAnswerTextCipher(&g_szComponentHitMessage_005a7e00[20], -1);
    matches = (short)(strcmp(&g_szComponentHitMessage_005a7e00[20],
                             g_szComponentHitMessage_005a7e00) == 0);
    ReleaseTextFont(0);
    return matches;
}

/* Function start: 0x4333F8 */
void SceneEnterHook(void)
{
}

/* Function start: 0x40FCD0 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows)
{
    g_pDebugOverlay_004a2548 =
        new DebugOverlayConsole(module, window, columns, rows, 0);
}

/* Function start: 0x40FD73 */
void DestroyGlobalDebugOverlayConsole(void)
{
    delete g_pDebugOverlay_004a2548;
#ifdef WC1_SDL
    g_pDebugOverlay_004a2548 = 0;
#endif
}

#pragma function(strcpy)
/* Function start: 0x40FDAD */
void SystemDebugPrintf(const char *format, ...)
{
#if 0
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
#else
    (void)format;
#endif
#else
    va_list arguments;

    va_start(arguments, format);
    if (format != 0)
        vsprintf(g_szSystemDebugBuffer_004a2550, format, arguments);
    else
        strcpy(g_szSystemDebugBuffer_004a2550, "\n");
    va_end(arguments);
    DebugOverlayPrintf(g_pDebugOverlay_004a2548,
                       g_szSystemDebugBuffer_004a2550);
    WriteDebugString(g_szSystemDebugBuffer_004a2550);
#endif
}

/* Function start: 0x40FE23 */
char PumpMessagesDuringWait(void)
{
    return g_pDebugOverlay_004a2548->WaitForKey();
}

/* Function start: 0x40FE3E */
unsigned char TakeDebugStepFlag(void)
{
#if 0
    unsigned char value = (unsigned char)g_dwDebugOverlayKeyLatch_0049cb2c;

    g_dwDebugOverlayKeyLatch_0049cb2c = 0;
    return value;
#endif
    return g_pDebugOverlay_004a2548->TakeStepFlag();
}

/* Function start: 0x40FE59 */
void SetDebugTextPosition(short row, short column)
{
    g_pDebugOverlay_004a2548->SetTextPosition(row, column);
}

/* Function start: 0x40FE7E */
void SetDebugCursorPosition(short column, short row)
{
    g_pDebugOverlay_004a2548->SetCursorPosition(column, row);
}

/* Function start: 0x40FEA3 */
void ResetDebugOverlay(void)
{
    g_pDebugOverlay_004a2548->Reset();
}

/* Function start: 0x40FEBE */
void ClearDebugOverlay(void)
{
    g_pDebugOverlay_004a2548->Clear();
}

/* Function start: 0x40FED9 */
void ClearDebugPauseFlags(void)
{
#if 0
    g_dwDebugOverlayKeyLatch_0049cb2c = 0;
    g_dwDebugOverlayKey_0049cb28 = 0;
#endif
    g_pDebugOverlay_004a2548->ClearPauseFlags();
}

/* Function start: 0x40FF40 */
void DebugOverlayConsole::SetTextPosition(int row, int column)
{
    cursorColumn = column;
    cursorRow = row;
}

/* Function start: 0x40FF70 */
void DebugOverlayConsole::SetCursorPosition(int column, int row)
{
    SetTextPosition(row, column);
}

/* Function start: 0x40FFA0 */
void DebugOverlayConsole::Reset(void)
{
    Clear();
}

/* Function start: 0x40FFC0 */
unsigned char DebugOverlayConsole::TakeStepFlag(void)
{
    DWORD value;

    value = g_dwDebugOverlayKeyLatch_0049cb2c;
    g_dwDebugOverlayKeyLatch_0049cb2c = 0;
    return (unsigned char)value;
}

/* Function start: 0x40FFF0 */
void DebugOverlayConsole::ClearPauseFlags(void)
{
    g_dwDebugOverlayKeyLatch_0049cb2c = 0;
    g_dwDebugOverlayKey_0049cb28 = 0;
}

/* Function start: WC2_UNMAPPED */
unsigned short EraseTextContextBackground(TextContext *context)
{
    unsigned short colour;

    colour = context->backgroundColour;
    if (colour == 0xff)
        colour = g_cSecondaryViewBufferColour_0049cb4c;
    ClearViewport(context->viewport, colour);
    return 0;
}

/* Function start: 0x42C43D */
int DisplayTrainSimHighScoreTable(short phase)
{
    short completed;

    (void)phase;
    completed = 1;
    DrawFilledViewportRect(
        &g_stModalSourceViewport_005d2c50,
        g_stTrainSimPanelBounds_00469dc0.left,
        g_stTrainSimPanelBounds_00469dc0.top,
        g_stTrainSimPanelBounds_00469dc0.right,
        g_stTrainSimPanelBounds_00469dc0.bottom,
        g_cSecondaryViewBufferColour_0049cb4c);
    *(ShortRect *)&g_stTrainSimHighScoreDisplayViewport_005a7bf0.left =
        g_stTrainSimPanelBounds_00469dc0;
    CopyViewportContents(
        &g_stTrainSimHighScoreBufferViewport_005a7bb0,
        &g_stTrainSimHighScoreDisplayViewport_005a7bf0);
    SetFrameTimerPeriodDirect(0x2d0);
    do {
        if ((short)IsFrameTickElapsed() != 0)
            break;
        DIBslam();
        DIBslamReal();
        if (CheckEscaped() != 0) {
            completed = 0;
            break;
        }
    } while (1);
    return completed;
}

/* Function start: WC2_UNMAPPED */
int AnimateTrainSimTitle(void)
{
    short completed;
    short y;

    completed = 0;
    y = 0x6b;
    DrawFilledViewportRect(
        &g_stModalSourceViewport_005d2c50,
        g_stTrainSimPanelBounds_00469dc0.left,
        g_stTrainSimPanelBounds_00469dc0.top,
        g_stTrainSimPanelBounds_00469dc0.right,
        g_stTrainSimPanelBounds_00469dc0.bottom,
        g_cSecondaryViewBufferColour_0049cb4c);
    DIBslam();
    DIBslamReal();
    y = (short)(y - ReadWord(
        (unsigned short *)g_stTrainSimTextContext_005a7bd0.font));
    g_stTrainSimTitleDisplayViewport_005a7b90.top = y;
    g_stTrainSimTitleDisplayViewport_005a7b90.bottom =
        (short)(y + ReadWord(
            (unsigned short *)g_stTrainSimTextContext_005a7bd0.font) + 2);
    if (CheckEscaped() == 0) {
        for (;;) {
            SetFrameTimerPeriodDirect(3);
            if (g_stTrainSimTitleDisplayViewport_005a7b90.top <=
                g_stTrainSimPanelBounds_00469dc0.top)
                break;
            g_stTrainSimTitleDisplayViewport_005a7b90.top--;
            g_stTrainSimTitleDisplayViewport_005a7b90.bottom--;
            CopyViewportContents(
                &g_stTrainSimPanelViewport_00469da8,
                &g_stTrainSimTitleDisplayViewport_005a7b90);
            DIBslam();
            DIBslamReal();
            WaitForFrameTick();
            if (CheckEscaped() != 0)
                return completed;
        }
        completed = 1;
    }
    return completed;
}

/* Function start: WC2_UNMAPPED */
unsigned char *GetHighScoreEntry(short i)
{
    short k = (short)g_aHighScoreEntries_005a7c30[i].pilotIndex;

    if (8 < k)
        return g_apszBuiltInHighScoreNames_00469de0[k - 9];
    return (unsigned char *)g_aPilotRecords_005988d0[k].callsign;
}

/* Function start: WC2_UNMAPPED */
unsigned int GetHighScoreValue(short i)
{
    return g_aHighScoreEntries_005a7c30[i].score;
}

/* Function start: WC2_UNMAPPED */
unsigned int SetHighScoreEntry(short i, unsigned char b, unsigned int v)
{
    g_aHighScoreEntries_005a7c30[i].pilotIndex = (signed char)b;
    return g_aHighScoreEntries_005a7c30[i].score = v;
}

/* Function start: WC2_UNMAPPED */
void SortTrainSimHighScores(void)
{
    HighScoreEntry temporary;
    short outer;
    short inner;

    outer = 0;
    do {
        inner = (short)(outer + 1);
        if (inner < 6) {
            do {
                if ((int)g_aHighScoreEntries_005a7c30[outer].score <
                    (int)g_aHighScoreEntries_005a7c30[inner].score) {
                    temporary = g_aHighScoreEntries_005a7c30[inner];
                    g_aHighScoreEntries_005a7c30[inner] =
                        g_aHighScoreEntries_005a7c30[outer];
                    g_aHighScoreEntries_005a7c30[outer] = temporary;
                }
                inner++;
            } while (inner < 6);
        }
        outer++;
    } while (outer < 6);
}

/* Function start: WC2_UNMAPPED */
short FindTrainSimHighScore(short pilot)
{
    short index;

    index = 5;
    do {
        if (g_aHighScoreEntries_005a7c30[index].pilotIndex == pilot)
            return index;
        index--;
    } while (index >= 0);
    return index;
}

/* Function start: WC2_UNMAPPED */
short InsertTrainSimHighScore(short pilot, unsigned int score)
{
    if (FindTrainSimHighScore(pilot) != -1)
        SetHighScoreEntry(FindTrainSimHighScore(pilot),
                          (unsigned char)pilot, score);
    else
        SetHighScoreEntry(5, (unsigned char)pilot, score);
    SortTrainSimHighScores();
    return FindTrainSimHighScore(pilot);
}

/* Function start: WC2_UNMAPPED */
void InitializeTrainSimHighScores(void)
{
    short slot;
    short candidate;
    short previous;
    int score;

    slot = 0;
    score = (short)RandomBelowOrEqual(2000) + 10000;
    do {
        do {
            do {
                candidate = (short)RandomInRange(0, 14);
            } while (candidate == 8);
            previous = 0;
            while (previous < slot) {
                if (g_aHighScoreEntries_005a7c30[previous].pilotIndex ==
                    candidate)
                    candidate = -1;
                previous++;
            }
        } while (candidate == -1);
        score -= (short)RandomBelowOrEqual(1500) + 100;
        SetHighScoreEntry(slot, (unsigned char)candidate, score);
        slot++;
    } while (slot < 5);
    SetHighScoreEntry(5, 8, 0);
}

/* Function start: WC2_UNMAPPED */
short IsHighScoreSlotUsed(short i)
{
    return g_aHighScoreEntries_005a7c30[i].pilotIndex != -1;
}

/* Function start: WC2_UNMAPPED */
void AddRandomTrainSimHighScores(void)
{
    short remaining;
    short scale;
    short pilot;
    short slot;
    int baseScore;
    int scoreRange;

    remaining = 3;
    scale = 1;
    do {
        do {
            pilot = RandomInRange(0, 14);
        } while (pilot == 8 ||
                 (pilot < 9 &&
                  g_stCampaignState_0059ca50
                          .personalityDeathMission[pilot] != 0) ||
                 (FindTrainSimHighScore(pilot) == -1 &&
                  RandomBelow(100) > 20));
        slot = FindTrainSimHighScore(pilot);
        if (slot == -1)
            slot = RandomInRange(0, 5);
        baseScore = (int)g_aHighScoreEntries_005a7c30[slot].score;
        scoreRange =
            (int)g_aHighScoreEntries_005a7c30[0].score - baseScore + 2000;
        while (scoreRange / scale > 30000)
            scale = (short)(scale * 2);
        InsertTrainSimHighScore(
            pilot,
            (unsigned int)((int)RandomBelowOrEqual(
                               (short)(scoreRange / scale)) *
                               scale +
                           baseScore + 50));
        remaining--;
    } while (remaining != 0);
}

/* Function start: WC2_UNMAPPED */
void DrawTextInputCursor(char character)
{
    unsigned char savedBackground =
        g_pCurrentTextContext_005c8d1c->backgroundColour;
    unsigned char colour = g_pCurrentTextContext_005c8d1c->colour;
    short savedX;
    char cursor[2] = { character, 0 };

    savedX = g_pCurrentTextContext_005c8d1c->cursorX;
    g_pCurrentTextContext_005c8d1c->cursorX = (short)(savedX + 1);
    g_pCurrentTextContext_005c8d1c->backgroundColour = colour;
    DrawFormattedText(cursor);
    g_pCurrentTextContext_005c8d1c->backgroundColour = savedBackground;
    g_pCurrentTextContext_005c8d1c->cursorX = savedX;
}

/* Function start: 0x459348 */
void ClearTextInputCharacter(char character)
{
    Viewport clearArea;
    short characterWidth;

    characterWidth = (short)GetFontCharWidth(character);
    clearArea = *g_pCurrentTextContext_005c8d1c->viewport;
    clearArea.left = g_pCurrentTextContext_005c8d1c->cursorX;
    clearArea.right = (short)(clearArea.left + characterWidth - 1);
    clearArea.top = g_pCurrentTextContext_005c8d1c->cursorY;
    clearArea.bottom = (short)(clearArea.top +
        ReadWord((unsigned short *)g_pCurrentTextContext_005c8d1c->font) - 1);
    SuspendMouseCursor();
    ClearViewport(&clearArea,
                  g_pCurrentTextContext_005c8d1c->backgroundColour);
    ResumeMouseCursor();
}

/* Function start: WC2_UNMAPPED */
void ClearNextTextInputCharacter(char character)
{
    short savedX;

    savedX = g_pCurrentTextContext_005c8d1c->cursorX;
    g_pCurrentTextContext_005c8d1c->cursorX++;
    ClearTextInputCharacter(character);
    g_pCurrentTextContext_005c8d1c->cursorX = savedX;
}

/* Function start: 0x4597E3 */
short ReadTextInput(char *destination, short maximumLength,
                    volatile short mode)
{
    Viewport inputViewport;
    Viewport *savedViewport;
    char *savedText;
    unsigned int savedFrameState;
    int savedX;
    short savedY;
    short inputLength;
    short accepted;
    short handled;
    unsigned char savedBackground;
    unsigned char key;
    unsigned char character;
    char input[40];

    savedBackground = g_pCurrentTextContext_005c8d1c->backgroundColour;
    accepted = 0;
    if (savedBackground == 0xff) {
        g_pCurrentTextContext_005c8d1c->backgroundColour = g_cSecondaryViewBufferColour_0049cb4c;
    }
    savedViewport = g_pCurrentTextContext_005c8d1c->viewport;
    savedText = g_pCurrentTextContext_005c8d1c->text;
    g_pCurrentTextContext_005c8d1c->text = input;
    savedX = g_pCurrentTextContext_005c8d1c->cursorX;
    savedY = g_pCurrentTextContext_005c8d1c->cursorY;
    inputViewport = *savedViewport;
    g_pCurrentTextContext_005c8d1c->viewport = &inputViewport;
    DosStrcpy(input, destination);
    inputLength = DosStrlen(input);

    inputViewport.left = savedX;
    inputViewport.top = savedY;
    inputViewport.bottom = (short)(inputViewport.top +
        ReadWord((unsigned short *)g_pCurrentTextContext_005c8d1c->font));
    inputViewport.right = (short)(inputViewport.left +
        MeasureTextPixelWidthClamped(input));
    ClearViewport(&inputViewport,
                  g_pCurrentTextContext_005c8d1c->backgroundColour);
    inputViewport.right = savedViewport->right;
    DrawFormattedText(input);
    DrawTextInputCursor(' ');

    savedFrameState = DAT_0046505c;
    DAT_0046505c = 0;
    do {
        handled = 0;
        do {
            DIBslam();
            DIBslamReal();
            key = (unsigned char)WaitForStreamInputKey();
            if (key == 13) {
                handled++;
                if (input[0] == 0)
                    return 0;
                accepted++;
                ClearNextTextInputCharacter(' ');
            } else if (key == 27) {
                ClearNextTextInputCharacter(' ');
                inputViewport.left = savedX;
                inputViewport.top = savedY;
                inputViewport.bottom = (short)(inputViewport.top +
                    ReadWord((unsigned short *)
                        g_pCurrentTextContext_005c8d1c->font));
                inputViewport.right = (short)(inputViewport.left +
                    MeasureTextPixelWidthClamped(input));
                ClearViewport(&inputViewport,
                              g_pCurrentTextContext_005c8d1c->backgroundColour);
                g_pCurrentTextContext_005c8d1c->cursorX = savedX;
                g_pCurrentTextContext_005c8d1c->text = savedText;
                g_pCurrentTextContext_005c8d1c->viewport = savedViewport;
                return 0;
            } else if (key == 8 && inputLength != 0) {
                inputLength--;
                handled++;
                ClearNextTextInputCharacter(' ');
                EraseLastTextInputCharacter();
                DrawTextInputCursor(' ');
                input[inputLength] = 0;
            } else {
                if (inputLength < maximumLength &&
                    ((key >= 'A' && key <= 'Z') ||
                     (key >= 'a' && key <= 'z') ||
                     (key >= '0' && key <= '9') ||
                     (key == ' ' && inputLength != 0))) {
                    character = key;
                    if (mode == 1) {
                        character = (unsigned char)toupper(key);
                    } else if (mode == 2) {
                        if (key < '0' || key > '9')
                            character = 0;
                    }
#ifdef WC1_SDL
                    else if (isalpha((unsigned char)key)) {
#else
                    else if (__mb_cur_max > 1
                                 ? _isctype(key, _ALPHA)
                                 : (_pctype[key] & _ALPHA)) {
#endif
                        character = (unsigned char)(key | 0x20);
                        if (GetShiftKeyState() != 0)
                            character &= 0xdf;
                    }
                    if (character == 0)
                        goto redraw;
                    ClearNextTextInputCharacter(' ');
                    input[inputLength++] = (char)character;
                    input[inputLength] = 0;
                    SetTextCursor((unsigned short)savedX,
                                  (unsigned short)savedY);
                    DrawFormattedText(input);
                    DrawTextInputCursor(' ');
                }
                handled++;
            }
redraw:
            DIBslam();
            DIBslamReal();
        } while (handled == 0);
    } while (accepted == 0);

    DAT_0046505c = savedFrameState;
    DosStrcpy(destination, input);
    g_pCurrentTextContext_005c8d1c->text = savedText;
    g_pCurrentTextContext_005c8d1c->viewport = savedViewport;
    g_pCurrentTextContext_005c8d1c->backgroundColour = savedBackground;
    return 1;
}

/* Function start: 0x42075F */
void ReadRequiredPilotField(short x, short y, const char *label,
                            char *destination, short maximumLength)
{
    short cursorX;

    cursorX = (short)(g_stTrainSimPanelViewport_00469da8.left + x);
    g_stTrainSimTextContext_005a7bd0.alignment = 0;
    SetTextCursor((unsigned short)cursorX, (unsigned short)y);
    DrawFormattedText(label);
    do {
        DosStrcpy(destination,
                  (const char *)&g_dwTrainSimStringPadding_00469e68);
    } while (ReadTextInput(destination, maximumLength, 0) == 0);
}

/* Function start: 0x4348C3 */
void PromptForPilotField(short x, short y, const char *label,
                         char *destination, short maximumLength,
                         const char *defaultText)
{
    g_stTrainSimTextContext_005a7bd0.alignment = 0;
    SetTextCursor((unsigned short)(g_stTrainSimPanelViewport_00469da8.left + x),
                  (unsigned short)y);
    DrawFormattedText(label);
    do {
        DosStrcpy(destination, defaultText);
    } while (ReadTextInput(destination, maximumLength, 0) == 0);
}

/* Function start: 0x4347D6 */
void InitializeTrainSimTextPanel(void)
{
    g_stTrainSimPanelViewport_00469da8 = g_stScreenViewport_005d21a0;
    *(ShortRect *)&g_stTrainSimPanelViewport_00469da8.left =
        g_stTrainSimPanelBounds_00469dc0;
    ClearViewport(&g_stTrainSimPanelViewport_00469da8, g_cSecondaryViewBufferColour_0049cb4c);
    g_stTrainSimTextContext_005a7bd0.text = g_szDefaultTextBuffer_005d2b80;
    ResetStringBuilder(&g_stTrainSimTextContext_005a7bd0);
    SetTextContext(&g_stTrainSimTextContext_005a7bd0);
    InitializeTextContextFromFont(&g_stTrainSimTextContext_005a7bd0, 1,
                                  g_cDefaultTextColour_004699cc,
                                  (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    g_stTrainSimTextContext_005a7bd0.viewport =
        &g_stTrainSimPanelViewport_00469da8;
    EraseTextContextBackground(&g_stTrainSimTextContext_005a7bd0);
}

/* Function start: 0x42EBD0 */
void ShowTrainSimTextMessage(const char *message)
{
    SetTextCursor(
        (unsigned short)g_stTrainSimPanelViewport_00469da8.left,
        (unsigned short)(g_stTrainSimPanelViewport_00469da8.top + 2));
    g_stTrainSimTextContext_005a7bd0.alignment = 2;
    FormatTextBufferFromStart(message);
    FormatTextBufferFromStart(g_szTextFlushToken_00469e6c);
    DIBslam();
    DIBslamReal();
}

/* Function start: WC2_UNMAPPED */
void EnterPilotNameAndCallsign(void)
{
    unsigned char *backdrop;

    ClearViewport(&g_stScreenViewport_005d21a0, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        reinterpret_cast<char *>(static_cast<unsigned int>(
            (unsigned short)g_cCockpitLogicalFile_005a7c74)), 0, 0);
    DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
    InitializeTrainSimTextPanel();
    ShowTrainSimTextMessage(g_szNewPilotPrompt_00469e70);
    PromptForPilotField(10,
                        (short)(g_stTrainSimTextContext_005a7bd0.cursorY + 2),
                        g_szPilotNameLabel_00469ec8,
                        g_stCampaignState_0059ca50.currentPilot->name,
                        13, g_szDefaultPilotName_00469ec0);
    PromptForPilotField(10,
                        (short)(g_stTrainSimTextContext_005a7bd0.cursorY + 10),
                        g_szCallsignLabel_00469ee0,
                        g_stCampaignState_0059ca50.currentPilot->callsign,
                        13, g_szDefaultCallsign_00469ed4);
    if (g_nOriginDevUnlock_0049d774 != 0) {
        memcpy(g_stCampaignState_0059ca50.currentPilot->callsign,
               g_szCheaterCallsign_00469eec, 8);
    }
    ReleasePacketHandle(backdrop);
}

/* Function start: WC2_UNMAPPED */
void UpdateTrainSimHighScores(int score)
{
    short slot;
    unsigned int previousScore;
    char message[100];

    slot = FindTrainSimHighScore(8);
    if (slot == -1)
        previousScore = g_aHighScoreEntries_005a7c30[5].score;
    else
        previousScore = g_aHighScoreEntries_005a7c30[slot].score;
    slot = -1;
    if (score > (int)previousScore)
        slot = InsertTrainSimHighScore(8, (unsigned int)score);

    if (DAT_004688e0 != 0) {
        EnterPilotNameAndCallsign();
        return;
    }
    InitializeTrainSimTextPanel();
    if (slot != -1)
        sprintf(message, g_szHighScoreCongratulations_00469ef4,
                slot + 1);
    else
        sprintf(message, g_szLowScoreMessage_00469f38, score);
    ShowTrainSimTextMessage(message);
    SetEventManagerPump(PollJoystickButtonEvents);
    DIBslam();
    DIBslamReal();
    WaitForInputKey();
    SetEventManagerPump(PollMenuInputDevices);
}

/* Function start: WC2_UNMAPPED */
void ShowTrainSimHighScores(void)
{
    unsigned char *backdrop;
    char score[20];
    short titleWidth;
    short lineHeight;
    short titleLeft;
    short row;

    SetEventManagerPump(PollJoystickButtonEvents);
    ClearViewport(&g_stScreenViewport_005d21a0, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        reinterpret_cast<char *>(static_cast<unsigned int>(
            (unsigned short)g_cCockpitLogicalFile_005a7c74)), 0, 0);
    DrawSpriteDefault(&g_stScreenViewport_005d21a0, 0, 0, backdrop, 0);
    InitializeTextContextFromFont(&g_stTrainSimTextContext_005a7bd0, 1,
                                  g_cDefaultTextColour_004699cc,
                                  (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    SetTextContext(&g_stTrainSimTextContext_005a7bd0);
    SetViewportRect(&g_stTrainSimPanelViewport_00469da8,
                    0, 0, 319, 199);
    g_stTrainSimTextContext_005a7bd0.text =
        g_szDefaultTextBuffer_005d2b80;
    ResetStringBuilder(&g_stTrainSimTextContext_005a7bd0);
    titleWidth = (short)((MeasureTextPixelWidthClamped(
        g_szTrainSimTitle_00469dc8) & 0xfff8) + 8);
    g_stTrainSimPanelViewport_00469da8.right = titleWidth;
    g_stTrainSimPanelViewport_00469da8.bottom =
        (short)(ReadWord((unsigned short *)
            g_stTrainSimTextContext_005a7bd0.font) + 2);
    AllocateViewport(&g_stTrainSimPanelViewport_00469da8,
                     g_cSecondaryViewBufferColour_0049cb4c, 0);
    g_stTrainSimTextContext_005a7bd0.viewport =
        &g_stTrainSimPanelViewport_00469da8;
    EraseTextContextBackground(&g_stTrainSimTextContext_005a7bd0);
    SetTextCursor(0, 1);
    DrawFormattedText(g_szTrainSimTitle_00469dc8);
    lineHeight = (short)ReadWord((unsigned short *)
        g_stTrainSimTextContext_005a7bd0.font);

    g_stTrainSimTitleDisplayViewport_005a7b90 = g_stScreenViewport_005d21a0;
    *(ShortRect *)&g_stTrainSimTitleDisplayViewport_005a7b90.left =
        g_stTrainSimPanelBounds_00469dc0;
    ClearViewport(&g_stTrainSimTitleDisplayViewport_005a7b90,
                  g_cSecondaryViewBufferColour_0049cb4c);
    titleLeft = (short)((160 - titleWidth / 2) & 0xfffe);
    g_stTrainSimTitleDisplayViewport_005a7b90.left = titleLeft;
    g_stTrainSimTitleDisplayViewport_005a7b90.right =
        (short)(titleLeft + titleWidth);
    g_stTrainSimTitleDisplayViewport_005a7b90.top =
        (short)(RandomInRange(0, 0x4e) + 0x1d);
    g_stTrainSimTitleDisplayViewport_005a7b90.bottom =
        (short)(g_stTrainSimTitleDisplayViewport_005a7b90.top +
                lineHeight + 2);
    if (g_stTrainSimTitleDisplayViewport_005a7b90.bottom > 109) {
        g_stTrainSimTitleDisplayViewport_005a7b90.bottom = 109;
    }

    row = 0;
    InitializeTextContextFromFont(
        &g_stTrainSimHighScoreTextContext_005a7c10, 1,
        g_cDefaultTextColour_004699cc,
        (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    *(ShortRect *)&g_stTrainSimHighScoreBufferViewport_005a7bb0.left =
        g_stTrainSimPanelBounds_00469dc0;
    AllocateViewport(&g_stTrainSimHighScoreBufferViewport_005a7bb0,
                     g_cSecondaryViewBufferColour_0049cb4c, 0);
    g_stTrainSimHighScoreDisplayViewport_005a7bf0 = g_stScreenViewport_005d21a0;
    lineHeight = (short)(lineHeight + 3);
    g_stTrainSimHighScoreTextContext_005a7c10.viewport =
        &g_stTrainSimHighScoreBufferViewport_005a7bb0;
    g_stTrainSimHighScoreTextContext_005a7c10.text =
        g_szDefaultTextBuffer_005d2b80;
    *(ShortRect *)&g_stTrainSimHighScoreDisplayViewport_005a7bf0.left =
        g_stTrainSimPanelBounds_00469dc0;
    SetTextContext(&g_stTrainSimHighScoreTextContext_005a7c10);
    ResetStringBuilder(&g_stTrainSimHighScoreTextContext_005a7c10);
    EraseTextContextBackground(&g_stTrainSimHighScoreTextContext_005a7c10);
    SetTextCursor(
        (unsigned short)g_stTrainSimHighScoreBufferViewport_005a7bb0.left,
        (unsigned short)(
            g_stTrainSimHighScoreBufferViewport_005a7bb0.top + 1));
    FormatTextBufferFromStart(g_szHighScoresHeading_00469f70, 2);
    g_stTrainSimHighScoreTextContext_005a7c10.alignment = 0;
    do {
        if (IsHighScoreSlotUsed(row)) {
            ResetStringBuilder(&g_stTrainSimHighScoreTextContext_005a7c10);
            sprintf(score, g_szHighScoreNumberFormat_00469f80,
                    GetHighScoreValue(row));
            DrawFormattedText(g_szHighScoreRowFormat_00469f88,
                g_stTrainSimHighScoreBufferViewport_005a7bb0.left + 10,
                lineHeight * (row + 1) +
                    g_stTrainSimHighScoreBufferViewport_005a7bb0.top + 1,
                row + 1, GetHighScoreEntry(row),
                g_stTrainSimHighScoreBufferViewport_005a7bb0.left + 150,
                score);
        }
        row++;
    } while (row < 6);
    FlushInputEvents();
    DIBslam();
    DIBslamReal();
    do {
        if (DisplayTrainSimHighScoreTable(1) == 0)
            break;
    } while (AnimateTrainSimTitle() != 0);
    ReleaseTextFont(1);
    free_viewport(&g_stTrainSimPanelViewport_00469da8);
    free_viewport(&g_stTrainSimHighScoreBufferViewport_005a7bb0);
    ReleasePacketHandle(backdrop);
}

/* Function start: WC2_UNMAPPED */
unsigned char *LoadTrainSimOpponentShape(int opponent)
{
    g_cObjectResourceLogicalFile_005a86b0 =
        (signed char)(opponent + 0x16);
    return (unsigned char *)FetchDiskPacketRetrying(
        reinterpret_cast<char *>(static_cast<unsigned int>(
            (unsigned short)g_cObjectResourceLogicalFile_005a86b0)), 1, 0);
}

/* Function start: 0x4353D4 */
short SelectTrainSimMission(short *mission)
{
    InputEventState event;
    Viewport menuViewport;
    ShortPoint positions[4];
    unsigned char *topLeftShape;
    unsigned char *bottomLeftShape;
    unsigned char *topRightShape;
    unsigned char *bottomRightShape;
    short cancelled;
    short savedInputMode;
    short eventType;
    signed char selection;
    signed char region;
    unsigned char activate;

    cancelled = 0;
    selection = 0;
    SetTextContext(&g_stTrainSimTextContext_005a7bd0);
    InitializeTextContextFromFont(
        &g_stTrainSimTextContext_005a7bd0, 1,
        g_cDefaultTextColour_004699cc, (signed char)g_cSecondaryViewBufferColour_0049cb4c);
    *(ShortRect *)&g_stTrainSimTitleDisplayViewport_005a7b90.left =
        g_stTrainSimPanelBounds_00469dc0;
    g_stTrainSimTextContext_005a7bd0.viewport =
        &g_stTrainSimTitleDisplayViewport_005a7b90;
    EraseTextContextBackground(&g_stTrainSimTextContext_005a7bd0);
    SetTextCursor(
        (unsigned short)g_stTrainSimTitleDisplayViewport_005a7b90.left,
        (unsigned short)(
            g_stTrainSimTitleDisplayViewport_005a7b90.top + 30));
    g_stTrainSimTextContext_005a7bd0.alignment = 2;
    FormatTextBufferFromStart(g_szSelectEnemy_00469f98);

    menuViewport = g_stScreenViewport_005d21a0;
    *(ShortRect *)&menuViewport.left = g_stTrainSimPanelBounds_00469dc0;
    topLeftShape = LoadTrainSimOpponentShape(9);
    bottomLeftShape = LoadTrainSimOpponentShape(10);
    topRightShape = LoadTrainSimOpponentShape(11);
    bottomRightShape = LoadTrainSimOpponentShape(12);

    AlignSpriteFrameToRectCorner(
        &g_stTrainSimPanelBounds_00469dc0, &positions[0], 0,
        topLeftShape, 0);
    GetShapeFrameBounds(
        &g_aTrainSimMissionRegions_00469df8[0].left,
        positions[0].x, positions[0].y, topLeftShape, 0);
    AlignSpriteFrameToRectCorner(
        &g_stTrainSimPanelBounds_00469dc0, &positions[1], 2,
        bottomLeftShape, 0);
    GetShapeFrameBounds(
        &g_aTrainSimMissionRegions_00469df8[1].left,
        positions[1].x, positions[1].y, bottomLeftShape, 0);
    AlignSpriteFrameToRectCorner(
        &g_stTrainSimPanelBounds_00469dc0, &positions[2], 1,
        topRightShape, 0);
    GetShapeFrameBounds(
        &g_aTrainSimMissionRegions_00469df8[2].left,
        positions[2].x, positions[2].y, topRightShape, 0);
    AlignSpriteFrameToRectCorner(
        &g_stTrainSimPanelBounds_00469dc0, &positions[3], 3,
        bottomRightShape, 0);
    GetShapeFrameBounds(
        &g_aTrainSimMissionRegions_00469df8[3].left,
        positions[3].x, positions[3].y, bottomRightShape, 0);

    DrawSpriteDefault(&menuViewport, positions[0].x, positions[0].y,
                      topLeftShape, 0);
    DrawSpriteDefault(&menuViewport, positions[0].x, positions[0].y,
                      topLeftShape, 2);
    DrawSpriteDefault(&menuViewport, positions[1].x, positions[1].y,
                      bottomLeftShape, 0);
    DrawSpriteDefault(&menuViewport, positions[1].x, positions[1].y,
                      bottomLeftShape, 2);
    DrawSpriteDefault(&menuViewport, positions[2].x, positions[2].y,
                      topRightShape, 0);
    DrawSpriteDefault(&menuViewport, positions[2].x, positions[2].y,
                      topRightShape, 2);
    DrawSpriteDefault(&menuViewport, positions[3].x, positions[3].y,
                      bottomRightShape, 0);
    DrawSpriteDefault(&menuViewport, positions[3].x, positions[3].y,
                      bottomRightShape, 2);

    g_stMouseCursorState_0059ab10.viewport = &g_stTrainSimTitleDisplayViewport_005a7b90;
    SetEventManagerPump(PollMenuInputDevices);
    EventManagerHook(UpdateTrainSimMenuCursor);
    g_nMenuInputRepeatDelay_005a8208 = 6;
    WarpMouseTo(160, 100);
    ResumeMouseCursor();
    savedInputMode = (signed char)g_bInputMode_0059a848;
    g_bInputMode_0059a848 = 1;

    do {
        if (g_bSceneEscapeRequested_0049d4b0 != 0)
            break;
        eventType = PollInputEvent(&event);
        switch (eventType) {
        case 2:
select_region:
            region = (signed char)FindMenuRegionAtPoint(
                event.x, event.y,
                g_aTrainSimMissionRegions_00469df8);
            if (region >= 0 && region < 4)
                selection = (signed char)(region + 1);
            break;
        case 3:
        case 5:
            ClearInputKeyStatePreservingModifiers();
            activate = 0;
            if ((short)event.value == 0x1c ||
                (short)event.value == 0x39) {
                activate = 1;
            } else {
                MoveMenuPointerFromKeyboard(&event);
            }
            if (activate != 0)
                goto select_region;
            break;
        case 13:
            UpdateTrainSimMenuCursor();
            break;
        }
        DIBslam();
        DIBslamReal();
    } while (selection == 0);

    if (g_bSceneEscapeRequested_0049d4b0 != 0)
        cancelled = 1;
    ReleasePacketHandle(topLeftShape);
    ReleasePacketHandle(bottomLeftShape);
    ReleasePacketHandle(topRightShape);
    ReleasePacketHandle(bottomRightShape);
    g_bInputMode_0059a848 = (unsigned char)savedInputMode;
    SetEventManagerPump(0);
    EventManagerHook(0);
    SuspendMouseCursor();
    ReleaseTextFont(1);
    *mission = (short)(selection - 1);
    return cancelled < 1;
}
