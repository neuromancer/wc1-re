/*
 *  Pilot name entry, high scores, inter-scene transitions and ownership of
 *  the Win32 developer overlay console.
 *
 *  Address range 0x425000-0x426fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: EnterPilotNameAndCallsign/ShowTrainSimHighScores; string band 0x469D74-0x469F98.
 */
#include "wc1.h"

/* Function start: 0x425AF0 */
void SceneEnterHook(void)
{
}

/* Function start: 0x425B00 */
void CreateDebugOverlayConsole(HINSTANCE module, HWND window,
                               short columns, short rows)
{
    g_pDebugOverlay_004763f0 =
        new DebugOverlayConsole(module, window, columns, rows, 0);
}

/* Function start: 0x425B90 */
void DestroyGlobalDebugOverlayConsole(void)
{
    delete g_pDebugOverlay_004763f0;
}

/* Function start: 0x425BB0 */
void SystemDebugPrintf(const char *, ...)
{
}

/* Function start: 0x425BC0 */
char PumpMessagesDuringWait(void)
{
    return g_pDebugOverlay_004763f0->WaitForKey();
}

/* Function start: 0x425BD0 */
unsigned char TakeDebugStepFlag(void)
{
    unsigned char value = (unsigned char)g_dwDebugOverlayKeyLatch_0046964c;

    g_dwDebugOverlayKeyLatch_0046964c = 0;
    return value;
}

/* Function start: 0x425BE0 */
void ResetDiskPromptTimer(void)
{
}

/* Function start: 0x425C10 */
void DiskPromptDrawHook(void)
{
}

/* Function start: 0x425C20 */
void ClearDebugPauseFlags(void)
{
    g_dwDebugOverlayKeyLatch_0046964c = 0;
    g_dwDebugOverlayKey_00469648 = 0;
}

/* Function start: 0x425C30 */
unsigned int GetDebugKeyState(unsigned int *p)
{
    char c = *(char *)((int)p + 0xd);

    if (*(char *)((int)p + 0xd) == -1)
        c = DAT_0046999c;
    ClearViewport((Viewport *)*p, (unsigned char)c);
    return 0;
}

/* Function start: 0x425DF0 */
unsigned char *GetHighScoreEntry(short i)
{
    short k = (short)g_aHighScoreEntries_005a7c30[i].pilotIndex;

    if (8 < k)
        return g_apszBuiltInHighScoreNames_00469de0[k - 9];
    return (unsigned char *)g_aPilotRecords_005988d0[k].callsign;
}

/* Function start: 0x425E20 */
unsigned int GetHighScoreValue(short i)
{
    return g_aHighScoreEntries_005a7c30[i].score;
}

/* Function start: 0x425E30 */
void SetHighScoreEntry(short i, unsigned char b, unsigned int v)
{
    g_aHighScoreEntries_005a7c30[i].pilotIndex = (signed char)b;
    g_aHighScoreEntries_005a7c30[i].score = v;
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

/* Function start: 0x425ED0 */
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

/* Function start: 0x425EF0 */
short InsertTrainSimHighScore(short pilot, unsigned int score)
{
    if (FindTrainSimHighScore(pilot) == -1)
        SetHighScoreEntry(5, (unsigned char)pilot, score);
    else
        SetHighScoreEntry(FindTrainSimHighScore(pilot),
                          (unsigned char)pilot, score);
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
            candidate = (short)RandomInRange(0, 14);
            if (candidate == 8)
                candidate = -1;
            previous = 0;
            while (candidate != -1 && previous < slot) {
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

/* Function start: 0x425FE0 */
int IsHighScoreSlotUsed(short i)
{
    return g_aHighScoreEntries_005a7c30[i].pilotIndex != -1;
}

/* Function start: 0x4260E0 */
void DrawTextInputCursor(char character)
{
    unsigned char *colour;
    unsigned char savedBackground;
    short savedX;
    char cursor[2];

    savedBackground = g_pCurrentTextContext_0059af8c->backgroundColour;
    colour = &g_pCurrentTextContext_0059af8c->colour;
    cursor[1] = 0;
    cursor[0] = character;
    savedX = g_pCurrentTextContext_0059af8c->cursorX;
    g_pCurrentTextContext_0059af8c->cursorX = (short)(savedX + 1);
    g_pCurrentTextContext_0059af8c->backgroundColour =
        *colour;
    DrawFormattedText(cursor);
    g_pCurrentTextContext_0059af8c->backgroundColour = savedBackground;
    g_pCurrentTextContext_0059af8c->cursorX = savedX;
}

/* Function start: 0x426140 */
void ClearTextInputCharacter(char character)
{
    Viewport clearArea;
    short characterWidth;

    characterWidth = (short)GetFontCharWidth(character);
    clearArea = *g_pCurrentTextContext_0059af8c->viewport;
    clearArea.left = g_pCurrentTextContext_0059af8c->cursorX;
    clearArea.right = (short)(clearArea.left + characterWidth - 1);
    clearArea.top = g_pCurrentTextContext_0059af8c->cursorY;
    clearArea.bottom = (short)(clearArea.top +
        ReadWord((unsigned short *)g_pCurrentTextContext_0059af8c->font) - 1);
    LeaveAllocationScope();
    ClearViewport(&clearArea,
                  g_pCurrentTextContext_0059af8c->backgroundColour);
    EnterAllocationScope();
}

/* Function start: 0x4261D0 */
void ClearNextTextInputCharacter(char character)
{
    short savedX;

    savedX = g_pCurrentTextContext_0059af8c->cursorX;
    g_pCurrentTextContext_0059af8c->cursorX++;
    ClearTextInputCharacter(character);
    g_pCurrentTextContext_0059af8c->cursorX = savedX;
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

    savedBackground = g_pCurrentTextContext_0059af8c->backgroundColour;
    if (savedBackground == 0xff) {
        g_pCurrentTextContext_0059af8c->backgroundColour = DAT_0046999c;
    }
    savedViewport = g_pCurrentTextContext_0059af8c->viewport;
    savedText = g_pCurrentTextContext_0059af8c->text;
    savedX = g_pCurrentTextContext_0059af8c->cursorX;
    savedY = g_pCurrentTextContext_0059af8c->cursorY;
    inputViewport = *savedViewport;
    g_pCurrentTextContext_0059af8c->viewport = &inputViewport;
    g_pCurrentTextContext_0059af8c->text = input;
    DosStrcpy(input, destination);
    inputLength = DosStrlen(input);

    inputViewport.left = savedX;
    inputViewport.top = savedY;
    inputViewport.right = (short)(inputViewport.left +
        MeasureTextPixelWidthClamped(input));
    inputViewport.bottom = (short)(inputViewport.top +
        ReadWord((unsigned short *)g_pCurrentTextContext_0059af8c->font));
    ClearViewport(&inputViewport,
                  g_pCurrentTextContext_0059af8c->backgroundColour);
    inputViewport.right = savedViewport->right;
    DrawFormattedText(input);
    DrawTextInputCursor(' ');

    savedFrameState = DAT_0046505c;
    DAT_0046505c = 0;
    accepted = 0;
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
                        g_pCurrentTextContext_0059af8c->font));
                inputViewport.right = (short)(inputViewport.left +
                    MeasureTextPixelWidthClamped(input));
                ClearViewport(&inputViewport,
                              g_pCurrentTextContext_0059af8c->backgroundColour);
                g_pCurrentTextContext_0059af8c->cursorX = savedX;
                g_pCurrentTextContext_0059af8c->text = savedText;
                g_pCurrentTextContext_0059af8c->viewport = savedViewport;
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
                    } else if (isalpha(key)) {
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
    g_pCurrentTextContext_0059af8c->text = savedText;
    g_pCurrentTextContext_0059af8c->viewport = savedViewport;
    g_pCurrentTextContext_0059af8c->backgroundColour = savedBackground;
    return 1;
}

/* Function start: 0x426600 */
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

/* Function start: 0x426660 */
void InitializeTrainSimTextPanel(void)
{
    g_stTrainSimPanelViewport_00469da8 = DAT_005a6ba0;
    *(ShortRect *)&g_stTrainSimPanelViewport_00469da8.left =
        g_stTrainSimPanelBounds_00469dc0;
    ClearViewport(&g_stTrainSimPanelViewport_00469da8, DAT_0046999c);
    g_stTrainSimTextContext_005a7bd0.text = g_szDefaultTextBuffer_005a7590;
    ResetStringBuilder(&g_stTrainSimTextContext_005a7bd0);
    SetTextContext(&g_stTrainSimTextContext_005a7bd0);
    InitializeTextContextFromFont(&g_stTrainSimTextContext_005a7bd0, 1,
                                  g_cDefaultTextColour_004699cc,
                                  (signed char)DAT_0046999c);
    g_stTrainSimTextContext_005a7bd0.viewport =
        &g_stTrainSimPanelViewport_00469da8;
    GetDebugKeyState((unsigned int *)&g_stTrainSimTextContext_005a7bd0);
}

/* Function start: 0x426700 */
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

/* Function start: 0x426750 */
void EnterPilotNameAndCallsign(void)
{
    unsigned char *backdrop;

    ClearViewport(&DAT_005a6ba0, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        (short)g_cCockpitLogicalFile_005a7c74, 0, 0);
    DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
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
    if (g_nOriginDevUnlock_00469ff4 != 0) {
        memcpy(g_stCampaignState_0059ca50.currentPilot->callsign,
               g_szCheaterCallsign_00469eec, 8);
    }
    ReleasePacketHandle((int)backdrop);
}

/* Function start: 0x426820 */
void UpdateTrainSimHighScores(int score)
{
    short slot;
    unsigned int previousScore;
    const char *format;
    char message[100];

    slot = FindTrainSimHighScore(8);
    previousScore = g_aHighScoreEntries_005a7c30[5].score;
    if (slot != -1)
        previousScore = g_aHighScoreEntries_005a7c30[slot].score;
    slot = -1;
    if ((int)previousScore < score)
        slot = InsertTrainSimHighScore(8, (unsigned int)score);

    if (DAT_004688e0 == 0) {
        InitializeTrainSimTextPanel();
        if (slot == -1) {
            format = g_szLowScoreMessage_00469f38;
        } else {
            score = slot + 1;
            format = g_szHighScoreCongratulations_00469ef4;
        }
        sprintf(message, format, score);
        ShowTrainSimTextMessage(message);
        SetEventManagerPump(PollJoystickButtonEvents);
        DIBslam();
        DIBslamReal();
        WaitForInputKey();
        SetEventManagerPump(PollMenuInputDevices);
        return;
    }
    EnterPilotNameAndCallsign();
}

/* Function start: 0x4268E0 */
void ShowTrainSimHighScores(void)
{
    TextContext context;
    unsigned char *backdrop;
    unsigned char *name;
    char score[20];
    short row;
    short lineHeight;

    SetEventManagerPump(PollJoystickButtonEvents);
    ClearViewport(&DAT_005a6ba0, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        (short)g_cCockpitLogicalFile_005a7c74, 0, 0);
    DrawSpriteDefault(&DAT_005a6ba0, 0, 0, backdrop, 0);
    memset(&context, 0, sizeof(context));
    context.viewport = &DAT_005a6ba0;
    context.text = g_szDefaultTextBuffer_005a7590;
    InitializeTextContextFromFont(&context, 1,
                                  g_cDefaultTextColour_004699cc,
                                  (signed char)DAT_0046999c);
    ResetStringBuilder(&context);
    SetTextContext(&context);
    lineHeight = (short)(ReadWord((unsigned short *)context.font) + 3);
    SetTextCursor(160, 30);
    context.alignment = 2;
    DrawFormattedText(g_szHighScoresHeading_00469f70);
    context.alignment = 0;
    row = 0;
    do {
        if (IsHighScoreSlotUsed(row)) {
            sprintf(score, g_szHighScoreNumberFormat_00469f80,
                    GetHighScoreValue(row));
            name = GetHighScoreEntry(row);
            DrawFormattedText(g_szHighScoreRowFormat_00469f88,
                              58, (short)(48 + lineHeight * row),
                              row + 1, name, 208, score);
        }
        row++;
    } while (row < 6);
    FlushInputEvents();
    DIBslam();
    DIBslamReal();
    WaitForInputKey();
    ReleasePacketHandle((int)backdrop);
    SetEventManagerPump(PollMenuInputDevices);
}

/* Function start: 0x426C50 */
void LoadSceneBackdrop(char n)
{
    DAT_005a86b0 = n + 0x16;
    FetchDiskPacketRetrying((short)DAT_005a86b0, 1, 0);
}
