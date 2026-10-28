/*
 *  Cockpit HUD (Mac `cockpt` compilation unit): weapon, damage, target,
 *  objective, message, and pilot displays.
 *
 *  Address range 413a10-41807f (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac `cockpt` symbol list identifies the surviving
 *  functions throughout this Win32 range; port-specific split helpers remain
 *  interleaved in their original address order.
 */
#define WC1_COCKPT_SOURCE
#include "wc1.h"

static short asVduSelectionSound[2] = { 0x7f, 0 }; /* 0x00469000 */
char *pszPendingHudMessage = 0; /* 0x00469004 */
static char *pszDisplayedHudMessage = 0; /* 0x00469008 */
static signed char cRenderedPilotHandFrame = -1; /* 0x0046900c */
/* Nothing in the shipped executable references these eight zero bytes. */
static int DAT_00469010[2] = { 0, 0 }; /* 0x00469010 */
/* 0x00469018 */
static short asPilotHandOffsets[34] = {
    6, -3, 7, 2, 7, 9, 7, 12, 8, 13, 0, -1, -1, -1,
    -4, -1, -6, -1, 6, 0, 8, 0, 10, 0, 13, 3, 8, -7,
    6, -9, 5, -11, 5, -14
};
unsigned char *pCockpitPilotShape = 0; /* 0x0046905c */
unsigned char *pCockpitExplosionBackground = 0; /* 0x00469060 */
unsigned char *pCockpitExplosionShape = 0; /* 0x00469064 */
short nCockpitExplosionFrame = 8; /* 0x00469068 */
unsigned char *pReleaseWeaponDisplayBackground = 0; /* 0x0046906c */
static signed char cReleaseWeaponDisplayFrame = -1; /* 0x00469070 */
static signed char cReleaseWeaponDisplayTicks = 0; /* 0x00469074 */
static signed char cReleaseWeaponDisplayState = 0; /* 0x00469078 */
static char szNoWeaponName[5] = "None"; /* 0x00469080 */
static int nDisplayedObjectiveRange = 40000; /* 0x00469088 */
#ifdef SDL_PORT
static char *pszObjectiveStatusMessage = szObjectiveStatusMessage;
#else
/* 0x0046908c */
static char *pszObjectiveStatusMessage = "Wait for ??????????????????";
#endif
static short nScannerTargetObject = -1; /* 0x00469090 */
/* 0x00469098 */
static int aiScannerGridRows[79] = {
    5, 13, 16, -1,
    5, 13, 16, -1,
    5, 13, 16, -1,
    4, 13, 16, -1,
    4, 12, 16, -1,
    2, 3, 4, 12, 15, -1,
    0, 1, 5, 12, 15, -1,
    6, 11, 15, -1,
    7, 11, 14, -1,
    8, 10, 14, -1,
    9, 13, -1,
    8, 13, -1,
    6, 7, 12, -1,
    4, 5, 11, -1,
    0, 1, 2, 3, 10, -1,
    9, -1,
    7, 8, -1,
    4, 5, 6, -1,
    0, 1, 2, 3, -1,
    -2
};
/* 0x004691d4 */
static const char *pszMissileLockedMessage = "MISSILE LOCKED ";
static unsigned char bTargetBracketVisible = 1; /* 0x004691d8 */
/* 0x004691e0 */
static short asHudMessageOrigins[10] = {
    18, 14, 71, 5, 80, 29, 49, 27, 14, 13
};
static short nTargetLockMarkerX = -0x7fff; /* 0x004691f4 */
static ShortRect stTargetBracketBounds = {-0x7fff, 0, 0, 0}; /* 0x004691f8 */
static ShortRect stPreviousTargetBracketBounds = {-0x7fff, 0, 0, 0}; /* 0x00469200 */
short nNavPointerObject = -1; /* 0x00469208 */
static Viewport stTrainSimVduSource = {0}; /* 0x00469210 */
/* 0x00469228 */
static ShortPoint aaCockpitDamagePositions[5][4] = {
    {{224, 5}, {132, 96}, {233, 107}, {149, 161}},
    {{177, 6}, {153, 142}, {103, 140}, {55, 183}},
    {{107, 25}, {211, 32}, {21, 178}, {300, 178}},
    {{74, 10}, {294, 19}, {197, 105}, {105, 134}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
unsigned char *pConfedCommBackground = 0; /* 0x00469278 */
unsigned char *pCommStaticShape = 0; /* 0x0046927c */
unsigned char *pKilrathiCommBackground = 0; /* 0x00469280 */
static int nCommPortraitFrame = -1; /* 0x00469284 */
static int bForceDamageDisplayRedraw = 0; /* 0x00469288 */

/* 0x00475ff0 */
static unsigned char abMouseCursorBackground[0x400];

enum ObjectType eReleaseWpnDisplayType; /* 0x005a7dc0 */
unsigned char *pRadarMarkerBackground; /* 0x005a7dc4 */
signed char bStickIndicatorFrameState; /* 0x005a7dc8 */
short nCurrentNoticeTimer; /* 0x005a7dca */
short nPendingCockpitDamageValue; /* 0x005a7dcc */
HudMessageSlot aCockpitHudMessageSlotsTable[2]; /* 0x005a7dd0 */
signed char cLastTargetObject; /* 0x005a7df2 */
int nSavedMouseCursorY; /* 0x005a7df4 */
int nStoredMouseCursorPosX; /* 0x005a7df8 */
char szComponentHitMessage[40]; /* 0x005a7e00 */
short nTargetLockMarkerY; /* 0x005a7e28 */
short nCarrierMissionShipIndexValue; /* 0x005a7e2a */
CockpitReadout aCockpitReadouts[6]; /* 0x005a7e30 */
short nScannerCursorX; /* 0x005a7e6c */
short nScannerCursorY; /* 0x005a7e6e */
unsigned char abCockpitIndicatorStatesArray[7]; /* 0x005a7e70 */
/* Function start: 0x413A10 */
void EmitTextString(void (__stdcall *writer)(int character), const char *text)
{
    short character;

    while ((character = (signed char)*text++) != 0)
        writer((short)character);
}

/* Function start: 0x413A40 */
void FormatTextTokens(void (__stdcall *writer)(int),
#ifdef SDL_PORT
                      const char *format, va_list *arguments)
#else
                      const char *format, va_list arguments)
#endif
{
    short character;
    char number[64];
    char *text;

    character = (signed char)*format++;
    while (character != 0) {
        switch (character) {
        default:
            writer((short)character);
            break;
        case '%':
            character = (signed char)*format++;
            switch (character) {
            case 'B':
#ifdef SDL_PORT
                pCurTextContext->backgroundColour =
                    (unsigned char)va_arg(*arguments, int);
#else
                pCurTextContext->backgroundColour =
                    (unsigned char)va_arg(arguments, int);
#endif
                break;
            case 'D':
#ifdef SDL_PORT
                text = ltoa(va_arg(*arguments, long), number, 10);
#else
                text = ltoa(va_arg(arguments, long), number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 'F':
#ifdef SDL_PORT
                pCurTextContext->colour =
                    (unsigned char)va_arg(*arguments, int);
#else
                pCurTextContext->colour =
                    (unsigned char)va_arg(arguments, int);
#endif
                break;
            case 'J':
#ifdef SDL_PORT
                pCurTextContext->alignment =
                    (unsigned char)va_arg(*arguments, int);
#else
                pCurTextContext->alignment =
                    (unsigned char)va_arg(arguments, int);
#endif
                break;
            case 'P':
                DrawTextString(pCurTextContext->text);
                break;
            case 'U':
#ifdef SDL_PORT
                text = ultoa(va_arg(*arguments, unsigned long), number, 10);
#else
                text = ultoa(va_arg(arguments, unsigned long), number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 'X':
#ifdef SDL_PORT
                pCurTextContext->cursorX =
                    (short)va_arg(*arguments, int);
#else
                pCurTextContext->cursorX =
                    va_arg(arguments, short);
#endif
                break;
            case 'Y':
#ifdef SDL_PORT
                pCurTextContext->cursorY =
                    (short)va_arg(*arguments, int);
#else
                pCurTextContext->cursorY =
                    va_arg(arguments, short);
#endif
                break;
            case 'c':
#ifdef SDL_PORT
                writer((short)va_arg(*arguments, int));
#else
                writer(va_arg(arguments, short));
#endif
                break;
            case 'd':
#ifdef SDL_PORT
                text = itoa((short)va_arg(*arguments, int), number, 10);
#else
                text = itoa((int)va_arg(arguments, short), number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 's':
#ifdef SDL_PORT
                text = va_arg(*arguments, char *);
#else
                text = va_arg(arguments, char *);
#endif
                EmitTextString(writer, text);
                break;
            case 'u':
#ifdef SDL_PORT
                text = ultoa((unsigned short)va_arg(*arguments, unsigned int),
                             number, 10);
#else
                text = ultoa(va_arg(arguments, unsigned short), number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 'x':
#ifdef SDL_PORT
                text = ultoa((unsigned short)va_arg(*arguments, unsigned int),
                             number, 16);
#else
                text = ultoa(va_arg(arguments, unsigned short), number, 16);
#endif
                EmitTextString(writer, strupr(text));
                break;
            default:
                writer((short)character);
                break;
            }
            break;
        }
        character = (signed char)*format++;
    }
}

/* Function start: 0x413C40 */
void DrawFormattedText(const char *format, ...)
{
#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    FormatTextTokens((void (__stdcall *)(int))DrawTextCharacter,
                     format, &arguments);
    va_end(arguments);
#else
    FormatTextTokens((void (__stdcall *)(int))DrawTextCharacter,
                     format, (va_list)(&format + 1));
#endif
    if (pCurTextContext->viewport->pixels ==
        stScreenView.pixels)
        DIBslam();
}

/* Function start: 0x413C70 */
void FormatTextBufferFromStart(const char *format, ...)
{
    pCurTextContext->textCursor =
        pCurTextContext->text;
#ifdef SDL_PORT
    {
        va_list arguments;

        va_start(arguments, format);
        FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                         format, &arguments);
        va_end(arguments);
    }
#else
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (va_list)(&format + 1));
#endif
    if (pCurTextContext->viewport->pixels ==
        stScreenView.pixels)
        DIBslam();
}

/* Function start: 0x413CB0 */
void AppendFormattedText(const char *format, ...)
{
#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, &arguments);
    va_end(arguments);
#else
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (va_list)(&format + 1));
#endif
    if (pCurTextContext->viewport->pixels ==
        stScreenView.pixels)
        DIBslam();
}

/* Function start: 0x413CE0 */
void FatalErrorAndExit(const char *format, ...)
{
    char text[0xfc];

#ifdef SDL_PORT
    va_list arguments;

    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
#else
    vsprintf(text, format, (char *)(&format + 1));
#endif
    ShutdownEventManager();
    exit_squadron(text);
}

/* Function start: 0x413D20 */
unsigned short IsCockpitExplosionActive(void)
{
    return nCockpitExplosionFrame < 8;
}

/* Function start: 0x413D40 */
void EraseCockpitReadoutRegion(Viewport *viewport, short left,
                               short top, short right, short bottom,
                               short colour)
{
    Viewport clippedViewport;

    clippedViewport = *viewport;
    if (right >= left && bottom >= top) {
        clippedViewport.left = left;
        clippedViewport.top = top;
        clippedViewport.right = right;
        clippedViewport.bottom = bottom;
        ClearViewport(&clippedViewport, colour);
    }
}

/* Function start: 0x413DA0 */
void vdu_polygon(signed char bar, short percent)
{
    signed char filledFrame;
    signed char emptyFrame;
    signed char swapFrame;
    short direction;
    short left;
    short top;
    short bottom;
    short right;
    short inputPercent;
    short extent;

    /* Preserve the input percentage before using its parameter for bar length. */
    inputPercent = percent;
    percent = aaCockpitBars[(int)cCurrentCockpitView][(int)bar].length;
    extent = (short)((inputPercent * percent) / 100);
    left = stCockpitBar.left =
        aaCockpitBars[(int)cCurrentCockpitView][(int)bar].left;
    if (left != -99) {
        right = stCockpitBar.right =
            aaCockpitBars[(int)cCurrentCockpitView][(int)bar].right;
        top = stCockpitBar.top =
            aaCockpitBars[(int)cCurrentCockpitView][(int)bar].top;
        bottom = stCockpitBar.bottom =
            aaCockpitBars[(int)cCurrentCockpitView][(int)bar].bottom;
        emptyFrame = (signed char)
            aaCockpitBars[(int)cCurrentCockpitView][(int)bar].emptyFrame;
        filledFrame = (signed char)
            aaCockpitBars[(int)cCurrentCockpitView][(int)bar].filledFrame;
        direction = aaCockpitBars[(int)cCurrentCockpitView][(int)bar].direction;

        if (direction < 2) {
            if (direction == 1) {
                extent = (short)(percent - extent);
                swapFrame = emptyFrame;
                emptyFrame = filledFrame;
                filledFrame = swapFrame;
            }
            stCockpitBar.bottom -= extent;
            if (stCockpitBar.top <= stCockpitBar.bottom)
                DrawSpriteDefault(&stCockpitBar, left, top,
                                  pCockpitDamageShape,
                                  filledFrame);
            stCockpitBar.top = (short)(stCockpitBar.bottom + 1);
            stCockpitBar.bottom = bottom;
            if (stCockpitBar.top <= bottom)
                DrawSpriteDefault(&stCockpitBar, left, top,
                                  pCockpitDamageShape,
                                  emptyFrame);
        } else {
            if (direction == 3) {
                extent = (short)(percent - extent);
                swapFrame = emptyFrame;
                emptyFrame = filledFrame;
                filledFrame = swapFrame;
            }
            stCockpitBar.right -= extent;
            if (stCockpitBar.left <= stCockpitBar.right)
                DrawSpriteDefault(&stCockpitBar, left, top,
                                  pCockpitDamageShape,
                                  filledFrame);
            stCockpitBar.left = (short)(stCockpitBar.right + 1);
            stCockpitBar.right = right;
            if (stCockpitBar.left <= right)
                DrawSpriteDefault(&stCockpitBar, left, top,
                                  pCockpitDamageShape,
                                  emptyFrame);
        }
    }
}

/* Function start: 0x413F70 */
unsigned int InitializeCockpitReadout(signed char slot,
                                      TextContext *context)
{
    aCockpitReadouts[(int)slot].context = context;
    aCockpitReadouts[(int)slot].x = context->cursorX;
    aCockpitReadouts[(int)slot].y = context->cursorY;
    aCockpitReadouts[(int)slot].previousRight = 0;
    return 0;
}

/* Function start: 0x413FB0 */
void DrawCockpitReadout(signed char slot, const char *text)
{
#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        SdlDrawSwcCockpitReadout(slot, text);
        return;
    }
#endif
    if (aCockpitReadouts[slot].x != -99) {
        SetTextContext(aCockpitReadouts[slot].context);
        SetTextCursor((unsigned short)aCockpitReadouts[slot].x,
                      (unsigned short)aCockpitReadouts[slot].y);
        DrawFormattedText(text);
        EraseCockpitReadoutRegion(
            &stScreenView, aCockpitReadouts[slot].context->cursorX,
            aCockpitReadouts[slot].y,
            aCockpitReadouts[slot].previousRight,
            (short)(*(short *)aCockpitReadouts[slot].context->font +
                    aCockpitReadouts[slot].y - 1),
            cBlackColour);
        aCockpitReadouts[slot].previousRight =
            aCockpitReadouts[slot].context->cursorX;
    }
}

/* Function start: 0x414050 */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top)
{
    EraseCockpitReadoutRegion(
        &stScreenView, left, top, aCockpitReadouts[slot].previousRight,
        (short)(*(short *)aCockpitReadouts[slot].context->font +
                aCockpitReadouts[slot].y),
        cBlackColour);
}

/* Function start: 0x4140A0 */
short DrawHudMessageSlot(HudMessageSlot *slot)
{
    unsigned char savedColour;
    short oldDrawColour;
    short showingEraseColour;

    if (nCameraViewMode != 0)
        return 1;

    oldDrawColour = slot->drawColour;
    if (((int)nTickCount60Hz / 40) % 3 == 0)
        slot->drawColour = cBlackColour;
    else
        slot->drawColour = slot->colour;

    if (slot->flashCount != -1) {
        if (slot->drawColour == cBlackColour &&
            oldDrawColour == slot->colour) {
            slot->flashCount = (signed char)MaxShort(
                0, (short)(slot->flashCount - 1));
        }
        if (slot->flashCount == 0)
            slot->drawColour = cBlackColour;
    }

    showingEraseColour = slot->drawColour == cBlackColour;
#ifdef SDL_PORT
    /* SWC presents these shared message slots through its SDL text layer. */
    if (SdlSwcFlightActive())
        return showingEraseColour;
#endif
    savedColour = slot->context->colour;
    slot->context->colour = (unsigned char)slot->drawColour;
    DrawTextAt(slot->context, slot->x, slot->y, slot->text, 2);
    slot->context->colour = savedColour;
    return showingEraseColour;
}

/* Function start: 0x414180 */
void ClearHudMessageSlot(HudMessageSlot *slot)
{
    slot->flashCount = 0;
    if (slot->text != 0)
        DrawHudMessageSlot(slot);
    slot->text = 0;
}

/* Function start: 0x4141B0 */
void ClearHudMessageIfMatching(HudMessageSlot *slot, const char *text)
{
    if (slot->text == text)
        ClearHudMessageSlot(slot);
}

/* Function start: 0x4141D0 */
void ClearHudGunReadouts(void)
{
    aCockpitHudMessageSlotsTable[0].text = 0;
    aCockpitHudMessageSlotsTable[0].flashCount = 0;
    aCockpitHudMessageSlotsTable[1].text = 0;
    aCockpitHudMessageSlotsTable[1].flashCount = 0;
}

/* Function start: 0x4141F0 */
void SetHudMessageSlot(HudMessageSlot *slot, TextContext *context,
                       short x, short y, const char *text,
                       unsigned short colour, signed char flashCount)
{
    if (slot->text != 0)
        ClearHudMessageSlot(slot);
    slot->context = context;
    slot->x = x;
    slot->y = y;
    slot->colour = colour;
    slot->drawColour = colour;
    slot->text = text;
    slot->flashCount = flashCount;
}

/* Function start: 0x414240 */
void UpdateMessage(HudMessageSlot *slot)
{
    short showingEraseColour;

    if (slot->text != 0) {
        showingEraseColour = DrawHudMessageSlot(slot);
        if (slot->flashCount == 0 && showingEraseColour != 0)
            ClearHudMessageSlot(slot);
    }
}

/* Function start: 0x414270 */
void set_global_message(const char *text, unsigned short colour,
                        int flashCount)
{
    SetHudMessageSlot(&aCockpitHudMessageSlotsTable[1], &stRightVduTxtContext,
                      stRightVduViewport.left,
                      (short)(stRightVduViewport.bottom - 6),
                      text, colour, (signed char)flashCount);
}

/* Function start: 0x4142B0 */
void CockpitMessage(const char *text, unsigned short colour,
                    int flashCount)
{
    if (text != aCockpitHudMessageSlotsTable[1].text)
        set_global_message(text, colour, flashCount);
}

/* Function start: 0x4142D0 */
/* Nothing in the shipped executable calls this. */
void ClearGlobalMessage(void)
{
    ClearHudMessageSlot(&aCockpitHudMessageSlotsTable[1]);
}

/* Function start: 0x4142E0 */
void remove_message(const char *text)
{
    ClearHudMessageIfMatching(&aCockpitHudMessageSlotsTable[1], text);
}

/* Function start: 0x414300 */
short kilrathi_near(short obj, short range)
{
    short ship;

    ship = 0;
    do {
        if (aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
            aeShipSide[ship] == SIDE_KILRATHI &&
            (short)IsPointWithinRange(&aShipPositionsArray[obj],
                                      &aShipPositionsArray[ship],
                                      range) != 0)
            return 1;
        ship++;
    } while (ship < 10);
    return 0;
}

/* Function start: 0x414380 */
short auto_pilot_valid(short showReason)
{
    const char *reason;

#ifdef SDL_PORT
    if (SdlSwcFlightActive())
        return SdlSwcAutopilotValid(showReason);
#endif
    reason = 0;
    if (cMissionObjectiveCountValue == 0)
        return 0;
    if (distance_from_point(
            0,
            &aMissionObjectivesArray[
                cCurrentObjective].position) < 8000) {
        reason = "Already Near";
    } else if (kilrathi_near(0, 16000) != 0) {
        reason = "Enemy Near";
    } else if (pActiveHazardField != 0) {
        reason = "Hazard Near";
    }
    if (showReason != 0 && reason != 0)
        set_global_message(reason, cYellowColour, 3);
    return reason == 0;
}

/* Function start: 0x414410 */
void *reset_cockpit(void)
{
    memset(abCockpitLightGoalArray, 0,
           sizeof(abCockpitLightGoalArray));
    memset(abCockpitIndicatorStatesArray, 0,
           sizeof(abCockpitIndicatorStatesArray));
    return abCockpitIndicatorStatesArray;
}

/* Function start: 0x414440 */
unsigned int SetCockpitLightBlink(signed char light, short interval)
{
    if (interval < 20) {
        if (interval == 0) {
            abCockpitLightGoalArray[(int)light] ^= 1;
            return 0;
        }
        if (nCurrentSpaceFrameValue % interval == 0) {
            abCockpitLightGoalArray[(int)light] ^= 1;
            return 0;
        }
    } else {
        abCockpitLightGoalArray[(int)light] = 0;
    }
    return 0;
}

/* Function start: 0x414490 */
void draw_cockpit_lights(void)
{
    signed char light;
    short x;
    short y;
    short frame;

    if (nRenderedSpaceFrame % 4 == 0) {
        if (auto_pilot_valid(0) != 0)
            abCockpitLightGoalArray[4] = 1;
        else
            abCockpitLightGoalArray[4] = 0;
    }
    for (light = 0; light < 7; light++) {
        if (bCockpitlessView == 0) {
            if (abCockpitIndicatorStatesArray[(int)light] !=
                abCockpitLightGoalArray[(int)light]) {
                x = aasCockpitLightX[
                    (int)cCurrentCockpitView][(int)light];
                y = aasCockpitLightY[
                    (int)cCurrentCockpitView][(int)light];
                if (abCockpitLightGoalArray[(int)light] == 1)
                    frame = aacCockpitLightOnFrame[
                        (int)cCurrentCockpitView][(int)light];
                else
                    frame = aacCockpitLightOffFrame[
                        (int)cCurrentCockpitView][(int)light];
                DrawSpriteDefault(&stScreenView, x, y,
                                  pCockpitDamageShape, frame);
                abCockpitIndicatorStatesArray[(int)light] =
                    abCockpitLightGoalArray[(int)light];
            }
            continue;
        }
        {
            short cockpitlessY;

            x = aasCockpitLightX[
                (int)cCurrentCockpitView][(int)light];
            cockpitlessY = aasCockpitLightY[
                (int)cCurrentCockpitView][(int)light];
            if (abCockpitLightGoalArray[(int)light] == 1)
                frame = aacCockpitLightOnFrame[
                    (int)cCurrentCockpitView][(int)light];
            else
                frame = aacCockpitLightOffFrame[
                    (int)cCurrentCockpitView][(int)light];
            DrawSpriteDefault(&stScreenView, x, cockpitlessY,
                              pCockpitDamageShape, frame);
            abCockpitIndicatorStatesArray[(int)light] =
                abCockpitLightGoalArray[(int)light];
        }
    }
}

/* Function start: 0x4145B0 */
void update_lights(void)
{
    short fuelPercent;

    fuelPercent = (short)(
        (anShipFuelsTable[0] * 100) /
        aObjectTypeData[
            aeObjectTypeList[0]].lifetime);
    SetCockpitLightBlink(6, fuelPercent);
    vdu_polygon(0, fuelPercent);
    vdu_polygon(1, asShipWpnEnergyList[0]);

    if (nTrainSimActive == 0) {
        if (calculate_damage_level() >= 3 &&
            (int)aasShipShield[0][1] +
                (int)aasShipShield[0][0] < 10) {
            SetCockpitLightBlink(3, 2);
            if (nCurrentDamageAlarmSfxHandle == 0 ||
                nCurrentSpaceFrameValue % 10 == 0)
                PlaySfxWaveFileByNumber(0x20, -1, 0);
        } else if (nCurrentDamageAlarmSfxHandle != 0) {
            FlushSoundEffectsAndLog(
                nCurrentDamageAlarmSfxHandle, 1);
            nCurrentDamageAlarmSfxHandle = 0;
            abCockpitLightGoalArray[3] = 0;
        }
    }
}

/* Function start: 0x414690 */
void update_bars(void)
{
    ObjectTypeData *typeData;
    short forePercent;
    short aftPercent;

    typeData = &aObjectTypeData[aeObjectTypeList[0]];
    vdu_polygon(2,
        (short)((aasShipArmor[0][0] * 100) /
                typeData->armorFront));
    vdu_polygon(3,
        (short)((aasShipArmor[0][1] * 100) /
                typeData->armorRear));
    vdu_polygon(4,
        (short)((aasShipArmor[0][2] * 100) /
                typeData->armorRight));
    vdu_polygon(5,
        (short)((aasShipArmor[0][3] * 100) /
                typeData->armorLeft));
    forePercent = (short)((aasShipShield[0][0] * 100) /
                          typeData->shieldFore);
    SetCockpitLightBlink(0, forePercent);
    vdu_polygon(6, forePercent);
    DrawCockpitReadout(
        4, itoa((int)aasShipShield[0][0],
                szTxtWorkspaceBuffer, 10));
    aftPercent = (short)((aasShipShield[0][1] * 100) /
                         typeData->shieldAft);
    SetCockpitLightBlink(1, aftPercent);
    vdu_polygon(7, aftPercent);
    DrawCockpitReadout(
        5, itoa((int)aasShipShield[0][1],
                szTxtWorkspaceBuffer, 10));
}

/* Function start: 0x4147E0 */
short get_mode(short i)
{
    return ausVduModeStack[
        ((int)acVduModeStackDepth[i] + i * 4) * 2];
}

/* Function start: 0x414800 */
void set_mode(short i, int state)
{
    if (get_mode(i) != state)
        ClearHudMessageSlot(&aCockpitHudMessageSlotsTable[i]);
    acVduModeStackDepth[i] = 0;
    *(int *)&ausVduModeStack[i * 8] = state;
}

/* Function start: 0x414850 */
unsigned short SetVduModeIfChanged(short i, int state)
{
    short changed;

    changed = get_mode(i) != state;
    if (changed != 0)
        set_mode(i, state);
    return changed;
}

/* Function start: 0x414890 */
short GetVduModeStackDepth(short i)
{
    return acVduModeStackDepth[i];
}

/* Function start: 0x4148A0 */
void push_mode(short i, int state)
{
    ClearHudMessageSlot(&aCockpitHudMessageSlotsTable[i]);
    acVduModeStackDepth[i]++;
    *(int *)&ausVduModeStack[
        ((int)acVduModeStackDepth[i] + i * 4) * 2] = state;
}

/* Function start: 0x4148E0 */
void pop_mode(short i)
{
    ClearHudMessageSlot(&aCockpitHudMessageSlotsTable[i]);
    acVduModeStackDepth[i]--;
}

/* Function start: 0x414910 */
void set_new_vdu(short vdu)
{
    if (get_mode(vdu) == 0) {
        malf_noise(vdu, 1, cDarkGreenColour, 0x17, 0);
    } else {
        if (vdu == 1)
            ClearViewport(&stRightVduViewport, cBlackColour);
        else
            ClearViewport(&stLeftVdu, cBlackColour);
    }
    anVduModeCache[vdu] = (unsigned int)get_mode(vdu);
}

/* Function start: 0x414980 */
short update_vid_disp(short vdu)
{
    short changed;

    changed = get_mode(vdu) != (int)anVduModeCache[vdu];
    if (changed != 0)
        set_new_vdu(vdu);
    return changed;
}

/* Function start: 0x4149C0 */
void InvalidateVduMode(short i)
{
    anVduModeCache[i] = 0;
}

/* Function start: 0x4149E0 */
void clear_message_time(void)
{
    nCurrentNoticeTimer = 0;
}

/* Function start: 0x4149F0 */
short message_showing(void)
{
#ifdef SDL_PORT
    if (SdlSwcFlightActive())
        return SdlSwcMessageShowing();
#endif
    return 0 < nCurrentNoticeTimer;
}

/* Function start: 0x414A10 */
unsigned short set_message_time(unsigned short v)
{
    nCurrentNoticeTimer = (short)v;
    return 0;
}

/* Function start: 0x414A20 */
void check_message(void)
{
    if (message_showing() &&
        (nCurrentNoticeTimer = nCurrentNoticeTimer - 1,
         nCurrentNoticeTimer <= 0))
        EndCommMenu();
}

/* Function start: 0x414A50 */
unsigned int update_digital_readouts(void)
{
    long velocity;
    short speed;

    SetTextContext(&stCurrentCockpitReadoutTextCtx);
    speed = (short)((anShipSpeed[0] >> 8) * 10);
    DrawCockpitReadout(
        2, itoa((int)speed,
                szTxtWorkspaceBuffer, 10));
    velocity = MultiplyFixed(
        Vector_magnitude(&aShipVelocity[0]), 0xa00);
    DrawCockpitReadout(
        3, itoa((int)(short)(velocity >> 8),
                szTxtWorkspaceBuffer, 10));
    return 0;
}

/* Function start: 0x414AD0 */
void PlayTargetLockSfx(void)
{
    PlaySfxWaveFileByNumber(0x18, -1, 0);
}

/* Function start: 0x414AE0 */
void malf_sound(void)
{
    PlaySfxWaveFileByNumber(0x1f, -1, 0);
}

/* Function start: 0x414AF0 */
short malf(char component)
{
    int damage = acPlayerComponentDamageList[(int)component];

#ifdef SDL_PORT
    if (SdlSwcFlightActive())
        return SdlSwcComponentMalfunction(component);
#endif
    return (unsigned short)RandomInRange(0, 15) < damage * damage;
}

/* Function start: 0x414B20 */
unsigned short vdu_malf(short vdu, short sound)
{
    if (nCameraViewMode == 0
#ifdef SDL_PORT
        && !SdlSwcFlightActive()
#endif
    )
        malf_noise(vdu, 1, cDarkGreenColour, sound, 0);
    set_mode(vdu, 0);
    return 0;
}

/* Function start: 0x414B70 */
void ShowComponentHitHudMessage(const char *text, unsigned short colour,
                                short flashCount)
{
    if (nTrainSimActive == 0 && get_mode(0) != 0) {
        if (aCockpitHudMessageSlotsTable[0].text != 0)
            ClearHudMessageSlot(&aCockpitHudMessageSlotsTable[0]);
        DosStrcpy(szComponentHitMessage, text);
        SetHudMessageSlot(&aCockpitHudMessageSlotsTable[0],
                          &stLeftVduTextContext,
                          stLeftVdu.left,
                          (short)(stLeftVdu.bottom - 6),
                          szComponentHitMessage,
                          colour, (signed char)flashCount);
    }
}

/* Function start: 0x414BF0 */
int damage_your_component(char component, char amount, char maximum)
{
    int index = (int)component;
    char text[40];

    acPlayerComponentDamageList[index] = (signed char)MinShort(
        (short)(acPlayerComponentDamageList[index] + amount),
        (short)maximum);
    if (malf(component) != 0 && index == 3) {
        vdu_malf(0, 0x18);
        vdu_malf(1, 0x18);
    }
    if (get_mode(0) == 2 || get_mode(0) == 1) {
        sprintf(text, "%s HIT",
                apszComponentNames[index]);
        ShowComponentHitHudMessage(text, (unsigned char)cRedColour, 5);
    }
    return acPlayerComponentDamageList[index];
}

/* Function start: 0x414CB0 */
void RemovePlayerReleaseWeapon(signed char weapon)
{
    enum ObjectType preferredType;
    int hardpoint;

    preferredType = ((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[weapon].type;
#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        remove_weapon(0, weapon);
        nSelectedReleaseWeaponIndex = -1;
        select_new_release_weapon(preferredType);
        return;
    }
#endif
    eReleaseWpnDisplayType = preferredType;
    cReleaseWeaponDisplayFrame =
        (signed char)(preferredType * 2 - 0x2f);
    hardpoint = ((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[weapon].hardpoint;
    stReleaseWeaponDisplayPosition.x =
        (short)(aWeaponDisplayPositions[hardpoint].x +
                stWpnDispOrigin.x);
    stReleaseWeaponDisplayPosition.y =
        (short)(aWeaponDisplayPositions[hardpoint].y +
                stWpnDispOrigin.y);
    cReleaseWeaponDisplayTicks = 3;
    cReleaseWeaponDisplayState = 0;
    if (pReleaseWeaponDisplayBackground == 0)
        cReleaseWeaponDisplayFrame = -1;
    remove_weapon(0, weapon);
    nSelectedReleaseWeaponIndex = -1;
    select_new_release_weapon(preferredType);
}

/* Function start: 0x414D50 */
void fire_computer_graphic_missile(void)
{
    short visible;
    unsigned char *background;

    if (cReleaseWeaponDisplayFrame == -1)
        return;
    visible = nCameraViewMode == 0 && get_mode(0) == 1;
    if (cReleaseWeaponDisplayState != 0) {
        RestoreSpriteBackground(&stLeftVdu, pReleaseWeaponDisplayBackground,
                                stReleaseWeaponDisplayPosition.x,
                                stReleaseWeaponDisplayPosition.y,
                                pCockpitWeaponShapePtr,
                                cReleaseWeaponDisplayFrame);
        background = pReleaseWeaponDisplayBackground;
    } else {
        background = pReleaseWeaponDisplayBackground;
    }
    if (stReleaseWeaponDisplayPosition.y > stLeftVdu.top - 10 &&
        stReleaseWeaponDisplayPosition.y < stLeftVdu.bottom) {
        if (eReleaseWpnDisplayType == OBJECT_TYPE_SPACE_MINE)
            stReleaseWeaponDisplayPosition.y += cReleaseWeaponDisplayTicks;
        else
            stReleaseWeaponDisplayPosition.y -= cReleaseWeaponDisplayTicks;
        cReleaseWeaponDisplayTicks++;
        if (visible != 0) {
            CaptureSpriteBackground(&stLeftVdu,
                                    background,
                                    stReleaseWeaponDisplayPosition.x,
                                    stReleaseWeaponDisplayPosition.y,
                                    pCockpitWeaponShapePtr,
                                    cReleaseWeaponDisplayFrame);
            DrawSpriteDefault(&stLeftVdu, stReleaseWeaponDisplayPosition.x,
                              stReleaseWeaponDisplayPosition.y, pCockpitWeaponShapePtr,
                              cReleaseWeaponDisplayFrame);
        }
        cReleaseWeaponDisplayState = (signed char)visible;
    } else {
        cReleaseWeaponDisplayFrame = -1;
    }
}

/* Function start: 0x414EA0 */
void show_weapon_disp(void)
{
    signed char count;
    ShipWeaponSlot *weapons;
    ShipWeaponSlot *selectedWeapon;
    const char *releaseName;
    const char *gunName;
    enum ObjectType selectedGunType;
    short frame;

    selectedWeapon =
        &((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[
            nSelectedReleaseWeaponIndex];
    set_new_vdu(0);
    DrawTextAt(&stLeftVduTextContext,
               stLeftVdu.left, stLeftVdu.top,
               "WEAPON DISPLAY", 2);
    DrawViewportLine(&stLeftVdu, (short)(stLeftVdu.left + 2),
                     (short)(stLeftVdu.top + 5),
                     (short)(stLeftVdu.right - 2),
                     (short)(stLeftVdu.top + 5),
                     cPrimaryTextColour);

    releaseName = szNoWeaponName;
    if (nSelectedReleaseWeaponIndex != -1)
        releaseName = aObjectTypeData[
            selectedWeapon->type].displayName;
    selectedGunType = eSelectedGunType;
    switch ((int)selectedGunType) {
    case -1:
        gunName = szNoWeaponName;
        break;
    case 0x80:
        gunName = "Full Guns";
        break;
    default:
        gunName = aObjectTypeData[
            selectedGunType].displayName;
        break;
    }
    DrawFormattedText("\nWeapon: %s", releaseName);
    DrawFormattedText("\nGun: %s", gunName);

    stWpnDispOrigin.x =
        (short)(stLeftVdu.left +
                aWeaponDisplayOrigins[
                    (int)cCurrentCockpitView].x);
    stWpnDispOrigin.y =
        (short)(stLeftVdu.top +
                aWeaponDisplayOrigins[
                    (int)cCurrentCockpitView].y);
    DrawSpriteDefault(&stLeftVdu, stWpnDispOrigin.x,
                      stWpnDispOrigin.y,
                      pCockpitWeaponShapePtr, 0);
    weapons = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    count = 0;
    if ((signed char)aShipWeaponsTable[0][0] > 0)
        do {
            count++;
            frame = (short)((int)weapons->type * 2 +
                            weapons->disabled - 0x2f);
            DrawSpriteDefault(
                &stLeftVdu,
                (short)(stWpnDispOrigin.x +
                        aWeaponDisplayPositions[
                            weapons->hardpoint].x),
                (short)(stWpnDispOrigin.y +
                        aWeaponDisplayPositions[
                            weapons->hardpoint].y),
                pCockpitWeaponShapePtr, frame);
            weapons++;
        } while (count < (signed char)aShipWeaponsTable[0][0]);
}

/* Function start: 0x415040 */
void update_status_text(void)
{
}

/* Function start: 0x415050 */
short sighted(short objective)
{
    return (aMissionObjectivesArray[objective].flags & 4) == 4;
}

/* Function start: 0x415070 */
short visited(short objective)
{
    return (aMissionObjectivesArray[objective].flags & 1) == 1;
}

/* Function start: 0x415090 */
short achieved(short objective)
{
    return (aMissionObjectivesArray[objective].flags & 2) == 2;
}

/* Function start: 0x4150B0 */
void flag_objective(short objective, unsigned char flags)
{
    aMissionObjectivesArray[objective].flags |= flags;
}

/* Function start: 0x4150D0 */
void DrawCalculatingLabel(void)
{
    if (nCurObjectiveRangeValue <= 0) {
        DrawCockpitReadout(0, "CALCULATING");
    } else {
        strcpy(szTxtWorkspaceBuffer +
                   strlen(ltoa((long)nCurObjectiveRangeValue,
                               szTxtWorkspaceBuffer, 10)),
               " km");
        DrawCockpitReadout(0, szTxtWorkspaceBuffer);
    }
    nDisplayedObjectiveRange =
        nCurObjectiveRangeValue;
}

/* Function start: 0x415130 */
const char *objective_name(short objective)
{
    if (objective >= (short)cMissionObjectiveCountValue)
        return "NONE";
    if (*aMissionObjectivesArray[objective].name == '?' &&
        sighted(objective) == 0)
        return "UNKNOWN";
    return aMissionObjectivesArray[objective].displayName;
}

/* Function start: 0x415180 */
void show_navigation_disp(void)
{
    DrawTextAt(&stRightVduTxtContext,
               stRightVduViewport.left, stRightVduViewport.top,
               "COMP NAVIGATION", 2);
    DrawFormattedText("\n\nDESTINATION\n  %s",
                      objective_name((short)cCurrentObjective));
    DrawFormattedText("\n\nRANGE\n  ");
    InitializeCockpitReadout(0, &stRightVduTxtContext);
    DrawFormattedText("\n\n(N)ew Objective");
    DrawCalculatingLabel();
}

/* Function start: 0x4151F0 */
short hidden_objective(short objective)
{
    short nameHidden;
    short hidden;
    short ship;

    nameHidden =
        *aMissionObjectivesArray[objective].displayName == '.' ||
        *aMissionObjectivesArray[objective].name == '.';
    if (nameHidden != 0 ||
        (mobile_objective(objective) != 0 &&
         aMissionShips[
             (signed char)aMissionObjectivesArray[
                 objective].index].state != 0))
        hidden = 1;
    else
        hidden = 0;
    if (hidden == 0 && mobile_objective(objective) != 0 &&
        aMissionObjectivesArray[
            cMissionObjectiveCountValue].type == 0) {
        ship = find_ship_index(
            (short)aMissionObjectivesArray[objective].index);
        if (aMissionShips[
                (signed char)aMissionObjectivesArray[
                    objective].index].missionType ==
                MISSION_TYPE_WARP_ARRIVE &&
            ship != -1)
            hidden = 1;
    }
    return hidden;
}

/* Function start: 0x4152C0 */
int set_new_objective(short pathIndex)
{
    if (pathIndex < 0) {
        pathIndex = -1;
        do {
            pathIndex++;
        } while (abFlightPathList[pathIndex] != -1 &&
                 pathIndex < (short)cMissionObjectiveCountValue);
        pathIndex--;
    }
    if (pathIndex > (short)cMissionObjectiveCountValue)
        pathIndex = 0;
    if (abFlightPathList[pathIndex] == -1)
        pathIndex = 0;
    cCurrentNavPtIdx = (signed char)pathIndex;
    if (hidden_objective(
            (short)abFlightPathList[pathIndex]) != 0)
        return 0;
    cCurrentObjective =
        abFlightPathList[cCurrentNavPtIdx];
    aeShipObjectiveList[0] =
        (enum ShipObjective)aMissionObjectivesArray[
            (short)cCurrentObjective].type;
    set_objective_range(0);
    return 1;
}

/* Function start: 0x415370 */
short cycle_next_objective(void)
{
    short wraps;

    wraps = 0;
    do {
        if (set_new_objective(
                (short)((short)cCurrentNavPtIdx + 1)) != 0)
            break;
        if (cCurrentNavPtIdx == 0)
            wraps++;
    } while (wraps < 3);
    if (wraps >= 3) {
        cCurrentNavPtIdx = 0;
        cCurrentObjective = abFlightPathList[0];
    }
    return wraps < 3;
}

/* Function start: 0x4153D0 */
void set_next_destination(void)
{
    set_new_objective(0);
    do {
        if (set_new_objective(
                (short)cCurrentNavPtIdx) != 0 &&
            visited((short)abFlightPathList[
                cCurrentNavPtIdx]) == 0)
            break;
        cCurrentNavPtIdx++;
    } while (cCurrentNavPtIdx <
                 cMissionObjectiveCountValue &&
             abFlightPathList[
                 cCurrentNavPtIdx] != -1);
    if (cCurrentNavPtIdx >=
            cMissionObjectiveCountValue ||
        abFlightPathList[
            cCurrentNavPtIdx] == -1) {
        set_new_objective(0);
        cycle_next_objective();
    }
    InvalidateVduMode(1);
}

/* Function start: 0x415470 */
short LocateMobileObjective(short objective)
{
    short ship = -1;

    if (mobile_objective(objective) != 0) {
        ship = locate_ship(
            aMissionObjectivesArray[objective].index,
            &aMissionObjectivesArray[objective].position);
    }
    return ship;
}

/* Function start: 0x4154C0 */
unsigned int someone_coming(void)
{
    short ship = 0;
    signed char navPoint;

    navPoint = cCurrentNavPtIdx;
    do {
        if (aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
            aeShipMissionType[ship] == MISSION_TYPE_COME_HOME &&
            abShipNavPtIdx[ship] <= navPoint)
            return 1;
        ship = ship + 1;
    } while (ship < 10);
    return 0;
}

/* Function start: 0x415510 */
short escorting_a_ship(void)
{
    enum ShipMissionType missionType = aeShipMissionType[0];
    unsigned int (*checkArrivingShips)(void) = someone_coming;

    if (missionType != MISSION_TYPE_ESCORT) {
        unsigned int arriving = checkArrivingShips();

        if (arriving == 0)
            return 0;
    }
    return 1;
}

/* Function start: 0x415530 */
void flag_reached(short objective, short reached)
{
    short carrierMissionShip;
    short carrierObject;
    short objectiveType;
    short advanceDestination;
    short markVisited;

#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        SdlSwcFlagReached(objective, reached);
        return;
    }
#endif
    objectiveType = (short)aMissionObjectivesArray[objective].type;
    carrierMissionShip = anShipMissionShips[0];
    carrierObject = find_ship_index(anShipMissionShips[0]);
    markVisited = objective != cCurrentObjective;
    advanceDestination = 0;
    if (objective == cCurrentObjective) {
        if (reached != 0 || escorting_a_ship() == 0 ||
            carrierObject == -1 ||
            aMissionObjectivesArray[objective].index ==
                anShipMissionShips[0]) {
            if (visited(objective) != 0)
                CockpitMessage("Already Visited", cYellowColour, 4);
            else
                CockpitMessage("Objective Reached", cYellowColour, 4);
            advanceDestination = markVisited = 1;
        } else {
            if (objectiveType != 1 ||
                aMissionShips[carrierMissionShip].state != 1) {
                sprintf(pszObjectiveStatusMessage,
                        "Wait for %s",
                        aObjectTypeData[
                            aeObjectTypeList[carrierObject]].
                                displayName);
                CockpitMessage(pszObjectiveStatusMessage,
                               cYellowColour, 4);
            }
        }
    }
    if (objectiveType != 1 && markVisited != 0) {
        if (visited(objective) == 0 && carrierObject != -1 &&
            aMissionObjectivesArray[objective].index ==
                anShipMissionShips[0] &&
            aeObjectTypeList[carrierObject] !=
                OBJECT_TYPE_TIGERS_CLAW)
            send_message(carrierObject, 6);
        flag_objective(objective, 1);
    }
    if (advanceDestination != 0)
        set_next_destination();
}

/* Objective tracking, scanner, and target-lock callees from other modules. */
void ComputeVectorDelta(FixedVector *, FixedVector *, FixedVector *);
short FixedToShortSaturating(int value);
void transform_to_objects_frame(const FixedVector *source,
                                FixedVector *destination,
                                short obj);
void rectangular_to_spherical(const FixedVector *rectangular,
                              SphericalVector *spherical);
void DrawViewportPixel(Viewport *viewport, short x, short y,
                       short colour);
int GetViewportPixel(Viewport *viewport, short x, short y);
short RandomBelowOrEqual(short n);

/* Function start: 0x4156D0 */
void check_sighting(short objective, short range, short object)
{
    if (sighted(objective) == 0 && range < 16000 &&
        (object == -1 ||
         asObjectScreenXList[object] != (short)0x8001))
        flag_objective(objective, 4);
}

/* Function start: 0x415720 */
void check_visit(short objective, short range)
{
    int reachedRange;

    if ((short)aMissionObjectivesArray[objective].type == 3 ||
        (short)aMissionObjectivesArray[objective].type == 4)
        reachedRange = 6000;
    else
        reachedRange = 1500;
    if (range < reachedRange)
        flag_reached(objective, 0);
}

/* Function start: 0x415770 */
void update_objective_location(short objective)
{
    FixedVector delta;
    short range;
    short object;

    object = LocateMobileObjective(objective);
    if (sighted(objective) != 0 && visited(objective) != 0 &&
        cCurrentObjective != objective)
        return;
    ComputeVectorDelta(&aShipPositionsArray[0],
                       &aMissionObjectivesArray[objective].position,
                       &delta);
    range = FixedToShortSaturating(
        Vector_magnitude(&delta));
    check_sighting(objective, range, object);
    if (mobile_objective(objective) != 0) {
        if (object != -1)
            check_visit(objective, range);
    } else if (aMissionNavPoints[
                   aMissionObjectivesArray[objective].index].type >= 1) {
        check_visit(objective, range);
    }
}

/* Function start: 0x415850 */
unsigned int objective_lost(short objective)
{
    unsigned short state;

    state = aMissionShips[
        aMissionObjectivesArray[objective].index].state;
    switch (aMissionObjectivesArray[objective].type) {
        case 2:
        case 3:
            return state >= 1;
        case 4:
            return state == 3;
        default:
            return 0;
    }
}

/* Function start: 0x4158A0 */
void check_objectives(void)
{
    if (objective_lost((short)cCurrentObjective) != 0) {
        cycle_next_objective();
        InvalidateVduMode(1);
    } else {
        update_objective_location((short)cCurrentObjective);
    }
    if (nDisplayedObjectiveRange !=
        nCurObjectiveRangeValue)
        DrawCalculatingLabel();
}

/* Function start: 0x4158F0 */
void rotational_pos_to_scanner_pos(signed char object,
                                   const SphericalVector *position)
{
    short horizontal;
#ifdef SDL_PORT
    const CockpitScannerGeometry *scanner;

    scanner = SdlSwcFlightActive() ? &stSwcCockpitScanner :
        &stCockpitLayout.scanner[(int)cCurrentCockpitView];
#endif

    horizontal = position->yaw;
    if (abs((int)horizontal) < 45)
        nScannerCursorX = (short)(
#ifdef SDL_PORT
            scanner->centerX + horizontal / 4);
#else
            stCockpitLayout.scanner[
                (int)cCurrentCockpitView].centerX + horizontal / 4);
#endif
    else
        nScannerCursorX = (short)(
#ifdef SDL_PORT
            scanner->centerX + horizontal / 6);
#else
            stCockpitLayout.scanner[
                (int)cCurrentCockpitView].centerX + horizontal / 6);
#endif

    nScannerCursorY = (short)(
#ifdef SDL_PORT
        scanner->centerY + position->pitch / -3);
    /* CODE_13 +0x25cc adds one to X before applying the Mac limits. */
    if (SdlSwcFlightActive())
        nScannerCursorX++;
    nScannerCursorX = MinShort(scanner->maximumX, nScannerCursorX);
    nScannerCursorX = MaxShort(scanner->minimumX, nScannerCursorX);
    nScannerCursorY = MinShort(scanner->maximumY, nScannerCursorY);
    nScannerCursorY = MaxShort(scanner->minimumY, nScannerCursorY);
#else
        stCockpitLayout.scanner[
            (int)cCurrentCockpitView].centerY + position->pitch / -3);
    nScannerCursorX =
        MinShort(stCockpitLayout.scanner[
                     (int)cCurrentCockpitView].maximumX,
                 nScannerCursorX);
    nScannerCursorX =
        MaxShort(stCockpitLayout.scanner[
                     (int)cCurrentCockpitView].minimumX,
                 nScannerCursorX);
    nScannerCursorY =
        MinShort(stCockpitLayout.scanner[
                     (int)cCurrentCockpitView].maximumY,
                 nScannerCursorY);
    nScannerCursorY =
        MaxShort(stCockpitLayout.scanner[
                     (int)cCurrentCockpitView].minimumY,
                 nScannerCursorY);
#endif
    asScannerObjectPositionX[(int)object] =
        nScannerCursorX;
    asScannerObjectY[(int)object] =
        nScannerCursorY;
}

/* Function start: 0x415A30 */
short mobile_objective(short objective)
{
    short type = (short)aMissionObjectivesArray[objective].type;

    if (type == 1 || type == 3 || type == 4 || type == 2)
        return 1;
    return 0;
}

/* Function start: 0x415A70 */
void ResetScannerContacts(void)
{
    short i = 10;

    do {
        asScannerObjectPositionX[i] = 0;
        i = i - 1;
    } while (i != 0);
    nScannerTargetObject = -1;
}

/* Function start: 0x415A90 */
void clear_head_up_display(void)
{
    short object;

    if (nScannerTargetObject != -1) {
        DrawViewportPixel(&stScreenView,
                          asScannerObjectPositionX[nScannerTargetObject],
                          asScannerObjectY[nScannerTargetObject],
                          asScannerBackgroundColour[
                              nScannerTargetObject]);
        asScannerObjectPositionX[nScannerTargetObject] = 0;
    }
    if (asScannerObjectPositionX[10] != 0) {
        RestoreSpriteBackground(&stScreenView,
                                pRadarMarkerBackground,
                                asScannerObjectPositionX[10],
                                asScannerObjectY[10],
                                pTgtLockShapePtr, 2);
        asScannerObjectPositionX[10] = 0;
    }
    object = 9;
    do {
        if (asScannerObjectPositionX[object] != 0) {
            DrawViewportPixel(&stScreenView,
                              asScannerObjectPositionX[object],
                              asScannerObjectY[object],
                              asScannerBackgroundColour[object]);
            asScannerObjectPositionX[object] = 0;
        }
        object--;
    } while (object != 0);
    nScannerTargetObject = -1;
}

/* Function start: 0x415B70 */
unsigned int set_objective_range(short showOnScanner)
{
    FixedVector relative;
    FixedVector rotated;
    SphericalVector spherical;

    LocateMobileObjective((short)cCurrentObjective);
    ComputeVectorDelta(&aShipPositionsArray[0],
                       &aMissionObjectivesArray[
                           (short)cCurrentObjective].position,
                       &relative);
    transform_to_objects_frame(&relative, &rotated, 0);
    rectangular_to_spherical(&rotated, &spherical);
    if (showOnScanner != 0)
        rotational_pos_to_scanner_pos(10, &spherical);
    nCurObjectiveRangeValue = spherical.radius >> 8;
    return 0;
}

/* Function start: 0x415C00 */
short get_color(short object, unsigned short *colour)
{
    enum ObjectClass objectClass;

    objectClass = aeObjectClass[object];
    if (objectClass < OBJECT_CLASS_MISSILE)
        return 0;
    if (objectClass == OBJECT_CLASS_SHIP) {
        if (aeShipSide[object] == SIDE_KILRATHI)
            *colour = cRedColour;
        else if (aeShipSide[object] == SIDE_IMPERIAL)
            *colour = cBlueColour;
        else
            *colour = cPrimaryTextColour;
        return 1;
    }
    if (objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        if (aeShipSide[object] == SIDE_KILRATHI)
            *colour = cOrangeColour;
        else if (aeObjectTypeList[object] ==
                 OBJECT_TYPE_TIGERS_CLAW)
            *colour = cViewportClearColour;
        else
            *colour = cDarkGreyColour;
        return 1;
    }
    if (acShipTargetTable[object] == 0) {
        *colour = cYellowColour;
        return 1;
    }
    return 0;
}

/* Function start: 0x415CE0 */
unsigned int draw_3d_scanner(void)
{
    int row;
    SphericalVector spherical;
    unsigned short colour;
    short object;
    const int *grid;
    int gridValue;

    if (bCockpitlessView != 0) {
        row = 0;
        if (aiScannerGridRows[0] != -2) {
            grid = aiScannerGridRows;
            do {
                gridValue = *grid;
                if (gridValue == -1) {
                    row++;
                } else {
                    DrawViewportPixel(&stScreenView,
                        (short)(stCockpitLayout.scanner[
                            (int)cCurrentCockpitView].centerX + row),
                        (short)(stCockpitLayout.scanner[
                            (int)cCurrentCockpitView].centerY + gridValue), 0xaa);
                    if (gridValue != 0)
                        DrawViewportPixel(&stScreenView,
                            (short)(stCockpitLayout.scanner[
                                (int)cCurrentCockpitView].centerX + row),
                            (short)(stCockpitLayout.scanner[
                                (int)cCurrentCockpitView].centerY - gridValue), 0xaa);
                    if (row != 0) {
                        DrawViewportPixel(&stScreenView,
                            (short)(stCockpitLayout.scanner[
                                (int)cCurrentCockpitView].centerX - row),
                            (short)(stCockpitLayout.scanner[
                                (int)cCurrentCockpitView].centerY + gridValue), 0xaa);
                        if (gridValue != 0)
                            DrawViewportPixel(&stScreenView,
                                (short)(stCockpitLayout.scanner[
                                    (int)cCurrentCockpitView].centerX - row),
                                (short)(stCockpitLayout.scanner[
                                    (int)cCurrentCockpitView].centerY - gridValue), 0xaa);
                    }
                }
                grid++;
            } while (*grid != -2);
        }
    }

    clear_head_up_display();
    nScannerTargetObject = acShipTargetTable[0];
    if (nScannerTargetObject != -1 &&
        aeObjectClass[nScannerTargetObject] <
            OBJECT_CLASS_SHIP)
        nScannerTargetObject = -1;

    for (object = 1; object <= 9; object++) {
        if (get_color(object, &colour) != 0) {
            rectangular_to_spherical(&aObjectViewPositionsList[object],
                                     &spherical);
            if (spherical.radius < 0xea6000) {
                rotational_pos_to_scanner_pos((signed char)object,
                                               &spherical);
                asScannerBackgroundColour[object] =
                    (short)GetViewportPixel(
                        &stScreenView, nScannerCursorX,
                        nScannerCursorY);
                if (nScannerTargetObject != object)
                    DrawViewportPixel(&stScreenView,
                                      nScannerCursorX,
                                      nScannerCursorY,
                                      colour);
            }
        }
    }

    if (get_mode(1) == 5) {
        set_objective_range(1);
        CaptureSpriteBackground(&stScreenView,
                                pRadarMarkerBackground,
                                nScannerCursorX,
                                nScannerCursorY,
                                pTgtLockShapePtr, 2);
        DrawSpriteDefault(&stScreenView,
                          nScannerCursorX,
                          nScannerCursorY,
                          pTgtLockShapePtr, 2);
    }

    object = nScannerTargetObject;
    if (object != -1 && get_color(object, &colour) != 0) {
        DrawViewportPixel(&stScreenView,
            asScannerObjectPositionX[nScannerTargetObject],
            asScannerObjectY[nScannerTargetObject],
            (short)(nCurrentSpaceFrameValue % 2) != 0 ? (short)colour : cBlackColour);
    }
    return 0;
}

/* Function start: 0x415FC0 */
void start_lock(unsigned short v)
{
    bTargetLockReadoutDirty = 0;
    nTargetLockCountdown = v;
    nTargetLockMarkerAngle =
        RandomBelowOrEqual(0x167);
}

/* Function start: 0x415FF0 */
unsigned short starting_lock(unsigned short v)
{
    if (nTargetLockCountdown == -1) {
        start_lock(v);
        return 1;
    }
    return 0;
}

/* Function start: 0x416010 */
void lock_off(void)
{
    if (nTargetLockCountdown > -1)
        bTargetLockReadoutDirty = 1;
    remove_message(pszMissileLockedMessage);
    nTargetLockCountdown = -1;
}

/* Function start: 0x416040 */
short CheckTargetLockMalfunction(void)
{
    short countdown;

    if (malf(5) != 0) {
        countdown = -10;
        lock_off();
        countdown = (short)(countdown - RandomBelowOrEqual(30));
        nTargetLockCountdown = countdown;
        PlaySfxWaveFileByNumber(7, -1, 0);
        return 1;
    }
    return 0;
}

/* Function start: 0x416090 */
short decrement_lock_time(short screenX)
{
    (void)screenX;
    if (nTargetLockCountdown > 0) {
        if (malf(5) == 0) {
            nTargetLockCountdown--;
            bTargetLockAcquired =
                nTargetLockCountdown == 0;
            if (bTargetLockAcquired != 0) {
                if (CheckTargetLockMalfunction() == 0)
                    PlaySfxWaveFileByNumber(0x16, -1, 0);
                CockpitMessage(pszMissileLockedMessage,
                               cRedColour, 2);
                return 1;
            }
            PlaySfxWaveFileByNumber(0x15, -1, 0);
        }
        return 1;
    }
    return 0;
}

/* Function start: 0x416120 */
void target_locking(signed char target)
{
    enum ObjectType weaponType;
    short x;
    short y;
    short countdown;
    int radiusSquared;

    if (target != -1 &&
        aeShipSide[(short)target] != aeShipSide[0] &&
        acPlayerComponentDamageList[5] < 4) {
        x = asObjectScreenXList[(short)target];
        if (x == -0x7fff)
            return;
        y = asObjectScreenYArray[(short)target];
        countdown = nTargetLockCountdown;
        if (countdown < -1) {
            nTargetLockCountdown = ++countdown;
            return;
        }
        radiusSquared = y * y;
        radiusSquared += x * x;
        if (radiusSquared > 0xe10) {
            lock_off();
            return;
        }
#ifdef SDL_PORT
        /* With the -1 sentinel, the original reads the zero-filled word at
           0x0059CAAA just before the weapon table and then turns locking off.
           Native globals have sanitizer redzones, so make that result explicit. */
        if (nSelectedReleaseWeaponIndex == -1) {
            lock_off();
            return;
        }
        weaponType = ((ShipWeaponSlot *)&aShipWeaponsTable[0][1])[
            nSelectedReleaseWeaponIndex].type;
#else
        weaponType = *(enum ObjectType *)(
            &aShipWeaponsTable[0][1] +
            nSelectedReleaseWeaponIndex * 7);
#endif
        switch (weaponType) {
        default:
            lock_off();
            return;
        case OBJECT_TYPE_HEAT_SEEKING_MISSILE:
            get_facing_range_from_object(0, (short)target);
            if (nTargetFacingValue > -0x41) {
                lock_off();
                return;
            }
            if (starting_lock(0x12) == 0)
                decrement_lock_time(x);
            return;
        case OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE:
            if (starting_lock(0x20) != 0)
                return;
            decrement_lock_time(x);
            return;
        }
    }
    lock_off();
}

/* Function start: 0x416220 */
void SetRectBounds(Viewport *viewport, unsigned short a, unsigned short b,
                   unsigned short c, unsigned short d)
{
    viewport->left = (short)a;
    viewport->top = (short)b;
    viewport->right = (short)c;
    viewport->bottom = (short)d;
}

/* Function start: 0x416250 */
short GetRectHeight(const Viewport *viewport)
{
    return viewport->right - viewport->left;
}

/* Target-box callees from other modules. */
short GetTransformedShapeBounds(Viewport *viewport, short x, short y,
                              unsigned char *shape, short frame,
                              short angle, short scale, short flip,
                              short *bounds);
void DrawViewportBorder(Viewport *viewport, short left, short top,
                        short right, short bottom,
                        short colour);

/* Function start: 0x416260 */
void print_message_text(char *text, unsigned short colour)
{
    TextContext context;
    Viewport viewport;
    char wrapped[84];
    char source[84];
    short charactersPerLine;
    short lastSpace = -1;
    short position;
    int characterPosition;
    char *input;
    char *output;

    if (text == 0)
        return;

    DosStrcpy(source, text);
    wrapped[0] = *"";
    viewport = *stHudMessageTextContext.viewport;
    context = stHudMessageTextContext;
    context.viewport = &viewport;

    SetRectBounds(&viewport,
                  (unsigned short)asHudMessageOrigins[
                      (int)cCurrentCockpitView * 2],
                  (unsigned short)asHudMessageOrigins[
                      (int)cCurrentCockpitView * 2 + 1],
                  (unsigned short)(
                      319 - asHudMessageOrigins[
                                (int)cCurrentCockpitView * 2]),
                  (unsigned short)(
                      asHudMessageOrigins[
                          (int)cCurrentCockpitView * 2 + 1] + 60));
    context.backgroundColour = 0xff;
    context.colour = colour;
    charactersPerLine = GetRectHeight(&viewport) / 6;
    input = source;
    output = wrapped;
    for (position = 0; *input != 0; position++, input++, output++) {
        *output = *input;
        if (*output == ' ')
            lastSpace = position;
        characterPosition = position;
        if ((characterPosition + 1) % charactersPerLine == 0) {
            if (lastSpace == -1) {
                output[1] = '\n';
                output++;
            } else {
                characterPosition -= lastSpace;
                output[characterPosition] = '\n';
            }
        }
    }
    *output = 0;

    if (bCockpitlessView != 0) {
        switch ((int)cCurrentCockpitView) {
        case 0:
            DrawTextAt(&context,
                       asHudMessageOrigins[(int)cCurrentCockpitView * 2],
                       (short)(asHudMessageOrigins[
                                   (int)cCurrentCockpitView * 2 + 1] + 10),
                       wrapped, 2);
            break;
        case 1:
            DrawTextAt(&context,
                       asHudMessageOrigins[(int)cCurrentCockpitView * 2],
                       (short)(asHudMessageOrigins[
                                   (int)cCurrentCockpitView * 2 + 1] + 25),
                       wrapped, 2);
            break;
        case 2:
            DrawTextAt(&context,
                       asHudMessageOrigins[(int)cCurrentCockpitView * 2],
                       (short)(asHudMessageOrigins[
                                   (int)cCurrentCockpitView * 2 + 1] + 50),
                       wrapped, 2);
            break;
        case 3:
            DrawTextAt(&context,
                       asHudMessageOrigins[(int)cCurrentCockpitView * 2],
                       asHudMessageOrigins[(int)cCurrentCockpitView * 2 + 1],
                       wrapped, 2);
            break;
        default:
            break;
        }
    } else {
        DrawTextAt(&context,
                   asHudMessageOrigins[(int)cCurrentCockpitView * 2],
                   asHudMessageOrigins[(int)cCurrentCockpitView * 2 + 1],
                   wrapped, 2);
    }
    pszDisplayedHudMessage = pszPendingHudMessage;
}

/* Function start: 0x416460 */
void ShowHudTextLine(char *s, unsigned short b)
{
    pszPendingHudMessage = s;
    print_message_text(s, b);
}

/* Function start: 0x416480 */
void SetHudTextColour(short v)
{
    if (v != 0)
        EndCommMenu();
    {
        char *text = pszDisplayedHudMessage;

        print_message_text(text, (unsigned char)cPrimaryViewBufferColour);
    }
}

/* Function start: 0x4164B0 */
void draw_target_box(unsigned short colour, signed char object,
                     short solid, short drawLockMarker, short padding,
                     ShortRect *savedBounds)
{
    short centerX;
    short centerY;
    ShortRect bounds;
    int colourValue;
    short valid;

    colourValue = (short)colour;
    if ((int)(unsigned char)cPrimaryViewBufferColour == colourValue) {
        valid = savedBounds->left != -0x7fff;
        bounds = *savedBounds;
    } else {
        valid = object != -1 &&
                asObjectScreenXList[object] != -0x7fff;
        if (valid != 0) {
            centerX = (short)(asObjectScreenXList[object] +
                              nCurrentViewCenterPosX);
            centerY = (short)(asObjectScreenYArray[object] +
                              nCurrentViewCenterPosY);
            if ((short)GetTransformedShapeBounds(
                    &stSpaceBufferViewport, centerX, centerY,
                    apObjShapeArray[object],
                    asObjectViewFrames[object],
                    asObjScreenAngles[object],
                    asObjScreenScalesArray[object],
                    asObjectFlipArray[object],
                    (short *)&bounds) != 0) {
                bounds.left = (short)(bounds.left - padding);
                bounds.top = (short)(bounds.top - padding);
                bounds.right = (short)(bounds.right + padding);
                bounds.bottom = (short)(bounds.bottom + padding);
            } else {
                valid = 0;
            }
        }
    }
    if (valid != 0) {
        if ((int)(unsigned char)cRedColour == colourValue &&
            aeShipSide[object] == aeShipSide[0]) {
            colour = (unsigned char)cBlueColour;
        }
        if (solid != 0) {
            DrawViewportBorder(&stSpaceBufferViewport, bounds.left, bounds.top,
                               bounds.right, bounds.bottom, colour);
        } else {
            short segmentLength;

            segmentLength =
                (short)(((int)bounds.right - bounds.left) / 6 + 1);
            DrawViewportLine(&stSpaceBufferViewport, bounds.left, bounds.top,
                             (short)(bounds.left + segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&stSpaceBufferViewport, bounds.left, bounds.bottom,
                             (short)(bounds.left + segmentLength), bounds.bottom,
                             colour);
            DrawViewportLine(&stSpaceBufferViewport, bounds.right, bounds.top,
                             (short)(bounds.right - segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&stSpaceBufferViewport, bounds.right, bounds.bottom,
                             (short)(bounds.right - segmentLength), bounds.bottom,
                             colour);
            segmentLength =
                (short)(((int)bounds.bottom - bounds.top) / 6 + 1);
            DrawViewportLine(&stSpaceBufferViewport, bounds.left, bounds.top,
                             bounds.left, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&stSpaceBufferViewport, bounds.left, bounds.bottom,
                             bounds.left,
                             (short)(bounds.bottom - segmentLength), colour);
            DrawViewportLine(&stSpaceBufferViewport, bounds.right, bounds.top,
                             bounds.right, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&stSpaceBufferViewport, bounds.right, bounds.bottom,
                             bounds.right,
                             (short)(bounds.bottom - segmentLength), colour);
        }
        if (drawLockMarker != 0) {
            if ((int)(short)colour != (int)(unsigned char)cPrimaryViewBufferColour) {
                if (nTargetLockCountdown > -1) {
                    nTargetLockMarkerAngle = (short)(
                        nTargetLockMarkerAngle +
                        anObjectRollRotationList[0] +
                        anObjPitchRot[0]);
                    centerX = (short)(centerX +
                        ((CosFixed(nTargetLockMarkerAngle) *
                          nTargetLockCountdown * 2) >> 8));
                    centerY = (short)(centerY +
                        ((SinFixed(nTargetLockMarkerAngle) *
                          nTargetLockCountdown * 2) >> 8));
                    DrawSpriteDefault(&stSpaceBufferViewport, centerX, centerY,
                                      pTgtLockShapePtr, 1);
                    nTargetLockMarkerX = centerX;
                    nTargetLockMarkerY = centerY;
                }
            } else if (nTargetLockMarkerX != -0x7fff) {
                DrawSolidColourSprite(&stSpaceBufferViewport,
                                      nTargetLockMarkerX,
                                      nTargetLockMarkerY,
                                      pTgtLockShapePtr, 1,
                                      cPrimaryViewBufferColour);
                nTargetLockMarkerX = -0x7fff;
            }
        }
        if ((int)(unsigned char)cPrimaryViewBufferColour == (int)(short)colour) {
            savedBounds->left = -0x7fff;
        } else {
            *savedBounds = bounds;
        }
    } else {
        savedBounds->left = -0x7fff;
    }
}

/* Navigation-pointer callees. */
void remove_object(short obj);
long DivideFixed(int numerator, int denominator);

/* Function start: 0x4168A0 */
void remove_nav_pointer(void)
{
    if (nNavPointerObject != -1)
        remove_object(nNavPointerObject);
}

/* Function start: 0x4168C0 */
void draw_nav_pointer(void)
{
    FixedVector objectivePosition;
    FixedVector direction;
    FixedVector viewPosition;
    int distance;
    short active;
    short object;

    if (get_mode(1) == 5 &&
        nCannedSceneMode != 4 &&
        (nCameraViewMode == 0 || nCameraViewMode == 4))
        active = 1;
    else
        active = 0;
#ifdef SDL_PORT
    /* SWC CODE_13 +0x3460 keeps navigation visible alongside target mode. */
    if (SdlSwcFlightActive())
        active = nTrainSimActive == 0 && nCannedSceneMode != 4 &&
            nCameraViewMode == 0 && cCurrentObjective >= 0 &&
            cCurrentObjective < cMissionObjectiveCountValue;
#endif
    if (active == 0) {
        remove_nav_pointer();
        return;
    }
    object = nNavPointerObject;
    if (object == -1) {
        object = find_vacant_3d_object();
        nNavPointerObject = object;
        if (object == -1)
            return;
        aeObjectClass[object] = OBJECT_CLASS_PLANET;
        asObjectViewFrames[object] = 3;
        asObjScreenAngles[object] = 0;
        asObjScreenScalesArray[object] = 0x100;
        apObjShapeArray[object] =
            pTgtLockShapePtr;
        acObjOwnersTable[object] = -1;
    }
    nNavPointerObject = object;
    asObjectScreenXList[object] = (short)0x8001;
    asObjDistancesTable[object] = 0;
    objectivePosition = aMissionObjectivesArray[
        (signed char)cCurrentObjective].position;
    ComputeVectorDelta(&aShipPositionsArray[EYE_OBJECT],
                       &objectivePosition, &direction);
    distance = Vector_magnitude(&direction);
    if (asObjectCollisionRadius[EYE_OBJECT] * 0x100 >=
        distance)
        return;
    transform_to_objects_frame(&direction, &viewPosition,
                               EYE_OBJECT);
    if (asObjectCollisionRadius[EYE_OBJECT] * 0x100 >
        viewPosition.z)
        return;
    if (DivideFixed(viewPosition.z, distance) < 0x94)
        return;
    asObjectScreenXList[nNavPointerObject] = (short)(DivideFixed(
        MultiplyFixed(
            (short)(nScreenWidth & ~1) << 7,
            viewPosition.x),
        viewPosition.z) >> 8);
    asObjectScreenYArray[nNavPointerObject] = (short)(DivideFixed(
        MultiplyFixed(
            (short)(nScreenWidth & ~1) << 7,
            viewPosition.y),
        viewPosition.z) >> 8);
    asObjDistancesTable[nNavPointerObject] = 0x4a38;
}

/* Function start: 0x416AC0 */
unsigned int overlay_head_up_display(void)
{
    target_locking(acShipTargetTable[0]);
    if (message_showing() && nCommSpeakerObject != -1) {
        cLastTargetObject =
            (signed char)nCommSpeakerObject;
        draw_target_box(cYellowColour,
                        cLastTargetObject,
                        0, 0, 2,
                        &stPreviousTargetBracketBounds);
    }
    if (nTargetLockCountdown == 0) {
        if ((short)(nRenderedSpaceFrame % 2) == 0)
            bTargetBracketVisible ^= 1;
        if (bTargetBracketVisible == 1) {
            draw_target_box(cRedColour,
                            acShipTargetTable[0],
                            nTargetLockMode,
                            1, 1,
                            &stTargetBracketBounds);
        }
    } else {
        draw_target_box(cRedColour,
                        acShipTargetTable[0],
                        nTargetLockMode,
                        1, 1,
                        &stTargetBracketBounds);
    }

    if (bCockpitlessView != 0) {
        switch (cCurrentCockpitView) {
        case 0:
        case 2:
            goto centered_sight;
        case 1:
            DrawSpriteDefault(&stSpaceBufferViewport,
                              nCurrentViewCenterPosX,
                              (short)(nCurrentViewCenterPosY - 1),
                              pTgtLockShapePtr, 0);
            break;
        case 3:
            DrawSpriteDefault(&stSpaceBufferViewport,
                              nCurrentViewCenterPosX,
                              (short)(nCurrentViewCenterPosY + 14),
                              pTgtLockShapePtr, 0);
            break;
        default:
            goto no_sight;
        }
        goto no_sight;
    }
centered_sight:
    DrawSpriteDefault(&stSpaceBufferViewport,
                      nCurrentViewCenterPosX,
                      nCurrentViewCenterPosY,
                      pTgtLockShapePtr, 0);

no_sight:
    DAT_0046c05c = 0;
    if (pszPendingHudMessage != 0)
        ShowHudTextLine(pszPendingHudMessage, nPendingHudMessageColour);
    if (bMouseCursorVisible == 1) {
        nStoredMouseCursorPosX = stMouseCursorState.x;
        nSavedMouseCursorY = stMouseCursorState.y;
        CaptureSpriteBackground(stMouseCursorState.viewport,
                                abMouseCursorBackground,
                                stMouseCursorState.x,
                                stMouseCursorState.y,
                                stMouseCursorState.shape,
                                stMouseCursorState.frame);
        DrawSpriteDefault(stMouseCursorState.viewport,
                          stMouseCursorState.x,
                          stMouseCursorState.y,
                          stMouseCursorState.shape,
                          stMouseCursorState.frame);
    }
    return 0;
}

/* Function start: 0x416C90 */
void RestoreCockpitExplosionIfVisible(void)
{
    if (IsCockpitExplosionActive() &&
        pCockpitExplosionBackground != 0) {
        RestoreCockpitExplosionBackground();
    }
}

/* Function start: 0x416CB0 */
unsigned int RestoreTransientCockpitGraphics(void)
{
    char *displayedMessage;
    void (*restoreBackground)(Viewport *viewport, unsigned char *background,
                               short x, short y, unsigned char *shape,
                               short) = RestoreSpriteBackground;
    void (*drawTargetBracket)(unsigned short colour, signed char object,
                             short solid, short drawLockMarker, short padding,
                             ShortRect *savedBounds) = draw_target_box;

    if (bMouseCursorVisible == 1) {
        restoreBackground(stMouseCursorState.viewport,
                          abMouseCursorBackground,
                          (short)nStoredMouseCursorPosX,
                          (short)nSavedMouseCursorY,
                          stMouseCursorState.shape,
                          (short)stMouseCursorState.frame);
    }
    if (cLastTargetObject != -1) {
        drawTargetBracket(cPrimaryViewBufferColour,
                          cLastTargetObject,
                          0, 0, 2,
                          &stPreviousTargetBracketBounds);
        cLastTargetObject = -1;
    }
    drawTargetBracket(cPrimaryViewBufferColour, acShipTargetTable[0],
                      nTargetLockMode, 1, 1,
                      &stTargetBracketBounds);
    /* Keep this read separate from the following pointer comparison. */
    displayedMessage = *(char *volatile *)&pszDisplayedHudMessage;
    if (displayedMessage != pszPendingHudMessage &&
        displayedMessage != 0)
        SetHudTextColour(0);
    if (IsCockpitExplosionActive() &&
        pCockpitExplosionBackground != 0) {
        if (bCockpitlessView == 0) {
            short x = stCockpitExplosionPosition.x;

            CaptureSpriteBackground(
                &stScreenView, pCockpitExplosionBackground,
                x, stCockpitExplosionPosition.y,
                pCockpitExplosionShape,
                nCockpitExplosionFrame);
        }
        if (bCockpitlessView == 0) {
            DrawSpriteDefault(&stScreenView,
                              stCockpitExplosionPosition.x,
                              stCockpitExplosionPosition.y,
                              pCockpitExplosionShape,
                              nCockpitExplosionFrame);
        }
        cRenderedPilotHandFrame = -1;
    }
    return 0;
}

/* Function start: 0x416DE0 */
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration)
{
    if (bInflightComputerActive == 0) {
        if (message_showing())
            SetHudTextColour(1);
        pszPendingHudMessage = text;
        nPendingHudMessageColour = colour;
        set_message_time(duration);
    }
}

/* Function start: 0x416E20 */
void malf_noise(short vdu, int effect, unsigned short colour,
                short sound, short refresh)
{
    Viewport *viewport = vdu == 0 ? &stLeftVdu : &stRightVduViewport;

    if (sound != -1) {
        if (sound == 0x17)
            PlaySnowStaticSound();
        else
            PlaySfxWaveFileByNumber(sound, -1, 0);
    }
    snow_viewport(viewport, effect, colour);
    if (refresh != 0)
        set_new_vdu(vdu);
}

/* Function start: 0x416E90 */
void build_your_target_list(short *hasEnemy)
{
    short distance;
    enum Side playerSide;
    signed char object;
    signed char targetIndex;

    *hasEnemy = 0;
    cViableTargetCount = 0;
    object = 1;
    do {
        if (aeObjectClass[(int)object] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[(int)object] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            asObjectScreenXList[(int)object] != (short)0x8001 &&
            (unsigned short)(distance = asObjDistancesTable[(int)object]) <
                12000) {
            targetIndex = cViableTargetCount;
            asViableTgtDistList[(int)targetIndex] = distance;
            acViableTargetList[(int)targetIndex] = object;
            cViableTargetCount++;
            playerSide = aeShipSide[0];
            if (aeShipSide[(int)object] != playerSide)
                *hasEnemy = 1;
        }
        object++;
    } while (object <= 9);
    if (cViableTargetCount > 1)
        sort_viable_target_list();
}

/* Function start: 0x416F30 */
void cycle_onscreen_targets(void)
{
    signed char index;
    short previousTarget;
    signed char count;
    short hasEnemy;

    previousTarget = acShipTargetTable[0];
    build_your_target_list(&hasEnemy);
    count = *(volatile signed char *)&cViableTargetCount;
    if (count == 0) {
        acShipTargetTable[0] = -1;
    } else {
        for (index = 0;
             index < count &&
             acViableTargetList[(int)index] !=
                 acShipTargetTable[0];
             index++)
            ;
        do {
            index = (signed char)((index + 1) %
                                  count);
            acShipTargetTable[0] =
                acViableTargetList[(int)index];
            if (hasEnemy == 0)
                break;
        } while (aeShipSide[
                     acShipTargetTable[0]] ==
                 aeShipSide[0]);
    }
    if (acShipTargetTable[0] != previousTarget)
        nTargetLockCountdown = -1;
}

/* Function start: 0x416FD0 */
void check_target(void)
{
    short selectNewTarget;
    short oldTarget;
    short targetIndex;
    short hasEnemy;
    unsigned char count;

    selectNewTarget = 1;
    oldTarget = acShipTargetTable[0];
    if (oldTarget != -1 &&
        aeSpecialManeuvers[oldTarget] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        acShipTargetTable[0] = -1;
        oldTarget = -1;
    }
    if (nTargetLockMode != 0 &&
        (short)(nRenderedSpaceFrame % 8) == 0 &&
        malf(5) != 0) {
        nTargetLockMode = 0;
        malf_sound();
    }
    if (oldTarget != -1 &&
        (nTargetLockMode != 0 ||
         asObjectScreenXList[oldTarget] != (short)0x8001) &&
        (nTargetLockMode != 0 ||
         aeShipSide[oldTarget] != aeShipSide[0]))
        return;
    if (oldTarget == -1)
        nTargetLockMode = 0;

    build_your_target_list(&hasEnemy);
    count = cViableTargetCount;
    if (count == 0) {
        if (nTargetLockMode != 0)
            acShipTargetTable[0] = oldTarget;
        else
            acShipTargetTable[0] = -1;
    } else {
        if (hasEnemy == 0 && oldTarget != -1 &&
            aeShipSide[oldTarget] ==
                aeShipSide[0] &&
            (nTargetLockMode != 0 ||
             asObjectScreenXList[oldTarget] != (short)0x8001)) {
            selectNewTarget = 0;
            acShipTargetTable[0] = oldTarget;
        }
        if (selectNewTarget != 0) {
            for (targetIndex = 0;
                 targetIndex < (signed char)count &&
                 aeShipSide[
                     (short)acViableTargetList[targetIndex]] ==
                     aeShipSide[0];
                 targetIndex++)
                ;
            acShipTargetTable[0] =
                acViableTargetList[
                    targetIndex % (signed char)count];
        }
    }
    if (acShipTargetTable[0] != oldTarget) {
        if (oldTarget != -1 && acShipTargetTable[0] == -1)
            nTargetLockMode = 0;
        nTargetLockCountdown = -1;
    }
}

/* Function start: 0x417190 */
void update_missile_warning(void)
{
    if (missile_on_tail(0) != 0) {
        SetCockpitLightBlink(2, 1);
        if (nTrainSimActive == 0)
            spacetrack(3, 1, -1);
    } else {
        abCockpitLightGoalArray[2] = 0;
    }
}

/* Function start: 0x4171D0 */
void determine_pilot_hand(void)
{
    short yaw;
    short pitch;

    yaw = nYawInputValue / 2;
    pitch = nPitchInput / 2;
    if (yaw > 0) {
        bStickIndicatorFrameState =
            (signed char)MinShort((short)(yaw + 8), 12);
        return;
    }
    if (yaw < 0) {
        bStickIndicatorFrameState =
            (signed char)MinShort((short)(4 - yaw), 8);
        return;
    }
    if (pitch > 0) {
        bStickIndicatorFrameState =
            (signed char)MinShort((short)(pitch + 12), 16);
        return;
    }
    if (pitch < 0) {
        bStickIndicatorFrameState =
            (signed char)MinShort((short)-pitch, 4);
        return;
    }
    bStickIndicatorFrameState = 0;
}

/* Function start: 0x417260 */
void DrawPilotHandFrame(void)
{
    int view = (int)cCurrentCockpitView;
    short x = (short)(asPilotHandOrigins[view * 2] -
                      stPilotHandView.left);
    short y = (short)(asPilotHandOrigins[view * 2 + 1] -
                      stPilotHandView.top);

    CopyViewportContents(&stPilotHandBackdropView,
                         &stPilotHandComposite);
    DrawSpriteDefault(&stPilotHandComposite, x, y, pCurrentPilotHandShapePtr,
                      (short)bStickIndicatorFrameState);
    DrawSpriteDefault(
        &stPilotHandComposite,
        (short)(x + asPilotHandOffsets[
            bStickIndicatorFrameState * 2]),
        (short)(y + asPilotHandOffsets[
            bStickIndicatorFrameState * 2 + 1]),
        pCurrentPilotHandShapePtr, 0x11);
    CopyViewportContents(&stPilotHandComposite, &stPilotHandView);
    cRenderedPilotHandFrame = bStickIndicatorFrameState;
}

/* Function start: 0x417320 */
void CopyTrainSimPilotViewToRightVdu(void)
{
    Viewport destination;

    destination = stPilotHandBackdropView;
    if (stTrainSimVduSource.left == 0) {
        stTrainSimVduSource = stPilotHandView;
        stTrainSimVduSource.left = stRightVduViewport.left;
        stTrainSimVduSource.right = stPilotHandView.right;
        stTrainSimVduSource.top = stPilotHandView.top;
        stTrainSimVduSource.bottom = stRightVduViewport.bottom;
    }
    destination.left = (short)(stTrainSimVduSource.left -
                               stPilotHandView.left);
    destination.bottom = (short)(stTrainSimVduSource.bottom -
                                 stPilotHandView.top);
    CopyViewportContents(&stTrainSimVduSource, &destination);
}

/* Function start: 0x4173C0 */
void animate_pilot(void)
{
    if (pCurrentPilotHandShapePtr != 0) {
        determine_pilot_hand();
        if (*(volatile signed char *)&cRenderedPilotHandFrame !=
            bStickIndicatorFrameState)
            DrawPilotHandFrame();
    }
}

/* Function start: 0x4173F0 */
void ResetPilotHandAnimation(void)
{
    if (pCurrentPilotHandShapePtr != 0) {
        cRenderedPilotHandFrame = -1;
        CopyViewportContents(&stPilotHandView, &stPilotHandBackdropView);
        animate_pilot();
    }
}

/* Function start: 0x417420 */
void send_message(short obj, short message)
{
    if (nTrainSimActive == 0 &&
        aeObjectClass[obj] != OBJECT_CLASS_NULL &&
        nCannedSceneMode == 0) {
        if (nYourWingman != -1 &&
            nYourWingman == obj &&
            bRadioSilence != 0) {
            acWingmanMessageStatesArray[obj] = -1;
            return;
        }
        if (obj > -1 && obj < 10 &&
            aeObjectClass[obj] >= OBJECT_CLASS_SHIP) {
            if (acShipRating[obj] != -1) {
                acWingmanMessageStatesArray[obj] = message;
                return;
            }
            if (aeObjectTypeList[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW ||
                nShipMissionIndices[obj] ==
                    anShipMissionShips[0]) {
                acWingmanMessageStatesArray[obj] = message;
            } else if (aeShipSide[obj] == SIDE_KILRATHI) {
                acWingmanMessageStatesArray[obj] = message;
                return;
            }
        }
    }
}

/* Function start: 0x4174F0 */
void npc_communication(void)
{
    signed char obj;

    if (nCannedSceneMode == 0 &&
        nTrainSimActive == 0) {
        obj = 1;
        while (message_showing() == 0 && obj <= 9) {
            if (aeObjectClass[(short)obj] >=
                    OBJECT_CLASS_SHIP &&
                acWingmanMessageStatesArray[(short)obj] != -1) {
                vid_equiv((short)obj,
                          (short)acWingmanMessageStatesArray[(short)obj]);
                acWingmanMessageStatesArray[(short)obj] = -1;
            }
            obj++;
        }
        if (RandomBelowOrEqual(5000) > 4998) {
            for (obj = 1; nCommSpeakerObject == -1 && obj <= 9; obj++) {
                if (aeObjectClass[(short)obj] >=
                        OBJECT_CLASS_SHIP &&
                    aeShipSide[(short)obj] == SIDE_KILRATHI &&
                    (aeShipObjectiveList[(short)obj] ==
                         OBJECTIVE_ENGAGE_ENEMY ||
                     aeShipObjectiveList[(short)obj] ==
                         OBJECTIVE_DESTROY_SHIP) &&
                    (acShipRating[(short)obj] != -1 ||
                     RandomBelowOrEqual(100) < 20)) {
                    acWingmanMessageStatesArray[(short)obj] =
                        (signed char)(RandomBelowOrEqual(2) + 2);
                    return;
                }
            }
        }
    }
}

/* Function start: 0x417610 */
void clear_cockpit_damage(void)
{
    memset(anCockpitDamageState, 0,
           sizeof(anCockpitDamageState));
}

/* Function start: 0x417630 */
void explosion_draw(void)
{
    short damage;

    if (pCockpitPilotShape == 0)
        pCockpitPilotShape =
            FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 4, 0);
    damage = 0;
    do {
        if (anCockpitDamageState[damage] == 1) {
            DrawSpriteDefault(
                &stScreenView,
                aaCockpitDamagePositions[
                    (int)cCurrentCockpitView][damage].x,
                aaCockpitDamagePositions[
                    (int)cCurrentCockpitView][damage].y,
                pCockpitPilotShape, damage);
        }
        damage++;
    } while (damage < 4);
    FreePacketAndClear(&pCockpitPilotShape, 0);
}

/* Function start: 0x4176C0 */
unsigned int DrawPendingCockpitDamage(void)
{
    if (pCockpitPilotShape == 0)
        pCockpitPilotShape =
            FetchDiskPacketRetrying(
                (short)cCockpitLogicalFile, 4, 0);
    DrawSpriteDefault(&stScreenView,
                      stCockpitExplosionPosition.x,
                      stCockpitExplosionPosition.y,
                      pCockpitPilotShape,
                      nPendingCockpitDamageValue);
    if (pCurrentPilotHandShapePtr != 0) {
        DrawSpriteDefault(&stPilotHandBackdropView,
                          (short)(stCockpitExplosionPosition.x -
                                  stPilotHandView.left),
                          (short)(stCockpitExplosionPosition.y -
                                  stPilotHandView.top),
                          pCockpitPilotShape,
                          nPendingCockpitDamageValue);
    }
    FreePacketAndClear(&pCockpitPilotShape, 0);
    return 0;
}

/* Function start: 0x417760 */
void RestoreCockpitExplosionBackground(void)
{
    if (IsCockpitExplosionActive() &&
        pCockpitExplosionShape != 0 &&
        pCockpitExplosionBackground != 0) {
        RestoreSpriteBackground(
            &stScreenView, pCockpitExplosionBackground,
            stCockpitExplosionPosition.x,
            stCockpitExplosionPosition.y,
            pCockpitExplosionShape,
            nCockpitExplosionFrame);
    }
}

/* Function start: 0x4177B0 */
void cockpit_explosion(void)
{
    short frame;

    if (nCockpitExplosionFrame == 0x7fff)
        nCockpitExplosionFrame = 0;
    if (IsCockpitExplosionActive()) {
        frame = nCockpitExplosionFrame;
        if (frame == 0)
            PlaySfxWaveFileByNumber(0x1b, -1, 0);
        if (++nCockpitExplosionFrame == 3)
            DrawPendingCockpitDamage();
        if (IsCockpitExplosionActive() &&
            pCockpitExplosionShape != 0 &&
            pCockpitExplosionBackground != 0) {
            CaptureSpriteBackground(
                &stScreenView, pCockpitExplosionBackground,
                stCockpitExplosionPosition.x,
                stCockpitExplosionPosition.y,
                pCockpitExplosionShape,
                nCockpitExplosionFrame);
            DrawSpriteDefault(&stScreenView,
                              stCockpitExplosionPosition.x,
                              stCockpitExplosionPosition.y,
                              pCockpitExplosionShape,
                              nCockpitExplosionFrame);
            cRenderedPilotHandFrame = -1;
        }
        return;
    }
    FreePacketAndClear(&pCockpitExplosionShape, 0);
}

/* Function start: 0x4178A0 */
void place_damage_on_cockpit(short damage)
{
    unsigned int *state;

#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        SdlSwcCockpitDamage(damage);
        return;
    }
#endif
    if (nCameraViewMode == 0 && nTrainSimActive == 0 &&
        *(state = &anCockpitDamageState[damage]) == 0) {
        if (pCockpitExplosionShape == 0) {
            nPendingCockpitDamageValue = damage;
            *state = 1;
            explosion_draw();
            return;
        }
        nPendingCockpitDamageValue = damage;
        *state = 1;
        if (IsCockpitExplosionActive() == 0) {
            stCockpitExplosionPosition =
                aaCockpitDamagePositions[
                    (signed char)cCurrentCockpitView][damage];
            nCockpitExplosionFrame = 0x7fff;
        }
    }
}

/* Function start: 0x417910 */
void vid_transmit(void)
{
    short randomFrame;
    short speaker;

    speaker = nCommSpeakerObject;
    if (aeShipSide[speaker] == SIDE_NEUTRAL) {
        EndCommSessionWithWingman();
        return;
    }
    /* The original repeats the cockpitless check after frame parity. */
    if ((bCockpitlessView != 0 ||
         nRenderedSpaceFrame % 2 != 0 ||
         bCockpitlessView != 0) &&
        nCommPortraitIndex != -1 &&
        aapszPilotSpeeches[nCommPortraitIndex] != 0 &&
        bVideoImagesSuppressed == 0) {
        if (aeSpecialManeuvers[speaker] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
            if (aeShipSide[speaker] == SIDE_IMPERIAL) {
                DrawSpriteDefault(
                    &stRightVduViewport, stRightVduViewport.left,
                    stRightVduViewport.top,
                    pCommStaticShape,
                    (short)(asObjectCounterArray[speaker] / 5));
                return;
            }
            DrawSpriteDefault(&stRightVduViewport, stRightVduViewport.left,
                              stRightVduViewport.top,
                              pCommStaticShape, 2);
            return;
        }
        if (nCommPortraitFrame == -1) {
            nCommPortraitFrame =
                (unsigned short)RandomInRange(0, 2);
        }
        randomFrame = RandomInRange(0, 3);
        if (randomFrame < 3)
            nCommPortraitFrame = randomFrame;
        set_new_vdu(1);
        if (aeShipSide[nCommSpeakerObject] == SIDE_IMPERIAL) {
            DrawSpriteDefault(
                &stRightVduViewport, stRightVduViewport.left, stRightVduViewport.top,
                pConfedCommBackground, 0);
        } else {
            DrawSpriteDefault(
                &stRightVduViewport, stRightVduViewport.left, stRightVduViewport.top,
                pKilrathiCommBackground, 0);
        }
        DrawSpriteDefault(
            &stRightVduViewport, stRightVduViewport.left, stRightVduViewport.top,
            apCommunicationPortraitShapesTable[nCommPortraitIndex],
            (short)nCommPortraitFrame);
    }
}

/* Function start: 0x417AC0 */
void vid_equiv(short obj, short message)
{
    if (get_mode(1) != 4 &&
        nTrainSimActive == 0 &&
        nCannedSceneMode == 0 && nCameraViewMode == 0 &&
        message_showing() == 0)
        real_vid_transmit(obj, message);
}

/* Function start: 0x417B10 */
void update_dead_disp(short a)
{
    malf_noise(a, 1, cDarkGreenColour, 0x17, 0);
}

/* Function start: 0x417B30 */
void check_stranded(void)
{
    if (nTrainSimActive == 0 &&
        aMissionShips[
            nCarrierMissionShipIndexValue].state == 3 &&
        any_enemy(0, 30000) == 0)
        nArcadeState = 3;
}

/* Function start: 0x417B70 */
void update_VDUs(void)
{
    short changed;

    SetTextContext(&stLeftVduTextContext);
    if (bCockpitlessView != 0) {
        DrawFilledViewportRect(
            &stScreenView,
            stCockpitLayout.leftVduBounds[
                (int)cCurrentCockpitView].left,
            stCockpitLayout.leftVduBounds[
                (int)cCurrentCockpitView].top,
            stCockpitLayout.leftVduBounds[
                (int)cCurrentCockpitView].right,
            stCockpitLayout.leftVduBounds[
                (int)cCurrentCockpitView].bottom,
            0);
        DrawFilledViewportRect(
            &stScreenView,
            stCockpitLayout.rightVduBounds[
                (int)cCurrentCockpitView].left,
            stCockpitLayout.rightVduBounds[
                (int)cCurrentCockpitView].top,
            stCockpitLayout.rightVduBounds[
                (int)cCurrentCockpitView].right,
            stCockpitLayout.rightVduBounds[
                (int)cCurrentCockpitView].bottom,
            0);
    }
    changed = update_vid_disp(0);
    if (changed != 0) {
        switch (get_mode(0)) {
        case 0:
            update_dead_disp(0);
            break;
        case 1:
            show_weapon_disp();
            break;
        case 2:
            show_damage_disp();
            break;
        case 8:
            show_info_disp();
            break;
        }
    } else {
        switch (get_mode(0)) {
        case 0:
            update_dead_disp(0);
            break;
        case 1:
            if (bCockpitlessView != 0)
                show_weapon_disp();
            update_status_text();
            break;
        case 2:
            if (bCockpitlessView != 0) {
                bForceDamageDisplayRedraw = 1;
                show_damage_disp();
                bForceDamageDisplayRedraw = 0;
            }
            UpdateDamageDisplay();
            break;
        case 8:
            show_info_disp();
            break;
        }
    }
    if (get_mode(0) == 0)
        aCockpitHudMessageSlotsTable[0].text = 0;
    else
        UpdateMessage(&aCockpitHudMessageSlotsTable[0]);

    SetTextContext(&stRightVduTxtContext);
    changed = update_vid_disp(1);
    if (changed != 0) {
        switch (get_mode(1)) {
        case 0:
            update_dead_disp(1);
            break;
        case 3:
            show_target_disp();
            break;
        case 4:
            show_communications_disp();
            break;
        case 5:
            show_navigation_disp();
            break;
        case 6:
            vid_transmit();
            break;
        }
    } else {
        switch (get_mode(1)) {
        case 0:
            update_dead_disp(1);
            break;
        case 3:
            if (bCockpitlessView != 0)
                show_target_disp();
            DrawTargetRangeReadout();
            break;
        case 4:
            if (bCockpitlessView != 0)
                show_communications_disp();
            talk_equiv();
            break;
        case 5:
            if (bCockpitlessView != 0)
                show_navigation_disp();
            check_objectives();
            break;
        case 6:
            vid_transmit();
            break;
        }
    }
    if (get_mode(1) == 6 || get_mode(1) == 0)
        aCockpitHudMessageSlotsTable[1].text = 0;
    else
        UpdateMessage(&aCockpitHudMessageSlotsTable[1]);
    if (nTrainSimActive != 0 &&
        pCurrentPilotHandShapePtr != 0)
        CopyTrainSimPilotViewToRightVdu();
}

/* Function start: 0x417E70 */
void update_cockpit(void)
{
    check_target();
    repair_internal_damage();
    if (cMissionObjectiveCountValue != 0)
        update_objective_location(
            (short)(nCurrentSpaceFrameValue %
                    (short)cMissionObjectiveCountValue));
    if (nCameraViewMode == 0) {
        if (bCockpitlessView == 0)
            RestoreCockpitExplosionBackground();
        update_lights();
        update_missile_warning();
        draw_3d_scanner();
        update_digital_readouts();
        update_VDUs();
        if (bCockpitlessView == 0)
            animate_pilot();
        update_bars();
        draw_cockpit_lights();
        if (bCockpitlessView == 0)
            cockpit_explosion();
        npc_communication();
    }
    fire_computer_graphic_missile();
    check_stranded();
}

/* Function start: 0x417F00 */
void PlayCockpitSelectionSfx(short selectionSound)
{
    (void)selectionSound;
    PlaySfxWaveFileByNumber(0x19, -1, 0);
}

/* Function start: 0x417F10 */
void vdu_pop_all(short vdu)
{
    int mode;

    while ((short)GetVduModeStackDepth(vdu) > 0) {
        mode = get_mode(vdu);
        if (mode != 6)
            pop_mode(vdu);
        else
            EndCommMenu();
    }
}

/* Function start: 0x417F60 */
void SelectCockpitVduMode(short vdu, int mode)
{
    short changed;

#ifdef SDL_PORT
    if (SdlSwcFlightActive()) {
        set_mode(vdu, mode);
        return;
    }
#endif
    if (nCameraViewMode != 0)
        return;
    if (malf(3) != 0 ||
        (mode == 4 && malf(4) != 0)) {
        vdu_malf(vdu, 0x17);
        return;
    }
    PlayCockpitSelectionSfx(asVduSelectionSound[vdu]);
    changed = get_mode(vdu) != mode;
    if (changed != 0) {
        vdu_pop_all(vdu);
        InvalidateVduMode(vdu);
        if (mode != 4) {
            set_mode(vdu, mode);
            update_VDUs();
            return;
        }
        show_communications_disp();
        update_VDUs();
        return;
    }
    switch (mode) {
    case 1:
        if (bCurrentKey == 0x22)
            select_new_gun();
        else
            select_new_release_weapon((enum ObjectType)-1);
        break;
    case 2:
        nCurrentDmgDispTicks = 0;
        break;
    case 3:
        cycle_onscreen_targets();
        break;
    case 4:
        talk_equiv();
        break;
    case 5:
        InflightComputer();
        break;
    }
}
