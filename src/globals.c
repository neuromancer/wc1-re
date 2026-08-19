/*
 *  Definitions for the globals declared in include/globals.h.
 *
 *  Provisional: the original scatters these across the modules that own them,
 *  so this file has no counterpart in the shipped image and contributes no
 *  code.  It exists so that regrouping functions cannot break the link.
 */
#include "wc1.h"

/* Quarter-wave 16.16 cosine table embedded with the original raster
 * assembly.  Indices are tenths of a degree from 0 through 90 degrees. */
const int anRLEQuarterCosine[901] = { /* 0x0043d4bf */
    65536, 65536, 65536, 65535, 65534, 65534, 65532, 65531,
    65530, 65528, 65526, 65524, 65522, 65519, 65516, 65514,
    65510, 65507, 65504, 65500, 65496, 65492, 65488, 65483,
    65479, 65474, 65469, 65463, 65458, 65452, 65446, 65440,
    65434, 65427, 65421, 65414, 65407, 65399, 65392, 65384,
    65376, 65368, 65360, 65352, 65343, 65334, 65325, 65316,
    65306, 65296, 65287, 65277, 65266, 65256, 65245, 65234,
    65223, 65212, 65201, 65189, 65177, 65165, 65153, 65140,
    65128, 65115, 65102, 65088, 65075, 65061, 65048, 65033,
    65019, 65005, 64990, 64975, 64960, 64945, 64930, 64914,
    64898, 64882, 64866, 64850, 64833, 64816, 64799, 64782,
    64765, 64747, 64729, 64711, 64693, 64675, 64656, 64637,
    64618, 64599, 64580, 64560, 64540, 64520, 64500, 64480,
    64459, 64439, 64418, 64397, 64375, 64354, 64332, 64310,
    64288, 64266, 64243, 64220, 64197, 64174, 64151, 64128,
    64104, 64080, 64056, 64032, 64007, 63983, 63958, 63933,
    63907, 63882, 63856, 63830, 63804, 63778, 63752, 63725,
    63698, 63671, 63644, 63617, 63589, 63562, 63534, 63505,
    63477, 63449, 63420, 63391, 63362, 63332, 63303, 63273,
    63243, 63213, 63183, 63152, 63122, 63091, 63060, 63029,
    62997, 62966, 62934, 62902, 62870, 62837, 62805, 62772,
    62739, 62706, 62672, 62639, 62605, 62571, 62537, 62503,
    62468, 62434, 62399, 62364, 62328, 62293, 62257, 62222,
    62186, 62149, 62113, 62076, 62040, 62003, 61966, 61928,
    61891, 61853, 61815, 61777, 61739, 61700, 61662, 61623,
    61584, 61544, 61505, 61465, 61426, 61386, 61346, 61305,
    61265, 61224, 61183, 61142, 61101, 61059, 61018, 60976,
    60934, 60892, 60849, 60807, 60764, 60721, 60678, 60635,
    60591, 60547, 60504, 60459, 60415, 60371, 60326, 60281,
    60236, 60191, 60146, 60100, 60055, 60009, 59963, 59917,
    59870, 59824, 59777, 59730, 59683, 59635, 59588, 59540,
    59492, 59444, 59396, 59347, 59299, 59250, 59201, 59152,
    59102, 59053, 59003, 58953, 58903, 58853, 58803, 58752,
    58701, 58650, 58599, 58548, 58497, 58445, 58393, 58341,
    58289, 58236, 58184, 58131, 58078, 58025, 57972, 57918,
    57865, 57811, 57757, 57703, 57649, 57594, 57539, 57485,
    57430, 57374, 57319, 57264, 57208, 57152, 57096, 57040,
    56983, 56927, 56870, 56813, 56756, 56699, 56641, 56583,
    56526, 56468, 56410, 56351, 56293, 56234, 56175, 56116,
    56057, 55998, 55938, 55879, 55819, 55759, 55699, 55638,
    55578, 55517, 55456, 55395, 55334, 55273, 55211, 55149,
    55087, 55025, 54963, 54901, 54838, 54775, 54713, 54650,
    54586, 54523, 54459, 54396, 54332, 54268, 54204, 54139,
    54075, 54010, 53945, 53880, 53815, 53749, 53684, 53618,
    53552, 53486, 53420, 53354, 53287, 53221, 53154, 53087,
    53020, 52952, 52885, 52817, 52750, 52682, 52613, 52545,
    52477, 52408, 52339, 52270, 52201, 52132, 52063, 51993,
    51923, 51854, 51784, 51713, 51643, 51573, 51502, 51431,
    51360, 51289, 51218, 51146, 51075, 51003, 50931, 50859,
    50787, 50714, 50642, 50569, 50496, 50423, 50350, 50277,
    50203, 50130, 50056, 49982, 49908, 49834, 49760, 49685,
    49610, 49536, 49461, 49386, 49310, 49235, 49159, 49084,
    49008, 48932, 48856, 48779, 48703, 48626, 48549, 48472,
    48395, 48318, 48241, 48163, 48086, 48008, 47930, 47852,
    47774, 47695, 47617, 47538, 47459, 47380, 47301, 47222,
    47143, 47063, 46983, 46904, 46824, 46744, 46663, 46583,
    46502, 46422, 46341, 46260, 46179, 46098, 46016, 45935,
    45853, 45771, 45689, 45607, 45525, 45443, 45360, 45278,
    45195, 45112, 45029, 44946, 44862, 44779, 44695, 44612,
    44528, 44444, 44360, 44275, 44191, 44107, 44022, 43937,
    43852, 43767, 43682, 43597, 43511, 43425, 43340, 43254,
    43168, 43082, 42995, 42909, 42823, 42736, 42649, 42562,
    42475, 42388, 42301, 42213, 42126, 42038, 41950, 41862,
    41774, 41686, 41598, 41509, 41421, 41332, 41243, 41154,
    41065, 40976, 40887, 40797, 40708, 40618, 40528, 40438,
    40348, 40258, 40167, 40077, 39986, 39896, 39805, 39714,
    39623, 39532, 39441, 39349, 39258, 39166, 39074, 38982,
    38890, 38798, 38706, 38614, 38521, 38428, 38336, 38243,
    38150, 38057, 37964, 37870, 37777, 37684, 37590, 37496,
    37402, 37308, 37214, 37120, 37026, 36931, 36837, 36742,
    36647, 36552, 36457, 36362, 36267, 36172, 36076, 35981,
    35885, 35789, 35693, 35597, 35501, 35405, 35309, 35212,
    35116, 35019, 34923, 34826, 34729, 34632, 34535, 34437,
    34340, 34242, 34145, 34047, 33949, 33852, 33754, 33655,
    33557, 33459, 33361, 33262, 33163, 33065, 32966, 32867,
    32768, 32669, 32570, 32470, 32371, 32271, 32172, 32072,
    31972, 31872, 31772, 31672, 31572, 31472, 31372, 31271,
    31171, 31070, 30969, 30868, 30767, 30666, 30565, 30464,
    30363, 30261, 30160, 30058, 29956, 29855, 29753, 29651,
    29549, 29447, 29344, 29242, 29140, 29037, 28935, 28832,
    28729, 28626, 28523, 28420, 28317, 28214, 28111, 28007,
    27904, 27800, 27697, 27593, 27489, 27385, 27281, 27177,
    27073, 26969, 26865, 26760, 26656, 26551, 26447, 26342,
    26237, 26132, 26027, 25922, 25817, 25712, 25607, 25502,
    25396, 25291, 25185, 25080, 24974, 24868, 24762, 24656,
    24550, 24444, 24338, 24232, 24125, 24019, 23913, 23806,
    23699, 23593, 23486, 23379, 23272, 23165, 23058, 22951,
    22844, 22737, 22629, 22522, 22415, 22307, 22200, 22092,
    21984, 21876, 21769, 21661, 21553, 21445, 21336, 21228,
    21120, 21012, 20903, 20795, 20686, 20578, 20469, 20360,
    20252, 20143, 20034, 19925, 19816, 19707, 19598, 19489,
    19380, 19270, 19161, 19051, 18942, 18832, 18723, 18613,
    18504, 18394, 18284, 18174, 18064, 17954, 17844, 17734,
    17624, 17514, 17403, 17293, 17183, 17072, 16962, 16851,
    16741, 16630, 16520, 16409, 16298, 16187, 16076, 15966,
    15855, 15744, 15633, 15521, 15410, 15299, 15188, 15077,
    14965, 14854, 14742, 14631, 14519, 14408, 14296, 14185,
    14073, 13961, 13849, 13738, 13626, 13514, 13402, 13290,
    13178, 13066, 12954, 12842, 12729, 12617, 12505, 12393,
    12280, 12168, 12055, 11943, 11831, 11718, 11605, 11493,
    11380, 11268, 11155, 11042, 10929, 10817, 10704, 10591,
    10478, 10365, 10252, 10139, 10026, 9913, 9800, 9687,
    9574, 9461, 9347, 9234, 9121, 9008, 8894, 8781,
    8668, 8554, 8441, 8327, 8214, 8100, 7987, 7873,
    7760, 7646, 7533, 7419, 7305, 7192, 7078, 6964,
    6850, 6737, 6623, 6509, 6395, 6281, 6167, 6054,
    5940, 5826, 5712, 5598, 5484, 5370, 5256, 5142,
    5028, 4914, 4800, 4686, 4572, 4457, 4343, 4229,
    4115, 4001, 3887, 3773, 3658, 3544, 3430, 3316,
    3201, 3087, 2973, 2859, 2744, 2630, 2516, 2401,
    2287, 2173, 2059, 1944, 1830, 1716, 1601, 1487,
    1372, 1258, 1144, 1029, 915, 801, 686, 572,
    458, 343, 229, 114, 0
};

/* IFF chunk identifiers embedded between the original raster routines. */
const unsigned char abIffBmhdChunkId[4] = { 'B', 'M', 'H', 'D' }; /* 0x0043e778 */
const unsigned char abIffCmapChunkId[4] = { 'C', 'M', 'A', 'P' }; /* 0x0043e77c */
const unsigned char abIffBodyChunkId[4] = { 'B', 'O', 'D', 'Y' }; /* 0x0043e780 */

TitleMenuRegion aBarracksMenuRegions[21] = { /* 0x00463008 */
    { 1, 137, 88, 149, 94 },
    { 1, 110, 88, 136, 94 },
    { 1, 172, 88, 184, 94 },
    { 1, 185, 88, 210, 94 },
    { 1, 133, 98, 146, 107 },
    { 1, 100, 98, 132, 107 },
    { 1, 174, 98, 189, 107 },
    { 1, 190, 98, 220, 107 },
    { 1, 124, 114, 142, 128 },
    { 1, 81, 114, 123, 128 },
    { 1, 178, 114, 197, 128 },
    { 1, 198, 114, 238, 128 },
    { 1, 109, 141, 135, 164 },
    { 1, 50, 141, 108, 164 },
    { 1, 185, 141, 213, 164 },
    { 1, 214, 141, 268, 164 },
    { 1, 288, 39, 311, 85 },
    { 1, 9, 33, 39, 95 },
    { 1, 218, 37, 248, 78 },
    { 1, 86, 44, 181, 78 },
    { -1, 0, 0, 0, 0 }
};

short nHazardReferenceSpeed; /* 0x00465040 */
short nActiveHazards; /* 0x00465044 */
int bIxAudioEnabled = 1; /* 0x00465058 */
unsigned int bKeyEventQueueEnabled; /* 0x0046505c */
unsigned int nGuardedAllocationTotalBytes; /* 0x00465060 */
unsigned int nGuardedAllocationBytes; /* 0x00465064 */
unsigned int nGuardedAllocationPeakBytes; /* 0x00465068 */
int bShowKilrathiSagaCredits; /* 0x0046506c */
int bShowFrameRate; /* 0x00465070 */
int bDirectDrawModeCascadeEnabled = 1; /* 0x00465074 */
int bCockpitEnabled = 1; /* 0x0046507c */
int bMainWindowMinimized; /* 0x00465080 */
int bWindowMessagePumpActive; /* 0x004650a8 */
unsigned int bF1KeyLatch; /* 0x004650ac */
GuardedAllocation *pGuardedAllocationHead; /* 0x004650b0 */
const char szJoystickDevCapsFailure[26] = /* 0x004652dc */
    "Joystick: !joyGetDevCaps\n";
short nPacketError; /* 0x00465460 */
short nAutopilotFormationShipCount; /* 0x00465544 */
const short asEjectionPrimaryFrames[8] = { /* 0x00465550 */
    0, 1, 1, 3, 3, 0, 0, 0
};
const short asEjectionSecondaryFrames[8] = { /* 0x00465560 */
    -1, -1, 2, -1, 4, 0, 0, 0
};
const short asEjectionViewScript[12] = { /* 0x00465570 */
    3, 11, 14, 70, 3, 10, 14, 80, 3, 4, -1, 0
};
const char *pStrandedMessage = /* 0x00465588 */
    szStrandedMessage;
const char szStrandedMessage[72] = /* 0x0046558c */
    "\nWith your carrier\ndestroyed, you drift\nendlessly through\n"
    "the void...";
const char szViewTemplates[16] = "View Templates"; /* 0x004655d4 */
const char szStrandedTheEnd[12] = "THE END"; /* 0x004655e4 */
const short asMouthFramesByPhoneme[26] = { /* 0x004655f0 */
    0, 5, 4, 4, 1, 8, 4, 7, 0, 4, 4, 7, 5,
    4, 2, 5, 6, 4, 4, 4, 3, 4, 6, 4, 4, 4
};
const char szConversationIntegerFormat[4] = "%d"; /* 0x00465628 */
const char szConversationIntegerFormatAlt[4] = "%d"; /* 0x0046562c */
const char szCampaignDateFormat[12] = "%03d.%03d"; /* 0x00465630 */
const char szSavedCampaignDateFormat[12] = "%03d.%03d"; /* 0x0046563c */
const char szCampaignTimeFormat[12] = "%02d:%02d"; /* 0x00465648 */
const char szConversationTextFormat[12] = "%X%Y%F%s%P"; /* 0x00465654 */
const char szBriefingMapTextFormat[12] = "%X%Y%F%s%P"; /* 0x00465660 */
const char szCloseLookTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046566c */
signed char cScrambleLeftWalkerFrame = 7; /* 0x00465768 */
signed char cScrambleRightWalkerFrame = 10; /* 0x0046576c */
signed char acScrambleWalkerOverlayFrames[8] = { /* 0x00465770 */
    3, 4, 5, 3, 4, 6, 6, 3
};
short nScrambleLeftWalkerX = 70; /* 0x00465778 */
short nScrambleRightWalkerX = 170; /* 0x0046577c */
short nScrambleBackgroundX = 0; /* 0x00465780 */
const unsigned short ausScrambleActorAnimationA[8] = { /* 0x00465788 */
    0, 1, 2, 3, 4, 5, 0x80, 0
};
const unsigned short ausScrambleActorAnimationB[11] = { /* 0x00465798 */
    0, 0, 1, 2, 2, 2, 2, 2, 1, 1, 0x80
};
ScrambleAnimationActor aScrambleAnimationActors[5] = { /* 0x004657b0 */
    { 0, 0, 0, 0,  2,  0, 0,
      ausScrambleActorAnimationA, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 10,  0, 0,
      ausScrambleActorAnimationB, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 13,  0, 0,
      ausScrambleActorAnimationB, 0, 0, 0, 0 },
    { 0, 0, 0, 0,  0, -1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0,  1, -1, 0, 0, 0, 0, 0, 0 }
};
const ScrambleShipDetail aaScrambleShipDetails[4][32] = { /* 0x00465828 */
    {
        {  2,   23,   5 }, {  3, -112,  16 },
        {  6,  -94, -23 }, {  6,  -26,   5 },
        {  7,  -58, -11 }, {  7,  -98,   5 },
        {  7,  -14,  13 }, {  7,   60,   6 },
        {  7,  149,  24 }, {  9, -112, -24 },
        {  9,  -99,  -9 }, {  9, -116,  18 },
        {  9,  -81,  15 }, {  9,  -41,   9 },
        {  9,  -54,   1 }, {  9,  -12, -14 },
        {  9,   45,  18 }, {  9,   75,  11 },
        {  9,  130,  14 }, {  9,  166,  24 },
        {  0,  -68, -35 }, {  0,  -19,  -9 },
        {  0,   68,  13 }, {  8,  -76, -18 },
        {  8,  -33,  -1 }, {  8,   97,  31 },
        { 10,  -86,  10 }, { 10, -117,  47 },
        { 10,    9,   0 }, {  1, -111,  52 },
        {  1,  -50,  15 }, {  1,   75,   5 }
    },
    {
        {  3,  -88,  20 }, {  6,  -57,  29 },
        {  7, -139,  63 }, {  7,  -72,   5 },
        {  7,  -14,  28 }, {  9,  -91, -34 },
        {  9, -113, -14 }, {  9, -132,  33 },
        {  9,  -95,  34 }, {  9,  -60,  12 },
        {  9, -155,  52 }, {  9,  -66,  51 },
        {  9,  -56,  44 }, {  9,   44,  35 },
        {  9,   57,  25 }, {  9,   59,  -1 },
        {  9,  123,  41 }, {  9,  148,  30 },
        {  0, -135,  -2 }, {  0, -100,  59 },
        {  8,  -40,  11 }, { 10, -157, -19 },
        { 10,  -77,  35 }, {  6,  248,  17 },
        {  7,  300,  41 }, {  9,  172,  27 },
        {  9,  194,  12 }, {  9,  291,  17 },
        {  9,  290,  35 }, {  0,  233,  20 },
        {  8,  206,  17 }, { 10,  187,  42 }
    },
    {
        {  3,  -97,  -6 }, {  3,   89,   9 },
        {  6,  151,  -4 }, {  7,  -91, -39 },
        {  7,  -78, -16 }, {  7,  -56,  41 },
        {  7,  124,   1 }, {  7, -118,   0 },
        {  9, -107, -37 }, {  9, -127,  38 },
        {  9, -102,  21 }, {  9,  -67,  35 },
        {  9,  -54, -14 }, {  9,   12,  34 },
        {  9,   35,  39 }, {  9,   31,  21 },
        {  9,  130,  35 }, {  9,  185,  10 },
        {  0,  -42,  -3 }, {  0,   75,  35 },
        {  0,  177,  42 }, {  8, -126, -39 },
        { 10,  -16,  37 }, { 10,  148,   5 },
        {  1,   -3,  45 }, {  6,  160,  38 },
        {  7,  154,  16 }, {  9,  188,   3 },
        {  9,  150,  37 }, {  9,  214,  31 },
        {  9,  302,  16 }, {  8,  200,  15 }
    },
    {
        {  6,  103,  18 }, {  7, -114,   4 },
        {  7, -142,  39 }, {  7,   48,  23 },
        {  7,   88,  44 }, {  9,  -67, -30 },
        {  9, -134, -24 }, {  9,  -82,  -1 },
        {  9,  -45,  19 }, {  9, -153,  46 },
        {  9,   -8,  48 }, {  9,   11,  24 },
        {  9,   64,  27 }, {  9,  141,  38 },
        {  9,  154,  12 }, {  0, -105, -20 },
        {  0,   18,  14 }, {  0,  134,  20 },
        {  8, -153, -11 }, { 10,  -76,  64 },
        { 10,   33,  18 }, {  1,  -85, -28 },
        {  1, -154,  14 }, {  1,  148,  21 },
        {  3,  198,  23 }, {  6,  260,  14 },
        {  7,  238,  19 }, {  9,  165,  14 },
        {  9,  221,  22 }, {  0,  246,  16 },
        {  8,  221,  33 }, {  1,  282,  22 }
    }
};
const int anLandingDamageDetailCounts[4] = { /* 0x00465aa8 */
    0, 8, 16, 24
};
const char *apszLandingDamageComments[4] = { /* 0x00465ab8 */
    szLandingCleanComment,
    szLandingLightDamageComment,
    szLandingHeavyDamageComment,
    szLandingSurvivalComment
};
const signed char acLandingCanopyFramesHeavy[24] = { /* 0x00465ac8 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10,
    10, 10, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x40
};
const signed char acLandingCanopyFramesLight[24] = { /* 0x00465ae0 */
    0, 1, 2, 3, 11, 12, 13, 14, 14, 14, 14, 14,
    14, 14, 13, 12, 11, 3, 2, 1, 0, 0x40, 0, 0
};
const signed char acLandingCanopyFramesModerate[16] = { /* 0x00465af8 */
    0, 15, 16, 17, 18, 19, 19, 18,
    17, 16, 15, 0, 0x40, 0, 0, 0
};
const signed char *apLandingCanopyFrames[4] = { /* 0x00465b08 */
    acLandingCanopyFramesLight,
    acLandingCanopyFramesModerate,
    acLandingCanopyFramesHeavy,
    acLandingCanopyFramesHeavy
};
const ShortPoint aFuneralParticleOrigins[7] = { /* 0x00465b18 */
    { 234, 83 }, { 248, 85 }, { 260, 80 }, { 273, 78 },
    { 286, 75 }, { 299, 76 }, { 310, 74 }
};
const short asFuneralSceneBySeries[15] = { /* 0x00465b36 */
    0, 0, 1, 1, 1, 1, 1, 2, 3, 2, 3, 3, 2, 3, 0
};
int bFuneralShowTheEnd; /* 0x00465b54 */
const char szLandingCleanComment[32] = /* 0x00465b58 */
    "You got away pretty clean, sir!";
const char szLandingLightDamageComment[48] = /* 0x00465b78 */
    "Looks like it got a little hot out there, sir!";
const char szLandingHeavyDamageComment[36] = /* 0x00465ba8 */
    "You sure got yourself shot up, sir!";
const char szLandingSurvivalComment[44] = /* 0x00465bcc */
    "Glad to see you made it back alive, sir.";
const char szLandingCommentFormat[12] = "%X%Y%F%s%P"; /* 0x00465bf8 */
const char szFuneralTheEnd[8] = "THE END"; /* 0x00465c04 */
const char szFuneralTextFormat[12] = "%X%Y%F%s%P"; /* 0x00465c0c */
const char szFuneralCompanyCommand[20] = /* 0x00465c18 */
    "%X%Y%FCompany...%P";
const char szFuneralAttentionCommand[20] = /* 0x00465c2c */
    "%X%YAtten-SHUN!%P";
const char szFuneralPrepareArmsCommand[20] = /* 0x00465c40 */
    "%X%YPrepare arms!%P";
const char szFuneralFireCommand[16] = /* 0x00465c54 */
    "%X%Y%FFire!%P";
short nEnemySighting = 0x7fff; /* 0x00465c7c */
short nDebriefingPersonality; /* 0x00465c80 */
int bInitialFormationSetup; /* 0x00465c84 */
void *aapPacketReferences[4][0x25]; /* 0x00465c88 */
const ShortVector aaFormationPositions[5][8] = { /* 0x00465ed8 */
    {
        { 0, 0, 0 }, { -750, 0, 0 }, { 750, 0, 0 }, { 0, 0, -750 },
        { 0, 0, 750 }, { -750, 0, -750 }, { 750, 0, -750 },
        { 0, 0, -1500 }
    },
    {
        { 0, 0, 0 }, { 750, 0, 0 }, { -750, -100, -250 },
        { 1500, -100, -250 }, { -1500, -200, -500 },
        { -2250, -300, -750 }, { 2250, -200, -500 },
        { 3000, -300, -750 }
    },
    {
        { 0, 0, 0 }, { 750, 0, -500 }, { -750, 0, -500 },
        { 0, 0, -1000 }, { -1500, 0, -1000 }, { -750, 0, -1500 },
        { 1500, 0, -1000 }, { 750, 0, -1500 }
    },
    {
        { 0, 0, 0 }, { 750, 0, -250 }, { 0, 325, -500 },
        { 750, -325, -750 }, { 0, 325, -500 }, { 750, -325, -750 },
        { 0, 0, -1000 }, { 750, 0, -1250 }
    },
    {
        { 0, 0, 0 }, { 0, 0, -750 }, { -750, 0, -500 },
        { -750, 0, -1250 }, { 750, 0, -500 }, { 750, 0, -1250 },
        { 0, 500, -500 }, { 0, 500, -1250 }
    }
};
const int anPilotTurnInterval[18] = { /* 0x00465fc8 */
    5, 5, 4, 4, 3, 3, 3, 2, 2, 1, 3, 3, 3, 3, 2, 2, 2, 1
};
const char szErrorLoadingPilotSpeech[32] = /* 0x00466010 */
    "Error loading pilot speech";
const ShortPoint aBarracksBunkOrigins[8] = { /* 0x004693c8 */
    {109, 86}, {170, 86}, {98, 95}, {173, 95},
    {78, 110}, {176, 110}, {42, 136}, {183, 136}
};
const char *apszSaveCampaignMenuLabels[2] = { /* 0x004693e8 */
    szSaveCampaignMenuLabel,
    szSaveCampaignMenuLabelAlt
};
char *apszBarracksMenuLabels[20] = { /* 0x004693f0 */
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    (char *)szMissionHangarMenuLabel,
    (char *)szReturnToBarMenuLabel,
    (char *)szQuitGameMenuLabel,
    (char *)szViewMedalsMenuLabel
};
unsigned int dwModalBoundsTopLeft = 0x00280018; /* 0x00469440 */
unsigned int dwModalBoundsBottomRight = 0x003c0128; /* 0x00469444 */
ModalTextPanel *pModalTextPanel; /* 0x00469448 */
const short asCampaignPilotFiles[3] = {58, 61, 74}; /* 0x00469450 */
const short asCampaignBriefingFiles[3] = {10, 62, 73}; /* 0x00469458 */
const short asMissionDataFiles[3] = {15, 52, 72}; /* 0x00469460 */
const char szSaveCampaignMenuLabel[24] = /* 0x00469468 */
    "Save this campaign  ";
const char szSaveCampaignMenuLabelAlt[24] = /* 0x00469480 */
    "Save this campaign  ";
const char szMissionHangarMenuLabel[16] = "Mission Hangar"; /* 0x00469498 */
const char szReturnToBarMenuLabel[20] = "Return to the Bar"; /* 0x004694a8 */
const char szQuitGameMenuLabel[20] = "Quit Wing Commander"; /* 0x004694bc */
const char szViewMedalsMenuLabel[20] = "View your medals"; /* 0x004694d0 */
short nPaletteTransitionInitialise = 1; /* 0x00469640 */
int nDebugOverlayConsoleCount; /* 0x00469644 */
DWORD dwDebugOverlayKey; /* 0x00469648 */
DWORD dwDebugOverlayKeyLatch; /* 0x0046964c */
HHOOK hDebugKeyboardHook; /* 0x00469650 */
const char szDebugOverlayFontName[8] = "Courier"; /* 0x00469654 */
const char szDebugOverlaySpinner[5] = "-\\|/"; /* 0x0046965c */
const char szDebugOverlayNewline[4] = "\n"; /* 0x00469664 */
char szDiskMarkerFile[9] = "DISK.000"; /* 0x00469688 */
short nDiskPromptBorderColour = 0x50; /* 0x00469694 */
short nKeyboardPointerStep = 4; /* 0x004696a4 */
const short asPlayerDamageSystemTable[50] = { /* 0x00469878 */
    0, 8, 6, 5, 0, 3, 5, 5, 7, 6,
    0, 8, 6, 5, 4, 3, 4, 0, 4, 4,
    1, 2, 5, 2, 7, 3, 4, 7, 5, 1,
    1, 4, 1, 5, 2, 3, 4, 7, 2, 1,
    4, 4, 4, 4, 0, 8, 6, 5, 4, 0
};
const enum ObjectType aeShipHitDebrisTypes[3] = { /* 0x00469950 */
    OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK,
    OBJECT_TYPE_DEBRIS_SHIP_TUBING,
    OBJECT_TYPE_DEBRIS_O_RING
};
const signed char acGunRefireDelay[4] = {6, 10, 4, 0}; /* 0x0046995c */
const char szWeaponDestroyed[20] = "Weapon destroyed"; /* 0x00469960 */
const char szFuelTanksHit[16] = "Fuel tanks hit"; /* 0x00469974 */
const char szComponentFixedFormat[8] = "%s FIXD"; /* 0x00469984 */
const char szNeedLock[12] = "Need Lock"; /* 0x0046998c */
short bSlowSceneAnimation; /* 0x00469998 */
unsigned char cBlackColour; /* 0x0046999c */
unsigned char cViewportClearColour = 15; /* 0x004699a0 */
unsigned char cBlueColour = 0x25; /* 0x004699a4 */
unsigned char cYellowColour = 0x47; /* 0x004699a8 */
unsigned char cRedColour = 0x50; /* 0x004699ac */
unsigned char cDarkGreenColour = 0xaa; /* 0x004699b0 */
unsigned char cPrimaryTextColour = 0xa6; /* 0x004699b4 */
unsigned char cOrangeColour = 0x85; /* 0x004699b8 */
unsigned char cDarkBlueColour = 0x27; /* 0x004699bc */
unsigned char cDarkGreyColour = 7; /* 0x004699c0 */
unsigned char cLightGreyColour = 0x0b; /* 0x004699c4 */
unsigned char cMagentaColour = 0xb6; /* 0x004699c8 */
unsigned char cDefaultTextColour = 0xa8; /* 0x004699cc */
unsigned char cBrownColour = 0xfd; /* 0x004699d0 */
unsigned char cAsteroidColour = 0xf5; /* 0x004699d4 */
unsigned char cPrimaryViewBufferColour = 0xbf; /* 0x004699d8 */
const unsigned char abLegacyVideoModeColours[14] = { /* 0x004699e0 */
    9, 14, 4, 2, 10, 12, 15, 8, 7, 5, 2, 6, 7, 0
};
short asConversationTextColours[24] = { /* 0x004699f0 */
    0x25, 0xb6, 0x9a, 0x50, 0x94, 0x85, 0x27, 0xa6,
    0xfd, 0x47, 0xaa, 0x0b, 0x09, 0x0d, 0x03, 0x04,
    0x0b, 0x0c, 0x01, 0x0a, 0x06, 0x0e, 0x02, 0x07
};
int bGraphicsActive; /* 0x00469a20 */
const int anBaseMemoryReservationByVideoMode[4] = { /* 0x00469a90 */
    280000, 260000, 280000, 0
};
const int anFullMusicMemoryReservationByVideoMode[4] = { /* 0x00469aa0 */
    306000, 286000, 306000, 0
};
const int anExpandedMemoryReservationByVideoMode[3] = { /* 0x00469ab0 */
    258000, 238000, 258000
};
PacketResourceDescriptor aCommon3SpaceResources[12] = { /* 0x00469bc0 */
    { &aObjectTypeData[OBJECT_TYPE_THRUSTERS].shapeSet, 3, 0 },
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION0].shapeSet, 3, 1 },
    { &aObjectTypeData[OBJECT_TYPE_LASER_CANNON].shapeSet, 3, 6 },
    { &aObjectTypeData[OBJECT_TYPE_MASS_DRIVER_CANNON].shapeSet,
      3, 7 },
    { &aObjectTypeData[OBJECT_TYPE_NEUTRON_PARTICLE_GUN].shapeSet,
      3, 8 },
    { &aObjectTypeData[OBJECT_TYPE_LASER_SPARK].shapeSet, 3, 9 },
    { &aObjectTypeData[OBJECT_TYPE_DEBRIS_PIPE].shapeSet, 3, 4 },
    { &aObjectTypeData[OBJECT_TYPE_BLUE_SPARK].shapeSet, 3, 10 },
    { &aObjectTypeData[OBJECT_TYPE_RED_SPARK].shapeSet, 3, 11 },
    { &aObjectTypeData[OBJECT_TYPE_SPARK_TRAIL].shapeSet, 3, 12 },
    { &aObjectTypeData[OBJECT_TYPE_SPACE_MINE].shapeSet, 3, 15 },
    { 0, 0, 0 }
};
PacketResourceDescriptor aMissionResourceDescriptors[5] = { /* 0x00469c20 */
    { &aObjectTypeData[OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet,
      3, 14 },
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION1].shapeSet, 3, 2 },
    { &aObjectTypeData[OBJECT_TYPE_EXPLOSION2].shapeSet, 3, 3 },
    { &aObjectTypeData[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
};
PacketResourceDescriptor aCockpitResourceDescriptors[19] = { /* 0x00469c48 */
    { &pTargetLockShape, 8, 0 },
    { &pPilotHandShape, 2, 3 },
    { &pConfedCommBackground, 11, 0 },
    { &pCommStaticShape, 11, 11 },
    { &pKilrathiCommBackground, 11, 9 },
    { (unsigned char **)&pCockpitExplosionShape, 8, 5 },
    { &pCockpitIndicatorShape, 8, 4 },
    { &pCinematicViewBackdrop, 8, 6 },
    { &pRearViewBackdrop, 8, 7 },
    { &apCommPortraitShapes[0], 11, 1 },
    { &apCommPortraitShapes[1], 11, 2 },
    { &apCommPortraitShapes[2], 11, 3 },
    { &apCommPortraitShapes[3], 11, 4 },
    { &apCommPortraitShapes[4], 11, 5 },
    { &apCommPortraitShapes[5], 11, 6 },
    { &apCommPortraitShapes[6], 11, 7 },
    { &apCommPortraitShapes[7], 11, 8 },
    { &apCommPortraitShapes[8], 11, 10 },
    { 0, 0, 0 }
};
PacketResourceDescriptor aCockpitSecondaryResources[5] = { /* 0x00469ce0 */
    { &pCockpitDamageShape, -1, 7 },
    { &pCockpitWeaponShape, -1, 9 },
    { &pCockpitIndicatorShape, 8, 4 },
    { &pTargetLockShape, 8, 0 },
    { 0, 0, 0 }
};
PacketResourceDescriptor aCockpitPrimaryResources[8] = { /* 0x00469d08 */
    { &pCockpitPilotShape, -1, 4 },
    { &pCockpitDamageShape, -1, 7 },
    { &pCockpitWeaponShape, -1, 9 },
    { &apCockpitShapes[0], -1, 0 },
    { &apCockpitShapes[1], -1, 1 },
    { &apCockpitShapes[2], -1, 2 },
    { &apCockpitShapes[3], -1, 3 },
    { 0, 0, 0 }
};
unsigned int bIntroSceneResourcesActive = 1; /* 0x00469d48 */
short asConstellationObjectIndices[4] = { /* 0x00469d50 */
    -1, -1, -1, -1
};
int bCockpitResourcesActive; /* 0x00469d58 */
int b3SpaceObjectsActive; /* 0x00469d5c */
const short asSceneAnimationLogicalFiles[8] = { /* 0x00469d60 */
    63, 64, 65, 66, 67, 68, 69, 70
};
short bSceneAnimationWaitCommand; /* 0x00469d70 */
const char szSceneAnimationTextFormat[12] = "%X%Y%F%s%P"; /* 0x00469d74 */
const char szMeanwhile[16] = "Meanwhile..."; /* 0x00469d80 */
const char szEmptyAnswerInput[4] = ""; /* 0x00469d90 */
const char szAnswerLabel[9] = "Answer: "; /* 0x00469d94 */
Viewport stTrainSimPanelViewport; /* 0x00469da8 */
const ShortRect stTrainSimPanelBounds = { /* 0x00469dc0 */
    0x30, 0x1d, 0x110, 0x6d
};
const char szTrainSimTitle[24] = "SQUADRON: TRAINSIM"; /* 0x00469dc8 */
unsigned char *apszBuiltInHighScoreNames[6] = { /* 0x00469de0 */
    &aszBuiltInHighScores[0],
    &aszBuiltInHighScores[8],
    &aszBuiltInHighScores[16],
    &aszBuiltInHighScores[24],
    &aszBuiltInHighScores[32],
    &aszBuiltInHighScores[40]
};
TitleMenuRegion aTrainSimMissionRegions[5] = { /* 0x00469df8 */
    { 1, 47, 29, 67, 49 },
    { 1, 47, 89, 67, 109 },
    { 1, 251, 29, 271, 49 },
    { 1, 251, 89, 271, 109 },
    { -1, 0, 0, 0, 0 }
};
int nTrainSimActive; /* 0x00469e2c */
short nTrainSimMission; /* 0x00469e30 */
short nArcadeWave; /* 0x00469e34 */
unsigned char aszBuiltInHighScores[48] = /* 0x00469e38 */
    "BISHOP\0\0"
    "GOBLIN\0\0"
    "JEFFTEP\0"
    "MANGLER\0"
    "THE MAN\0"
    "MONGO\0\0\0";
unsigned int dwTrainSimStringPadding; /* 0x00469e68 */
const char szTextFlushToken[4] = "%P"; /* 0x00469e6c */
const char szNewPilotPrompt[80] = /* 0x00469e70 */
    "CONGRATULATIONS!\n"
    "YOU HAVE A TOP SCORE!\n"
    "PLEASE ENTER YOUR\n"
    "NAME AND CALLSIGN:\n";
const char szDefaultPilotName[8] = "Blair"; /* 0x00469ec0 */
const char szPilotNameLabel[12] = "LAST NAME: "; /* 0x00469ec8 */
const char szDefaultCallsign[12] = "Maverick"; /* 0x00469ed4 */
const char szCallsignLabel[12] = "CALLSIGN : "; /* 0x00469ee0 */
const char szCheaterCallsign[8] = "CHEATER"; /* 0x00469eec */
const char szHighScoreCongratulations[68] = /* 0x00469ef4 */
    "*******\n"
    "CONGRATULATIONS!\n"
    "YOU HAVE SCORE NUMBER\n"
    ">>>> %d <<<<\n"
    "*******";
const char szLowScoreMessage[56] = /* 0x00469f38 */
    "> SORRY <\n\n"
    "YOUR SCORE IS ONLY\n"
    "%ld0\n\n"
    "PLEASE PLAY AGAIN!";
const char szHighScoresHeading[16] = "%JHIGH SCORES%P"; /* 0x00469f70 */
const char szHighScoreNumberFormat[8] = "%0ld"; /* 0x00469f80 */
const char szHighScoreRowFormat[16] = /* 0x00469f88 */
    "%X%Y%d. %s%X%s0";
const char szSelectEnemy[16] = "SELECT\nENEMY%P"; /* 0x00469f98 */
int nCannedSceneMode; /* 0x00469fac */
int nArcadeState; /* 0x00469fb0 */
short nFrameSkipCounter = 1; /* 0x00469fb4 */
short nFrameSkip = 1; /* 0x00469fb8 */
int nSfxVolumeSetting = 0x14; /* 0x00469fbc */
int nMusicVolumeSetting = 0x14; /* 0x00469fc0 */
int bViewportDirty; /* 0x00469fc4 */
int anVolumeLevels[11] = { /* 0x00469fc8 */
    0, 40000, 50000, 55000, 60000, 61000,
    61500, 62000, 63000, 63500, 64000
};
int nOriginDevUnlock; /* 0x00469ff4 */
int bPlayerCollisionsEnabled = 1; /* 0x00469ff8 */
int bPlayerVulnerable = 1; /* 0x00469ffc */
int bPlayerCollisionResponse = 1; /* 0x0046a000 */
int bViewBufferEnabled; /* 0x0046a004 */
int bCockpitlessView; /* 0x0046a008 */
int nShowMemoryStatus; /* 0x0046a00c */
short nStartNavPointOverride = -1; /* 0x0046a010 */
short nArcadeBonusCountdown; /* 0x0046a014 */
int bMouseCursorVisible; /* 0x0046a018 */
unsigned char bInputPollingGuard; /* 0x0046a01c */
InputDeviceSample stLastPolledFlightInput; /* 0x0046a020 */
int bMouseAfterburnerControl; /* 0x0046a02c */
const short asMouseYawThresholds[6] = { /* 0x0046a030 */
    10, 37, 52, 57, 62, 1070
};
const short asMousePitchThresholds[6] = { /* 0x0046a040 */
    5, 18, 27, 35, 38, 1040
};
unsigned int dwLastSecondaryButtonPress; /* 0x0046a04c */
unsigned char bFlightRollLatch; /* 0x0046a050 */
unsigned char bAfterburnerButtonLatched; /* 0x0046a054 */
short nMouseYawInput; /* 0x0046a058 */
short nMousePitchInput; /* 0x0046a05c */
const char szSpaceBuffer[16] = "SPACE BUFFER"; /* 0x0046a1d0 */
const char szAllocateBufferTag[4] = "AB"; /* 0x0046a1e0 */
ActiveSoundEntry *pActiveSoundHead; /* 0x0046a438 */
ActiveSoundEntry *pActiveSoundTail; /* 0x0046a43c */
int bAudioSystemInitialized; /* 0x0046a440 */
WaveTableEntry *pWaveTableHead; /* 0x0046a444 */
WaveTableEntry *pWaveTableTail; /* 0x0046a448 */
const char szPlayWaveOpenError[36] = /* 0x0046a46c */
    "playWAVE Unable to open file '%s'";
const short asCarrierLaunchApproachDeltaX[24] = { /* 0x0046a550 */
    -1, -1, -1, -2, -2, -2, -2, -3,
    -3, -3, -3, -3, -2, -2, -2, -2,
    -1, -1, -1, -1, -1, 0, 0, 0
};
const signed char acCarrierLaunchApproachFrames[24] = { /* 0x0046a580 */
    36, 32, 32, 32, 32, 32, 32, 32,
    25, 25, 25, 25, 25, 25, 25, 25,
    18, 18, 18, 18, 18, 18, 18, 18
};
const ShortPoint aCarrierLaunchFighterPath[9] = { /* 0x0046a598 */
    { -2, 1 }, { -1, 1 }, { -1, 1 },
    { -1, 0 }, { -1, 1 }, { -1, 0 },
    { -1, 1 }, { -1, 0 }, { 0, 0 }
};
const short asCarrierLaunchFighterDeltaY[16] = { /* 0x0046a5bc */
    0, 0, 3, 3, 3, 2, 2, 2,
    2, 2, 1, 1, 1, 1, 1, 1
};
const short asCarrierLaunchViewData[16] = { /* 0x0046a5dc */
    0, 0, 12, 11,
    1, 90, 90, 0, 9, -90, 0, 0, 20, 3, 15, -1
};
const char szPressAnyKeyWhenReady[28] = /* 0x0046a5fc */
    "Press any key when ready";
unsigned char *pDamageDisplayBackground; /* 0x0046a748 */
const ShortPoint aDamageDisplayPositions[9] = { /* 0x0046a750 */
    {36, 37}, {36, 28}, {36, 30}, {36, 23}, {36, 19},
    {36, 15}, {36, 24}, {36, 16}, {36, 22}
};
const char *apszComponentNames[9] = { /* 0x0046a778 */
    szIonDrive,
    szPowerPlant,
    szShieldGenerator,
    szComputerSystem,
    szIntercomUnit,
    szTargetTracking,
    szAccelerationAbsorbers,
    szEjectorSystem,
    szRepairSystems
};
const unsigned char abDamageDisplayFrames[9] = { /* 0x0046a7a0 */
    21, 22, 23, 24, 25, 26, 27, 28, 29
};
const char *apszDamageSeverityNames[5] = { /* 0x0046a7b0 */
    szDamageOk,
    szDamageLight,
    szDamageModerate,
    szDamageHeavy,
    szDamageDestroyed
};
short nScriptedViewObject = -1; /* 0x0046a8d0 */
int bScriptedView; /* 0x0046a8d4 */
const short asConstellationVelocity[16] = { /* 0x0046a8d8 */
    8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1
};
const short asConstellationFrame[16] = { /* 0x0046a8f8 */
    0, 16, 16, 0, 4, 4, 20, 20, 24, 8, 8, 24, 28, 12, 12, 28
};
short nConstellationDirection = -1; /* 0x0046a918 */
unsigned char *pPacketDecompressionWorkspace; /* 0x0046a91c */
unsigned short wPacketDecompressionInputSizeOverride; /* 0x0046a920 */
unsigned short wPacketCompressionFormatFlags; /* 0x0046a924 */
const ShortRect aTargetArmorClipRects[4] = { /* 0x0046a928 */
    { 12, -20, 29, 20 },
    { -11, 1, 11, 20 },
    { -11, -20, 11, 0 },
    { -29, -20, -12, 20 }
};
char szEmptyTargetDisplayText[4] = ""; /* 0x0046a948 */
const char szLockedTarget[20] = "   LOCKED TARGET"; /* 0x0046a94c */
const char szTextColourStringColourFormat[8] = "%F%s%F"; /* 0x0046a960 */
const char szAutoTargetting[20] = "  AUTO TARGETTING"; /* 0x0046a968 */
const char szTextColourStringFormat[8] = "%F%s"; /* 0x0046a97c */
const char szTargetLabel[12] = "\nTarget:"; /* 0x0046a984 */
const char szNoTarget[8] = " None"; /* 0x0046a990 */
const char szWingmanTargetNameFormat[4] = " %s"; /* 0x0046a998 */
const char szAceTargetNameFormat[4] = " %s"; /* 0x0046a99c */
const char szShipTargetNameFormat[4] = " %s"; /* 0x0046a9a0 */
const char szRangeLabel[12] = "\nRange : "; /* 0x0046a9a4 */
const char szTargetTooFar[8] = "TOO FAR"; /* 0x0046a9b0 */
const char szRangeMetresSuffix[4] = " m"; /* 0x0046a9b8 */
const char szTargetOffscreenRange[8] = "----- m"; /* 0x0046a9bc */
short nMusicPlaybackMode; /* 0x0046a9f8 */
int bMusicCommandSuppressed; /* 0x0046a9fc */
short nMusicTrackComplete = 1; /* 0x0046aa04 */
short nFuneralSequenceActive; /* 0x0046aa10 */
int nCurrentMusicTrack = -1; /* 0x0046aa14 */
int nMusicStreamSet = -1; /* 0x0046aa18 */
volatile int nInFlightMusicSlotA = -1; /* 0x0046aa24 */
volatile int nInFlightMusicSlotB = -1; /* 0x0046aa2c */
int nWaitForMusicEnabled = 1; /* 0x0046aa30 */
int nFlightSoundEffectsEnabled = 1; /* 0x0046aa34 */
int nInitialFlightMusicPending = 1; /* 0x0046aa38 */
int nCombatMusicActive; /* 0x0046aa3c */
int nInFlightMusicActive; /* 0x0046aa40 */
short nPassingShipSoundObject = -1; /* 0x0046aa48 */
short nPassingShipSoundCountdown; /* 0x0046aa4c */
const unsigned char abFireworkSoundDescriptor[7] = { /* 0x0046ab70 */
    0x00, 0x80, 0x40, 0x40, 0x3c, 0x00, 0x00
};
const char szSfxWaveFormat[12] = "sfx%02i.wav"; /* 0x0046ad2c */
unsigned char abTaggedAllocationPrefix[8] = { /* 0x0046ad88 */
    'j', 'e', 'f', 'f', 0, 0, 0, 0
};
const char *apszCampaignVictoryText[4] = { /* 0x0046ad90 */
    szCampaignVictoryOpening,
    szCampaignVictoryAttack,
    szCampaignVictoryConclusion,
    0
};
const char *pszTigerClawEscapeOpening = /* 0x0046ada0 */
    szTigerClawEscapeOpening;
const char *pszTigerClawEscapeJump = /* 0x0046ada4 */
    szTigerClawEscapeJump;
const char *pszTigerClawEscapeClosing = /* 0x0046ada8 */
    szTigerClawEscapeClosing;
const ShortVector aCampaignVictoryProjectileOrigins[4] = { /* 0x0046adb0 */
    { -55, 42, 0 }, { -68, 46, 0 },
    { 60, 42, 16 }, { 73, 46, 16 }
};
const char *pszTheEnd = szTheEnd; /* 0x0046adc8 */
const char *pszForNow = szForNow; /* 0x0046adcc */
const char szCampaignVictoryOpening[68] = /* 0x0046add0 */
    "Destroying the remains of the Kilrathi naval power in the sector...";
const char szCampaignVictoryAttack[44] = /* 0x0046ae14 */
    "The Tiger's Claw closes in for the kill...";
const char szCampaignVictoryConclusion[52] = /* 0x0046ae40 */
    "And the last Kilrathi planet in the sector falls!";
const char szTigerClawEscapeOpening[64] = /* 0x0046ae74 */
    "Fleeing from the overwelming Kilrathi forces in the sector...";
const char szTigerClawEscapeJump[48] = /* 0x0046aeb4 */
    "The Tiger's Claw manages to jump out. Barely.";
const char szTigerClawEscapeClosing[44] = /* 0x0046aee4 */
    "There'll be other sectors, other battles...";
const char szTheEnd[8] = "The End"; /* 0x0046af10 */
const char szForNow[12] = "For Now..."; /* 0x0046af18 */
const char szCampaignVictoryTextFormat[12] = "%X%Y%s%P"; /* 0x0046af24 */
const char szTigerClawEscapeOpeningFormat[12] = "%X%Y%s%P"; /* 0x0046af30 */
const char szTigerClawEscapeJumpFormat[12] = "%X%Y%s%P"; /* 0x0046af3c */
const char szTigerClawEscapeClosingFormat[12] = "%X%Y%s%P"; /* 0x0046af48 */
short nInputDoubleClickInterval = 1; /* 0x0046af54 */
short nMenuPointerSpeed = 2; /* 0x0046af58 */
short nCommMenuChoiceCount = -1; /* 0x0046af60 */
short nCommMenuReuseMode; /* 0x0046af64 */
signed char cPendingCommMenuAction = 1; /* 0x0046af6c */
unsigned char bRadioSilence; /* 0x0046af70 */
unsigned char bVideoImagesSuppressed; /* 0x0046af74 */
unsigned char bCommVideoEnabled = 1; /* 0x0046af78 */
signed char cCommMenuRecipient = -1; /* 0x0046afc4 */
short nCommSpeakerObject; /* 0x0046afc8 */
short nCommSpeakerRating; /* 0x0046afcc */
short nCommPortraitIndex = -1; /* 0x0046afd0 */
const char szConfedCommFormat[8] = "%s: %Fs"; /* 0x0046b150 */
const char szKilrathiAceCommFormat[8] = "%s: %Fs"; /* 0x0046b158 */
const char szShipCommFormat[8] = "%s: %Fs"; /* 0x0046b160 */
unsigned short nVideoMode = 0xffff; /* 0x0046b168 */
const signed char acGraphicsModeByAnimationSpeed[8] = { /* 0x0046b178 */
    0x13, 0x0d, 4, 9, 7, 0, 0, 0
};
const GUID guidDirectDraw2 = { /* 0x00463118 */
    0xb3a6f3e0, 0x2b43, 0x11cf,
    { 0xa2, 0xde, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56 }
};
LPDIRECTDRAW2 pDirectDraw2; /* 0x0046b1a4 */
LPDIRECTDRAWSURFACE pPrimarySurface; /* 0x0046b1a8 */
LPDIRECTDRAWSURFACE pSecondarySurface; /* 0x0046b1ac */
LPDIRECTDRAWPALETTE pDirectDrawPalette; /* 0x0046b1b0 */
int nDIBCascadeLevel = -1; /* 0x0046b1b4 */
int nFrameIntervalMs = 62; /* 0x0046b1b8 */
int nFrameDeadlineMs; /* 0x0046b1bc */
int nPreviousFrameTimeMs; /* 0x0046b1c4 */
int bSpaceFlightFrameTiming; /* 0x0046b1c8 */
float fSpaceFlightFrameRate = 20.0f; /* 0x0046b1cc */
float fCinematicFrameRate = 16.0f; /* 0x0046b1d0 */
const short aasCockpitLightX[5][7] = { /* 0x0046dca8 */
    {101, 101, 189, 137, 265, -99, 203},
    {198, 198, 133, 145, 234, 234, -99},
    {76, 76, 139, 257, 20, 20, -99},
    {194, 194, 96, 137, 96, 96, -99},
    {-99, -99, 179, 133, -99, -99, -99}
};
const short aasCockpitLightY[5][7] = { /* 0x0046dcf0 */
    {122, 145, 121, 7, 121, -99, 147},
    {120, 139, 114, 114, 179, 188, -99},
    {164, 183, 22, 181, 162, 178, -99},
    {119, 142, 136, 15, 126, 116, -99},
    {-99, -99, 115, 114, -99, -99, -99}
};
const signed char aacCockpitLightOffFrame[5][7] = { /* 0x0046dd38 */
    {14, 14, 14, 11, 13, 14, 14},
    {12, 12, 12, 13, 12, 12, 12},
    {9, 9, 9, 14, 9, 9, 9},
    {15, 15, 13, 11, 13, 13, 13},
    {5, 5, 5, 5, 5, 5, 5}
};
const signed char aacCockpitLightOnFrame[5][7] = { /* 0x0046dd60 */
    {3, 3, 3, 0, 2, 3, 3},
    {2, 2, 2, 3, 2, 2, 2},
    {1, 1, 1, 6, 1, 1, 1},
    {4, 4, 2, 0, 2, 2, 2},
    {1, 1, 1, 1, 1, 1, 1}
};
const CockpitBarDefinition aaCockpitBars[5][8] = { /* 0x0046dd88 */
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
const CockpitLayout stCockpitLayout = { /* 0x0046e008 */
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
unsigned char bCurrentKey = 0x80; /* 0x0046c014 */
signed char cPreviousKey = (signed char)0x80; /* 0x0046c018 */
short nCurrentWave = -1; /* 0x0046c01c */
int bIntroSecondaryScene; /* 0x0046c024 */
signed char abHazardObjects[0x14] = { /* 0x0046c028 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
short nEyePitchRate = 1; /* 0x0046c004 */
short nEyeYawRate = 1; /* 0x0046c008 */
short nEyeRollRate = 1; /* 0x0046c00c */
short DAT_0046c010 = -1;
signed char cCurrentObjective = -1; /* 0x0046c020 */
int nCameraViewMode = -1; /* 0x0046c03c */
signed char cViewObject = -1; /* 0x0046c000 */
short nExternalViewShip = -1; /* 0x0046c040 */
short nEjectedPilotObject; /* 0x0046c044 */
short nClosestVisibleObject = -1; /* 0x0046c048 */
short nYourWingman = -1; /* 0x0046c04c */
short nPlayerCollisionObject = -1; /* 0x0046c050 */
enum ObjectType eSelectedGunType = (enum ObjectType)-1; /* 0x0046c054 */
int nSelectedReleaseWeaponIndex = -1; /* 0x0046c058 */
short DAT_0046c05c;
short bTargetLockReadoutDirty; /* 0x0046c060 */
short nTargetLockCountdown; /* 0x0046c064 */
short nTargetLockMarkerAngle; /* 0x0046c068 */
signed char cTargetDisplayObject = -1; /* 0x0046c06c */
int bTargetLockAcquired; /* 0x0046c074 */
short nTargetLockMode; /* 0x0046c078 */
int bMissileCameraEnabled; /* 0x0046c07c */
int bEngageAllowed; /* 0x0046c080 */
short nAutoEngageTimer = -1; /* 0x0046c084 */
signed char cViableTargetCount; /* 0x0046c088 */
const short asIntroCameraSequence[20] = { /* 0x0046c090 */
    0, -1000, 0, -4263, 2, 15, 1, 0, 30, 0,
    3, 15, 4, 30, 1, 13, 14, 400, -1, 0
};
const short asCannedSequence32[] = { /* 0x0046c0b8 */
    0, 20, 1, 0, 0, 180, 40, 1, 0, 0, 180, 40,
    2, 0, 400, -1
};
const short asCannedSequence33[] = { /* 0x0046c0d8 */
    0, 20, 0, 20, 1, 0, -15, -60, 50, 0, 400, -1
};
const short asCannedSequence34[] = { /* 0x0046c0f0 */
    0, 20, 0, 20, 1, 0, 15, 60, 50, 0, 400, -1
};
const short asCannedSequence35[42] = { /* 0x0046c108 */
    0, 17, 0, 6, 3, 0, 6, 3, 0, 6, 3, 0, 6, 3,
    0, 6, 3, 0, 6, 3, 0, 6, 3,
    1, 0, 0, 180, 40, 1, 0, 0, 180, 40,
    1, 0, -30, 60, 40, 0, 400, -1
};
const short asCampaignVictoryViewScript[24] = { /* 0x0046c160 */
    16, 38, 0, 1200, 0, 1600, 1, 180,
    0, 0, 15, 3, 4, 14, 100, 2,
    15, 3, 15, 14, 120, -1, 0, 0
};
const short asCannedSequence37[] = { /* 0x0046c190 */
    0, 100, 1, 0, 30, 0, 50, 0, 20, 2, 0, 400, -1
};
const short asCannedSequence38[] = { /* 0x0046c1b0 */
    0, 110, 1, 0, 30, 0, 50, 0, 10, 2, 0, 400, -1
};
const short asCannedSequence39[] = { /* 0x0046c1d0 */
    0, 10, 2, 0, 400, -1
};
const short asCannedSequence40[] = { /* 0x0046c1e0 */
    0, 40,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60, -1
};
const short asCannedSequence41[3] = { 0, 400, -1 }; /* 0x0046c230 */
const short asTigerClawEscapeViewScript[12] = { /* 0x0046c238 */
    0, 0, 0, 0, 2, 15, 3, 15, 14, 400, -1, 0
};
const short asCannedSequence42[] = { /* 0x0046c250 */
    0, 120, 1, 0, 0, 180, 50, 3, 1, 0, 0, 180, 50, 3, -1
};
const short asCannedSequence43[] = { /* 0x0046c270 */
    0, 120, 1, 0, 0, 180, 50, 3, 1, 0, 0, 180, 50,
    3, 1, 0, 0, 180, 50, 3, -1
};
const short asCannedSequence44[] = { /* 0x0046c2a0 */
    0, 5, 3, 0, 5, 3, 0, 5, 3, 0, 5, 3,
    1, 0, 0, 180, 50, 1, 0, 0, 180, 50,
    1, 0, 0, 180, 50, 1, 0, 0, 180, 50, -1
};
const short asCannedSequence45[] = { 0, 20, 2, -1 }; /* 0x0046c2e8 */

const signed char acDirectionShapeFrame[ /* 0x0046db28 */
    WC1_DIRECTION_VIEW_COUNT * WC1_DIRECTION_SHAPE_TABLE_COUNT] = {
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
const signed char acDirectionShapeFlip[ /* 0x0046dbe8 */
    WC1_DIRECTION_VIEW_COUNT * WC1_DIRECTION_SHAPE_TABLE_COUNT] = {
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

MissionNavPoint aMissionNavPoints[WC1_MISSION_NAV_POINT_COUNT] = { /* 0x0046c2f0 */
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { OBJECT_TYPE_DRALTHI, OBJECT_TYPE_HORNET },
        { 32, 33, 34, 35, -1, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { OBJECT_TYPE_ASTEROID_FIELD, (enum ObjectType)-1 },
        { 36, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { OBJECT_TYPE_GRATHA, OBJECT_TYPE_RAPIER },
        { 37, 38, 39, 40, 41, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
        { OBJECT_TYPE_KRANT, OBJECT_TYPE_SCIMITAR },
        { 42, 43, 44, 45, -1, -1, -1, -1, -1, -1 }
    }
};

MissionShipRecord
    aMissionShips[WC1_MISSION_SHIP_STORAGE_COUNT] = { /* 0x0046c948 */
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { 0, 0, 0 }, -155, 0, 0, 0, 40, 3,
      { asCannedSequence32 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { -154521, -25600, -232012 }, -155, 0, 0, 0, 40, 3,
      { asCannedSequence33 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { -61849, 25600, -232012 }, -155, 0, 0, 0, 40, 3,
      { asCannedSequence34 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_HORNET, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { 324582, 0, 696038 }, -155, 0, 0, 0, 40, 3,
      { asCannedSequence35 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_ASTEROID_FIELD, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE, 17, { 0, 0, 0 }, 0, 0, 0, 0,
      30000, 3, { 0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 102400, -153600, -332800 }, 0, 0, -30, 0, 80, 3,
      { asCannedSequence37 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -332800 }, 0, 0, 90, 0, 62, 3,
      { asCannedSequence38 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -153600 }, 0, 0, -30, 0, 80, 3,
      { asCannedSequence39 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_RAPIER, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -819200 }, 0, 0, 30, 0, 60, 3,
      { asCannedSequence40 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_RAPIER, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 102400, -153600, -819200 }, 0, 0, 0, 0, 80, 3,
      { asCannedSequence41 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      19, { -51200, 76800, -1382400 }, 0, 0, 0, 0, 50, 3,
      { asCannedSequence42 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      19, { 153600, 0, -1280000 }, 0, 0, 60, 0, 50, 3,
      { asCannedSequence43 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      19, { -249856, 0, 1139200 }, 155, 0, 0, 0, 50, 3,
      { asCannedSequence44 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_SCIMITAR, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE, 19, { -76800, 0, 768000 },
      155, 0, 0, 0, 50, 3, { asCannedSequence45 },
      0, 0, 0, -1, 0, 0 }
};
int aiIntelligenceEvent[10]; /* 0x0046d368 */
signed char aDefenseManeuversNovice[8] = { /* 0x0046d390 */
    24, 34, 13, 14, -1, 0, 0, 0
};
signed char aDefenseManeuversVeteran[8] = { /* 0x0046d398 */
    8, 13, 15, 14, 19, 24, -1, 0
};
signed char aDefenseManeuversElite[16] = { /* 0x0046d3a0 */
    8, 15, 17, 23, 19, 9, 20, 34, 14, -1, 0, 0, 0, 0, 0, 0
};
signed char aDefenseManeuversAce[16] = { /* 0x0046d3b0 */
    17, 23, 15, 19, 9, 14, 20, 12, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char aDefenseManeuversBoss[16] = { /* 0x0046d3c0 */
    15, 19, 12, 11, 17, 23, 7, 35, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char *apDefenseManeuvers[5] = { /* 0x0046d3d0 */
    aDefenseManeuversNovice,
    aDefenseManeuversVeteran,
    aDefenseManeuversElite,
    aDefenseManeuversAce,
    aDefenseManeuversBoss
};
ManeuverChoice aRatedManeuverChoices[13][9][3] = { /* 0x0046d3e8 */
    {
        { { 70, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 80, 29, 37 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 7, 8 }, { 75, 14, 17 }, { 100, 21, -1 } },
        { { 80, 41, 29 }, { 80, 41, 24 }, { 100, 21, -1 } },
        { { 60, 26, 29 }, { 70, 26, 29 }, { 90, 26, 29 } },
        { { 50, 16, 12 }, { 50, 13, 20 }, { 100, 21, -1 } },
        { { 100, 19, -1 }, { 50, 9, 20 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 31, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 55, 12, 19 }, { 70, 11, 12 }, { 100, 21, -1 } },
        { { 60, 29, 41 }, { 80, 29, 41 }, { 100, 33, -1 } },
        { { 80, 26, 29 }, { 50, 26, 29 }, { 100, 29, -1 } },
        { { 50, 15, 16 }, { 50, 16, 32 }, { 100, 21, -1 } },
        { { 50, 7, 10 }, { 50, 14, 9 }, { 100, 21, -1 } },
        { { 50, 25, 2 }, { 50, 25, 2 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 30 }, { 60, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 60, 15, 17 }, { 50, 12, 17 }, { 100, 21, -1 } },
        { { 20, 41, 30 }, { 30, 41, 17 }, { 40, 41, 21 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 40, 32, 17 }, { 20, 32, 16 }, { 100, 21, -1 } },
        { { 50, 17, 20 }, { 50, 23, 36 }, { 100, 20, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 21, -1 } }
    },
    {
        { { 50, 40, 30 }, { 90, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 80, 30, 37 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 70, 7, 16 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 20, 31, 41 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 80, 26, 31 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 75, 16, 15 }, { 75, 16, 15 }, { 100, 44, -1 } },
        { { 50, 12, 16 }, { 100, 44, -1 }, { 100, 44, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 60, 40, 30 }, { 100, 40, -1 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 50, 15, 16 }, { 60, 15, 7 }, { 70, 20, 16 } },
        { { 70, 41, 17 }, { 80, 41, 20 }, { 100, 20, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 16, 13 }, { 50, 16, 20 }, { 100, 21, -1 } },
        { { 50, 20, 36 }, { 50, 11, 10 }, { 100, 20, -1 } },
        { { 25, 25, 2 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 20, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 50, 7, 8 }, { 50, 9, 24 }, { 100, 12, -1 } },
        { { 80, 41, 29 }, { 80, 41, 20 }, { 100, 41, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 20, 32, 16 }, { 50, 16, 20 }, { 100, 21, -1 } },
        { { 50, 19, 14 }, { 50, 14, 12 }, { 100, 8, -1 } },
        { { 50, 17, 23 }, { 50, 17, 23 }, { 30, 25, 2 } }
    },
    {
        { { 50, 30, 31 }, { 60, 40, 31 }, { 100, 39, -1 } },
        { { 80, 29, 37 }, { 80, 29, 37 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 50, 9, 10 }, { 60, 7, 11 }, { 100, 20, -1 } },
        { { 50, 31, 41 }, { 80, 31, 23 }, { 100, 33, -1 } },
        { { 80, 26, 29 }, { 70, 26, 29 }, { 100, 26, -1 } },
        { { 50, 32, 16 }, { 50, 20, 16 }, { 100, 21, -1 } },
        { { 50, 36, 12 }, { 50, 20, 8 }, { 50, 19, 20 } },
        { { 80, 25, 2 }, { 50, 25, 17 }, { 100, 2, -1 } }
    },
    {
        { { 50, 40, 30 }, { 80, 40, 30 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 60, 15, 8 }, { 70, 19, 14 }, { 100, 21, -1 } },
        { { 70, 29, 20 }, { 50, 29, 20 }, { 100, 41, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 16, 19 }, { 50, 16, 8 }, { 100, 21, -1 } },
        { { 50, 36, 12 }, { 50, 19, 20 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 100, -1, -1 }, { 100, -1, -1 }, { 100, -1, -1 } },
        { { 0, -1, -1 }, { 0, -1, -1 }, { 0, -1, -1 } },
        { { 0, -1, -1 }, { 0, -1, -1 }, { 0, -1, -1 } }
    },
    {
        { { 60, 40, 29 }, { 60, 40, 29 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 16, 38 }, { 75, 7, 20 }, { 100, 21, -1 } },
        { { 60, 41, 31 }, { 60, 41, 31 }, { 100, 21, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 32, 20 }, { 50, 32, 20 }, { 100, 21, -1 } },
        { { 50, 19, 20 }, { 50, 9, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 50, 40, 31 }, { 70, 40, 31 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 97, 12, 22 }, { 95, 20, 22 }, { 100, 21, -1 } },
        { { 40, 41, 31 }, { 20, 41, 31 }, { 100, 21, -1 } },
        { { 50, 26, 31 }, { 100, 31, -1 }, { 100, 26, -1 } },
        { { 50, 16, 20 }, { 50, 19, 20 }, { 100, 21, -1 } },
        { { 100, 20, 36 }, { 50, 12, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 25, -1 }, { 100, 2, -1 } }
    },
    {
        { { 70, 40, 30 }, { 70, 16, 30 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 23, 20 }, { 75, 12, 17 }, { 100, 21, -1 } },
        { { 30, 41, 29 }, { 50, 41, 20 }, { 100, 21, -1 } },
        { { 100, 26, -1 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 20, 32, 20 }, { 50, 16, 20 }, { 100, 21, -1 } },
        { { 50, 16, 19 }, { 50, 12, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 80, 40, 31 }, { 90, 40, 31 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 15, 20 }, { 75, 16, 19 }, { 100, 21, -1 } },
        { { 50, 41, 31 }, { 50, 41, 31 }, { 100, 21, -1 } },
        { { 80, 26, 31 }, { 100, 26, -1 }, { 100, 26, -1 } },
        { { 50, 16, 20 }, { 50, 32, 20 }, { 100, 21, -1 } },
        { { 70, 19, 17 }, { 50, 11, 20 }, { 100, 21, -1 } },
        { { 100, 25, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    }
};
ManeuverChoice aKilrathiManeuverChoices[5][9][3] = { /* 0x0046d808 */
    {
        { { 70, 40, 45 }, { 80, 40, 45 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 90, 17, 46 }, { 90, 17, 46 }, { 100, 21, -1 } },
        { { 50, 46, 45 }, { 80, 46, 45 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 20, 17, 46 }, { 100, 21, -1 } },
        { { 100, 46, -1 }, { 25, 17, 46 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 45 }, { 70, 40, 45 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 30 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 50, 8, 46 }, { 50, 14, 46 }, { 100, 21, -1 } },
        { { 70, 45, 46 }, { 60, 45, 46 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 50, 38, 46 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 80, 40, 29 }, { 80, 40, 29 }, { 100, 21, -1 } },
        { { 80, 29, 37 }, { 50, 29, 40 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 21, -1 } },
        { { 60, 46, 45 }, { 60, 46, 45 }, { 100, 21, -1 } },
        { { 30, 41, 45 }, { 30, 41, 45 }, { 100, 21, -1 } },
        { { 100, 29, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 46, -1 } },
        { { 50, 29, 46 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 50, 40, 29 }, { 50, 40, 29 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 30, 12, 29 }, { 98, 46, 22 }, { 100, 21, -1 } },
        { { 90, 29, 31 }, { 100, 29, -1 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 46, -1 } },
        { { 50, 36, 46 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    },
    {
        { { 40, 40, 29 }, { 40, 40, 29 }, { 100, 21, -1 } },
        { { 100, 18, -1 }, { 100, 18, -1 }, { 100, 21, -1 } },
        { { 100, 39, -1 }, { 100, 39, -1 }, { 100, 39, -1 } },
        { { 90, 46, 36 }, { 90, 46, 12 }, { 100, 21, -1 } },
        { { 100, 29, -1 }, { 100, 29, -1 }, { 100, 21, -1 } },
        { { 100, 45, -1 }, { 100, 45, -1 }, { 100, 45, -1 } },
        { { 100, 46, -1 }, { 100, 46, -1 }, { 100, 21, -1 } },
        { { 10, 11, 46 }, { 100, 46, -1 }, { 100, 46, -1 } },
        { { 100, 2, -1 }, { 100, 2, -1 }, { 100, 2, -1 } }
    }
};
signed char acPilotAggression[24] = { /* 0x0046d9a0 */
    3, 3, 3, 2, 2, 3, 2, 3, 0, 3, 2, 5,
    4, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0
};
signed char acPilotRecovery[20] = { /* 0x0046d9b8 */
    6, 7, 8, 8, 9, 8, 6, 8, 10, 7,
    8, 9, 7, 0, 7, 7, 8, 8, 0, 0
};
InputEvent *pInputEventHead; /* 0x0046da90 */
InputEvent *pInputEventTail; /* 0x0046da94 */
int bInputEventPoolInitialized; /* 0x0046da98 */
unsigned char *pDrawnMouseCursorShape; /* 0x0046da9c */
int nMouseCursorShowCount; /* 0x0046daa0 */
short nScreenWidth = 320; /* 0x0046daa4 */
short nScreenHeight = 200; /* 0x0046daa8 */
const ScreenViewportGeometry aScreenViewportGeometry[6] = { /* 0x0046dab8 */
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 320, 128, 0, 24, 0, 24, -24576, -1 },
    { 320, 200, 0, 0, 0, 0, -1536, -1 }
};
const short asPilotHandOrigins[10] = { /* 0x0046e120 */
    154, 187, 154, 187, 154, 187, 154, 187, 154, 187
};
CampaignDate stSavedCampaignDate = {20, 340}; /* 0x0046e188 */
const TalkingHeadOrigin aTalkingHeadOrigins[11] = { /* 0x0046e190 */
    {161, 60, 161, 90},
    {161, 60, 161, 87},
    {160, 60, 159, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {161, 60, 161, 90},
    {160, 53, 160, 88},
};
const signed char abBriefingSmallCharacterAnimation[24] = { /* 0x0046e1e8 */
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0
};
const signed char abBriefingLargeCharacterAnimation[24] = { /* 0x0046e200 */
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0
};
BriefingCharacterLayout aBriefingCharacters[8] = { /* 0x0046e218 */
    { 60, 123,  10, 95, 176, 1, 0, 2,
      abBriefingSmallCharacterAnimation, 0},
    {316, 123, 264, 94, 176, 1, 2, 1,
      abBriefingSmallCharacterAnimation, 0},
    {193, 123, 141, 95, 176, 1, 3, 1,
      abBriefingSmallCharacterAnimation, 0},
    {250, 124, 199, 93, 176, 1, 4, 1,
      abBriefingSmallCharacterAnimation, 0},
    {124, 123,  71, 94, 176, 1, 5, 1,
      abBriefingSmallCharacterAnimation, 0},
    {103, 122,  29, 76, 256, 1, 6, 2,
      abBriefingLargeCharacterAnimation, 0},
    {191, 122, 118, 76, 256, 1, 8, 1,
      abBriefingLargeCharacterAnimation, 0},
    {287, 122, 212, 76, 256, 1, 9, 1,
      abBriefingLargeCharacterAnimation, 0}
};
const short asMedalDisplayX[5] = {191, 199, 207, 216, 228}; /* 0x0046e2d0 */
const char *apszMedalNames[5] = { /* 0x0046e2e0 */
    szBronzeStar,
    szSilverStar,
    szGoldStar,
    szGoldenSun,
    szTerranMedalOfValor
};
unsigned char *pMedalSceneShape; /* 0x0046e2f4 */
const char *pszMedalsPilotSummary = /* 0x0046e2f8 */
    szMedalsPilotSummary;
const signed char aBriefingPortraitOffsetX[8][12] = { /* 0x0046e300 */
    {  0,  0, -2, -2, -1, -2, -4, -3, -5, -7, -7, -4},
    { -1,  0, -2, -3, -3, -4, -3, -4, -6, -8, -7, -6},
    {  0,  1,  0,  0,  0,  0,  0, -1, -2, -4, -5, -3},
    {  0, -1, -2, -3, -1, -3, -2, -3, -4, -4, -5, -4},
    {  2,  0,  0,  0,  0,  0,  0,  0, -2, -3, -4, -2},
    {  1,  1,  1,  0,  1,  0, -6, -4, -4, -9, -8, -6},
    { -2, -1, -4, -4, -4, -7, -8, -7, -5, -8, -9, -7},
    {  0,  0,  0,  0,  0, -3, -3, -7, -5, -7, -8, -7}
};
const signed char aBriefingPortraitOffsetY[8][12] = { /* 0x0046e360 */
    { -6, -2, -3, -1, -4,-10,-12,-20,-32,-38,-42,-43},
    { -5,  0, -4, -4, -7,-12,-16,-23,-32,-37,-42,-42},
    { -5, -3,  0,  1,  0, -7,-13,-23,-33,-38,-42,-42},
    { -3, -2, -1,  0, -5,-10,-16,-25,-33,-38,-41,-41},
    { -6,  0,  2,  0, -6,-11,-18,-25,-33,-39,-42,-42},
    { -8, -2,  1, -3, -6,-16,-23,-35,-46,-55,-61,-61},
    { -6, -4, -3, -5, -9,-14,-23,-34,-49,-57,-61,-62},
    {-10, -5,  0,  0, -3, -7,-24,-32,-47,-55,-59,-60}
};
const short aBriefingPortraitScale[8][12] = { /* 0x0046e3c0 */
    {357, 357, 357, 359, 355,   0, 352, 355, 353, 353, 355,   1},
    {355,   0, 354, 354, 352, 354, 355, 355, 352, 352, 353, 350},
    {354, 354, 359,   0, 358, 357, 357, 356, 357,   0,   0,   2},
    {354, 354, 353, 356, 358, 356, 356, 355, 356,   0, 357, 357},
    {  0,   0,   0,   0, 358,   0, 358,   0,   0,   4,   0, 359},
    {  0, 358, 358,   0, 357,   0, 350, 354, 357, 354,   0, 358},
    {356, 356, 352, 354, 352, 349, 350, 352,   0,   0,   0,   0},
    {  0,   0,   0, 359,   0, 350, 354, 350, 356, 357, 357, 357}
};
const int aiBriefingLeftPanelVelocity[12] = { /* 0x0046e480 */
    1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4
};
const int aiBriefingPodiumVelocity[12] = { /* 0x0046e4b0 */
    1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 6
};
const int aiBriefingRightPanelVelocity[12] = { /* 0x0046e4e0 */
    2, 2, 3, 4, 4, 4, 5, 5, 5, 6, 7, 8
};
const signed char abBriefingPodiumFrames[40] = { /* 0x0046e510 */
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 14, 13, 12, 11, 10,  9,
     8,  7,  6,  5,  4,  0,  0,  0,  0,  0
};
const signed char abDebriefingEstablishDeltas[48] = { /* 0x0046e538 */
    -2, -2, -1, -1,  0,  0,  1,  0,
     1,  0,  1,  0,  1,  0,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  0,  1,  0,  1,
     0,  1,  0,  1,  0,  0, -1, -1
};
short nDebriefingLeftX; /* 0x0046e56c */
short nDebriefingPilotX = 80; /* 0x0046e570 */
short nDebriefingRightX = 278; /* 0x0046e574 */
short nDebriefingOfficerX = 200; /* 0x0046e578 */
short nDebriefingPodiumX = 344; /* 0x0046e57c */
short nConversationCharacter = -1; /* 0x0046e580 */
short nTalkingHeadFace = -1; /* 0x0046e584 */
short nConversationBackdropFrame = -1; /* 0x0046e588 */
int bConversationConstellation; /* 0x0046e58c */
int bConversationOverlay; /* 0x0046e590 */
const char szBronzeStar[12] = "Bronze Star"; /* 0x0046e594 */
const char szSilverStar[12] = "Silver Star"; /* 0x0046e5a0 */
const char szGoldStar[12] = "Gold Star"; /* 0x0046e5ac */
const char szGoldenSun[12] = "Golden Sun"; /* 0x0046e5b8 */
const char szTerranMedalOfValor[24] = /* 0x0046e5c4 */
    "Terran Medal of Valor";
const char szMedalsPilotSummary[40] = /* 0x0046e5dc */
    "$R $N, aka $C.\n$S system, dateline $D.";
const char szViewMedalsTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e604 */
const char szMedalChestTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e610 */
const char szMedalLongShotTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e61c */
const char szMedalEstablishTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e628 */
const char szPinMedalTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e634 */
const char szEstablishingShotTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e640 */
const char szBriefingReturnTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e64c */
const char szDismissedTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e658 */
const char szDebriefEstablishTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e664 */
const char szFuneralLongShotTextFormat[12] = "%X%Y%F%s%P"; /* 0x0046e670 */
void *apRasterDriverCallbacks[13]; /* 0x0046e69c */
char szRasterDriverName[13] = "MCGA.DLL"; /* 0x0046e6d0 */
unsigned char *pRLEEncodeBuffer; /* 0x0046e6dd */
int nRLEPendingSkip; /* 0x0046e6e1 */
unsigned char *pRLEScanlineStart; /* 0x0046e6e5 */
unsigned char *pRLEScanCursor; /* 0x0046e6e9 */
unsigned char *pRLEOutputCursor; /* 0x0046e6ed */
unsigned char *pRLELiteralStart; /* 0x0046e6f1 */
int nRLEEncodedMinimumX; /* 0x0046e705 */
int nRLEEncodedMinimumY; /* 0x0046e709 */
int nRLEEncodedMaximumX; /* 0x0046e70d */
int nRLEEncodedMaximumY; /* 0x0046e711 */
unsigned char abRasterDecodeBuffer[0xd00]; /* 0x0046ec15 */
unsigned char abRasterPaletteUsageScratch[0x300]; /* 0x0046f915 */
unsigned char abRasterFadeAccumulator[0x300]; /* 0x0046fc15 */
const unsigned char abGifLZWCodeMasks[9] = { /* 0x0046ff15 */
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};
const unsigned char abGifInterlaceRowSteps[5] = { /* 0x0046ff1e */
    8, 8, 4, 2, 0
};
const unsigned char abGifInterlaceRowStarts[5] = { /* 0x0046ff23 */
    0, 4, 2, 1, 0
};
RasterClip *pGifRasterTarget; /* 0x0046ff28 */
unsigned char abRasterPaletteTranslation[256]; /* 0x0046ff2c */
RLETransformVertex aRLETransformVertices[4]; /* 0x0047002c */
int anRLESourceSteps[4]; /* 0x0047007c */
const char *pszBlankRoomMenuLabel = /* 0x00470090 */
    szBlankRoomMenuLabel;
const char *apszPilotRankNames[5] = { /* 0x00470098 */
    szSecondLieutenantRank,
    szFirstLieutenantRank,
    szCaptainRank,
    szMajorRank,
    szLieutenantColonelRank
};
CampaignState stInitialCampaignState = { /* 0x004700b0 */
    0,
    OBJECT_TYPE_HORNET,
    {0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0,
    1,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1},
    {110, 2654},
    {6, 0},
    0,
    0,
    0,
    0
};
PilotRecord aInitialPilotRecords[9] = { /* 0x00470108 */
    {"TANAKA",   "SPIRIT",  3, 1, 11, 14, 1},
    {"ST.JOHN",  "HUNTER",  4, 2, 25, 32, 4},
    {"CHEN",     "BOSSMAN", 1, 3, 35, 37, 2},
    {"CASEY",    "ICEMAN",  0, 3, 28, 43, 1},
    {"DEVEREAUX", "ANGEL",   0, 2, 22, 20, 1},
    {"TAGGART",  "PALADIN", 2, 3, 42, 34, 2},
    {"MARSHALL", "MANIAC",  4, 0, 5, 6, 1},
    {"KHUMALO",  "KNIGHT",  3, 2, 18, 23, 3},
    {"PELLEY",   "GOBLIN",  0, 0, 0, 0, 0}
};
signed char abSpiritRecRoomAnimation[48] = { /* 0x00470260 */
    0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 3, 4, 3, 3, 4,
    3, 4, 3, 4, 5, 3, 4, 5, 3, 4, 3, 4, 3, 4, 5, 3,
    2, 2, 2, 3, 2, 2, 2, 1, 1, 1, -1, 0, 0, 0, 0, 0
};
signed char abHunterRecRoomAnimation[40] = { /* 0x00470290 */
    0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 2, 2,
    3, 3, 4, 4, 5, 3, 3, 4, 4, 5, 5, 5, 2, 2, 1, 1,
    0, 0, -1, 0, 0, 0, 0, 0
};
signed char abAngelRecRoomAnimation[48] = { /* 0x004702b8 */
    3, 4, 3, 3, 4, 3, 4, 4, 3, 4, 3, 4, 0, 0, 0, 2,
    0, 2, 0, 1, 0, 2, 0, 0, 2, 0, 1, 0, 2, 3, 3, 4,
    3, 4, 4, 5, 5, 5, 5, 5, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char abKnightRecRoomAnimation[40] = { /* 0x004702e8 */
    0, 0, 0, 0, 0, 1, 0, 1, 0, 5, 0, 0, 1, 2, 3, 4,
    4, 3, 2, 2, 2, 3, 4, 4, 5, 5, 0, 0, 1, 0, 1, 2,
    1, 0, 1, -1, 0, 0, 0, 0
};
signed char abIcemanRecRoomAnimation[40] = { /* 0x00470310 */
    0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 3,
    3, 2, 2, 1, 0, 0, -1, 0
};
signed char abManiacRecRoomAnimation[24] = { /* 0x00470338 */
    0, 1, 2, 0, 1, 2, 0, 3, 2, 0, 1, 1,
    0, 4, 2, 0, 1, 2, 0, 5, 5, 5, 5, -1
};
signed char abPaladinRecRoomAnimation[56] = { /* 0x00470350 */
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 2, 2, 2, 3, 2,
    2, 3, 2, 3, 2, 3, 2, 4, 4, 2, 3, 2, 3, 2, 3, 2,
    3, 2, 3, 4, 4, 5, 5, 4, 4, 4, 4, 5, 5, 4, 2, 2,
    2, -1, 0, 0, 0, 0, 0, 0
};
signed char abBossmanRecRoomAnimation[48] = { /* 0x00470388 */
    3, 3, 3, 2, 3, 2, 3, 2, 3, 3, 2, 3, 1, 3, 1, 3,
    0, 3, 0, 3, 0, 2, 1, 2, 1, 3, 3, 4, 5, 5, 4, 5,
    4, 5, 1, 3, 1, 3, 1, 3, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char abShotglassIdleAnimation[24] = { /* 0x004703b8 */
    0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 2, 0,
    0, 3, 3, 3, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char abShotglassGlassAnimation[32] = { /* 0x004703d0 */
    3, 6, 7, 8, 8, 9, 9, 10, 10, 10, 12, 11, 10, 12, 11, 10,
    12, 11, 10, 12, 11, 10, 13, 13, 13, 10, 9, 8, 8, 3, 3, -1
};
signed char abShotglassPourAnimation[80] = { /* 0x004703f0 */
    3, 3, 6, 6, 7, 8, 8, 14, 15, 15, 16, 17, 18, 18, 18, 18,
    17, 19, 19, 20, 20, 21, 20, 21, 20, 21, 20, 21, 20, 21, 20, 21,
    22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 28, 29, 30, 31,
    31, 31, 31, 31, 32, 32, 31, 31, 31, 33, 33, 32, 34, 34, 35, 35,
    36, 36, 37, 37, 37, 38, 38, 39, 40, 3, 3, 3, -1, 0, 0, 0
};
signed char abShotglassWipeAnimation[24] = { /* 0x00470440 */
    3, 4, 3, 4, 5, 3, 4, 3, 5, 6, 3, 3,
    3, 3, 4, 3, 4, 3, -1, 0, 0, 0, 0, 0
};
signed char *apRecRoomAnimations[14] = { /* 0x00470458 */
    abSpiritRecRoomAnimation,
    abHunterRecRoomAnimation,
    abBossmanRecRoomAnimation,
    abIcemanRecRoomAnimation,
    abAngelRecRoomAnimation,
    abPaladinRecRoomAnimation,
    abManiacRecRoomAnimation,
    abKnightRecRoomAnimation,
    0,
    abShotglassIdleAnimation,
    abShotglassGlassAnimation,
    abShotglassPourAnimation,
    abShotglassWipeAnimation,
    0
};
const ShortPoint aRecRoomCharacterOrigins[3] = { /* 0x00470490 */
    {94, 59}, {161, 79}, {202, 79}
};
short nRecRoomPanelOriginX = 158; /* 0x0047049c */
short nRecRoomPanelOriginY = 28; /* 0x0047049e */
TitleMenuRegion aRecRoomMenuRegions[7] = { /* 0x004704a0 */
    { 1, 94, 59, 130, 95 },
    { 1, 161, 79, 180, 95 },
    { 1, 210, 79, 240, 95 },
    { 1, 180, 50, 250, 75 },
    { 1, 275, 50, 319, 135 },
    { 1, 0, 100, 120, 190 },
    { -1, 0, 0, 0, 0 }
};
char *apszRecRoomBaseLabels[3] = { /* 0x004704e8 */
    szTalkToShotglass,
    szTalkToFirstPilot,
    szTalkToSecondPilot
};
char *apszRecRoomMenuLabels[6] = { /* 0x004704f8 */
    0, 0, 0,
    szCheckPilotScores,
    szEnterBarracks,
    szFlyTrainingMission
};
int bPanRoomTransition; /* 0x00470510 */
CampaignDate stChalkBoardDate = {-1, -1}; /* 0x00470514 */
short asChalkBoardPilotOrder[9] = { /* 0x00470518 */
    0, 1, 2, 3, 4, 5, 6, 7, 8
};
const char szBlankRoomMenuLabel[4] = " "; /* 0x0047052c */
const char szSecondLieutenantRank[8] = "2ND LT."; /* 0x00470530 */
const char szFirstLieutenantRank[8] = "1ST LT."; /* 0x00470538 */
const char szCaptainRank[8] = "CAPTAIN"; /* 0x00470540 */
const char szMajorRank[8] = "MAJOR"; /* 0x00470548 */
const char szLieutenantColonelRank[12] = "LT. COL."; /* 0x00470550 */
char szTalkToShotglass[20] = "Talk to SHOTGLASS."; /* 0x0047055c */
char szTalkToFirstPilot[24] = "Talk to ??????????????"; /* 0x00470570 */
char szTalkToSecondPilot[24] = "Talk to ??????????????"; /* 0x00470588 */
char szCheckPilotScores[20] = "Check pilot scores"; /* 0x004705a0 */
char szEnterBarracks[16] = "Enter barracks"; /* 0x004705b4 */
char szFlyTrainingMission[24] = "Fly training mission"; /* 0x004705c4 */
const char szRoomMenuLabelFormat[12] = "%X%Y%s%P"; /* 0x004705dc */
const char szEmptyRoomMenuLabel[4] = ""; /* 0x004705e8 */
const char szTalkToPilotFormat1[16] = "Talk to %s."; /* 0x004705ec */
const char szTalkToPilotFormat2[16] = "Talk to %s."; /* 0x004705fc */
const char szChalkBoardHeading[32] = /* 0x0047060c */
    "%X%YCARRIER - TIGER'S CLAW%P";
const char szChalkBoardPilotHeading[12] = "%X%YPILOT"; /* 0x0047062c */
const char szChalkBoardScoreHeading[20] = /* 0x00470638 */
    "%X%YSORTIES KILLS";
const char szChalkBoardPilotFormat[12] = "%X%Y%s %s"; /* 0x0047064c */
const char szChalkBoardScoreFormat[12] = "%X%Y%d%X%d"; /* 0x00470658 */
const char szKilledInAction[4] = "KIA"; /* 0x00470664 */
const char szChalkBoardKiaFormat[16] = "%X%Y     %s"; /* 0x00470668 */
unsigned char abPaletteTranslation[256] = { /* 0x00470678 */
      0,   1,   2,   3,   4,   5,   6,   7,
      8,   9,  10,  11,  12,  13,  14,  15,
     16,  17,  18,  19,  20,  21,  22,  23,
     24,  25,  26,  27,  28,  29,  30,  31,
     32,  33,  34,  35,  36,  37,  38,  39,
     40,  41,  42,  43,  44,  45,  46,  47,
     48,  49,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,
     64,  65,  66,  67,  68,  69,  70,  71,
     72,  73,  74,  75,  76,  77,  78,  79,
     80,  81,  82,  83,  84,  85,  86,  87,
     88,  89,  90,  91,  92,  93,  94,  95,
     96,  97,  98,  99, 100, 101, 102, 103,
    104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
    128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151,
    152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183,
    184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199,
    200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215,
    216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231,
    232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247,
    248, 249, 250, 251, 252, 253, 254, 255
};
unsigned short awAbsoluteCosine[360] = { /* 0x00470778 */
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251,
    250, 250, 248, 247, 246, 245, 243, 242, 241, 239, 237, 236,
    234, 232, 230, 228, 226, 224, 221, 219, 217, 215, 212, 210,
    207, 204, 202, 198, 196, 193, 190, 187, 184, 181, 178, 175,
    171, 168, 165, 161, 158, 154, 150, 147, 143, 139, 136, 131,
    128, 124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  83,
     79,  74,  71,  66,  62,  58,  53,  49,  45,  40,  36,  31,
     27,  22,  18,  13,   9,   4,   0,   4,   9,  13,  18,  22,
     27,  31,  36,  40,  45,  49,  53,  58,  62,  66,  71,  75,
     79,  83,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
    128, 131, 136, 139, 143, 147, 150, 154, 158, 161, 165, 168,
    171, 175, 178, 181, 184, 187, 190, 193, 196, 198, 202, 204,
    207, 210, 212, 215, 217, 219, 221, 224, 226, 228, 230, 232,
    234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 250,
    250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 251,
    250, 250, 248, 247, 246, 245, 243, 242, 241, 239, 237, 236,
    234, 232, 230, 228, 226, 224, 221, 219, 217, 215, 212, 210,
    207, 204, 202, 198, 196, 193, 190, 187, 184, 181, 178, 175,
    171, 168, 165, 161, 158, 154, 150, 147, 143, 139, 136, 131,
    128, 124, 120, 116, 112, 108, 104, 100,  96,  92,  88,  83,
     79,  74,  71,  66,  62,  58,  53,  49,  45,  40,  36,  31,
     27,  22,  18,  13,   9,   4,   0,   4,   9,  13,  18,  22,
     27,  31,  36,  40,  45,  49,  53,  58,  62,  66,  71,  75,
     79,  83,  88,  92,  96, 100, 104, 108, 112, 116, 120, 124,
    128, 131, 136, 139, 143, 147, 150, 154, 158, 161, 165, 168,
    171, 175, 178, 181, 184, 187, 190, 193, 196, 198, 202, 204,
    207, 210, 212, 215, 217, 219, 221, 224, 226, 228, 230, 232,
    234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 250,
    250, 251, 252, 253, 254, 254, 255, 255, 255, 255, 255, 255
};
unsigned short awAbsoluteSine[360] = { /* 0x00470a48 */
      0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  45,  49,
     53,  58,  62,  66,  71,  75,  79,  83,  88,  92,  96, 100,
    104, 108, 112, 116, 120, 124, 128, 131, 136, 139, 143, 147,
    150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
    190, 193, 196, 198, 202, 204, 207, 210, 212, 215, 217, 219,
    221, 224, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242,
    243, 245, 246, 247, 248, 250, 250, 251, 252, 253, 254, 254,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 254, 254, 253, 252, 251, 250, 250, 248, 247, 246, 245,
    243, 242, 241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
    221, 219, 217, 215, 212, 210, 207, 204, 202, 198, 196, 193,
    190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
    150, 147, 143, 139, 136, 131, 128, 124, 120, 116, 112, 108,
    104, 100,  96,  92,  88,  83,  79,  74,  71,  66,  62,  58,
     53,  49,  45,  40,  36,  31,  27,  22,  18,  13,   9,   4,
      0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  45,  49,
     53,  58,  62,  66,  71,  75,  79,  83,  88,  92,  96, 100,
    104, 108, 112, 116, 120, 124, 128, 131, 136, 139, 143, 147,
    150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
    190, 193, 196, 198, 202, 204, 207, 210, 212, 215, 217, 219,
    221, 224, 226, 228, 230, 232, 234, 236, 237, 239, 241, 242,
    243, 245, 246, 247, 248, 250, 250, 251, 252, 253, 254, 254,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 254, 254, 253, 252, 251, 250, 250, 248, 247, 246, 245,
    243, 242, 241, 239, 237, 236, 234, 232, 230, 228, 226, 224,
    221, 219, 217, 215, 212, 210, 207, 204, 202, 198, 196, 193,
    190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
    150, 147, 143, 139, 136, 131, 128, 124, 120, 116, 112, 108,
    104, 100,  96,  92,  88,  83,  79,  74,  71,  66,  62,  58,
     53,  49,  45,  40,  36,  31,  27,  22,  18,  13,   9,   4
};
const char szInvalidShapeAllocation[12] = "not jefftep"; /* 0x00470d18 */
const char szBadViewport[12] = "bad vport"; /* 0x00470d24 */
const char szShapeRLEVersion[8] = "1.00"; /* 0x00470d30 */
const char szShapeRLEOverflow[20] = "qq copy overflow"; /* 0x00470d38 */
const char szBadShapeFlip[12] = "bad flip"; /* 0x00470d4c */
const char szCopyLine[12] = "__copyline"; /* 0x00470d58 */
const char szWipeLine[12] = "__wipeline"; /* 0x00470d64 */
const char szTriangle[12] = "triangle"; /* 0x00470d70 */
const char szShadedTriangle[16] = "shaded_triangle"; /* 0x00470d7c */
const char szShadowDraw[12] = "shadow_draw"; /* 0x00470d8c */
const char szFizzleFade[12] = "fizzle_fade"; /* 0x00470d98 */
const char szSnowViewport[16] = "snow_viewport"; /* 0x00470da4 */
int nInsertKeyState; /* 0x00475b68 */
int nClearedKeyboardState; /* 0x00475b6c */
int nOemPeriodKeyState; /* 0x00475bf0 */
int nHomeKeyState; /* 0x00475bf4 */
int nPageDownKeyState; /* 0x00475bf8 */
int nOemCommaKeyState; /* 0x00475bfc */
int nRightArrowKeyState; /* 0x00475c00 */
int nEndKeyState; /* 0x00475c04 */
int nClearKeyState; /* 0x00475c08 */
int nDeleteKeyState; /* 0x00475c0c */
int nUpArrowKeyState; /* 0x00475c10 */
int nDownArrowKeyState; /* 0x00475c14 */
char szStreamsPath[0x100]; /* 0x00475c18 */
int nPageUpKeyState; /* 0x00475d18 */
int nLeftArrowKeyState; /* 0x00475d1c */
char szReadDataFileError[0x40]; /* 0x00475d20 */
char szCreateDataFileError[0x40]; /* 0x00475d60 */
char szWriteDataFileError[0x40]; /* 0x00475da0 */
char szSeekDataFileError[0x80]; /* 0x00475de0 */
int bMonoDebugInstalled; /* 0x00475e70 */
HANDLE hMonoDebugDevice; /* 0x00475e74 */
short DAT_00475e78;
unsigned char bCurrentManeuverReroll; /* 0x00475e7c */
NavMapLabel aNavMapLabels[20]; /* 0x00475e80 */
ShortRect aNavMapExclusionRects[21]; /* 0x00475f48 */
unsigned char abMouseCursorBackground[0x400]; /* 0x00475ff0 */
DebugOverlayConsole *pDebugOverlay; /* 0x004763f0 */
int nFlightFrameTotalTicks; /* 0x004764f8 */
int nFlightCockpitTicks; /* 0x004764fc */
LARGE_INTEGER liFlightAfterInput; /* 0x00476500 */
LARGE_INTEGER liFlightFrameEnd; /* 0x00476508 */
int nFlightPresentTicks; /* 0x00476510 */
LARGE_INTEGER liFlightFrameStart; /* 0x00476518 */
LARGE_INTEGER liFlightAfterSimulation; /* 0x00476520 */
int nFlightSimulationTicks; /* 0x00476528 */
LARGE_INTEGER liFlightAfterCockpit; /* 0x00476530 */
int nFlightInputTicks; /* 0x00476538 */
LARGE_INTEGER liFlightAfterRender; /* 0x00476540 */
int nFlightRenderTicks; /* 0x00476548 */
WaveTableEntry *pSnowStaticWave; /* 0x0047654c */
IxSound *pSnowStaticSound; /* 0x00476550 */
int nDisplayedComponentDamage; /* 0x00476554 */
char szSfxWavePath[0xc8]; /* 0x00476558 */
unsigned char szFrameRateText[32]; /* 0x00476620 */
unsigned int DAT_00476640;
unsigned int DAT_00476644;
unsigned char *pDIBPixelBuffer; /* 0x00476648 */
unsigned int nDIBWidth; /* 0x0047664c */
unsigned int nDIBHeight; /* 0x00476650 */
unsigned char abDIBPixelBackup[0xfa00]; /* 0x00476658 */
int DAT_00486058;
int nDIBSlamCount; /* 0x00486060 */
HWND hDIBWindow; /* 0x00486074 */
char szDIBErrorMessage[0x98]; /* 0x00486078 */
unsigned char abDIBPaletteCache[1024]; /* 0x00486110 */
float fMeasuredFrameRate; /* 0x00486510 */
int bDIBSlamPending; /* 0x00486518 */
char szSpaceFlightMaxFpsMessage[0x80]; /* 0x00486528 */
unsigned char abCursorSaveArea[0x1000]; /* 0x004865a8 */
RasterSurface stRasterSurface; /* 0x004875a8 */
unsigned char abShapeTransformScratch[0xfa00]; /* 0x004875c0 */
RasterClip stRasterClip; /* 0x00496fc0 */
unsigned char abSolidColourTranslation[256]; /* 0x00497648 */
unsigned char abShapeRLEScratch[0x100000]; /* 0x00497748 */
extern unsigned int dwStreamerState; /* 0x00597cd0 */
int bStreamerAudioPlaying; /* 0x00597748 */
char szStreamerPath[128]; /* 0x00597750 */
int DAT_00598888;
int DAT_0059888c;
int DAT_00598890;
Viewport stRoomScreenViewport; /* 0x005988a0 */
short nSavedRoomControllerX; /* 0x005988b4 */
unsigned char *pRecRoomRoster; /* 0x005988b8 */
unsigned char *pMissionCampaignData; /* 0x005988bc */
unsigned char *apRecRoomCharacterShapes[3]; /* 0x005988c0 */
PilotRecord aPilotRecords[9]; /* 0x005988d0 */
ConstellationObjectDefinition *pConstellationDefinitions; /* 0x00598a28 */
PilotRecord *apWingmanPilots[8]; /* 0x00598a30 */
unsigned char *pRecRoomBackgroundShape; /* 0x00598a50 */
Viewport stRoomDisplayViewport; /* 0x00598a60 */
Viewport stRoomMouseViewport; /* 0x00598a80 */
unsigned char *apRecRoomTextData[3]; /* 0x00598aa0 */
short nRoomMenuCursorFrame; /* 0x00598ab0 */
TitleMenuRegion *pRoomMenuRegions; /* 0x00598ab2 */
char **ppszRoomMenuLabels; /* 0x00598ab6 */
const char *pszCurrentRoomMenuLabel; /* 0x00598aba */
TextContext stRoomMenuTextContext; /* 0x00598abe */
unsigned char *apRecRoomSceneData[3]; /* 0x00598ae0 */
unsigned char *pBriefingPacket; /* 0x00598aec */
unsigned char *pBriefingTextData; /* 0x00598af0 */
short *pMouthAnimationCommands; /* 0x00598af4 */
unsigned char *pMedalTextData; /* 0x00598af8 */
unsigned char *pDebriefingSceneData; /* 0x00598afc */
char szTextScratchBuffer[256]; /* 0x00598b00 */
unsigned char *pBriefingSceneData; /* 0x00598c00 */
unsigned char *pConversationBackdropShape; /* 0x00598c04 */
short nConversationMedalIndex; /* 0x00598c08 */
short nConversationSceneType; /* 0x00598c0a */
unsigned char *pTalkingHeadShape; /* 0x00598c0c */
short nConversationTextColour; /* 0x00598c10 */
unsigned char *pBriefingAnimationShape; /* 0x00598c14 */
short *pFaceAnimationCommands; /* 0x00598c18 */
unsigned char *pBriefingBodyShape; /* 0x00598c1c */
unsigned char *pMedalSceneData; /* 0x00598c20 */
unsigned char *pBriefingPortraitShape; /* 0x00598c24 */
unsigned char *pDebriefingTextData; /* 0x00598c28 */
unsigned char *pBriefingCloseupShape; /* 0x00598c2c */
unsigned char *pConversationOverlayShape; /* 0x00598c30 */
unsigned char bFilteredKeyWaitStarted; /* 0x00598c34 */
InputEvent aInputEventPool[0x100]; /* 0x00598c40 */
int nMouseCursorSavedY; /* 0x0059a840 */
int nMouseCursorSavedX; /* 0x0059a844 */
unsigned char bInputMode; /* 0x0059a848 */
unsigned char bFilteredKeyWaitActive; /* 0x0059a849 */
int bMouseCursorDrawn; /* 0x0059a84c */
short nEventManagerActive; /* 0x0059a850 */
short nViewCenterX; /* 0x0059a852 */
short nViewCenterY; /* 0x0059a854 */
short bExpandedMemoryAvailable; /* 0x0059a856 */
short nPacketDecompressSourceFile; /* 0x0059a858 */
unsigned char abInputKeyState[0x80]; /* 0x0059a860 */
int nMouseCursorDrawnY; /* 0x0059a8e0 */
int nMouseCursorDrawnX; /* 0x0059a8e4 */
int anObjectDepthPlaced[WC1_SPACE_OBJECT_COUNT]; /* 0x0059a8f0 */
short DAT_0059a9f0;
signed char cScreenViewportMode; /* 0x0059a9f2 */
const ScreenViewportGeometry * volatile
    pScreenViewportGeometry; /* 0x0059a9f4 */
int anSortedObject[WC1_SPACE_OBJECT_COUNT]; /* 0x0059aa00 */
short nPacketDecompressInputPosition; /* 0x0059ab00 */
void *pPacketDecompressInput; /* 0x0059ab04 */
unsigned short wCurrentInputModifiers; /* 0x0059ab08 */
MouseCursorState stMouseCursorState; /* 0x0059ab10 */
void (*pEventManagerPump)(void); /* 0x0059ab2c */
int nPacketDecompressResult; /* 0x0059ab30 */
short DAT_0059ab34;
short nPacketDecompressPending; /* 0x0059ab36 */
unsigned short wPacketDecompressInputSize; /* 0x0059ab38 */
short nPacketDecompressWorkspaceSegment; /* 0x0059ab3a */
volatile int bFrameTickPending; /* 0x0059ab3c */
int bMouseCursorDamagePending; /* 0x0059ab40 */
int nMouseCursorDamageRight; /* 0x0059ab44 */
int nMouseCursorDamageBottom; /* 0x0059ab48 */
int DAT_0059ab4c;
short nViewportOriginY; /* 0x0059ab50 */
short nViewportOriginX; /* 0x0059ab52 */
unsigned int nTickCount60Hz; /* 0x0059ab54 */
unsigned char bEscapePressed; /* 0x0059ab58 */
int nMouseCursorDamageLeft; /* 0x0059ab5c */
int nMouseCursorDamageTop; /* 0x0059ab60 */
short DAT_0059ab64;
int aiInputEventSlotUsed[0x100]; /* 0x0059ab70 */
MouseCursorState stHostMouseState; /* 0x0059af70 */
TextContext *pCurrentTextContext; /* 0x0059af8c */
int nInputTickScale; /* 0x0059af90 */
FixedVector aObjectViewPosition[WC1_SPACE_OBJECT_COUNT]; /* 0x0059afa0 */
short anObjectPitchRotation[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b2a0 */
int anShipSpeed[64]; /* 0x0059b320 */
short nSpaceFrame; /* 0x0059b420 */
unsigned int anShipAlertFlags[12]; /* 0x0059b430 */
FixedVector vPlayerAcceleration; /* 0x0059b460 */
int anShipFuel[12]; /* 0x0059b470 */
short asObjectDistance[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b4a0 */
ShortVector aShipFormationOffset[10]; /* 0x0059b520 */
enum ObjectType aeObjectType[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b560 */
short asObjectAnimationDelay[WC1_SPACE_OBJECT_COUNT]; /* 0x0059b660 */
FixedVector aShipRightVector[64]; /* 0x0059b6e0 */
FixedVector aShipUpVector[64]; /* 0x0059b9e0 */
FixedVector aShipForwardVector[64]; /* 0x0059bce0 */
HazardField *pActiveHazardField; /* 0x0059bfe0 */
signed char acPlayerComponentDamage[9]; /* 0x0059bff0 */
signed char abFlightPath[WC1_MISSION_OBJECTIVE_COUNT]; /* 0x0059c000 */
FixedVector aShipVelocity[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c010 */
short anYawGoal[16]; /* 0x0059c310 */
short asObjectCounter[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c330 */
short nTargetShip; /* 0x0059c3b0 */
enum SpecialManeuver aeSpecialManeuver[12]; /* 0x0059c3c0 */
enum ShipMissionType aeShipMissionType[12]; /* 0x0059c3f0 */
short asShipCount[16]; /* 0x0059c420 */
short asShipMaximumSpeed[16]; /* 0x0059c440 */
signed char acShipDamage[10]; /* 0x0059c460 */
signed char cMissionObjectiveCount; /* 0x0059c46a */
short asViableTargetDistance[16]; /* 0x0059c470 */
FixedVector aShipPosition[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c490 */
short asObjectRadarRadius[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c790 */
short asShipAfterburnerTimer[16]; /* 0x0059c810 */
short nShipMissionIndices[10]; /* 0x0059c830 */
signed char acShipCommunicator[16]; /* 0x0059c850 */
FixedVector vStarFieldMotion; /* 0x0059c860 */
signed char cCurrentNavPointIndex; /* 0x0059c86c */
short asObjectFlip[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c870 */
short nEyeRollGoal; /* 0x0059c8f0 */
FixedVector vPreviousStarFieldMotion; /* 0x0059c900 */
short nHazardFieldCount; /* 0x0059c90c */
signed char acLastAttacker[16]; /* 0x0059c910 */
signed char acViableTarget[16]; /* 0x0059c920 */
short asActionCount[10]; /* 0x0059c930 */
short nEyeYawGoal; /* 0x0059c944 */
short asObjectScreenScale[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c950 */
short asObjectAfterburnerVelocity[WC1_SPACE_OBJECT_COUNT]; /* 0x0059c9d0 */
CampaignState stCampaignState; /* 0x0059ca50 */
unsigned char aShipWeapons[10][0x47]; /* 0x0059cab0 */
signed char acShipRating[16]; /* 0x0059cd80 */
short asObjectScreenAngle[WC1_SPACE_OBJECT_COUNT]; /* 0x0059cd90 */
short nTargetRange; /* 0x0059ce10 */
unsigned int anVduModeCache[2]; /* 0x0059ce18 */
signed char acObjectOwner[64]; /* 0x0059ce20 */
signed char acShipTarget[16]; /* 0x0059ce60 */
short nPreviousPitchInput; /* 0x0059ce70 */
short nPreviousYawInput; /* 0x0059ce72 */
short nPreviousRollInput; /* 0x0059ce74 */
short anObjectYawRotation[WC1_SPACE_OBJECT_COUNT]; /* 0x0059ce80 */
signed char acPilotHitPoints[16]; /* 0x0059cf00 */
signed char acTurnRegulator[16]; /* 0x0059cf10 */
unsigned char abCollisionAlertTarget[0x10]; /* 0x0059cf20 */
int aiPilotLevel[12]; /* 0x0059cf30 */
short asTargetListRange[16]; /* 0x0059cf60 */
short asObjectDrawY[WC1_SPACE_OBJECT_COUNT]; /* 0x0059cf80 */
short asObjectDrawX[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d000 */
short asPreviousObjectDistance[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d080 */
enum ObjectClass aeObjectClass[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d100 */
enum ShipObjective aeShipObjective[12]; /* 0x0059d200 */
short asObjectViewFrame[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d230 */
int nCurrentObjectiveRange; /* 0x0059d2b0 */
signed char acWingmanMessageState[16]; /* 0x0059d2c0 */
short asCollisionCountdown[16]; /* 0x0059d2d0 */
unsigned char *apObjectShape[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d2f0 */
short nPitchInput; /* 0x0059d3f0 */
short nYawInput; /* 0x0059d3f2 */
short nRollInput; /* 0x0059d3f4 */
short asShipWingLeader[16]; /* 0x0059d400 */
/* The Win32 object allocator reserves slots 0 through 9 for live ships. */
short aasShipArmor[10][4]; /* 0x0059d420 */
short asShipWeaponEnergy[10]; /* 0x0059d470 */
signed char acFormationMemberList[16]; /* 0x0059d490 */
signed char acShipIonDriveDamage[16]; /* 0x0059d4a0 */
short anShipMissionShip[16]; /* 0x0059d4b0 */
FixedVector vToTarget; /* 0x0059d4d0 */
short asCannedCommand[16]; /* 0x0059d4e0 */
unsigned short ausVduModeStack[16]; /* 0x0059d500 */
char acShipSequence[10]; /* 0x0059d520 */
short nTargetFacing; /* 0x0059d52a */
FixedVector aShipDestination[10]; /* 0x0059d530 */
short aasShipShield[10][2]; /* 0x0059d5b0 */
enum ShipTactic aeShipTactic[12]; /* 0x0059d5e0 */
signed char abShipExhaustHeat[10]; /* 0x0059d610 */
short nRenderedSpaceFrame; /* 0x0059d61a */
short nEyePitchGoal; /* 0x0059d61c */
signed char acShipStress[16]; /* 0x0059d620 */
short anRollGoal[16]; /* 0x0059d630 */
enum Side aeShipSide[12]; /* 0x0059d650 */
signed char acShipAiCooldown[16]; /* 0x0059d680 */
FixedVector vCollisionDelta; /* 0x0059d690 */
signed char acLastCollisionObject[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d6a0 */
short aasShipMaximumShield[10][2]; /* 0x0059d6e0 */
short asObjectCollisionRadius[64]; /* 0x0059d710 */
signed char acShipPointingMode[16]; /* 0x0059d790 */
short anPitchGoal[16]; /* 0x0059d7a0 */
signed char abShipNavPointIndex[16]; /* 0x0059d7c0 */
signed char acTurnInterval[16]; /* 0x0059d7d0 */
short anObjectRollRotation[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d7e0 */
signed char abShipTurn[16]; /* 0x0059d860 */
HazardField aHazardFields[7]; /* 0x0059d870 */
short nFacingToTarget; /* 0x0059d920 */
short asObjectScreenY[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d930 */
short asObjectScreenX[WC1_SPACE_OBJECT_COUNT]; /* 0x0059d9b0 */
short asObjectAnimationIndex[WC1_SPACE_OBJECT_COUNT]; /* 0x0059da30 */
signed char cCockpitView; /* 0x0059dab0 */
MissionObjective aMissionObjectives[WC1_MISSION_OBJECTIVE_COUNT]; /* 0x0059dac0 */
enum ShipManeuver aeShipManeuver[12]; /* 0x0059dcb0 */
const short *apCannedSequence[12]; /* 0x0059dce0 */
FixedVector aShipMissionSpot[10]; /* 0x0059dd10 */
short asCapitalShipViewFrame[16]; /* 0x0059dd90 */
signed char acObjectCollisionGraceTicks[ /* 0x0059ddb0 */
    WC1_SPACE_OBJECT_COUNT];
ObjectResourceSlot aObjectResourceSlots[4]; /* 0x0059ddf0 */
signed char acShipDestroyedWeaponCount[16]; /* 0x0059de30 */
short asObjectScale[WC1_SPACE_OBJECT_COUNT]; /* 0x0059de40 */
signed char acVduModeStackDepth[16]; /* 0x0059dec0 */
signed char acShipSpawnNavPoint[16]; /* 0x0059ded0 */
short asShipAccumulatedDamage[WC1_SPACE_OBJECT_COUNT]; /* 0x0059dee0 */
short nCurrentNavPoint; /* 0x0059df60 */
int nJoystickCalibrationMinimumY; /* 0x0059df64 */
int nJoystickCalibrationMinimumX; /* 0x0059df68 */
int nJoystickCalibrationMaximumX; /* 0x0059df6c */
int nJoystickCalibrationMaximumY; /* 0x0059df70 */
unsigned short awPaletteEntryAllocation[256]; /* 0x0059df80 */
unsigned char *apCommPortraitShapes[16]; /* 0x0059e180 */
char szHudMessageBuffer[0x60]; /* 0x0059e1c0 */
char *aapszPilotSpeech[14][11]; /* 0x0059e220 */
signed char abCommMenuChoiceCommand[7]; /* 0x0059e488 */
const char *pszCommMenuHeading; /* 0x0059e490 */
char szCommMenuHeadingBuffer[0x40]; /* 0x0059e4a0 */
const char *apszCommMenuChoiceText[7]; /* 0x0059e4e0 */
int anInputButton2PressTime[2]; /* 0x0059e500 */
short asInputButton1DoubleClick[2]; /* 0x0059e508 */
short asInputButton1Changed[2]; /* 0x0059e50c */
short asInputButton2Changed[2]; /* 0x0059e510 */
short asPreviousInputButtons[2]; /* 0x0059e514 */
int anInputButton1PressTime[2]; /* 0x0059e518 */
short asInputButton2DoubleClick[2]; /* 0x0059e520 */
void *apPacketHandles[0x1000]; /* 0x0059e530 */
int aiPacketHandleOffsets[0x1000]; /* 0x005a2530 */
int nPacketHandleCount; /* 0x005a6530 */
Viewport *pAllocatedScreenViewport; /* 0x005a6534 */
int *DAT_005a6538;
unsigned short awScreenRowOffsets[202]; /* 0x005a6540 */
int nScreenAllocationState; /* 0x005a66e0 */
Viewport *pAllocatedScreenViewportMirror; /* 0x005a66e4 */
int aiSoundEffectSourceActive[0x41]; /* 0x005a66ec */
unsigned char abSoundPlaybackSlots[0xf8]; /* 0x005a67f0 */
int nPassingShipSoundCooldown; /* 0x005a68e8 */
int nAllocateViewportCalls; /* 0x005a68ec */
void *pLastPacketAllocation; /* 0x005a68f0 */
FireworkState aFireworks[30]; /* 0x005a6900 */
unsigned char *pFireworkShape; /* 0x005a6a68 */
ConstellationStar aConstellationStars[10]; /* 0x005a6a70 */
Viewport *pConstellationViewport; /* 0x005a6aac */
short nConstellationStarCount; /* 0x005a6ab0 */
ConstellationParticle aConstellationParticles[16]; /* 0x005a6ac0 */
Viewport stConstellationViewport; /* 0x005a6b40 */
short nConstellationParticleCount; /* 0x005a6b54 */
const short *pViewScript; /* 0x005a6b58 */
short nExternalViewDistance; /* 0x005a6b5c */
Viewport stPilotHand; /* 0x005a6b60 */
Viewport stLeftVdu; /* 0x005a6b80 */
ScreenViewportPacket *pScreenViewportPacket; /* 0x005a6b94 */
Viewport stScreen; /* 0x005a6ba0 */
TextContext stHudMessageTextContext; /* 0x005a6bc0 */
Viewport stCockpitBar; /* 0x005a6be0 */
unsigned char *pTargetLockShape; /* 0x005a6bf4 */
unsigned char *apTextFonts[4]; /* 0x005a6c00 */
FontWorkspace **apFontWorkspaces[4]; /* 0x005a6c10 */
FixedVector aDirectionViewRightVector[WC1_DIRECTION_VIEW_COUNT]; /* 0x005a6c20 */
FixedVector aDirectionViewUpVector[WC1_DIRECTION_VIEW_COUNT]; /* 0x005a6f10 */
FixedVector aDirectionViewForwardVector[WC1_DIRECTION_VIEW_COUNT]; /* 0x005a7200 */
TextContext stLeftVduTextContext; /* 0x005a74f0 */
Viewport stSpaceBuffer; /* 0x005a7510 */
Viewport stRightVdu; /* 0x005a7530 */
Viewport stPilotHandBackdrop; /* 0x005a7550 */
unsigned char *pCockpitWeaponShape; /* 0x005a7564 */
Viewport stConversationTextViewport; /* 0x005a7570 */
char szDefaultTextBuffer[0xc8]; /* 0x005a7590 */
unsigned char *pCockpitIndicatorShape; /* 0x005a7658 */
unsigned char *pConstellationShape; /* 0x005a765c */
unsigned char *pCommMenuCursorShape; /* 0x005a7660 */
Viewport stModalSourceViewport; /* 0x005a7670 */
unsigned char *pPilotHandShape; /* 0x005a7684 */
Viewport stPilotHandComposite; /* 0x005a7690 */
Viewport stSceneBuffer; /* 0x005a76b0 */
short aPaletteFadeEntries[6][3]; /* 0x005a76d0 */
unsigned char *pCockpitDamageShape; /* 0x005a76f4 */
TextContext stRightVduTextContext; /* 0x005a7700 */
TextContext stCockpitReadoutTextContext; /* 0x005a7720 */
TextContext stDefaultTextContext; /* 0x005a7740 */
TextContext stConversationTextContext; /* 0x005a7760 */
short asDamageFlashColour[3]; /* 0x005a7780 */
short nDamageDisplayTicks; /* 0x005a7786 */
short nWeaponDisplayOriginX; /* 0x005a7788 */
short nWeaponDisplayOriginY; /* 0x005a778a */
char szDamageStatusText[0x48]; /* 0x005a7790 */
ShortPoint stDamageSpritePosition; /* 0x005a77d8 */
signed char cDamageDisplayComponent; /* 0x005a77dc */
signed char cDamageDisplayFrame; /* 0x005a77dd */
signed char cDamagedComponentCount; /* 0x005a77de */
int nDamageDisplayState; /* 0x005a77e0 */
int nDamageDisplayPhase; /* 0x005a77e4 */
unsigned char *pLaunchDoorShape; /* 0x005a77e8 */
unsigned int nFrameTimerId; /* 0x005a77ec */
unsigned char abPaletteTriplets[256][3]; /* 0x005a77f0 */
InputDeviceSample stPreviousFlightInput; /* 0x005a7af0 */
short bMouseButtonEventQueued; /* 0x005a7afc */
short bKeyboardEventQueued; /* 0x005a7afe */
short bMouseMoveEventQueued; /* 0x005a7b00 */
char *pStartupArguments[30]; /* 0x005a7b10 */
short bJoystickEventQueued; /* 0x005a7b88 */
short nArcadeStartupParameter; /* 0x005a7b8a */
Viewport stTrainSimTitleDisplayViewport; /* 0x005a7b90 */
Viewport stTrainSimHighScoreBufferViewport; /* 0x005a7bb0 */
int nArcadeScore; /* 0x005a7bc4 */
TextContext stTrainSimTextContext; /* 0x005a7bd0 */
Viewport stTrainSimHighScoreDisplayViewport; /* 0x005a7bf0 */
TextContext stTrainSimHighScoreTextContext; /* 0x005a7c10 */
short nArcadeTimeRemaining; /* 0x005a7c2c */
HighScoreEntry aHighScoreEntries[6]; /* 0x005a7c30 */
int nArcadeWaveBonus; /* 0x005a7c50 */
unsigned char *pSceneAnimationSceneData; /* 0x005a7c54 */
unsigned char *pSceneAnimationPrimaryShape; /* 0x005a7c58 */
unsigned char *pSceneAnimationTextData; /* 0x005a7c5c */
unsigned char *pSceneAnimationPacket; /* 0x005a7c60 */
SceneAnimationObject *pSceneAnimationObjects; /* 0x005a7c64 */
short nSceneAnimationWaitFrames; /* 0x005a7c68 */
unsigned char *pSceneAnimationDefinitions; /* 0x005a7c6c */
unsigned char *pSceneAnimationSecondaryShape; /* 0x005a7c70 */
signed char cCockpitLogicalFile; /* 0x005a7c74 */
unsigned char *apCockpitShapes[4]; /* 0x005a7c80 */
unsigned char *pCinematicViewBackdrop; /* 0x005a7c90 */
unsigned char *pRearViewBackdrop; /* 0x005a7c94 */
int bPlayerDestroyed; /* 0x005a7c98 */
short nPlayerKillCount; /* 0x005a7c9c */
short asCollisionTime[10]; /* 0x005a7ca0 */
int nWingmanKilledThisMission; /* 0x005a7cb4 */
short nWingmanKillCount; /* 0x005a7cb8 */
short nLastFoundShip; /* 0x005a7cba */
short asCollisionPartner[10]; /* 0x005a7cc0 */
int nMemoryConfiguration; /* 0x005a7cd4 */
unsigned int dwOriginalFreeMemory; /* 0x005a7cd8 */
unsigned char *pMouseCursorResource; /* 0x005a7cdc */
int nAvailableGameMemory; /* 0x005a7ce0 */
int nSceneResourceBudget; /* 0x005a7ce4 */
int nAfterburnerSoundDeadline; /* 0x005a7ce8 */
int bAfterburnerSfxActive; /* 0x005a7cec */
DiskFileRecord *pDiskFileRecords; /* 0x005a7cf0 */
Viewport stDiskPromptBackgroundViewport; /* 0x005a7d00 */
unsigned char abDiskPromptDriveState[2]; /* 0x005a7d20 */
Viewport stDiskPromptViewport; /* 0x005a7d40 */
int bPointerMovedByKeyboard; /* 0x005a7d54 */
TextContext stDiskPromptTextContext; /* 0x005a7d60 */
unsigned int dwDiskPromptTopLeft; /* 0x005a7d80 */
unsigned int dwDiskPromptBottomRight; /* 0x005a7d84 */
short *pPaletteTransitionDirection; /* 0x005a7d88 */
short *pPaletteTransitionDelta; /* 0x005a7d8c */
short nPaletteTransitionMaxDelta; /* 0x005a7d90 */
short *pPaletteTransitionAccumulator; /* 0x005a7d94 */
short nPaletteTransitionCountdown; /* 0x005a7d98 */
int DAT_005a7d9c;
signed char cCapitalShipLogicalFile; /* 0x005a7da0 */
FixedVector vNormalizedToTarget; /* 0x005a7db0 */
short nReleaseWeaponDisplayX; /* 0x005a7dbc */
short nReleaseWeaponDisplayY; /* 0x005a7dbe */
enum ObjectType eReleaseWeaponDisplayType; /* 0x005a7dc0 */
unsigned char *pScannerMarkerBackground; /* 0x005a7dc4 */
unsigned char bStickIndicatorFrame; /* 0x005a7dc8 */
short nMessageTimer; /* 0x005a7dca */
short nPendingCockpitDamage; /* 0x005a7dcc */
HudMessageSlot aHudMessageSlots[2]; /* 0x005a7dd0 */
signed char cPreviousTargetObject; /* 0x005a7df2 */
int nSavedMouseCursorY; /* 0x005a7df4 */
int nSavedMouseCursorX; /* 0x005a7df8 */
char szComponentHitMessage[40]; /* 0x005a7e00 */
short nTargetLockMarkerY; /* 0x005a7e28 */
short nCarrierMissionShipIndex; /* 0x005a7e2a */
CockpitReadout aCockpitReadouts[6]; /* 0x005a7e30 */
short nScannerCursorX; /* 0x005a7e6c */
short nScannerCursorY; /* 0x005a7e6e */
unsigned char abCockpitLightState[7]; /* 0x005a7e70 */
short asScannerObjectY[11]; /* 0x005a7e80 */
volatile ShortPoint stCockpitExplosionPosition; /* 0x005a7e98 */
short asScannerObjectX[11]; /* 0x005a7ea0 */
unsigned char abCockpitLightGoal[7]; /* 0x005a7eb8 */
int nDamageAlarmSfxHandle; /* 0x005a7ec0 */
short asScannerBackgroundColour[10]; /* 0x005a7ed0 */
unsigned int anCockpitDamageState[4]; /* 0x005a7ef0 */
unsigned short DAT_005a7f00;
short nStarFieldJRotation; /* 0x005a7f02 */
short nStarFieldIRotation; /* 0x005a7f04 */
short nExternalViewAngle; /* 0x005a7f06 */
unsigned char *pTitleShape; /* 0x005a7f08 */
int nViewportAllocationCount; /* 0x005a7f0c */
unsigned char *apViewportAllocations[128]; /* 0x005a7f10 */
int nFreeViewportCalls; /* 0x005a8110 */
short nPendingCampaignIndex; /* 0x005a8114 */
short nMissionMedalScore; /* 0x005a8116 */
short nCampaignDataSet; /* 0x005a8118 */
volatile int nNearHeapSize; /* 0x005a811c */
volatile int nNearHeapBase; /* 0x005a8120 */
int nNearHeapFirstDescriptor; /* 0x005a8124 */
void * volatile pNearHeapAllocation; /* 0x005a8128 */
short nNavMapMaximumX; /* 0x005a812c */
short nNavMapMinimumX; /* 0x005a812e */
unsigned short awNavObjectiveLabelIndex[16]; /* 0x005a8130 */
short nNavMapMaximumY; /* 0x005a8150 */
short nNavMapCentreX; /* 0x005a8152 */
short nNavMapMinimumY; /* 0x005a8154 */
TextContext stNavMapTextContext; /* 0x005a8160 */
short nNavMapCentreY; /* 0x005a817c */
TextContext stNavLabelTextContext; /* 0x005a8180 */
short nActiveInputDevice; /* 0x005a819c */
int nJoystickVerticalDeadZone; /* 0x005a81a0 */
int nJoystickHorizontalDeadZone; /* 0x005a81a4 */
int nJoystickUpScale; /* 0x005a81a8 */
int nJoystickLeftScale; /* 0x005a81ac */
int nJoystickMaximumX; /* 0x005a81b0 */
int nJoystickMaximumY; /* 0x005a81b4 */
int nJoystickMinimumX; /* 0x005a81b8 */
int nJoystickMinimumY; /* 0x005a81bc */
int nJoystickRawX; /* 0x005a81c0 */
int nJoystickRawY; /* 0x005a81c4 */
int nJoystickVerticalRange; /* 0x005a81c8 */
int nJoystickHorizontalRange; /* 0x005a81cc */
int nJoystickRightScale; /* 0x005a81d0 */
int nJoystickDownScale; /* 0x005a81d4 */
int nJoystickCentreY; /* 0x005a81d8 */
int nJoystickCentreX; /* 0x005a81dc */
int nJoystickFailureValue; /* 0x005a81e0 */
InputDeviceSample aInputDeviceSamples[2]; /* 0x005a81f0 */
short nMenuInputRepeatDelay; /* 0x005a8208 */
unsigned char abMissionAuxData[0x28]; /* 0x005a8210 */
unsigned char abSeriesAuxData[0x28]; /* 0x005a8240 */
MissionObjectiveSource aMissionObjectiveSources[16]; /* 0x005a8270 */
short nMissionEntryNavPoint; /* 0x005a8690 */
short nHomeMissionShipIndex; /* 0x005a8692 */
short nPlayerMissionShipIndex; /* 0x005a8694 */
short nInitialMissionShipIndices[8]; /* 0x005a8696 */
short DAT_005a86a6;
CampaignDate *pCurrentCampaignDate; /* 0x005a86a8 */
CampaignDate *pElapsedCampaignDate; /* 0x005a86ac */
signed char cObjectResourceLogicalFile; /* 0x005a86b0 */
Viewport *pScrambleViewport; /* 0x005a86b4 */
short nScrambleCanopyFrame; /* 0x005a86b8 */
unsigned char *pScrambleDetailShape; /* 0x005a86bc */
short nScrambleCockpitScale; /* 0x005a86c0 */
short nScrambleCockpitDetailX; /* 0x005a86c4 */
short nScrambleCockpitDetailY; /* 0x005a86c6 */
short nFuneralCasketX; /* 0x005a86c8 */
short nFuneralCasketY; /* 0x005a86ca */
short nScrambleShipDetailCount; /* 0x005a86cc */
signed char acScrambleShipDetailIndices[24]; /* 0x005a86d0 */
signed char cScrambleWalkTicks; /* 0x005a86e8 */
unsigned char *pConversationSpecialShape; /* 0x005a86ec */
ShortPoint aFuneralParticles[7]; /* 0x005a86f0 */
unsigned char *pScrambleBackgroundShape; /* 0x005a870c */
short nFuneralParticleDistance; /* 0x005a8710 */
short nScrambleBackgroundY; /* 0x005a8712 */
short nScrambleBackgroundRightX; /* 0x005a8714 */
short nFuneralForegroundX; /* 0x005a8718 */
short nFuneralForegroundY; /* 0x005a871a */
short nFuneralMainScale; /* 0x005a871c */
short nFuneralRifleFrame; /* 0x005a871e */
short nFuneralBaseX; /* 0x005a8720 */
short nFuneralBaseY; /* 0x005a8722 */
short nScrambleShipX; /* 0x005a8724 */
short nScrambleShipY; /* 0x005a8726 */
short nFuneralParticleScale; /* 0x005a8728 */
unsigned char *pScrambleHangarShape; /* 0x005a872c */
unsigned char *pScrambleCockpitShape; /* 0x005a8730 */
short nScrambleWalkerY; /* 0x005a8734 */
short nScrambleCanopyOffset; /* 0x005a8736 */
short nFuneralMainDistance; /* 0x005a8738 */
signed char bScrambleCanopyClosed; /* 0x005a873a */
short nFuneralGuardFrame; /* 0x005a873c */
short nScrambleOverlayX; /* 0x005a8740 */
short nScrambleOverlayY; /* 0x005a8742 */
unsigned char *pScrambleOverlayShape; /* 0x005a8744 */
signed char cScrambleWalkerPair; /* 0x005a8748 */
unsigned char *pScrambleCanopyShape; /* 0x005a874c */
unsigned char *pScrambleShipShape; /* 0x005a8750 */
short nTalkingHeadFaceX; /* 0x005a8754 */
short nTalkingHeadFaceY; /* 0x005a8756 */
short nTalkingHeadMouthY; /* 0x005a8758 */
short nTalkingHeadMouthX; /* 0x005a875a */
char szSoundDebugMessage[512]; /* 0x005a8760 */
unsigned char *pIntroFont; /* 0x005a8960 */
unsigned int nSystemKeyDown; /* 0x005a8964 */
JOYINFO aJoystickInfo[2]; /* 0x005a8970 */
int nHostMouseMessageX; /* 0x005a8990 */
int nHostMouseMessageY; /* 0x005a8994 */
int bHostPrimaryMouseButton; /* 0x005a8998 */
int bHostSecondaryMouseButton; /* 0x005a899c */
HWND hMainWindow; /* 0x005a89a0 */
HANDLE hSingleInstanceSemaphore; /* 0x005a89a4 */
unsigned long dwGameClockBase; /* 0x005a89a8 */
GuardedAllocation *pGuardedAllocationTail; /* 0x005a89ac */
char szMemoryUsage[0x80]; /* 0x005a89b0 */
HDC hMainWindowDC; /* 0x005a8a30 */
HCURSOR hPreviousCursor; /* 0x005a8a34 */
unsigned int nSessionEndTime; /* 0x005a8a38 */
int bMainWindowAlive; /* 0x005a8a3c */
HINSTANCE hAppInstance; /* 0x005a8a40 */
unsigned int nSessionStartTime; /* 0x005a8a44 */
unsigned short awPaletteRgbWords[0x300]; /* 0x005a8a50 */

/* Embedded animation command streams at 0x00466030-0x00466400.  Each
 * command occupies the low word of a four-byte source record. */
static unsigned int anAnimExplosion0[12] = { /* 0x00466030 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa000, 0
};
static unsigned int anAnimExplosion1[22] = { /* 0x00466060 */
    0, 0x406, 1, 0x406, 2, 0x406, 0x406, 3, 0x406, 0x406,
    4, 0x406, 0x406, 0x406, 5, 0x406, 0x406, 0x406, 0x406,
    0x406, 0xa000, 0
};
static unsigned int anAnimExplosion2[14] = { /* 0x004660b8 */
    0, 0x406, 1, 0x406, 2, 0x406, 3, 0x406, 4, 0x406, 5,
    6, 7, 0xa000
};
static unsigned int anAnimLaserSpark[8] = { /* 0x004660f0 */
    0, 1, 2, 3, 4, 5, 0xa000, 0
};
static unsigned int anAnimBlueSpark[6] = { /* 0x00466110 */
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int anAnimRedSpark[6] = { /* 0x00466128 */
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int anAnimSparkTrail[6] = { /* 0x00466140 */
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int anAnimGirder[8] = { /* 0x00466158 */
    0, 1, 2, 3, 4, 5, 0x9000, 0
};
static unsigned int anAnimTubing[8] = { /* 0x00466178 */
    6, 7, 8, 9, 10, 11, 0x9000, 0
};
static unsigned int anAnimGlass[18] = { /* 0x00466198 */
    12, 13, 14, 15, 16, 17, 18, 19, 0x92, 0x91, 0x90,
    0x8f, 0x8e, 0x8d, 0x8c, 20, 0x9000, 0
};
static unsigned int anAnimORing[8] = { /* 0x004661e0 */
    21, 22, 23, 24, 25, 26, 0x9000, 0
};
static unsigned int anAnimPipe[8] = { /* 0x00466200 */
    27, 28, 29, 30, 31, 32, 0x9000, 0
};
static unsigned int anAnimMetalSheet[14] = { /* 0x00466220 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int anAnimWing[18] = { /* 0x00466258 */
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 0x9000, 0
};
static unsigned int anAnimMine[14] = { /* 0x004662a0 */
    0, 1, 2, 0x41, 0x9000, 0, 0, 1, 2, 3, 4, 5, 0x9000, 0
};
static unsigned int anAnimAsteroidForward[16] = { /* 0x004662d8 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0x9000, 0
};
static unsigned int anAnimAsteroidShortForward[14] = { /* 0x00466318 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int anAnimAsteroidReverse[16] = { /* 0x00466350 */
    13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000, 0
};
static unsigned int anAnimAsteroidShortReverse[14] = { /* 0x00466390 */
    12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000
};
static unsigned int anAnimEjectedPilot[14] = { /* 0x004663c8 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int anAnimHyperspaceJumpFlash[22] = { /* 0x00466400 */
    0, 0x402, 0x404, 0x404, 0x408, 0x408, 0x408, 0x410,
    0x410, 0x410, 0x410, 0x820, 0x820, 0x810, 0x808, 0x804,
    1, 0x804, 0x804, 0x804, 0x804, 0xa000
};

/* Child/hardpoint offsets at 0x004682F0. */
const ShortVector aChildOffsets[56] = { /* 0x004682f0 */
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
};

const ShortPoint aWeaponDisplayPositions[32] = { /* 0x00468440 */
    {64, 28}, {8, 28}, {45, 25}, {27, 25}, {36, 23},
    {20, 32}, {27, 24}, {31, 31}, {36, 22}, {41, 31},
    {51, 32}, {45, 24}, {8, 32}, {18, 32}, {25, 11},
    {26, 32}, {36, 28}, {46, 32}, {47, 11}, {54, 32},
    {64, 32}, {12, 35}, {17, 37}, {26, 16}, {28, 31},
    {37, 41}, {37, 29}, {45, 31}, {47, 16}, {56, 37},
    {61, 35}, {0, 0}
};

const ShortPoint aWeaponDisplayOrigins[5] = { /* 0x004684c0 */
    {0, 16}, {0, 16}, {0, 16}, {0, 16}, {0, 16}
};

ObjectTypeData aObjectTypeData[OBJECT_TYPE_COUNT] = { /* 0x00466458 */
    /* 0: Hornet */
    {
        &aszObjectTypeDisplayNames[0x00],
        OBJECT_CLASS_SHIP, 100, 125, 1024, 5, 3392, 3, 5, 4000,
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
        &aszObjectTypeDisplayNames[0x08],
        OBJECT_CLASS_SHIP, 120, 135, 1024, 3, -12144, 3, 6, 6000,
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
        &aszObjectTypeDisplayNames[0x10],
        OBJECT_CLASS_SHIP, 165, 160, 1152, 6, 17856, 4, 7, 6000,
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
        &aszObjectTypeDisplayNames[0x1c],
        OBJECT_CLASS_SHIP, 180, 200, 1152, 3, -27680, 4, 8, 8000,
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
        &aszObjectTypeDisplayNames[0x24],
        OBJECT_CLASS_CAPITAL_SHIP, 240, 400, 1024, 5, 3392, 3, 70,
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
        &aszObjectTypeDisplayNames[0x2c],
        OBJECT_CLASS_CAPITAL_SHIP, 240, 400, 1024, 4, 3392, 3, 60,
        20000, 15, 10, 0, 128, 2, 2, 2, 10000,
        {
            1,
            27, 0, 0, 0, 54, 0, 0
        },
        120, 120, 80, 80, 60, 60, 0, 0
    },
    /* 6: Drayman */
    {
        &aszObjectTypeDisplayNames[0x38],
        OBJECT_CLASS_CAPITAL_SHIP, 240, 400, 1024, 4, 3392, 3, 60,
        10000, 15, 10, 0, 128, 2, 2, 2, 20000,
        {
            1,
            27, 0, 0, 0, 54, 0, 0
        },
        120, 120, 80, 80, 60, 60, 0, 0
    },
    /* 7: Exeter */
    {
        &aszObjectTypeDisplayNames[0x40],
        OBJECT_CLASS_CAPITAL_SHIP, 500, 5000, 2048, 2, 3392, 3, 200,
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
        &aszObjectTypeDisplayNames[0x48],
        OBJECT_CLASS_CAPITAL_SHIP, 700, 10000, 4096, 1, 3392, 3,
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
        &aszObjectTypeDisplayNames[0x58],
        OBJECT_CLASS_SHIP, 120, 120, 1024, 10, 3392, 3, 5, 4000,
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
        &aszObjectTypeDisplayNames[0x60],
        OBJECT_CLASS_SHIP, 160, 140, 1024, 6, 3392, 3, 7, 6000,
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
        &aszObjectTypeDisplayNames[0x68],
        OBJECT_CLASS_SHIP, 140, 126, 1024, 5, 3392, 3, 6, 6000,
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
        &aszObjectTypeDisplayNames[0x70],
        OBJECT_CLASS_SHIP, 140, 126, 1024, 4, 3392, 3, 7, 7000,
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
        &aszObjectTypeDisplayNames[0x78],
        OBJECT_CLASS_SHIP, 160, 180, 1024, 7, 3392, 3, 7, 8000,
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
        &aszObjectTypeDisplayNames[0x80],
        OBJECT_CLASS_CAPITAL_SHIP, 200, 200, 1536, 4, 3392, 3, 45,
        12000, 15, 10, 0, 460, 4, 4, 4, 4000,
        { 0 },
        70, 70, 80, 80, 60, 60, 0, 0
    },
    /* 15: Dorkir */
    {
        &aszObjectTypeDisplayNames[0x88],
        OBJECT_CLASS_CAPITAL_SHIP, 260, 400, 2048, 5, 3392, 3, 35,
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
        &aszObjectTypeDisplayNames[0x90],
        OBJECT_CLASS_CAPITAL_SHIP, 260, 400, 2048, 5, 3392, 3, 35,
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
        &aszObjectTypeDisplayNames[0x98],
        OBJECT_CLASS_CAPITAL_SHIP, 325, 3000, 4096, 3, 3392, 3, 90,
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
        &aszObjectTypeDisplayNames[0xa0],
        OBJECT_CLASS_CAPITAL_SHIP, 450, 10000, 4096, 2, 3392, 3, 110,
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
        &aszObjectTypeDisplayNames[0xa8],
        OBJECT_CLASS_CAPITAL_SHIP, 600, 10000, 2048, 1, 3392, 3, 320,
        30000, 15, 10, 0, 204, 1, 1, 1, 10000,
        { 0 },
        70, 70, 80, 80, 60, 60, 0, 0
    },
    /* 20: Sivar */
    {
        &aszObjectTypeDisplayNames[0xb0],
        OBJECT_CLASS_CAPITAL_SHIP, 400, 12000, 4096, 1, 3392, 3, 200,
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
        &aszObjectTypeDisplayNames[0xb8],
        OBJECT_CLASS_CAPITAL_SHIP, 400, 20000, 2048, 4, 3392, 3, 120,
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
        &aszObjectTypeDisplayNames[0xc4],
        OBJECT_CLASS_PROJECTILE, 10, 0, 512, 7, 30, 0, 25, 0,
        160, 0, 0, 0, 0, 0, 0, 0, { 0 },
        0, 0, 0, 0, 0, 0, 0, 0
    },
    { &aszObjectTypeDisplayNames[0xd4],
      OBJECT_CLASS_PROJECTILE, 10, 1, 832, 14, 20, 0, 40, 0,
      140, 0 }, /* 25: neutron particle gun */
    { &aszObjectTypeDisplayNames[0xe0],
      OBJECT_CLASS_PROJECTILE, 10, 0, 512, 9, 25, 0, 30, 0,
      120, 0 }, /* 26: mass driver cannon */
    { &aszObjectTypeDisplayNames[0xec],
      OBJECT_CLASS_PROJECTILE, 15, 0, 832, 15, 40, 0, 50, 1000,
      150, 0 }, /* 27: turret bolt; resources alias laser */
    { &aszObjectTypeDisplayNames[0xf0],
      OBJECT_CLASS_MISSILE, 20, 5, 768, 500, 120, 0, 4, 14500,
      130, 0, 0, 1433, 15, 15, 15, 100 }, /* 28 */
    { &aszObjectTypeDisplayNames[0xf8],
      OBJECT_CLASS_MISSILE, 20, 5, 768, 400, 140, 0, 4, 13500,
      110, 0, 0, 1689, 11, 11, 11, 100 }, /* 29 */
    { &aszObjectTypeDisplayNames[0x104],
      OBJECT_CLASS_MISSILE, 20, 5, 768, 400, 160, 0, 4, 10500,
      90, 0, 0, 1689, 11, 11, 11, 100 }, /* 30 */
    { &aszObjectTypeDisplayNames[0x110],
      OBJECT_CLASS_MISSILE, 20, 5, 768, 400, 110, 0, 4, 11500,
      110, 0, 0, 1689, 11, 11, 11, 100 }, /* 31 */
    { &aszObjectTypeDisplayNames[0x11c],
      OBJECT_CLASS_MISSILE, 25, 10, 768, 400, 200, 0, 4, 30000,
      50, 0, 0, 1280, 10, 10, 10, 100 }, /* 32 */
    { &aszObjectTypeDisplayNames[0x120],
      OBJECT_CLASS_MINE, 20, 5, 768, 110, 120, 0, 4, 10000,
      20, 20, (unsigned char *)anAnimMine,
      0, 0, 2, 2 },
    { &aszObjectTypeDisplayNames[0x12c],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidForward,
      0, 0, 1, 13 },
    { &aszObjectTypeDisplayNames[0x130],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidShortForward,
      0, 0, 1, 12 },
    { &aszObjectTypeDisplayNames[0x134],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidReverse,
      0, 0, 1, 13 },
    { &aszObjectTypeDisplayNames[0x138],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidShortReverse,
      0, 0, 1, 12 },
    { &aszObjectTypeDisplayNames[0x13c],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidForward,
      0, 0, 2, 13 },
    { &aszObjectTypeDisplayNames[0x140],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidShortForward,
      0, 0, 2, 12 },
    { &aszObjectTypeDisplayNames[0x144],
      OBJECT_CLASS_DEBRIS, 10, 4, 192, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)anAnimAsteroidForward,
      0, 0, 2, 13 },
    { &aszObjectTypeDisplayNames[0x148],
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimGirder,
      0, 0, 2, 5 },
    { &aszObjectTypeDisplayNames[0x14c],
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimTubing,
      0, 0, 2, 5 },
    { &aszObjectTypeDisplayNames[0x150],
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimMetalSheet,
      0, 0, 1, 11 },
    { &aszObjectTypeDisplayNames[0x154],
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimWing,
      0, 0, 1, 15 },
    { &aszObjectTypeDisplayNames[0x158],
      OBJECT_CLASS_DEBRIS, 20, 2, 768, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimGlass,
      0, 0, 1, 15 },
    { &aszObjectTypeDisplayNames[0x15c],
      OBJECT_CLASS_DEBRIS, 2, 1, 1792, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimORing,
      0, 0, 1, 5 },
    { &aszObjectTypeDisplayNames[0x160],
      OBJECT_CLASS_DEBRIS, 6, 1, 1536, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimPipe,
      0, 0, 1, 5 },
    { &aszObjectTypeDisplayNames[0x164],
      OBJECT_CLASS_EXPLOSION, 0, 0, 768, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)anAnimExplosion0,
      0, 0, 1, 0 },
    { &aszObjectTypeDisplayNames[0x168],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)anAnimExplosion1,
      0, 0, 1, 0 },
    { &aszObjectTypeDisplayNames[0x16c],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)anAnimExplosion2,
      0, 0, 1, 0 },
    { &aszObjectTypeDisplayNames[0x170],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimLaserSpark,
      0, 0, 1, 0 },
    { &aszObjectTypeDisplayNames[0x174],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimRedSpark,
      0, 0, 2, 3 },
    { &aszObjectTypeDisplayNames[0x178],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimBlueSpark,
      0, 0, 2, 3 },
    { &aszObjectTypeDisplayNames[0x17c],
      OBJECT_CLASS_EXPLOSION, 1, 1, 256, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimSparkTrail,
      0, 0, 2, 3 },
    { &aszObjectTypeDisplayNames[0x180],
      OBJECT_CLASS_FIXED_OBJECT, 0, 0, 256, 0, 0, 0, 0 },
    { &aszObjectTypeDisplayNames[0x184],
      OBJECT_CLASS_DEBRIS, 6, 1, 512, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimEjectedPilot,
      0, 0, 1, 12 },
    { &aszObjectTypeDisplayNames[0x188],
      OBJECT_CLASS_EXPLOSION, 0, 0, 1024, 0, 0, 0, 0, 0,
      0, 0, (unsigned char *)anAnimHyperspaceJumpFlash,
      0, 0, 1, 0 }
};
unsigned char bMessageSpeed = 2; /* 0x0046af68 */
unsigned char abManeuverRerollChance[47] = { /* 0x00465678 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
};
void (*apShipAiManeuverHandlers[47])(short, short) = { /* 0x004656a8 */
    /* 00 */ (void (*)(short, short))Mnone,
    /* 01 */ (void (*)(short, short))Mnone,
    /* 02 */ (void (*)(short, short))Mveer_away,
    /* 03 */ (void (*)(short, short))Mreset,
    /* 04 */ (void (*)(short, short))Mfull_ahead,
    /* 05 */ (void (*)(short, short))Mthink,
    /* 06 */ (void (*)(short, short))Mram_missile,
    /* 07 */ (void (*)(short, short))Mturn_n_kick,
    /* 08 */ (void (*)(short, short))Mtight_loop,
    /* 09 */ (void (*)(short, short))Mhard_break,
    /* 10 */ (void (*)(short, short))Msit_n_spin,
    /* 11 */ (void (*)(short, short))Mturn_n_spin,
    /* 12 */ (void (*)(short, short))Mburnout,
    /* 13 */ (void (*)(short, short))Mwabble,
    /* 14 */ (void (*)(short, short))Mroll_over,
    /* 15 */ (void (*)(short, short))Mhard_turn,
    /* 16 */ (void (*)(short, short))Mfish_hook,
    /* 17 */ (void (*)(short, short))Msplit_left,
    /* 18 */ (void (*)(short, short))Msit_n_fire,
    /* 19 */ (void (*)(short, short))Mkickit,
    /* 20 */ (void (*)(short, short))Mturn_n_kick,
    /* 21 */ (void (*)(short, short))Mrout_me,
    /* 22 */ (void (*)(short, short))Mdrop_a_mine,
    /* 23 */ (void (*)(short, short))Msplit_right,
    /* 24 */ (void (*)(short, short))Mzig_zag,
    /* 25 */ (void (*)(short, short))Mgloat,
    /* 26 */ (void (*)(short, short))Mtail_fire,
    /* 27 */ (void (*)(short, short))Mtarget_laser,
    /* 28 */ (void (*)(short, short))Mtarget_missile,
    /* 29 */ (void (*)(short, short))Mstrafe_enemy,
    /* 30 */ (void (*)(short, short))Mbest_strafe,
    /* 31 */ (void (*)(short, short))Mstrafe_n_roll,
    /* 32 */ (void (*)(short, short))Mkill_missile,
    /* 33 */ (void (*)(short, short))Msuicide_run,
    /* 34 */ (void (*)(short, short))Mzig_zag_pitch,
    /* 35 */ (void (*)(short, short))ShipAiState35,
    /* 36 */ (void (*)(short, short))Mbest_strafe,
    /* 37 */ (void (*)(short, short))Mget_distance,
    /* 38 */ (void (*)(short, short))Mcorkscrew,
    /* 39 */ (void (*)(short, short))Mtry2tail,
    /* 40 */ (void (*)(short, short))Mzip_past,
    /* 41 */ (void (*)(short, short))Mbuzz_debris,
    /* 42 */ (void (*)(short, short))Mline_up_drop,
    /* 43 */ (void (*)(short, short))Mchill,
    /* 44 */ (void (*)(short, short))ShipAiState44,
    /* 45 */ (void (*)(short, short))Mbest_strafe,
    /* 46 */ (void (*)(short, short))Mreset
};

const char *apszKilrathiAceNames[4] = { /* 0x0046af80 */
    &aszKilrathiAceNames[0x00],
    &aszKilrathiAceNames[0x08],
    &aszKilrathiAceNames[0x10],
    &aszKilrathiAceNames[0x18]
};

const char *apszCommMenuText[13] = { /* 0x0046af90 */
    &aszCommMenuText[0x00],
    &aszCommMenuText[0x10],
    &aszCommMenuText[0x24],
    &aszCommMenuText[0x38],
    &aszCommMenuText[0x48],
    &aszCommMenuText[0x58],
    &aszCommMenuText[0x64],
    &aszCommMenuText[0x78],
    &aszCommMenuText[0x8c],
    &aszCommMenuText[0x9c],
    &aszCommMenuText[0xb0],
    &aszCommMenuText[0xc4],
    &aszCommMenuText[0xd8]
};

/* Ordered display names transcribed from the image.  The Sega CD inspector
 * proves that these index ObjectType, not a ship-only class enum. */
const char aszObjectTypeDisplayNames[0x18c] = /* 0x004684d4 */
    "Hornet\0\0"
    "Rapier\0\0"
    "Scimitar\0\0\0\0"
    "Raptor\0\0"
    "Venture\0"
    "Dilligent\0\0\0"
    "Drayman\0"
    "Exeter\0\0"
    "Tiger's Claw\0\0\0\0"
    "Salthi\0\0"
    "Dralthi\0"
    "Krant\0\0\0"
    "Gratha\0\0"
    "Jalthi\0\0"
    "Spikeri\0"
    "Dorkir\0\0"
    "Lumbari\0"
    "Ralari\0\0"
    "Fralthi\0"
    "Snakeir\0"
    "Sivar\0\0\0"
    "Star post\0\0\0"
    "Laser cannon\0\0\0\0"
    "Neutron gun\0"
    "Mass driver\0\0\0\0\0"
    "Dart DF\0"
    "Javelin HS\0\0"
    "Pilum FF\0\0\0\0"
    "Spiculum IR\0\0\0\0\0"
    "Porcupine\0\0\0";

const char aszKilrathiAceNames[32] = /* 0x0046afd4 */
    "Bhurak\0\0"
    "Dakhath\0"
    "Khajja\0\0"
    "Bakhtosh";

const char aszCommMenuText[0xe8] = /* 0x0046aff8 */
    "Never mind...\0\0\0"
    "Attack my target!\0\0\0"
    "Help me out here\0\0\0\0"
    "Return to base.\0"
    "Die furball!\0\0\0\0"
    "Slag off!\0\0\0"
    "Bite it cat face.\0\0\0"
    "Break and attack.\0\0\0"
    "Keep formation!\0"
    "Form on my wing.\0\0\0\0"
    "Keep radio silence\0\0"
    "Broadcast freely\0\0\0\0"
    "Request Landing";

const char szIonDrive[12] = "Ion drive"; /* 0x0046a7c4 */
const char szPowerPlant[12] = "Power plant"; /* 0x0046a7d0 */
const char szShieldGenerator[16] = "Shield gen'r"; /* 0x0046a7dc */
const char szComputerSystem[16] = "Computer sys"; /* 0x0046a7ec */
const char szIntercomUnit[16] = "InterCom unit"; /* 0x0046a7fc */
const char szTargetTracking[16] = "Target track"; /* 0x0046a80c */
const char szAccelerationAbsorbers[16] = "Accel absorbers"; /* 0x0046a81c */
const char szEjectorSystem[16] = "Ejector system"; /* 0x0046a82c */
const char szRepairSystems[16] = "Repair systems"; /* 0x0046a83c */
const char szDamageOk[4] = "Ok"; /* 0x0046a84c */
const char szDamageLight[8] = "Light"; /* 0x0046a850 */
const char szDamageModerate[12] = "Moderate"; /* 0x0046a858 */
const char szDamageHeavy[8] = "Heavy"; /* 0x0046a864 */
const char szDamageDestroyed[12] = "Destroyed"; /* 0x0046a86c */
const char szDamageReport[16] = "DAMAGE REPORT"; /* 0x0046a878 */
const char szNoInternalDamage[20] = /* 0x0046a888 */
    "NO INTERNAL\n\nDAMAGE";
const char szDamagedUnitCountFormat[20] = /* 0x0046a89c */
    "%d Unit%c Damaged";
const char szDamageStatusFormat[16] = "%s\nDamage: %s"; /* 0x0046a8b0 */
const char szDamageStatusFormatHighRes[16] = /* 0x0046a8c0 */
    "%s\nDamage: %s";
