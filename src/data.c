/*
 *  Shared initialized state and display tables: the event queue, cursor and
 *  screen geometry used by the event manager, the direction-frame tables used
 *  by the 3-D object code, and the cockpit light, bar, layout and pilot-hand
 *  tables.
 *
 *  Data only. This block lies between game.c's literals and the following
 *  units' literal-only data, so it is a separate object linked before the
 *  random/fixed-point unit; the Super Wing Commander symbols also name a
 *  `data` module. The code that owned it, if any, is unknown.
 */
#include "wc1.h"

/* 0x0046da90 */
InputEvent *pInputEventHead = 0;

/* 0x0046da94 */
InputEvent *pInputEventTail = 0;

/* 0x0046da98 */
int bInputEventPoolInitialized = 0;

/* 0x0046da9c */
unsigned char *pDrawnMouseCursorShape = 0;

/* 0x0046daa0 */
int nMouseCursorShowCount = 0;

/* 0x0046daa4 */
short nScreenWidth = 320;

/* 0x0046daa8 */
short nScreenHeight = 200;

/* Nothing in the shipped executable references these three zero words. */
/* 0x0046daac */
int DAT_0046daac = 0;

/* 0x0046dab0 */
int DAT_0046dab0 = 0;

/* 0x0046dab4 */
int DAT_0046dab4 = 0;

/* The seventh, all-zero entry fills the original span up to the direction
 * tables; no mode selects it. */
/* 0x0046dab8 */
ScreenViewportGeometry aScreenViewportGeometry[7] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 320, 128, 0, 24, 0, 24, -24576, -1 },
    { 320, 200, 0, 0, 0, 0, -1536, -1 },
    { 0, 0, 0, 0, 0, 0, 0, 0 }
};

/* 0x0046db28 */
signed char acDirectionShapeFrame[DIRECTION_VIEW_COUNT * DIRECTION_SHAPE_TABLE_COUNT] = {
    0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2,
    14, 13, 12, 11, 10, 9, 8, 9, 10, 11, 12, 13,
    15, 16, 17, 18, 19, 20, 21, 20, 19, 18, 17, 16,
    28, 27, 26, 25, 24, 23, 22, 23, 24, 25, 26, 27,
    29, 30, 31, 32, 33, 34, 35, 34, 33, 32, 31, 30,
    36,
    0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2,
    14, 13, 12, 11, 10, 9, 8, 9, 10, 11, 12, 13,
    15, 16, 17, 18, 19, 20, 21, 20, 19, 18, 17, 16,
    14, 13, 12, 11, 10, 9, 8, 9, 10, 11, 12, 13,
    1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 0,
    0, 1, 2, 3, 1, 3, 2, 1, 2, 3, 1, 3, 2,
    4, 5, 6, 4, 6, 5, 4, 5, 6, 4, 6, 5,
    7, 8, 9, 7, 9, 8, 7, 8, 9, 7, 9, 8,
    10, 11, 12, 10, 12, 11, 10, 11, 12, 10, 12, 11,
    13, 14, 15, 13, 15, 14, 13, 14, 15, 13, 15, 14, 16
};

/* 0x0046dbe8 */
signed char acDirectionShapeFlip[DIRECTION_VIEW_COUNT * DIRECTION_SHAPE_TABLE_COUNT] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0
};

/* 0x0046dca8 */
short aasCockpitLightX[5][7] = {
    {101, 101, 189, 137, 265, -99, 203},
    {198, 198, 133, 145, 234, 234, -99},
    {76, 76, 139, 257, 20, 20, -99},
    {194, 194, 96, 137, 96, 96, -99},
    {-99, -99, 179, 133, -99, -99, -99}
};

/* 0x0046dcf0 */
short aasCockpitLightY[5][7] = {
    {122, 145, 121, 7, 121, -99, 147},
    {120, 139, 114, 114, 179, 188, -99},
    {164, 183, 22, 181, 162, 178, -99},
    {119, 142, 136, 15, 126, 116, -99},
    {-99, -99, 115, 114, -99, -99, -99}
};

/* 0x0046dd38 */
signed char aacCockpitLightOffFrame[5][7] = {
    {14, 14, 14, 11, 13, 14, 14},
    {12, 12, 12, 13, 12, 12, 12},
    {9, 9, 9, 14, 9, 9, 9},
    {15, 15, 13, 11, 13, 13, 13},
    {5, 5, 5, 5, 5, 5, 5}
};

/* 0x0046dd60 */
signed char aacCockpitLightOnFrame[5][7] = {
    {3, 3, 3, 0, 2, 3, 3},
    {2, 2, 2, 3, 2, 2, 2},
    {1, 1, 1, 6, 1, 1, 1},
    {4, 4, 2, 0, 2, 2, 2},
    {1, 1, 1, 1, 1, 1, 1}
};

/* 0x0046dd88 */
CockpitBarDefinition aaCockpitBars[5][8] = {
    {
        {0, 215, 122, 219, 152, 31, 16, 5},
        {2, 143, 97, 175, 99, 33, 12, 1},
        {0, 114, 127, 123, 130, 4, 17, 6},
        {1, 114, 143, 123, 146, 4, 20, 9},
        {2, 109, 132, 112, 141, 4, 18, 7},
        {3, 125, 132, 128, 141, 4, 19, 8},
        {0, 109, 122, 128, 125, 4, 15, 4},
        {1, 109, 148, 128, 151, 4, 21, 10}
    },
    {
        {2, 96, 14, 134, 17, 39, 10, 0},
        {2, 185, 14, 223, 17, 39, 11, 1},
        {0, 213, 124, 220, 127, 4, 15, 5},
        {1, 213, 138, 220, 141, 4, 18, 8},
        {2, 205, 129, 208, 136, 4, 16, 6},
        {3, 225, 129, 228, 136, 4, 17, 7},
        {0, 205, 116, 228, 122, 7, 14, 4},
        {1, 205, 143, 228, 149, 7, 19, 9}
    },
    {
        {2, 249, 165, 293, 169, 45, 11, 3},
        {2, 131, 13, 189, 17, 59, 8, 0},
        {0, -99, -99, -99, -99, 0, 0, 0},
        {1, -99, -99, -99, -99, 0, 0, 0},
        {2, 45, 172, 51, 181, 7, 12, 4},
        {3, 70, 172, 76, 181, 7, 13, 5},
        {0, 49, 164, 72, 170, 7, 10, 2},
        {1, 49, 183, 72, 189, 7, 15, 7}
    },
    {
        {0, 123, 118, 126, 148, 31, 14, 3},
        {2, 144, 100, 174, 102, 31, 12, 1},
        {0, 207, 124, 216, 127, 4, 17, 6},
        {1, 207, 140, 216, 143, 4, 20, 9},
        {2, 202, 129, 205, 138, 4, 18, 7},
        {3, 218, 129, 221, 138, 4, 19, 8},
        {0, 202, 119, 221, 122, 4, 16, 5},
        {1, 202, 145, 221, 148, 4, 21, 10}
    },
    {
        {0, 180, 124, 184, 146, 22, 6, 2},
        {2, 143, 110, 175, 112, 33, 4, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, -99, -99, 0, 0, 0, 0, 0},
        {0, 134, 124, 138, 146, 32, 7, 3},
        {0, -99, -99, 0, 0, 0, 0, 0}
    }
};

/* 0x0046e008 */
CockpitLayout stCockpitLayout = {
    {
        {{99, 130}, {227, 122}, {38, 164}, {192, 126},
         {-99, -99}, {0, 0}},
        {{99, 140}, {227, 140}, {38, 184}, {192, 136},
         {-99, -99}, {0, 0}},
        {{201, 105}, {156, 14}, {197, 145}, {262, 102},
         {219, 115}, {0, 0}},
        {{112, 105}, {156, 19}, {105, 145}, {38, 102},
         {71, 115}, {0, 0}}
    },
    {
        {10, 133, 82, 198}, {0, 99, 73, 165}, {6, 1, 78, 66},
        {11, 111, 84, 176}, {48, 126, 120, 191}
    },
    {
        {236, 133, 309, 198}, {246, 97, 319, 165},
        {241, 1, 313, 66}, {235, 111, 308, 176},
        {198, 126, 270, 191}
    },
    {
        {159, 126, 145, 113, 173, 139},
        {103, 132, 89, 119, 117, 145},
        {160, 39, 146, 26, 174, 52},
        {159, 129, 145, 116, 173, 142},
        {159, 130, 145, 117, 173, 143}
    },
    {0, 0},
    {
        {120, 152, 203, 199}, {120, 152, 205, 199},
        {120, 151, 213, 199}, {120, 152, 205, 199},
        {120, 152, 203, 199}
    }
};

/* 0x0046e120 */
short asPilotHandOrigins[10] = {
    154, 187, 154, 187, 154, 187, 154, 187, 154, 187
};

/* The original contains ten initialized signed words here. Their
 * owner and purpose are not established by any identified reference. */
/* 0x0046e138 */
short DAT_0046e138[10] = {
    -400, -400, -400, -400, -400,
    -400, -400, -400, -400, -400
};
/* Linker commons defined here; LINK orders them by identifier hash. */
short nUnusedInputTimerFlag; /* 0x0059ab64 */
int aiInputEventSlotUsedTable[0x100]; /* 0x0059ab70 */
MouseCursorState stHostMouseState; /* 0x0059af70 */
TextContext *pCurTextContext; /* 0x0059af8c */
int nInputTickScale; /* 0x0059af90 */
FixedVector aObjectViewPositionsList[SPACE_OBJECT_COUNT]; /* 0x0059afa0 */
short anObjPitchRot[SPACE_OBJECT_COUNT]; /* 0x0059b2a0 */
int anShipSpeed[64]; /* 0x0059b320 */
short nCurrentSpaceFrameValue; /* 0x0059b420 */
unsigned int anShipAlertFlags[12]; /* 0x0059b430 */
FixedVector vPlayerAccel; /* 0x0059b460 */
int anShipFuelsTable[12]; /* 0x0059b470 */
unsigned short asObjDistancesTable[SPACE_OBJECT_COUNT]; /* 0x0059b4a0 */
ShortVector aShipFormationDeltaList[10]; /* 0x0059b520 */
enum ObjectType aeObjectTypeList[SPACE_OBJECT_COUNT]; /* 0x0059b560 */
short asObjectAnimationDelay[SPACE_OBJECT_COUNT]; /* 0x0059b660 */

