/*
 *  Pilot name entry, high scores, inter-scene transitions and ownership of
 *  the Win32 developer overlay console.
 *
 *  Address range 0x425000-0x426fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: EnterPilotNameAndCallsign/ShowTrainSimHighScores; string band 0x469D74-0x469F98.
 */
#include "wc1.h"

/* Function start: 0x425730 */
unsigned int WaitForKeyExceptXOrF12(void)
{
    int key;

    bFilteredKeyWaitStarted =
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
    ClearViewport(stConversationTextContext.viewport,
                  cBlackColour);
    SetTextContext(&stConversationTextContext);
    pIntroFont =
        (unsigned char *)FetchDiskPacketRetrying(9, 1, 0);
    print_subtitle(&stSceneBuffer, 0x40, szMeanwhile);
    PanToScreen(&stSceneBuffer, &stScreen);
    FreePacketAndClear((int *)&pIntroFont, 0);
    WaitForSceneAdvance(100, 0);
    ClearViewport(&stModalSourceViewport, cBlackColour);
    DIBslam();
    DIBslamReal();
    SceneDirector(6, pSceneAnimationSceneData,
                  pSceneAnimationTextData);
    SaveGamePalette();
    StopMusic(30);
    FadeViewportPaletteToColour(&stModalSourceViewport,
                                cBlackColour, 1);
    ClearViewport(&stModalSourceViewport, cBlackColour);
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
    char *promptLine;
    char *scan;
    char *responseLine;
    short line;
    char *packet;

    packet = 0;
    PromptInsertNumberedDisk(0x3c);
    line = 0;
    packet = (char *)FetchDiskPacketRetrying(0x3c, 0, 0);
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

/* Function start: 0x4259B0 */
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
    ClearViewport(&stScreen, cBlackColour);
    LoadAnswerPromptAndResponse(entry, szHudMessageBuffer,
                                szComponentHitMessage);
    ApplyAnswerTextCipher(szHudMessageBuffer, 1);
    InitializeModalTextPanel(&panel, 0, bounds[0], bounds[1],
                             cViewportClearColour,
                             cBlackColour, cRedColour);
    DrawModalTextPanel(&panel, 6, 10, 2,
                       szHudMessageBuffer);
    szComponentHitMessage[20] =
        szEmptyAnswerInput[0];
    PromptForTextInput(65, 125, (char *)szAnswerLabel,
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
#ifdef WC1_SDL
    pDebugOverlay = 0;
#endif
}

/* Function start: 0x425BB0 */
void SystemDebugPrintf(const char *format, ...)
{
#ifdef WC1_SDL
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
void ResetDiskPromptTimer(void)
{
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

/* Function start: 0x425C30 */
unsigned short EraseTextContextBackground(TextContext *context)
{
    unsigned short colour;

    colour = context->backgroundColour;
    if (colour == 0xff)
        colour = cBlackColour;
    ClearViewport(context->viewport, colour);
    return 0;
}

/* Function start: 0x425C60 */
int DisplayTrainSimHighScoreTable(short phase)
{
    short completed;

    (void)phase;
    completed = 1;
    DrawFilledViewportRect(
        &stModalSourceViewport,
        stTrainSimPanelBounds.left,
        stTrainSimPanelBounds.top,
        stTrainSimPanelBounds.right,
        stTrainSimPanelBounds.bottom,
        cBlackColour);
    *(ShortRect *)&stTrainSimHighScoreDisplayViewport.left =
        stTrainSimPanelBounds;
    CopyViewportContents(
        &stTrainSimHighScoreBufferViewport,
        &stTrainSimHighScoreDisplayViewport);
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

/* Function start: 0x425D00 */
int AnimateTrainSimTitle(void)
{
    short completed;
    short y;

    completed = 0;
    y = 0x6b;
    DrawFilledViewportRect(
        &stModalSourceViewport,
        stTrainSimPanelBounds.left,
        stTrainSimPanelBounds.top,
        stTrainSimPanelBounds.right,
        stTrainSimPanelBounds.bottom,
        cBlackColour);
    DIBslam();
    DIBslamReal();
    y = (short)(y - ReadWord(
        (unsigned short *)stTrainSimTextContext.font));
    stTrainSimTitleDisplayViewport.top = y;
    stTrainSimTitleDisplayViewport.bottom =
        (short)(y + ReadWord(
            (unsigned short *)stTrainSimTextContext.font) + 2);
    if (CheckEscaped() == 0) {
        for (;;) {
            SetFrameTimerPeriodDirect(3);
            if (stTrainSimTitleDisplayViewport.top <=
                stTrainSimPanelBounds.top)
                break;
            stTrainSimTitleDisplayViewport.top--;
            stTrainSimTitleDisplayViewport.bottom--;
            CopyViewportContents(
                &stTrainSimPanelViewport,
                &stTrainSimTitleDisplayViewport);
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

/* Function start: 0x425DF0 */
unsigned char *GetHighScoreEntry(short i)
{
    short k = (short)aHighScoreEntries[i].pilotIndex;

    if (8 < k)
        return apszBuiltInHighScoreNames[k - 9];
    return (unsigned char *)aPilotRecords[k].callsign;
}

/* Function start: 0x425E20 */
unsigned int GetHighScoreValue(short i)
{
    return aHighScoreEntries[i].score;
}

/* Function start: 0x425E30 */
unsigned int SetHighScoreEntry(short i, unsigned char b, unsigned int v)
{
    aHighScoreEntries[i].pilotIndex = (signed char)b;
    return aHighScoreEntries[i].score = v;
}

/* Function start: 0x425E50 */
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
                if ((int)aHighScoreEntries[outer].score <
                    (int)aHighScoreEntries[inner].score) {
                    temporary = aHighScoreEntries[inner];
                    aHighScoreEntries[inner] =
                        aHighScoreEntries[outer];
                    aHighScoreEntries[outer] = temporary;
                }
                inner++;
            } while (inner < 6);
        }
        outer++;
    } while (outer < 6);
}

/* Function start: 0x425ED0 */
short FindTrainSimHighScore(short pilot)
{
    short index;

    index = 5;
    do {
        if (aHighScoreEntries[index].pilotIndex == pilot)
            return index;
        index--;
    } while (index >= 0);
    return index;
}

/* Function start: 0x425EF0 */
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

/* Function start: 0x425F40 */
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
                if (aHighScoreEntries[previous].pilotIndex ==
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

/* Function start: 0x425FE0 */
short IsHighScoreSlotUsed(short i)
{
    return aHighScoreEntries[i].pilotIndex != -1;
}

/* Function start: 0x426000 */
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
                  stCampaignState
                          .personalityDeathMission[pilot] != 0) ||
                 (FindTrainSimHighScore(pilot) == -1 &&
                  RandomBelow(100) > 20));
        slot = FindTrainSimHighScore(pilot);
        if (slot == -1)
            slot = RandomInRange(0, 5);
        baseScore = (int)aHighScoreEntries[slot].score;
        scoreRange =
            (int)aHighScoreEntries[0].score - baseScore + 2000;
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

/* Function start: 0x4260E0 */
void DrawTextInputCursor(char character)
{
    unsigned char savedBackground =
        pCurrentTextContext->backgroundColour;
    unsigned char colour = pCurrentTextContext->colour;
    short savedX;
    char cursor[2] = { character, 0 };

    savedX = pCurrentTextContext->cursorX;
    pCurrentTextContext->cursorX = (short)(savedX + 1);
    pCurrentTextContext->backgroundColour = colour;
    DrawFormattedText(cursor);
    pCurrentTextContext->backgroundColour = savedBackground;
    pCurrentTextContext->cursorX = savedX;
}

/* Function start: 0x426140 */
void ClearTextInputCharacter(char character)
{
    Viewport clearArea;
    short characterWidth;

    characterWidth = (short)GetFontCharWidth(character);
    clearArea = *pCurrentTextContext->viewport;
    clearArea.left = pCurrentTextContext->cursorX;
    clearArea.right = (short)(clearArea.left + characterWidth - 1);
    clearArea.top = pCurrentTextContext->cursorY;
    clearArea.bottom = (short)(clearArea.top +
        ReadWord((unsigned short *)pCurrentTextContext->font) - 1);
    LeaveAllocationScope();
    ClearViewport(&clearArea,
                  pCurrentTextContext->backgroundColour);
    EnterAllocationScope();
}

/* Function start: 0x4261D0 */
void ClearNextTextInputCharacter(char character)
{
    short savedX;

    savedX = pCurrentTextContext->cursorX;
    pCurrentTextContext->cursorX++;
    ClearTextInputCharacter(character);
    pCurrentTextContext->cursorX = savedX;
}

/* Function start: 0x426200 */
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

    savedBackground = pCurrentTextContext->backgroundColour;
    accepted = 0;
    if (savedBackground == 0xff) {
        pCurrentTextContext->backgroundColour = cBlackColour;
    }
    savedViewport = pCurrentTextContext->viewport;
    savedText = pCurrentTextContext->text;
    pCurrentTextContext->text = input;
    savedX = pCurrentTextContext->cursorX;
    savedY = pCurrentTextContext->cursorY;
    inputViewport = *savedViewport;
    pCurrentTextContext->viewport = &inputViewport;
    DosStrcpy(input, destination);
    inputLength = DosStrlen(input);

    inputViewport.left = savedX;
    inputViewport.top = savedY;
    inputViewport.bottom = (short)(inputViewport.top +
        ReadWord((unsigned short *)pCurrentTextContext->font));
    inputViewport.right = (short)(inputViewport.left +
        MeasureTextPixelWidthClamped(input));
    ClearViewport(&inputViewport,
                  pCurrentTextContext->backgroundColour);
    inputViewport.right = savedViewport->right;
    DrawFormattedText(input);
    DrawTextInputCursor(' ');

    savedFrameState = bKeyEventQueueEnabled;
    bKeyEventQueueEnabled = 0;
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
                        pCurrentTextContext->font));
                inputViewport.right = (short)(inputViewport.left +
                    MeasureTextPixelWidthClamped(input));
                ClearViewport(&inputViewport,
                              pCurrentTextContext->backgroundColour);
                pCurrentTextContext->cursorX = savedX;
                pCurrentTextContext->text = savedText;
                pCurrentTextContext->viewport = savedViewport;
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

    bKeyEventQueueEnabled = savedFrameState;
    DosStrcpy(destination, input);
    pCurrentTextContext->text = savedText;
    pCurrentTextContext->viewport = savedViewport;
    pCurrentTextContext->backgroundColour = savedBackground;
    return 1;
}

/* Function start: 0x4265A0 */
void ReadRequiredPilotField(short x, short y, const char *label,
                            char *destination, short maximumLength)
{
    short cursorX;

    cursorX = (short)(stTrainSimPanelViewport.left + x);
    stTrainSimTextContext.alignment = 0;
    SetTextCursor((unsigned short)cursorX, (unsigned short)y);
    DrawFormattedText(label);
    do {
        DosStrcpy(destination,
                  (const char *)&dwTrainSimStringPadding);
    } while (ReadTextInput(destination, maximumLength, 0) == 0);
}

/* Function start: 0x426600 */
void PromptForPilotField(short x, short y, const char *label,
                         char *destination, short maximumLength,
                         const char *defaultText)
{
    stTrainSimTextContext.alignment = 0;
    SetTextCursor((unsigned short)(stTrainSimPanelViewport.left + x),
                  (unsigned short)y);
    DrawFormattedText(label);
    do {
        DosStrcpy(destination, defaultText);
    } while (ReadTextInput(destination, maximumLength, 0) == 0);
}

/* Function start: 0x426660 */
void InitializeTrainSimTextPanel(void)
{
    stTrainSimPanelViewport = stScreen;
    *(ShortRect *)&stTrainSimPanelViewport.left =
        stTrainSimPanelBounds;
    ClearViewport(&stTrainSimPanelViewport, cBlackColour);
    stTrainSimTextContext.text = szDefaultTextBuffer;
    ResetStringBuilder(&stTrainSimTextContext);
    SetTextContext(&stTrainSimTextContext);
    InitializeTextContextFromFont(&stTrainSimTextContext, 1,
                                  cDefaultTextColour,
                                  (signed char)cBlackColour);
    stTrainSimTextContext.viewport =
        &stTrainSimPanelViewport;
    EraseTextContextBackground(&stTrainSimTextContext);
}

/* Function start: 0x426700 */
void ShowTrainSimTextMessage(const char *message)
{
    SetTextCursor(
        (unsigned short)stTrainSimPanelViewport.left,
        (unsigned short)(stTrainSimPanelViewport.top + 2));
    stTrainSimTextContext.alignment = 2;
    FormatTextBufferFromStart(message);
    FormatTextBufferFromStart(szTextFlushToken);
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x426750 */
void EnterPilotNameAndCallsign(void)
{
    unsigned char *backdrop;

    ClearViewport(&stScreen, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        (short)cCockpitLogicalFile, 0, 0);
    DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
    InitializeTrainSimTextPanel();
    ShowTrainSimTextMessage(szNewPilotPrompt);
    PromptForPilotField(10,
                        (short)(stTrainSimTextContext.cursorY + 2),
                        szPilotNameLabel,
                        stCampaignState.currentPilot->name,
                        13, szDefaultPilotName);
    PromptForPilotField(10,
                        (short)(stTrainSimTextContext.cursorY + 10),
                        szCallsignLabel,
                        stCampaignState.currentPilot->callsign,
                        13, szDefaultCallsign);
    if (nOriginDevUnlock != 0) {
        memcpy(stCampaignState.currentPilot->callsign,
               szCheaterCallsign, 8);
    }
    ReleasePacketHandle(backdrop);
}

/* Function start: 0x426820 */
void UpdateTrainSimHighScores(int score)
{
    short slot;
    unsigned int previousScore;
    char message[100];

    slot = FindTrainSimHighScore(8);
    if (slot == -1)
        previousScore = aHighScoreEntries[5].score;
    else
        previousScore = aHighScoreEntries[slot].score;
    slot = -1;
    if (score > (int)previousScore)
        slot = InsertTrainSimHighScore(8, (unsigned int)score);

    if (bCampaignStartupMode != 0) {
        EnterPilotNameAndCallsign();
        return;
    }
    InitializeTrainSimTextPanel();
    if (slot != -1)
        sprintf(message, szHighScoreCongratulations,
                slot + 1);
    else
        sprintf(message, szLowScoreMessage, score);
    ShowTrainSimTextMessage(message);
    SetEventManagerPump(PollJoystickButtonEvents);
    DIBslam();
    DIBslamReal();
    WaitForInputKey();
    SetEventManagerPump(PollMenuInputDevices);
}

/* Function start: 0x4268E0 */
void ShowTrainSimHighScores(void)
{
    unsigned char *backdrop;
    char score[20];
    short titleWidth;
    short lineHeight;
    short titleLeft;
    short row;

    SetEventManagerPump(PollJoystickButtonEvents);
    ClearViewport(&stScreen, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        (short)cCockpitLogicalFile, 0, 0);
    DrawSpriteDefault(&stScreen, 0, 0, backdrop, 0);
    InitializeTextContextFromFont(&stTrainSimTextContext, 1,
                                  cDefaultTextColour,
                                  (signed char)cBlackColour);
    SetTextContext(&stTrainSimTextContext);
    SetViewportRect(&stTrainSimPanelViewport,
                    0, 0, 319, 199);
    stTrainSimTextContext.text =
        szDefaultTextBuffer;
    ResetStringBuilder(&stTrainSimTextContext);
    titleWidth = (short)((MeasureTextPixelWidthClamped(
        szTrainSimTitle) & 0xfff8) + 8);
    stTrainSimPanelViewport.right = titleWidth;
    stTrainSimPanelViewport.bottom =
        (short)(ReadWord((unsigned short *)
            stTrainSimTextContext.font) + 2);
    AllocateViewport(&stTrainSimPanelViewport,
                     cBlackColour, 0);
    stTrainSimTextContext.viewport =
        &stTrainSimPanelViewport;
    EraseTextContextBackground(&stTrainSimTextContext);
    SetTextCursor(0, 1);
    DrawFormattedText(szTrainSimTitle);
    lineHeight = (short)ReadWord((unsigned short *)
        stTrainSimTextContext.font);

    stTrainSimTitleDisplayViewport = stScreen;
    *(ShortRect *)&stTrainSimTitleDisplayViewport.left =
        stTrainSimPanelBounds;
    ClearViewport(&stTrainSimTitleDisplayViewport,
                  cBlackColour);
    titleLeft = (short)((160 - titleWidth / 2) & 0xfffe);
    stTrainSimTitleDisplayViewport.left = titleLeft;
    stTrainSimTitleDisplayViewport.right =
        (short)(titleLeft + titleWidth);
    stTrainSimTitleDisplayViewport.top =
        (short)(RandomInRange(0, 0x4e) + 0x1d);
    stTrainSimTitleDisplayViewport.bottom =
        (short)(stTrainSimTitleDisplayViewport.top +
                lineHeight + 2);
    if (stTrainSimTitleDisplayViewport.bottom > 109) {
        stTrainSimTitleDisplayViewport.bottom = 109;
    }

    row = 0;
    InitializeTextContextFromFont(
        &stTrainSimHighScoreTextContext, 1,
        cDefaultTextColour,
        (signed char)cBlackColour);
    *(ShortRect *)&stTrainSimHighScoreBufferViewport.left =
        stTrainSimPanelBounds;
    AllocateViewport(&stTrainSimHighScoreBufferViewport,
                     cBlackColour, 0);
    stTrainSimHighScoreDisplayViewport = stScreen;
    lineHeight = (short)(lineHeight + 3);
    stTrainSimHighScoreTextContext.viewport =
        &stTrainSimHighScoreBufferViewport;
    stTrainSimHighScoreTextContext.text =
        szDefaultTextBuffer;
    *(ShortRect *)&stTrainSimHighScoreDisplayViewport.left =
        stTrainSimPanelBounds;
    SetTextContext(&stTrainSimHighScoreTextContext);
    ResetStringBuilder(&stTrainSimHighScoreTextContext);
    EraseTextContextBackground(&stTrainSimHighScoreTextContext);
    SetTextCursor(
        (unsigned short)stTrainSimHighScoreBufferViewport.left,
        (unsigned short)(
            stTrainSimHighScoreBufferViewport.top + 1));
    FormatTextBufferFromStart(szHighScoresHeading, 2);
    stTrainSimHighScoreTextContext.alignment = 0;
    do {
        if (IsHighScoreSlotUsed(row)) {
            ResetStringBuilder(&stTrainSimHighScoreTextContext);
            sprintf(score, szHighScoreNumberFormat,
                    GetHighScoreValue(row));
            DrawFormattedText(szHighScoreRowFormat,
                stTrainSimHighScoreBufferViewport.left + 10,
                lineHeight * (row + 1) +
                    stTrainSimHighScoreBufferViewport.top + 1,
                row + 1, GetHighScoreEntry(row),
                stTrainSimHighScoreBufferViewport.left + 150,
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
    free_viewport(&stTrainSimPanelViewport);
    free_viewport(&stTrainSimHighScoreBufferViewport);
    ReleasePacketHandle(backdrop);
}

/* Function start: 0x426C50 */
unsigned char *LoadTrainSimOpponentShape(int opponent)
{
    cObjectResourceLogicalFile =
        (signed char)(opponent + 0x16);
    return (unsigned char *)FetchDiskPacketRetrying(
        (short)cObjectResourceLogicalFile, 1, 0);
}

/* Function start: 0x426C70 */
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
    SetTextContext(&stTrainSimTextContext);
    InitializeTextContextFromFont(
        &stTrainSimTextContext, 1,
        cDefaultTextColour, (signed char)cBlackColour);
    *(ShortRect *)&stTrainSimTitleDisplayViewport.left =
        stTrainSimPanelBounds;
    stTrainSimTextContext.viewport =
        &stTrainSimTitleDisplayViewport;
    EraseTextContextBackground(&stTrainSimTextContext);
    SetTextCursor(
        (unsigned short)stTrainSimTitleDisplayViewport.left,
        (unsigned short)(
            stTrainSimTitleDisplayViewport.top + 30));
    stTrainSimTextContext.alignment = 2;
    FormatTextBufferFromStart(szSelectEnemy);

    menuViewport = stScreen;
    *(ShortRect *)&menuViewport.left = stTrainSimPanelBounds;
    topLeftShape = LoadTrainSimOpponentShape(9);
    bottomLeftShape = LoadTrainSimOpponentShape(10);
    topRightShape = LoadTrainSimOpponentShape(11);
    bottomRightShape = LoadTrainSimOpponentShape(12);

    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[0], 0,
        topLeftShape, 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[0].left,
        positions[0].x, positions[0].y, topLeftShape, 0);
    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[1], 2,
        bottomLeftShape, 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[1].left,
        positions[1].x, positions[1].y, bottomLeftShape, 0);
    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[2], 1,
        topRightShape, 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[2].left,
        positions[2].x, positions[2].y, topRightShape, 0);
    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[3], 3,
        bottomRightShape, 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[3].left,
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

    stMouseCursorState.viewport = &stTrainSimTitleDisplayViewport;
    SetEventManagerPump(PollMenuInputDevices);
    EventManagerHook(UpdateTrainSimMenuCursor);
    nMenuInputRepeatDelay = 6;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    savedInputMode = (signed char)bInputMode;
    bInputMode = 1;

    do {
        if (bEscapePressed != 0)
            break;
        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 2:
select_region:
            region = (signed char)FindMenuRegionAtPoint(
                event.x, event.y,
                aTrainSimMissionRegions);
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

    if (bEscapePressed != 0)
        cancelled = 1;
    ReleasePacketHandle(topLeftShape);
    ReleasePacketHandle(bottomLeftShape);
    ReleasePacketHandle(topRightShape);
    ReleasePacketHandle(bottomRightShape);
    bInputMode = (unsigned char)savedInputMode;
    SetEventManagerPump(0);
    EventManagerHook(0);
    LeaveAllocationScope();
    ReleaseTextFont(1);
    *mission = (short)(selection - 1);
    return cancelled < 1;
}
