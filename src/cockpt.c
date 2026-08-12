/*
 *  Cockpit HUD (Mac `cockpt` compilation unit): weapon, damage, target,
 *  objective, message, and pilot displays.
 *
 *  Address range 0x413000-0x417fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: the Mac `cockpt` symbol list identifies the surviving
 *  functions throughout this Win32 range; port-specific split helpers remain
 *  interleaved in their original address order.
 */
#include "wc1.h"

short g_asVduSelectionSound_00469000[2] = { 0x7f, 0 };
char *DAT_00469004 = 0;
char *DAT_00469008 = 0;
unsigned char DAT_0046900c = 0xff;
short g_asPilotHandOffsets_00469018[34] = {
    6, -3, 7, 2, 7, 9, 7, 12, 8, 13, 0, -1, -1, -1,
    -4, -1, -6, -1, 6, 0, 8, 0, 10, 0, 13, 3, 8, -7,
    6, -9, 5, -11, 5, -14
};
unsigned char *g_pCockpitPilotShape_0046905c = 0;
unsigned char *g_pCockpitExplosionBackground_00469060 = 0;
unsigned char * volatile g_pCockpitExplosionShape_00469064 = 0;
volatile short g_nCockpitExplosionFrame_00469068 = 8;
unsigned char *g_pReleaseWeaponDisplayBackground_0046906c = 0;
signed char g_cReleaseWeaponDisplayFrame_00469070 = -1;
signed char g_cReleaseWeaponDisplayTicks_00469074 = 0;
signed char g_cReleaseWeaponDisplayState_00469078 = 0;
int g_nDisplayedObjectiveRange_00469088 = 40000;
char *g_pszObjectiveStatusMessage_0046908c =
    g_szObjectiveStatusMessage_0046928c;
short g_nScannerTargetObject_00469090 = -1;
int g_aiScannerGridRows_00469098[79] = {
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
char *PTR_s_MISSILE_LOCKED_004691d4 = (char *)g_szMissileLocked_004692a8;
unsigned char g_bTargetBracketVisible_004691d8 = 1;
unsigned char g_abTargetBracketPadding_004691d9[7] = {0};
short DAT_004691e0[10] = {
    18, 14, 71, 5, 80, 29, 49, 27, 14, 13
};
short g_nTargetLockMarkerX_004691f4 = -0x7fff;
ShortRect g_stTargetBracketBounds_004691f8 = {-0x7fff, 0, 0, 0};
ShortRect g_stPreviousTargetBracketBounds_00469200 = {-0x7fff, 0, 0, 0};
short DAT_00469208 = -1;
Viewport g_stTrainSimVduSource_00469210 = {0};
ShortPoint g_aaCockpitDamagePositions_00469228[5][4] = {
    {{224, 5}, {132, 96}, {233, 107}, {149, 161}},
    {{177, 6}, {153, 142}, {103, 140}, {55, 183}},
    {{107, 25}, {211, 32}, {21, 178}, {300, 178}},
    {{74, 10}, {294, 19}, {197, 105}, {105, 134}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
unsigned char *g_pConfedCommBackground_00469278 = 0;
unsigned char *g_pCommStaticShape_0046927c = 0;
unsigned char *g_pKilrathiCommBackground_00469280 = 0;
int g_nCommPortraitFrame_00469284 = -1;
int g_bForceDamageDisplayRedraw_00469288 = 0;
char g_szObjectiveStatusMessage_0046928c[28] =
    "Wait for ??????????????????";
char g_szMissileLocked_004692a8[16] = "MISSILE LOCKED ";
char g_szAlreadyNear_004692b8[16] = "Already Near";
char g_szEnemyNear_004692c8[12] = "Enemy Near";
char g_szHazardNear_004692d4[12] = "Hazard Near";
char g_szComponentHitFormat_004692e0[8] = "%s HIT";
char g_szCalculating_0046931c[12] = "CALCULATING";
char g_szRangeKilometresSuffix_00469328[4] = " km";
char g_szNoObjective_0046932c[8] = "NONE";
char g_szUnknownObjective_00469334[8] = "UNKNOWN";
char g_szCompNavigation_0046933c[16] = "COMP NAVIGATION";
char g_szDestinationFormat_0046934c[20] =
    "\n\nDESTINATION\n  %s";
char g_szNavigationRangeLabel_00469360[12] = "\n\nRANGE\n  ";
char g_szNewObjectivePrompt_0046936c[20] =
    "\n\n(N)ew Objective";
char g_szAlreadyVisited_00469380[16] = "Already Visited";
char g_szObjectiveReached_00469390[20] = "Objective Reached";
char g_szWaitForFormat_004693a4[12] = "Wait for %s";
unsigned char DAT_004693b0 = 0;
char *g_pGameVersion_004693b4 = g_szGameVersion_004693b8;
char g_szGameVersion_004693b8[9] = "1.03F-95";

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
                      const char *format, unsigned long *arguments)
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
                g_pCurrentTextContext_0059af8c->backgroundColour =
                    (unsigned char)*arguments++;
                break;
            case 'D':
                text = _ltoa((long)*arguments++, number, 10);
                EmitTextString(writer, text);
                break;
            case 'F':
                g_pCurrentTextContext_0059af8c->colour =
                    (unsigned char)*arguments++;
                break;
            case 'J':
                g_pCurrentTextContext_0059af8c->alignment =
                    (unsigned char)*arguments++;
                break;
            case 'P':
                DrawTextString(g_pCurrentTextContext_0059af8c->text);
                break;
            case 'U':
                text = _ultoa(*arguments++, number, 10);
                EmitTextString(writer, text);
                break;
            case 'X':
                g_pCurrentTextContext_0059af8c->cursorX =
                    (short)*arguments++;
                break;
            case 'Y':
                g_pCurrentTextContext_0059af8c->cursorY =
                    (short)*arguments++;
                break;
            case 'c':
                writer((short)*arguments++);
                break;
            case 'd':
                text = _itoa((int)(short)*arguments++, number, 10);
                EmitTextString(writer, text);
                break;
            case 's':
                text = (char *)*arguments++;
                EmitTextString(writer, text);
                break;
            case 'u':
                text = _ultoa((unsigned short)*arguments++, number, 10);
                EmitTextString(writer, text);
                break;
            case 'x':
                text = _ultoa((unsigned short)*arguments++, number, 16);
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
    FormatTextTokens((void (__stdcall *)(int))DrawTextCharacter,
                     format, (unsigned long *)(&format + 1));
    if (g_pCurrentTextContext_0059af8c->viewport->pixels ==
        DAT_005a6ba0.pixels)
        DIBslam();
}

/* Function start: 0x413C70 */
void FormatTextBufferFromStart(const char *format, ...)
{
    g_pCurrentTextContext_0059af8c->textCursor =
        g_pCurrentTextContext_0059af8c->text;
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
    if (g_pCurrentTextContext_0059af8c->viewport->pixels ==
        DAT_005a6ba0.pixels)
        DIBslam();
}

/* Function start: 0x413CB0 */
void AppendFormattedText(const char *format, ...)
{
    FormatTextTokens((void (__stdcall *)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
    if (g_pCurrentTextContext_0059af8c->viewport->pixels ==
        DAT_005a6ba0.pixels)
        DIBslam();
}

/* Function start: 0x413CE0 */
void FatalErrorAndExit(const char *format, ...)
{
    char text[0xfc];

    vsprintf(text, format, (char *)(&format + 1));
    ShutdownEventManager();
    exit_squadron(text);
}

/* Function start: 0x413D20 */
unsigned short IsCockpitExplosionActive(void)
{
    return g_nCockpitExplosionFrame_00469068 < 8;
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
    int index;
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

    index = (int)bar + (int)g_cCockpitView_0059dab0 * 8;
    length = g_aaCockpitBars_0046dd88[0][index].length;
    extent = (short)(((int)percent * (int)length) / 100);
    left = g_aaCockpitBars_0046dd88[0][index].left;
    DAT_005a6be0.left = left;
    if (left != -99) {
        right = g_aaCockpitBars_0046dd88[0][index].right;
        top = g_aaCockpitBars_0046dd88[0][index].top;
        bottom = g_aaCockpitBars_0046dd88[0][index].bottom;
        DAT_005a6be0.right = right;
        DAT_005a6be0.top = top;
        DAT_005a6be0.bottom = bottom;
        emptyFrame = (signed char)
            g_aaCockpitBars_0046dd88[0][index].emptyFrame;
        filledFrame = (signed char)
            g_aaCockpitBars_0046dd88[0][index].filledFrame;
        direction = g_aaCockpitBars_0046dd88[0][index].direction;

        if (direction < 2) {
            if (direction == 1) {
                extent = (short)(length - extent);
                swapFrame = filledFrame;
                filledFrame = emptyFrame;
                emptyFrame = swapFrame;
            }
            DAT_005a6be0.bottom -= extent;
            if (DAT_005a6be0.top <= DAT_005a6be0.bottom)
                DrawSpriteDefault(&DAT_005a6be0, left, top,
                                  g_pCockpitDamageShape_005a76f4,
                                  filledFrame);
            DAT_005a6be0.top = (short)(DAT_005a6be0.bottom + 1);
            DAT_005a6be0.bottom = bottom;
            if (DAT_005a6be0.top <= bottom)
                DrawSpriteDefault(&DAT_005a6be0, left, top,
                                  g_pCockpitDamageShape_005a76f4,
                                  emptyFrame);
        } else {
            if (direction == 3) {
                extent = (short)(length - extent);
                swapFrame = filledFrame;
                filledFrame = emptyFrame;
                emptyFrame = swapFrame;
            }
            DAT_005a6be0.right -= extent;
            if (DAT_005a6be0.left <= DAT_005a6be0.right)
                DrawSpriteDefault(&DAT_005a6be0, left, top,
                                  g_pCockpitDamageShape_005a76f4,
                                  filledFrame);
            DAT_005a6be0.left = (short)(DAT_005a6be0.right + 1);
            DAT_005a6be0.right = right;
            if (DAT_005a6be0.left <= right)
                DrawSpriteDefault(&DAT_005a6be0, left, top,
                                  g_pCockpitDamageShape_005a76f4,
                                  emptyFrame);
        }
    }
}

/* Function start: 0x413F70 */
unsigned int InitializeCockpitReadout(signed char slot,
                                      TextContext *context)
{
    g_aCockpitReadouts_005a7e30[(int)slot].context = context;
    g_aCockpitReadouts_005a7e30[(int)slot].x = context->cursorX;
    g_aCockpitReadouts_005a7e30[(int)slot].y = context->cursorY;
    g_aCockpitReadouts_005a7e30[(int)slot].previousRight = 0;
    return 0;
}

/* Function start: 0x413FB0 */
void DrawCockpitReadout(signed char slot, char *text)
{
    CockpitReadout *readout;

    readout = &g_aCockpitReadouts_005a7e30[(int)slot];
    if (readout->x != -99) {
        SetTextContext(readout->context);
        SetTextCursor((unsigned short)readout->x,
                      (unsigned short)readout->y);
        DrawFormattedText(text);
        EraseCockpitReadoutRegion(
            &DAT_005a6ba0, readout->context->cursorX, readout->y,
            readout->previousRight,
            (short)(*(short *)readout->context->font + readout->y - 1),
            DAT_0046999c);
        readout->previousRight = readout->context->cursorX;
    }
}

/* Function start: 0x414050 */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top)
{
    CockpitReadout *readout;

    readout = &g_aCockpitReadouts_005a7e30[(int)slot];
    EraseCockpitReadoutRegion(
        &DAT_005a6ba0, left, top, readout->previousRight,
        (short)(*(short *)readout->context->font + readout->y),
        DAT_0046999c);
}

/* Function start: 0x4140A0 */
short DrawHudMessageSlot(HudMessageSlot *slot)
{
    unsigned char savedColour;
    short oldDrawColour;
    short showingEraseColour;

    if (DAT_0046c03c != 0)
        return 1;

    oldDrawColour = slot->drawColour;
    if (((int)DAT_0059ab54 / 40) % 3 == 0)
        slot->drawColour = DAT_0046999c;
    else
        slot->drawColour = slot->colour;

    if (slot->flashCount != -1) {
        if (slot->drawColour == DAT_0046999c &&
            oldDrawColour == slot->colour) {
            slot->flashCount = (signed char)MaxShort(
                0, (short)(slot->flashCount - 1));
        }
        if (slot->flashCount == 0)
            slot->drawColour = DAT_0046999c;
    }

    showingEraseColour = slot->drawColour == DAT_0046999c;
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
void ClearHudMessageIfMatching(HudMessageSlot *slot, char *text)
{
    if (slot->text == text)
        ClearHudMessageSlot(slot);
}

/* Function start: 0x4141D0 */
void ClearHudGunReadouts(void)
{
    DAT_005a7ddd = 0;
    DAT_005a7ddc = 0;
    DAT_005a7dee = 0;
    DAT_005a7ded = 0;
}

/* Function start: 0x4141F0 */
void SetHudMessageSlot(HudMessageSlot *slot, TextContext *context,
                       short x, short y, char *text,
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
void set_global_message(char *text, unsigned short colour,
                        int flashCount)
{
    SetHudMessageSlot(&DAT_005a7de1, &DAT_005a7700,
                      DAT_005a7530.left,
                      (short)(DAT_005a7530.bottom - 6),
                      text, colour, (signed char)flashCount);
}

/* Function start: 0x4142B0 */
void CockpitMessage(char *text, unsigned short colour,
                    int flashCount)
{
    if (text != DAT_005a7dee)
        set_global_message(text, colour, flashCount);
}

/* Function start: 0x4142E0 */
void remove_message(char *text)
{
    ClearHudMessageIfMatching(&DAT_005a7de1, text);
}

/* Function start: 0x414300 */
short kilrathi_near(short obj, short range)
{
    short ship;

    ship = 0;
    do {
        if (g_aeObjectClass_0059d100[ship] >= OBJECT_CLASS_SHIP &&
            g_aeShipSide_0059d650[ship] == SIDE_KILRATHI &&
            IsPointWithinRange(&g_aShipPosition_0059c490[obj],
                               &g_aShipPosition_0059c490[ship],
                               range) != 0)
            return 1;
        ship++;
    } while (ship < 10);
    return 0;
}

/* Function start: 0x414380 */
short auto_pilot_valid(short showReason)
{
    char *reason;

    reason = 0;
    if (g_cMissionObjectiveCount_0059c46a == 0)
        return 0;
    if (distance_from_point(
            0,
            &g_aMissionObjectives_0059dac0[
                g_cCurrentObjective_0046c020].position) < 8000) {
        reason = "Already Near";
    } else if (kilrathi_near(0, 16000) != 0) {
        reason = "Enemy Near";
    } else if (g_pActiveHazardField_0059bfe0 != 0) {
        reason = "Hazard Near";
    }
    if (showReason != 0 && reason != 0)
        set_global_message(reason, DAT_004699a8, 3);
    return reason == 0;
}

/* Function start: 0x414410 */
void *reset_cockpit(void)
{
    memset(g_abCockpitLightGoal_005a7eb8, 0,
           sizeof(g_abCockpitLightGoal_005a7eb8));
    memset(g_abCockpitLightState_005a7e70, 0,
           sizeof(g_abCockpitLightState_005a7e70));
    return g_abCockpitLightState_005a7e70;
}

/* Function start: 0x414440 */
unsigned int SetCockpitLightBlink(signed char light, short interval)
{
    if (interval < 20) {
        if (interval == 0) {
            g_abCockpitLightGoal_005a7eb8[(int)light] ^= 1;
            return 0;
        }
        if (g_nSpaceFrame_0059b420 % interval == 0) {
            g_abCockpitLightGoal_005a7eb8[(int)light] ^= 1;
            return 0;
        }
    } else {
        g_abCockpitLightGoal_005a7eb8[(int)light] = 0;
    }
    return 0;
}

/* Function start: 0x414490 */
void draw_cockpit_lights(void)
{
    int index;
    signed char light;
    short x;
    short y;
    short frame;

    if (g_nRenderedSpaceFrame_0059d61a % 4 == 0) {
        if (auto_pilot_valid(0) != 0)
            g_abCockpitLightGoal_005a7eb8[4] = 1;
        else
            g_abCockpitLightGoal_005a7eb8[4] = 0;
    }
    light = 0;
    do {
        if (DAT_0046a008 == 0) {
            if (g_abCockpitLightState_005a7e70[(int)light] !=
                g_abCockpitLightGoal_005a7eb8[(int)light]) {
                index = (int)g_cCockpitView_0059dab0 * 7 + (int)light;
                x = g_aasCockpitLightX_0046dca8[0][index];
                y = g_aasCockpitLightY_0046dcf0[0][index];
                if (g_abCockpitLightGoal_005a7eb8[(int)light] == 1)
                    frame = g_aacCockpitLightOnFrame_0046dd60[0][index];
                else
                    frame = g_aacCockpitLightOffFrame_0046dd38[0][index];
                DrawSpriteDefault(&DAT_005a6ba0, x, y,
                                  g_pCockpitDamageShape_005a76f4, frame);
                g_abCockpitLightState_005a7e70[(int)light] =
                    g_abCockpitLightGoal_005a7eb8[(int)light];
            }
        } else {
            index = (int)g_cCockpitView_0059dab0 * 7 + (int)light;
            x = g_aasCockpitLightX_0046dca8[0][index];
            y = g_aasCockpitLightY_0046dcf0[0][index];
            if (g_abCockpitLightGoal_005a7eb8[(int)light] == 1)
                frame = g_aacCockpitLightOnFrame_0046dd60[0][index];
            else
                frame = g_aacCockpitLightOffFrame_0046dd38[0][index];
            DrawSpriteDefault(&DAT_005a6ba0, x, y,
                              g_pCockpitDamageShape_005a76f4, frame);
            g_abCockpitLightState_005a7e70[(int)light] =
                g_abCockpitLightGoal_005a7eb8[(int)light];
        }
        light++;
    } while (light < 7);
}

/* Function start: 0x4145B0 */
void update_lights(void)
{
    short fuelPercent;

    fuelPercent = (short)(
        (g_anShipFuel_0059b470[0] * 100) /
        *(int *)&g_aObjectTypeData_00466458[
            g_aeObjectType_0059b560[0]].lifetime);
    SetCockpitLightBlink(6, fuelPercent);
    vdu_polygon(0, fuelPercent);
    vdu_polygon(1, g_asShipWeaponEnergy_0059d470[0]);

    if (g_nTrainSimActive_00469e2c == 0) {
        if (calculate_damage_level() >= 3 &&
            (int)g_aasShipShield_0059d5b0[0][1] +
                (int)g_aasShipShield_0059d5b0[0][0] < 10) {
            SetCockpitLightBlink(3, 2);
            if (DAT_005a7ec0 == 0 ||
                g_nSpaceFrame_0059b420 % 10 == 0)
                PlaySfxWaveFileByNumber(0x20, -1, 0);
        } else if (DAT_005a7ec0 != 0) {
            ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
                DAT_005a7ec0, 1);
            DAT_005a7ec0 = 0;
            g_abCockpitLightGoal_005a7eb8[3] = 0;
        }
    }
}

/* Function start: 0x414690 */
void update_bars(void)
{
    ObjectTypeData *typeData;
    short forePercent;
    short aftPercent;

    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[0]];
    vdu_polygon(2,
        (short)((g_aasShipArmor_0059d420[0][0] * 100) /
                typeData->armorFront));
    vdu_polygon(3,
        (short)((g_aasShipArmor_0059d420[0][1] * 100) /
                typeData->armorRear));
    vdu_polygon(4,
        (short)((g_aasShipArmor_0059d420[0][2] * 100) /
                typeData->armorRight));
    vdu_polygon(5,
        (short)((g_aasShipArmor_0059d420[0][3] * 100) /
                typeData->armorLeft));
    forePercent = (short)((g_aasShipShield_0059d5b0[0][0] * 100) /
                          typeData->shieldFore);
    SetCockpitLightBlink(0, forePercent);
    vdu_polygon(6, forePercent);
    DrawCockpitReadout(
        4, _itoa((int)g_aasShipShield_0059d5b0[0][0],
                 g_szTextScratchBuffer_00598b00, 10));
    aftPercent = (short)((g_aasShipShield_0059d5b0[0][1] * 100) /
                         typeData->shieldAft);
    SetCockpitLightBlink(1, aftPercent);
    vdu_polygon(7, aftPercent);
    DrawCockpitReadout(
        5, _itoa((int)g_aasShipShield_0059d5b0[0][1],
                 g_szTextScratchBuffer_00598b00, 10));
}

/* Function start: 0x4147E0 */
unsigned short get_mode(short i)
{
    return DAT_0059d500[((int)DAT_0059dec0[i] + i * 4) * 2];
}

/* Function start: 0x414800 */
void set_mode(short i, int state)
{
    if ((short)get_mode(i) != state)
        ClearHudMessageSlot(&DAT_005a7dd0[i]);
    DAT_0059dec0[i] = 0;
    *(int *)&DAT_0059d500[i * 8] = state;
}

/* Function start: 0x414850 */
unsigned short SetVduModeIfChanged(short i, int state)
{
    short changed;

    changed = (short)get_mode(i) != state;
    if (changed != 0)
        set_mode(i, state);
    return changed;
}

/* Function start: 0x414890 */
int GetVduModeStackDepth(short i)
{
    return (char)DAT_0059dec0[i];
}

/* Function start: 0x4148A0 */
void push_mode(short i, int state)
{
    ClearHudMessageSlot(&DAT_005a7dd0[i]);
    DAT_0059dec0[i] = DAT_0059dec0[i] + 1;
    *(int *)&DAT_0059d500[((int)DAT_0059dec0[i] + i * 4) * 2] = state;
}

/* Function start: 0x4148E0 */
void pop_mode(short i)
{
    ClearHudMessageSlot(&DAT_005a7dd0[i]);
    DAT_0059dec0[i] = DAT_0059dec0[i] - 1;
}

/* Function start: 0x414910 */
void set_new_vdu(short vdu)
{
    if ((short)get_mode(vdu) == 0) {
        malf_noise(vdu, 1, DAT_004699b0, 0x17, 0);
    } else {
        if (vdu == 1)
            ClearViewport(&DAT_005a7530, DAT_0046999c);
        else
            ClearViewport(&DAT_005a6b80, DAT_0046999c);
    }
    DAT_0059ce18[vdu] = (unsigned int)(short)get_mode(vdu);
}

/* Function start: 0x414980 */
short update_vid_disp(short vdu)
{
    short changed;

    changed = (short)get_mode(vdu) != (int)DAT_0059ce18[vdu];
    if (changed != 0)
        set_new_vdu(vdu);
    return changed;
}

/* Function start: 0x4149C0 */
void InvalidateVduMode(short i)
{
    DAT_0059ce18[i] = 0;
}

/* Function start: 0x4149E0 */
void clear_message_time(void)
{
    DAT_005a7dca = 0;
}

/* Function start: 0x4149F0 */
unsigned short message_showing(void)
{
    return 0 < DAT_005a7dca;
}

/* Function start: 0x414A10 */
unsigned short set_message_time(unsigned short v)
{
    DAT_005a7dca = (short)v;
    return 0;
}

/* Function start: 0x414A20 */
void check_message(void)
{
    if (message_showing() &&
        (DAT_005a7dca = DAT_005a7dca - 1, DAT_005a7dca <= 0))
        EndCommMenu();
}

/* Function start: 0x414A50 */
unsigned int update_digital_readouts(void)
{
    long velocity;

    SetTextContext(&DAT_005a7720);
    DrawCockpitReadout(
        2, _itoa((int)(short)((g_anShipSpeed_0059b320[0] >> 8) * 10),
                 g_szTextScratchBuffer_00598b00, 10));
    velocity = MultiplyFixed(
        Vector_magnitude(&g_aShipVelocity_0059c010[0]), 0xa00);
    DrawCockpitReadout(
        3, _itoa((int)(short)(velocity >> 8),
                 g_szTextScratchBuffer_00598b00, 10));
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
unsigned short malf(char component)
{
    int damage = g_acPlayerComponentDamage_0059bff0[(int)component];

    return (unsigned short)RandomInRange(0, 15) < damage * damage;
}

/* Function start: 0x414B20 */
unsigned short vdu_malf(short vdu, short sound)
{
    if (DAT_0046c03c == 0)
        malf_noise(vdu, 1, DAT_004699b0, sound, 0);
    set_mode(vdu, 0);
    return 0;
}

/* Function start: 0x414B70 */
void ShowComponentHitHudMessage(char *text, unsigned short colour,
                                signed char flashCount)
{
    if (g_nTrainSimActive_00469e2c == 0 && get_mode(0) != 0) {
        if (DAT_005a7ddd != 0)
            ClearHudMessageSlot(&DAT_005a7dd0[0]);
        DosStrcpy(g_szComponentHitMessage_005a7e00, text);
        SetHudMessageSlot(&DAT_005a7dd0[0], &DAT_005a74f0,
                          DAT_005a6b80.left,
                          (short)(DAT_005a6b80.bottom - 6),
                          g_szComponentHitMessage_005a7e00,
                          colour, flashCount);
    }
}

/* Function start: 0x414BF0 */
int damage_your_component(char component, char amount, char maximum)
{
    int index = (int)component;
    char text[40];

    g_acPlayerComponentDamage_0059bff0[index] = (signed char)MinShort(
        (short)(g_acPlayerComponentDamage_0059bff0[index] + amount),
        (short)maximum);
    if (malf(component) != 0 && index == 3) {
        vdu_malf(0, 0x18);
        vdu_malf(1, 0x18);
    }
    if ((short)get_mode(0) == 2 || (short)get_mode(0) == 1) {
        sprintf(text, g_szComponentHitFormat_004692e0,
                g_apszComponentNames_0046a778[index]);
        ShowComponentHitHudMessage(text, (unsigned char)DAT_004699ac, 5);
    }
    return g_acPlayerComponentDamage_0059bff0[index];
}

/* Function start: 0x414CB0 */
void RemovePlayerReleaseWeapon(signed char weapon)
{
    volatile ShipWeaponSlot *loadout;
    enum ObjectType preferredType;
    int hardpoint;

    loadout = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];
    preferredType = loadout[weapon].type;
    g_eReleaseWeaponDisplayType_005a7dc0 = preferredType;
    g_cReleaseWeaponDisplayFrame_00469070 =
        (signed char)(preferredType * 2 - 0x2f);
    hardpoint = loadout[weapon].hardpoint;
    g_nReleaseWeaponDisplayX_005a7dbc =
        (short)(g_aWeaponDisplayPositions_00468440[hardpoint].x +
                g_nWeaponDisplayOriginX_005a7788);
    g_nReleaseWeaponDisplayY_005a7dbe =
        (short)(g_aWeaponDisplayPositions_00468440[hardpoint].y +
                g_nWeaponDisplayOriginY_005a778a);
    g_cReleaseWeaponDisplayTicks_00469074 = 3;
    g_cReleaseWeaponDisplayState_00469078 = 0;
    if (g_pReleaseWeaponDisplayBackground_0046906c == 0)
        g_cReleaseWeaponDisplayFrame_00469070 = -1;
    remove_weapon(0, weapon);
    g_nSelectedReleaseWeaponIndex_0046c058 = -1;
    select_new_release_weapon(preferredType);
}

/* Function start: 0x414D50 */
void fire_computer_graphic_missile(void)
{
    short visible;

    if (g_cReleaseWeaponDisplayFrame_00469070 == -1)
        return;
    visible = DAT_0046c03c == 0 && (short)get_mode(0) == 1;
    if (g_cReleaseWeaponDisplayState_00469078 != 0) {
        RestoreSpriteBackground(
            &DAT_005a6b80, g_pReleaseWeaponDisplayBackground_0046906c,
            g_nReleaseWeaponDisplayX_005a7dbc,
            g_nReleaseWeaponDisplayY_005a7dbe,
            g_pCockpitWeaponShape_005a7564,
            g_cReleaseWeaponDisplayFrame_00469070);
    }
    if (g_nReleaseWeaponDisplayY_005a7dbe > DAT_005a6b80.top - 10 &&
        g_nReleaseWeaponDisplayY_005a7dbe < DAT_005a6b80.bottom) {
        if (g_eReleaseWeaponDisplayType_005a7dc0 ==
            OBJECT_TYPE_SPACE_MINE)
            g_nReleaseWeaponDisplayY_005a7dbe +=
                (short)g_cReleaseWeaponDisplayTicks_00469074;
        else
            g_nReleaseWeaponDisplayY_005a7dbe -=
                (short)g_cReleaseWeaponDisplayTicks_00469074;
        g_cReleaseWeaponDisplayTicks_00469074++;
        if (visible != 0) {
            CaptureSpriteBackground(
                &DAT_005a6b80,
                g_pReleaseWeaponDisplayBackground_0046906c,
                g_nReleaseWeaponDisplayX_005a7dbc,
                g_nReleaseWeaponDisplayY_005a7dbe,
                g_pCockpitWeaponShape_005a7564,
                g_cReleaseWeaponDisplayFrame_00469070);
            DrawSpriteDefault(
                &DAT_005a6b80, g_nReleaseWeaponDisplayX_005a7dbc,
                g_nReleaseWeaponDisplayY_005a7dbe,
                g_pCockpitWeaponShape_005a7564,
                g_cReleaseWeaponDisplayFrame_00469070);
        }
        g_cReleaseWeaponDisplayState_00469078 = (signed char)visible;
    } else {
        g_cReleaseWeaponDisplayFrame_00469070 = -1;
    }
}

/* Function start: 0x414EA0 */
void show_weapon_disp(void)
{
    ShipWeaponSlot *weapons;
    ShipWeaponSlot *selectedWeapon;
    const char *releaseName;
    const char *gunName;
    signed char count;

    selectedWeapon =
        &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1])[
            g_nSelectedReleaseWeaponIndex_0046c058];
    set_new_vdu(0);
    DrawTextAt(&DAT_005a74f0, DAT_005a6b80.left, DAT_005a6b80.top,
               "WEAPON DISPLAY", 2);
    DrawViewportLine(&DAT_005a6b80, (short)(DAT_005a6b80.left + 2),
                     (short)(DAT_005a6b80.top + 5),
                     (short)(DAT_005a6b80.right - 2),
                     (short)(DAT_005a6b80.top + 5), DAT_004699b4);

    releaseName = "";
    if (g_nSelectedReleaseWeaponIndex_0046c058 != -1)
        releaseName = g_aObjectTypeData_00466458[
            selectedWeapon->type].displayName;
    gunName = "";
    if (g_eSelectedGunType_0046c054 == (enum ObjectType)-1)
        gunName = "";
    else if ((int)g_eSelectedGunType_0046c054 == 0x80)
        gunName = "Full Guns";
    else
        gunName = g_aObjectTypeData_00466458[
            g_eSelectedGunType_0046c054].displayName;
    DrawFormattedText("Weapon: %s\n", releaseName);
    DrawFormattedText("Gun: %s\n", gunName);

    g_nWeaponDisplayOriginX_005a7788 =
        (short)(DAT_005a6b80.left +
                g_aWeaponDisplayOrigins_004684c0[
                    (int)g_cCockpitView_0059dab0].x);
    g_nWeaponDisplayOriginY_005a778a =
        (short)(DAT_005a6b80.top +
                g_aWeaponDisplayOrigins_004684c0[
                    (int)g_cCockpitView_0059dab0].y);
    DrawSpriteDefault(&DAT_005a6b80, g_nWeaponDisplayOriginX_005a7788,
                      g_nWeaponDisplayOriginY_005a778a,
                      g_pCockpitWeaponShape_005a7564, 0);
    weapons = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];
    count = 0;
    if ((signed char)g_aShipWeapons_0059cab0[0][0] > 0)
        do {
            count++;
            DrawSpriteDefault(
                &DAT_005a6b80,
                (short)(g_nWeaponDisplayOriginX_005a7788 +
                        g_aWeaponDisplayPositions_00468440[
                            weapons->hardpoint].x),
                (short)(g_nWeaponDisplayOriginY_005a778a +
                        g_aWeaponDisplayPositions_00468440[
                            weapons->hardpoint].y),
                g_pCockpitWeaponShape_005a7564,
                (short)((int)weapons->type * 2 +
                        weapons->disabled - 0x2f));
            weapons++;
        } while (count < (signed char)g_aShipWeapons_0059cab0[0][0]);
}

/* Function start: 0x415040 */
void update_status_text(void)
{
}

/* Function start: 0x415050 */
short sighted(short objective)
{
    return (g_aMissionObjectives_0059dac0[objective].flags & 4) == 4;
}

/* Function start: 0x415070 */
short visited(short objective)
{
    return (g_aMissionObjectives_0059dac0[objective].flags & 1) == 1;
}

/* Function start: 0x415090 */
short achieved(short objective)
{
    return (g_aMissionObjectives_0059dac0[objective].flags & 2) == 2;
}

/* Function start: 0x4150B0 */
void flag_objective(short objective, unsigned char flags)
{
    g_aMissionObjectives_0059dac0[objective].flags |= flags;
}

/* Function start: 0x4150D0 */
void DrawCalculatingLabel(void)
{
    if (g_nCurrentObjectiveRange_0059d2b0 <= 0) {
        DrawCockpitReadout(0, (char *)g_szCalculating_0046931c);
    } else {
        strcpy(g_szTextScratchBuffer_00598b00 +
                   strlen(_ltoa((long)g_nCurrentObjectiveRange_0059d2b0,
                                g_szTextScratchBuffer_00598b00, 10)),
               " km");
        DrawCockpitReadout(0, g_szTextScratchBuffer_00598b00);
    }
    g_nDisplayedObjectiveRange_00469088 =
        g_nCurrentObjectiveRange_0059d2b0;
}

/* Function start: 0x415130 */
char *objective_name(short objective)
{
    if (objective >= (short)g_cMissionObjectiveCount_0059c46a)
        return (char *)g_szNoObjective_0046932c;
    if (*g_aMissionObjectives_0059dac0[objective].name == '?' &&
        sighted(objective) == 0)
        return (char *)g_szUnknownObjective_00469334;
    return (char *)g_aMissionObjectives_0059dac0[objective].displayName;
}

/* Function start: 0x415180 */
void show_navigation_disp(void)
{
    DrawTextAt(&DAT_005a7700, DAT_005a7530.left, DAT_005a7530.top,
               (char *)g_szCompNavigation_0046933c, 2);
    DrawFormattedText(g_szDestinationFormat_0046934c,
                      objective_name((short)g_cCurrentObjective_0046c020));
    DrawFormattedText(g_szNavigationRangeLabel_00469360);
    InitializeCockpitReadout(0, &DAT_005a7700);
    DrawFormattedText(g_szNewObjectivePrompt_0046936c);
    DrawCalculatingLabel();
}

/* Function start: 0x4151F0 */
short hidden_objective(short objective)
{
    short nameHidden;
    short hidden;
    short ship;

    nameHidden =
        *g_aMissionObjectives_0059dac0[objective].displayName == '.' ||
        *g_aMissionObjectives_0059dac0[objective].name == '.';
    if (nameHidden != 0 ||
        (mobile_objective(objective) != 0 &&
         g_aMissionShips_0046c948[
             (signed char)g_aMissionObjectives_0059dac0[
                 objective].index].state != 0))
        hidden = 1;
    else
        hidden = 0;
    if (hidden == 0 && mobile_objective(objective) != 0 &&
        g_aMissionObjectives_0059dac0[
            g_cMissionObjectiveCount_0059c46a].type == 0) {
        ship = find_ship_index(
            (short)g_aMissionObjectives_0059dac0[objective].index);
        if (g_aMissionShips_0046c948[
                (signed char)g_aMissionObjectives_0059dac0[
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
        } while (g_abFlightPath_0059c000[pathIndex] != -1 &&
                 pathIndex < (short)g_cMissionObjectiveCount_0059c46a);
        pathIndex--;
    }
    if (pathIndex > (short)g_cMissionObjectiveCount_0059c46a)
        pathIndex = 0;
    if (g_abFlightPath_0059c000[pathIndex] == -1)
        pathIndex = 0;
    g_cCurrentNavPointIndex_0059c86c = (signed char)pathIndex;
    if (hidden_objective(
            (short)g_abFlightPath_0059c000[pathIndex]) != 0)
        return 0;
    g_cCurrentObjective_0046c020 =
        g_abFlightPath_0059c000[g_cCurrentNavPointIndex_0059c86c];
    g_aeShipObjective_0059d200[0] =
        (enum ShipObjective)g_aMissionObjectives_0059dac0[
            (short)g_cCurrentObjective_0046c020].type;
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
                (short)((short)g_cCurrentNavPointIndex_0059c86c + 1)) != 0)
            break;
        if (g_cCurrentNavPointIndex_0059c86c == 0)
            wraps++;
    } while (wraps < 3);
    if (wraps >= 3) {
        g_cCurrentNavPointIndex_0059c86c = 0;
        g_cCurrentObjective_0046c020 = g_abFlightPath_0059c000[0];
    }
    return wraps < 3;
}

/* Function start: 0x4153D0 */
void set_next_destination(void)
{
    set_new_objective(0);
    do {
        if (set_new_objective(
                (short)g_cCurrentNavPointIndex_0059c86c) != 0 &&
            visited((short)g_abFlightPath_0059c000[
                g_cCurrentNavPointIndex_0059c86c]) == 0)
            break;
        g_cCurrentNavPointIndex_0059c86c++;
    } while (g_cCurrentNavPointIndex_0059c86c <
                 g_cMissionObjectiveCount_0059c46a &&
             g_abFlightPath_0059c000[
                 g_cCurrentNavPointIndex_0059c86c] != -1);
    if (g_cCurrentNavPointIndex_0059c86c >=
            g_cMissionObjectiveCount_0059c46a ||
        g_abFlightPath_0059c000[
            g_cCurrentNavPointIndex_0059c86c] == -1) {
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
            g_aMissionObjectives_0059dac0[objective].index,
            &g_aMissionObjectives_0059dac0[objective].position);
    }
    return ship;
}

/* Function start: 0x4154C0 */
unsigned int someone_coming(void)
{
    short ship = 0;

    do {
        if (g_aeObjectClass_0059d100[ship] >= OBJECT_CLASS_SHIP &&
            g_aeShipMissionType_0059c3f0[ship] == MISSION_TYPE_COME_HOME &&
            g_abShipNavPointIndex_0059d7c0[ship] <= g_cCurrentNavPointIndex_0059c86c)
            return 1;
        ship = ship + 1;
    } while (ship < 10);
    return 0;
}

/* Function start: 0x415510 */
unsigned int escorting_a_ship(void)
{
    if (g_aeShipMissionType_0059c3f0[0] != MISSION_TYPE_ESCORT) {
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

    carrierMissionShip = g_anShipMissionShip_0059d4b0[0];
    objectiveType = (short)g_aMissionObjectives_0059dac0[objective].type;
    carrierObject = find_ship_index(carrierMissionShip);
    markVisited = objective != g_cCurrentObjective_0046c020;
    advanceDestination = 0;
    if (objective == g_cCurrentObjective_0046c020) {
        if (reached == 0 && escorting_a_ship() != 0 &&
            carrierObject != -1 &&
            g_aMissionObjectives_0059dac0[objective].index !=
                g_anShipMissionShip_0059d4b0[0]) {
            if (objectiveType != 1 ||
                g_aMissionShips_0046c948[carrierMissionShip].state != 1) {
                sprintf(g_pszObjectiveStatusMessage_0046908c,
                        g_szWaitForFormat_004693a4,
                        g_aObjectTypeData_00466458[
                            g_aeObjectType_0059b560[carrierObject]].
                                displayName);
                CockpitMessage(g_pszObjectiveStatusMessage_0046908c,
                               DAT_004699a8, 4);
            }
        } else {
            advanceDestination = 1;
            CockpitMessage(
                visited(objective) != 0
                    ? (char *)g_szAlreadyVisited_00469380
                    : (char *)g_szObjectiveReached_00469390,
                DAT_004699a8, 4);
            markVisited = advanceDestination;
        }
    }
    if (objectiveType != 1 && markVisited != 0) {
        if (visited(objective) == 0 && carrierObject != -1 &&
            g_aMissionObjectives_0059dac0[objective].index ==
                g_anShipMissionShip_0059d4b0[0] &&
            g_aeObjectType_0059b560[carrierObject] !=
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
         g_asObjectScreenX_0059d9b0[object] != (short)0x8001))
        flag_objective(objective, 4);
}

/* Function start: 0x415720 */
void check_visit(short objective, short range)
{
    int reachedRange;

    if ((short)g_aMissionObjectives_0059dac0[objective].type == 3 ||
        (short)g_aMissionObjectives_0059dac0[objective].type == 4)
        reachedRange = 6000;
    else
        reachedRange = 1500;
    if (range < reachedRange)
        flag_reached(objective, 0);
}

/* Function start: 0x415770 */
void update_objective_location(short objective)
{
    MissionObjective *missionObjective;
    FixedVector delta;
    short object;
    short range;

    object = LocateMobileObjective(objective);
    missionObjective = &g_aMissionObjectives_0059dac0[objective];
    if (sighted(objective) != 0 && visited(objective) != 0 &&
        g_cCurrentObjective_0046c020 != objective)
        return;
    ComputeVectorDelta(&g_aShipPosition_0059c490[0],
                       &missionObjective->position, &delta);
    range = FixedToShortSaturating(
        (int)Vector_magnitude(&delta));
    check_sighting(objective, range, object);
    if (mobile_objective(objective) != 0) {
        if (object != -1)
            check_visit(objective, range);
    } else if (g_aMissionNavPoints_0046c2f0[
                   missionObjective->index].type >= 1) {
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
    state = g_aMissionShips_0046c948[
        g_aMissionObjectives_0059dac0[objectiveIndex].index].state;
    objectiveType = g_aMissionObjectives_0059dac0[objectiveIndex].type;
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
    if (objective_lost((short)g_cCurrentObjective_0046c020) != 0) {
        cycle_next_objective();
        InvalidateVduMode(1);
    } else {
        update_objective_location((short)g_cCurrentObjective_0046c020);
    }
    if (g_nDisplayedObjectiveRange_00469088 !=
        g_nCurrentObjectiveRange_0059d2b0)
        DrawCalculatingLabel();
}

/* Function start: 0x4158F0 */
void rotational_pos_to_scanner_pos(signed char object,
                                   const SphericalVector *position)
{
    short horizontal;

    horizontal = position->yaw;
    if (abs((int)horizontal) < 45)
        g_nScannerCursorX_005a7e6c = (short)(
            g_stCockpitLayout_0046e000.scanner[
                (int)g_cCockpitView_0059dab0].centerX + horizontal / 4);
    else
        g_nScannerCursorX_005a7e6c = (short)(
            g_stCockpitLayout_0046e000.scanner[
                (int)g_cCockpitView_0059dab0].centerX + horizontal / 6);

    g_nScannerCursorY_005a7e6e = (short)(
        g_stCockpitLayout_0046e000.scanner[
            (int)g_cCockpitView_0059dab0].centerY + position->pitch / -3);
    g_nScannerCursorX_005a7e6c =
        MinShort(g_stCockpitLayout_0046e000.scanner[
                     (int)g_cCockpitView_0059dab0].maximumX,
                 g_nScannerCursorX_005a7e6c);
    g_nScannerCursorX_005a7e6c =
        MaxShort(g_stCockpitLayout_0046e000.scanner[
                     (int)g_cCockpitView_0059dab0].minimumX,
                 g_nScannerCursorX_005a7e6c);
    g_nScannerCursorY_005a7e6e =
        MinShort(g_stCockpitLayout_0046e000.scanner[
                     (int)g_cCockpitView_0059dab0].maximumY,
                 g_nScannerCursorY_005a7e6e);
    g_nScannerCursorY_005a7e6e =
        MaxShort(g_stCockpitLayout_0046e000.scanner[
                     (int)g_cCockpitView_0059dab0].minimumY,
                 g_nScannerCursorY_005a7e6e);
    g_asScannerObjectX_005a7ea0[(int)object] =
        g_nScannerCursorX_005a7e6c;
    g_asScannerObjectY_005a7e80[(int)object] =
        g_nScannerCursorY_005a7e6e;
}

/* Function start: 0x415A30 */
short mobile_objective(short objective)
{
    int type = g_aMissionObjectives_0059dac0[objective].type;

    if (type == 1 || type == 3 || type == 4 || type == 2)
        return 1;
    return 0;
}

/* Function start: 0x415A70 */
void ResetScannerContacts(void)
{
    short i = 10;

    do {
        g_asScannerObjectX_005a7ea0[i] = 0;
        i = i - 1;
    } while (i != 0);
    g_nScannerTargetObject_00469090 = -1;
}

/* Function start: 0x415A90 */
void clear_head_up_display(void)
{
    short object;

    if (g_nScannerTargetObject_00469090 != -1) {
        object = g_nScannerTargetObject_00469090;
        DrawViewportPixel(&DAT_005a6ba0,
                          g_asScannerObjectX_005a7ea0[object],
                          g_asScannerObjectY_005a7e80[object],
                          g_asScannerBackgroundColour_005a7ed0[object]);
        g_asScannerObjectX_005a7ea0[
            g_nScannerTargetObject_00469090] = 0;
    }
    if (g_asScannerObjectX_005a7ea0[10] != 0) {
        RestoreSpriteBackground(&DAT_005a6ba0,
                                g_pScannerMarkerBackground_005a7dc4,
                                g_asScannerObjectX_005a7ea0[10],
                                g_asScannerObjectY_005a7e80[10],
                                g_pTargetLockShape_005a6bf4, 2);
        g_asScannerObjectX_005a7ea0[10] = 0;
    }
    object = 9;
    do {
        if (g_asScannerObjectX_005a7ea0[object] != 0) {
            DrawViewportPixel(&DAT_005a6ba0,
                              g_asScannerObjectX_005a7ea0[object],
                              g_asScannerObjectY_005a7e80[object],
                              g_asScannerBackgroundColour_005a7ed0[object]);
            g_asScannerObjectX_005a7ea0[object] = 0;
        }
        object--;
    } while (object != 0);
    g_nScannerTargetObject_00469090 = -1;
}

/* Function start: 0x415B70 */
unsigned int set_objective_range(short showOnScanner)
{
    FixedVector relative;
    FixedVector rotated;
    SphericalVector spherical;

    LocateMobileObjective((short)g_cCurrentObjective_0046c020);
    ComputeVectorDelta(&g_aShipPosition_0059c490[0],
                       &g_aMissionObjectives_0059dac0[
                           (short)g_cCurrentObjective_0046c020].position,
                       &relative);
    transform_to_objects_frame(&relative, &rotated, 0);
    rectangular_to_spherical(&rotated, &spherical);
    if (showOnScanner != 0)
        rotational_pos_to_scanner_pos(10, &spherical);
    g_nCurrentObjectiveRange_0059d2b0 = spherical.radius >> 8;
    return 0;
}

/* Function start: 0x415C00 */
short get_color(short object, unsigned short *colour)
{
    enum ObjectClass objectClass;

    objectClass = g_aeObjectClass_0059d100[object];
    if (objectClass < OBJECT_CLASS_MISSILE)
        return 0;
    if (objectClass == OBJECT_CLASS_SHIP) {
        if (g_aeShipSide_0059d650[object] == SIDE_KILRATHI)
            *colour = DAT_004699ac;
        else if (g_aeShipSide_0059d650[object] == SIDE_IMPERIAL)
            *colour = DAT_004699a4;
        else
            *colour = DAT_004699b4;
        return 1;
    }
    if (objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        if (g_aeShipSide_0059d650[object] == SIDE_KILRATHI)
            *colour = DAT_004699b8;
        else if (g_aeObjectType_0059b560[object] ==
                 OBJECT_TYPE_TIGERS_CLAW)
            *colour = g_cViewportClearColour_004699a0;
        else
            *colour = DAT_004699c0;
        return 1;
    }
    if (g_acShipTarget_0059ce60[object] == 0) {
        *colour = DAT_004699a8;
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

    if (DAT_0046a008 != 0 && g_aiScannerGridRows_00469098[0] != -2) {
        scanner = &g_stCockpitLayout_0046e000.scanner[
            (int)g_cCockpitView_0059dab0];
        row = 0;
        grid = g_aiScannerGridRows_00469098;
        do {
            if (*grid == -1) {
                row++;
            } else {
                DrawViewportPixel(&DAT_005a6ba0,
                                  (short)(scanner->centerX + row),
                                  (short)(scanner->centerY + *grid), 0xaa);
                if (*grid != 0)
                    DrawViewportPixel(&DAT_005a6ba0,
                                      (short)(scanner->centerX + row),
                                      (short)(scanner->centerY - *grid), 0xaa);
                if (row != 0) {
                    DrawViewportPixel(&DAT_005a6ba0,
                                      (short)(scanner->centerX - row),
                                      (short)(scanner->centerY + *grid), 0xaa);
                    if (*grid != 0)
                        DrawViewportPixel(&DAT_005a6ba0,
                                          (short)(scanner->centerX - row),
                                          (short)(scanner->centerY - *grid),
                                          0xaa);
                }
            }
            grid++;
        } while (*grid != -2);
    }

    clear_head_up_display();
    g_nScannerTargetObject_00469090 = g_acShipTarget_0059ce60[0];
    if (g_nScannerTargetObject_00469090 != -1 &&
        g_aeObjectClass_0059d100[g_nScannerTargetObject_00469090] <
            OBJECT_CLASS_SHIP)
        g_nScannerTargetObject_00469090 = -1;

    object = 1;
    do {
        if (get_color(object, &colour) != 0) {
            rectangular_to_spherical(&g_aObjectViewPosition_0059afa0[object],
                                     &spherical);
            if (spherical.radius < 0xea6000) {
                rotational_pos_to_scanner_pos((signed char)object,
                                               &spherical);
                g_asScannerBackgroundColour_005a7ed0[object] =
                    (short)GetViewportPixel(
                        &DAT_005a6ba0, g_nScannerCursorX_005a7e6c,
                        g_nScannerCursorY_005a7e6e);
                if (g_nScannerTargetObject_00469090 != object)
                    DrawViewportPixel(&DAT_005a6ba0,
                                      g_nScannerCursorX_005a7e6c,
                                      g_nScannerCursorY_005a7e6e,
                                      colour);
            }
        }
        object++;
    } while (object < 10);

    if ((short)get_mode(1) == 5) {
        set_objective_range(1);
        CaptureSpriteBackground(&DAT_005a6ba0,
                                g_pScannerMarkerBackground_005a7dc4,
                                g_nScannerCursorX_005a7e6c,
                                g_nScannerCursorY_005a7e6e,
                                g_pTargetLockShape_005a6bf4, 2);
        DrawSpriteDefault(&DAT_005a6ba0,
                          g_nScannerCursorX_005a7e6c,
                          g_nScannerCursorY_005a7e6e,
                          g_pTargetLockShape_005a6bf4, 2);
    }

    if (g_nScannerTargetObject_00469090 != -1 &&
        get_color(g_nScannerTargetObject_00469090, &colour) != 0) {
        if ((abs((int)g_nSpaceFrame_0059b420) & 1) == 0)
            colour = DAT_0046999c;
        object = g_nScannerTargetObject_00469090;
        DrawViewportPixel(&DAT_005a6ba0,
                          g_asScannerObjectX_005a7ea0[object],
                          g_asScannerObjectY_005a7e80[object], colour);
    }
    return 0;
}

/* Function start: 0x415FC0 */
void start_lock(unsigned short v)
{
    g_bTargetLockReadoutDirty_0046c060 = 0;
    g_nTargetLockCountdown_0046c064 = v;
    g_nTargetLockMarkerAngle_0046c068 =
        (short)RandomBelowOrEqual(0x167);
}

/* Function start: 0x415FF0 */
unsigned short starting_lock(unsigned short v)
{
    if (g_nTargetLockCountdown_0046c064 == -1) {
        start_lock(v);
        return 1;
    }
    return 0;
}

/* Function start: 0x416010 */
void lock_off(void)
{
    if (g_nTargetLockCountdown_0046c064 > -1)
        g_bTargetLockReadoutDirty_0046c060 = 1;
    remove_message(PTR_s_MISSILE_LOCKED_004691d4);
    g_nTargetLockCountdown_0046c064 = -1;
}

/* Function start: 0x416040 */
short CheckTargetLockMalfunction(void)
{
    short countdown;

    if (malf(5) != 0) {
        countdown = -10;
        lock_off();
        countdown = (short)(countdown - RandomBelowOrEqual(30));
        g_nTargetLockCountdown_0046c064 = countdown;
        PlaySfxWaveFileByNumber(7, -1, 0);
        return 1;
    }
    return 0;
}

/* Function start: 0x416090 */
short decrement_lock_time(short screenX)
{
    (void)screenX;
    if (g_nTargetLockCountdown_0046c064 > 0) {
        if (malf(5) == 0) {
            g_nTargetLockCountdown_0046c064--;
            g_bTargetLockAcquired_0046c074 =
                g_nTargetLockCountdown_0046c064 == 0;
            if (g_bTargetLockAcquired_0046c074 != 0) {
                if (CheckTargetLockMalfunction() == 0)
                    PlaySfxWaveFileByNumber(0x16, -1, 0);
                CockpitMessage(PTR_s_MISSILE_LOCKED_004691d4,
                               DAT_004699ac, 2);
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
    short y;
    short x;

    if (target != -1 &&
        g_aeShipSide_0059d650[(short)target] != g_aeShipSide_0059d650[0] &&
        g_acPlayerComponentDamage_0059bff0[5] < 4) {
        x = g_asObjectScreenX_0059d9b0[(short)target];
        if (x == -0x7fff)
            return;
        y = g_asObjectScreenY_0059d930[(short)target];
        if (g_nTargetLockCountdown_0046c064 < -1) {
            g_nTargetLockCountdown_0046c064++;
            return;
        }
        if (x * x + y * y > 0xe10) {
            lock_off();
            return;
        }
        weaponType = *(enum ObjectType *)(
            &g_aShipWeapons_0059cab0[0][1] +
            g_nSelectedReleaseWeaponIndex_0046c058 * 7);
        if (weaponType != OBJECT_TYPE_HEAT_SEEKING_MISSILE) {
            if (weaponType != OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE) {
                lock_off();
                return;
            }
            goto image_recognition_lock;
        }

        get_facing_range_from_object(0, (short)target);
        if (g_nTargetFacing_0059d52a > -0x41) {
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
    wrapped[0] = (char)DAT_004693b0;
    viewport = *DAT_005a6bc0.viewport;
    context = DAT_005a6bc0;
    context.viewport = &viewport;

    SetRectBounds(&viewport,
                  (unsigned short)DAT_004691e0[
                      (int)g_cCockpitView_0059dab0 * 2],
                  (unsigned short)DAT_004691e0[
                      (int)g_cCockpitView_0059dab0 * 2 + 1],
                  (unsigned short)(
                      319 - DAT_004691e0[
                                (int)g_cCockpitView_0059dab0 * 2]),
                  (unsigned short)(
                      DAT_004691e0[
                          (int)g_cCockpitView_0059dab0 * 2 + 1] + 60));
    context.backgroundColour = 0xff;
    context.colour = colour;
    input = source;
    output = wrapped;
    width = GetRectHeight(&viewport);
    charactersPerLine = (short)(width / 6);
    position = 0;
    if (*input != 0) {
        do {
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
            position++;
            input++;
            output++;
        } while (*input != 0);
    }
    *output = 0;

    {
        int view;

        view = (int)g_cCockpitView_0059dab0;
        if (DAT_0046a008 != 0) {
            switch (view) {
            case 0:
                DrawTextAt(&context, DAT_004691e0[view * 2],
                           (short)(DAT_004691e0[view * 2 + 1] + 10),
                           wrapped, 2);
                break;
            case 1:
                DrawTextAt(&context, DAT_004691e0[view * 2],
                           (short)(DAT_004691e0[view * 2 + 1] + 25),
                           wrapped, 2);
                break;
            case 2:
                DrawTextAt(&context, DAT_004691e0[view * 2],
                           (short)(DAT_004691e0[view * 2 + 1] + 50),
                           wrapped, 2);
                break;
            case 3:
                DrawTextAt(&context, DAT_004691e0[view * 2],
                           DAT_004691e0[view * 2 + 1], wrapped, 2);
                break;
            default:
                goto message_finished;
            }
        } else {
            DrawTextAt(&context, DAT_004691e0[view * 2],
                       DAT_004691e0[view * 2 + 1], wrapped, 2);
        }
    }
message_finished:
    DAT_00469008 = DAT_00469004;
}

/* Function start: 0x416460 */
void ShowHudTextLine(char *s, unsigned short b)
{
    DAT_00469004 = s;
    print_message_text(s, b);
}

/* Function start: 0x416480 */
void SetHudTextColour(short v)
{
    if (v != 0)
        EndCommMenu();
    print_message_text(DAT_00469008, (unsigned char)DAT_004699d8);
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
    if ((int)(unsigned char)DAT_004699d8 == colourValue) {
        valid = savedBounds->left != -0x7fff;
        bounds = *savedBounds;
    } else {
        if (object == -1) {
            valid = 0;
        } else {
            valid = 1;
            if (g_asObjectScreenX_0059d9b0[object] == -0x7fff)
                valid = 0;
        }
        if (valid != 0) {
            centerX = (short)(g_asObjectScreenX_0059d9b0[object] +
                              g_nViewCenterX_0059a852);
            centerY = (short)(g_asObjectScreenY_0059d930[object] +
                              g_nViewCenterY_0059a854);
            if ((short)GetTransformedShapeBounds(
                    &DAT_005a7510, centerX, centerY,
                    g_apObjectShape_0059d2f0[object],
                    g_asObjectViewFrame_0059d230[object],
                    g_asObjectScreenAngle_0059cd90[object],
                    g_asObjectScreenScale_0059c950[object],
                    g_asObjectFlip_0059c870[object],
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
        if ((int)(unsigned char)DAT_004699ac == colourValue &&
            g_aeShipSide_0059d650[object] == g_aeShipSide_0059d650[0]) {
            colour = (unsigned char)DAT_004699a4;
        }
        if (solid != 0) {
            DrawViewportBorder(&DAT_005a7510, bounds.left, bounds.top,
                               bounds.right, bounds.bottom, colour);
        } else {
            segmentLength =
                (short)(((int)bounds.right - bounds.left) / 6 + 1);
            DrawViewportLine(&DAT_005a7510, bounds.left, bounds.top,
                             (short)(bounds.left + segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&DAT_005a7510, bounds.left, bounds.bottom,
                             (short)(bounds.left + segmentLength), bounds.bottom,
                             colour);
            DrawViewportLine(&DAT_005a7510, bounds.right, bounds.top,
                             (short)(bounds.right - segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&DAT_005a7510, bounds.right, bounds.bottom,
                             (short)(bounds.right - segmentLength), bounds.bottom,
                             colour);
            segmentLength =
                (short)(((int)bounds.bottom - bounds.top) / 6 + 1);
            DrawViewportLine(&DAT_005a7510, bounds.left, bounds.top,
                             bounds.left, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&DAT_005a7510, bounds.left, bounds.bottom,
                             bounds.left,
                             (short)(bounds.bottom - segmentLength), colour);
            DrawViewportLine(&DAT_005a7510, bounds.right, bounds.top,
                             bounds.right, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&DAT_005a7510, bounds.right, bounds.bottom,
                             bounds.right,
                             (short)(bounds.bottom - segmentLength), colour);
        }
        if (drawLockMarker != 0) {
            if ((int)(short)colour != (int)(unsigned char)DAT_004699d8) {
                if (g_nTargetLockCountdown_0046c064 > -1) {
                    g_nTargetLockMarkerAngle_0046c068 = (short)(
                        g_nTargetLockMarkerAngle_0046c068 +
                        g_anObjectRollRotation_0059d7e0[0] +
                        g_anObjectPitchRotation_0059b2a0[0]);
                    centerX = (short)(centerX +
                        ((CosFixed(g_nTargetLockMarkerAngle_0046c068) *
                          g_nTargetLockCountdown_0046c064 * 2) >> 8));
                    centerY = (short)(centerY +
                        ((SinFixed(g_nTargetLockMarkerAngle_0046c068) *
                          g_nTargetLockCountdown_0046c064 * 2) >> 8));
                    DrawSpriteDefault(&DAT_005a7510, centerX, centerY,
                                      g_pTargetLockShape_005a6bf4, 1);
                    g_nTargetLockMarkerX_004691f4 = centerX;
                    g_nTargetLockMarkerY_005a7e28 = centerY;
                }
            } else if (g_nTargetLockMarkerX_004691f4 != -0x7fff) {
                DrawSolidColourSprite(&DAT_005a7510,
                                      g_nTargetLockMarkerX_004691f4,
                                      g_nTargetLockMarkerY_005a7e28,
                                      g_pTargetLockShape_005a6bf4, 1,
                                      DAT_004699d8);
                g_nTargetLockMarkerX_004691f4 = -0x7fff;
            }
        }
        if ((int)(unsigned char)DAT_004699d8 == (int)(short)colour) {
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
    if (DAT_00469208 != -1)
        remove_object(DAT_00469208);
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

    if ((short)get_mode(1) == 5 &&
        g_nCannedSceneMode_00469fac != 4 &&
        (DAT_0046c03c == 0 || DAT_0046c03c == 4))
        active = 1;
    else
        active = 0;
    if (active == 0) {
        remove_nav_pointer();
        return;
    }
    object = DAT_00469208;
    if (object == -1) {
        object = find_vacant_3d_object();
        DAT_00469208 = object;
        if (object == -1)
            return;
        g_asObjectViewFrame_0059d230[object] = 3;
        g_acObjectOwner_0059ce20[object] = -1;
        g_asObjectScreenAngle_0059cd90[object] = 0;
        g_asObjectScreenScale_0059c950[object] = 0x100;
        g_aeObjectClass_0059d100[object] = OBJECT_CLASS_PLANET;
        g_apObjectShape_0059d2f0[object] =
            g_pTargetLockShape_005a6bf4;
        DAT_00469208 = object;
        g_asObjectScreenX_0059d9b0[object] = (short)0x8001;
        g_asObjectDistance_0059b4a0[object] = 0;
    }
    objectivePosition = g_aMissionObjectives_0059dac0[
        (signed char)g_cCurrentObjective_0046c020].position;
    ComputeVectorDelta(&g_aShipPosition_0059c490[WC1_EYE_OBJECT],
                       &objectivePosition, &direction);
    distance = (int)Vector_magnitude(&direction);
    if (g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] * 0x100 >=
        distance)
        return;
    transform_to_objects_frame(&direction, &viewPosition,
                               WC1_EYE_OBJECT);
    if (g_asObjectCollisionRadius_0059d710[WC1_EYE_OBJECT] * 0x100 >
        viewPosition.z)
        return;
    if (DivideFixed(viewPosition.z, distance) < 0x94)
        return;
    g_asObjectScreenX_0059d9b0[object] = (short)(DivideFixed(
        (int)MultiplyFixed(
            ((short)g_nScreenWidth_0046daa4 & ~1) << 7,
            viewPosition.x),
        viewPosition.z) >> 8);
    g_asObjectScreenY_0059d930[object] = (short)(DivideFixed(
        (int)MultiplyFixed(
            ((short)g_nScreenWidth_0046daa4 & ~1) << 7,
            viewPosition.y),
        viewPosition.z) >> 8);
    g_asObjectDistance_0059b4a0[object] = 0x4a38;
}

/* Function start: 0x416AC0 */
unsigned int overlay_head_up_display(void)
{
    target_locking(g_acShipTarget_0059ce60[0]);
    if (message_showing() && g_nCommSpeakerObject_0046afc8 != -1) {
        g_cPreviousTargetObject_005a7df2 =
            (signed char)g_nCommSpeakerObject_0046afc8;
        draw_target_box(DAT_004699a8,
                        g_cPreviousTargetObject_005a7df2,
                        0, 0, 2,
                        &g_stPreviousTargetBracketBounds_00469200);
    }
    if (g_nTargetLockCountdown_0046c064 == 0) {
        if ((short)(g_nRenderedSpaceFrame_0059d61a % 2) == 0)
            g_bTargetBracketVisible_004691d8 ^= 1;
        if (g_bTargetBracketVisible_004691d8 == 1) {
            draw_target_box(DAT_004699ac,
                            g_acShipTarget_0059ce60[0],
                            g_nTargetLockMode_0046c078,
                            1, 1,
                            &g_stTargetBracketBounds_004691f8);
        }
    } else {
        draw_target_box(DAT_004699ac,
                        g_acShipTarget_0059ce60[0],
                        g_nTargetLockMode_0046c078,
                        1, 1,
                        &g_stTargetBracketBounds_004691f8);
    }

    if (DAT_0046a008 != 0) {
        switch (g_cCockpitView_0059dab0) {
        case 0:
        case 2:
            goto centered_sight;
        case 1:
            DrawSpriteDefault(&DAT_005a7510,
                              g_nViewCenterX_0059a852,
                              (short)(g_nViewCenterY_0059a854 - 1),
                              g_pTargetLockShape_005a6bf4, 0);
            break;
        case 3:
            DrawSpriteDefault(&DAT_005a7510,
                              g_nViewCenterX_0059a852,
                              (short)(g_nViewCenterY_0059a854 + 14),
                              g_pTargetLockShape_005a6bf4, 0);
            break;
        default:
            goto no_sight;
        }
        goto no_sight;
    }
centered_sight:
    DrawSpriteDefault(&DAT_005a7510,
                      g_nViewCenterX_0059a852,
                      g_nViewCenterY_0059a854,
                      g_pTargetLockShape_005a6bf4, 0);

no_sight:
    DAT_0046c05c = 0;
    if (DAT_00469004 != 0)
        ShowHudTextLine(DAT_00469004, (unsigned char)DAT_005a7f00);
    if (g_bMouseCursorVisible_0046a018 == 1) {
        g_nSavedMouseCursorX_005a7df8 = g_nMouseX_0059ab10;
        g_nSavedMouseCursorY_005a7df4 = g_nMouseY_0059ab12;
        CaptureSpriteBackground(DAT_0059ab23,
                                g_abMouseCursorBackground_00475ff0,
                                g_nMouseX_0059ab10,
                                g_nMouseY_0059ab12,
                                DAT_0059ab19, DAT_0059ab1d);
        DrawSpriteDefault(DAT_0059ab23,
                          g_nMouseX_0059ab10,
                          g_nMouseY_0059ab12,
                          DAT_0059ab19, DAT_0059ab1d);
    }
    return 0;
}

/* Function start: 0x416C90 */
void RestoreCockpitExplosionIfVisible(void)
{
    if (IsCockpitExplosionActive() &&
        g_pCockpitExplosionBackground_00469060 != 0) {
        RestoreCockpitExplosionBackground();
    }
}

/* Function start: 0x416CB0 */
unsigned int RestoreTransientCockpitGraphics(void)
{
    if (g_bMouseCursorVisible_0046a018 == 1) {
        RestoreSpriteBackground(DAT_0059ab23,
                                g_abMouseCursorBackground_00475ff0,
                                (short)g_nSavedMouseCursorX_005a7df8,
                                (short)g_nSavedMouseCursorY_005a7df4,
                                DAT_0059ab19, (short)DAT_0059ab1d);
    }
    if (g_cPreviousTargetObject_005a7df2 != -1) {
        draw_target_box(DAT_004699d8,
                        g_cPreviousTargetObject_005a7df2,
                        0, 0, 2,
                        &g_stPreviousTargetBracketBounds_00469200);
        g_cPreviousTargetObject_005a7df2 = -1;
    }
    draw_target_box(DAT_004699d8, g_acShipTarget_0059ce60[0],
                    g_nTargetLockMode_0046c078, 1, 1,
                    &g_stTargetBracketBounds_004691f8);
    if (DAT_00469008 != DAT_00469004 && DAT_00469008 != 0)
        SetHudTextColour(0);
    if (IsCockpitExplosionActive() &&
        g_pCockpitExplosionBackground_00469060 != 0) {
        if (DAT_0046a008 == 0) {
            CaptureSpriteBackground(
                &DAT_005a6ba0, g_pCockpitExplosionBackground_00469060,
                g_nCockpitExplosionX_005a7e98,
                g_nCockpitExplosionY_005a7e9a,
                g_pCockpitExplosionShape_00469064,
                g_nCockpitExplosionFrame_00469068);
        }
        if (DAT_0046a008 == 0) {
            DrawSpriteDefault(&DAT_005a6ba0,
                              g_nCockpitExplosionX_005a7e98,
                              g_nCockpitExplosionY_005a7e9a,
                              g_pCockpitExplosionShape_00469064,
                              g_nCockpitExplosionFrame_00469068);
        }
        DAT_0046900c = 0xff;
    }
    return 0;
}

/* Function start: 0x416DE0 */
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration)
{
    if (g_bInflightComputerActive_00468754 == 0) {
        if (message_showing())
            SetHudTextColour(1);
        DAT_005a7f00 = colour;
        DAT_00469004 = text;
        set_message_time(duration);
    }
}

/* Function start: 0x416E20 */
void malf_noise(short vdu, int effect, unsigned short colour,
                short sound, short refresh)
{
    Viewport *viewport = vdu == 0 ? &DAT_005a6b80 : &DAT_005a7530;

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
    g_cViableTargetCount_0046c088 = 0;
    object = 1;
    do {
        if (g_aeObjectClass_0059d100[(int)object] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_0059c3c0[(int)object] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asObjectScreenX_0059d9b0[(int)object] != (short)0x8001 &&
            (unsigned short)g_asObjectDistance_0059b4a0[(int)object] <
                12000) {
            targetIndex = g_cViableTargetCount_0046c088;
            g_asViableTargetDistance_0059c470[(int)targetIndex] =
                g_asObjectDistance_0059b4a0[(int)object];
            g_acViableTarget_0059c920[(int)targetIndex] = object;
            g_cViableTargetCount_0046c088++;
            if (g_aeShipSide_0059d650[(int)object] !=
                g_aeShipSide_0059d650[0])
                *hasEnemy = 1;
        }
        object++;
    } while (object <= 9);
    if (g_cViableTargetCount_0046c088 > 1)
        sort_viable_target_list();
}

/* Function start: 0x416F30 */
void cycle_onscreen_targets(void)
{
    signed char previousTarget;
    signed char index;
    short hasEnemy;

    previousTarget = g_acShipTarget_0059ce60[0];
    build_your_target_list(&hasEnemy);
    if (g_cViableTargetCount_0046c088 == 0) {
        g_acShipTarget_0059ce60[0] = -1;
    } else {
        index = 0;
        while (index < g_cViableTargetCount_0046c088 &&
               g_acViableTarget_0059c920[(int)index] !=
                   g_acShipTarget_0059ce60[0])
            index++;
        do {
            index = (signed char)((index + 1) %
                                  g_cViableTargetCount_0046c088);
            g_acShipTarget_0059ce60[0] =
                g_acViableTarget_0059c920[(int)index];
            if (hasEnemy == 0)
                break;
        } while (g_aeShipSide_0059d650[
                     (int)g_acShipTarget_0059ce60[0]] ==
                 g_aeShipSide_0059d650[0]);
    }
    if (g_acShipTarget_0059ce60[0] != previousTarget)
        g_nTargetLockCountdown_0046c064 = -1;
}

/* Function start: 0x416FD0 */
void check_target(void)
{
    short selectNewTarget;
    short oldTarget;
    short targetIndex;
    short hasEnemy;

    selectNewTarget = 1;
    oldTarget = (short)g_acShipTarget_0059ce60[0];
    if (oldTarget != -1 &&
        g_aeSpecialManeuver_0059c3c0[oldTarget] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        g_acShipTarget_0059ce60[0] = -1;
        oldTarget = -1;
    }
    if (g_nTargetLockMode_0046c078 != 0 &&
        (short)(g_nRenderedSpaceFrame_0059d61a % 8) == 0 &&
        malf(5) != 0) {
        g_nTargetLockMode_0046c078 = 0;
        malf_sound();
    }
    if (oldTarget != -1 &&
        (g_nTargetLockMode_0046c078 != 0 ||
         (g_asObjectScreenX_0059d9b0[oldTarget] != (short)0x8001 &&
          (g_nTargetLockMode_0046c078 != 0 ||
           g_aeShipSide_0059d650[oldTarget] !=
               g_aeShipSide_0059d650[0]))))
        return;
    if (oldTarget == -1)
        g_nTargetLockMode_0046c078 = 0;

    build_your_target_list(&hasEnemy);
    if (g_cViableTargetCount_0046c088 == 0) {
        if (g_nTargetLockMode_0046c078 != 0)
            g_acShipTarget_0059ce60[0] = (signed char)oldTarget;
        else
            g_acShipTarget_0059ce60[0] = -1;
    } else {
        if (hasEnemy == 0 && oldTarget != -1 &&
            g_aeShipSide_0059d650[oldTarget] ==
                g_aeShipSide_0059d650[0] &&
            (g_nTargetLockMode_0046c078 != 0 ||
             g_asObjectScreenX_0059d9b0[oldTarget] != (short)0x8001)) {
            selectNewTarget = 0;
            g_acShipTarget_0059ce60[0] = (signed char)oldTarget;
        }
        if (selectNewTarget != 0) {
            targetIndex = 0;
            while (targetIndex < g_cViableTargetCount_0046c088 &&
                   g_aeShipSide_0059d650[
                       (short)g_acViableTarget_0059c920[targetIndex]] ==
                       g_aeShipSide_0059d650[0])
                targetIndex++;
            g_acShipTarget_0059ce60[0] =
                g_acViableTarget_0059c920[
                    targetIndex % (short)g_cViableTargetCount_0046c088];
        }
    }
    if ((short)g_acShipTarget_0059ce60[0] != oldTarget) {
        if (oldTarget != -1 && g_acShipTarget_0059ce60[0] == -1)
            g_nTargetLockMode_0046c078 = 0;
        g_nTargetLockCountdown_0046c064 = -1;
    }
}

/* Function start: 0x417190 */
void update_missile_warning(void)
{
    if (missile_on_tail(0) != 0) {
        SetCockpitLightBlink(2, 1);
        if (g_nTrainSimActive_00469e2c == 0)
            spacetrack(3, 1, -1);
    } else {
        g_abCockpitLightGoal_005a7eb8[2] = 0;
    }
}

/* Function start: 0x4171D0 */
void determine_pilot_hand(void)
{
    short yaw;
    short pitch;

    yaw = g_nYawInput_0059d3f2 / 2;
    pitch = g_nPitchInput_0059d3f0 / 2;
    if (yaw > 0) {
        g_bStickIndicatorFrame_005a7dc8 =
            (unsigned char)MinShort((short)(yaw + 8), 12);
        return;
    }
    if (yaw < 0) {
        g_bStickIndicatorFrame_005a7dc8 =
            (unsigned char)MinShort((short)(4 - yaw), 8);
        return;
    }
    if (pitch > 0) {
        g_bStickIndicatorFrame_005a7dc8 =
            (unsigned char)MinShort((short)(pitch + 12), 16);
        return;
    }
    if (pitch < 0) {
        g_bStickIndicatorFrame_005a7dc8 =
            (unsigned char)MinShort((short)-pitch, 4);
        return;
    }
    g_bStickIndicatorFrame_005a7dc8 = 0;
}

/* Function start: 0x417260 */
void DrawPilotHandFrame(void)
{
    int view = (int)g_cCockpitView_0059dab0;
    short x = (short)(g_asPilotHandOrigins_0046e120[view * 2] -
                      DAT_005a6b60.left);
    short y = (short)(g_asPilotHandOrigins_0046e120[view * 2 + 1] -
                      DAT_005a6b60.top);

    CopyViewportContents(&DAT_005a7550, &DAT_005a7690);
    DrawSpriteDefault(&DAT_005a7690, x, y, g_pPilotHandShape_005a7684,
                      (short)(signed char)g_bStickIndicatorFrame_005a7dc8);
    DrawSpriteDefault(
        &DAT_005a7690,
        (short)(x + g_asPilotHandOffsets_00469018[
            (signed char)g_bStickIndicatorFrame_005a7dc8 * 2]),
        (short)(y + g_asPilotHandOffsets_00469018[
            (signed char)g_bStickIndicatorFrame_005a7dc8 * 2 + 1]),
        g_pPilotHandShape_005a7684, 0x11);
    CopyViewportContents(&DAT_005a7690, &DAT_005a6b60);
    DAT_0046900c = g_bStickIndicatorFrame_005a7dc8;
}

/* Function start: 0x417320 */
void CopyTrainSimPilotViewToRightVdu(void)
{
    Viewport destination;

    destination = DAT_005a7550;
    if (g_stTrainSimVduSource_00469210.left == 0) {
        g_stTrainSimVduSource_00469210 = DAT_005a6b60;
        g_stTrainSimVduSource_00469210.left = DAT_005a7530.left;
        g_stTrainSimVduSource_00469210.top = DAT_005a6b60.top;
        g_stTrainSimVduSource_00469210.right = DAT_005a6b60.right;
        g_stTrainSimVduSource_00469210.bottom = DAT_005a7530.bottom;
    }
    destination.left = (short)(g_stTrainSimVduSource_00469210.left -
                               DAT_005a6b60.left);
    destination.bottom = (short)(g_stTrainSimVduSource_00469210.bottom -
                                 DAT_005a6b60.top);
    CopyViewportContents(&g_stTrainSimVduSource_00469210, &destination);
}

/* Function start: 0x4173C0 */
void animate_pilot(void)
{
    if (g_pPilotHandShape_005a7684 != 0) {
        determine_pilot_hand();
        if (DAT_0046900c != g_bStickIndicatorFrame_005a7dc8)
            DrawPilotHandFrame();
    }
}

/* Function start: 0x4173F0 */
void ResetPilotHandAnimation(void)
{
    if (g_pPilotHandShape_005a7684 != 0) {
        DAT_0046900c = 0xff;
        CopyViewportContents(&DAT_005a6b60, &DAT_005a7550);
        animate_pilot();
    }
}

/* Function start: 0x417420 */
void send_message(short obj, signed char message)
{
    if (g_nTrainSimActive_00469e2c == 0 &&
        g_aeObjectClass_0059d100[obj] != OBJECT_CLASS_NULL &&
        g_nCannedSceneMode_00469fac == 0) {
        if (g_nYourWingman_0046c04c != -1 &&
            g_nYourWingman_0046c04c == obj &&
            g_bRadioSilence_0046af70 != 0) {
            g_acWingmanMessageState_0059d2c0[obj] = -1;
            return;
        }
        if (obj >= 0 && obj < 10 &&
            g_aeObjectClass_0059d100[obj] >= OBJECT_CLASS_SHIP) {
            if (g_acShipRating_0059cd80[obj] != -1) {
                g_acWingmanMessageState_0059d2c0[obj] = message;
                return;
            }
            if (g_aeObjectType_0059b560[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW ||
                g_nShipMissionIndices_0059c830[obj] ==
                    g_anShipMissionShip_0059d4b0[0]) {
                g_acWingmanMessageState_0059d2c0[obj] = message;
            } else if (g_aeShipSide_0059d650[obj] == SIDE_KILRATHI) {
                g_acWingmanMessageState_0059d2c0[obj] = message;
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

    if (g_nCannedSceneMode_00469fac == 0 &&
        g_nTrainSimActive_00469e2c == 0) {
        messageActive = (short)message_showing();
        obj = 1;
        while (messageActive == 0 && obj < 10) {
            if (g_aeObjectClass_0059d100[(short)obj] >=
                    OBJECT_CLASS_SHIP &&
                g_acWingmanMessageState_0059d2c0[(short)obj] != -1) {
                message = g_acWingmanMessageState_0059d2c0[(short)obj];
                vid_equiv((short)obj, (short)message);
                g_acWingmanMessageState_0059d2c0[(short)obj] = -1;
            }
            messageActive = (short)message_showing();
            obj++;
        }
        if ((short)RandomBelowOrEqual(5000) > 4998 &&
            g_nCommSpeakerObject_0046afc8 == -1) {
            obj = 1;
            while (obj < 10) {
                if (g_aeObjectClass_0059d100[(short)obj] >=
                        OBJECT_CLASS_SHIP &&
                    g_aeShipSide_0059d650[(short)obj] == SIDE_KILRATHI &&
                    (g_aeShipObjective_0059d200[(short)obj] ==
                         OBJECTIVE_ENGAGE_ENEMY ||
                     g_aeShipObjective_0059d200[(short)obj] ==
                         OBJECTIVE_DESTROY_SHIP) &&
                    (g_acShipRating_0059cd80[(short)obj] != -1 ||
                     (short)RandomBelowOrEqual(100) < 20)) {
                    g_acWingmanMessageState_0059d2c0[(short)obj] =
                        (signed char)(RandomBelowOrEqual(2) + 2);
                    return;
                }
                obj++;
                if (g_nCommSpeakerObject_0046afc8 != -1)
                    return;
            }
        }
    }
}

/* Function start: 0x417610 */
void clear_cockpit_damage(void)
{
    memset(g_anCockpitDamageState_005a7ef0, 0,
           sizeof(g_anCockpitDamageState_005a7ef0));
}

/* Function start: 0x417630 */
void explosion_draw(void)
{
    short damage;

    if (g_pCockpitPilotShape_0046905c == 0)
        g_pCockpitPilotShape_0046905c =
            (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 4, 0);
    damage = 0;
    do {
        if (g_anCockpitDamageState_005a7ef0[damage] == 1) {
            DrawSpriteDefault(
                &DAT_005a6ba0,
                g_aaCockpitDamagePositions_00469228[
                    (int)g_cCockpitView_0059dab0][damage].x,
                g_aaCockpitDamagePositions_00469228[
                    (int)g_cCockpitView_0059dab0][damage].y,
                g_pCockpitPilotShape_0046905c, damage);
        }
        damage++;
    } while (damage < 4);
    FreePacketAndClear((int *)&g_pCockpitPilotShape_0046905c, 0);
}

/* Function start: 0x4176C0 */
unsigned int DrawPendingCockpitDamage(void)
{
    if (g_pCockpitPilotShape_0046905c == 0)
        g_pCockpitPilotShape_0046905c =
            (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 4, 0);
    DrawSpriteDefault(&DAT_005a6ba0,
                      g_nCockpitExplosionX_005a7e98,
                      g_nCockpitExplosionY_005a7e9a,
                      g_pCockpitPilotShape_0046905c,
                      g_nPendingCockpitDamage_005a7dcc);
    if (g_pPilotHandShape_005a7684 != 0) {
        DrawSpriteDefault(&DAT_005a7550,
                          (short)(g_nCockpitExplosionX_005a7e98 -
                                  DAT_005a6b60.left),
                          (short)(g_nCockpitExplosionY_005a7e9a -
                                  DAT_005a6b60.top),
                          g_pCockpitPilotShape_0046905c,
                          g_nPendingCockpitDamage_005a7dcc);
    }
    FreePacketAndClear((int *)&g_pCockpitPilotShape_0046905c, 0);
    return 0;
}

/* Function start: 0x417760 */
void RestoreCockpitExplosionBackground(void)
{
    if (IsCockpitExplosionActive() &&
        g_pCockpitExplosionShape_00469064 != 0 &&
        g_pCockpitExplosionBackground_00469060 != 0) {
        RestoreSpriteBackground(
            &DAT_005a6ba0, g_pCockpitExplosionBackground_00469060,
            g_nCockpitExplosionX_005a7e98,
            g_nCockpitExplosionY_005a7e9a,
            g_pCockpitExplosionShape_00469064,
            g_nCockpitExplosionFrame_00469068);
    }
}

/* Function start: 0x4177B0 */
void cockpit_explosion(void)
{
    short frame;

    if (g_nCockpitExplosionFrame_00469068 == 0x7fff)
        g_nCockpitExplosionFrame_00469068 = 0;
    if (IsCockpitExplosionActive()) {
        frame = g_nCockpitExplosionFrame_00469068;
        if (frame == 0)
            PlaySfxWaveFileByNumber(0x1b, -1, 0);
        if (++g_nCockpitExplosionFrame_00469068 == 3)
            DrawPendingCockpitDamage();
        if (IsCockpitExplosionActive() &&
            g_pCockpitExplosionShape_00469064 != 0 &&
            g_pCockpitExplosionBackground_00469060 != 0) {
            CaptureSpriteBackground(
                &DAT_005a6ba0, g_pCockpitExplosionBackground_00469060,
                g_nCockpitExplosionX_005a7e98,
                g_nCockpitExplosionY_005a7e9a,
                g_pCockpitExplosionShape_00469064,
                g_nCockpitExplosionFrame_00469068);
            DrawSpriteDefault(&DAT_005a6ba0,
                              g_nCockpitExplosionX_005a7e98,
                              g_nCockpitExplosionY_005a7e9a,
                              g_pCockpitExplosionShape_00469064,
                              g_nCockpitExplosionFrame_00469068);
            DAT_0046900c = 0xff;
        }
        return;
    }
    FreePacketAndClear((int *)&g_pCockpitExplosionShape_00469064, 0);
}

/* Function start: 0x4178A0 */
void place_damage_on_cockpit(short damage)
{
    if (DAT_0046c03c == 0 && g_nTrainSimActive_00469e2c == 0 &&
        g_anCockpitDamageState_005a7ef0[damage] == 0) {
        g_nPendingCockpitDamage_005a7dcc = damage;
        g_anCockpitDamageState_005a7ef0[damage] = 1;
        if (g_pCockpitExplosionShape_00469064 == 0) {
            explosion_draw();
            return;
        }
        if (IsCockpitExplosionActive() == 0) {
            g_nCockpitExplosionFrame_00469068 = 0x7fff;
            *(unsigned int *)(void *)&g_nCockpitExplosionX_005a7e98 =
                *(const unsigned int *)(const void *)
                    &g_aaCockpitDamagePositions_00469228[
                        (signed char)g_cCockpitView_0059dab0][damage];
        }
    }
}

/* Function start: 0x417910 */
void vid_transmit(void)
{
    short randomFrame;
    short speaker;
    unsigned char *background;

    speaker = g_nCommSpeakerObject_0046afc8;
    if (g_aeShipSide_0059d650[speaker] == SIDE_NEUTRAL) {
        EndCommSessionWithWingman();
        return;
    }
    if ((DAT_0046a008 != 0 ||
         g_nRenderedSpaceFrame_0059d61a % 2 != 0) &&
        g_nCommPortraitIndex_0046afd0 != -1 &&
        g_aapszPilotSpeech_0059e220[g_nCommPortraitIndex_0046afd0] != 0 &&
        g_bVideoImagesSuppressed_0046af74 == 0) {
        if (g_aeSpecialManeuver_0059c3c0[speaker] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
            if (g_aeShipSide_0059d650[speaker] == SIDE_IMPERIAL) {
                DrawSpriteDefault(
                    &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
                    g_pCommStaticShape_0046927c,
                    (short)(g_asObjectCounter_0059c330[speaker] / 5));
                return;
            }
            DrawSpriteDefault(&DAT_005a7530, DAT_005a7530.left,
                              DAT_005a7530.top,
                              g_pCommStaticShape_0046927c, 2);
            return;
        }
        if (g_nCommPortraitFrame_00469284 == -1) {
            g_nCommPortraitFrame_00469284 =
                (unsigned short)RandomInRange(0, 2);
        }
        randomFrame = RandomInRange(0, 3);
        if (randomFrame < 3)
            g_nCommPortraitFrame_00469284 = randomFrame;
        set_new_vdu(1);
        if (g_aeShipSide_0059d650[g_nCommSpeakerObject_0046afc8] ==
            SIDE_IMPERIAL) {
            background = g_pConfedCommBackground_00469278;
        } else {
            background = g_pKilrathiCommBackground_00469280;
        }
        DrawSpriteDefault(&DAT_005a7530, DAT_005a7530.left,
                          DAT_005a7530.top, background, 0);
        DrawSpriteDefault(
            &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
            g_apCommPortraitShapes_0059e180[g_nCommPortraitIndex_0046afd0],
            (short)g_nCommPortraitFrame_00469284);
    }
}

/* Function start: 0x417AC0 */
void vid_equiv(short obj, short message)
{
    if ((short)get_mode(1) != 4 &&
        g_nTrainSimActive_00469e2c == 0 &&
        g_nCannedSceneMode_00469fac == 0 && DAT_0046c03c == 0 &&
        (short)message_showing() == 0)
        real_vid_transmit(obj, message);
}

/* Function start: 0x417B10 */
void update_dead_disp(short a)
{
    malf_noise(a, 1, DAT_004699b0, 0x17, 0);
}

/* Function start: 0x417B30 */
void check_stranded(void)
{
    if (g_nTrainSimActive_00469e2c == 0 &&
        g_aMissionShips_0046c948[
            g_nCarrierMissionShipIndex_005a7e2a].state == 3 &&
        any_enemy(0, 30000) == 0)
        g_nArcadeState_00469fb0 = 3;
}

/* Function start: 0x417B70 */
void update_VDUs(void)
{
    short changed;

    SetTextContext(&DAT_005a74f0);
    if (DAT_0046a008 != 0) {
        DrawFilledViewportRect(
            &DAT_005a6ba0,
            g_stCockpitLayout_0046e000.leftVduBounds[
                (int)g_cCockpitView_0059dab0].left,
            g_stCockpitLayout_0046e000.leftVduBounds[
                (int)g_cCockpitView_0059dab0].top,
            g_stCockpitLayout_0046e000.leftVduBounds[
                (int)g_cCockpitView_0059dab0].right,
            g_stCockpitLayout_0046e000.leftVduBounds[
                (int)g_cCockpitView_0059dab0].bottom,
            0);
        DrawFilledViewportRect(
            &DAT_005a6ba0,
            g_stCockpitLayout_0046e000.rightVduBounds[
                (int)g_cCockpitView_0059dab0].left,
            g_stCockpitLayout_0046e000.rightVduBounds[
                (int)g_cCockpitView_0059dab0].top,
            g_stCockpitLayout_0046e000.rightVduBounds[
                (int)g_cCockpitView_0059dab0].right,
            g_stCockpitLayout_0046e000.rightVduBounds[
                (int)g_cCockpitView_0059dab0].bottom,
            0);
    }
    changed = update_vid_disp(0);
    if (changed != 0) {
        switch ((short)get_mode(0)) {
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
        switch ((short)get_mode(0)) {
        case 0:
            update_dead_disp(0);
            break;
        case 1:
            if (DAT_0046a008 != 0)
                show_weapon_disp();
            update_status_text();
            break;
        case 2:
            if (DAT_0046a008 != 0) {
                g_bForceDamageDisplayRedraw_00469288 = 1;
                show_damage_disp();
                g_bForceDamageDisplayRedraw_00469288 = 0;
            }
            UpdateDamageDisplay();
            break;
        case 8:
            show_info_disp();
            break;
        }
    }
    if ((short)get_mode(0) == 0)
        g_aHudMessageSlots_005a7dd0[0].text = 0;
    else
        UpdateMessage(&g_aHudMessageSlots_005a7dd0[0]);

    SetTextContext(&DAT_005a7700);
    changed = update_vid_disp(1);
    if (changed != 0) {
        switch ((short)get_mode(1)) {
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
        switch ((short)get_mode(1)) {
        case 0:
            update_dead_disp(1);
            break;
        case 3:
            if (DAT_0046a008 != 0)
                show_target_disp();
            DrawTargetRangeReadout();
            break;
        case 4:
            if (DAT_0046a008 != 0)
                show_communications_disp();
            talk_equiv();
            break;
        case 5:
            if (DAT_0046a008 != 0)
                show_navigation_disp();
            check_objectives();
            break;
        case 6:
            vid_transmit();
            break;
        }
    }
    if ((short)get_mode(1) == 6 || (short)get_mode(1) == 0)
        g_aHudMessageSlots_005a7dd0[1].text = 0;
    else
        UpdateMessage(&g_aHudMessageSlots_005a7dd0[1]);
    if (g_nTrainSimActive_00469e2c != 0 &&
        g_pPilotHandShape_005a7684 != 0)
        CopyTrainSimPilotViewToRightVdu();
}

/* Function start: 0x417E70 */
void update_cockpit(void)
{
    check_target();
    repair_internal_damage();
    if (g_cMissionObjectiveCount_0059c46a != 0)
        update_objective_location(
            (short)(g_nSpaceFrame_0059b420 %
                    (short)g_cMissionObjectiveCount_0059c46a));
    if (DAT_0046c03c == 0) {
        if (DAT_0046a008 == 0)
            RestoreCockpitExplosionBackground();
        update_lights();
        update_missile_warning();
        draw_3d_scanner();
        update_digital_readouts();
        update_VDUs();
        if (DAT_0046a008 == 0)
            animate_pilot();
        update_bars();
        draw_cockpit_lights();
        if (DAT_0046a008 == 0)
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
        mode = (short)get_mode(vdu);
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

    if (DAT_0046c03c != 0)
        return;
    if ((short)malf(3) != 0 ||
        (mode == 4 && (short)malf(4) != 0)) {
        vdu_malf(vdu, 0x17);
        return;
    }
    PlayCockpitSelectionSfx(g_asVduSelectionSound_00469000[vdu]);
    changed = (short)get_mode(vdu) != mode;
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
        if (g_bCurrentKey_0046c014 == 0x22)
            select_new_gun();
        else
            select_new_release_weapon((enum ObjectType)-1);
        break;
    case 2:
        g_nDamageDisplayTicks_005a7786 = 0;
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
