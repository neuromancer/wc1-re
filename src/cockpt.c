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

#pragma function(strlen, strcpy)

short g_asVduSelectionSound_00469000[2] = { 0x7f, 0 };
char *g_pszPendingHudMessage_0049afec = 0;
char *g_pszDisplayedHudMessage_0049aff0 = 0;
unsigned char *g_pHudMessageFrameShape_0049b288 = 0;
unsigned char *g_pHudMessageBackground_0049b28c = 0;
short g_bCaptureHudMessageBackground_0049b290;
short g_nHudMessageBackgroundDepth_0049b294;
short g_nHudMessageTop_0049ae92;
short g_nCockpitMessageOffsetY_005c849c;
signed char g_cHudMessageView_005d1c37;
short g_bDisplayWingmanTargetData_0049347c;
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
int g_nDisplayedObjectiveRange_0049b078 = 40000;
char *g_pszObjectiveStatusMessage_0046908c =
    g_szObjectiveStatusMessage_0046928c;
short g_nScannerTargetObject_0049b07c = -1;
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
const char *g_pszMissileLocked_0049b280 = g_szMissileLocked_004692a8;
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
char *g_pszGameVersion_0049b528 = g_szGameVersion_0049b52c;
char g_szGameVersion_0049b52c[9] = "1.03F-95";

/* Function start: 0x420340 */
void EmitTextString(void (*writer)(int), const char *text)
{
    short character;

    character = (signed char)*text++;
    while (character != 0) {
        writer((short)character);
        character = (signed char)*text++;
    }
}

/* Function start: 0x420378 */
void FormatTextTokens(void (*writer)(int),
#ifdef WC1_SDL
                      const char *format, va_list *arguments)
#else
                      const char *format, va_list arguments)
#endif
{
#if 0
    short character;
    char number[64];
    char *text;

    if (g_pCurrentTextContext_005c8d1c == 0)
        return;
    character = (signed char)*format++;
    while (character != 0) {
        if (character != '%') {
            writer((short)character);
        } else {
            character = (signed char)*format++;
            switch (character) {
            case 'B':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->backgroundColour =
                    (unsigned char)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->backgroundColour =
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
                g_pCurrentTextContext_005c8d1c->colour =
                    (unsigned char)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->colour =
                    (unsigned char)*arguments++;
#endif
                break;
            case 'J':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->alignment =
                    (unsigned char)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->alignment =
                    (unsigned char)*arguments++;
#endif
                break;
            case 'P':
#if 0
                DrawTextString(g_pCurrentTextContext_005c8d1c->text);
#else
                if (g_bCinematicSpriteFontEnabled_005c82a7 == 0)
                    DrawTextString(g_pCurrentTextContext_005c8d1c->text);
                else
                    DrawWrappedCinematicText(
                        g_pCurrentTextContext_005c8d1c->text);
#endif
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
                g_pCurrentTextContext_005c8d1c->cursorX =
                    (short)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->cursorX =
                    (short)*arguments++;
#endif
                break;
            case 'Y':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->cursorY =
                    (short)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->cursorY =
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
#else
    short character;
    char number[64];

    if (g_pCurrentTextContext_005c8d1c == 0)
        return;
    character = (signed char)*format++;
    while (character != 0) {
        if (character == '%') {
            character = (signed char)*format++;
            switch (character) {
            case 'd':
#ifdef WC1_SDL
                EmitTextString(
                    writer,
                    _itoa((short)va_arg(*arguments, int), number, 10));
#else
                EmitTextString(writer,
                               _itoa(va_arg(arguments, short), number, 10));
#endif
                break;
            case 'u':
#ifdef WC1_SDL
                EmitTextString(
                    writer,
                    _ultoa((unsigned short)va_arg(*arguments, unsigned int),
                           number, 10));
#else
                EmitTextString(
                    writer,
                    _ultoa(va_arg(arguments, unsigned short), number, 10));
#endif
                break;
            case 'D':
#ifdef WC1_SDL
                EmitTextString(writer,
                               _ltoa(va_arg(*arguments, long), number, 10));
#else
                EmitTextString(writer,
                               _ltoa(va_arg(arguments, long), number, 10));
#endif
                break;
            case 'U':
#ifdef WC1_SDL
                EmitTextString(
                    writer,
                    _ultoa(va_arg(*arguments, unsigned long), number, 10));
#else
                EmitTextString(
                    writer,
                    _ultoa(va_arg(arguments, unsigned long), number, 10));
#endif
                break;
            case 'x':
#ifdef WC1_SDL
                EmitTextString(
                    writer,
                    _strupr(_ultoa(
                        (unsigned short)va_arg(*arguments, unsigned int),
                        number, 16)));
#else
                EmitTextString(
                    writer,
                    _strupr(_ultoa(va_arg(arguments, unsigned short),
                                   number, 16)));
#endif
                break;
            case 'c':
#ifdef WC1_SDL
                writer((short)va_arg(*arguments, int));
#else
                writer(va_arg(arguments, short));
#endif
                break;
            case 's':
#ifdef WC1_SDL
                EmitTextString(writer, va_arg(*arguments, char *));
#else
                EmitTextString(writer, va_arg(arguments, char *));
#endif
                break;
            case 'X':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->cursorX =
                    (short)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->cursorX =
                    va_arg(arguments, short);
#endif
                break;
            case 'Y':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->cursorY =
                    (short)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->cursorY =
                    va_arg(arguments, short);
#endif
                break;
            case 'B':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->backgroundColour =
                    (unsigned char)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->backgroundColour =
                    va_arg(arguments, unsigned char);
#endif
                break;
            case 'F':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->colour =
                    (unsigned char)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->colour =
                    va_arg(arguments, unsigned char);
#endif
                break;
            case 'P':
                if (g_bCinematicSpriteFontEnabled_005c82a7 == 0)
                    DrawTextString(g_pCurrentTextContext_005c8d1c->text);
                else
                    DrawWrappedCinematicText(
                        g_pCurrentTextContext_005c8d1c->text);
                break;
            case 'J':
#ifdef WC1_SDL
                g_pCurrentTextContext_005c8d1c->alignment =
                    (unsigned char)va_arg(*arguments, int);
#else
                g_pCurrentTextContext_005c8d1c->alignment =
                    va_arg(arguments, unsigned char);
#endif
                break;
            default:
                writer(character);
                break;
            }
        } else {
            writer(character);
        }
        character = (signed char)*format++;
    }
#endif
}

/* Function start: 0x42067F */
void DrawFormattedText(const char *format, ...)
{
#if 0
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    FormatTextTokens((void (*)(int))DrawTextCharacter,
                     format, &arguments);
    va_end(arguments);
#else
    FormatTextTokens((void (*)(int))DrawTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    if (g_pCurrentTextContext_005c8d1c->viewport->pixels ==
        g_stScreenViewport_005d21a0.pixels)
        DIBslam();
#else
    va_list arguments;

    va_start(arguments, format);
    if (g_bCinematicSpriteFontEnabled_005c82a7 == 0) {
        FormatTextTokens((void (*)(int))DrawTextCharacter, format,
#ifdef WC1_SDL
                         &arguments);
#else
                         arguments);
#endif
    } else {
        FormatTextTokens((void (*)(int))DrawCinematicFontCharacter,
                         format,
#ifdef WC1_SDL
                         &arguments);
#else
                         arguments);
#endif
    }
    va_end(arguments);
    if (g_pCurrentTextContext_005c8d1c->viewport->pixels ==
        g_stScreenViewport_005d21a0.pixels)
        MarkDibDirty();
#endif
}

/* Function start: WC2_UNMAPPED */
void FormatTextBufferFromStart(const char *format, ...)
{
    g_pCurrentTextContext_005c8d1c->textCursor =
        g_pCurrentTextContext_005c8d1c->text;
#ifdef WC1_SDL
    {
        va_list arguments;

        va_start(arguments, format);
        FormatTextTokens((void (*)(int))AppendTextCharacter,
                         format, &arguments);
        va_end(arguments);
    }
#else
    FormatTextTokens((void (*)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    if (g_pCurrentTextContext_005c8d1c->viewport->pixels ==
        g_stScreenViewport_005d21a0.pixels)
        DIBslam();
}

/* Function start: 0x4206F2 */
void AppendFormattedText(const char *format, ...)
{
#if 0
#ifdef WC1_SDL
    va_list arguments;

    va_start(arguments, format);
    FormatTextTokens((void (*)(int))AppendTextCharacter,
                     format, &arguments);
    va_end(arguments);
#else
    FormatTextTokens((void (*)(int))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
#endif
    if (g_pCurrentTextContext_005c8d1c->viewport->pixels ==
        g_stScreenViewport_005d21a0.pixels)
        DIBslam();
#else
    va_list arguments;

    va_start(arguments, format);
    if (g_pCurrentTextContext_005c8d1c != 0) {
        g_pCurrentTextContext_005c8d1c->textCursor =
            g_pCurrentTextContext_005c8d1c->text;
        FormatTextTokens((void (*)(int))AppendTextCharacter, format,
#ifdef WC1_SDL
                         &arguments);
#else
                         arguments);
#endif
        va_end(arguments);
        if (g_pCurrentTextContext_005c8d1c->viewport->pixels ==
            g_stScreenViewport_005d21a0.pixels)
            MarkDibDirty();
    }
#endif
}

/* Function start: 0x4380B0 */
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

/* Function start: 0x4380FD */
short IsCockpitExplosionActive(void)
{
    return g_nCockpitExplosionFrame_00469068 < 8;
}

/* Function start: 0x438129 */
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

/* Function start: 0x438194 */
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
    definition = &g_aaCockpitBars_0046dd88[
        (int)g_cCockpitView_0059dab0][(int)bar];
    length = definition->length;
#else
    index = (int)bar + (int)g_cCockpitView_0059dab0 * 8;
    length = g_aaCockpitBars_0046dd88[0][index].length;
#endif
    extent = (short)(((int)percent * (int)length) / 100);
#ifdef WC1_SDL
    left = definition->left;
#else
    left = g_aaCockpitBars_0046dd88[0][index].left;
#endif
    DAT_005a6be0.left = left;
    if (left != -99) {
#ifdef WC1_SDL
        right = definition->right;
        top = definition->top;
        bottom = definition->bottom;
#else
        right = g_aaCockpitBars_0046dd88[0][index].right;
        top = g_aaCockpitBars_0046dd88[0][index].top;
        bottom = g_aaCockpitBars_0046dd88[0][index].bottom;
#endif
        DAT_005a6be0.right = right;
        DAT_005a6be0.top = top;
        DAT_005a6be0.bottom = bottom;
#ifdef WC1_SDL
        emptyFrame = (signed char)definition->emptyFrame;
        filledFrame = (signed char)definition->filledFrame;
        direction = definition->direction;
#else
        emptyFrame = (signed char)
            g_aaCockpitBars_0046dd88[0][index].emptyFrame;
        filledFrame = (signed char)
            g_aaCockpitBars_0046dd88[0][index].filledFrame;
        direction = g_aaCockpitBars_0046dd88[0][index].direction;
#endif

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

/* Function start: 0x438454 */
unsigned int InitializeCockpitReadout(signed char slot,
                                      TextContext *context)
{
    g_aCockpitReadouts_005a7e30[(int)slot].context = context;
    g_aCockpitReadouts_005a7e30[(int)slot].x = context->cursorX;
    g_aCockpitReadouts_005a7e30[(int)slot].y = context->cursorY;
    g_aCockpitReadouts_005a7e30[(int)slot].previousRight = 0;
    return 0;
}

/* Function start: 0x4384AD */
void DrawCockpitReadout(signed char slot, const char *text)
{
    CockpitReadout *readout;

    readout = &g_aCockpitReadouts_005a7e30[(int)slot];
    if (readout->x != -99) {
        SetTextContext(readout->context);
        SetTextCursor((unsigned short)readout->x,
                      (unsigned short)readout->y);
        DrawFormattedText(text);
        EraseCockpitReadoutRegion(
            &g_stScreenViewport_005d21a0, readout->context->cursorX, readout->y,
            readout->previousRight,
            (short)(*(short *)readout->context->font + readout->y - 1),
            g_cSecondaryViewBufferColour_0049cb4c);
        readout->previousRight = readout->context->cursorX;
    }
}

/* Function start: WC2_UNMAPPED */
void EraseCockpitReadoutAtPosition(signed char slot, short left,
                                   short top)
{
    CockpitReadout *readout;

    readout = &g_aCockpitReadouts_005a7e30[(int)slot];
    EraseCockpitReadoutRegion(
        &g_stScreenViewport_005d21a0, left, top, readout->previousRight,
        (short)(*(short *)readout->context->font + readout->y),
        g_cSecondaryViewBufferColour_0049cb4c);
}

/* Function start: 0x4385B3 */
short DrawHudMessageSlot(HudMessageSlot *slot)
{
    unsigned char savedColour;
    short oldDrawColour;
    short showingEraseColour;

    if (g_nCurrentView_00492fa8 != 0)
        return 1;

    oldDrawColour = slot->drawColour;
    if (((int)DAT_0059ab54 / 40) % 3 == 0)
        slot->drawColour = g_cSecondaryViewBufferColour_0049cb4c;
    else
        slot->drawColour = slot->colour;

    if (slot->flashCount != -1) {
        if (slot->drawColour == g_cSecondaryViewBufferColour_0049cb4c &&
            oldDrawColour == slot->colour) {
            slot->flashCount = (signed char)MaxShort(
                0, (short)(slot->flashCount - 1));
        }
        if (slot->flashCount == 0)
            slot->drawColour = g_cSecondaryViewBufferColour_0049cb4c;
    }

    showingEraseColour = slot->drawColour == g_cSecondaryViewBufferColour_0049cb4c;
    savedColour = slot->context->colour;
    slot->context->colour = (unsigned char)slot->drawColour;
    DrawTextAt(slot->context, slot->x, slot->y, slot->text, 2);
    slot->context->colour = savedColour;
    return showingEraseColour;
}

/* Function start: 0x438711 */
void ClearHudMessageSlot(HudMessageSlot *slot)
{
#if 0
    slot->flashCount = 0;
    if (slot->text != 0)
        DrawHudMessageSlot(slot);
    slot->text = 0;
#else
    slot->flashCount = 0;
    if (slot->text != 0)
        DrawHudMessageSlot(slot);
    if (slot->text == 0) {
        slot->text = 0;
    } else {
        slot->text = 0;
        if (slot->text == 0 && g_nCurrentView_00492fa8 == 0 &&
            get_mode(0) == 1 && IsCockpitWeaponShapeLoaded() != 0)
            show_weapon_disp();
    }
#endif
}

/* Function start: 0x4387A7 */
void ClearHudMessageIfMatching(HudMessageSlot *slot, const char *text)
{
    if (slot->text == text)
        ClearHudMessageSlot(slot);
}

/* Function start: 0x4387CD */
void ClearHudGunReadouts(void)
{
    g_aHudMessageSlots_005a7dd0[0].text = 0;
    g_aHudMessageSlots_005a7dd0[0].flashCount = 0;
    g_aHudMessageSlots_005a7dd0[1].text = 0;
    g_aHudMessageSlots_005a7dd0[1].flashCount = 0;
}

/* Function start: 0x4387FA */
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

/* Function start: 0x438864 */
void UpdateMessage(HudMessageSlot *slot)
{
    short showingEraseColour;

    if (slot->text != 0) {
        showingEraseColour = DrawHudMessageSlot(slot);
        if (slot->flashCount == 0 && showingEraseColour != 0)
            ClearHudMessageSlot(slot);
    }
}

/* Function start: 0x4388B6 */
void set_global_message(const char *text, unsigned short colour,
                        int flashCount)
{
    SetHudMessageSlot(&g_aHudMessageSlots_005a7dd0[1], &DAT_005a7700,
                      DAT_005a7530.left,
                      (short)(DAT_005a7530.bottom - 6),
                      text, colour, (signed char)flashCount);
}

/* Function start: 0x4388F5 */
void CockpitMessage(const char *text, unsigned short colour,
                    int flashCount)
{
    if (text != g_aHudMessageSlots_005a7dd0[1].text)
        set_global_message(text, colour, flashCount);
}

/* Function start: 0x43893F */
void remove_message(const char *text)
{
    ClearHudMessageIfMatching(&g_aHudMessageSlots_005a7dd0[1], text);
}

/* Function start: 0x43895F */
short kilrathi_near(short obj, short range)
{
#if 0
    short ship;

    ship = 0;
    do {
        if (g_aeObjectClass_00495328[ship] >= OBJECT_CLASS_SHIP &&
            g_asShipSide_004955d0[ship] == SIDE_KILRATHI &&
            IsPointWithinRange(&g_aShipPosition_00494550[obj],
                               &g_aShipPosition_00494550[ship],
                               range) != 0)
            return 1;
        ship++;
    } while (ship < 10);
    return 0;
#else
    short ship;

    for (ship = 1; ship < 10; ship++) {
        if (g_aeObjectClass_00495328[ship] >= OBJECT_CLASS_SHIP &&
            g_asShipSide_004955d0[ship] == SIDE_KILRATHI &&
            IsPointWithinRange(&g_aShipPosition_00494550[obj],
                               &g_aShipPosition_00494550[ship],
                               range) != 0)
            return 1;
    }
    return 0;
#endif
}

/* Function start: 0x4389FE */
short IsMissionObjectiveOutOfSystem(short objectiveIndex)
{
    short navPoint;
    MissionObjective *objective;

    objective = &g_aMissionObjectives_004932a8[objectiveIndex];
    switch (objective->type) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
        navPoint = find_ships_sphere(objective->index);
        if (navPoint == -1)
            return 0;
        break;
    case 0:
    case 5:
        navPoint = objective->index;
        break;
    }
    if (g_aMissionNavPoints_00491e98[navPoint].systemIndex ==
        g_nCurrentNavPoint_004931bc)
        return 0;
    return 1;
}

/* Function start: 0x438ADD */
short auto_pilot_valid(short showReason)
{
#if 0
    const char *reason;

    reason = 0;
    if (g_cMissionObjectiveCount_00493294 == 0)
        return 0;
    if (distance_from_point(
            0,
            &g_aMissionObjectives_004932a8[
                g_cCurrentObjective_004931cc].position) < 8000) {
        reason = "Already Near";
    } else if (kilrathi_near(0, 16000) != 0) {
        reason = "Enemy Near";
    } else if (g_pActiveHazardField_00493278 != 0) {
        reason = "Hazard Near";
    }
    if (showReason != 0 && reason != 0)
        set_global_message(reason, g_ucHudHighlightColour_0049cb58, 3);
    return reason == 0;
#else
    const char *reason;
    short distance;
    short object;

    reason = 0;
    if (g_cMissionObjectiveCount_00493294 == 0)
        return 0;
    if (g_asShipSide_004955d0[0] == SIDE_KILRATHI)
        return 0;
    if (IsCockpitExplosionActive() != 0)
        return 0;
    if (IsMissionObjectiveOutOfSystem(
            (short)g_cCurrentObjective_004931cc) != 0)
        reason = "Jump Point";
    distance = distance_from_point(
        0, &g_aMissionObjectives_004932a8[
               (signed char)g_cCurrentObjective_004931cc].position);
    object = 1;
    while (object < 10) {
        if (g_aeObjectClass_00495328[object] > OBJECT_CLASS_SHIP &&
            g_asShipMissionType_00495de8[object] ==
                MISSION_TYPE_GOTO_WARP &&
            g_asShipSystemIndex_00495e00[object] ==
                g_nCurrentNavPoint_004931bc &&
            equ_vector(
                &g_aMissionObjectives_004932a8[
                    (signed char)g_cCurrentObjective_004931cc].position,
                &g_aShipDestination_004953f0[object]) == 0) {
            sprintf(g_pszAutopilotWaitReason_0049b050, "Wait for %s",
                    g_apszShipName_00495da8[object]);
            reason = g_pszAutopilotWaitReason_0049b050;
            break;
        }
        object++;
    }
    if (distance < 8000) {
        reason = "Already Near";
    } else if (kilrathi_near(0, 16000) != 0) {
        reason = "Enemy Near";
    } else if (g_pActiveHazardField_00493278 != 0) {
        reason = "Hazard Near";
    }
    if (showReason != 0 && reason != 0)
        CockpitMessage(reason, g_ucHudHighlightColour_0049cb58, 3);
    return reason == 0;
#endif
}

/* Function start: 0x438CEB */
void reset_cockpit(void)
{
#if 0
    memset(g_abCockpitLightGoal_005d1eb8, 0,
           sizeof(g_abCockpitLightGoal_005d1eb8));
    memset(g_abCockpitLightState_005d1e70, 0,
           sizeof(g_abCockpitLightState_005d1e70));
    return g_abCockpitLightState_005d1e70;
#else
    short light;

    for (light = 0; light < 7; light++) {
        g_abCockpitLightState_005d1e70[light] = 0;
        g_abCockpitLightGoal_005d1eb8[light] = 0;
    }
#endif
}

/* Function start: 0x438D30 */
void SetCockpitLightBlink(signed char light, short interval)
{
#if 0
    if (interval < 20) {
        if (interval == 0) {
            g_abCockpitLightGoal_005d1eb8[(int)light] ^= 1;
            return 0;
        }
        if (g_nSpaceFrame_00493134 % interval == 0) {
            g_abCockpitLightGoal_005d1eb8[(int)light] ^= 1;
            return 0;
        }
    } else {
        g_abCockpitLightGoal_005d1eb8[(int)light] = 0;
    }
    return 0;
#else
    if (interval < 20) {
        if (interval == 0) {
            g_abCockpitLightGoal_005d1eb8[(int)light] ^= 1;
        } else if (g_nSpaceFrame_00493134 % interval == 0) {
            g_abCockpitLightGoal_005d1eb8[(int)light] ^= 1;
        }
    } else {
        g_abCockpitLightGoal_005d1eb8[(int)light] = 0;
    }
#endif
}

/* Function start: 0x438DAD */
void draw_cockpit_lights(void)
{
    signed char light;
    short x;
    short y;
    short frame;

    if (g_nRenderedSpaceFrame_00493138 % 4 == 0) {
        if (auto_pilot_valid(0) != 0)
            g_abCockpitLightGoal_005d1eb8[4] = 1;
        else
            g_abCockpitLightGoal_005d1eb8[4] = 0;
    }
    light = 0;
    do {
        if (g_nCockpitDisplayMode_0049d71c == 0) {
            if (g_abCockpitLightState_005d1e70[(int)light] !=
                g_abCockpitLightGoal_005d1eb8[(int)light]) {
                x = g_aasCockpitLightX_0046dca8[
                    (int)g_cCockpitView_0059dab0][(int)light];
                y = g_aasCockpitLightY_0046dcf0[
                    (int)g_cCockpitView_0059dab0][(int)light];
                if (g_abCockpitLightGoal_005d1eb8[(int)light] == 1)
                    frame = g_aacCockpitLightOnFrame_0046dd60[
                        (int)g_cCockpitView_0059dab0][(int)light];
                else
                    frame = g_aacCockpitLightOffFrame_0046dd38[
                        (int)g_cCockpitView_0059dab0][(int)light];
                DrawSpriteDefault(&g_stScreenViewport_005d21a0, x, y,
                                  g_pCockpitDamageShape_005a76f4, frame);
                g_abCockpitLightState_005d1e70[(int)light] =
                    g_abCockpitLightGoal_005d1eb8[(int)light];
            }
        } else {
            x = g_aasCockpitLightX_0046dca8[
                (int)g_cCockpitView_0059dab0][(int)light];
            y = g_aasCockpitLightY_0046dcf0[
                (int)g_cCockpitView_0059dab0][(int)light];
            if (g_abCockpitLightGoal_005d1eb8[(int)light] == 1)
                frame = g_aacCockpitLightOnFrame_0046dd60[
                    (int)g_cCockpitView_0059dab0][(int)light];
            else
                frame = g_aacCockpitLightOffFrame_0046dd38[
                    (int)g_cCockpitView_0059dab0][(int)light];
            DrawSpriteDefault(&g_stScreenViewport_005d21a0, x, y,
                              g_pCockpitDamageShape_005a76f4, frame);
            g_abCockpitLightState_005d1e70[(int)light] =
                g_abCockpitLightGoal_005d1eb8[(int)light];
        }
        light++;
    } while (light < 7);
}

/* Function start: 0x438F62 */
void update_lights(void)
{
    short fuelPercent;

    fuelPercent = (short)(
        (g_anShipFuel_0059b470[0] * 100) /
        *(int *)&g_aObjectTypeData_00496d30[
            g_acObjectType_00493980[0]].lifetime);
    SetCockpitLightBlink(6, fuelPercent);
    vdu_polygon(0, fuelPercent);
    vdu_polygon(1, g_asShipWeaponEnergy_0059d470[0]);

    if (g_nTrainSimActive_00469e2c == 0) {
        if (calculate_damage_level() >= 3 &&
            (int)g_aasShipShield_00495518[0][1] +
                (int)g_aasShipShield_00495518[0][0] < 10) {
            SetCockpitLightBlink(3, 2);
            if (DAT_005a7ec0 == 0 ||
                g_nSpaceFrame_00493134 % 10 == 0)
                PlaySfxWaveFileByNumber(0x20, -1, 0);
        } else if (DAT_005a7ec0 != 0) {
            ((void (__cdecl *)(int, int))FlushSoundEffectsAndLog)(
                DAT_005a7ec0, 1);
            DAT_005a7ec0 = 0;
            g_abCockpitLightGoal_005d1eb8[3] = 0;
        }
    }
}

/* Function start: 0x439076 */
void update_bars(void)
{
    ObjectTypeData *typeData;
    short forePercent;
    short aftPercent;

    typeData = &g_aObjectTypeData_00496d30[g_acObjectType_00493980[0]];
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
    forePercent = (short)((g_aasShipShield_00495518[0][0] * 100) /
                          typeData->shieldFore);
    SetCockpitLightBlink(0, forePercent);
    vdu_polygon(6, forePercent);
    DrawCockpitReadout(
        4, _itoa((int)g_aasShipShield_00495518[0][0],
                 g_szTextScratchBuffer_005d1c40, 10));
    aftPercent = (short)((g_aasShipShield_00495518[0][1] * 100) /
                         typeData->shieldAft);
    SetCockpitLightBlink(1, aftPercent);
    vdu_polygon(7, aftPercent);
    DrawCockpitReadout(
        5, _itoa((int)g_aasShipShield_00495518[0][1],
                 g_szTextScratchBuffer_005d1c40, 10));
}

/* Function start: 0x439264 */
short get_mode(short i)
{
    return DAT_0059d500[
        ((int)g_acVduModeStackDepth_0059dec0[i] + i * 4) * 2];
}

/* Function start: 0x43928E */
void set_mode(short i, int state)
{
    if (get_mode(i) != state)
        ClearHudMessageSlot(&g_aHudMessageSlots_005a7dd0[i]);
    g_acVduModeStackDepth_0059dec0[i] = 0;
    *(int *)&DAT_0059d500[i * 8] = state;
}

/* Function start: 0x4392F1 */
unsigned short SetVduModeIfChanged(short i, int state)
{
    short changed;

    changed = get_mode(i) != state;
    if (changed != 0)
        set_mode(i, state);
    return changed;
}

/* Function start: 0x43934D */
int GetVduModeStackDepth(short i)
{
    return g_acVduModeStackDepth_0059dec0[i];
}

/* Function start: 0x439369 */
void push_mode(short i, int state)
{
    ClearHudMessageSlot(&g_aHudMessageSlots_005a7dd0[i]);
    g_acVduModeStackDepth_0059dec0[i]++;
    *(int *)&DAT_0059d500[
        ((int)g_acVduModeStackDepth_0059dec0[i] + i * 4) * 2] = state;
}

/* Function start: 0x4393B3 */
void pop_mode(short i)
{
    ClearHudMessageSlot(&g_aHudMessageSlots_005a7dd0[i]);
    g_acVduModeStackDepth_0059dec0[i]--;
}

/* Function start: 0x4393E1 */
void set_new_vdu(short vdu)
{
    if (get_mode(vdu) == 0) {
        malf_noise(vdu, 1, g_ucVduStaticColour_0049cb60, 0x17, 0);
    } else {
        if (vdu == 1)
            ClearViewport(&DAT_005a7530, g_cSecondaryViewBufferColour_0049cb4c);
        else
            ClearViewport(&DAT_005a6b80, g_cSecondaryViewBufferColour_0049cb4c);
    }
    DAT_0059ce18[vdu] = (unsigned int)get_mode(vdu);
}

/* Function start: 0x4394A0 */
short update_vid_disp(short vdu)
{
    short changed;

    changed = get_mode(vdu) != (int)DAT_0059ce18[vdu];
    if (changed != 0)
        set_new_vdu(vdu);
    return changed;
}

/* Function start: 0x439500 */
void InvalidateVduMode(short i)
{
    DAT_0059ce18[i] = 0;
}

/* Function start: 0x43951A */
void clear_message_time(void)
{
    g_nHudMessageTime_005d1c32 = 0;
}

/* Function start: 0x43952E */
short message_showing(void)
{
    return 0 < g_nHudMessageTime_005d1c32;
}

/* Function start: 0x439559 */
void set_message_time(short duration)
{
    g_nHudMessageTime_005d1c32 = duration;
    if (g_nHudMessageTime_005d1c32 == -1)
        g_bSpeechPlaybackComplete_004a266c = 0;
#if 0
    return 0;
#endif
}

/* Function start: 0x439588 */
void check_message(void)
{
#if 0
    if (message_showing() &&
        (g_nHudMessageTime_005d1c32 = g_nHudMessageTime_005d1c32 - 1,
         g_nHudMessageTime_005d1c32 <= 0))
        EndCommMenu();
#else
    if (message_showing() != 0) {
        if (g_nHudMessageTime_005d1c32 == -1) {
            if (g_bSpeechPlaybackComplete_004a266c != 0) {
                clear_message_time();
                if (g_aeSpecialManeuver_00495600[
                        g_nCommSpeakerObject_0049b794] ==
                        SPECIAL_MANEUVER_UNKNOWN_9 &&
                    g_pCommPortraitResource_0049b788 != 0) {
                    g_nCommDeathSequenceFrame_0049ae84 = 1;
                } else {
                    EndCommMenu();
                }
                if (g_bEjectionSequencePending_00493058 != 0) {
                    ejection_sequence(
                        g_aObjectTypeData_00496d30[
                            g_acObjectType_00493980[0]].field_16,
                        1);
                    g_nPlayerFuel_00495638 =
                        *(int *)&g_aObjectTypeData_00496d30[
                            g_acObjectType_00493980[0]].lifetime;
                    g_bEjectionSequencePending_00493058 = 0;
                }
            }
        } else {
            g_nHudMessageTime_005d1c32--;
            if (g_nHudMessageTime_005d1c32 == 0) {
                EndCommMenu();
                if (g_bEjectionSequencePending_00493058 != 0) {
                    ejection_sequence(
                        g_aObjectTypeData_00496d30[
                            g_acObjectType_00493980[0]].field_16,
                        1);
                    g_nPlayerFuel_00495638 =
                        *(int *)&g_aObjectTypeData_00496d30[
                            g_acObjectType_00493980[0]].lifetime;
                    g_bEjectionSequencePending_00493058 = 0;
                }
            }
        }
    }
#endif
}

/* Function start: 0x4396BF */
void update_digital_readouts(void)
{
#if 0
    long velocity;

    SetTextContext(&DAT_005a7720);
    DrawCockpitReadout(
        2, _itoa((int)(short)((g_anShipSpeed_0059b320[0] >> 8) * 10),
                 g_szTextScratchBuffer_005d1c40, 10));
    velocity = MultiplyFixed(
        Vector_magnitude(&g_aShipVelocity_0059c010[0]), 0xa00);
    DrawCockpitReadout(
        3, _itoa((int)(short)(velocity >> 8),
                 g_szTextScratchBuffer_005d1c40, 10));
    return 0;
#else
    short speed;
    short velocity;

    SetTextContext(&DAT_005a7720);
    speed = (short)(g_anShipSpeed_0059b320[0] >> 8);
    speed = (short)(speed * 10);
    DrawCockpitReadout(
        2, _itoa(speed, g_szTextScratchBuffer_005d1c40, 10));
    velocity = (short)(MultiplyFixed(
        Vector_magnitude(&g_aShipVelocity_0059c010[0]), 0xa00) >> 8);
    DrawCockpitReadout(
        3, _itoa(velocity, g_szTextScratchBuffer_005d1c40, 10));
#endif
}

/* Function start: WC2_UNMAPPED */
void PlayTargetLockSfx(void)
{
    PlaySfxWaveFileByNumber(0x18, -1, 0);
}

/* Function start: 0x439753 */
void malf_sound(void)
{
#if 0
    PlaySfxWaveFileByNumber(0x1f, -1, 0);
#else
    LogUnknownSoundEffect(
        g_abSoundEffectDefinitions_0049bf18 + 0xf0,
        0, 0x7f, 0x64, 1, 1);
    PlaySfxWaveFileByNumber(0x1f, -1, 0);
#endif
}

/* Function start: 0x439789 */
short malf(char component)
{
#if 0
    int damage = g_acPlayerComponentDamage_00493470[(int)component];

    return (unsigned short)RandomInRange(0, 15) < damage * damage;
#else
    return g_acPlayerComponentDamage_00493470[(int)component] *
               g_acPlayerComponentDamage_00493470[(int)component] >
           (unsigned short)RandomInRange(0, 15);
#endif
}

/* Function start: 0x4397D5 */
void vdu_malf(short vdu, short sound)
{
#if 0
    if (g_nCurrentView_00492fa8 == 0)
        malf_noise(vdu, 1, g_ucVduStaticColour_0049cb60, sound, 0);
    set_mode(vdu, 0);
    return 0;
#else
    if (g_nCurrentView_00492fa8 == 0 &&
        ((vdu == 0 && g_stLeftVduViewport_005d2180.top > 0) ||
         (vdu == 1 && g_stRightVduViewport_005d2b20.top > 0))) {
        malf_noise(vdu, 1, g_ucVduStaticColour_0049cb60, sound, 0);
        set_mode(vdu, 0);
    }
#endif
}

/* Function start: 0x43984F */
void ShowComponentHitHudMessage(const char *text, unsigned short colour,
                                short flashCount)
{
    if (g_nTrainSimActive_00469e2c == 0 && get_mode(0) != 0) {
        if (g_aHudMessageSlots_005a7dd0[0].text != 0)
            ClearHudMessageSlot(&g_aHudMessageSlots_005a7dd0[0]);
        DosStrcpy(g_szComponentHitMessage_005a7e00, text);
        SetHudMessageSlot(&g_aHudMessageSlots_005a7dd0[0], &DAT_005a74f0,
                          DAT_005a6b80.left,
                          (short)(DAT_005a6b80.bottom - 6),
                          g_szComponentHitMessage_005a7e00,
                          colour, (signed char)flashCount);
    }
}

/* Function start: 0x4398CB */
int damage_your_component(char component, char amount, char maximum)
{
    int index = (int)component;
    char text[40];

    g_acPlayerComponentDamage_00493470[index] = (signed char)MinShort(
        (short)(g_acPlayerComponentDamage_00493470[index] + amount),
        (short)maximum);
    if (malf(component) != 0 && index == 3) {
        vdu_malf(0, 0x18);
        vdu_malf(1, 0x18);
    }
    if (get_mode(0) == 2 || get_mode(0) == 1) {
        sprintf(text, g_szComponentHitFormat_004692e0,
                g_apszComponentNames_0046a778[index]);
        ShowComponentHitHudMessage(text, (unsigned char)DAT_004699ac, 5);
    }
    return g_acPlayerComponentDamage_00493470[index];
}

/* Function start: 0x4399C6 */
void RemovePlayerReleaseWeapon(signed char weapon)
{
#if 0
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
    g_nSelectedReleaseWeaponIndex_004934e0 = -1;
    select_new_release_weapon(preferredType);
#else
    lock_off();
    g_nReleaseWeaponDisplayType_005d1c28 =
        ((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1])[weapon].type;
    g_cReleaseWeaponDisplayFrame_0049b060 = 1;
    g_nReleaseWeaponDisplayX_005d1c24 =
        (short)(g_aWeaponDisplayPositions_005d1de0[weapon].x +
                g_nWeaponDisplayOriginX_005d4254);
    g_nReleaseWeaponDisplayY_005d1c26 =
        (short)(g_aWeaponDisplayPositions_005d1de0[weapon].y +
                g_nWeaponDisplayOriginY_005d4256);
    g_cReleaseWeaponDisplayTicks_0049b064 = 3;
    g_cReleaseWeaponDisplayState_0049b068 = 0;
    if (g_pReleaseWeaponDisplayBackground_0049b05c == 0)
        g_cReleaseWeaponDisplayFrame_0049b060 = -1;
    remove_weapon(0, weapon);
#endif
}

/* Function start: 0x439A5A */
void fire_computer_graphic_missile(void)
{
    short visible;

    if (g_cReleaseWeaponDisplayFrame_00469070 == -1)
        return;
    visible = g_nCurrentView_00492fa8 == 0 && get_mode(0) == 1;
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

/* Function start: 0x439BB7 */
void show_weapon_disp(void)
{
    ShipWeaponSlot *weapons;
    ShipWeaponSlot *selectedWeapon;
    const char *releaseName;
    const char *gunName;
    enum ObjectType selectedGunType;
    signed char count;

    selectedWeapon =
        &((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1])[
            g_nSelectedReleaseWeaponIndex_004934e0];
    set_new_vdu(0);
    DrawTextAt(&DAT_005a74f0, DAT_005a6b80.left, DAT_005a6b80.top,
               "WEAPON DISPLAY", 2);
    DrawViewportLine(&DAT_005a6b80, (short)(DAT_005a6b80.left + 2),
                     (short)(DAT_005a6b80.top + 5),
                     (short)(DAT_005a6b80.right - 2),
                     (short)(DAT_005a6b80.top + 5), g_ucPrimaryTextColour_0049cb64);

    releaseName = "";
    if (g_nSelectedReleaseWeaponIndex_004934e0 != -1)
        releaseName = g_aObjectTypeData_00496d30[
            selectedWeapon->type].displayName;
    selectedGunType = g_eSelectedGunType_0046c054;
    gunName = "";
    switch ((int)selectedGunType) {
    case -1:
        gunName = "";
        break;
    case 0x80:
        gunName = "Full Guns";
        break;
    default:
        gunName = g_aObjectTypeData_00496d30[
            selectedGunType].displayName;
        break;
    }
    DrawFormattedText("\nWeapon: %s", releaseName);
    DrawFormattedText("\nGun: %s", gunName);

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

/* Function start: 0x439F5F */
void update_status_text(void)
{
#if 0
#else
    short x;
    short index;
    short y;
    short frame;

    for (index = 0;
         index < (signed char)g_aShipWeapons_004956b0[0][0];
         index++) {
        if (((ShipWeaponSlot *)&g_aShipWeapons_004956b0[0][1])[
                index].type == 11) {
            g_nWeaponDisplayOriginX_005d4254 =
                (short)(g_stLeftVduViewport_005d2180.left +
                        g_nWeaponDisplayOffsetX_0049ae8c);
            g_nWeaponDisplayOriginY_005d4256 =
                (short)(g_nWeaponDisplayOffsetY_0049ae8e +
                        g_stLeftVduViewport_005d2180.top);
            x = g_aWeaponDisplayPositions_005d1de0[index].x;
            y = g_aWeaponDisplayPositions_005d1de0[index].y;

            if (g_aHudMessageSlots_005d1d40[0].text == 0 &&
                x < 0x29 && x > 0x1f) {
                if (g_asGunCooldown_005c8d70[0] > 0) {
                    frame = 0x1d;
                    DrawSpriteDefault(
                        &g_stLeftVduViewport_005d2180,
                        (short)(g_nWeaponDisplayOriginX_005d4254 + x),
                        (short)(g_nWeaponDisplayOriginY_005d4256 + y),
                        g_pCockpitWeaponShape_005d2b54, frame);
                } else {
                    frame = 0x1a;
                    DrawSpriteDefault(
                        &g_stLeftVduViewport_005d2180,
                        (short)(g_nWeaponDisplayOriginX_005d4254 + x),
                        (short)(g_nWeaponDisplayOriginY_005d4256 + y),
                        g_pCockpitWeaponShape_005d2b54, frame);
                }
            }

            if (g_cReleaseWeaponDisplayFrame_0049b060 == -1 && x < 0x1f) {
                if (g_asGunCooldown_005c8d70[1] > 0) {
                    frame = 0x1e;
                    DrawSpriteDefault(
                        &g_stLeftVduViewport_005d2180,
                        (short)(g_nWeaponDisplayOriginX_005d4254 + x),
                        (short)(g_nWeaponDisplayOriginY_005d4256 + y),
                        g_pCockpitWeaponShape_005d2b54, frame);
                } else {
                    frame = 0x1b;
                    DrawSpriteDefault(
                        &g_stLeftVduViewport_005d2180,
                        (short)(g_nWeaponDisplayOriginX_005d4254 + x),
                        (short)(g_nWeaponDisplayOriginY_005d4256 + y),
                        g_pCockpitWeaponShape_005d2b54, frame);
                }
            }

            if (g_cReleaseWeaponDisplayFrame_0049b060 == -1 && x > 0x29) {
                if (g_asGunCooldown_005c8d70[2] > 0) {
                    frame = 0x1f;
                    DrawSpriteDefault(
                        &g_stLeftVduViewport_005d2180,
                        (short)(g_nWeaponDisplayOriginX_005d4254 + x),
                        (short)(g_nWeaponDisplayOriginY_005d4256 + y),
                        g_pCockpitWeaponShape_005d2b54, frame);
                } else {
                    frame = 0x1c;
                    DrawSpriteDefault(
                        &g_stLeftVduViewport_005d2180,
                        (short)(g_nWeaponDisplayOriginX_005d4254 + x),
                        (short)(g_nWeaponDisplayOriginY_005d4256 + y),
                        g_pCockpitWeaponShape_005d2b54, frame);
                }
            }
        }
    }
#endif
}

/* Function start: 0x43A1EB */
short sighted(short objective)
{
    return (g_aMissionObjectives_004932a8[objective].flags & 4) == 4;
}

/* Function start: 0x43A21E */
short visited(short objective)
{
    return (g_aMissionObjectives_004932a8[objective].flags & 1) == 1;
}

/* Function start: 0x43A251 */
short achieved(short objective)
{
#if 0
    return (g_aMissionObjectives_004932a8[objective].flags & 2) == 2;
#else
    return (g_aMissionObjectives_004932a8[objective].flags & 2) == 2 ||
           g_aMissionObjectives_004932a8[objective].type == -1;
#endif
}

/* Function start: 0x43A29E */
void flag_objective(short objective, unsigned char flags)
{
    g_aMissionObjectives_004932a8[objective].flags |= flags;
}

/* Function start: 0x43A2CD */
void DrawCalculatingLabel(void)
{
#if 0
    if (g_nCurrentObjectiveRange_004931c8 <= 0) {
        DrawCockpitReadout(0, g_szCalculating_0046931c);
    } else {
        strcpy(g_szTextScratchBuffer_005d1c40 +
                   strlen(_ltoa((long)g_nCurrentObjectiveRange_004931c8,
                                g_szTextScratchBuffer_005d1c40, 10)),
               " km");
        DrawCockpitReadout(0, g_szTextScratchBuffer_005d1c40);
    }
    g_nDisplayedObjectiveRange_0049b078 =
        g_nCurrentObjectiveRange_004931c8;
#else
    if (IsMissionObjectiveOutOfSystem(
            (short)g_cCurrentObjective_004931cc) != 0) {
        DrawCockpitReadout(0, "Out system");
    } else {
        if (g_nCurrentObjectiveRange_004931c8 < 1) {
            DrawCockpitReadout(0, "CALCULATING");
        } else {
            strcpy(g_szTextScratchBuffer_005d1c40 +
                       strlen(_ltoa((long)g_nCurrentObjectiveRange_004931c8,
                                    g_szTextScratchBuffer_005d1c40, 10)),
                   " km");
            DrawCockpitReadout(0, g_szTextScratchBuffer_005d1c40);
        }
        g_nDisplayedObjectiveRange_0049b078 =
            g_nCurrentObjectiveRange_004931c8;
    }
#endif
}

/* Function start: 0x43A374 */
const char *objective_name(short objective)
{
    if (objective >= (short)g_cMissionObjectiveCount_00493294)
        return g_szNoObjective_0046932c;
    if (*g_aMissionObjectives_004932a8[objective].name == '?' &&
        sighted(objective) == 0)
        return g_szUnknownObjective_00469334;
    return g_aMissionObjectives_004932a8[objective].displayName;
}

/* Function start: 0x43A3F8 */
void show_navigation_disp(void)
{
    DrawTextAt(&DAT_005a7700, DAT_005a7530.left, DAT_005a7530.top,
               g_szCompNavigation_0046933c, 2);
    DrawFormattedText(g_szDestinationFormat_0046934c,
                      objective_name((short)g_cCurrentObjective_004931cc));
    DrawFormattedText(g_szNavigationRangeLabel_00469360);
    InitializeCockpitReadout(0, &DAT_005a7700);
    DrawFormattedText(g_szNewObjectivePrompt_0046936c);
    DrawCalculatingLabel();
}

/* Function start: 0x43A474 */
short hidden_objective(short objective)
{
    short nameHidden;
    short hidden;
    short ship;

    nameHidden =
        *g_aMissionObjectives_004932a8[objective].displayName == '.' ||
        *g_aMissionObjectives_004932a8[objective].name == '.';
    if (nameHidden != 0 ||
        (mobile_objective(objective) != 0 &&
         g_aMissionShips_00492290[
             (signed char)g_aMissionObjectives_004932a8[
                 objective].index].state != 0))
        hidden = 1;
    else
        hidden = 0;
    if (hidden == 0 && mobile_objective(objective) != 0 &&
        g_aMissionObjectives_004932a8[
            g_cMissionObjectiveCount_00493294].type == 0) {
        ship = find_ship_index(
            (short)g_aMissionObjectives_004932a8[objective].index);
        if (g_aMissionShips_00492290[
                (signed char)g_aMissionObjectives_004932a8[
                    objective].index].missionType ==
                MISSION_TYPE_WARP_ARRIVE &&
            ship != -1)
            hidden = 1;
    }
    return hidden;
}

/* Function start: 0x43A75E */
short set_new_objective(short pathIndex)
{
    if (pathIndex < 0) {
        pathIndex = -1;
        do {
            pathIndex++;
        } while (g_abFlightPath_004932a0[pathIndex] != -1 &&
                 pathIndex < (short)g_cMissionObjectiveCount_00493294);
        pathIndex--;
    }
    if (pathIndex > (short)g_cMissionObjectiveCount_00493294)
        pathIndex = 0;
    if (g_abFlightPath_004932a0[pathIndex] == -1)
        pathIndex = 0;
    g_cCurrentNavPointIndex_00493298 = (signed char)pathIndex;
    if (hidden_objective(
            (short)g_abFlightPath_004932a0[pathIndex]) != 0)
        return 0;
    g_cCurrentObjective_004931cc =
        g_abFlightPath_004932a0[g_cCurrentNavPointIndex_00493298];
    g_aeShipObjective_00495f08[0] =
        (enum ShipObjective)g_aMissionObjectives_004932a8[
            (short)g_cCurrentObjective_004931cc].type;
    set_objective_range(0);
    return 1;
}

/* Function start: 0x43A851 */
short cycle_next_objective(void)
{
    short wraps;

    wraps = 0;
#if 0
    do {
        if (set_new_objective(
                (short)((short)g_cCurrentNavPointIndex_00493298 + 1)) != 0)
            break;
        if (g_cCurrentNavPointIndex_00493298 == 0)
            wraps++;
    } while (wraps < 3);
#else
    while (wraps < 3) {
        if (set_new_objective(
                (short)((short)g_cCurrentNavPointIndex_00493298 + 1)) != 0)
            break;
        if (g_cCurrentNavPointIndex_00493298 == 0)
            wraps++;
    }
#endif
    if (wraps >= 3) {
        g_cCurrentNavPointIndex_00493298 = 0;
        g_cCurrentObjective_004931cc = g_abFlightPath_004932a0[
            g_cCurrentNavPointIndex_00493298];
    }
    return wraps < 3;
}

/* Function start: 0x43A8EA */
void set_next_destination(void)
{
    set_new_objective(0);
    do {
        if (set_new_objective(
                (short)g_cCurrentNavPointIndex_00493298) != 0 &&
            visited((short)g_abFlightPath_004932a0[
                g_cCurrentNavPointIndex_00493298]) == 0)
            break;
        g_cCurrentNavPointIndex_00493298++;
    } while (g_cCurrentNavPointIndex_00493298 <
                 g_cMissionObjectiveCount_00493294 &&
             g_abFlightPath_004932a0[
                 g_cCurrentNavPointIndex_00493298] != -1);
    if (g_cCurrentNavPointIndex_00493298 >=
            g_cMissionObjectiveCount_00493294 ||
        g_abFlightPath_004932a0[
            g_cCurrentNavPointIndex_00493298] == -1) {
        set_new_objective(0);
        cycle_next_objective();
    }
    if (get_mode(1) == 5)
        InvalidateVduMode(1);
}

/* Function start: 0x43A9D7 */
short LocateMobileObjective(short objective)
{
    short ship = -1;

    if (mobile_objective(objective) != 0) {
        ship = locate_ship(
            g_aMissionObjectives_004932a8[objective].index,
            &g_aMissionObjectives_004932a8[objective].position);
    }
    return ship;
}

/* Function start: 0x43AA3F */
short is_any_ship_returning_to_current_nav_point(void)
{
    short ship;

    for (ship = 0; ship < 10; ship++) {
        if (g_aeObjectClass_00495328[ship] >= OBJECT_CLASS_SHIP &&
            g_asShipMissionType_00495de8[ship] == MISSION_TYPE_COME_HOME &&
            g_abShipNavPointIndex_00495f60[ship] <= g_cCurrentNavPointIndex_00493298)
            return 1;
    }
    return 0;
}

/* Function start: 0x43AAC3 */
unsigned int escorting_a_ship(void)
{
    if (g_asShipMissionType_00495de8[0] != MISSION_TYPE_ESCORT) {
        if (is_any_ship_returning_to_current_nav_point() == 0)
            return 0;
    }
    return 1;
}

/* Function start: 0x43AAFF */
void flag_reached(short objective, short reached)
{
    short carrierMissionShip;
    short carrierObject;
    short objectiveType;
    short advanceDestination;
    short markVisited;

    carrierMissionShip = g_anShipMissionShip_00495e00[0];
    objectiveType = (short)g_aMissionObjectives_004932a8[objective].type;
    carrierObject = find_ship_index(carrierMissionShip);
    markVisited = objective != g_cCurrentObjective_004931cc;
    advanceDestination = 0;
    if (objective == g_cCurrentObjective_004931cc) {
        if (reached == 0 && escorting_a_ship() != 0 &&
            carrierObject != -1 &&
            g_aMissionObjectives_004932a8[objective].index !=
                g_anShipMissionShip_00495e00[0]) {
            if (objectiveType != 1 ||
                g_aMissionShips_00492290[carrierMissionShip].state != 1) {
                sprintf(g_pszObjectiveStatusMessage_0046908c,
                        g_szWaitForFormat_004693a4,
                        g_aObjectTypeData_00496d30[
                            g_acObjectType_00493980[carrierObject]].
                                displayName);
                CockpitMessage(g_pszObjectiveStatusMessage_0046908c,
                               g_ucHudHighlightColour_0049cb58, 4);
            }
        } else {
            advanceDestination = 1;
            CockpitMessage(
                visited(objective) != 0
                    ? (char *)g_szAlreadyVisited_00469380
                    : (char *)g_szObjectiveReached_00469390,
                g_ucHudHighlightColour_0049cb58, 4);
            markVisited = advanceDestination;
        }
    }
    if (objectiveType != 1 && markVisited != 0) {
        if (visited(objective) == 0 && carrierObject != -1 &&
            g_aMissionObjectives_004932a8[objective].index ==
                g_anShipMissionShip_00495e00[0] &&
            g_acObjectType_00493980[carrierObject] !=
                OBJECT_TYPE_TIGERS_CLAW)
            send_message(carrierObject, 6);
        flag_objective(objective, 1);
    }
    if (advanceDestination != 0)
        set_next_destination();
}

/* Function start: 0x43AD61 */
void check_sighting(short objective, short range, short object)
{
    if (sighted(objective) == 0 && range < 16000 &&
        (object == -1 ||
         g_asObjectScreenX_00493598[object] != (short)0x8001))
        flag_objective(objective, 4);
}

/* Function start: 0x43ADC4 */
void check_visit(short objective, short range)
{
    int reachedRange;

    if ((short)g_aMissionObjectives_004932a8[objective].type == 3 ||
        (short)g_aMissionObjectives_004932a8[objective].type == 4)
        reachedRange = 6000;
    else
        reachedRange = 1500;
    if (range < reachedRange)
        flag_reached(objective, 0);
}

/* Function start: 0x43AE2F */
void update_objective_location(short objective)
{
    FixedVector delta;
    short object;
    short range;

    object = LocateMobileObjective(objective);
    if (sighted(objective) != 0 && visited(objective) != 0 &&
        g_cCurrentObjective_004931cc != objective)
        return;
    ComputeVectorDelta(&g_aShipPosition_00494550[0],
                       &g_aMissionObjectives_004932a8[objective].position,
                       &delta);
    range = FixedToShortSaturating(
        Vector_magnitude(&delta));
    check_sighting(objective, range, object);
    if (mobile_objective(objective) != 0) {
        if (object != -1)
            check_visit(objective, range);
    } else if (g_aMissionNavPoints_00491e98[
                   g_aMissionObjectives_004932a8[objective].index].type >= 1) {
        check_visit(objective, range);
    }
}

/* Function start: 0x43AFD3 */
short objective_lost(short objective)
{
    short state;

    state = g_aMissionShips_00492290[
        g_aMissionObjectives_004932a8[objective].index].state;
    switch (g_aMissionObjectives_004932a8[objective].type) {
        case 2:
        case 3:
            return state != 0;
        case 4:
            return state == 3;
        default:
            return 0;
    }
}

/* Function start: 0x43B089 */
void check_objectives(void)
{
    if (objective_lost((short)g_cCurrentObjective_004931cc) != 0) {
        cycle_next_objective();
        InvalidateVduMode(1);
    } else {
        update_objective_location((short)g_cCurrentObjective_004931cc);
    }
    if (g_nDisplayedObjectiveRange_0049b078 !=
        g_nCurrentObjectiveRange_004931c8)
        DrawCalculatingLabel();
}

/* Function start: 0x43B0EB */
void rotational_pos_to_scanner_pos(signed char object,
                                   const SphericalVector *position)
{
    short horizontal;

    horizontal = position->yaw;
    if (abs((int)horizontal) < 45)
        g_nScannerCursorX_005a7e6c = (short)(
            g_stCockpitLayout_0046e008.scanner[
                (int)g_cCockpitView_0059dab0].centerX + horizontal / 4);
    else
        g_nScannerCursorX_005a7e6c = (short)(
            g_stCockpitLayout_0046e008.scanner[
                (int)g_cCockpitView_0059dab0].centerX + horizontal / 6);

    g_nScannerCursorY_005a7e6e = (short)(
        g_stCockpitLayout_0046e008.scanner[
            (int)g_cCockpitView_0059dab0].centerY + position->pitch / -3);
    g_nScannerCursorX_005a7e6c =
        MinShort(g_stCockpitLayout_0046e008.scanner[
                     (int)g_cCockpitView_0059dab0].maximumX,
                 g_nScannerCursorX_005a7e6c);
    g_nScannerCursorX_005a7e6c =
        MaxShort(g_stCockpitLayout_0046e008.scanner[
                     (int)g_cCockpitView_0059dab0].minimumX,
                 g_nScannerCursorX_005a7e6c);
    g_nScannerCursorY_005a7e6e =
        MinShort(g_stCockpitLayout_0046e008.scanner[
                     (int)g_cCockpitView_0059dab0].maximumY,
                 g_nScannerCursorY_005a7e6e);
    g_nScannerCursorY_005a7e6e =
        MaxShort(g_stCockpitLayout_0046e008.scanner[
                     (int)g_cCockpitView_0059dab0].minimumY,
                 g_nScannerCursorY_005a7e6e);
    g_asScannerObjectX_005d1ea0[(int)object] =
        g_nScannerCursorX_005a7e6c;
    g_asScannerObjectY_005a7e80[(int)object] =
        g_nScannerCursorY_005a7e6e;
}

/* Function start: 0x43B1F0 */
short mobile_objective(short objective)
{
    int type = g_aMissionObjectives_004932a8[objective].type;

    if (type == 1 || type == 3 || type == 4 || type == 2)
        return 1;
    return 0;
}

/* Function start: 0x43B258 */
void ResetScannerContacts(void)
{
    short i = 10;

#if 0
    do {
        g_asScannerObjectX_005d1ea0[i] = 0;
        i = i - 1;
    } while (i != 0);
#else
    for (; i > 0; i--)
        g_asScannerObjectX_005d1ea0[i] = 0;
#endif
    g_nScannerTargetObject_0049b07c = -1;
}

/* Function start: 0x43B29D */
void clear_head_up_display(void)
{
    short object;

    if (g_nScannerTargetObject_0049b07c != -1) {
        object = g_nScannerTargetObject_0049b07c;
        DrawViewportPixel(&g_stScreenViewport_005d21a0,
                          g_asScannerObjectX_005d1ea0[object],
                          g_asScannerObjectY_005a7e80[object],
                          g_asScannerBackgroundColour_005a7ed0[object]);
        g_asScannerObjectX_005d1ea0[
            g_nScannerTargetObject_0049b07c] = 0;
    }
    if (g_asScannerObjectX_005d1ea0[10] != 0) {
        RestoreSpriteBackground(&g_stScreenViewport_005d21a0,
                                g_pScannerMarkerBackground_005a7dc4,
                                g_asScannerObjectX_005d1ea0[10],
                                g_asScannerObjectY_005a7e80[10],
                                g_pTargetLockShape_005a6bf4, 2);
        g_asScannerObjectX_005d1ea0[10] = 0;
    }
    object = 9;
    do {
        if (g_asScannerObjectX_005d1ea0[object] != 0) {
            DrawViewportPixel(&g_stScreenViewport_005d21a0,
                              g_asScannerObjectX_005d1ea0[object],
                              g_asScannerObjectY_005a7e80[object],
                              g_asScannerBackgroundColour_005a7ed0[object]);
            g_asScannerObjectX_005d1ea0[object] = 0;
        }
        object--;
    } while (object != 0);
    g_nScannerTargetObject_0049b07c = -1;
}

/* Function start: 0x43B4CF */
unsigned int set_objective_range(short showOnScanner)
{
    FixedVector relative;
    FixedVector rotated;
    SphericalVector spherical;

    LocateMobileObjective((short)g_cCurrentObjective_004931cc);
    ComputeVectorDelta(&g_aShipPosition_00494550[0],
                       &g_aMissionObjectives_004932a8[
                           (short)g_cCurrentObjective_004931cc].position,
                       &relative);
    transform_to_objects_frame(&relative, &rotated, 0);
    rectangular_to_spherical(&rotated, &spherical);
    if (showOnScanner != 0)
        rotational_pos_to_scanner_pos(10, &spherical);
    g_nCurrentObjectiveRange_004931c8 = spherical.radius >> 8;
    return 0;
}

/* Function start: 0x43B61F */
short get_color(short object, unsigned short *colour)
{
    enum ObjectClass objectClass;

    objectClass = g_aeObjectClass_00495328[object];
    if (objectClass < OBJECT_CLASS_MISSILE)
        return 0;
    if (objectClass == OBJECT_CLASS_SHIP) {
        if (g_asShipSide_004955d0[object] == SIDE_KILRATHI)
            *colour = DAT_004699ac;
        else if (g_asShipSide_004955d0[object] == SIDE_IMPERIAL)
            *colour = DAT_004699a4;
        else
            *colour = g_ucPrimaryTextColour_0049cb64;
        return 1;
    }
    if (objectClass == OBJECT_CLASS_CAPITAL_SHIP) {
        if (g_asShipSide_004955d0[object] == SIDE_KILRATHI)
            *colour = DAT_004699b8;
        else if (g_acObjectType_00493980[object] ==
                 OBJECT_TYPE_TIGERS_CLAW)
            *colour = g_cViewportClearColour_004699a0;
        else
            *colour = DAT_004699c0;
        return 1;
    }
    if (g_acShipTarget_00495f20[object] == 0) {
        *colour = g_ucHudHighlightColour_0049cb58;
        return 1;
    }
    return 0;
}

/* Function start: 0x43B7C0 */
unsigned int draw_3d_scanner(void)
{
    const int *grid;
    const CockpitScannerGeometry *scanner;
    SphericalVector spherical;
    unsigned short colour;
    short object;
    short row;

    if (g_nCockpitDisplayMode_0049d71c != 0 && g_aiScannerGridRows_00469098[0] != -2) {
        scanner = &g_stCockpitLayout_0046e008.scanner[
            (int)g_cCockpitView_0059dab0];
        row = 0;
        grid = g_aiScannerGridRows_00469098;
        do {
            if (*grid == -1) {
                row++;
            } else {
                DrawViewportPixel(&g_stScreenViewport_005d21a0,
                                  (short)(scanner->centerX + row),
                                  (short)(scanner->centerY + *grid), 0xaa);
                if (*grid != 0)
                    DrawViewportPixel(&g_stScreenViewport_005d21a0,
                                      (short)(scanner->centerX + row),
                                      (short)(scanner->centerY - *grid), 0xaa);
                if (row != 0) {
                    DrawViewportPixel(&g_stScreenViewport_005d21a0,
                                      (short)(scanner->centerX - row),
                                      (short)(scanner->centerY + *grid), 0xaa);
                    if (*grid != 0)
                        DrawViewportPixel(&g_stScreenViewport_005d21a0,
                                          (short)(scanner->centerX - row),
                                          (short)(scanner->centerY - *grid),
                                          0xaa);
                }
            }
            grid++;
        } while (*grid != -2);
    }

    clear_head_up_display();
    g_nScannerTargetObject_0049b07c = g_acShipTarget_00495f20[0];
    if (g_nScannerTargetObject_0049b07c != -1 &&
        g_aeObjectClass_00495328[g_nScannerTargetObject_0049b07c] <
            OBJECT_CLASS_SHIP)
        g_nScannerTargetObject_0049b07c = -1;

    for (object = 1; object < 10; object++) {
        if (get_color(object, &colour) != 0) {
            rectangular_to_spherical(&g_aObjectViewPosition_0059afa0[object],
                                     &spherical);
            if (spherical.radius < 0xea6000) {
                rotational_pos_to_scanner_pos((signed char)object,
                                               &spherical);
                g_asScannerBackgroundColour_005a7ed0[object] =
                    (short)GetViewportPixel(
                        &g_stScreenViewport_005d21a0, g_nScannerCursorX_005a7e6c,
                        g_nScannerCursorY_005a7e6e);
                if (g_nScannerTargetObject_0049b07c != object)
                    DrawViewportPixel(&g_stScreenViewport_005d21a0,
                                      g_nScannerCursorX_005a7e6c,
                                      g_nScannerCursorY_005a7e6e,
                                      colour);
            }
        }
    }

    if (get_mode(1) == 5) {
        set_objective_range(1);
        CaptureSpriteBackground(&g_stScreenViewport_005d21a0,
                                g_pScannerMarkerBackground_005a7dc4,
                                g_nScannerCursorX_005a7e6c,
                                g_nScannerCursorY_005a7e6e,
                                g_pTargetLockShape_005a6bf4, 2);
        DrawSpriteDefault(&g_stScreenViewport_005d21a0,
                          g_nScannerCursorX_005a7e6c,
                          g_nScannerCursorY_005a7e6e,
                          g_pTargetLockShape_005a6bf4, 2);
    }

    if (g_nScannerTargetObject_0049b07c != -1 &&
        get_color(g_nScannerTargetObject_0049b07c, &colour) != 0) {
        if ((abs((int)g_nSpaceFrame_00493134) & 1) == 0)
            colour = g_cSecondaryViewBufferColour_0049cb4c;
        object = g_nScannerTargetObject_0049b07c;
        DrawViewportPixel(&g_stScreenViewport_005d21a0,
                          g_asScannerObjectX_005d1ea0[object],
                          g_asScannerObjectY_005a7e80[object], colour);
    }
    return 0;
}

/* Function start: 0x43BE1B */
void start_lock(unsigned short v)
{
    g_nTargetLockMusicCooldown_005d1e78 = -100;
    g_bTargetLockReadoutDirty_004934e8 = 0;
    g_nTargetLockCountdown_004934ec = v;
    g_nTargetLockMarkerAngle_004934f0 =
        RandomBelowOrEqual(0x167);
}

/* Function start: 0x43BE55 */
unsigned short starting_lock(unsigned short v)
{
    if (g_nTargetLockCountdown_004934ec == -1) {
        start_lock(v);
        return 1;
    }
    return 0;
}

/* Function start: 0x43BE8D */
void lock_off(void)
{
#if 0
    if (g_nTargetLockCountdown_004934ec > -1)
        g_bTargetLockReadoutDirty_004934e8 = 1;
    remove_message(g_pszMissileLocked_0049b280);
    g_nTargetLockCountdown_004934ec = -1;
#else
    if (g_nTargetLockCountdown_004934ec > -1) {
        g_bTargetLockReadoutDirty_004934e8 = 1;
        if (((ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1])[
                g_nSelectedReleaseWeaponIndex_004934e0].type == 0x13) {
            StopMusic(0);
            g_bRestorePlayerTarget_00493500 = 0;
        }
    }
    remove_message(g_pszMissileLocked_0049b280);
    g_nTargetLockCountdown_004934ec = -1;
    g_nTargetLockMusicCooldown_005d1e78 = -98;
#endif
}

/* Function start: 0x43BEFF */
short CheckTargetLockMalfunction(void)
{
    short countdown;

    if (malf(5) != 0) {
        countdown = -10;
        lock_off();
        countdown = (short)(countdown - RandomBelowOrEqual(30));
        g_nTargetLockCountdown_004934ec = countdown;
        PlaySfxWaveFileByNumber(7, -1, 0);
        return 1;
    }
    return 0;
}

/* Function start: 0x43BF67 */
short decrement_lock_time(short screenX)
{
    (void)screenX;
    if (g_nTargetLockCountdown_004934ec > 0) {
        if (malf(5) == 0) {
            g_nTargetLockCountdown_004934ec--;
            g_bTargetLockAcquired_0046c074 =
                g_nTargetLockCountdown_004934ec == 0;
            if (g_bTargetLockAcquired_0046c074 != 0) {
                if (CheckTargetLockMalfunction() == 0)
                    PlaySfxWaveFileByNumber(0x16, -1, 0);
                CockpitMessage(g_pszMissileLocked_0049b280,
                               DAT_004699ac, 2);
                return 1;
            }
            PlaySfxWaveFileByNumber(0x15, -1, 0);
        }
        return 1;
    }
    return 0;
}

/* Function start: 0x43C048 */
void target_locking(signed char target)
{
    enum ObjectType weaponType;
    short x;
    short y;

    if (target != -1 &&
        g_asShipSide_004955d0[(short)target] != g_asShipSide_004955d0[0] &&
        g_acPlayerComponentDamage_00493470[5] < 4) {
        x = g_asObjectScreenX_00493598[(short)target];
        if (x == -0x7fff)
            return;
        y = g_asObjectScreenY_00493628[(short)target];
        if (g_nTargetLockCountdown_004934ec < -1) {
            g_nTargetLockCountdown_004934ec++;
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
        if (g_nSelectedReleaseWeaponIndex_004934e0 == -1) {
            lock_off();
            return;
        }
#endif
        weaponType = *(enum ObjectType *)(
            &g_aShipWeapons_0059cab0[0][1] +
            g_nSelectedReleaseWeaponIndex_004934e0 * 7);
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

/* Function start: 0x43C30D */
void SetRectBounds(Viewport *viewport, unsigned short a, unsigned short b,
                   unsigned short c, unsigned short d)
{
    viewport->left = (short)a;
    viewport->top = (short)b;
    viewport->right = (short)c;
    viewport->bottom = (short)d;
}

/* Function start: 0x43C344 */
short GetRectHeight(const Viewport *viewport)
{
    return viewport->right - viewport->left;
}

/* Function start: 0x43C364 */
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
    viewport = *g_stSpaceTextContext_005d21c0.viewport;
    context = g_stSpaceTextContext_005d21c0;
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

        view = (int)g_cCockpitView_0059dab0;
        if (g_nCockpitDisplayMode_0049d71c != 0) {
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
                break;
            }
        } else {
            DrawTextAt(&context, DAT_004691e0[view * 2],
                       DAT_004691e0[view * 2 + 1], wrapped, 2);
        }
    }
    g_pszDisplayedHudMessage_0049aff0 = g_pszPendingHudMessage_0049afec;
}

/* Function start: 0x43C5B0 */
void RestoreHudMessageBackground(void)
{
#if 0
    /* WC1 had no separate restore helper. */
#else
    if (g_nHudMessageBackgroundDepth_0049b294 != 0) {
        RestoreSpriteBackground(
            &g_stScreenViewport_005d21a0,
            g_pHudMessageBackground_0049b28c,
            0,
            (short)(g_nHudMessageTop_0049ae92 +
                    g_nCockpitMessageOffsetY_005c849c),
            g_pHudMessageFrameShape_0049b288,
            0);
    }
    g_nHudMessageBackgroundDepth_0049b294 = 0;
#endif
}

/* Function start: 0x43C570 */
void ShowHudTextLine(void)
{
#if 0
    g_pszPendingHudMessage_0049afec = s;
    print_message_text(s, b);
#else
    CaptureSpriteBackground(
        &g_stScreenViewport_005d21a0,
        g_pHudMessageBackground_0049b28c,
        0,
        (short)(g_nHudMessageTop_0049ae92 +
                g_nCockpitMessageOffsetY_005c849c),
        g_pHudMessageFrameShape_0049b288,
        0);
    g_nHudMessageBackgroundDepth_0049b294++;
#endif
}

/* Function start: 0x43C601 */
void SetHudTextColour(char *text, int colour)
{
#if 0
    if (v != 0)
        EndCommMenu();
    print_message_text(g_pszDisplayedHudMessage_0049aff0, (unsigned char)g_cPrimaryViewBufferColour_0049cb88);
#else
    if (g_bCaptureHudMessageBackground_0049b290 != 0) {
        ShowHudTextLine();
        g_bCaptureHudMessageBackground_0049b290 = 0;
    }
    g_pszPendingHudMessage_0049afec = text;
    g_cHudMessageView_005d1c37 = (signed char)g_nCurrentView_00492fa8;
    print_message_text(g_pszPendingHudMessage_0049afec, colour);
#endif
}

/* Function start: 0x43C64D */
void ClearHudMessageDisplay(short closeCommMenu)
{
    if (closeCommMenu != 0)
        EndCommMenu();
    if (g_cHudMessageView_005d1c37 == g_nCurrentView_00492fa8 &&
        g_pszDisplayedHudMessage_0049aff0 != 0 &&
        g_nHudMessageBackgroundDepth_0049b294 != 0) {
        print_message_text(g_pszDisplayedHudMessage_0049aff0,
                           g_cPrimaryViewBufferColour_0049cb88);
        RestoreHudMessageBackground();
    }
    g_bCaptureHudMessageBackground_0049b290 = 1;
    if (g_bDisplayWingmanTargetData_0049347c != 0)
        SelectCockpitVduMode(1, 9);
}

/* Function start: 0x43C6D8 */
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
    if ((int)(unsigned char)g_cPrimaryViewBufferColour_0049cb88 == colourValue) {
        valid = savedBounds->left != -0x7fff;
        bounds = *savedBounds;
    } else {
        if (object == -1) {
            valid = 0;
        } else {
            valid = 1;
            if (g_asObjectScreenX_00493598[object] == -0x7fff)
                valid = 0;
        }
        if (valid != 0) {
            centerX = (short)(g_asObjectScreenX_00493598[object] +
                              g_nViewCenterX_0059a852);
            centerY = (short)(g_asObjectScreenY_00493628[object] +
                              g_nViewCenterY_0059a854);
            if ((short)GetTransformedShapeBounds(
                    &g_stViewBuffer_005d2b00, centerX, centerY,
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
            g_asShipSide_004955d0[object] == g_asShipSide_004955d0[0]) {
            colour = (unsigned char)DAT_004699a4;
        }
        if (solid != 0) {
            DrawViewportBorder(&g_stViewBuffer_005d2b00, bounds.left, bounds.top,
                               bounds.right, bounds.bottom, colour);
        } else {
            segmentLength =
                (short)(((int)bounds.right - bounds.left) / 6 + 1);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.left, bounds.top,
                             (short)(bounds.left + segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.left, bounds.bottom,
                             (short)(bounds.left + segmentLength), bounds.bottom,
                             colour);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.right, bounds.top,
                             (short)(bounds.right - segmentLength), bounds.top,
                             colour);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.right, bounds.bottom,
                             (short)(bounds.right - segmentLength), bounds.bottom,
                             colour);
            segmentLength =
                (short)(((int)bounds.bottom - bounds.top) / 6 + 1);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.left, bounds.top,
                             bounds.left, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.left, bounds.bottom,
                             bounds.left,
                             (short)(bounds.bottom - segmentLength), colour);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.right, bounds.top,
                             bounds.right, (short)(bounds.top + segmentLength),
                             colour);
            DrawViewportLine(&g_stViewBuffer_005d2b00, bounds.right, bounds.bottom,
                             bounds.right,
                             (short)(bounds.bottom - segmentLength), colour);
        }
        if (drawLockMarker != 0) {
            if ((int)(short)colour != (int)(unsigned char)g_cPrimaryViewBufferColour_0049cb88) {
                if (g_nTargetLockCountdown_004934ec > -1) {
                    g_nTargetLockMarkerAngle_004934f0 = (short)(
                        g_nTargetLockMarkerAngle_004934f0 +
                        g_anObjectRollRotation_0059d7e0[0] +
                        g_anObjectPitchRotation_0059b2a0[0]);
                    centerX = (short)(centerX +
                        ((CosFixed(g_nTargetLockMarkerAngle_004934f0) *
                          g_nTargetLockCountdown_004934ec * 2) >> 8));
                    centerY = (short)(centerY +
                        ((SinFixed(g_nTargetLockMarkerAngle_004934f0) *
                          g_nTargetLockCountdown_004934ec * 2) >> 8));
                    DrawSpriteDefault(&g_stViewBuffer_005d2b00, centerX, centerY,
                                      g_pTargetLockShape_005a6bf4, 1);
                    g_nTargetLockMarkerX_004691f4 = centerX;
                    g_nTargetLockMarkerY_005a7e28 = centerY;
                }
            } else if (g_nTargetLockMarkerX_004691f4 != -0x7fff) {
                DrawSolidColourSprite(&g_stViewBuffer_005d2b00,
                                      g_nTargetLockMarkerX_004691f4,
                                      g_nTargetLockMarkerY_005a7e28,
                                      g_pTargetLockShape_005a6bf4, 1,
                                      g_cPrimaryViewBufferColour_0049cb88);
                g_nTargetLockMarkerX_004691f4 = -0x7fff;
            }
        }
        if ((int)(unsigned char)g_cPrimaryViewBufferColour_0049cb88 == (int)(short)colour) {
            savedBounds->left = -0x7fff;
        } else {
            *savedBounds = bounds;
        }
    } else {
        savedBounds->left = -0x7fff;
    }
}

/* Function start: 0x43CBD3 */
void remove_nav_pointer(void)
{
    if (DAT_00469208 != -1)
        remove_object(DAT_00469208);
}

/* Function start: 0x43CBFD */
void draw_nav_pointer(void)
{
    FixedVector objectivePosition;
    FixedVector direction;
    FixedVector viewPosition;
    int distance;
    short active;
    short object;

    if (get_mode(1) == 5 &&
        g_nCannedSceneMode_0049021c != 4 &&
        (g_nCurrentView_00492fa8 == 0 || g_nCurrentView_00492fa8 == 4))
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
        g_acObjectOwner_00495208[object] = -1;
        g_asObjectScreenAngle_0059cd90[object] = 0;
        g_asObjectScreenScale_0059c950[object] = 0x100;
        g_aeObjectClass_00495328[object] = OBJECT_CLASS_PLANET;
        g_apObjectShape_0059d2f0[object] =
            g_pTargetLockShape_005a6bf4;
        DAT_00469208 = object;
        g_asObjectScreenX_00493598[object] = (short)0x8001;
        g_asObjectDistance_0059b4a0[object] = 0;
    }
    objectivePosition = g_aMissionObjectives_004932a8[
        (signed char)g_cCurrentObjective_004931cc].position;
    ComputeVectorDelta(&g_aShipPosition_00494550[WC1_EYE_OBJECT],
                       &objectivePosition, &direction);
    distance = Vector_magnitude(&direction);
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
    g_asObjectScreenX_00493598[object] = (short)(DivideFixed(
        MultiplyFixed(
            ((short)g_nScreenWidth_0046daa4 & ~1) << 7,
            viewPosition.x),
        viewPosition.z) >> 8);
    g_asObjectScreenY_00493628[object] = (short)(DivideFixed(
        MultiplyFixed(
            ((short)g_nScreenWidth_0046daa4 & ~1) << 7,
            viewPosition.y),
        viewPosition.z) >> 8);
    g_asObjectDistance_0059b4a0[object] = 0x4a38;
}

/* Function start: 0x43D386 */
unsigned int overlay_head_up_display(void)
{
    target_locking(g_acShipTarget_00495f20[0]);
    if (message_showing() && g_nCommSpeakerObject_0049b794 != -1) {
        g_cPreviousTargetObject_005a7df2 =
            (signed char)g_nCommSpeakerObject_0049b794;
        draw_target_box(g_ucHudHighlightColour_0049cb58,
                        g_cPreviousTargetObject_005a7df2,
                        0, 0, 2,
                        &g_stPreviousTargetBracketBounds_00469200);
    }
    if (g_nTargetLockCountdown_004934ec == 0) {
        if ((short)(g_nRenderedSpaceFrame_00493138 % 2) == 0)
            g_bTargetBracketVisible_004691d8 ^= 1;
        if (g_bTargetBracketVisible_004691d8 == 1) {
            draw_target_box(DAT_004699ac,
                            g_acShipTarget_00495f20[0],
                            g_nTargetLockMode_0046c078,
                            1, 1,
                            &g_stTargetBracketBounds_004691f8);
        }
    } else {
        draw_target_box(DAT_004699ac,
                        g_acShipTarget_00495f20[0],
                        g_nTargetLockMode_0046c078,
                        1, 1,
                        &g_stTargetBracketBounds_004691f8);
    }

    if (g_nCockpitDisplayMode_0049d71c != 0) {
        switch (g_cCockpitView_0059dab0) {
        case 0:
        case 2:
            goto centered_sight;
        case 1:
            DrawSpriteDefault(&g_stViewBuffer_005d2b00,
                              g_nViewCenterX_0059a852,
                              (short)(g_nViewCenterY_0059a854 - 1),
                              g_pTargetLockShape_005a6bf4, 0);
            break;
        case 3:
            DrawSpriteDefault(&g_stViewBuffer_005d2b00,
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
    DrawSpriteDefault(&g_stViewBuffer_005d2b00,
                      g_nViewCenterX_0059a852,
                      g_nViewCenterY_0059a854,
                      g_pTargetLockShape_005a6bf4, 0);

no_sight:
    DAT_0046c05c = 0;
    if (g_pszPendingHudMessage_0049afec != 0)
        SetHudTextColour(g_pszPendingHudMessage_0049afec,
                         (unsigned char)DAT_005a7f00);
    if (g_bMouseCursorVisible_0046a018 == 1) {
        g_nSavedMouseCursorX_005a7df8 = g_stMouseCursorState_0059ab10.x;
        g_nSavedMouseCursorY_005a7df4 = g_stMouseCursorState_0059ab10.y;
        CaptureSpriteBackground(g_stMouseCursorState_0059ab10.viewport,
                                g_abMouseCursorBackground_00475ff0,
                                g_stMouseCursorState_0059ab10.x,
                                g_stMouseCursorState_0059ab10.y,
                                g_stMouseCursorState_0059ab10.shape,
                                g_stMouseCursorState_0059ab10.frame);
        DrawSpriteDefault(g_stMouseCursorState_0059ab10.viewport,
                          g_stMouseCursorState_0059ab10.x,
                          g_stMouseCursorState_0059ab10.y,
                          g_stMouseCursorState_0059ab10.shape,
                          g_stMouseCursorState_0059ab10.frame);
    }
    return 0;
}

/* Function start: 0x43D7F2 */
void RestoreCockpitExplosionIfVisible(void)
{
    if (IsCockpitExplosionActive() &&
        g_pCockpitExplosionBackground_00469060 != 0) {
        RestoreCockpitExplosionBackground();
    }
}

/* Function start: 0x43D81F */
unsigned int RestoreTransientCockpitGraphics(void)
{
    if (g_bMouseCursorVisible_0046a018 == 1) {
        RestoreSpriteBackground(g_stMouseCursorState_0059ab10.viewport,
                                g_abMouseCursorBackground_00475ff0,
                                (short)g_nSavedMouseCursorX_005a7df8,
                                (short)g_nSavedMouseCursorY_005a7df4,
                                g_stMouseCursorState_0059ab10.shape,
                                (short)g_stMouseCursorState_0059ab10.frame);
    }
    if (g_cPreviousTargetObject_005a7df2 != -1) {
        draw_target_box(g_cPrimaryViewBufferColour_0049cb88,
                        g_cPreviousTargetObject_005a7df2,
                        0, 0, 2,
                        &g_stPreviousTargetBracketBounds_00469200);
        g_cPreviousTargetObject_005a7df2 = -1;
    }
    draw_target_box(g_cPrimaryViewBufferColour_0049cb88, g_acShipTarget_00495f20[0],
                    g_nTargetLockMode_0046c078, 1, 1,
                    &g_stTargetBracketBounds_004691f8);
    if (g_pszDisplayedHudMessage_0049aff0 != g_pszPendingHudMessage_0049afec && g_pszDisplayedHudMessage_0049aff0 != 0)
        ClearHudMessageDisplay(0);
    if (IsCockpitExplosionActive() &&
        g_pCockpitExplosionBackground_00469060 != 0) {
        if (g_nCockpitDisplayMode_0049d71c == 0) {
            CaptureSpriteBackground(
                &g_stScreenViewport_005d21a0, g_pCockpitExplosionBackground_00469060,
                g_stCockpitExplosionPosition_005a7e98.x,
                g_stCockpitExplosionPosition_005a7e98.y,
                g_pCockpitExplosionShape_00469064,
                g_nCockpitExplosionFrame_00469068);
        }
        if (g_nCockpitDisplayMode_0049d71c == 0) {
            DrawSpriteDefault(&g_stScreenViewport_005d21a0,
                              g_stCockpitExplosionPosition_005a7e98.x,
                              g_stCockpitExplosionPosition_005a7e98.y,
                              g_pCockpitExplosionShape_00469064,
                              g_nCockpitExplosionFrame_00469068);
        }
        DAT_0046900c = 0xff;
    }
    return 0;
}

/* Function start: 0x43D956 */
void SetHudMessageText(char *text, unsigned short colour,
                       unsigned short duration)
{
    if (g_bInflightComputerActive_00468754 == 0) {
        if (message_showing())
            ClearHudMessageDisplay(1);
        DAT_005a7f00 = colour;
        g_pszPendingHudMessage_0049afec = text;
        set_message_time(duration);
    }
}

/* Function start: 0x43D9DA */
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

/* Function start: 0x43DA68 */
void build_your_target_list(short *hasEnemy)
{
    signed char object;
    signed char targetIndex;

    *hasEnemy = 0;
    g_cViableTargetCount_0046c088 = 0;
    object = 1;
    do {
        if (g_aeObjectClass_00495328[(int)object] >= OBJECT_CLASS_SHIP &&
            g_aeSpecialManeuver_00495600[(int)object] !=
                SPECIAL_MANEUVER_UNKNOWN_9 &&
            g_asObjectScreenX_00493598[(int)object] != (short)0x8001 &&
            (unsigned short)g_asObjectDistance_0059b4a0[(int)object] <
                12000) {
            targetIndex = g_cViableTargetCount_0046c088;
            g_asViableTargetDistance_0059c470[(int)targetIndex] =
                g_asObjectDistance_0059b4a0[(int)object];
            g_acViableTarget_0059c920[(int)targetIndex] = object;
            g_cViableTargetCount_0046c088++;
            if (g_asShipSide_004955d0[(int)object] !=
                g_asShipSide_004955d0[0])
                *hasEnemy = 1;
        }
        object++;
    } while (object <= 9);
    if (g_cViableTargetCount_0046c088 > 1)
        sort_viable_target_list();
}

/* Function start: 0x43DD1B */
void cycle_onscreen_targets(void)
{
    signed char previousTarget;
    signed char index;
    short hasEnemy;

    previousTarget = g_acShipTarget_00495f20[0];
    build_your_target_list(&hasEnemy);
    if (g_cViableTargetCount_0046c088 == 0) {
        g_acShipTarget_00495f20[0] = -1;
    } else {
        for (index = 0;
             index < g_cViableTargetCount_0046c088 &&
             g_acViableTarget_0059c920[(int)index] !=
                 g_acShipTarget_00495f20[0];
             index++)
            ;
        do {
            index = (signed char)((index + 1) %
                                  g_cViableTargetCount_0046c088);
            g_acShipTarget_00495f20[0] =
                g_acViableTarget_0059c920[(int)index];
            if (hasEnemy == 0)
                break;
        } while (g_asShipSide_004955d0[
                     g_acShipTarget_00495f20[0]] ==
                 g_asShipSide_004955d0[0]);
    }
    if (g_acShipTarget_00495f20[0] != previousTarget)
        g_nTargetLockCountdown_004934ec = -1;
}

/* Function start: 0x43DDFC */
void check_target(void)
{
    short selectNewTarget;
    short oldTarget;
    short targetIndex;
    short hasEnemy;

    selectNewTarget = 1;
    oldTarget = g_acShipTarget_00495f20[0];
    if (oldTarget != -1 &&
        g_aeSpecialManeuver_00495600[oldTarget] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        g_acShipTarget_00495f20[0] = -1;
        oldTarget = -1;
    }
    if (g_nTargetLockMode_0046c078 != 0 &&
        (short)(g_nRenderedSpaceFrame_00493138 % 8) == 0 &&
        malf(5) != 0) {
        g_nTargetLockMode_0046c078 = 0;
        malf_sound();
    }
    if (oldTarget != -1 &&
        (g_nTargetLockMode_0046c078 != 0 ||
         (g_asObjectScreenX_00493598[oldTarget] != (short)0x8001 &&
          (g_nTargetLockMode_0046c078 != 0 ||
           g_asShipSide_004955d0[oldTarget] !=
               g_asShipSide_004955d0[0]))))
        return;
    if (oldTarget == -1)
        g_nTargetLockMode_0046c078 = 0;

    build_your_target_list(&hasEnemy);
    if (g_cViableTargetCount_0046c088 == 0) {
        if (g_nTargetLockMode_0046c078 != 0)
            g_acShipTarget_00495f20[0] = oldTarget;
        else
            g_acShipTarget_00495f20[0] = -1;
    } else {
        if (hasEnemy == 0 && oldTarget != -1 &&
            g_asShipSide_004955d0[oldTarget] ==
                g_asShipSide_004955d0[0] &&
            (g_nTargetLockMode_0046c078 != 0 ||
             g_asObjectScreenX_00493598[oldTarget] != (short)0x8001)) {
            selectNewTarget = 0;
            g_acShipTarget_00495f20[0] = oldTarget;
        }
        if (selectNewTarget != 0) {
            for (targetIndex = 0;
                 targetIndex < g_cViableTargetCount_0046c088 &&
                 g_asShipSide_004955d0[
                     (short)g_acViableTarget_0059c920[targetIndex]] ==
                     g_asShipSide_004955d0[0];
                 targetIndex++)
                ;
            g_acShipTarget_00495f20[0] =
                g_acViableTarget_0059c920[
                    targetIndex % (short)g_cViableTargetCount_0046c088];
        }
    }
    if (g_acShipTarget_00495f20[0] != oldTarget) {
        if (oldTarget != -1 && g_acShipTarget_00495f20[0] == -1)
            g_nTargetLockMode_0046c078 = 0;
        g_nTargetLockCountdown_004934ec = -1;
    }
}

/* Function start: 0x43E1B2 */
void update_missile_warning(void)
{
    if (FindMissileTargetingObject(0) != 0) {
        SetCockpitLightBlink(2, 1);
        if (g_nTrainSimActive_00469e2c == 0)
            spacetrack(3, 1, -1);
    } else {
        g_abCockpitLightGoal_005d1eb8[2] = 0;
    }
}

/* Function start: 0x43E297 */
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

/* Function start: 0x43E371 */
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

/* Function start: 0x43F110 */
void CopyTrainSimPilotViewToVdus(void)
{
#if 0
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
#else
    Viewport destination;
    ShortRect intersection;
    ShortRect rightBounds;
    ShortRect leftBounds;
    ShortRect cockpitBounds;

    destination = g_stPilotHandBackgroundViewport_005d2b40;
    rightBounds.left = g_stRightVduViewport_005d2b20.left;
    rightBounds.top = g_stRightVduViewport_005d2b20.top;
    rightBounds.right = g_stRightVduViewport_005d2b20.right;
    rightBounds.bottom = g_stRightVduViewport_005d2b20.bottom;
    leftBounds.left = g_stLeftVduViewport_005d2180.left;
    leftBounds.top = g_stLeftVduViewport_005d2180.top;
    leftBounds.right = g_stLeftVduViewport_005d2180.right;
    leftBounds.bottom = g_stLeftVduViewport_005d2180.bottom;
    cockpitBounds.left = g_stCockpitViewport_005d2160.left;
    cockpitBounds.top = g_stCockpitViewport_005d2160.top;
    cockpitBounds.right = g_stCockpitViewport_005d2160.right;
    cockpitBounds.bottom = g_stCockpitViewport_005d2160.bottom;

    if (GetViewportIntersection(&intersection, &cockpitBounds,
                                &rightBounds) != 0) {
        if (g_stTrainSimRightVduSource_0049b2c0.left == 0) {
            g_stTrainSimRightVduSource_0049b2c0 =
                g_stCockpitViewport_005d2160;
            g_stTrainSimRightVduSource_0049b2c0.left = rightBounds.left;
            g_stTrainSimRightVduSource_0049b2c0.right = cockpitBounds.right;
            g_stTrainSimRightVduSource_0049b2c0.top = cockpitBounds.top;
            g_stTrainSimRightVduSource_0049b2c0.bottom = rightBounds.bottom;
        }
        destination.left = (short)(
            g_stTrainSimRightVduSource_0049b2c0.left - cockpitBounds.left);
        destination.bottom = (short)(
            g_stTrainSimRightVduSource_0049b2c0.bottom - cockpitBounds.top);
        CopyViewportContents(&g_stTrainSimRightVduSource_0049b2c0,
                             &destination);
    }

    if (GetViewportIntersection(&intersection, &cockpitBounds,
                                &leftBounds) != 0) {
        if (g_stTrainSimLeftVduSource_0049b2d8.left == 0) {
            g_stTrainSimLeftVduSource_0049b2d8 =
                g_stCockpitViewport_005d2160;
            g_stTrainSimLeftVduSource_0049b2d8.left = cockpitBounds.left;
            g_stTrainSimLeftVduSource_0049b2d8.right = leftBounds.right;
            g_stTrainSimLeftVduSource_0049b2d8.top = cockpitBounds.top;
            g_stTrainSimLeftVduSource_0049b2d8.bottom = leftBounds.bottom;
        }
        destination.left = (short)(
            g_stTrainSimLeftVduSource_0049b2d8.left - cockpitBounds.left);
        destination.bottom = (short)(
            g_stTrainSimLeftVduSource_0049b2d8.bottom - cockpitBounds.top);
        CopyViewportContents(&g_stTrainSimLeftVduSource_0049b2d8,
                             &destination);
    }
#endif
}

/* Function start: 0x43E43A */
void animate_pilot(void)
{
    if (g_pPilotHandShape_005a7684 != 0) {
        determine_pilot_hand();
        if (DAT_0046900c != g_bStickIndicatorFrame_005a7dc8)
            DrawPilotHandFrame();
    }
}

/* Function start: 0x43E472 */
void ResetPilotHandAnimation(void)
{
    if (g_pPilotHandShape_005a7684 != 0) {
        DAT_0046900c = 0xff;
        CopyViewportContents(&DAT_005a6b60, &DAT_005a7550);
        animate_pilot();
    }
}

/* Function start: 0x43E4A8 */
void send_message(short obj, signed char message)
{
    if (g_nTrainSimActive_00469e2c == 0 &&
        g_aeObjectClass_00495328[obj] != OBJECT_CLASS_NULL &&
        g_nCannedSceneMode_0049021c == 0) {
        if (g_nYourWingman_0049346c != -1 &&
            g_nYourWingman_0049346c == obj &&
            g_bRadioSilence_0046af70 != 0) {
            g_acWingmanMessageState_0059d2c0[obj] = -1;
            return;
        }
        if (obj >= 0 && obj < 10 &&
            g_aeObjectClass_00495328[obj] >= OBJECT_CLASS_SHIP) {
            if (g_acShipRating_0059cd80[obj] != -1) {
                g_acWingmanMessageState_0059d2c0[obj] = message;
                return;
            }
            if (g_acObjectType_00493980[obj] ==
                    OBJECT_TYPE_TIGERS_CLAW ||
                g_nShipMissionIndices_0059c830[obj] ==
                    g_anShipMissionShip_00495e00[0]) {
                g_acWingmanMessageState_0059d2c0[obj] = message;
            } else if (g_asShipSide_004955d0[obj] == SIDE_KILRATHI) {
                g_acWingmanMessageState_0059d2c0[obj] = message;
                return;
            }
        }
    }
}

/* Function start: 0x43E5DA */
void npc_communication(void)
{
    signed char message;
    signed char obj;
    short messageActive;

    if (g_nCannedSceneMode_0049021c == 0 &&
        g_nTrainSimActive_00469e2c == 0) {
        messageActive = message_showing();
        obj = 1;
        while (messageActive == 0 && obj < 10) {
            if (g_aeObjectClass_00495328[(short)obj] >=
                    OBJECT_CLASS_SHIP &&
                g_acWingmanMessageState_0059d2c0[(short)obj] != -1) {
                message = g_acWingmanMessageState_0059d2c0[(short)obj];
                vid_equiv((short)obj, (short)message);
                g_acWingmanMessageState_0059d2c0[(short)obj] = -1;
            }
            messageActive = message_showing();
            obj++;
        }
        if (RandomBelowOrEqual(5000) > 4998 &&
            g_nCommSpeakerObject_0049b794 == -1) {
            obj = 1;
            while (obj < 10) {
                if (g_aeObjectClass_00495328[(short)obj] >=
                        OBJECT_CLASS_SHIP &&
                    g_asShipSide_004955d0[(short)obj] == SIDE_KILRATHI &&
                    (g_aeShipObjective_00495f08[(short)obj] ==
                         OBJECTIVE_ENGAGE_ENEMY ||
                     g_aeShipObjective_00495f08[(short)obj] ==
                         OBJECTIVE_DESTROY_SHIP) &&
                    (g_acShipRating_0059cd80[(short)obj] != -1 ||
                     RandomBelowOrEqual(100) < 20)) {
                    g_acWingmanMessageState_0059d2c0[(short)obj] =
                        (signed char)(RandomBelowOrEqual(2) + 2);
                    return;
                }
                obj++;
                if (g_nCommSpeakerObject_0049b794 != -1)
                    return;
            }
        }
    }
}

/* Function start: 0x43E870 */
void clear_cockpit_damage(void)
{
#if 0
    memset(g_asCockpitDamageState_005d1ee8, 0,
           sizeof(g_asCockpitDamageState_005d1ee8));
#else
    short *damageState;
    short damage;

    damageState = g_asCockpitDamageState_005d1ee8;
    for (damage = 0; damage < 4; damage++, damageState++)
        *damageState = 0;
#endif
}

/* Function start: 0x43E8B2 */
void explosion_draw(void)
{
    short damage;

    if (g_pCockpitPilotShape_0046905c == 0)
        g_pCockpitPilotShape_0046905c =
            FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 4, 0);
    damage = 0;
    do {
        if (g_asCockpitDamageState_005d1ee8[damage] == 1) {
            DrawSpriteDefault(
                &g_stScreenViewport_005d21a0,
                g_aaCockpitDamagePositions_00469228[
                    (int)g_cCockpitView_0059dab0][damage].x,
                g_aaCockpitDamagePositions_00469228[
                    (int)g_cCockpitView_0059dab0][damage].y,
                g_pCockpitPilotShape_0046905c, damage);
        }
        damage++;
    } while (damage < 4);
    FreePacketAndClear(&g_pCockpitPilotShape_0046905c, 0);
}

/* Function start: 0x43E9E2 */
unsigned int DrawPendingCockpitDamage(void)
{
    if (g_pCockpitPilotShape_0046905c == 0)
        g_pCockpitPilotShape_0046905c =
            FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 4, 0);
    DrawSpriteDefault(&g_stScreenViewport_005d21a0,
                      g_stCockpitExplosionPosition_005a7e98.x,
                      g_stCockpitExplosionPosition_005a7e98.y,
                      g_pCockpitPilotShape_0046905c,
                      g_nPendingCockpitDamage_005a7dcc);
    if (g_pPilotHandShape_005a7684 != 0) {
        DrawSpriteDefault(&DAT_005a7550,
                          (short)(g_stCockpitExplosionPosition_005a7e98.x -
                                  DAT_005a6b60.left),
                          (short)(g_stCockpitExplosionPosition_005a7e98.y -
                                  DAT_005a6b60.top),
                          g_pCockpitPilotShape_0046905c,
                          g_nPendingCockpitDamage_005a7dcc);
    }
    FreePacketAndClear(&g_pCockpitPilotShape_0046905c, 0);
    return 0;
}

/* Function start: 0x43EAE3 */
void RestoreCockpitExplosionBackground(void)
{
    if (IsCockpitExplosionActive() &&
        g_pCockpitExplosionShape_00469064 != 0 &&
        g_pCockpitExplosionBackground_00469060 != 0) {
        RestoreSpriteBackground(
            &g_stScreenViewport_005d21a0, g_pCockpitExplosionBackground_00469060,
            g_stCockpitExplosionPosition_005a7e98.x,
            g_stCockpitExplosionPosition_005a7e98.y,
            g_pCockpitExplosionShape_00469064,
            g_nCockpitExplosionFrame_00469068);
    }
}

/* Function start: 0x43EB46 */
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
                &g_stScreenViewport_005d21a0, g_pCockpitExplosionBackground_00469060,
                g_stCockpitExplosionPosition_005a7e98.x,
                g_stCockpitExplosionPosition_005a7e98.y,
                g_pCockpitExplosionShape_00469064,
                g_nCockpitExplosionFrame_00469068);
            DrawSpriteDefault(&g_stScreenViewport_005d21a0,
                              g_stCockpitExplosionPosition_005a7e98.x,
                              g_stCockpitExplosionPosition_005a7e98.y,
                              g_pCockpitExplosionShape_00469064,
                              g_nCockpitExplosionFrame_00469068);
            DAT_0046900c = 0xff;
        }
        return;
    }
    FreePacketAndClear(&g_pCockpitExplosionShape_00469064, 0);
}

/* Function start: 0x43EC50 */
void place_damage_on_cockpit(short damage)
{
#if 0
    if (g_nCurrentView_00492fa8 == 0 && g_nTrainSimActive_00469e2c == 0 &&
        g_asCockpitDamageState_005d1ee8[damage] == 0) {
        g_nPendingCockpitDamage_005a7dcc = damage;
        g_asCockpitDamageState_005d1ee8[damage] = 1;
        if (g_pCockpitExplosionShape_00469064 == 0) {
            explosion_draw();
            return;
        }
        if (IsCockpitExplosionActive() == 0) {
            g_nCockpitExplosionFrame_00469068 = 0x7fff;
            g_stCockpitExplosionPosition_005a7e98 =
                g_aaCockpitDamagePositions_00469228[
                    (signed char)g_cCockpitView_0059dab0][damage];
        }
    }
#else
    if (g_nCurrentView_00492fa8 == 0 &&
        g_asCockpitDamageState_005d1ee8[damage] == 0) {
        g_asCockpitDamageState_005d1ee8[damage] = 1;
        g_nPendingCockpitDamage_005d1c34 = damage;
        if (g_pCockpitExplosionShape_0049b048 == 0) {
            explosion_draw();
        } else if (IsCockpitExplosionActive() == 0) {
            g_stCockpitExplosionPosition_005d1e98 =
                g_aCockpitDamagePositions_0049ae98[damage];
            g_nCockpitExplosionFrame_0049b04c = 0x7fff;
        }
    }
#endif
}

/* Function start: 0x43ECD9 */
void vid_transmit(void)
{
    short randomFrame;
    short speaker;
    unsigned char *background;

    speaker = g_nCommSpeakerObject_0049b794;
    if (g_asShipSide_004955d0[speaker] == SIDE_NEUTRAL) {
        EndCommSessionWithWingman();
        return;
    }
    if ((g_nCockpitDisplayMode_0049d71c != 0 ||
         g_nRenderedSpaceFrame_00493138 % 2 != 0) &&
        g_nCommPortraitIndex_0049b79c != -1 &&
        g_aapszPilotSpeech_0059e220[g_nCommPortraitIndex_0049b79c] != 0 &&
        g_bVideoImagesSuppressed_0046af74 == 0) {
        if (g_aeSpecialManeuver_00495600[speaker] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
            if (g_asShipSide_004955d0[speaker] == SIDE_IMPERIAL) {
                DrawSpriteDefault(
                    &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
                    g_pCommStaticShape_0046927c,
                    (short)(g_asObjectCounter_00494be0[speaker] / 5));
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
        if (g_asShipSide_004955d0[g_nCommSpeakerObject_0049b794] ==
            SIDE_IMPERIAL) {
            background = g_pConfedCommBackground_00469278;
        } else {
            background = g_pKilrathiCommBackground_00469280;
        }
        DrawSpriteDefault(&DAT_005a7530, DAT_005a7530.left,
                          DAT_005a7530.top, background, 0);
        DrawSpriteDefault(
            &DAT_005a7530, DAT_005a7530.left, DAT_005a7530.top,
            g_apCommPortraitShapes_0059e180[g_nCommPortraitIndex_0049b79c],
            (short)g_nCommPortraitFrame_00469284);
    }
}

/* Function start: 0x43F042 */
void vid_equiv(short obj, short message)
{
    if (get_mode(1) != 4 &&
        g_nTrainSimActive_00469e2c == 0 &&
        g_nCannedSceneMode_0049021c == 0 && g_nCurrentView_00492fa8 == 0 &&
        message_showing() == 0)
        real_vid_transmit(obj, message);
}

/* Function start: 0x43F09D */
void update_dead_disp(short a)
{
    malf_noise(a, 1, g_ucVduStaticColour_0049cb60, 0x17, 0);
}

/* Function start: 0x43F0C3 */
void check_stranded(void)
{
    if (g_nTrainSimActive_00469e2c == 0 &&
        g_aMissionShips_00492290[
            g_nHomeMissionShipIndex_005d1e22].state == 3 &&
        any_enemy(0, 30000) == 0)
        g_nArcadeState_0049d75c = 3;
}

/* Function start: 0x43F2F5 */
void update_VDUs(void)
{
    short changed;

    SetTextContext(&DAT_005a74f0);
    if (g_nCockpitDisplayMode_0049d71c != 0) {
        DrawFilledViewportRect(
            &g_stScreenViewport_005d21a0,
            g_stCockpitLayout_0046e008.leftVduBounds[
                (int)g_cCockpitView_0059dab0].left,
            g_stCockpitLayout_0046e008.leftVduBounds[
                (int)g_cCockpitView_0059dab0].top,
            g_stCockpitLayout_0046e008.leftVduBounds[
                (int)g_cCockpitView_0059dab0].right,
            g_stCockpitLayout_0046e008.leftVduBounds[
                (int)g_cCockpitView_0059dab0].bottom,
            0);
        DrawFilledViewportRect(
            &g_stScreenViewport_005d21a0,
            g_stCockpitLayout_0046e008.rightVduBounds[
                (int)g_cCockpitView_0059dab0].left,
            g_stCockpitLayout_0046e008.rightVduBounds[
                (int)g_cCockpitView_0059dab0].top,
            g_stCockpitLayout_0046e008.rightVduBounds[
                (int)g_cCockpitView_0059dab0].right,
            g_stCockpitLayout_0046e008.rightVduBounds[
                (int)g_cCockpitView_0059dab0].bottom,
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
            if (g_nCockpitDisplayMode_0049d71c != 0)
                show_weapon_disp();
            update_status_text();
            break;
        case 2:
            if (g_nCockpitDisplayMode_0049d71c != 0) {
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
    if (get_mode(0) == 0)
        g_aHudMessageSlots_005a7dd0[0].text = 0;
    else
        UpdateMessage(&g_aHudMessageSlots_005a7dd0[0]);

    SetTextContext(&DAT_005a7700);
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
            if (g_nCockpitDisplayMode_0049d71c != 0)
                show_target_disp();
            DrawTargetRangeReadout();
            break;
        case 4:
            if (g_nCockpitDisplayMode_0049d71c != 0)
                show_communications_disp();
            RefreshCommunicationMenu();
            break;
        case 5:
            if (g_nCockpitDisplayMode_0049d71c != 0)
                show_navigation_disp();
            check_objectives();
            break;
        case 6:
            vid_transmit();
            break;
        }
    }
    if (get_mode(1) == 6 || get_mode(1) == 0)
        g_aHudMessageSlots_005a7dd0[1].text = 0;
    else
        UpdateMessage(&g_aHudMessageSlots_005a7dd0[1]);
    if (g_nTrainSimActive_00469e2c != 0 &&
        g_pPilotHandShape_005a7684 != 0)
        CopyTrainSimPilotViewToVdus();
}

/* Function start: 0x43F6C9 */
void update_cockpit(void)
{
    check_target();
    repair_internal_damage();
    if (g_cMissionObjectiveCount_00493294 != 0)
        update_objective_location(
            (short)(g_nSpaceFrame_00493134 %
                    (short)g_cMissionObjectiveCount_00493294));
    if (g_nCurrentView_00492fa8 == 0) {
        if (g_nCockpitDisplayMode_0049d71c == 0)
            RestoreCockpitExplosionBackground();
        update_lights();
        update_missile_warning();
        draw_3d_scanner();
        update_digital_readouts();
        update_VDUs();
        if (g_nCockpitDisplayMode_0049d71c == 0)
            animate_pilot();
        update_bars();
        draw_cockpit_lights();
        if (g_nCockpitDisplayMode_0049d71c == 0)
            cockpit_explosion();
        npc_communication();
    }
    fire_computer_graphic_missile();
    check_stranded();
}

/* Function start: 0x43F81B */
short IsCockpitWeaponShapeLoaded(void)
{
    return g_pCockpitWeaponShape_005d2b54 != 0;
}

/* Function start: 0x43F849 */
void PlayCockpitSelectionSfx(short selectionSound)
{
    (void)selectionSound;
    PlaySfxWaveFileByNumber(0x19, -1, 0);
}

/* Function start: 0x43F862 */
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

/* Function start: 0x43F8CD */
void SelectCockpitVduMode(short vdu, int mode)
{
    short changed;

    if (g_nCurrentView_00492fa8 != 0)
        return;
    if (malf(3) != 0 ||
        (mode == 4 && malf(4) != 0)) {
        vdu_malf(vdu, 0x17);
        return;
    }
    PlayCockpitSelectionSfx(g_asVduSelectionSound_00469000[vdu]);
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
        RefreshCommunicationMenu();
        break;
    case 5:
        InflightComputer();
        break;
    }
}
