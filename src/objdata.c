/**
 * @file
 * @ingroup data
 * @brief Object definitions, animation streams and weapon display coordinates.
 *
 * Data only. The initialized tables precede their name literals, between the
 * pilot-speech diagnostic and the navigation state. The original object
 * name, and any code it had, are unknown.
 */
#include "wc1.h"

/* Embedded animation command streams. Each command occupies the low word
 * of a four-byte source record. The 21 arrays form one contiguous data run. */
static unsigned int anAnimExplosion0[12] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa000, 0
}; /* 0x00466030 */
static unsigned int anAnimExplosion1[22] = {
    0, 0x406, 1, 0x406, 2, 0x406, 0x406, 3, 0x406, 0x406,
    4, 0x406, 0x406, 0x406, 5, 0x406, 0x406, 0x406, 0x406,
    0x406, 0xa000, 0
}; /* 0x00466060 */
static unsigned int anAnimExplosion2[14] = {
    0, 0x406, 1, 0x406, 2, 0x406, 3, 0x406, 4, 0x406, 5,
    6, 7, 0xa000
}; /* 0x004660b8 */
static unsigned int anAnimLaserSpark[8] = {
    0, 1, 2, 3, 4, 5, 0xa000, 0
}; /* 0x004660f0 */
static unsigned int anAnimBlueSpark[6] = {
    0, 1, 2, 3, 0xa000, 0
}; /* 0x00466110 */
static unsigned int anAnimRedSpark[6] = {
    0, 1, 2, 3, 0xa000, 0
}; /* 0x00466128 */
static unsigned int anAnimSparkTrail[6] = {
    0, 1, 2, 3, 0xa000, 0
}; /* 0x00466140 */
static unsigned int anAnimGirder[8] = {
    0, 1, 2, 3, 4, 5, 0x9000, 0
}; /* 0x00466158 */
static unsigned int anAnimTubing[8] = {
    6, 7, 8, 9, 10, 11, 0x9000, 0
}; /* 0x00466178 */
static unsigned int anAnimGlass[18] = {
    12, 13, 14, 15, 16, 17, 18, 19, 0x92, 0x91, 0x90,
    0x8f, 0x8e, 0x8d, 0x8c, 20, 0x9000, 0
}; /* 0x00466198 */
static unsigned int anAnimORing[8] = {
    21, 22, 23, 24, 25, 26, 0x9000, 0
}; /* 0x004661e0 */
static unsigned int anAnimPipe[8] = {
    27, 28, 29, 30, 31, 32, 0x9000, 0
}; /* 0x00466200 */
static unsigned int anAnimMetalSheet[14] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
}; /* 0x00466220 */
static unsigned int anAnimWing[18] = {
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 0x9000, 0
}; /* 0x00466258 */
static unsigned int anAnimMine[14] = {
    0, 1, 2, 0x41, 0x9000, 0, 0, 1, 2, 3, 4, 5, 0x9000, 0
}; /* 0x004662a0 */
static unsigned int anAnimAsteroidForward[16] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0x9000, 0
}; /* 0x004662d8 */
static unsigned int anAnimAsteroidShortForward[14] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
}; /* 0x00466318 */
static unsigned int anAnimAsteroidReverse[16] = {
    13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000, 0
}; /* 0x00466350 */
static unsigned int anAnimAsteroidShortReverse[14] = {
    12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000
}; /* 0x00466390 */
static unsigned int anAnimEjectedPilot[14] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
}; /* 0x004663c8 */
static unsigned int anAnimHyperspaceJumpFlash[22] = {
    0, 0x402, 0x404, 0x404, 0x408, 0x408, 0x408, 0x410,
    0x410, 0x410, 0x410, 0x820, 0x820, 0x810, 0x808, 0x804,
    1, 0x804, 0x804, 0x804, 0x804, 0xa000
}; /* 0x00466400 */

/* 0x00466458 */
ObjectTypeData aObjectTypeData[OBJECT_TYPE_COUNT] = {
    /* 0: Hornet */
    {
        "Hornet",
        OBJECT_CLASS_SHIP, 100, 125, 1024, 5, 200000, 5, 4000,
        42, 30, 0, 819, 8, 9, 8, 900,
        {
            5,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            28, 0, 0, 0, 2, 0, 0,
            28, 0, 0, 0, 3, 0, 1,
            29, 0, 0, 0, 4, 0, 1
        },
        40, 40, 45, 40, 30, 30, 0, 0
    },
    /* 1: Rapier */
    {
        "Rapier",
        OBJECT_CLASS_SHIP, 120, 135, 1024, 3, 250000, 6, 6000,
        45, 25, 0, 1075, 10, 10, 10, 1000,
        {
            9,
            24, 0, 0, 0, 14, 0, 1,
            24, 0, 0, 0, 18, 0, 1,
            25, 0, 0, 0, 12, 0, 0,
            25, 0, 0, 0, 20, 0, 0,
            31, 0, 0, 0, 16, 0, 1,
            30, 0, 0, 0, 15, 0, 0,
            30, 0, 0, 0, 17, 0, 1,
            28, 0, 0, 0, 13, 0, 1,
            28, 0, 0, 0, 19, 0, 1
        },
        80, 75, 60, 55, 50, 50, 0, 0
    },
    /* 2: Scimitar */
    {
        "Scimitar",
        OBJECT_CLASS_SHIP, 165, 160, 1152, 6, 280000, 7, 6000,
        36, 15, 0, 614, 6, 6, 7, 1300,
        {
            7,
            26, 0, 0, 0, 6, 0, 0,
            26, 0, 0, 0, 11, 0, 0,
            28, 0, 0, 0, 5, 0, 0,
            28, 0, 0, 0, 10, 0, 1,
            29, 0, 0, 0, 7, 0, 1,
            29, 0, 0, 0, 8, 0, 1,
            29, 0, 0, 0, 9, 0, 1
        },
        60, 50, 85, 80, 65, 65, 0, 0
    },
    /* 3: Raptor */
    {
        "Raptor",
        OBJECT_CLASS_SHIP, 180, 200, 1152, 3, 300000, 8, 8000,
        40, 25, 0, 588, 6, 5, 6, 2000,
        {
            10,
            25, 0, 0, 0, 23, 0, 0,
            25, 0, 0, 0, 28, 0, 0,
            26, 0, 0, 0, 21, 0, 1,
            26, 0, 0, 0, 30, 0, 1,
            29, 0, 0, 0, 22, 0, 0,
            29, 0, 0, 0, 29, 0, 1,
            31, 0, 0, 0, 24, 0, 1,
            31, 0, 0, 0, 27, 0, 1,
            30, 0, 0, 0, 26, 0, 1,
            33, 0, 0, 0, 25, 0, 1
        },
        70, 70, 100, 90, 80, 80, 0, 0
    },
    /* 4: Venture */
    {
        "Venture",
        OBJECT_CLASS_CAPITAL_SHIP, 240, 400, 1024, 5, 200000, 70,
        20000, 25, 10, 0, 256, 3, 3, 3, 4000,
        {
            2,
            27, 0, 0, 0, 51, 0, 0,
            27, 0, 0, 0, 50, 0, 0
        },
        150, 150, 110, 100, 100, 110, 0, 0
    },
    /* 5: Dilligent */
    {
        "Dilligent",
        OBJECT_CLASS_CAPITAL_SHIP, 240, 400, 1024, 4, 200000, 60,
        20000, 15, 10, 0, 128, 2, 2, 2, 10000,
        {
            1,
            27, 0, 0, 0, 54, 0, 0
        },
        120, 120, 80, 80, 60, 60, 0, 0
    },
    /* 6: Drayman */
    {
        "Drayman",
        OBJECT_CLASS_CAPITAL_SHIP, 240, 400, 1024, 4, 200000, 60,
        10000, 15, 10, 0, 128, 2, 2, 2, 20000,
        {
            1,
            27, 0, 0, 0, 54, 0, 0
        },
        120, 120, 80, 80, 60, 60, 0, 0
    },
    /* 7: Exeter */
    {
        "Exeter",
        OBJECT_CLASS_CAPITAL_SHIP, 500, 5000, 2048, 2, 200000, 200,
        30000, 20, 15, 0, 256, 2, 2, 2, 20000,
        {
            5,
            31, 0, 0, 0, 47, 0, 0,
            27, 0, 0, 0, 48, 0, 0,
            27, 0, 0, 0, 49, 0, 0,
            27, 0, 0, 0, 50, 0, 0,
            27, 0, 0, 0, 51, 0, 0
        },
        240, 240, 220, 200, 200, 200, 0, 0
    },
    /* 8: Tiger's Claw */
    {
        N_("Tiger's Claw"),
        OBJECT_CLASS_CAPITAL_SHIP, 700, 10000, 4096, 1, 200000,
        560, 30000, 0, 0, 0, 256, 1, 1, 1, 20000,
        {
            8,
            27, 0, 0, 0, 33, 0, 0,
            27, 0, 0, 0, 34, 0, 0,
            27, 0, 0, 0, 35, 0, 0,
            27, 0, 0, 0, 36, 0, 0,
            27, 0, 0, 0, 37, 0, 0,
            27, 0, 0, 0, 38, 0, 0,
            27, 0, 0, 0, 39, 0, 0,
            27, 0, 0, 0, 40, 0, 0
        },
        300, 300, 240, 200, 250, 250, 0, 0
    },
    /* 9: Salthi */
    {
        "Salthi",
        OBJECT_CLASS_SHIP, 120, 120, 1024, 10, 200000, 5, 4000,
        48, 30, 0, 972, 14, 12, 22, 1000,
        {
            3,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            28, 0, 0, 0, 31, 0, 1
        },
        35, 35, 30, 20, 15, 15, 0, 0
    },
    /* 10: Dralthi */
    {
        "Dralthi",
        OBJECT_CLASS_SHIP, 160, 140, 1024, 6, 200000, 7, 6000,
        40, 23, 0, 768, 10, 14, 10, 1200,
        {
            7,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            33, 0, 0, 0, 32, 0, 0,
            33, 0, 0, 0, 32, 0, 1,
            33, 0, 0, 0, 32, 0, 1,
            29, 0, 0, 0, 31, 0, 0,
            29, 0, 0, 0, 31, 0, 1
        },
        50, 50, 45, 35, 30, 30, 0, 0
    },
    /* 11: Krant */
    {
        "Krant",
        OBJECT_CLASS_SHIP, 140, 126, 1024, 5, 200000, 6, 6000,
        36, 20, 0, 716, 7, 10, 7, 1200,
        {
            6,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            30, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 0,
            29, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 1
        },
        80, 80, 90, 100, 80, 80, 0, 0
    },
    /* 12: Gratha */
    {
        "Gratha",
        OBJECT_CLASS_SHIP, 140, 126, 1024, 4, 200000, 7, 7000,
        32, 20, 0, 614, 6, 6, 14, 1400,
        {
            10,
            24, 0, 0, 0, 0, 0, 0,
            24, 0, 0, 0, 1, 0, 0,
            26, 0, 0, 0, 21, 0, 0,
            26, 0, 0, 0, 30, 0, 0,
            31, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 0,
            29, 0, 0, 0, 31, 0, 1,
            33, 0, 0, 0, 32, 0, 1,
            33, 0, 0, 0, 32, 0, 1,
            33, 0, 0, 0, 32, 0, 1
        },
        100, 95, 140, 120, 100, 100, 0, 0
    },
    /* 13: Jalthi */
    {
        "Jalthi",
        OBJECT_CLASS_SHIP, 160, 180, 1024, 7, 200000, 7, 8000,
        28, 20, 0, 512, 5, 5, 5, 1600,
        {
            8,
            25, 0, 0, 0, 41, 0, 0,
            25, 0, 0, 0, 44, 0, 0,
            24, 0, 0, 0, 42, 0, 0,
            24, 0, 0, 0, 43, 0, 0,
            24, 0, 0, 0, 45, 0, 0,
            24, 0, 0, 0, 46, 0, 0,
            30, 0, 0, 0, 31, 0, 1,
            29, 0, 0, 0, 31, 0, 0
        },
        160, 160, 200, 100, 170, 170, 0, 0
    },
    /* 14: Spikeri */
    {
        "Spikeri",
        OBJECT_CLASS_CAPITAL_SHIP, 200, 200, 1536, 4, 200000, 45,
        12000, 15, 10, 0, 460, 4, 4, 4, 4000,
        { 0 },
        70, 70, 80, 80, 60, 60, 0, 0
    },
    /* 15: Dorkir */
    {
        "Dorkir",
        OBJECT_CLASS_CAPITAL_SHIP, 260, 400, 2048, 5, 200000, 35,
        24000, 15, 10, 0, 204, 2, 2, 2, 5000,
        {
            4,
            27, 0, 0, 0, 54, 0, 0,
            33, 0, 0, 0, 55, 0, 1,
            33, 0, 0, 0, 55, 0, 1,
            33, 0, 0, 0, 55, 0, 1
        },
        170, 100, 90, 60, 90, 90, 0, 0
    },
    /* 16: Lumbari */
    {
        "Lumbari",
        OBJECT_CLASS_CAPITAL_SHIP, 260, 400, 2048, 5, 200000, 35,
        16000, 15, 10, 0, 204, 2, 2, 2, 5000,
        {
            4,
            27, 0, 0, 0, 54, 0, 0,
            33, 0, 0, 0, 55, 0, 1,
            33, 0, 0, 0, 55, 0, 1,
            33, 0, 0, 0, 55, 0, 1
        },
        70, 70, 80, 80, 60, 60, 0, 0
    },
    /* 17: Ralari */
    {
        "Ralari",
        OBJECT_CLASS_CAPITAL_SHIP, 325, 3000, 4096, 3, 200000, 90,
        20000, 15, 10, 0, 256, 2, 2, 2, 18000,
        {
            7,
            31, 0, 0, 0, 47, 0, 0,
            27, 0, 0, 0, 48, 0, 0,
            27, 0, 0, 0, 49, 0, 0,
            27, 0, 0, 0, 50, 0, 0,
            27, 0, 0, 0, 51, 0, 0,
            27, 0, 0, 0, 52, 0, 0,
            27, 0, 0, 0, 53, 0, 0
        },
        200, 120, 200, 90, 180, 180, 0, 0
    },
    /* 18: Fralthi */
    {
        "Fralthi",
        OBJECT_CLASS_CAPITAL_SHIP, 450, 10000, 4096, 2, 200000, 110,
        30000, 15, 10, 0, 256, 2, 2, 2, 10000,
        {
            8,
            31, 0, 0, 0, 47, 0, 0,
            31, 0, 0, 0, 47, 0, 0,
            27, 0, 0, 0, 48, 0, 0,
            27, 0, 0, 0, 49, 0, 0,
            27, 0, 0, 0, 50, 0, 0,
            27, 0, 0, 0, 51, 0, 0,
            27, 0, 0, 0, 52, 0, 0,
            27, 0, 0, 0, 53, 0, 0
        },
        270, 170, 280, 140, 260, 260, 0, 0
    },
    /* 19: Snakeir */
    {
        "Snakeir",
        OBJECT_CLASS_CAPITAL_SHIP, 600, 10000, 2048, 1, 200000, 320,
        30000, 15, 10, 0, 204, 1, 1, 1, 10000,
        { 0 },
        70, 70, 80, 80, 60, 60, 0, 0
    },
    /* 20: Sivar */
    {
        "Sivar",
        OBJECT_CLASS_CAPITAL_SHIP, 400, 12000, 4096, 1, 200000, 200,
        32000, 20, 15, 0, 179, 1, 1, 1, 15000,
        {
            8,
            31, 0, 0, 0, 47, 0, 0,
            31, 0, 0, 0, 47, 0, 0,
            27, 0, 0, 0, 48, 0, 0,
            27, 0, 0, 0, 49, 0, 0,
            27, 0, 0, 0, 50, 0, 0,
            27, 0, 0, 0, 51, 0, 0,
            27, 0, 0, 0, 52, 0, 0,
            27, 0, 0, 0, 53, 0, 0
        },
        270, 170, 280, 140, 260, 260, 0, 0
    },
    /* 21: Kilrathi base */
    {
        N_("Star post"),
        OBJECT_CLASS_CAPITAL_SHIP, 400, 20000, 2048, 4, 200000, 120,
        32000, 0, 0, 0, 0, 0, 0, 0, 10000,
        {
            8,
            27, 0, 0, 0, 33, 0, 0,
            27, 0, 0, 0, 36, 0, 0,
            27, 0, 0, 0, 37, 0, 0,
            27, 0, 0, 0, 40, 0, 0,
            30, 0, 0, 0, 33, 0, 0,
            30, 0, 0, 0, 36, 0, 0,
            30, 0, 0, 0, 37, 0, 0,
            30, 0, 0, 0, 40, 0, 0
        },
        200, 200, 180, 180, 180, 180, 0, 0
    },
    { 0 }, /* 22: asteroid field */
    { 0 }, /* 23: mine field */
    /* 24: laser cannon bolt */
    {
        N_("Laser cannon"),
        OBJECT_CLASS_PROJECTILE, 10, 0, 512, 7, 30, 25, 0,
        160, 0, 0, 0, 0, 0, 0, 0, { 0 },
        0, 0, 0, 0, 0, 0, 0, 0
    },
    { N_("Neutron gun"),
      OBJECT_CLASS_PROJECTILE, 10, 1, 832, 14, 20, 40, 0,
      140, 0 }, /* 25: neutron particle gun */
    { N_("Mass driver"),
      OBJECT_CLASS_PROJECTILE, 10, 0, 512, 9, 25, 30, 0,
      120, 0 }, /* 26: mass driver cannon */
    { "",
      OBJECT_CLASS_PROJECTILE, 15, 0, 832, 15, 40, 50, 1000,
      150, 0 }, /* 27: turret bolt; resources alias laser */
    { N_("Dart DF"),
      OBJECT_CLASS_MISSILE, 20, 5, 768, 500, 120, 4, 14500,
      130, 0, 0, 1433, 15, 15, 15, 100 }, /* 28 */
    { N_("Javelin HS"),
      OBJECT_CLASS_MISSILE, 20, 5, 768, 400, 140, 4, 13500,
      110, 0, 0, 1689, 11, 11, 11, 100 }, /* 29 */
    { N_("Pilum FF"),
      OBJECT_CLASS_MISSILE, 20, 5, 768, 400, 160, 4, 10500,
      90, 0, 0, 1689, 11, 11, 11, 100 }, /* 30 */
    { N_("Spiculum IR"),
      OBJECT_CLASS_MISSILE, 20, 5, 768, 400, 110, 4, 11500,
      110, 0, 0, 1689, 11, 11, 11, 100 }, /* 31 */
    { "",
      OBJECT_CLASS_MISSILE, 25, 10, 768, 400, 200, 4, 30000,
      50, 0, 0, 1280, 10, 10, 10, 100 }, /* 32 */
    { N_("Porcupine"),
      OBJECT_CLASS_MINE, 20, 5, 768, 110, 120, 4, 10000,
      20, 20, (unsigned char *)anAnimMine,
      0, 0, 2, 2 },
    { "",
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidForward,
      0, 0, 1, 13 },
    { "",
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidShortForward,
      0, 0, 1, 12 },
    { "",
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidReverse,
      0, 0, 1, 13 },
    { "",
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidShortReverse,
      0, 0, 1, 12 },
    { "",
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidForward,
      0, 0, 2, 13 },
    { "",
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidShortForward,
      0, 0, 2, 12 },
    { "",
      OBJECT_CLASS_DEBRIS, 10, 4, 192, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidForward,
      0, 0, 2, 13 },
    { "",
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimGirder,
      0, 0, 2, 5 },
    { "",
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimTubing,
      0, 0, 2, 5 },
    { "",
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimMetalSheet,
      0, 0, 1, 11 },
    { "",
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimWing,
      0, 0, 1, 15 },
    { "",
      OBJECT_CLASS_DEBRIS, 20, 2, 768, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimGlass,
      0, 0, 1, 15 },
    { "",
      OBJECT_CLASS_DEBRIS, 2, 1, 1792, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimORing,
      0, 0, 1, 5 },
    { "",
      OBJECT_CLASS_DEBRIS, 6, 1, 1536, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimPipe,
      0, 0, 1, 5 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 768, 0, 0, -1, 6000,
      0, 0, (unsigned char *)anAnimExplosion0,
      0, 0, 1, 0 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, -1, 6000,
      0, 0, (unsigned char *)anAnimExplosion1,
      0, 0, 1, 0 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, -1, 6000,
      0, 0, (unsigned char *)anAnimExplosion2,
      0, 0, 1, 0 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimLaserSpark,
      0, 0, 1, 0 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimRedSpark,
      0, 0, 2, 3 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimBlueSpark,
      0, 0, 2, 3 },
    { "",
      OBJECT_CLASS_EXPLOSION, 1, 1, 256, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimSparkTrail,
      0, 0, 2, 3 },
    { "",
      OBJECT_CLASS_FIXED_OBJECT, 0, 0, 256, 0, 0, 0 },
    { "",
      OBJECT_CLASS_DEBRIS, 6, 1, 512, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimEjectedPilot,
      0, 0, 1, 12 },
    { "",
      OBJECT_CLASS_EXPLOSION, 0, 0, 1024, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimHyperspaceJumpFlash,
      0, 0, 1, 0 }
};

/* Child/hardpoint offsets. */
/* 0x004682f0 */
ShortVector aChildOffsets[CHILD_OFFSET_COUNT] = {
    { 120, 10, 20 }, { -120, 10, 20 }, { 75, 45, -30 },
    { -75, 45, -30 }, { 0, 50, 10 }, { -100, 10, -40 },
    { -90, 0, 30 }, { -30, -40, -30 }, { 0, 0, 0 },
    { 30, -40, -30 }, { 100, 10, -40 }, { 90, 0, 30 },
    { -140, 10, 30 }, { -100, 10, 0 }, { -75, 0, -40 },
    { -30, 10, -20 }, { 0, 10, 10 }, { 30, 10, -20 },
    { 75, 0, -40 }, { 100, 10, 0 }, { 140, 10, 30 },
    { -120, -10, 0 }, { -100, 10, -20 }, { -90, 0, 40 },
    { -30, 20, -20 }, { 0, 10, -80 }, { 0, 10, 10 },
    { 30, 20, -20 }, { 90, 0, 40 }, { 100, 10, -20 },
    { 120, -10, 0 }, { 0, 10, 10 }, { 0, 0, -60 },
    { 0, 0, 500 }, { -200, 0, 250 }, { 200, 0, 250 },
    { -300, 0, 0 }, { 300, 0, 0 }, { -200, 0, -250 },
    { 200, 0, -250 }, { 0, 0, -500 }, { -130, 40, 20 },
    { -110, 20, 20 }, { -90, 0, 20 }, { 130, 40, 20 },
    { 110, 20, 20 }, { 90, 0, 20 }, { 0, 0, 400 },
    { -50, -20, 350 }, { 50, 20, 350 }, { -150, 20, 0 },
    { 150, -20, 0 }, { -75, -20, -350 }, { 75, 20, -350 },
    { 0, 100, 350 }, { 0, 0, -300 }
#ifdef SDL_PORT
    /* SM2.EXE 0x4a8dd0, hardpoints 56..62. */
    , {-100, 35, -20}, {0, 0, 0}, {100, 35, -20},
    {-120, 38, -20}, {120, 38, -20}, {-50, 24, 40}, {50, 24, 40}
#endif
};

/* 0x00468440 */
ShortPoint aWeaponDisplayPositions[WEAPON_DISPLAY_POSITION_COUNT] = {
    {64, 28}, {8, 28}, {45, 25}, {27, 25}, {36, 23},
    {20, 32}, {27, 24}, {31, 31}, {36, 22}, {41, 31},
    {51, 32}, {45, 24}, {8, 32}, {18, 32}, {25, 11},
    {26, 32}, {36, 28}, {46, 32}, {47, 11}, {54, 32},
    {64, 32}, {12, 35}, {17, 37}, {26, 16}, {28, 31},
    {37, 41}, {37, 29}, {45, 31}, {47, 16}, {56, 37},
    {61, 35}, {0, 0}
#ifdef SDL_PORT
    /* SM2.EXE 0x4a8f50, Dralthi weapon-diagram positions. */
    , [56] = {26, 27}, {20, 20}, {44, 27}, {18, 24},
    {52, 24}, {29, 22}, {41, 22}
#endif
};

/* 0x004684c0 */
ShortPoint aWeaponDisplayOrigins[COCKPIT_VIEW_COUNT] = {
    {0, 16}, {0, 16}, {0, 16}, {0, 16}, {0, 16}
#ifdef SDL_PORT
    , [OBJECT_TYPE_DRALTHI] = {0, 16}
#endif
};
/* Linker commons defined here; LINK orders them by identifier hash. */
short nMenuInputRepeatDelay; /* 0x005a8208 */
unsigned char abMissionAuxData[0x28]; /* 0x005a8210 */

