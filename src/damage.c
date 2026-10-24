/*
 *  Cockpit component-damage display.
 *
 *  Address range 42c800-42cdaf (see docs/ORDER.md).
 *  Initialized damage tables follow the preceding sound/configuration literals.
 */
#define WC1_DAMAGE_SOURCE
#include "wc1.h"

unsigned char *pDamageDisplayBackground = 0; /* 0x0046a748 */
/* 0x0046a750 */
static ShortPoint aDamageDisplayPositions[9] = {
    {36, 37}, {36, 28}, {36, 30}, {36, 23}, {36, 19},
    {36, 15}, {36, 24}, {36, 16}, {36, 22}
};
/* 0x0046a778 */
const char *apszComponentNames[9] = {
    "Ion drive",
    "Power plant",
    "Shield gen'r",
    "Computer sys",
    "InterCom unit",
    "Target track",
    "Accel absorbers",
    "Ejector system",
    "Repair systems"
};
/* 0x0046a7a0 */
static unsigned char abDamageDisplayFrames[9] = {
    21, 22, 23, 24, 25, 26, 27, 28, 29
};
/* 0x0046a7b0 */
static const char *apszDamageSeverityNames[5] = {
    "Ok",
    "Light",
    "Moderate",
    "Heavy",
    "Destroyed"
};

static int nDisplayedComponentDamage; /* 0x00476554 */


/* Linker commons defined here; LINK orders them by identifier hash. */
short asInputButton2DoubleClick[2]; /* 0x0059e520 */
void *apPacketHandlesList[0x1000]; /* 0x0059e530 */
int aiPktHandleDeltas[0x1000]; /* 0x005a2530 */
int nPacketHandleTotal; /* 0x005a6530 */
Viewport *pAllocatedScreenViewport; /* 0x005a6534 */
int *pFullScreenRecord; /* 0x005a6538 */
unsigned short awScreenRowOffsets[202]; /* 0x005a6540 */
ScreenAllocation stCurrentScreenAlloc; /* 0x005a66e0 */

/* Function start: 0x42C800 */
void show_damage_disp(void)
{
    signed char component;
    char message[40];

    nDmgDispState = 0;
    cDamagedComponentCount = 0;
    for (component = 0; component < 9; component++) {
        if (acPlayerComponentDamageList[component] >= 1)
            cDamagedComponentCount++;
    }

    set_new_vdu(0);
    DrawTextAt(&stLeftVduTextContext,
               stLeftVdu.left, stLeftVdu.top,
               "DAMAGE REPORT", 2);
    DrawViewportLine(&stLeftVdu,
                     (short)(stLeftVdu.left + 2),
                     (short)(stLeftVdu.top + 6),
                     (short)(stLeftVdu.right - 2),
                     (short)(stLeftVdu.top + 6),
                     cPrimaryTextColour);
    if (cDamagedComponentCount == 0) {
        DrawTextAt(&stLeftVduTextContext, stLeftVdu.left,
                   (short)(stLeftVdu.top + 20),
                   "NO INTERNAL\n\nDAMAGE", 2);
        return;
    }

    component = cCurrentCockpitView;
    stWpnDispOrigin.x =
        (short)(aWeaponDisplayOrigins[component].x +
                stLeftVdu.left);
    stWpnDispOrigin.y =
        (short)(aWeaponDisplayOrigins[component].y +
                stLeftVdu.top);
    /* The original reloads X from memory for the sprite argument. */
    DrawSpriteDefault(&stLeftVdu,
                      stWpnDispOrigin.x,
                      stWpnDispOrigin.y,
                      pCockpitWeaponShapePtr, 0);
    sprintf(message, "%d Unit%c Damaged",
            (int)cDamagedComponentCount,
            (int)(signed char)(cDamagedComponentCount == 1 ? ' ' : 's'));
    ShowComponentHitHudMessage(message, cPrimaryTextColour, -1);
}

/* Function start: 0x42C970 */
void UpdateDamageDisplay(void)
{
    short previousComponentCount;
    signed char componentCount;
    signed char component;
    signed char attempts;
    signed char damage;
    signed char componentIndex;

    previousComponentCount = cDamagedComponentCount;
    componentCount = 0;
    for (componentIndex = 0; componentIndex < 9; componentIndex++) {
        if (acPlayerComponentDamageList[componentIndex] >= 1)
            componentCount++;
    }

    cDamagedComponentCount = componentCount;
    if ((short)componentCount != previousComponentCount) {
        InvalidateVduMode(0);
        return;
    }
    cDamagedComponentCount = componentCount;
    if (componentCount == 0)
        return;

    if (bCockpitlessView == 0) {
        nCurrentDmgDispTicks--;
        if (nCurrentDmgDispTicks > 0)
            return;
        if (nDamageDisplayPhase == 1) {
            component = cDamageDisplayComponentValue;
            attempts = 0;
            nCurrentDmgDispTicks = 50;
            do {
                component++;
                if (component >= 9)
                    component = 0;
                damage = acPlayerComponentDamageList[component];
                nDisplayedComponentDamage = (int)damage;
                if (nDisplayedComponentDamage >= 1) {
                    cDamageDisplayComponentValue = component;
                    break;
                }
                attempts++;
            } while (attempts < 9);

            sprintf(
                szDmgStateText,
                "%s\nDamage: %s",
                apszComponentNames[
                    cDamageDisplayComponentValue],
                apszDamageSeverityNames[(int)damage]);
            DrawTextAt(&stLeftVduTextContext,
                       (short)(stLeftVdu.left + 1),
                       (short)(stLeftVdu.top + 7),
                       szDmgStateText, 2);
            cDamageDisplayFrame =
                (signed char)abDamageDisplayFrames[
                    cDamageDisplayComponentValue];
            stDamageSpritePos =
                aDamageDisplayPositions[
                    cDamageDisplayComponentValue];
            stDamageSpritePos.x =
                (short)(stDamageSpritePos.x +
                        stWpnDispOrigin.x);
            stDamageSpritePos.y =
                (short)(stDamageSpritePos.y +
                        stWpnDispOrigin.y);
            CaptureSpriteBackground(
                &stLeftVdu, pDamageDisplayBackground,
                stDamageSpritePos.x,
                stDamageSpritePos.y,
                pCockpitWeaponShapePtr,
                (short)cDamageDisplayFrame);
            DrawViewportLine(
                &stLeftVdu, (short)(stLeftVdu.left + 36),
                (short)(stLeftVdu.top + 22),
                stDamageSpritePos.x,
                stDamageSpritePos.y,
                0xa9);
            DrawSpriteDefault(
                &stLeftVdu, stDamageSpritePos.x,
                stDamageSpritePos.y,
                pCockpitWeaponShapePtr,
                (short)cDamageDisplayFrame);
        } else {
            RestoreSpriteBackground(
                &stLeftVdu, pDamageDisplayBackground,
                stDamageSpritePos.x,
                stDamageSpritePos.y,
                pCockpitWeaponShapePtr,
                (short)cDamageDisplayFrame);
            stLeftVduTextContext.colour = cBlackColour;
            DrawTextAt(&stLeftVduTextContext,
                       (short)(stLeftVdu.left + 1),
                       (short)(stLeftVdu.top + 7),
                       szDmgStateText, 2);
            stLeftVduTextContext.colour = cPrimaryTextColour;
            DrawViewportLine(
                &stLeftVdu, (short)(stLeftVdu.left + 36),
                (short)(stLeftVdu.top + 22),
                stDamageSpritePos.x,
                stDamageSpritePos.y,
                cBlackColour);
            nCurrentDmgDispTicks = 2;
        }
        nDamageDisplayPhase =
            nDamageDisplayPhase == 0;
        return;
    }

    nCurrentDmgDispTicks--;
    if (nCurrentDmgDispTicks <= 0) {
        component = cDamageDisplayComponentValue;
        attempts = 0;
        nCurrentDmgDispTicks = 50;
        for (;;) {
            component++;
            if (component >= 9)
                component = 0;
            nDisplayedComponentDamage =
                (int)acPlayerComponentDamageList[component];
            if (nDisplayedComponentDamage >= 1) {
                cDamageDisplayComponentValue = component;
                return;
            }
            attempts++;
            if (attempts >= 9)
                return;
        }
    }

    sprintf(
        szDmgStateText,
        "%s\nDamage: %s",
        apszComponentNames[cDamageDisplayComponentValue],
        apszDamageSeverityNames[
            nDisplayedComponentDamage]);
    DrawTextAt(&stLeftVduTextContext, (short)(stLeftVdu.left + 1),
               (short)(stLeftVdu.top + 7),
               szDmgStateText, 2);
    cDamageDisplayFrame =
        (signed char)abDamageDisplayFrames[
            cDamageDisplayComponentValue];
    stDamageSpritePos =
        aDamageDisplayPositions[
            cDamageDisplayComponentValue];
    stDamageSpritePos.x =
        (short)(stDamageSpritePos.x +
                stWpnDispOrigin.x);
    stDamageSpritePos.y =
        (short)(stDamageSpritePos.y +
                stWpnDispOrigin.y);
    CaptureSpriteBackground(
        &stLeftVdu, pDamageDisplayBackground,
        stDamageSpritePos.x,
        stDamageSpritePos.y,
        pCockpitWeaponShapePtr,
        (short)cDamageDisplayFrame);
    DrawViewportLine(&stLeftVdu,
                     (short)(stLeftVdu.left + 36),
                     (short)(stLeftVdu.top + 22),
                     stDamageSpritePos.x,
                     stDamageSpritePos.y, 0xa9);
    DrawSpriteDefault(&stLeftVdu,
                      stDamageSpritePos.x,
                      stDamageSpritePos.y,
                      pCockpitWeaponShapePtr,
                      (short)cDamageDisplayFrame);
}

/* Function start: 0x42CDA0 */
unsigned short GetJoystickPresentUnused(void)
{
    return 1;
}
