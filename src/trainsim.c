/*
 *  Training-simulator menus, pilot text input, high scores, and mission loop.
 *
 *  Address range 425c30-4272ef (see docs/ORDER.md). The initialized panel,
 *  resource tables, and state precede this unit's name and UI literal pool.
 */
#define WC1_TRAINSIM_SOURCE
#include "wc1.h"

/* Nothing in the shipped executable references this word. The panel that
 * follows is eight-byte aligned, which accounts for the next four zeros. */
int DAT_00469da0 = 10; /* 0x00469da0 */
Viewport stTrainSimPanelViewport = {0}; /* 0x00469da8 */
ShortRect stTrainSimPanelBounds = {
    0x30, 0x1d, 0x110, 0x6d
}; /* 0x00469dc0 */
char szTrainSimTitle[24] = "SQUADRON: TRAINSIM"; /* 0x00469dc8 */
unsigned char *apszBuiltInHighScoreNames[6] = {
    "BISHOP", "GOBLIN", "JEFFTEP", "MANGLER", "THE MAN", "MONGO"
}; /* 0x00469de0 */
TitleMenuRegion aTrainSimMissionRegions[5] = {
    { 1, 47, 29, 67, 49 },
    { 1, 47, 89, 67, 109 },
    { 1, 251, 29, 271, 49 },
    { 1, 251, 89, 271, 109 },
    { -1, 0, 0, 0, 0 }
}; /* 0x00469df8 */
int nTrainSimActive = 0; /* 0x00469e2c */
short nTrainSimMission = 0; /* 0x00469e30 */
short nArcadeWave = 0; /* 0x00469e34 */

Viewport stTrainSimulatorHighScoreBufView; /* 0x005a7bb0 */
int nCurrentArcadeScore; /* 0x005a7bc4 */
TextContext stTrainSimTxtContext; /* 0x005a7bd0 */
Viewport stTrainSimHighScoreDisplayViewport; /* 0x005a7bf0 */
TextContext stTrainSimHighScoreTextContext; /* 0x005a7c10 */
short nArcadeTimeRemaining; /* 0x005a7c2c */
HighScoreEntry aHighScoreEntriesArray[6]; /* 0x005a7c30 */
int nArcadeWaveBonusValue; /* 0x005a7c50 */

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
        &stModalSrcViewport,
        stTrainSimPanelBounds.left,
        stTrainSimPanelBounds.top,
        stTrainSimPanelBounds.right,
        stTrainSimPanelBounds.bottom,
        cBlackColour);
    *(ShortRect *)&stTrainSimHighScoreDisplayViewport.left =
        stTrainSimPanelBounds;
    CopyViewportContents(
        &stTrainSimulatorHighScoreBufView,
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

    completed = 0;
    DrawFilledViewportRect(
        &stModalSrcViewport,
        stTrainSimPanelBounds.left,
        stTrainSimPanelBounds.top,
        stTrainSimPanelBounds.right,
        stTrainSimPanelBounds.bottom,
        cBlackColour);
    DIBslam();
    DIBslamReal();
    stCurrentTrainSimTitleDisplayViewport.top = (short)(0x6b - ReadWord(
        (unsigned short *)stTrainSimTxtContext.font));
    stCurrentTrainSimTitleDisplayViewport.bottom =
        (short)(stCurrentTrainSimTitleDisplayViewport.top + ReadWord(
            (unsigned short *)stTrainSimTxtContext.font) + 2);
    if (CheckEscaped() == 0) {
        do {
            SetFrameTimerPeriodDirect(3);
            if (stCurrentTrainSimTitleDisplayViewport.top <=
                stTrainSimPanelBounds.top) {
                completed = 1;
                break;
            }
            stCurrentTrainSimTitleDisplayViewport.top--;
            stCurrentTrainSimTitleDisplayViewport.bottom--;
            CopyViewportContents(
                &stTrainSimPanelViewport,
                &stCurrentTrainSimTitleDisplayViewport);
            DIBslam();
            DIBslamReal();
            WaitForFrameTick();
        } while (CheckEscaped() == 0);
    }
    return completed;
}

/* Function start: 0x425DF0 */
unsigned char *GetHighScoreEntry(short i)
{
    short k = (short)aHighScoreEntriesArray[i].pilotIndex;

    if (8 < k)
        return apszBuiltInHighScoreNames[k - 9];
    return (unsigned char *)aPilotEntriesList[k].callsign;
}

/* Function start: 0x425E20 */
unsigned int GetHighScoreValue(short i)
{
    return aHighScoreEntriesArray[i].score;
}

/* Function start: 0x425E30 */
unsigned int SetHighScoreEntry(short i, unsigned char b, unsigned int v)
{
    aHighScoreEntriesArray[i].pilotIndex = (signed char)b;
    return aHighScoreEntriesArray[i].score = v;
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
                if ((int)aHighScoreEntriesArray[outer].score <
                    (int)aHighScoreEntriesArray[inner].score) {
                    temporary = aHighScoreEntriesArray[inner];
                    aHighScoreEntriesArray[inner] =
                        aHighScoreEntriesArray[outer];
                    aHighScoreEntriesArray[outer] = temporary;
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
        if (aHighScoreEntriesArray[index].pilotIndex == pilot)
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
                if (aHighScoreEntriesArray[previous].pilotIndex ==
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
    return aHighScoreEntriesArray[i].pilotIndex != -1;
}

/* Function start: 0x426000 */
void AddRandomTrainSimHighScores(void)
{
    short remaining;
    short pilot;
    short scale;
    short slot;
    int scoreRange;
    unsigned int score;
    int baseScore;

    remaining = 3;
    scale = 1;
    do {
        do {
            pilot = RandomInRange(0, 14);
        } while (pilot == 8 ||
                 (pilot <= 8 &&
                  stCampaignState
                          .personalityDeathMission[pilot] != 0) ||
                 (FindTrainSimHighScore(pilot) == -1 &&
                  RandomBelow(100) > 20));
        slot = FindTrainSimHighScore(pilot);
        if (slot == -1)
            slot = RandomInRange(0, 5);
        baseScore = (int)aHighScoreEntriesArray[slot].score;
        scoreRange =
            (int)aHighScoreEntriesArray[0].score - baseScore + 2000;
        while (scoreRange / scale > 30000)
            scale = (short)(scale * 2);
        score = (unsigned int)((int)RandomBelowOrEqual(
                                   (short)(scoreRange / scale)) *
                                   scale +
                               baseScore + 50);
        InsertTrainSimHighScore(pilot, score);
        remaining--;
    } while (remaining != 0);
}

/* Function start: 0x4260E0 */
void DrawTextInputCursor(char character)
{
    unsigned char savedBackground =
        pCurTextContext->backgroundColour;
    unsigned char colour = pCurTextContext->colour;
    short savedX;
    char cursor[2] = { character, 0 };

    savedX = pCurTextContext->cursorX;
    pCurTextContext->cursorX = (short)(savedX + 1);
    pCurTextContext->backgroundColour = colour;
    DrawFormattedText(cursor);
    pCurTextContext->backgroundColour = savedBackground;
    pCurTextContext->cursorX = savedX;
}

/* Function start: 0x426140 */
void ClearTextInputCharacter(short character)
{
    Viewport clearArea;
    short characterWidth;

    characterWidth = (short)GetFontCharWidth(character);
    clearArea = *pCurTextContext->viewport;
    clearArea.left = pCurTextContext->cursorX;
    clearArea.right = (short)(clearArea.left + characterWidth - 1);
    clearArea.top = pCurTextContext->cursorY;
    clearArea.bottom = (short)(clearArea.top +
        ReadWord((unsigned short *)pCurTextContext->font) - 1);
    LeaveAllocationScope();
    ClearViewport(&clearArea,
                  pCurTextContext->backgroundColour);
    EnterAllocationScope();
}

/* Function start: 0x4261D0 */
void ClearNextTextInputCharacter(short character)
{
    short savedX;

    savedX = pCurTextContext->cursorX;
    pCurTextContext->cursorX++;
    ClearTextInputCharacter(character);
    pCurTextContext->cursorX = savedX;
}

/* Function start: 0x426200 */
short ReadTextInput(char *destination, short maximumLength,
                    short mode)
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
    char input[40];

    accepted = 0;
    savedBackground = pCurTextContext->backgroundColour;
    if (savedBackground == 0xff) {
        pCurTextContext->backgroundColour = cBlackColour;
    }
    savedText = pCurTextContext->text;
    savedViewport = pCurTextContext->viewport;
    pCurTextContext->text = input;
    savedX = pCurTextContext->cursorX;
    savedY = pCurTextContext->cursorY;
    inputViewport = *pCurTextContext->viewport;
    pCurTextContext->viewport = &inputViewport;
    DosStrcpy(input, destination);
    inputLength = DosStrlen(input);

    inputViewport.left = savedX;
    inputViewport.top = savedY;
    inputViewport.bottom = (short)(inputViewport.top +
        ReadWord((unsigned short *)pCurTextContext->font));
    inputViewport.right = (short)(inputViewport.left +
        MeasureTextPixelWidthClamped(input));
    ClearViewport(&inputViewport,
                  pCurTextContext->backgroundColour);
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
            switch (key) {
            case 13:
                handled++;
                if (input[0] == 0)
                    return 0;
                accepted++;
                ClearNextTextInputCharacter(' ');
                break;
            case 27:
                ClearNextTextInputCharacter(' ');
                inputViewport.left = savedX;
                inputViewport.top = savedY;
                inputViewport.bottom = (short)(inputViewport.top +
                    ReadWord((unsigned short *)
                        pCurTextContext->font));
                inputViewport.right = (short)(inputViewport.left +
                    MeasureTextPixelWidthClamped(input));
                ClearViewport(&inputViewport,
                              pCurTextContext->backgroundColour);
                pCurTextContext->cursorX = savedX;
                pCurTextContext->text = savedText;
                pCurTextContext->viewport = savedViewport;
                return 0;
            default:
                if (key == 8 && inputLength != 0) {
                    inputLength--;
                    handled++;
                    ClearNextTextInputCharacter(' ');
                    EraseLastTextInputCharacter();
                    DrawTextInputCursor(' ');
                    input[inputLength] = 0;
                } else {
                    if (maximumLength > inputLength &&
                        ((key >= 'A' && key <= 'Z') ||
                         (key >= 'a' && key <= 'z') ||
                         (key >= '0' && key <= '9') ||
                         (key == ' ' && inputLength != 0))) {
                        switch (mode) {
                        case 1:
                            key = (unsigned char)toupper(key);
                            break;
                        case 2:
                            if (key < '0' || key > '9')
                                key = 0;
                            break;
                        default:
#ifdef SDL_PORT
                            if (isalpha((unsigned char)key)) {
#else
                            if (__mb_cur_max > 1
                                    ? _isctype(key, _ALPHA)
                                    : (_pctype[key] & _ALPHA)) {
#endif
                                key = (unsigned char)(key | 0x20);
                                if (GetShiftKeyState() != 0)
                                    key &= 0xdf;
                            }
                            break;
                        }
                        if (key == 0)
                            goto redraw;
                        ClearNextTextInputCharacter(' ');
                        input[inputLength++] = (char)key;
                        input[inputLength] = 0;
                        SetTextCursor((unsigned short)savedX,
                                      (unsigned short)savedY);
                        DrawFormattedText(input);
                        DrawTextInputCursor(' ');
                    }
                    handled++;
                }
                break;
            }
redraw:
            DIBslam();
            DIBslamReal();
        } while (handled == 0);
    } while (accepted == 0);

    bKeyEventQueueEnabled = savedFrameState;
    DosStrcpy(destination, input);
    pCurTextContext->text = savedText;
    pCurTextContext->viewport = savedViewport;
    pCurTextContext->backgroundColour = savedBackground;
    return 1;
}

/* Function start: 0x4265A0 */
void ReadRequiredPilotField(short x, short y, const char *label,
                            char *destination, short maximumLength)
{
    stTrainSimTxtContext.alignment = 0;
    SetTextCursor((unsigned short)(stTrainSimPanelViewport.left + x),
                  (unsigned short)y);
    DrawFormattedText(label);
    do {
        DosStrcpy(destination, "");
    } while (ReadTextInput(destination, maximumLength, 0) == 0);
}

/* Function start: 0x426600 */
void PromptForPilotField(short x, short y, const char *label,
                         char *destination, short maximumLength,
                         const char *defaultText)
{
    stTrainSimTxtContext.alignment = 0;
    x += stTrainSimPanelViewport.left;
    SetTextCursor((unsigned short)x, (unsigned short)y);
    DrawFormattedText(label);
    do {
        DosStrcpy(destination, defaultText);
    } while (ReadTextInput(destination, maximumLength, 0) == 0);
}

/* Function start: 0x426660 */
void InitializeTrainSimTextPanel(void)
{
    stTrainSimPanelViewport = stScreenView;
    *(ShortRect *)&stTrainSimPanelViewport.left =
        stTrainSimPanelBounds;
    ClearViewport(&stTrainSimPanelViewport, cBlackColour);
    stTrainSimTxtContext.text = szDefaultTxtBuf;
    ResetStringBuilder(&stTrainSimTxtContext);
    SetTextContext(&stTrainSimTxtContext);
    InitializeTextContextFromFont(&stTrainSimTxtContext, 1,
                                  cDefaultTextColour,
                                  (signed char)cBlackColour);
    stTrainSimTxtContext.viewport =
        &stTrainSimPanelViewport;
    EraseTextContextBackground(&stTrainSimTxtContext);
}

/* Function start: 0x426700 */
void ShowTrainSimTextMessage(const char *message)
{
    SetTextCursor(
        (unsigned short)stTrainSimPanelViewport.left,
        (unsigned short)(stTrainSimPanelViewport.top + 2));
    stTrainSimTxtContext.alignment = 2;
    FormatTextBufferFromStart(message);
    FormatTextBufferFromStart("%P");
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x426750 */
void EnterPilotNameAndCallsign(void)
{
    unsigned char *backdrop;

    ClearViewport(&stScreenView, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        (short)cCockpitLogicalFile, 0, 0);
    DrawSpriteDefault(&stScreenView, 0, 0, backdrop, 0);
    InitializeTrainSimTextPanel();
    ShowTrainSimTextMessage("CONGRATULATIONS!\n"
                            "YOU HAVE A TOP SCORE!\n"
                            "PLEASE ENTER YOUR\n"
                            "NAME AND CALLSIGN:\n");
    PromptForPilotField(10,
                        (short)(stTrainSimTxtContext.cursorY + 2),
                        "LAST NAME: ",
                        stCampaignState.currentPilot->name,
                        13, "Blair");
    PromptForPilotField(10,
                        (short)(stTrainSimTxtContext.cursorY + 10),
                        "CALLSIGN : ",
                        stCampaignState.currentPilot->callsign,
                        13, "Maverick");
    if (nOriginDevUnlock != 0) {
        memcpy(stCampaignState.currentPilot->callsign,
               "CHEATER", 8);
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
        previousScore = aHighScoreEntriesArray[5].score;
    else
        previousScore = aHighScoreEntriesArray[slot].score;
    slot = -1;
    if (score > (int)previousScore)
        slot = InsertTrainSimHighScore(8, (unsigned int)score);

    if (bCampaignStartupMode != 0) {
        EnterPilotNameAndCallsign();
        return;
    }
    InitializeTrainSimTextPanel();
    if (slot != -1)
        sprintf(message, "*******\n"
                         "CONGRATULATIONS!\n"
                         "YOU HAVE SCORE NUMBER\n"
                         ">>>> %d <<<<\n"
                         "*******",
                slot + 1);
    else
        sprintf(message, "> SORRY <\n\n"
                         "YOUR SCORE IS ONLY\n"
                         "%ld0\n\n"
                         "PLEASE PLAY AGAIN!", score);
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
    ClearViewport(&stScreenView, 0);
    backdrop = (unsigned char *)FetchDiskPacketRetrying(
        (short)cCockpitLogicalFile, 0, 0);
    DrawSpriteDefault(&stScreenView, 0, 0, backdrop, 0);
    InitializeTextContextFromFont(&stTrainSimTxtContext, 1,
                                  cDefaultTextColour,
                                  (signed char)cBlackColour);
    SetTextContext(&stTrainSimTxtContext);
    SetViewportRect(&stTrainSimPanelViewport,
                    0, 0, 319, 199);
    stTrainSimTxtContext.text =
        szDefaultTxtBuf;
    ResetStringBuilder(&stTrainSimTxtContext);
    titleWidth = (short)((MeasureTextPixelWidthClamped(
        szTrainSimTitle) & 0xfff8) + 8);
    stTrainSimPanelViewport.right = titleWidth;
    stTrainSimPanelViewport.bottom =
        (short)(ReadWord((unsigned short *)
            stTrainSimTxtContext.font) + 2);
    AllocateViewport(&stTrainSimPanelViewport,
                     cBlackColour, 0);
    stTrainSimTxtContext.viewport =
        &stTrainSimPanelViewport;
    EraseTextContextBackground(&stTrainSimTxtContext);
    SetTextCursor(0, 1);
    DrawFormattedText(szTrainSimTitle);
    lineHeight = (short)ReadWord((unsigned short *)
        stTrainSimTxtContext.font);

    stCurrentTrainSimTitleDisplayViewport = stScreenView;
    *(ShortRect *)&stCurrentTrainSimTitleDisplayViewport.left =
        stTrainSimPanelBounds;
    ClearViewport(&stCurrentTrainSimTitleDisplayViewport,
                  cBlackColour);
    titleLeft = (short)((160 - titleWidth / 2) & 0xfffe);
    stCurrentTrainSimTitleDisplayViewport.left = titleLeft;
    stCurrentTrainSimTitleDisplayViewport.right =
        (short)(titleLeft + titleWidth);
    stCurrentTrainSimTitleDisplayViewport.top =
        (short)(RandomInRange(0, 0x4e) + 0x1d);
    stCurrentTrainSimTitleDisplayViewport.bottom =
        (short)(stCurrentTrainSimTitleDisplayViewport.top +
                lineHeight + 2);
    if (stCurrentTrainSimTitleDisplayViewport.bottom > 109) {
        stCurrentTrainSimTitleDisplayViewport.bottom = 109;
    }

    InitializeTextContextFromFont(
        &stTrainSimHighScoreTextContext, 1,
        cDefaultTextColour,
        (signed char)cBlackColour);
    *(ShortRect *)&stTrainSimulatorHighScoreBufView.left =
        stTrainSimPanelBounds;
    AllocateViewport(&stTrainSimulatorHighScoreBufView,
                     cBlackColour, 0);
    stTrainSimHighScoreDisplayViewport = stScreenView;
    stTrainSimHighScoreTextContext.viewport =
        &stTrainSimulatorHighScoreBufView;
    stTrainSimHighScoreTextContext.text =
        szDefaultTxtBuf;
    *(ShortRect *)&stTrainSimHighScoreDisplayViewport.left =
        stTrainSimPanelBounds;
    lineHeight = (short)(lineHeight + 3);
    SetTextContext(&stTrainSimHighScoreTextContext);
    ResetStringBuilder(&stTrainSimHighScoreTextContext);
    EraseTextContextBackground(&stTrainSimHighScoreTextContext);
    SetTextCursor(
        (unsigned short)stTrainSimulatorHighScoreBufView.left,
        (unsigned short)(
            stTrainSimulatorHighScoreBufView.top + 1));
    FormatTextBufferFromStart("%JHIGH SCORES%P", 2);
    stTrainSimHighScoreTextContext.alignment = 0;
    row = 0;
    do {
        if (IsHighScoreSlotUsed(row)) {
            ResetStringBuilder(&stTrainSimHighScoreTextContext);
            sprintf(score, "%0ld",
                    GetHighScoreValue(row));
            DrawFormattedText("%X%Y%d. %s%X%s0",
                stTrainSimulatorHighScoreBufView.left + 10,
                lineHeight * (row + 1) +
                    stTrainSimulatorHighScoreBufView.top + 1,
                row + 1, GetHighScoreEntry(row),
                stTrainSimulatorHighScoreBufView.left + 150,
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
    free_viewport(&stTrainSimulatorHighScoreBufView);
    ReleasePacketHandle(backdrop);
}

/* Function start: 0x426C50 */
unsigned char *LoadTrainSimOpponentShape(int opponent)
{
    cCurrentObjResourceLogicalFile =
        (signed char)(opponent + 0x16);
    return (unsigned char *)FetchDiskPacketRetrying(
        (short)cCurrentObjResourceLogicalFile, 1, 0);
}

/* Function start: 0x426C70 */
short SelectTrainSimMission(short *mission)
{
    InputEventState event;
    Viewport menuViewport;
    ShortPoint positions[4];
    unsigned char *shapes[4];
    unsigned short cancelled;
    short savedInputMode;
    short eventType;
    int key;
    signed char selection;
    signed char region;
    int regionIndex;
    unsigned char activate;

    cancelled = 0;
    selection = 0;
    SetTextContext(&stTrainSimTxtContext);
    InitializeTextContextFromFont(
        &stTrainSimTxtContext, 1,
        cDefaultTextColour, (signed char)cBlackColour);
    *(ShortRect *)&stCurrentTrainSimTitleDisplayViewport.left =
        stTrainSimPanelBounds;
    stTrainSimTxtContext.viewport =
        &stCurrentTrainSimTitleDisplayViewport;
    EraseTextContextBackground(&stTrainSimTxtContext);
    SetTextCursor(
        (unsigned short)stCurrentTrainSimTitleDisplayViewport.left,
        (unsigned short)(
            stCurrentTrainSimTitleDisplayViewport.top + 30));
    stTrainSimTxtContext.alignment = 2;
    FormatTextBufferFromStart("SELECT\nENEMY%P");

    menuViewport = stScreenView;
    *(ShortRect *)&menuViewport.left = stTrainSimPanelBounds;
    shapes[0] = LoadTrainSimOpponentShape(9);
    shapes[1] = LoadTrainSimOpponentShape(10);
    shapes[2] = LoadTrainSimOpponentShape(11);
    shapes[3] = LoadTrainSimOpponentShape(12);

    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[0], 0,
        shapes[0], 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[0].left,
        positions[0].x, positions[0].y, shapes[0], 0);
    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[1], 2,
        shapes[1], 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[1].left,
        positions[1].x, positions[1].y, shapes[1], 0);
    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[2], 1,
        shapes[2], 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[2].left,
        positions[2].x, positions[2].y, shapes[2], 0);
    AlignSpriteFrameToRectCorner(
        &stTrainSimPanelBounds, &positions[3], 3,
        shapes[3], 0);
    GetShapeFrameBounds(
        &aTrainSimMissionRegions[3].left,
        positions[3].x, positions[3].y, shapes[3], 0);

    DrawSpriteDefault(&menuViewport, positions[0].x, positions[0].y,
                      shapes[0], 0);
    DrawSpriteDefault(&menuViewport, positions[0].x, positions[0].y,
                      shapes[0], 2);
    DrawSpriteDefault(&menuViewport, positions[1].x, positions[1].y,
                      shapes[1], 0);
    DrawSpriteDefault(&menuViewport, positions[1].x, positions[1].y,
                      shapes[1], 2);
    DrawSpriteDefault(&menuViewport, positions[2].x, positions[2].y,
                      shapes[2], 0);
    DrawSpriteDefault(&menuViewport, positions[2].x, positions[2].y,
                      shapes[2], 2);
    DrawSpriteDefault(&menuViewport, positions[3].x, positions[3].y,
                      shapes[3], 0);
    DrawSpriteDefault(&menuViewport, positions[3].x, positions[3].y,
                      shapes[3], 2);

    stMouseCursorState.viewport = &stCurrentTrainSimTitleDisplayViewport;
    SetEventManagerPump(PollMenuInputDevices);
    EventManagerHook(UpdateTrainSimMenuCursor);
    nMenuInputRepeatDelay = 6;
    WarpMouseTo(160, 100);
    EnterAllocationScope();
    savedInputMode = (signed char)bInputModeState;
    bInputModeState = 1;

    do {
        if (bEscapePressedFlag != 0)
            break;
        eventType = PollInputEvent(&event, 0xff);
        switch (eventType) {
        case 2:
select_region:
            region = (signed char)FindMenuRegionAtPoint(
                event.x, event.y,
                aTrainSimMissionRegions);
            regionIndex = region;
            if (regionIndex >= 0 && regionIndex <= 3)
                selection = (signed char)(region + 1);
            break;
        case 3:
        case 5:
            ClearInputKeyStatePreservingModifiers();
            activate = 0;
            key = (short)event.value;
            if (key == 0x1c || key == 0x39) {
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

    if (bEscapePressedFlag != 0)
        cancelled = 1;
    ReleasePacketHandle(shapes[0]);
    ReleasePacketHandle(shapes[1]);
    ReleasePacketHandle(shapes[2]);
    ReleasePacketHandle(shapes[3]);
    bInputModeState = (unsigned char)savedInputMode;
    SetEventManagerPump(0);
    EventManagerHook(0);
    LeaveAllocationScope();
    ReleaseTextFont(1);
    *mission = (short)(selection - 1);
    return cancelled == 0;
}

/* Function start: 0x427080 */
void RunTrainSim(void)
{
    unsigned int savedFrameState;
    short savedCampaign;
    short savedDataSet;
    short proceed;
    int result;

    proceed = 1;
    nArcadeWave = 0;
    nTrainSimMission = 0;
    bInputModeState = 1;
    SetEventManagerPump(PollMenuInputDevices);
    nArcadeWave = 0;
    nCurrentArcadeScore = 0;
    nArcadeBonusCountdown = 0;
    cCurrentCockpitView = 4;
    cCockpitLogicalFile = 21;

    if (bCampaignStartupMode == 0) {
        ShowTrainSimHighScores();
        proceed = SelectTrainSimMission(&nTrainSimMission);
    } else {
        nCurrentArcadeScore = 4000;
        nTrainSimMission = 2;
    }

    if (proceed != 0) {
        nCannedSceneMode = 0;
        nTrainSimActive = 1;
        PreloadMusicTrackHook(20);
        PreloadMusicTrackHook(21);
        PreloadMusicTrackHook(22);
        ResetStringBuilder(&stHudMessageTextContext);
        savedCampaign = stCampaignState.campaignIndex;
        savedDataSet = nCampaignDataSet;
        stCampaignState.campaignIndex = 0;
        nCampaignDataSet = 0;

        while (nTrainSimMission < 4) {
            nTrainSimActive = 1;
            FigureArcadeTime();
            init_mission(0, nTrainSimMission);
            ShowGetReadyScreen();

            if (bCampaignStartupMode != 0) {
                aasShipShield[0][0] = 0;
                aasShipMaximumShieldTable[0][0] = 0;
                acPlayerComponentDamageList[2] = 4;
                aasShipShield[0][1] = 0;
                aasShipMaximumShieldTable[0][1] = 0;
                nArcadeTimeRemaining = 100;
                nCurrentWave = 2;
                acShipDmg[0] = (signed char)(
                    aObjectTypeData[
                        aeObjectTypeList[0]].damageCapacity + 1);
                set_up_next_wave();
                nArcadeTimeRemaining = 25;
            }

            InvalidateVduMode(0);
            InvalidateVduMode(1);
            DIBslam();
            DIBslamReal();
            savedFrameState = bKeyEventQueueEnabled;
            bKeyEventQueueEnabled = 1;
            result = RunSpaceFlight(nArcadeWave);
            if (result != 1) {
                nArcadeState = 4;
                ShowGameOverScreen();
                nTrainSimMission = 4;
            } else {
                if (nTrainSimMission < 3)
                    nArcadeWave = 0;
                else
                    ShowVictoryScreen();
                nTrainSimMission++;
            }
            bKeyEventQueueEnabled = savedFrameState;
        }

        stCampaignState.campaignIndex = savedCampaign;
        nCampaignDataSet = savedDataSet;
        free_all_slots();
        free_cockpit();
        free_3Space();
        ReleaseMusicTrackHook(20);
        ReleaseMusicTrackHook(21);
        ReleaseMusicTrackHook(22);
        UpdateTrainSimHighScores(nCurrentArcadeScore);
        ShowTrainSimHighScores();
    }
    nTrainSimActive = 0;
}
