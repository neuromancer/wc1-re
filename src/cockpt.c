/*
 *  Cockpit HUD (Mac `cockpt` compilation unit): weapon, damage, target,
 *  objective, message, and pilot displays.
 *
 *  Address range 413000-417fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac `cockpt` symbol list identifies the surviving
 *  functions throughout this Win32 range; port-specific split helpers remain
 *  interleaved in their original address order.
 */
#include "wc1.h"

short asVduSelectionSound[2] = { 0x7f, 0 }; /* 0x00469000 */
char *pszPendingHudMessage = 0; /* 0x00469004 */
char *pszDisplayedHudMessage = 0; /* 0x00469008 */
unsigned char cRenderedPilotHandFrame = 0xff; /* 0x0046900c */
short asPilotHandOffsets[34] = { /* 0x00469018 */
    6, -3, 7, 2, 7, 9, 7, 12, 8, 13, 0, -1, -1, -1,
    -4, -1, -6, -1, 6, 0, 8, 0, 10, 0, 13, 3, 8, -7,
    6, -9, 5, -11, 5, -14
};
unsigned char *pCockpitPilotShape = 0; /* 0x0046905c */
unsigned char *pCockpitExplosionBackground = 0; /* 0x00469060 */
unsigned char * volatile pCockpitExplosionShape = 0; /* 0x00469064 */
volatile short nCockpitExplosionFrame = 8; /* 0x00469068 */
unsigned char *pReleaseWeaponDisplayBackground = 0; /* 0x0046906c */
signed char cReleaseWeaponDisplayFrame = -1; /* 0x00469070 */
signed char cReleaseWeaponDisplayTicks = 0; /* 0x00469074 */
signed char cReleaseWeaponDisplayState = 0; /* 0x00469078 */
int nDisplayedObjectiveRange = 40000; /* 0x00469088 */
char *pszObjectiveStatusMessage = /* 0x0046908c */
    szObjectiveStatusMessage;
short nScannerTargetObject = -1; /* 0x00469090 */
int aiScannerGridRows[79] = { /* 0x00469098 */
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
const char *PTR_s_MISSILE_LOCKED_004691d4 = szMissileLocked;
unsigned char bTargetBracketVisible = 1; /* 0x004691d8 */
unsigned char abTargetBracketPadding[7] = {0}; /* 0x004691d9 */
short asHudMessageOrigins[10] = { /* 0x004691e0 */
    18, 14, 71, 5, 80, 29, 49, 27, 14, 13
};
short nTargetLockMarkerX = -0x7fff; /* 0x004691f4 */
ShortRect stTargetBracketBounds = {-0x7fff, 0, 0, 0}; /* 0x004691f8 */
ShortRect stPreviousTargetBracketBounds = {-0x7fff, 0, 0, 0}; /* 0x00469200 */
short nNavPointerObject = -1; /* 0x00469208 */
Viewport stTrainSimVduSource = {0}; /* 0x00469210 */
ShortPoint aaCockpitDamagePositions[5][4] = { /* 0x00469228 */
    {{224, 5}, {132, 96}, {233, 107}, {149, 161}},
    {{177, 6}, {153, 142}, {103, 140}, {55, 183}},
    {{107, 25}, {211, 32}, {21, 178}, {300, 178}},
    {{74, 10}, {294, 19}, {197, 105}, {105, 134}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
unsigned char *pConfedCommBackground = 0; /* 0x00469278 */
unsigned char *pCommStaticShape = 0; /* 0x0046927c */
unsigned char *pKilrathiCommBackground = 0; /* 0x00469280 */
int nCommPortraitFrame = -1; /* 0x00469284 */
int bForceDamageDisplayRedraw = 0; /* 0x00469288 */
char szObjectiveStatusMessage[28] = /* 0x0046928c */
    "Wait for ??????????????????";
char szMissileLocked[16] = "MISSILE LOCKED "; /* 0x004692a8 */
char szAlreadyNear[16] = "Already Near"; /* 0x004692b8 */
char szEnemyNear[12] = "Enemy Near"; /* 0x004692c8 */
char szHazardNear[12] = "Hazard Near"; /* 0x004692d4 */
char szComponentHitFormat[8] = "%s HIT"; /* 0x004692e0 */
char szCalculating[12] = "CALCULATING"; /* 0x0046931c */
char szRangeKilometresSuffix[4] = " km"; /* 0x00469328 */
char szNoObjective[8] = "NONE"; /* 0x0046932c */
char szUnknownObjective[8] = "UNKNOWN"; /* 0x00469334 */
char szCompNavigation[16] = "COMP NAVIGATION"; /* 0x0046933c */
char szDestinationFormat[20] = /* 0x0046934c */
    "\n\nDESTINATION\n  %s";
char szNavigationRangeLabel[12] = "\n\nRANGE\n  "; /* 0x00469360 */
char szNewObjectivePrompt[20] = /* 0x0046936c */
    "\n\n(N)ew Objective";
char szAlreadyVisited[16] = "Already Visited"; /* 0x00469380 */
char szObjectiveReached[20] = "Objective Reached"; /* 0x00469390 */
char szWaitForFormat[12] = "Wait for %s"; /* 0x004693a4 */
unsigned char cEmptyHudMessage = 0; /* 0x004693b0 */
char *pGameVersion = szGameVersion; /* 0x004693b4 */
char szGameVersion[9] = "1.03F-95"; /* 0x004693b8 */

/* Function start: 0x413A10 */
void EmitTextString(void (__stdcall *writer)(int), const char *text)
{
    short character;

    character = (signed char)*text++;
    while (character != 0) {
        writer((short)character);
        character = (signed char)*text++;
    }
}

/* Function start: 0x413A40 */
void FormatTextTokens(void (__stdcall *writer)(int),
#ifdef WC1_SDL
                      const char *format, va_list *arguments)
#else
                      const char *format, unsigned long *arguments)
#endif
{
    short character;
    char number[64];
    char *text;

    character = (signed char)*format++;
    while (character != 0) {
        if (character != '%') {
            writer((short)character);
        } else {
            character = (signed char)*format++;
            switch (character) {
            case 'B':
#ifdef WC1_SDL
                pCurrentTextContext->backgroundColour =
                    (unsigned char)va_arg(*arguments, int);
#else
                pCurrentTextContext->backgroundColour =
                    (unsigned char)*arguments++;
#endif
                break;
            case 'D':
#ifdef WC1_SDL
                text = _ltoa(va_arg(*arguments, long), number, 10);
#else
                text = _ltoa((long)*arguments++, number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 'F':
#ifdef WC1_SDL
                pCurrentTextContext->colour =
                    (unsigned char)va_arg(*arguments, int);
#else
                pCurrentTextContext->colour =
                    (unsigned char)*arguments++;
#endif
                break;
            case 'J':
#ifdef WC1_SDL
                pCurrentTextContext->alignment =
                    (unsigned char)va_arg(*arguments, int);
#else
                pCurrentTextContext->alignment =
                    (unsigned char)*arguments++;
#endif
                break;
            case 'P':
                DrawTextString(pCurrentTextContext->text);
                break;
            case 'U':
#ifdef WC1_SDL
                text = _ultoa(va_arg(*arguments, unsigned long), number, 10);
#else
                text = _ultoa(*arguments++, number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 'X':
#ifdef WC1_SDL
                pCurrentTextContext->cursorX =
                    (short)va_arg(*arguments, int);
#else
                pCurrentTextContext->cursorX =
                    (short)*arguments++;
#endif
                break;
            case 'Y':
#ifdef WC1_SDL
                pCurrentTextContext->cursorY =
                    (short)va_arg(*arguments, int);
#else
                pCurrentTextContext->cursorY =
                    (short)*arguments++;
#endif
                break;
            case 'c':
#ifdef WC1_SDL
                writer((short)va_arg(*arguments, int));
#else
                writer((short)*arguments++);
#endif
                break;
            case 'd':
#ifdef WC1_SDL
                text = _itoa((short)va_arg(*arguments, int), number, 10);
#else
                text = _itoa((int)(short)*arguments++, number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 's':
#ifdef WC1_SDL
                text = va_arg(*arguments, char *);
#else
                text = (char *)*arguments++;
#endif
                EmitTextString(writer, text);
                break;
            case 'u':
#ifdef WC1_SDL
                text = _ultoa((unsigned short)va_arg(*arguments, unsigned int),
                              number, 10);
#else
                text = _ultoa((unsigned short)*arguments++, number, 10);
#endif
                EmitTextString(writer, text);
                break;
            case 'x':
#ifdef WC1_SDL
                text = _ultoa((unsigned short)va_arg(*arguments, unsigned int),
                              number, 16);
#else
                text = _ultoa((unsigned short)*arguments++, number, 16);
#endif
                EmitTextString(writer, _strupr(text));
                break;
            default:
                writer((short)character);
                break;
            }
        }
        character = (signed char)*format++;
    }
}

/* Function start: 0x413C40 */
void DrawFormattedText(const char *format, ...)
{
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    FormatTextTokens((void (__stdcall *)(int))DrawTextCharacter,
                     format, &arguments);
    va_end(arguments);
#else
    FormatTextTokens((void (__stdcall *)(int))DrawTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    if (pCurrentTextContext->viewport->pixels ==
        stScreen.pixels)
        DIBslam();
}

/* Function start: 0x413C70 */
void FormatTextBufferFromStart(const char *format, ...)
{
    pCurrentTextContext->textCursor =
        pCurrentTextContext->text;
#ifdef WC1_SDL
    {
        va_list arguments;

        va_start(arguments, format);
        FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                         format, &arguments);
        va_end(arguments);
    }
#else
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    if (pCurrentTextContext->viewport->pixels ==
        stScreen.pixels)
        DIBslam();
}

/* Function start: 0x413CB0 */
void AppendFormattedText(const char *format, ...)
{
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, &arguments);
    va_end(arguments);
#else
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    if (pCurrentTextContext->viewport->pixels ==
        stScreen.pixels)
        DIBslam();
}

/* Function start: 0x413CE0 */
void FatalErrorAndExit(const char *format, ...)
{
    char text[0xfc];

#ifdef WC1_SDL
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
#ifdef WC1_SDL
    const CockpitBarDefinition *definition;
#else
    int index;
#endif
    short direction;
    short left;
    short top;
    short right;
    short bottom;
    short length;
    short extent;
    signed char filledFrame;
    signed char emptyFrame;
    signed char swapFrame;

#ifdef WC1_SDL
    definition = &aaCockpitBars[
        (int)cCockpitView][(int)bar];
    length = definition->length;
#else
    index = (int)bar + (int)cCockpitView * 8;
    length = aaCockpitBars[0][index].length;
#endif
    extent = (short)(((int)percent * (int)length) / 100);
#ifdef WC1_SDL
    left = definition->left;
#else
    left = aaCockpitBars[0][index].left;
#endif
    stCockpitBar.left = left;
    if (left != -99) {
#ifdef WC1_SDL
        right = definition->right;
        top = definition->top;
        bottom = definition->bottom;
#else
        right = aaCockpitBars[0][index].right;
        top = aaCockpitBars[0][index].top;
        bottom = aaCockpitBars[0][index].bottom;
#endif
        stCockpitBar.right = right;
        stCockpitBar.top = top;
        stCockpitBar.bottom = bottom;
#ifdef WC1_SDL
        emptyFrame = (signed char)definition->emptyFrame;
        filledFrame = (signed char)definition->filledFrame;
        direction = definition->direction;
#else
        emptyFrame = (signed char)
            aaCockpitBars[0][index].emptyFrame;
        filledFrame = (signed char)
            aaCockpitBars[0][index].filledFrame;
        direction = aaCockpitBars[0][index].direction;
#endif

        if (direction < 2) {
            if (direction == 1) {
                extent = (short)(length - extent);
                swapFrame = filledFrame;
                filledFrame = emptyFrame;
                emptyFrame = swapFrame;
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
                extent = (short)(length - extent);
                swapFrame = filledFrame;
                filledFrame = emptyFrame;
                emptyFrame = swapFrame;
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
    CockpitReadout *readout;

    readout = &aCockpitReadouts[(int)slot];
    if (readout->x != -99) {
        SetTextContext(readout->context);
        SetTextCursor((unsigned short)readout->x,
                      (unsigned short)readout->y);
        DrawFormattedText(text);
        EraseCockpitReadoutRegion(
            &stScreen, readout->context->cursorX, readout->y,
            readout->previousRight,
            (short)(*(short *)readout->context->font + readout->y - 1),
            cBlackColour);
        readout->previousRight = readout->context->cursorX;
    }
}

/* Function start: 0x414050 */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top)
{
    CockpitReadout *readout;

    readout = &aCockpitReadouts[(int)slot];
    EraseCockpitReadoutRegion(
        &stScreen, left, top, readout->previousRight,
        (short)(*(short *)readout->context->font + readout->y),
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
    aHudMessageSlots[0].text = 0;
    aHudMessageSlots[0].flashCount = 0;
    aHudMessageSlots[1].text = 0;
    aHudMessageSlots[1].flashCount = 0;
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
    slot->text = text;
    slot->colour = colour;
    slot->drawColour = colour;
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
    SetHudMessageSlot(&aHudMessageSlots[1], &stRightVduTextContext,
                      stRightVdu.left,
                      (short)(stRightVdu.bottom - 6),
                      text, colour, (signed char)flashCount);
}

/* Function start: 0x4142B0 */
void CockpitMessage(const char *text, unsigned short colour,
                    int flashCount)
{
    if (text != aHudMessageSlots[1].text)
        set_global_message(text, colour, flashCount);
}

/* Function start: 0x4142E0 */
void remove_message(const char *text)
{
    ClearHudMessageIfMatching(&aHudMessageSlots[1], text);
}

/* Function start: 0x414300 */
short kilrathi_near(short obj, short range)
{
    short ship;

    ship = 0;
    do {
        if (aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
            aeShipSide[ship] == SIDE_KILRATHI &&
            IsPointWithinRange(&aShipPosition[obj],
                               &aShipPosition[ship],
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

    reason = 0;
    if (cMissionObjectiveCount == 0)
        return 0;
    if (distance_from_point(
            0,
            &aMissionObjectives[
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
    memset(abCockpitLightGoal, 0,
           sizeof(abCockpitLightGoal));
    memset(abCockpitLightState, 0,
           sizeof(abCockpitLightState));
    return abCockpitLightState;
}

/* Function start: 0x414440 */
unsigned int SetCockpitLightBlink(signed char light, short interval)
{
    if (interval < 20) {
        if (interval == 0) {
            abCockpitLightGoal[(int)light] ^= 1;
            return 0;
        }
        if (nSpaceFrame % interval == 0) {
            abCockpitLightGoal[(int)light] ^= 1;
            return 0;
        }
    } else {
        abCockpitLightGoal[(int)light] = 0;
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
            abCockpitLightGoal[4] = 1;
        else
            abCockpitLightGoal[4] = 0;
    }
    light = 0;
    do {
        if (bCockpitlessView == 0) {
            if (abCockpitLightState[(int)light] !=
                abCockpitLightGoal[(int)light]) {
                x = aasCockpitLightX[
                    (int)cCockpitView][(int)light];
                y = aasCockpitLightY[
                    (int)cCockpitView][(int)light];
                if (abCockpitLightGoal[(int)light] == 1)
                    frame = aacCockpitLightOnFrame[
                        (int)cCockpitView][(int)light];
                else
                    frame = aacCockpitLightOffFrame[
                        (int)cCockpitView][(int)light];
                DrawSpriteDefault(&stScreen, x, y,
                                  pCockpitDamageShape, frame);
                abCockpitLightState[(int)light] =
                    abCockpitLightGoal[(int)light];
            }
        } else {
            x = aasCockpitLightX[
                (int)cCockpitView][(int)light];
            y = aasCockpitLightY[
                (int)cCockpitView][(int)light];
            if (abCockpitLightGoal[(int)light] == 1)
                frame = aacCockpitLightOnFrame[
                    (int)cCockpitView][(int)light];
            else
                frame = aacCockpitLightOffFrame[
                    (int)cCockpitView][(int)light];
            DrawSpriteDefault(&stScreen, x, y,
                              pCockpitDamageShape, frame);
            abCockpitLightState[(int)light] =
                abCockpitLightGoal[(int)light];
        }
        light++;
    } while (light < 7);
}

/* Function start: 0x4145B0 */
void update_lights(void)
{
    short fuelPercent;

    fuelPercent = (short)(
        (anShipFuel[0] * 100) /
        *(int *)&aObjectTypeData[
            aeObjectType[0]].lifetime);
    SetCockpitLightBlink(6, fuelPercent);
    vdu_polygon(0, fuelPercent);
    vdu_polygon(1, asShipWeaponEnergy[0]);

    if (nTrainSimActive == 0) {
        if (calculate_damage_level() >= 3 &&
            (int)aasShipShield[0][1] +
                (int)aasShipShield[0][0] < 10) {
            SetCockpitLightBlink(3, 2);
            if (nDamageAlarmSfxHandle == 0 ||
                nSpaceFrame % 10 == 0)
                PlaySfxWaveFileByNumber(0x20, -1, 0);
        } else if (nDamageAlarmSfxHandle != 0) {
            ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
                nDamageAlarmSfxHandle, 1);
            nDamageAlarmSfxHandle = 0;
            abCockpitLightGoal[3] = 0;
        }
    }
}

/* Function start: 0x414690 */
void update_bars(void)
{
    ObjectTypeData *typeData;
    short forePercent;
    short aftPercent;

    typeData = &aObjectTypeData[aeObjectType[0]];
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
        4, _itoa((int)aasShipShield[0][0],
                 szTextScratchBuffer, 10));
    aftPercent = (short)((aasShipShield[0][1] * 100) /
                         typeData->shieldAft);
    SetCockpitLightBlink(1, aftPercent);
    vdu_polygon(7, aftPercent);
    DrawCockpitReadout(
        5, _itoa((int)aasShipShield[0][1],
                 szTextScratchBuffer, 10));
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
        ClearHudMessageSlot(&aHudMessageSlots[i]);
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
int GetVduModeStackDepth(short i)
{
    return acVduModeStackDepth[i];
}

/* Function start: 0x4148A0 */
void push_mode(short i, int state)
{
    ClearHudMessageSlot(&aHudMessageSlots[i]);
    acVduModeStackDepth[i]++;
    *(int *)&ausVduModeStack[
        ((int)acVduModeStackDepth[i] + i * 4) * 2] = state;
}

/* Function start: 0x4148E0 */
void pop_mode(short i)
{
    ClearHudMessageSlot(&aHudMessageSlots[i]);
    acVduModeStackDepth[i]--;
}

/* Function start: 0x414910 */
void set_new_vdu(short vdu)
{
    if (get_mode(vdu) == 0) {
        malf_noise(vdu, 1, cDarkGreenColour, 0x17, 0);
    } else {
        if (vdu == 1)
            ClearViewport(&stRightVdu, cBlackColour);
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
    nMessageTimer = 0;
}

/* Function start: 0x4149F0 */
short message_showing(void)
{
    return 0 < nMessageTimer;
}

/* Function start: 0x414A10 */
unsigned short set_message_time(unsigned short v)
{
    nMessageTimer = (short)v;
    return 0;
}

/* Function start: 0x414A20 */
void check_message(void)
{
    if (message_showing() &&
        (nMessageTimer = nMessageTimer - 1,
         nMessageTimer <= 0))
        EndCommMenu();
}

/* Function start: 0x414A50 */
unsigned int update_digital_readouts(void)
{
    long velocity;

    SetTextContext(&stCockpitReadoutTextContext);
    DrawCockpitReadout(
        2, _itoa((int)(short)((anShipSpeed[0] >> 8) * 10),
                 szTextScratchBuffer, 10));
    velocity = MultiplyFixed(
        Vector_magnitude(&aShipVelocity[0]), 0xa00);
    DrawCockpitReadout(
        3, _itoa((int)(short)(velocity >> 8),
                 szTextScratchBuffer, 10));
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
    int damage = acPlayerComponentDamage[(int)component];

    return (unsigned short)RandomInRange(0, 15) < damage * damage;
}

/* Function start: 0x414B20 */
unsigned short vdu_malf(short vdu, short sound)
{
    if (nCameraViewMode == 0)
        malf_noise(vdu, 1, cDarkGreenColour, sound, 0);
    set_mode(vdu, 0);
    return 0;
}

/* Function start: 0x414B70 */
void ShowComponentHitHudMessage(const char *text, unsigned short colour,
                                short flashCount)
{
    if (nTrainSimActive == 0 && get_mode(0) != 0) {
        if (aHudMessageSlots[0].text != 0)
            ClearHudMessageSlot(&aHudMessageSlots[0]);
        DosStrcpy(szComponentHitMessage, text);
        SetHudMessageSlot(&aHudMessageSlots[0],
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

    acPlayerComponentDamage[index] = (signed char)MinShort(
        (short)(acPlayerComponentDamage[index] + amount),
        (short)maximum);
    if (malf(component) != 0 && index == 3) {
        vdu_malf(0, 0x18);
        vdu_malf(1, 0x18);
    }
    if (get_mode(0) == 2 || get_mode(0) == 1) {
        sprintf(text, szComponentHitFormat,
                apszComponentNames[index]);
        ShowComponentHitHudMessage(text, (unsigned char)cRedColour, 5);
    }
    return acPlayerComponentDamage[index];
}

/* Function start: 0x414CB0 */
void RemovePlayerReleaseWeapon(signed char weapon)
{
    volatile ShipWeaponSlot *loadout;
    enum ObjectType preferredType;
    int hardpoint;

    loadout = (ShipWeaponSlot *)&aShipWeapons[0][1];
    preferredType = loadout[weapon].type;
    eReleaseWeaponDisplayType = preferredType;
    cReleaseWeaponDisplayFrame =
        (signed char)(preferredType * 2 - 0x2f);
    hardpoint = loadout[weapon].hardpoint;
    nReleaseWeaponDisplayX =
        (short)(aWeaponDisplayPositions[hardpoint].x +
                nWeaponDisplayOriginX);
    nReleaseWeaponDisplayY =
        (short)(aWeaponDisplayPositions[hardpoint].y +
                nWeaponDisplayOriginY);
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

    if (cReleaseWeaponDisplayFrame == -1)
        return;
    visible = nCameraViewMode == 0 && get_mode(0) == 1;
    if (cReleaseWeaponDisplayState != 0) {
        RestoreSpriteBackground(
            &stLeftVdu, pReleaseWeaponDisplayBackground,
            nReleaseWeaponDisplayX,
            nReleaseWeaponDisplayY,
            pCockpitWeaponShape,
            cReleaseWeaponDisplayFrame);
    }
    if (nReleaseWeaponDisplayY > stLeftVdu.top - 10 &&
        nReleaseWeaponDisplayY < stLeftVdu.bottom) {
        if (eReleaseWeaponDisplayType ==
            OBJECT_TYPE_SPACE_MINE)
            nReleaseWeaponDisplayY +=
                (short)cReleaseWeaponDisplayTicks;
        else
            nReleaseWeaponDisplayY -=
                (short)cReleaseWeaponDisplayTicks;
        cReleaseWeaponDisplayTicks++;
        if (visible != 0) {
            CaptureSpriteBackground(
                &stLeftVdu,
                pReleaseWeaponDisplayBackground,
                nReleaseWeaponDisplayX,
                nReleaseWeaponDisplayY,
                pCockpitWeaponShape,
                cReleaseWeaponDisplayFrame);
            DrawSpriteDefault(
                &stLeftVdu, nReleaseWeaponDisplayX,
                nReleaseWeaponDisplayY,
                pCockpitWeaponShape,
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
    ShipWeaponSlot *weapons;
    ShipWeaponSlot *selectedWeapon;
    const char *releaseName;
    const char *gunName;
    enum ObjectType selectedGunType;
    signed char count;

    selectedWeapon =
        &((ShipWeaponSlot *)&aShipWeapons[0][1])[
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

    releaseName = "";
    if (nSelectedReleaseWeaponIndex != -1)
        releaseName = aObjectTypeData[
            selectedWeapon->type].displayName;
    selectedGunType = eSelectedGunType;
    gunName = "";
    switch ((int)selectedGunType) {
    case -1:
        gunName = "";
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

    nWeaponDisplayOriginX =
        (short)(stLeftVdu.left +
                aWeaponDisplayOrigins[
                    (int)cCockpitView].x);
    nWeaponDisplayOriginY =
        (short)(stLeftVdu.top +
                aWeaponDisplayOrigins[
                    (int)cCockpitView].y);
    DrawSpriteDefault(&stLeftVdu, nWeaponDisplayOriginX,
                      nWeaponDisplayOriginY,
                      pCockpitWeaponShape, 0);
    weapons = (ShipWeaponSlot *)&aShipWeapons[0][1];
    count = 0;
    if ((signed char)aShipWeapons[0][0] > 0)
        do {
            count++;
            DrawSpriteDefault(
                &stLeftVdu,
                (short)(nWeaponDisplayOriginX +
                        aWeaponDisplayPositions[
                            weapons->hardpoint].x),
                (short)(nWeaponDisplayOriginY +
                        aWeaponDisplayPositions[
                            weapons->hardpoint].y),
                pCockpitWeaponShape,
                (short)((int)weapons->type * 2 +
                        weapons->disabled - 0x2f));
            weapons++;
        } while (count < (signed char)aShipWeapons[0][0]);
}

/* Function start: 0x415040 */
void update_status_text(void)
{
}

/* Function start: 0x415050 */
short sighted(short objective)
{
    return (aMissionObjectives[objective].flags & 4) == 4;
}

/* Function start: 0x415070 */
short visited(short objective)
{
    return (aMissionObjectives[objective].flags & 1) == 1;
}

/* Function start: 0x415090 */
short achieved(short objective)
{
    return (aMissionObjectives[objective].flags & 2) == 2;
}

/* Function start: 0x4150B0 */
void flag_objective(short objective, unsigned char flags)
{
    aMissionObjectives[objective].flags |= flags;
}

/* Function start: 0x4150D0 */
void DrawCalculatingLabel(void)
{
    if (nCurrentObjectiveRange <= 0) {
        DrawCockpitReadout(0, szCalculating);
    } else {
        strcpy(szTextScratchBuffer +
                   strlen(_ltoa((long)nCurrentObjectiveRange,
                                szTextScratchBuffer, 10)),
               " km");
        DrawCockpitReadout(0, szTextScratchBuffer);
    }
    nDisplayedObjectiveRange =
        nCurrentObjectiveRange;
}

/* Function start: 0x415130 */
const char *objective_name(short objective)
{
    if (objective >= (short)cMissionObjectiveCount)
        return szNoObjective;
    if (*aMissionObjectives[objective].name == '?' &&
        sighted(objective) == 0)
        return szUnknownObjective;
    return aMissionObjectives[objective].displayName;
}

/* Function start: 0x415180 */
void show_navigation_disp(void)
{
    DrawTextAt(&stRightVduTextContext,
               stRightVdu.left, stRightVdu.top,
               szCompNavigation, 2);
    DrawFormattedText(szDestinationFormat,
                      objective_name((short)cCurrentObjective));
    DrawFormattedText(szNavigationRangeLabel);
    InitializeCockpitReadout(0, &stRightVduTextContext);
    DrawFormattedText(szNewObjectivePrompt);
    DrawCalculatingLabel();
}

/* Function start: 0x4151F0 */
short hidden_objective(short objective)
{
    short nameHidden;
    short hidden;
    short ship;

    nameHidden =
        *aMissionObjectives[objective].displayName == '.' ||
        *aMissionObjectives[objective].name == '.';
    if (nameHidden != 0 ||
        (mobile_objective(objective) != 0 &&
         aMissionShips[
             (signed char)aMissionObjectives[
                 objective].index].state != 0))
        hidden = 1;
    else
        hidden = 0;
    if (hidden == 0 && mobile_objective(objective) != 0 &&
        aMissionObjectives[
            cMissionObjectiveCount].type == 0) {
        ship = find_ship_index(
            (short)aMissionObjectives[objective].index);
        if (aMissionShips[
                (signed char)aMissionObjectives[
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
        } while (abFlightPath[pathIndex] != -1 &&
                 pathIndex < (short)cMissionObjectiveCount);
        pathIndex--;
    }
    if (pathIndex > (short)cMissionObjectiveCount)
        pathIndex = 0;
    if (abFlightPath[pathIndex] == -1)
        pathIndex = 0;
    cCurrentNavPointIndex = (signed char)pathIndex;
    if (hidden_objective(
            (short)abFlightPath[pathIndex]) != 0)
        return 0;
    cCurrentObjective =
        abFlightPath[cCurrentNavPointIndex];
    aeShipObjective[0] =
        (enum ShipObjective)aMissionObjectives[
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
                (short)((short)cCurrentNavPointIndex + 1)) != 0)
            break;
        if (cCurrentNavPointIndex == 0)
            wraps++;
    } while (wraps < 3);
    if (wraps >= 3) {
        cCurrentNavPointIndex = 0;
        cCurrentObjective = abFlightPath[0];
    }
    return wraps < 3;
}

/* Function start: 0x4153D0 */
void set_next_destination(void)
{
    set_new_objective(0);
    do {
        if (set_new_objective(
                (short)cCurrentNavPointIndex) != 0 &&
            visited((short)abFlightPath[
                cCurrentNavPointIndex]) == 0)
            break;
        cCurrentNavPointIndex++;
    } while (cCurrentNavPointIndex <
                 cMissionObjectiveCount &&
             abFlightPath[
                 cCurrentNavPointIndex] != -1);
    if (cCurrentNavPointIndex >=
            cMissionObjectiveCount ||
        abFlightPath[
            cCurrentNavPointIndex] == -1) {
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
            aMissionObjectives[objective].index,
            &aMissionObjectives[objective].position);
    }
    return ship;
}

/* Function start: 0x4154C0 */
unsigned int someone_coming(void)
{
    short ship = 0;

    do {
        if (aeObjectClass[ship] >= OBJECT_CLASS_SHIP &&
            aeShipMissionType[ship] == MISSION_TYPE_COME_HOME &&
            abShipNavPointIndex[ship] <= cCurrentNavPointIndex)
            return 1;
        ship = ship + 1;
    } while (ship < 10);
    return 0;
}

/* Function start: 0x415510 */
unsigned int escorting_a_ship(void)
{
    if (aeShipMissionType[0] != MISSION_TYPE_ESCORT) {
        if (someone_coming() == 0)
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

    carrierMissionShip = anShipMissionShip[0];
    objectiveType = (short)aMissionObjectives[objective].type;
    carrierObject = find_ship_index(carrierMissionShip);
    markVisited = objective != cCurrentObjective;
    advanceDestination = 0;
    if (objective == cCurrentObjective) {
        if (reached == 0 && escorting_a_ship() != 0 &&
            carrierObject != -1 &&
            aMissionObjectives[objective].index !=
                anShipMissionShip[0]) {
            if (objectiveType != 1 ||
                aMissionShips[carrierMissionShip].state != 1) {
                sprintf(pszObjectiveStatusMessage,
                        szWaitForFormat,
                        aObjectTypeData[
                            aeObjectType[carrierObject]].
                                displayName);
                CockpitMessage(pszObjectiveStatusMessage,
                               cYellowColour, 4);
            }
        } else {
            advanceDestination = 1;
            CockpitMessage(
                visited(objective) != 0
                    ? (char *)szAlreadyVisited
                    : (char *)szObjectiveReached,
                cYellowColour, 4);
            markVisited = advanceDestination;
        }
    }
    if (objectiveType != 1 && markVisited != 0) {
        if (visited(objective) == 0 && carrierObject != -1 &&
            aMissionObjectives[objective].index ==
                anShipMissionShip[0] &&
            aeObjectType[carrierObject] !=
                OBJECT_TYPE_TIGERS_CLAW)
            send_message(carrierObject, 6);
        flag_objective(objective, 1);
    }
    if (advanceDestination != 0)
        set_next_destination();
}

/* Function start: 0x4156D0 */
void check_sighting(short objective, short range, short object)
{
    if (sighted(objective) == 0 && range < 16000 &&
        (object == -1 ||
         asObjectScreenX[object] != (short)0x8001))
        flag_objective(objective, 4);
}

/* Function start: 0x415720 */
void check_visit(short objective, short range)
{
    int reachedRange;

    if ((short)aMissionObjectives[objective].type == 3 ||
        (short)aMissionObjectives[objective].type == 4)
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
    short object;
    short range;

    object = LocateMobileObjective(objective);
    if (sighted(objective) != 0 && visited(objective) != 0 &&
        cCurrentObjective != objective)
        return;
    ComputeVectorDelta(&aShipPosition[0],
                       &aMissionObjectives[objective].position,
                       &delta);
    range = FixedToShortSaturating(
        Vector_magnitude(&delta));
    check_sighting(objective, range, object);
    if (mobile_objective(objective) != 0) {
        if (object != -1)
            check_visit(objective, range);
    } else if (aMissionNavPoints[
                   aMissionObjectives[objective].index].type >= 1) {
        check_visit(objective, range);
    }
}

/* Function start: 0x415850 */
unsigned int objective_lost(short objective)
{
    int objectiveIndex;
    unsigned short state;
    int objectiveType;

    objectiveIndex = (int)objective;
    state = aMissionShips[
        aMissionObjectives[objectiveIndex].index].state;
    objectiveType = aMissionObjectives[objectiveIndex].type;
    switch (objectiveType) {
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
        nCurrentObjectiveRange)
        DrawCalculatingLabel();
}

/* Function start: 0x4158F0 */
void rotational_pos_to_scanner_pos(signed char object,
                                   const SphericalVector *position)
{
    short horizontal;

    horizontal = position->yaw;
    if (abs((int)horizontal) < 45)
        nScannerCursorX = (short)(
            stCockpitLayout.scanner[
                (int)cCockpitView].centerX + horizontal / 4);
    else
        nScannerCursorX = (short)(
            stCockpitLayout.scanner[
                (int)cCockpitView].centerX + horizontal / 6);

    nScannerCursorY = (short)(
        stCockpitLayout.scanner[
            (int)cCockpitView].centerY + position->pitch / -3);
    nScannerCursorX =
        MinShort(stCockpitLayout.scanner[
                     (int)cCockpitView].maximumX,
                 nScannerCursorX);
    nScannerCursorX =
        MaxShort(stCockpitLayout.scanner[
                     (int)cCockpitView].minimumX,
                 nScannerCursorX);
    nScannerCursorY =
        MinShort(stCockpitLayout.scanner[
                     (int)cCockpitView].maximumY,
                 nScannerCursorY);
    nScannerCursorY =
        MaxShort(stCockpitLayout.scanner[
                     (int)cCockpitView].minimumY,
                 nScannerCursorY);
    asScannerObjectX[(int)object] =
        nScannerCursorX;
    asScannerObjectY[(int)object] =
        nScannerCursorY;
}

/* Function start: 0x415A30 */
short mobile_objective(short objective)
{
    int type = aMissionObjectives[objective].type;

    if (type == 1 || type == 3 || type == 4 || type == 2)
        return 1;
    return 0;
}

/* Function start: 0x415A70 */
void ResetScannerContacts(void)
{
    short i = 10;

    do {
        asScannerObjectX[i] = 0;
        i = i - 1;
    } while (i != 0);
    nScannerTargetObject = -1;
}

/* Function start: 0x415A90 */
void clear_head_up_display(void)
{
    short object;

    if (nScannerTargetObject != -1) {
        object = nScannerTargetObject;
        DrawViewportPixel(&stScreen,
                          asScannerObjectX[object],
                          asScannerObjectY[object],
                          asScannerBackgroundColour[object]);
        asScannerObjectX[
            nScannerTargetObject] = 0;
    }
    if (asScannerObjectX[10] != 0) {
        RestoreSpriteBackground(&stScreen,
                                pScannerMarkerBackground,
                                asScannerObjectX[10],
                                asScannerObjectY[10],
                                pTargetLockShape, 2);
        asScannerObjectX[10] = 0;
    }
    object = 9;
    do {
        if (asScannerObjectX[object] != 0) {
            DrawViewportPixel(&stScreen,
                              asScannerObjectX[object],
                              asScannerObjectY[object],
                              asScannerBackgroundColour[object]);
            asScannerObjectX[object] = 0;
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
    ComputeVectorDelta(&aShipPosition[0],
                       &aMissionObjectives[
                           (short)cCurrentObjective].position,
                       &relative);
    transform_to_objects_frame(&relative, &rotated, 0);
    rectangular_to_spherical(&rotated, &spherical);
    if (showOnScanner != 0)
        rotational_pos_to_scanner_pos(10, &spherical);
    nCurrentObjectiveRange = spherical.radius >> 8;
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
        else if (aeObjectType[object] ==
                 OBJECT_TYPE_TIGERS_CLAW)
            *colour = cViewportClearColour;
        else
            *colour = cDarkGreyColour;
        return 1;
    }
    if (acShipTarget[object] == 0) {
        *colour = cYellowColour;
        return 1;
    }
    return 0;
}

/* Function start: 0x415CE0 */
unsigned int draw_3d_scanner(void)
{
    const int *grid;
    const CockpitScannerGeometry *scanner;
    SphericalVector spherical;
    unsigned short colour;
    short object;
    short row;

    if (bCockpitlessView != 0 && aiScannerGridRows[0] != -2) {
        scanner = &stCockpitLayout.scanner[
            (int)cCockpitView];
        row = 0;
        grid = aiScannerGridRows;
        do {
            if (*grid == -1) {
                row++;
            } else {
                DrawViewportPixel(&stScreen,
                                  (short)(scanner->centerX + row),
                                  (short)(scanner->centerY + *grid), 0xaa);
                if (*grid != 0)
                    DrawViewportPixel(&stScreen,
                                      (short)(scanner->centerX + row),
                                      (short)(scanner->centerY - *grid), 0xaa);
                if (row != 0) {
                    DrawViewportPixel(&stScreen,
                                      (short)(scanner->centerX - row),
                                      (short)(scanner->centerY + *grid), 0xaa);
                    if (*grid != 0)
                        DrawViewportPixel(&stScreen,
                                          (short)(scanner->centerX - row),
                                          (short)(scanner->centerY - *grid),
                                          0xaa);
                }
            }
            grid++;
        } while (*grid != -2);
    }

    clear_head_up_display();
    nScannerTargetObject = acShipTarget[0];
    if (nScannerTargetObject != -1 &&
        aeObjectClass[nScannerTargetObject] <
            OBJECT_CLASS_SHIP)
        nScannerTargetObject = -1;

    for (object = 1; object < 10; object++) {
        if (get_color(object, &colour) != 0) {
            rectangular_to_spherical(&aObjectViewPosition[object],
                                     &spherical);
            if (spherical.radius < 0xea6000) {
                rotational_pos_to_scanner_pos((signed char)object,
                                               &spherical);
                asScannerBackgroundColour[object] =
                    (short)GetViewportPixel(
                        &stScreen, nScannerCursorX,
                        nScannerCursorY);
                if (nScannerTargetObject != object)
                    DrawViewportPixel(&stScreen,
                                      nScannerCursorX,
                                      nScannerCursorY,
                                      colour);
            }
        }
    }

    if (get_mode(1) == 5) {
        set_objective_range(1);
        CaptureSpriteBackground(&stScreen,
                                pScannerMarkerBackground,
                                nScannerCursorX,
                                nScannerCursorY,
                                pTargetLockShape, 2);
        DrawSpriteDefault(&stScreen,
                          nScannerCursorX,
                          nScannerCursorY,
                          pTargetLockShape, 2);
    }

    if (nScannerTargetObject != -1 &&
        get_color(nScannerTargetObject, &colour) != 0) {
        if ((abs((int)nSpaceFrame) & 1) == 0)
            colour = cBlackColour;
        object = nScannerTargetObject;
        DrawViewportPixel(&stScreen,
                          asScannerObjectX[object],
                          asScannerObjectY[object], colour);
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
    remove_message(PTR_s_MISSILE_LOCKED_004691d4);
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
                CockpitMessage(PTR_s_MISSILE_LOCKED_004691d4,
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

    if (target != -1 &&
        aeShipSide[(short)target] != aeShipSide[0] &&
        acPlayerComponentDamage[5] < 4) {
        x = asObjectScreenX[(short)target];
        if (x == -0x7fff)
            return;
        y = asObjectScreenY[(short)target];
        if (nTargetLockCountdown < -1) {
            nTargetLockCountdown++;
            return;
        }
        if (x * x + y * y > 0xe10) {
            lock_off();
            return;
        }
#ifdef WC1_SDL
        /* With the -1 sentinel, the original reads the zero-filled word at
           0x0059CAAA just before the weapon table and then turns locking off.
           Native globals have sanitizer redzones, so make that result explicit. */
        if (nSelectedReleaseWeaponIndex == -1) {
            lock_off();
            return;
        }
#endif
        weaponType = *(enum ObjectType *)(
            &aShipWeapons[0][1] +
            nSelectedReleaseWeaponIndex * 7);
        if (weaponType != OBJECT_TYPE_HEAT_SEEKING_MISSILE) {
            if (weaponType != OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE) {
                lock_off();
                return;
            }
            goto image_recognition_lock;
        }

        get_facing_range_from_object(0, (short)target);
        if (nTargetFacing > -0x41) {
            lock_off();
            return;
        }
        if (starting_lock(0x12) == 0)
            decrement_lock_time(x);
        return;

image_recognition_lock:
        if (starting_lock(0x20) != 0)
            return;
        decrement_lock_time(x);
        return;
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

/* Function start: 0x416260 */
void print_message_text(char *text, unsigned short colour)
{
    TextContext context;
    Viewport viewport;
    char wrapped[84];
    char source[84];
    char *input;
    char *output;
    int charactersPerLine;
    short lastSpace = -1;
    short position;
    short width;

    if (text == 0)
        return;

    DosStrcpy(source, text);
    wrapped[0] = (char)cEmptyHudMessage;
    viewport = *stHudMessageTextContext.viewport;
    context = stHudMessageTextContext;
    context.viewport = &viewport;

    SetRectBounds(&viewport,
                  (unsigned short)asHudMessageOrigins[
                      (int)cCockpitView * 2],
                  (unsigned short)asHudMessageOrigins[
                      (int)cCockpitView * 2 + 1],
                  (unsigned short)(
                      319 - asHudMessageOrigins[
                                (int)cCockpitView * 2]),
                  (unsigned short)(
                      asHudMessageOrigins[
                          (int)cCockpitView * 2 + 1] + 60));
    context.backgroundColour = 0xff;
    context.colour = colour;
    input = source;
    output = wrapped;
    width = GetRectHeight(&viewport);
    charactersPerLine = (short)(width / 6);
    position = 0;
    if (*input != 0) {
        for (; *input != 0; position++, input++, output++) {
            *output = *input;
            if (*input == ' ')
                lastSpace = position;
            if ((position + 1) % charactersPerLine == 0) {
                if (lastSpace == -1) {
                    output[1] = '\n';
                    output++;
                } else {
                    output[position - lastSpace] = '\n';
                }
            }
        }
    }
    *output = 0;

    {
        int view;

        view = (int)cCockpitView;
        if (bCockpitlessView != 0) {
            switch (view) {
            case 0:
                DrawTextAt(&context, asHudMessageOrigins[view * 2],
                           (short)(asHudMessageOrigins[view * 2 + 1] + 10),
                           wrapped, 2);
                break;
            case 1:
                DrawTextAt(&context, asHudMessageOrigins[view * 2],
                           (short)(asHudMessageOrigins[view * 2 + 1] + 25),
                           wrapped, 2);
                break;
            case 2:
                DrawTextAt(&context, asHudMessageOrigins[view * 2],
                           (short)(asHudMessageOrigins[view * 2 + 1] + 50),
                           wrapped, 2);
                break;
            case 3:
                DrawTextAt(&context, asHudMessageOrigins[view * 2],
                           asHudMessageOrigins[view * 2 + 1], wrapped, 2);
                break;
            default:
                break;
            }
        } else {
            DrawTextAt(&context, asHudMessageOrigins[view * 2],
                       asHudMessageOrigins[view * 2 + 1], wrapped, 2);
        }
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
    print_message_text(pszDisplayedHudMessage, (unsigned char)cPrimaryViewBufferColour);
}

/* Function start: 0x4164B0 */
void draw_target_box(unsigned short colour, signed char object,
                     short solid, short drawLockMarker, short padding,
                     ShortRect *savedBounds)
{
    short centerY;
    ShortRect bounds;
    int colourValue;
    short centerX;
    short segmentLength;
    short valid;

    colourValue = (short)colour;
    if ((int)(unsigned char)cPrimaryViewBufferColour == colourValue) {
        valid = savedBounds->left != -0x7fff;
        bounds = *savedBounds;
    } else {
        if (object == -1) {
            valid = 0;
        } else {
            valid = 1;
            if (asObjectScreenX[object] == -0x7fff)
                valid = 0;
        }
        if (valid != 0) {
            centerX = (short)(asObjectScreenX[object] +
                              nViewCenterX);
            centerY = (short)(asObjectScreenY[object] +
                              nViewCenterY);
            if ((short)GetTransformedShapeBounds(
                    &stSpaceBuffer, centerX, centerY,
                    apObjectShape[object],
                    asObjectViewFrame[object],
                    asObjectScreenAngle[object],
                    asObjectScreenScale[object],
                    asObjectFlip[object],
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
            DrawViewportBorder(&stSpaceBuffer, bounds.left, bounds.top,
                               bounds.right, bounds.bottom, colour);
        } else {
            segmentLength =
                (short)(((int)bounds.right - bounds.left) / 6 + 1);
            DrawViewportLine(&stSpaceBuffer, bounds.left, bounds.top,
                             (short)(bounds.left + segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&stSpaceBuffer, bounds.left, bounds.bottom,
                             (short)(bounds.left + segmentLength), bounds.bottom,
                             colour);
            DrawViewportLine(&stSpaceBuffer, bounds.right, bounds.top,
                             (short)(bounds.right - segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&stSpaceBuffer, bounds.right, bounds.bottom,
                             (short)(bounds.right - segmentLength), bounds.bottom,
                             colour);
            segmentLength =
                (short)(((int)bounds.bottom - bounds.top) / 6 + 1);
            DrawViewportLine(&stSpaceBuffer, bounds.left, bounds.top,
                             bounds.left, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&stSpaceBuffer, bounds.left, bounds.bottom,
                             bounds.left,
                             (short)(bounds.bottom - segmentLength), colour);
            DrawViewportLine(&stSpaceBuffer, bounds.right, bounds.top,
                             bounds.right, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&stSpaceBuffer, bounds.right, bounds.bottom,
                             bounds.right,
                             (short)(bounds.bottom - segmentLength), colour);
        }
        if (drawLockMarker != 0) {
            if ((int)(short)colour != (int)(unsigned char)cPrimaryViewBufferColour) {
                if (nTargetLockCountdown > -1) {
                    nTargetLockMarkerAngle = (short)(
                        nTargetLockMarkerAngle +
                        anObjectRollRotation[0] +
                        anObjectPitchRotation[0]);
                    centerX = (short)(centerX +
                        ((CosFixed(nTargetLockMarkerAngle) *
                          nTargetLockCountdown * 2) >> 8));
                    centerY = (short)(centerY +
                        ((SinFixed(nTargetLockMarkerAngle) *
                          nTargetLockCountdown * 2) >> 8));
                    DrawSpriteDefault(&stSpaceBuffer, centerX, centerY,
                                      pTargetLockShape, 1);
                    nTargetLockMarkerX = centerX;
                    nTargetLockMarkerY = centerY;
                }
            } else if (nTargetLockMarkerX != -0x7fff) {
                DrawSolidColourSprite(&stSpaceBuffer,
                                      nTargetLockMarkerX,
                                      nTargetLockMarkerY,
                                      pTargetLockShape, 1,
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
        asObjectViewFrame[object] = 3;
        acObjectOwner[object] = -1;
        asObjectScreenAngle[object] = 0;
        asObjectScreenScale[object] = 0x100;
        aeObjectClass[object] = OBJECT_CLASS_PLANET;
        apObjectShape[object] =
            pTargetLockShape;
        nNavPointerObject = object;
        asObjectScreenX[object] = (short)0x8001;
        asObjectDistance[object] = 0;
    }
    objectivePosition = aMissionObjectives[
        (signed char)cCurrentObjective].position;
    ComputeVectorDelta(&aShipPosition[WC1_EYE_OBJECT],
                       &objectivePosition, &direction);
    distance = Vector_magnitude(&direction);
    if (asObjectCollisionRadius[WC1_EYE_OBJECT] * 0x100 >=
        distance)
        return;
    transform_to_objects_frame(&direction, &viewPosition,
                               WC1_EYE_OBJECT);
    if (asObjectCollisionRadius[WC1_EYE_OBJECT] * 0x100 >
        viewPosition.z)
        return;
    if (DivideFixed(viewPosition.z, distance) < 0x94)
        return;
    asObjectScreenX[object] = (short)(DivideFixed(
        MultiplyFixed(
            ((short)nScreenWidth & ~1) << 7,
            viewPosition.x),
        viewPosition.z) >> 8);
    asObjectScreenY[object] = (short)(DivideFixed(
        MultiplyFixed(
            ((short)nScreenWidth & ~1) << 7,
            viewPosition.y),
        viewPosition.z) >> 8);
    asObjectDistance[object] = 0x4a38;
}

/* Function start: 0x416AC0 */
unsigned int overlay_head_up_display(void)
{
    target_locking(acShipTarget[0]);
    if (message_showing() && nCommSpeakerObject != -1) {
        cPreviousTargetObject =
            (signed char)nCommSpeakerObject;
        draw_target_box(cYellowColour,
                        cPreviousTargetObject,
                        0, 0, 2,
                        &stPreviousTargetBracketBounds);
    }
    if (nTargetLockCountdown == 0) {
        if ((short)(nRenderedSpaceFrame % 2) == 0)
            bTargetBracketVisible ^= 1;
        if (bTargetBracketVisible == 1) {
            draw_target_box(cRedColour,
                            acShipTarget[0],
                            nTargetLockMode,
                            1, 1,
                            &stTargetBracketBounds);
        }
    } else {
        draw_target_box(cRedColour,
                        acShipTarget[0],
                        nTargetLockMode,
                        1, 1,
                        &stTargetBracketBounds);
    }

    if (bCockpitlessView != 0) {
        switch (cCockpitView) {
        case 0:
        case 2:
            goto centered_sight;
        case 1:
            DrawSpriteDefault(&stSpaceBuffer,
                              nViewCenterX,
                              (short)(nViewCenterY - 1),
                              pTargetLockShape, 0);
            break;
        case 3:
            DrawSpriteDefault(&stSpaceBuffer,
                              nViewCenterX,
                              (short)(nViewCenterY + 14),
                              pTargetLockShape, 0);
            break;
        default:
            goto no_sight;
        }
        goto no_sight;
    }
centered_sight:
    DrawSpriteDefault(&stSpaceBuffer,
                      nViewCenterX,
                      nViewCenterY,
                      pTargetLockShape, 0);

no_sight:
    DAT_0046c05c = 0;
    if (pszPendingHudMessage != 0)
        ShowHudTextLine(pszPendingHudMessage, (unsigned char)DAT_005a7f00);
    if (bMouseCursorVisible == 1) {
        nSavedMouseCursorX = stMouseCursorState.x;
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
    if (bMouseCursorVisible == 1) {
        RestoreSpriteBackground(stMouseCursorState.viewport,
                                abMouseCursorBackground,
                                (short)nSavedMouseCursorX,
                                (short)nSavedMouseCursorY,
                                stMouseCursorState.shape,
                                (short)stMouseCursorState.frame);
    }
    if (cPreviousTargetObject != -1) {
        draw_target_box(cPrimaryViewBufferColour,
                        cPreviousTargetObject,
                        0, 0, 2,
                        &stPreviousTargetBracketBounds);
        cPreviousTargetObject = -1;
    }
    draw_target_box(cPrimaryViewBufferColour, acShipTarget[0],
                    nTargetLockMode, 1, 1,
                    &stTargetBracketBounds);
    if (pszDisplayedHudMessage != pszPendingHudMessage && pszDisplayedHudMessage != 0)
        SetHudTextColour(0);
    if (IsCockpitExplosionActive() &&
        pCockpitExplosionBackground != 0) {
        if (bCockpitlessView == 0) {
            CaptureSpriteBackground(
                &stScreen, pCockpitExplosionBackground,
                stCockpitExplosionPosition.x,
                stCockpitExplosionPosition.y,
                pCockpitExplosionShape,
                nCockpitExplosionFrame);
        }
        if (bCockpitlessView == 0) {
            DrawSpriteDefault(&stScreen,
                              stCockpitExplosionPosition.x,
                              stCockpitExplosionPosition.y,
                              pCockpitExplosionShape,
                              nCockpitExplosionFrame);
        }
        cRenderedPilotHandFrame = 0xff;
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
        DAT_005a7f00 = colour;
        pszPendingHudMessage = text;
        set_message_time(duration);
    }
}

/* Function start: 0x416E20 */
void malf_noise(short vdu, int effect, unsigned short colour,
                short sound, short refresh)
{
    Viewport *viewport = vdu == 0 ? &stLeftVdu : &stRightVdu;

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
    signed char object;
    signed char targetIndex;

    *hasEnemy = 0;
    cViableTargetCount = 0;
    object = 1;
    do {
        if (aeObjectClass[(int)object] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuver[(int)object] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            asObjectScreenX[(int)object] != (short)0x8001 &&
            (unsigned short)asObjectDistance[(int)object] <
                12000) {
            targetIndex = cViableTargetCount;
            asViableTargetDistance[(int)targetIndex] =
                asObjectDistance[(int)object];
            acViableTarget[(int)targetIndex] = object;
            cViableTargetCount++;
            if (aeShipSide[(int)object] !=
                aeShipSide[0])
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
    signed char previousTarget;
    signed char index;
    short hasEnemy;

    previousTarget = acShipTarget[0];
    build_your_target_list(&hasEnemy);
    if (cViableTargetCount == 0) {
        acShipTarget[0] = -1;
    } else {
        for (index = 0;
             index < cViableTargetCount &&
             acViableTarget[(int)index] !=
                 acShipTarget[0];
             index++)
            ;
        do {
            index = (signed char)((index + 1) %
                                  cViableTargetCount);
            acShipTarget[0] =
                acViableTarget[(int)index];
            if (hasEnemy == 0)
                break;
        } while (aeShipSide[
                     acShipTarget[0]] ==
                 aeShipSide[0]);
    }
    if (acShipTarget[0] != previousTarget)
        nTargetLockCountdown = -1;
}

/* Function start: 0x416FD0 */
void check_target(void)
{
    short selectNewTarget;
    short oldTarget;
    short targetIndex;
    short hasEnemy;

    selectNewTarget = 1;
    oldTarget = acShipTarget[0];
    if (oldTarget != -1 &&
        aeSpecialManeuver[oldTarget] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        acShipTarget[0] = -1;
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
         (asObjectScreenX[oldTarget] != (short)0x8001 &&
          (nTargetLockMode != 0 ||
           aeShipSide[oldTarget] !=
               aeShipSide[0]))))
        return;
    if (oldTarget == -1)
        nTargetLockMode = 0;

    build_your_target_list(&hasEnemy);
    if (cViableTargetCount == 0) {
        if (nTargetLockMode != 0)
            acShipTarget[0] = oldTarget;
        else
            acShipTarget[0] = -1;
    } else {
        if (hasEnemy == 0 && oldTarget != -1 &&
            aeShipSide[oldTarget] ==
                aeShipSide[0] &&
            (nTargetLockMode != 0 ||
             asObjectScreenX[oldTarget] != (short)0x8001)) {
            selectNewTarget = 0;
            acShipTarget[0] = oldTarget;
        }
        if (selectNewTarget != 0) {
            for (targetIndex = 0;
                 targetIndex < cViableTargetCount &&
                 aeShipSide[
                     (short)acViableTarget[targetIndex]] ==
                     aeShipSide[0];
                 targetIndex++)
                ;
            acShipTarget[0] =
                acViableTarget[
                    targetIndex % (short)cViableTargetCount];
        }
    }
    if (acShipTarget[0] != oldTarget) {
        if (oldTarget != -1 && acShipTarget[0] == -1)
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
        abCockpitLightGoal[2] = 0;
    }
}

/* Function start: 0x4171D0 */
void determine_pilot_hand(void)
{
    short yaw;
    short pitch;

    yaw = nYawInput / 2;
    pitch = nPitchInput / 2;
    if (yaw > 0) {
        bStickIndicatorFrame =
            (unsigned char)MinShort((short)(yaw + 8), 12);
        return;
    }
    if (yaw < 0) {
        bStickIndicatorFrame =
            (unsigned char)MinShort((short)(4 - yaw), 8);
        return;
    }
    if (pitch > 0) {
        bStickIndicatorFrame =
            (unsigned char)MinShort((short)(pitch + 12), 16);
        return;
    }
    if (pitch < 0) {
        bStickIndicatorFrame =
            (unsigned char)MinShort((short)-pitch, 4);
        return;
    }
    bStickIndicatorFrame = 0;
}

/* Function start: 0x417260 */
void DrawPilotHandFrame(void)
{
    int view = (int)cCockpitView;
    short x = (short)(asPilotHandOrigins[view * 2] -
                      stPilotHand.left);
    short y = (short)(asPilotHandOrigins[view * 2 + 1] -
                      stPilotHand.top);

    CopyViewportContents(&stPilotHandBackdrop,
                         &stPilotHandComposite);
    DrawSpriteDefault(&stPilotHandComposite, x, y, pPilotHandShape,
                      (short)(signed char)bStickIndicatorFrame);
    DrawSpriteDefault(
        &stPilotHandComposite,
        (short)(x + asPilotHandOffsets[
            (signed char)bStickIndicatorFrame * 2]),
        (short)(y + asPilotHandOffsets[
            (signed char)bStickIndicatorFrame * 2 + 1]),
        pPilotHandShape, 0x11);
    CopyViewportContents(&stPilotHandComposite, &stPilotHand);
    cRenderedPilotHandFrame = bStickIndicatorFrame;
}

/* Function start: 0x417320 */
void CopyTrainSimPilotViewToRightVdu(void)
{
    Viewport destination;

    destination = stPilotHandBackdrop;
    if (stTrainSimVduSource.left == 0) {
        stTrainSimVduSource = stPilotHand;
        stTrainSimVduSource.left = stRightVdu.left;
        stTrainSimVduSource.top = stPilotHand.top;
        stTrainSimVduSource.right = stPilotHand.right;
        stTrainSimVduSource.bottom = stRightVdu.bottom;
    }
    destination.left = (short)(stTrainSimVduSource.left -
                               stPilotHand.left);
    destination.bottom = (short)(stTrainSimVduSource.bottom -
                                 stPilotHand.top);
    CopyViewportContents(&stTrainSimVduSource, &destination);
}

/* Function start: 0x4173C0 */
void animate_pilot(void)
{
    if (pPilotHandShape != 0) {
        determine_pilot_hand();
        if (cRenderedPilotHandFrame != bStickIndicatorFrame)
            DrawPilotHandFrame();
    }
}

/* Function start: 0x4173F0 */
void ResetPilotHandAnimation(void)
{
    if (pPilotHandShape != 0) {
        cRenderedPilotHandFrame = 0xff;
        CopyViewportContents(&stPilotHand, &stPilotHandBackdrop);
        animate_pilot();
    }
}

/* Function start: 0x417420 */
void send_message(short obj, signed char message)
{
    if (nTrainSimActive == 0 &&
        aeObjectClass[obj] != OBJECT_CLASS_NULL &&
        nCannedSceneMode == 0) {
        if (nYourWingman != -1 &&
            nYourWingman == obj &&
            bRadioSilence != 0) {
            acWingmanMessageState[obj] = -1;
            return;
        }
        if (obj >= 0 && obj < 10 &&
            aeObjectClass[obj] >= OBJECT_CLASS_SHIP) {
            if (acShipRating[obj] != -1) {
                acWingmanMessageState[obj] = message;
                return;
            }
            if (aeObjectType[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW ||
                nShipMissionIndices[obj] ==
                    anShipMissionShip[0]) {
                acWingmanMessageState[obj] = message;
            } else if (aeShipSide[obj] == SIDE_KILRATHI) {
                acWingmanMessageState[obj] = message;
                return;
            }
        }
    }
}

/* Function start: 0x4174F0 */
void npc_communication(void)
{
    signed char message;
    signed char obj;
    short messageActive;

    if (nCannedSceneMode == 0 &&
        nTrainSimActive == 0) {
        messageActive = message_showing();
        obj = 1;
        while (messageActive == 0 && obj < 10) {
            if (aeObjectClass[(short)obj] >=
                    OBJECT_CLASS_SHIP &&
                acWingmanMessageState[(short)obj] != -1) {
                message = acWingmanMessageState[(short)obj];
                vid_equiv((short)obj, (short)message);
                acWingmanMessageState[(short)obj] = -1;
            }
            messageActive = message_showing();
            obj++;
        }
        if (RandomBelowOrEqual(5000) > 4998 &&
            nCommSpeakerObject == -1) {
            obj = 1;
            while (obj < 10) {
                if (aeObjectClass[(short)obj] >=
                        OBJECT_CLASS_SHIP &&
                    aeShipSide[(short)obj] == SIDE_KILRATHI &&
                    (aeShipObjective[(short)obj] ==
                         OBJECTIVE_ENGAGE_ENEMY ||
                     aeShipObjective[(short)obj] ==
                         OBJECTIVE_DESTROY_SHIP) &&
                    (acShipRating[(short)obj] != -1 ||
                     RandomBelowOrEqual(100) < 20)) {
                    acWingmanMessageState[(short)obj] =
                        (signed char)(RandomBelowOrEqual(2) + 2);
                    return;
                }
                obj++;
                if (nCommSpeakerObject != -1)
                    return;
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
                &stScreen,
                aaCockpitDamagePositions[
                    (int)cCockpitView][damage].x,
                aaCockpitDamagePositions[
                    (int)cCockpitView][damage].y,
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
    DrawSpriteDefault(&stScreen,
                      stCockpitExplosionPosition.x,
                      stCockpitExplosionPosition.y,
                      pCockpitPilotShape,
                      nPendingCockpitDamage);
    if (pPilotHandShape != 0) {
        DrawSpriteDefault(&stPilotHandBackdrop,
                          (short)(stCockpitExplosionPosition.x -
                                  stPilotHand.left),
                          (short)(stCockpitExplosionPosition.y -
                                  stPilotHand.top),
                          pCockpitPilotShape,
                          nPendingCockpitDamage);
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
            &stScreen, pCockpitExplosionBackground,
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
                &stScreen, pCockpitExplosionBackground,
                stCockpitExplosionPosition.x,
                stCockpitExplosionPosition.y,
                pCockpitExplosionShape,
                nCockpitExplosionFrame);
            DrawSpriteDefault(&stScreen,
                              stCockpitExplosionPosition.x,
                              stCockpitExplosionPosition.y,
                              pCockpitExplosionShape,
                              nCockpitExplosionFrame);
            cRenderedPilotHandFrame = 0xff;
        }
        return;
    }
    FreePacketAndClear(&pCockpitExplosionShape, 0);
}

/* Function start: 0x4178A0 */
void place_damage_on_cockpit(short damage)
{
    if (nCameraViewMode == 0 && nTrainSimActive == 0 &&
        anCockpitDamageState[damage] == 0) {
        nPendingCockpitDamage = damage;
        anCockpitDamageState[damage] = 1;
        if (pCockpitExplosionShape == 0) {
            explosion_draw();
            return;
        }
        if (IsCockpitExplosionActive() == 0) {
            nCockpitExplosionFrame = 0x7fff;
            stCockpitExplosionPosition =
                aaCockpitDamagePositions[
                    (signed char)cCockpitView][damage];
        }
    }
}

/* Function start: 0x417910 */
void vid_transmit(void)
{
    short randomFrame;
    short speaker;
    unsigned char *background;

    speaker = nCommSpeakerObject;
    if (aeShipSide[speaker] == SIDE_NEUTRAL) {
        EndCommSessionWithWingman();
        return;
    }
    if ((bCockpitlessView != 0 ||
         nRenderedSpaceFrame % 2 != 0) &&
        nCommPortraitIndex != -1 &&
        aapszPilotSpeech[nCommPortraitIndex] != 0 &&
        bVideoImagesSuppressed == 0) {
        if (aeSpecialManeuver[speaker] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
            if (aeShipSide[speaker] == SIDE_IMPERIAL) {
                DrawSpriteDefault(
                    &stRightVdu, stRightVdu.left,
                    stRightVdu.top,
                    pCommStaticShape,
                    (short)(asObjectCounter[speaker] / 5));
                return;
            }
            DrawSpriteDefault(&stRightVdu, stRightVdu.left,
                              stRightVdu.top,
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
        if (aeShipSide[nCommSpeakerObject] ==
            SIDE_IMPERIAL) {
            background = pConfedCommBackground;
        } else {
            background = pKilrathiCommBackground;
        }
        DrawSpriteDefault(&stRightVdu, stRightVdu.left,
                          stRightVdu.top, background, 0);
        DrawSpriteDefault(
            &stRightVdu, stRightVdu.left, stRightVdu.top,
            apCommPortraitShapes[nCommPortraitIndex],
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
            nCarrierMissionShipIndex].state == 3 &&
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
            &stScreen,
            stCockpitLayout.leftVduBounds[
                (int)cCockpitView].left,
            stCockpitLayout.leftVduBounds[
                (int)cCockpitView].top,
            stCockpitLayout.leftVduBounds[
                (int)cCockpitView].right,
            stCockpitLayout.leftVduBounds[
                (int)cCockpitView].bottom,
            0);
        DrawFilledViewportRect(
            &stScreen,
            stCockpitLayout.rightVduBounds[
                (int)cCockpitView].left,
            stCockpitLayout.rightVduBounds[
                (int)cCockpitView].top,
            stCockpitLayout.rightVduBounds[
                (int)cCockpitView].right,
            stCockpitLayout.rightVduBounds[
                (int)cCockpitView].bottom,
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
        aHudMessageSlots[0].text = 0;
    else
        UpdateMessage(&aHudMessageSlots[0]);

    SetTextContext(&stRightVduTextContext);
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
        aHudMessageSlots[1].text = 0;
    else
        UpdateMessage(&aHudMessageSlots[1]);
    if (nTrainSimActive != 0 &&
        pPilotHandShape != 0)
        CopyTrainSimPilotViewToRightVdu();
}

/* Function start: 0x417E70 */
void update_cockpit(void)
{
    check_target();
    repair_internal_damage();
    if (cMissionObjectiveCount != 0)
        update_objective_location(
            (short)(nSpaceFrame %
                    (short)cMissionObjectiveCount));
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
        nDamageDisplayTicks = 0;
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
