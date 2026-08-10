/*
 *  Cockpit HUD: weapon, damage, target and message displays.
 *
 *  Address range 0x413000-0x417fff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: Draw*Panel/Report* family; string band 0x4692B8-0x4693A4.
 */
#include "wc1.h"

/* Function start: 0x413A10 */
void EmitTextString(void (__stdcall *writer)(short), const char *text)
{
    short character;

    character = (signed char)*text++;
    while (character != 0) {
        writer((short)character);
        character = (signed char)*text++;
    }
}

/* Function start: 0x413A40 */
void FormatTextTokens(void (__stdcall *writer)(short),
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
    FormatTextTokens((void (__stdcall *)(short))DrawTextCharacter,
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
    FormatTextTokens((void (__stdcall *)(short))AppendTextCharacter,
                     format, (unsigned long *)(&format + 1));
    if (g_pCurrentTextContext_0059af8c->viewport->pixels ==
        DAT_005a6ba0.pixels)
        DIBslam();
}

/* Function start: 0x413CB0 */
void AppendFormattedText(const char *format, ...)
{
    FormatTextTokens((void (__stdcall *)(short))AppendTextCharacter,
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
    ClearWaitCursorFlag();
    exit_squadron(text);
}

/* Function start: 0x413D20 */
unsigned short IsCockpitExplosionActive(void)
{
    return g_nCockpitExplosionFrame_00469068 < 8;
}

/* Function start: 0x413F70 */
unsigned int GetSeriesRecordField(char slot, int rec)
{
    int k = slot * 10;

    *(int *)(&DAT_005a7e30[0] + k) = rec;
    *(unsigned short *)(&DAT_005a7e30[4] + k) = *(unsigned short *)(rec + 4);
    *(unsigned short *)(&DAT_005a7e30[6] + k) = *(unsigned short *)(rec + 6);
    return 0;
}

/* Function start: 0x4140A0 */
short DrawHudMessageSlot(HudMessageSlot *slot)
{
    unsigned char savedColour;
    unsigned short oldDrawColour;
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
short KilrathiShipWithinRange(short obj, short range)
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
short CanEngageAutopilot(short showReason)
{
    char *reason;

    reason = 0;
    if (g_cMissionObjectiveCount_0059c46a == 0)
        return 0;
    if (distance_from_point(
            0,
            &g_aMissionObjectives_0059dac5[
                g_cCurrentObjective_0046c020].position) < 8000) {
        reason = "Already Near";
    } else if (KilrathiShipWithinRange(0, 16000) != 0) {
        reason = "Enemy Near";
    } else if (g_pActiveHazardField_0059bfe0 != 0) {
        reason = "Hazard Near";
    }
    if (showReason != 0 && reason != 0)
        set_global_message(reason, DAT_004699a8, 3);
    return reason == 0;
}

/* Function start: 0x414410 */
void *ClearHudTargetVectors(void)
{
    DAT_005a7eb8 = 0;
    DAT_005a7e70 = 0;
    DAT_005a7ebc = 0;
    DAT_005a7e74 = 0;
    DAT_005a7ebe = 0;
    DAT_005a7e76 = 0;
    return &DAT_005a7eb8;
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

/* Function start: 0x4149C0 */
void ClearMessageSlot(short i)
{
    DAT_0059ce18[i] = 0;
}

/* Function start: 0x4149E0 */
void ClearAutopilotFlag(void)
{
    DAT_005a7dca = 0;
}

/* Function start: 0x4149F0 */
int IsAutopilotEngaged(void)
{
    return 0 < DAT_005a7dca;
}

/* Function start: 0x414A10 */
unsigned short SetAutopilotFlag(unsigned short v)
{
    DAT_005a7dca = (unsigned char)v;
    return 0;
}

/* Function start: 0x414A20 */
void RefreshAutopilotHud(void)
{
    if (IsAutopilotEngaged() && (DAT_005a7dca = DAT_005a7dca - 1, DAT_005a7dca < 1))
        EndCommMenu();
}

/* Function start: 0x414AD0 */
void PlayTargetLockSfx(void)
{
    PlaySfxWaveFileByNumber(0x18, -1, 0);
}

/* Function start: 0x414AE0 */
void PlayShieldHitSfx(void)
{
    PlaySfxWaveFileByNumber(0x1f, -1, 0);
}

/* Function start: 0x414AF0 */
int malf(char component)
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

/* Function start: 0x415040 */
void InputFilterHook(void)
{
}

/* Function start: 0x415050 */
short sighted(short objective)
{
    return (g_aMissionObjectives_0059dac5[objective].flags & 4) == 4;
}

/* Function start: 0x415070 */
short visited(short objective)
{
    return (g_aMissionObjectives_0059dac5[objective].flags & 1) == 1;
}

/* Function start: 0x415090 */
short achieved(short objective)
{
    return (g_aMissionObjectives_0059dac5[objective].flags & 2) == 2;
}

/* Function start: 0x4150B0 */
void flag_objective(short objective, unsigned char flags)
{
    g_aMissionObjectives_0059dac5[objective].flags |= flags;
}

/* Function start: 0x4153D0 */
void set_next_destination(void)
{
    signed char objective;

    do {
        g_cCurrentNavPointIndex_0059c86c++;
        if (g_cCurrentNavPointIndex_0059c86c >=
            g_cMissionObjectiveCount_0059c46a) {
            g_cCurrentNavPointIndex_0059c86c = 0;
            break;
        }
        objective = g_abFlightPath_0059c000[
            g_cCurrentNavPointIndex_0059c86c];
    } while (objective == -1);
    g_cCurrentObjective_0046c020 =
        g_abFlightPath_0059c000[g_cCurrentNavPointIndex_0059c86c];
}

/* Function start: 0x4154C0 */
unsigned int CheckForShipQueuedToCurrentNavPoint(void)
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
unsigned int GetShipAiScratch(void)
{
    if (g_aeShipMissionType_0059c3f0[0] != MISSION_TYPE_ESCORT) {
        if (CheckForShipQueuedToCurrentNavPoint() == 0)
            return 0;
    }
    return 1;
}

/* Function start: 0x415530 */
void flag_reached(short objective, short reached)
{
    int type = g_aMissionObjectives_0059dac5[objective].type;

    if (reached != 0 && type != 1)
        flag_objective(objective, 1);
    if (objective == g_cCurrentObjective_0046c020)
        set_next_destination();
}

/* Function start: 0x415A70 */
void ClearWeaponHardpoints(void)
{
    short i = 10;

    do {
        DAT_005a7ea0[i] = 0;
        i = i - 1;
    } while (i != 0);
    DAT_00469090 = 0xffff;
}

/* Function start: 0x415FC0 */
void BeginMissileLockWarning(unsigned short v)
{
    DAT_0046c060 = 0;
    DAT_0046c064 = v;
    DAT_0046c068 = (short)RandomBelowOrEqual(0x167);
}

/* Function start: 0x415FF0 */
unsigned int GetHudMessageSlot(unsigned short v)
{
    if (DAT_0046c064 == -1) {
        BeginMissileLockWarning(v);
        return 1;
    }
    return 0;
}

/* Function start: 0x416010 */
void EndMissileLockWarning(void)
{
    if (DAT_0046c064 >= 0)
        DAT_0046c060 = 1;
    remove_message(PTR_s_MISSILE_LOCKED_004691d4);
    DAT_0046c064 = -1;
}

/* Function start: 0x416220 */
void SetRectBounds(int p, unsigned short a, unsigned short b,
                   unsigned short c, unsigned short d)
{
    *(unsigned short *)(p + 8) = a;
    *(unsigned short *)(p + 10) = b;
    *(unsigned short *)(p + 12) = c;
    *(unsigned short *)(p + 14) = d;
}

/* Function start: 0x416250 */
short GetRectHeight(int p)
{
    return *(short *)(p + 0xc) - *(short *)(p + 8);
}

/* Function start: 0x416260 */
void print_message_text(char *text, unsigned char colour)
{
    char source[84];
    char wrapped[84];
    Viewport viewport;
    TextContext context;
    char *input;
    char *output;
    short lastSpace = -1;
    short position = 0;
    short width;
    short x;
    short y;
    int view;

    if (text == 0)
        return;

    DosStrcpy(source, text);
    wrapped[0] = (char)DAT_004693b0;
    viewport = *DAT_005a6bc0.viewport;
    context = DAT_005a6bc0;
    context.viewport = &viewport;

    view = (int)g_cCockpitView_0059dab0;
    x = DAT_004691e0[view * 2];
    y = DAT_004691e0[view * 2 + 1];
    SetRectBounds((int)&viewport, (unsigned short)x, (unsigned short)y,
                  (unsigned short)(319 - x), (unsigned short)(y + 60));
    context.colour = colour;
    context.backgroundColour = 0xff;
    width = GetRectHeight((int)&viewport);

    input = source;
    output = wrapped;
    if (*input != 0) {
        short charactersPerLine = (short)(width / 6);

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

    if (DAT_0046a008 != 0) {
        switch (view) {
        case 0:
            y = (short)(y + 10);
            break;
        case 1:
            y = (short)(y + 25);
            break;
        case 2:
            y = (short)(y + 50);
            break;
        case 3:
            break;
        default:
            DAT_00469008 = DAT_00469004;
            return;
        }
    }
    DrawTextAt(&context, x, y, wrapped, 2);
    DAT_00469008 = DAT_00469004;
}

/* Function start: 0x416460 */
void ShowHudTextLine(char *s, unsigned char b)
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
                if (DAT_0046c064 > -1) {
                    DAT_0046c068 = (short)(
                        DAT_0046c068 +
                        g_anObjectRollRotation_0059d7e0[0] +
                        g_anObjectPitchRotation_0059b2a0[0]);
                    centerX = (short)(centerX +
                        ((CosFixed(DAT_0046c068) * DAT_0046c064 * 2) >> 8));
                    centerY = (short)(centerY +
                        ((SinFixed(DAT_0046c068) * DAT_0046c064 * 2) >> 8));
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
void ReleaseCurrentTargetLock(void)
{
    if (DAT_00469208 != -1)
        remove_object(DAT_00469208);
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
    if (DAT_00468754 == 0) {
        if (IsAutopilotEngaged())
            SetHudTextColour(1);
        DAT_005a7f00 = colour;
        DAT_00469004 = text;
        SetAutopilotFlag(duration);
    }
}

/* Function start: 0x416E20 */
void malf_noise(short vdu, int effect, unsigned int colour,
                short sound, short refresh)
{
    Viewport *viewport = vdu == 0 ? &DAT_005a6b80 : &DAT_005a7530;

    if (sound != -1)
        PlaySfxWaveFileByNumber(sound == 0x17 ? 0x16 : sound, -1, 0);
    snow_viewport(viewport, effect, colour);
    if (refresh != 0)
        set_new_vdu(vdu);
}

/* Function start: 0x4171D0 */
void determine_pilot_hand(void)
{
    short yaw;
    short pitch;

    yaw = g_nYawInput_0059d3f2 / 2;
    pitch = g_nPitchInput_0059d3f0 / 2;
    if (yaw > 0) {
        g_bStickIndicatorFrame_005a7dc8 = (unsigned char)MinShort(yaw + 8, 12);
        return;
    }
    if (yaw < 0) {
        g_bStickIndicatorFrame_005a7dc8 = (unsigned char)MinShort(4 - yaw, 8);
        return;
    }
    if (pitch > 0) {
        g_bStickIndicatorFrame_005a7dc8 = (unsigned char)MinShort(pitch + 12, 16);
        return;
    }
    if (pitch < 0) {
        g_bStickIndicatorFrame_005a7dc8 = (unsigned char)MinShort(-pitch, 4);
        return;
    }
    g_bStickIndicatorFrame_005a7dc8 = 0;
}

/* Function start: 0x417260 */
void DrawPilotHandFrame(void)
{
    int view = (int)g_cCockpitView_0059dab0;
    short frame = (short)(signed char)g_bStickIndicatorFrame_005a7dc8;
    short x = (short)(g_asPilotHandOrigins_0046e120[view * 2] -
                      DAT_005a6b60.left);
    short y = (short)(g_asPilotHandOrigins_0046e120[view * 2 + 1] -
                      DAT_005a6b60.top);

    CopyViewportContents(&DAT_005a7550, &DAT_005a7690);
    DrawSpriteDefault(&DAT_005a7690, x, y, DAT_005a7684, frame);
    DrawSpriteDefault(
        &DAT_005a7690,
        (short)(x + g_asPilotHandOffsets_00469018[frame * 2]),
        (short)(y + g_asPilotHandOffsets_00469018[frame * 2 + 1]),
        DAT_005a7684, 0x11);
    CopyViewportContents(&DAT_005a7690, &DAT_005a6b60);
    DAT_0046900c = g_bStickIndicatorFrame_005a7dc8;
}

/* Function start: 0x4173C0 */
void animate_pilot(void)
{
    if (DAT_005a7684 != 0) {
        determine_pilot_hand();
        if (DAT_0046900c != g_bStickIndicatorFrame_005a7dc8)
            DrawPilotHandFrame();
    }
}

/* Function start: 0x4173F0 */
void ResetPilotHandAnimation(void)
{
    if (DAT_005a7684 != 0) {
        DAT_0046900c = 0xff;
        CopyViewportContents(&DAT_005a6b60, &DAT_005a7550);
        animate_pilot();
    }
}

/* Function start: 0x417420 */
void send_message(short obj, signed char message)
{
    if (obj < 0 || obj >= 10 ||
        g_aeObjectClass_0059d100[obj] == OBJECT_CLASS_NULL)
        return;
    if (g_acShipRating_0059cd80[obj] != -1 ||
        g_aeObjectType_0059b560[obj] == OBJECT_TYPE_TIGERS_CLAW ||
        g_nShipMissionIndices_0059c830[obj] ==
            g_anShipMissionShip_0059d4b0[0] ||
        g_aeShipSide_0059d650[obj] == SIDE_KILRATHI)
        ((signed char *)g_aeShipObjective_0059d200)[obj + 0xc0] = message;
}

/* Function start: 0x417610 */
void clear_cockpit_damage(void)
{
    DAT_005a7ef0 = 0;
    DAT_005a7ef4 = 0;
    DAT_005a7ef8 = 0;
    DAT_005a7efc = 0;
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

/* Function start: 0x417B10 */
void ShowDamageMessage(short a)
{
    malf_noise(a, 1, DAT_004699b0, 0x17, 0);
}

/* Function start: 0x417F00 */
void PlayMissileLaunchSfx(void)
{
    PlaySfxWaveFileByNumber(0x19, -1, 0);
}
