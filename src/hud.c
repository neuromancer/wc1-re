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
    ShutdownEventManager();
    exit_squadron(text);
}

/* Function start: 0x413D20 */
unsigned short IsCockpitExplosionActive(void)
{
    return g_nCockpitExplosionFrame_00469068 < 8;
}

/* Function start: 0x413DA0 */
void DrawCockpitBar(signed char bar, short percent)
{
    const CockpitBarDefinition *definition;
    short extent;
    short filledFrame;
    short emptyFrame;
    int view;

    view = (int)g_cCockpitView_0059dab0;
    if (view < 0 || view >= 5 || bar < 0 || bar >= 8)
        return;
    definition = &g_aaCockpitBars_0046dd88[view][(int)bar];
    if (definition->left == -99 || g_pCockpitDamageShape_005a76f4 == 0)
        return;
    if (percent < 0)
        percent = 0;
    if (percent > 100)
        percent = 100;
    extent = (short)((percent * definition->length) / 100);
    DAT_005a6be0.left = definition->left;
    DAT_005a6be0.top = definition->top;
    DAT_005a6be0.right = definition->right;
    DAT_005a6be0.bottom = definition->bottom;
    filledFrame = definition->filledFrame;
    emptyFrame = definition->emptyFrame;

    if (definition->direction < 2) {
        if (definition->direction == 1) {
            extent = (short)(definition->length - extent);
            filledFrame = definition->emptyFrame;
            emptyFrame = definition->filledFrame;
        }
        DAT_005a6be0.bottom = (short)(definition->bottom - extent);
        if (definition->top <= DAT_005a6be0.bottom)
            DrawSpriteDefault(&DAT_005a6be0, definition->left,
                              definition->top,
                              g_pCockpitDamageShape_005a76f4,
                              filledFrame);
        DAT_005a6be0.top = (short)(DAT_005a6be0.bottom + 1);
        DAT_005a6be0.bottom = definition->bottom;
        if (DAT_005a6be0.top <= definition->bottom)
            DrawSpriteDefault(&DAT_005a6be0, definition->left,
                              definition->top,
                              g_pCockpitDamageShape_005a76f4,
                              emptyFrame);
    } else {
        if (definition->direction == 3) {
            extent = (short)(definition->length - extent);
            filledFrame = definition->emptyFrame;
            emptyFrame = definition->filledFrame;
        }
        DAT_005a6be0.right = (short)(definition->right - extent);
        if (definition->left <= DAT_005a6be0.right)
            DrawSpriteDefault(&DAT_005a6be0, definition->left,
                              definition->top,
                              g_pCockpitDamageShape_005a76f4,
                              filledFrame);
        DAT_005a6be0.left = (short)(DAT_005a6be0.right + 1);
        DAT_005a6be0.right = definition->right;
        if (DAT_005a6be0.left <= definition->right)
            DrawSpriteDefault(&DAT_005a6be0, definition->left,
                              definition->top,
                              g_pCockpitDamageShape_005a76f4,
                              emptyFrame);
    }
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

/* Function start: 0x413FB0 */
void DrawCockpitReadout(signed char slot, char *text)
{
    unsigned char *record;
    TextContext *context;
    short x;
    short y;
    short previousRight;
    short height;

    record = &DAT_005a7e30[(int)slot * 10];
    context = *(TextContext **)record;
    x = *(short *)(record + 4);
    y = *(short *)(record + 6);
    if (context == 0 || x == -99)
        return;
    previousRight = *(short *)(record + 8);
    SetTextContext(context);
    SetTextCursor((unsigned short)x, (unsigned short)y);
    DrawFormattedText(text);
    if (previousRight > context->cursorX) {
        height = context->font == 0 ? 6 : *(short *)context->font;
        DrawFilledViewportRect(&DAT_005a6ba0, context->cursorX, y,
                               previousRight,
                               (short)(y + height - 1), DAT_0046999c);
    }
    *(short *)(record + 8) = context->cursorX;
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
void *reset_cockpit(void)
{
    memset(g_abCockpitLightGoal_005a7eb8, 0,
           sizeof(g_abCockpitLightGoal_005a7eb8));
    memset(g_abCockpitLightState_005a7e70, 0,
           sizeof(g_abCockpitLightState_005a7e70));
    return g_abCockpitLightGoal_005a7eb8;
}

/* Function start: 0x414440 */
unsigned int SetCockpitLightBlink(signed char light, short interval)
{
    if (interval < 20) {
        if (interval == 0 || g_nSpaceFrame_0059b420 % interval == 0)
            g_abCockpitLightGoal_005a7eb8[(int)light] ^= 1;
    } else {
        g_abCockpitLightGoal_005a7eb8[(int)light] = 0;
    }
    return 0;
}

/* Function start: 0x414490 */
void draw_cockpit_lights(void)
{
    int view;
    signed char light;
    short frame;

    view = (int)g_cCockpitView_0059dab0;
    if (view < 0 || view >= 5)
        return;
    light = 0;
    do {
        if (DAT_0046a008 != 0 ||
            g_abCockpitLightState_005a7e70[(int)light] !=
                g_abCockpitLightGoal_005a7eb8[(int)light]) {
            if (g_aasCockpitLightX_0046dca8[view][(int)light] != -99) {
                frame = g_abCockpitLightGoal_005a7eb8[(int)light]
                    ? g_aacCockpitLightOnFrame_0046dd60[view][(int)light]
                    : g_aacCockpitLightOffFrame_0046dd38[view][(int)light];
                DrawSpriteDefault(
                    &DAT_005a6ba0,
                    g_aasCockpitLightX_0046dca8[view][(int)light],
                    g_aasCockpitLightY_0046dcf0[view][(int)light],
                    g_pCockpitDamageShape_005a76f4, frame);
            }
            g_abCockpitLightState_005a7e70[(int)light] =
                g_abCockpitLightGoal_005a7eb8[(int)light];
        }
        light++;
    } while (light < 7);
}

/* Function start: 0x4145B0 */
void update_lights(void)
{
    ObjectTypeData *typeData;
    int fuelCapacity;
    short fuelPercent;

    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[0]];
    fuelCapacity = *(int *)&typeData->lifetime;
    if (fuelCapacity == 0)
        fuelPercent = 0;
    else
        fuelPercent = (short)((g_anShipFuel_0059b470[0] * 100) /
                              fuelCapacity);
    SetCockpitLightBlink(6, fuelPercent);
    DrawCockpitBar(0, fuelPercent);
    DrawCockpitBar(1, g_asShipWeaponEnergy_0059d470[0]);

    if (g_nTrainSimActive_00469e2c != 0)
        return;
    if (CanEngageAutopilot(0) == 0)
        SetCockpitLightBlink(3, 2);
}

/* Function start: 0x414690 */
void update_bars(void)
{
    ObjectTypeData *typeData;
    short forePercent;
    short aftPercent;
    char value[16];

    typeData = &g_aObjectTypeData_00466458[g_aeObjectType_0059b560[0]];
    DrawCockpitBar(2, typeData->armorFront == 0 ? 0 :
        (short)((g_aasShipArmor_0059d420[0][0] * 100) /
                typeData->armorFront));
    DrawCockpitBar(3, typeData->armorRear == 0 ? 0 :
        (short)((g_aasShipArmor_0059d420[0][1] * 100) /
                typeData->armorRear));
    DrawCockpitBar(4, typeData->armorRight == 0 ? 0 :
        (short)((g_aasShipArmor_0059d420[0][2] * 100) /
                typeData->armorRight));
    DrawCockpitBar(5, typeData->armorLeft == 0 ? 0 :
        (short)((g_aasShipArmor_0059d420[0][3] * 100) /
                typeData->armorLeft));
    forePercent = typeData->shieldFore == 0 ? 0 :
        (short)((g_aasShipShield_0059d5b0[0][0] * 100) /
                typeData->shieldFore);
    aftPercent = typeData->shieldAft == 0 ? 0 :
        (short)((g_aasShipShield_0059d5b0[0][1] * 100) /
                typeData->shieldAft);
    SetCockpitLightBlink(0, forePercent);
    DrawCockpitBar(6, forePercent);
    SetCockpitLightBlink(1, aftPercent);
    DrawCockpitBar(7, aftPercent);
    _itoa((int)g_aasShipShield_0059d5b0[0][0], value, 10);
    DrawCockpitReadout(4, value);
    _itoa((int)g_aasShipShield_0059d5b0[0][1], value, 10);
    DrawCockpitReadout(5, value);
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

/* Function start: 0x414980 */
short CheckVduModeChanged(short vdu)
{
    short mode;
    int previousMode;

    mode = (short)get_mode(vdu);
    previousMode = (int)DAT_0059ce18[vdu];
    if (mode != previousMode)
        set_new_vdu(vdu);
    return mode != previousMode;
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

/* Function start: 0x414A50 */
unsigned int update_digital_readouts(void)
{
    char value[16];
    long velocity;

    SetTextContext(&DAT_005a7720);
    _itoa((int)(short)((g_anShipSpeed_0059b320[0] >> 8) * 10),
          value, 10);
    DrawCockpitReadout(2, value);
    velocity = ComputeFixedVectorMagnitude(&g_aShipVelocity_0059c010[0]);
    velocity = MultiplyFixed(velocity, 0xa00);
    _itoa((int)(short)((unsigned long)velocity >> 8), value, 10);
    DrawCockpitReadout(3, value);
    return 0;
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
    g_cReleaseWeaponDisplayTicks_00469074 = 3;
    g_cReleaseWeaponDisplayState_00469078 = 0;
    g_nReleaseWeaponDisplayX_005a7dbc =
        (short)(g_aWeaponDisplayPositions_00468440[hardpoint].x +
                g_nWeaponDisplayOriginX_005a7788);
    g_nReleaseWeaponDisplayY_005a7dbe =
        (short)(g_aWeaponDisplayPositions_00468440[hardpoint].y +
                g_nWeaponDisplayOriginY_005a778a);
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
    short delta;

    if (g_cReleaseWeaponDisplayFrame_00469070 == -1)
        return;
    visible = DAT_0046c03c == 0 && (short)get_mode(0) == 1;
    if (g_cReleaseWeaponDisplayState_00469078 != 0 &&
        g_pReleaseWeaponDisplayBackground_0046906c != 0) {
        RestoreSpriteBackground(
            &DAT_005a6b80, g_pReleaseWeaponDisplayBackground_0046906c,
            g_nReleaseWeaponDisplayX_005a7dbc,
            g_nReleaseWeaponDisplayY_005a7dbe,
            g_pCockpitWeaponShape_005a7564,
            g_cReleaseWeaponDisplayFrame_00469070);
    }
    if (g_nReleaseWeaponDisplayY_005a7dbe > DAT_005a6b80.top - 10 &&
        g_nReleaseWeaponDisplayY_005a7dbe < DAT_005a6b80.bottom) {
        delta = (short)(signed char)g_cReleaseWeaponDisplayTicks_00469074;
        if (g_eReleaseWeaponDisplayType_005a7dc0 !=
            OBJECT_TYPE_SPACE_MINE)
            delta = -delta;
        g_nReleaseWeaponDisplayY_005a7dbe += delta;
        g_cReleaseWeaponDisplayTicks_00469074++;
        if (visible != 0 &&
            g_pReleaseWeaponDisplayBackground_0046906c != 0) {
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
void DrawWeaponDisplayPanel(void)
{
    ShipWeaponSlot *weapons;
    const char *releaseName;
    const char *gunName;
    short view;
    signed char count;
    signed char weapon;
    short frame;

    set_new_vdu(0);
    DrawTextAt(&DAT_005a74f0, DAT_005a6b80.left, DAT_005a6b80.top,
               "WEAPON DISPLAY", 2);
    DrawViewportLine(&DAT_005a6b80, (short)(DAT_005a6b80.left + 2),
                     (short)(DAT_005a6b80.top + 5),
                     (short)(DAT_005a6b80.right - 2),
                     (short)(DAT_005a6b80.top + 5), DAT_004699b4);

    weapons = (ShipWeaponSlot *)&g_aShipWeapons_0059cab0[0][1];
    releaseName = "";
    if (g_nSelectedReleaseWeaponIndex_0046c058 >= 0 &&
        g_nSelectedReleaseWeaponIndex_0046c058 <
            (signed char)g_aShipWeapons_0059cab0[0][0])
        releaseName = g_aObjectTypeData_00466458[
            weapons[g_nSelectedReleaseWeaponIndex_0046c058].type].displayName;
    gunName = "";
    if ((int)g_eSelectedGunType_0046c054 == 0x80)
        gunName = "Full Guns";
    else if ((int)g_eSelectedGunType_0046c054 >= 0 &&
             g_eSelectedGunType_0046c054 < OBJECT_TYPE_COUNT)
        gunName = g_aObjectTypeData_00466458[
            g_eSelectedGunType_0046c054].displayName;
    DrawFormattedText("Weapon: %s\n", releaseName);
    DrawFormattedText("Gun: %s\n", gunName);

    view = (short)g_cCockpitView_0059dab0;
    if (view < 0 || view >= 5 || g_pCockpitWeaponShape_005a7564 == 0)
        return;
    g_nWeaponDisplayOriginX_005a7788 =
        (short)(DAT_005a6b80.left + g_aWeaponDisplayOrigins_004684c0[view].x);
    g_nWeaponDisplayOriginY_005a778a =
        (short)(DAT_005a6b80.top + g_aWeaponDisplayOrigins_004684c0[view].y);
    DrawSpriteDefault(&DAT_005a6b80, g_nWeaponDisplayOriginX_005a7788,
                      g_nWeaponDisplayOriginY_005a778a,
                      g_pCockpitWeaponShape_005a7564, 0);
    count = (signed char)g_aShipWeapons_0059cab0[0][0];
    weapon = 0;
    while (weapon < count) {
        if (weapons[(int)weapon].hardpoint >= 0 &&
            weapons[(int)weapon].hardpoint < 10) {
            frame = (short)((int)weapons[(int)weapon].type * 2 +
                            weapons[(int)weapon].disabled - 0x2f);
            DrawSpriteDefault(
                &DAT_005a6b80,
                (short)(g_nWeaponDisplayOriginX_005a7788 +
                        g_aWeaponDisplayPositions_00468440[
                            weapons[(int)weapon].hardpoint].x),
                (short)(g_nWeaponDisplayOriginY_005a778a +
                        g_aWeaponDisplayPositions_00468440[
                            weapons[(int)weapon].hardpoint].y),
                g_pCockpitWeaponShape_005a7564, frame);
        }
        weapon++;
    }
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

/* Function start: 0x415770 */
void update_objective_location(short objective)
{
    MissionObjective *missionObjective;
    FixedVector delta;
    short range;
    short reachedRange;

    missionObjective = &g_aMissionObjectives_0059dac5[objective];
    if (sighted(objective) != 0 && visited(objective) != 0 &&
        g_cCurrentObjective_0046c020 != objective)
        return;
    ComputeVectorDelta(&g_aShipPosition_0059c490[0],
                       &missionObjective->position, &delta);
    range = FixedToShortSaturating(
        (int)ComputeFixedVectorMagnitude(&delta));
    if (sighted(objective) == 0 && range < 16000)
        flag_objective(objective, 4);
    reachedRange = (missionObjective->type == 3 ||
                    missionObjective->type == 4) ? 6000 : 1500;
    if (range < reachedRange)
        flag_reached(objective, 0);
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

/* Function start: 0x415A90 */
void clear_head_up_display(void)
{
    ClearWeaponHardpoints();
    DAT_00469090 = 0xffff;
    g_cPreviousTargetObject_005a7df2 = -1;
    g_stPreviousTargetBracketBounds_00469200.left = -0x7fff;
    g_stTargetBracketBounds_004691f8.left = -0x7fff;
    g_nTargetLockMarkerX_004691f4 = -0x7fff;
}

/* Function start: 0x415CE0 */
unsigned int overlay_head_up_display(void)
{
    signed char target;

    target = g_acShipTarget_0059ce60[0];
    if (target != -1 &&
        g_aeObjectClass_0059d100[(int)target] < OBJECT_CLASS_SHIP)
        target = -1;
    draw_target_box(DAT_004699ac, target,
                    g_nTargetLockMode_0046c078, 1, 1,
                    &g_stTargetBracketBounds_004691f8);
    g_cPreviousTargetObject_005a7df2 = target;
    return 0;
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

/* Function start: 0x416FD0 */
void check_target(void)
{
    short target;

    target = (short)g_acShipTarget_0059ce60[0];
    if (target < 0 || target >= WC1_SPACE_OBJECT_COUNT ||
        g_aeObjectClass_0059d100[target] < OBJECT_CLASS_SHIP ||
        g_aeSpecialManeuver_0059c3c0[target] ==
            SPECIAL_MANEUVER_UNKNOWN_9) {
        g_acShipTarget_0059ce60[0] = -1;
        g_nTargetLockMode_0046c078 = 0;
        DAT_0046c064 = -1;
    }
}

/* Function start: 0x417190 */
void update_missile_warning(void)
{
    if (missile_on_tail(0) == 0) {
        g_abCockpitLightGoal_005a7eb8[2] = 0;
        return;
    }
    SetCockpitLightBlink(2, 1);
    if (g_nTrainSimActive_00469e2c == 0)
        StartMusicTrack(3, 1, -1);
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

/* Function start: 0x4174F0 */
void npc_communication(void)
{
    signed char obj;

    if (g_nCannedSceneMode_00469fac != 0 ||
        g_nTrainSimActive_00469e2c != 0)
        return;
    obj = 1;
    do {
        if (g_aeObjectClass_0059d100[(int)obj] >= OBJECT_CLASS_SHIP &&
            g_acWingmanMessageState_0059d2c0[(int)obj] != -1) {
            /* The communications renderer consumes one queued speaker at a
             * time.  Clearing here preserves the original single-dispatch
             * cadence until its speech-selection helper is reconstructed. */
            g_acWingmanMessageState_0059d2c0[(int)obj] = -1;
            return;
        }
        obj++;
    } while (obj < 10);
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
    int view;

    if (g_pCockpitPilotShape_0046905c == 0)
        g_pCockpitPilotShape_0046905c =
            (unsigned char *)FetchDiskPacketRetrying(
                (short)g_cCockpitLogicalFile_005a7c74, 4, 0);
    view = (int)g_cCockpitView_0059dab0;
    if (view >= 0 && view < 5 && g_pCockpitPilotShape_0046905c != 0) {
        damage = 0;
        do {
            if (g_anCockpitDamageState_005a7ef0[damage] == 1) {
                DrawSpriteDefault(
                    &DAT_005a6ba0,
                    g_aaCockpitDamagePositions_00469228[view][damage].x,
                    g_aaCockpitDamagePositions_00469228[view][damage].y,
                    g_pCockpitPilotShape_0046905c, damage);
            }
            damage++;
        } while (damage < 4);
    }
    FreePacketAndClear((int *)&g_pCockpitPilotShape_0046905c, 0);
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
    if (g_nCockpitExplosionFrame_00469068 == 0x7fff)
        g_nCockpitExplosionFrame_00469068 = 0;
    if (!IsCockpitExplosionActive()) {
        FreePacketAndClear((int *)&g_pCockpitExplosionShape_00469064, 0);
        return;
    }
    if (g_nCockpitExplosionFrame_00469068 == 0)
        PlaySfxWaveFileByNumber(0x1b, -1, 0);
    g_nCockpitExplosionFrame_00469068++;
    if (!IsCockpitExplosionActive() ||
        g_pCockpitExplosionShape_00469064 == 0 ||
        g_pCockpitExplosionBackground_00469060 == 0)
        return;
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

/* Function start: 0x417B10 */
void ShowDamageMessage(short a)
{
    malf_noise(a, 1, DAT_004699b0, 0x17, 0);
}

/* Function start: 0x417B30 */
void check_stranded(void)
{
    int carrier;

    if (g_nTrainSimActive_00469e2c != 0)
        return;
    carrier = (int)g_nCarrierMissionShipIndex_005a7e2a;
    if (carrier >= 0 && carrier < WC1_MISSION_SHIP_COUNT &&
        g_aMissionShips_0046c948[carrier].state == 3 &&
        any_enemy(0, 30000) == 0)
        g_nArcadeState_00469fb0 = 3;
}

/* Function start: 0x417B70 */
void update_VDUs(void)
{
    short changed;
    short mode;

    SetTextContext(&DAT_005a74f0);
    changed = CheckVduModeChanged(0);
    mode = (short)get_mode(0);
    switch (mode) {
    case 0:
        ShowDamageMessage(0);
        break;
    case 1:
        if (changed != 0 || DAT_0046a008 != 0)
            DrawWeaponDisplayPanel();
        InputFilterHook();
        break;
    case 8:
        show_info_disp();
        break;
    }
    if (mode == 0)
        g_aHudMessageSlots_005a7dd0[0].text = 0;
    else
        UpdateMessage(&g_aHudMessageSlots_005a7dd0[0]);

    SetTextContext(&DAT_005a7700);
    changed = CheckVduModeChanged(1);
    mode = (short)get_mode(1);
    switch (mode) {
    case 0:
        ShowDamageMessage(1);
        break;
    case 3:
        if (changed != 0 || DAT_0046a008 != 0)
            DrawTargetLockDisplay();
        else
            DrawTargetRangeReadout();
        break;
    }
    if (mode == 0 || mode == 6)
        g_aHudMessageSlots_005a7dd0[1].text = 0;
    else
        UpdateMessage(&g_aHudMessageSlots_005a7dd0[1]);
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
        overlay_head_up_display();
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
void PlayMissileLaunchSfx(void)
{
    PlaySfxWaveFileByNumber(0x19, -1, 0);
}
