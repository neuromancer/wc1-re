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
const int g_anRLEQuarterCosine_0043d4bf[901] = {
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

TitleMenuRegion g_aBarracksMenuRegions_00463008[21] = {
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

short g_nHazardReferenceSpeed_00465040;
short g_nActiveHazards_00465044;
int DAT_00465058 = 1;
unsigned int DAT_0046505c;
unsigned int g_nGuardedAllocationTotalBytes_00465060;
unsigned int g_nGuardedAllocationBytes_00465064;
unsigned int g_nGuardedAllocationPeakBytes_00465068;
int DAT_0046506c;
int DAT_00465070;
int DAT_00465074 = 1;
int DAT_0046507c = 1;
int DAT_00465080;
int DAT_004650a8;
unsigned int DAT_004650ac;
GuardedAllocation *g_pGuardedAllocationHead_004650b0;
const char g_szJoystickDevCapsFailure_004652dc[26] =
    "Joystick: !joyGetDevCaps\n";
short g_nPacketError_00465460;
short g_nAutopilotFormationShipCount_00465544;
const short g_asEjectionPrimaryFrames_00465550[8] = {
    0, 1, 1, 3, 3, 0, 0, 0
};
const short g_asEjectionSecondaryFrames_00465560[8] = {
    -1, -1, 2, -1, 4, 0, 0, 0
};
const short g_asEjectionViewScript_00465570[12] = {
    3, 11, 14, 70, 3, 10, 14, 80, 3, 4, -1, 0
};
const char *g_pStrandedMessage_00465588 =
    g_szStrandedMessage_0046558c;
const char g_szStrandedMessage_0046558c[72] =
    "\nWith your carrier\ndestroyed, you drift\nendlessly through\n"
    "the void...";
const char g_szViewTemplates_004655d4[16] = "View Templates";
const char g_szStrandedTheEnd_004655e4[12] = "THE END";
const short g_asMouthFramesByPhoneme_004655f0[26] = {
    0, 5, 4, 4, 1, 8, 4, 7, 0, 4, 4, 7, 5,
    4, 2, 5, 6, 4, 4, 4, 3, 4, 6, 4, 4, 4
};
const char g_szConversationIntegerFormat_00465628[4] = "%d";
const char g_szConversationIntegerFormatAlt_0046562c[4] = "%d";
const char g_szCampaignDateFormat_00465630[12] = "%03d.%03d";
const char g_szSavedCampaignDateFormat_0046563c[12] = "%03d.%03d";
const char g_szCampaignTimeFormat_00465648[12] = "%02d:%02d";
const char g_szConversationTextFormat_00465654[12] = "%X%Y%F%s%P";
const char g_szBriefingMapTextFormat_00465660[12] = "%X%Y%F%s%P";
const char g_szCloseLookTextFormat_0046566c[12] = "%X%Y%F%s%P";
signed char g_cScrambleLeftWalkerFrame_00465768 = 7;
signed char g_cScrambleRightWalkerFrame_0046576c = 10;
const signed char g_acScrambleWalkerOverlayFrames_00465770[8] = {
    3, 4, 5, 3, 4, 6, 6, 3
};
short g_nScrambleLeftWalkerX_00465778 = 70;
short g_nScrambleRightWalkerX_0046577c = 170;
short g_nScrambleBackgroundX_00465780;
const unsigned short g_ausScrambleActorAnimationA_00465788[8] = {
    0, 1, 2, 3, 4, 5, 0x80, 0
};
const unsigned short g_ausScrambleActorAnimationB_00465798[11] = {
    0, 0, 1, 2, 2, 2, 2, 2, 1, 1, 0x80
};
ScrambleAnimationActor g_aScrambleAnimationActors_004657b0[5] = {
    { 0, 0, 0, 0,  2,  0, 0,
      g_ausScrambleActorAnimationA_00465788, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 10,  0, 0,
      g_ausScrambleActorAnimationB_00465798, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 13,  0, 0,
      g_ausScrambleActorAnimationB_00465798, 0, 0, 0, 0 },
    { 0, 0, 0, 0,  0, -1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0,  1, -1, 0, 0, 0, 0, 0, 0 }
};
const ScrambleShipDetail g_aaScrambleShipDetails_00465828[4][32] = {
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
const ShortPoint g_aFuneralParticleOrigins_00465b18[7] = {
    { 234, 83 }, { 248, 85 }, { 260, 80 }, { 273, 78 },
    { 286, 75 }, { 299, 76 }, { 310, 74 }
};
const short g_asFuneralSceneBySeries_00465b36[15] = {
    0, 0, 1, 1, 1, 1, 1, 2, 3, 2, 3, 3, 2, 3, 0
};
int g_bFuneralShowTheEnd_00465b54;
const char g_szTheEnd_00465c04[8] = "THE END";
const char g_szFuneralTextFormat_00465c0c[12] = "%X%Y%F%s%P";
const char g_szFuneralCompanyCommand_00465c18[20] =
    "%X%Y%FCompany...%P";
const char g_szFuneralAttentionCommand_00465c2c[20] =
    "%X%YAtten-SHUN!%P";
const char g_szFuneralPrepareArmsCommand_00465c40[20] =
    "%X%YPrepare arms!%P";
const char g_szFuneralFireCommand_00465c54[16] =
    "%X%Y%FFire!%P";
short g_nEnemySighting_00465c7c = 0x7fff;
short g_nDebriefingPersonality_00465c80;
int DAT_00465c84;
int g_aiPacketReferenceTable_00465c88[4][0x25];
const ShortVector g_aaFormationPositions_00465ed8[5][8] = {
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
const int g_anPilotTurnInterval_00465fc8[16] = {
    5, 5, 4, 4, 3, 3, 3, 2, 2, 1, 3, 3, 3, 3, 2, 2
};
const char g_szErrorLoadingPilotSpeech_00466010[32] =
    "Error loading pilot speech";
unsigned char *g_pIntroSceneResource_00467b84;
unsigned char *g_pIntroSceneResourceMirror_00467c0b;
unsigned char *g_pIntroBackgroundResource_00467eae;
short g_nNavMapCoordinateScaling_00468660;
short g_nNavMapScale_00468664 = 1;
const NavMapObjectiveStyle g_aNavMapObjectiveStyles_00468668[10] = {
    { 1, 2, &DAT_004699b4, &g_cDefaultTextColour_004699cc,
      &g_cDefaultTextColour_004699cc },
    { 3, 2, &DAT_0046999c, &g_cViewportClearColour_004699a0,
      &g_cDefaultTextColour_004699cc },
    { 4, 2, &DAT_004699c8, &DAT_004699c8,
      &g_cDefaultTextColour_004699cc },
    { 2, 3, &DAT_004699c8, &g_cDefaultTextColour_004699cc,
      &g_cDefaultTextColour_004699cc },
    { 2, 3, &DAT_004699ac, &DAT_004699ac,
      &g_cDefaultTextColour_004699cc },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 }
};
unsigned char *g_pNavMapShape_00468708;
unsigned short g_nNavMapLabelCount_0046870c;
short g_nNavMapReservedAreaCount_00468710;
const char g_szCampaignSector_00468718[16] = "Vega XR-231.3";
const char *g_apszShipMissionTypeNames_00468728[12] = {
    g_szMissionPatrol_00468758,
    g_szMissionEscort_00468760,
    g_szMissionStrike_00468768,
    g_szMissionDefend_00468770,
    g_szMissionWingman_00468778,
    g_szMissionFlee_00468780,
    g_szMissionGotoWarp_00468788,
    g_szMissionWarpArriveError_00468794,
    g_szMissionCannedSequenceError_00468798,
    g_szMissionRendezvous_0046879c,
    g_szMissionComeHomeError_004687a8,
    0
};
const char g_szMissionPatrol_00468758[8] = "Patrol";
const char g_szMissionEscort_00468760[8] = "Escort";
const char g_szMissionStrike_00468768[8] = "Strike";
const char g_szMissionDefend_00468770[8] = "Defend";
const char g_szMissionWingman_00468778[8] = "Wingman";
const char g_szMissionFlee_00468780[8] = "Flee";
const char g_szMissionGotoWarp_00468788[12] = "Goto Warp";
const char g_szMissionWarpArriveError_00468794[4] = "err";
const char g_szMissionCannedSequenceError_00468798[4] = "err";
const char g_szMissionRendezvous_0046879c[12] = "Rendezvous";
const char g_szMissionComeHomeError_004687a8[4] = "err";
const char g_szNavLabelTextFormat_004687ac[12] = "%X%Y%F%s";
const char g_szNavAsteroids_004687b8[12] = "Asteroids";
const char g_szNavMines_004687c4[8] = "Mines";
const char g_szInflightTimeFieldFormat_004687cc[8] = "%02d";
const char g_szStandardTimeFormat_004687d4[24] =
    "%X%YStandard time %s";
const char g_szStandardTimeBlank_004687ec[4] = " ";
const char g_szInflightTimeSuffixFormat_004687f0[8] = "%02d  ";
const char g_szStandardTimeColon_004687f8[4] = ":";
const char g_szNavLegendNewline_004687fc[4] = "\n";
const char g_szNavMissionFlightPath_00468800[20] = "MISSION FLIGHT PATH";
const char g_szNavHomeBase_00468814[12] = "HOME BASE";
const char g_szNavBlankLine_00468820[4] = "\n";
const char g_szNavTitleFormat_00468824[8] = "%s\n\n";
const char g_szNavSectorFormat_0046882c[12] = "Sector: %s\n";
const char g_szNavSystemFormat_00468838[16] = "System: %s\n\n";
const char g_szNavMissionFormat_00468848[8] = "* %s *\n";
const char g_szNavShipFormat_00468850[8] = "* %s *\n";
const char g_szNavNotesHeading_00468858[8] = "\nNotes\n";
const char g_szNavNoteFormat_00468860[4] = "%s\n";
const char g_szNavLocationFormat_00468864[48] =
    "%X%Y                         Location: %d.%d.%d";
const char g_szNavViewportName_00468894[8] = "VSCREEN";
const char g_szBriefingNavMapTitle_0046889c[20] = "Briefing Nav Map";
const char g_szConfedNavScan_004688b0[16] = "ConFed Nav Scan";
int g_bInflightComputerActive_00468754;
int DAT_004688cc;
int DAT_004688d0;
int DAT_004688d4;
int DAT_004688d8;
short DAT_004688dc;
int DAT_004688e0;
short DAT_004688e4 = -1;
short DAT_004688e8 = -1;
unsigned short DAT_004688ec = 1;
int DAT_004688f0;
const char *g_pszIntroOpeningText_00468910 =
    "In the distant future,\n"
    "mankind is locked in a deadly war...";
int g_nIntroCreditCount_00468a30 = 11;
const char *g_apszIntroCredits_00468a38[20] = {
    "Design\nby\nChris Roberts",
    "Software Engineers\nChris Roberts\nKen Demarest III\nPaul C. Isaac\nSteve Muchow\nHerman Miller\nSteve Beeman\n",
    "Dogfight Intelligence\nKen Demarest III\n\nDogfight Choreography\nSteve Beeman\nErin Roberts",
    "3Space System\nby\nChris Roberts\n\nOriginFX Graphic System\nChris Roberts\nJohn Miles",
    "OriginFX Sound System\nby\nHerman Miller",
    "Artwork\nDenis Loubet\nGlen Johnson\nDaniel Bourbonnais\nKeith Berdak\nJohn Watson",
    "Screenplay by Jeff George\n\nAdditional Writing\nSteve Cantrell\nPhilip Brogden",
    "Soundtrack by\nGeorge A. Sanger and Dave Govett",
    "Sound Effects by Marc Schaefgen",
    "Produced by\nChris Roberts and Warren Spector",
    "Directed by\nChris Roberts",
    "Windows 95 Team",
    "Combat Programmers\n\nJeff Mangler Everett\nJeff jefftep Grills\nChuck Bishop Karpiak\nKris Goblin Pelley",
    "Sound System\n\nRichard Cupcake Lyle",
    "Soundtrack Rescored by\n\nI Need Names",
    "Head Whiner\n\nAnthony Sommers",
    "Whiners\n\nMonte Mathis\nHal Milton\nDieter Martin",
    "Richard Zinser\nKanon Lillemon\n",
    "Special Thanks To\n\nSocks\nand\nCaffeine",
    0
};
const char g_szCalculating_0046931c[12] = "CALCULATING";
const char g_szRangeKilometresSuffix_00469328[4] = " km";
const char g_szNoObjective_0046932c[8] = "NONE";
const char g_szUnknownObjective_00469334[8] = "UNKNOWN";
const char g_szCompNavigation_0046933c[16] = "COMP NAVIGATION";
const char g_szDestinationFormat_0046934c[20] =
    "\n\nDESTINATION\n  %s";
const char g_szNavigationRangeLabel_00469360[12] = "\n\nRANGE\n  ";
const char g_szNewObjectivePrompt_0046936c[20] =
    "\n\n(N)ew Objective";

TitleMenuRegion g_aTitleMenuRegions_00468a88[5] = {
    { 1, 49, 48, 283, 99 },
    { 1, 49, 91, 283, 149 },
    { 1, 49, 134, 283, 149 },
    { 1, 49, 177, 283, 209 },
    { -1, 0, 0, 0, 0 }
};

PacketResourceDescriptor g_aIntroResourceDescriptors_00468ac0[3] = {
    { &g_pIntroBackgroundResource_00467eae, 3, 2 },
    { &g_pIntroSceneResource_00467b84, 3, 5 },
    { 0, 0, 0 }
};

int g_bTitleMenuSceneInitialized_00468ad8;
int g_nCapitalShipViewDistance_00468ff4 = 0x7d000;
unsigned char g_bLandingAuthorized_00468ff8;
int g_bAlternateChaseView_00468ffc;
const short g_asVduSelectionSound_00469000[2] = { 0x7f, 0 };
char *DAT_00469004;
char *DAT_00469008;
unsigned char DAT_0046900c = 0xff;
const short g_asPilotHandOffsets_00469018[34] = {
    6, -3, 7, 2, 7, 9, 7, 12, 8, 13, 0, -1, -1, -1,
    -4, -1, -6, -1, 6, 0, 8, 0, 10, 0, 13, 3, 8, -7,
    6, -9, 5, -11, 5, -14
};
unsigned char *g_pCockpitPilotShape_0046905c;
unsigned char *g_pCockpitExplosionBackground_00469060;
unsigned char * volatile g_pCockpitExplosionShape_00469064;
volatile short g_nCockpitExplosionFrame_00469068 = 8;
unsigned char *g_pReleaseWeaponDisplayBackground_0046906c;
signed char g_cReleaseWeaponDisplayFrame_00469070 = -1;
signed char g_cReleaseWeaponDisplayTicks_00469074;
signed char g_cReleaseWeaponDisplayState_00469078;
char *g_pszObjectiveStatusMessage_0046908c =
    g_szObjectiveStatusMessage_0046928c;
short g_nScannerTargetObject_00469090 = -1;
int g_nDisplayedObjectiveRange_00469088 = 40000;
const int g_aiScannerGridRows_00469098[79] = {
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
unsigned char g_abTargetBracketPadding_004691d9[7];
short DAT_004691e0[10] = {
    18, 14, 71, 5, 80, 29, 49, 27, 14, 13
};
short g_nTargetLockMarkerX_004691f4 = -0x7fff;
ShortRect g_stTargetBracketBounds_004691f8 = {-0x7fff, 0, 0, 0};
ShortRect g_stPreviousTargetBracketBounds_00469200 = {-0x7fff, 0, 0, 0};
short DAT_00469208 = -1;
Viewport g_stTrainSimVduSource_00469210;
const ShortPoint g_aaCockpitDamagePositions_00469228[5][4] = {
    {{224, 5}, {132, 96}, {233, 107}, {149, 161}},
    {{177, 6}, {153, 142}, {103, 140}, {55, 183}},
    {{107, 25}, {211, 32}, {21, 178}, {300, 178}},
    {{74, 10}, {294, 19}, {197, 105}, {105, 134}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};
unsigned char *g_pConfedCommBackground_00469278;
unsigned char *g_pCommStaticShape_0046927c;
unsigned char *g_pKilrathiCommBackground_00469280;
int g_nCommPortraitFrame_00469284 = -1;
int g_bForceDamageDisplayRedraw_00469288;
char g_szObjectiveStatusMessage_0046928c[28] =
    "Wait for ??????????????????";
const char g_szMissileLocked_004692a8[16] = "MISSILE LOCKED ";
const char g_szAlreadyNear_004692b8[16] = "Already Near";
const char g_szEnemyNear_004692c8[12] = "Enemy Near";
const char g_szHazardNear_004692d4[12] = "Hazard Near";
const char g_szComponentHitFormat_004692e0[8] = "%s HIT";
const char g_szAlreadyVisited_00469380[16] = "Already Visited";
const char g_szObjectiveReached_00469390[20] = "Objective Reached";
const char g_szWaitForFormat_004693a4[12] = "Wait for %s";
unsigned char DAT_004693b0;
const char *g_pGameVersion_004693b4 = g_szGameVersion_004693b8;
const char g_szGameVersion_004693b8[9] = "1.03F-95";
const ShortPoint g_aBarracksBunkOrigins_004693c8[8] = {
    {109, 86}, {170, 86}, {98, 95}, {173, 95},
    {78, 110}, {176, 110}, {42, 136}, {183, 136}
};
const char *g_apszSaveCampaignMenuLabels_004693e8[2] = {
    g_szSaveCampaignMenuLabel_00469468,
    g_szSaveCampaignMenuLabelAlt_00469480
};
char *g_apszBarracksMenuLabels_004693f0[20] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    (char *)g_szMissionHangarMenuLabel_00469498,
    (char *)g_szReturnToBarMenuLabel_004694a8,
    (char *)g_szQuitGameMenuLabel_004694bc,
    (char *)g_szViewMedalsMenuLabel_004694d0
};
unsigned int g_dwModalBoundsTopLeft_00469440 = 0x00280018;
unsigned int g_dwModalBoundsBottomRight_00469444 = 0x003c0128;
ModalTextPanel *g_pModalTextPanel_00469448;
const short g_asCampaignPilotFiles_00469450[3] = {58, 61, 74};
const short g_asCampaignBriefingFiles_00469458[3] = {10, 62, 73};
const short g_asMissionDataFiles_00469460[3] = {15, 52, 72};
const char g_szSaveCampaignMenuLabel_00469468[24] =
    "Save this campaign  ";
const char g_szSaveCampaignMenuLabelAlt_00469480[24] =
    "Save this campaign  ";
const char g_szMissionHangarMenuLabel_00469498[16] = "Mission Hangar";
const char g_szReturnToBarMenuLabel_004694a8[20] = "Return to the Bar";
const char g_szQuitGameMenuLabel_004694bc[20] = "Quit Wing Commander";
const char g_szViewMedalsMenuLabel_004694d0[20] = "View your medals";
short g_nPaletteTransitionInitialise_00469640 = 1;
int g_nDebugOverlayConsoleCount_00469644;
DWORD g_dwDebugOverlayKey_00469648;
DWORD g_dwDebugOverlayKeyLatch_0046964c;
HHOOK g_hDebugKeyboardHook_00469650;
const char g_szDebugOverlayFontName_00469654[8] = "Courier";
const char g_szDebugOverlaySpinner_0046965c[5] = "-\\|/";
const char g_szDebugOverlayNewline_00469664[4] = "\n";
char g_szDiskMarkerFile_00469688[9] = "DISK.000";
short g_nDiskPromptBorderColour_00469694 = 0x50;
short g_nKeyboardPointerStep_004696a4 = 4;
const short g_asPlayerDamageSystemTable_00469878[50] = {
    0, 8, 6, 5, 0, 3, 5, 5, 7, 6,
    0, 8, 6, 5, 4, 3, 4, 0, 4, 4,
    1, 2, 5, 2, 7, 3, 4, 7, 5, 1,
    1, 4, 1, 5, 2, 3, 4, 7, 2, 1,
    4, 4, 4, 4, 0, 8, 6, 5, 4, 0
};
const enum ObjectType g_aeShipHitDebrisTypes_00469950[3] = {
    OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK,
    OBJECT_TYPE_DEBRIS_SHIP_TUBING,
    OBJECT_TYPE_DEBRIS_O_RING
};
const signed char g_acGunRefireDelay_0046995c[4] = {6, 10, 4, 0};
const char g_szWeaponDestroyed_00469960[20] = "Weapon destroyed";
const char g_szFuelTanksHit_00469974[16] = "Fuel tanks hit";
const char g_szComponentFixedFormat_00469984[8] = "%s FIXD";
const char g_szNeedLock_0046998c[12] = "Need Lock";
short g_bSlowSceneAnimation_00469998;
unsigned char DAT_0046999c;
unsigned char g_cViewportClearColour_004699a0 = 15;
unsigned char DAT_004699a4 = 0x25;
unsigned char DAT_004699a8 = 0x47;
unsigned char DAT_004699ac = 0x50;
unsigned char DAT_004699b0 = 0xaa;
unsigned char DAT_004699b4 = 0xa6;
unsigned char DAT_004699b8 = 0x85;
unsigned char DAT_004699bc = 0x27;
unsigned char DAT_004699c0 = 7;
unsigned char DAT_004699c4 = 0x0b;
unsigned char DAT_004699c8 = 0xb6;
unsigned char g_cDefaultTextColour_004699cc = 0xa8;
unsigned char DAT_004699d4 = 0xf5;
unsigned char DAT_004699d8 = 0xbf;
const short g_asConversationTextColours_004699f0[16] = {
    0x25, 0xb6, 0x9a, 0x50, 0x94, 0x85, 0x27, 0xa6,
    0xfd, 0x47, 0xaa, 0x0b, 0x09, 0x0d, 0x03, 0x04
};
int g_bGraphicsActive_00469a20;
PacketResourceDescriptor g_aCommon3SpaceResources_00469bc0[12] = {
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_THRUSTERS].shapeSet, 3, 0 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_EXPLOSION0].shapeSet, 3, 1 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_LASER_CANNON].shapeSet, 3, 6 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_MASS_DRIVER_CANNON].shapeSet,
      3, 7 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_NEUTRON_PARTICLE_GUN].shapeSet,
      3, 8 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_LASER_SPARK].shapeSet, 3, 9 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_PIPE].shapeSet, 3, 4 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_BLUE_SPARK].shapeSet, 3, 10 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_RED_SPARK].shapeSet, 3, 11 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_SPARK_TRAIL].shapeSet, 3, 12 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_SPACE_MINE].shapeSet, 3, 15 },
    { 0, 0, 0 }
};
PacketResourceDescriptor g_aMissionResourceDescriptors_00469c20[5] = {
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_HYPERSPACE_JUMP_FLASH].shapeSet,
      3, 14 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_EXPLOSION1].shapeSet, 3, 2 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_EXPLOSION2].shapeSet, 3, 3 },
    { &g_aObjectTypeData_00466458[OBJECT_TYPE_DEBRIS_METAL_SHEET].shapeSet,
      3, 5 },
    { 0, 0, 0 }
};
PacketResourceDescriptor g_aCockpitSecondaryResources_00469ce0[5] = {
    { &g_pCockpitDamageShape_005a76f4, -1, 7 },
    { &g_pCockpitWeaponShape_005a7564, -1, 9 },
    { &g_pCockpitIndicatorShape_005a7658, 8, 4 },
    { &g_pTargetLockShape_005a6bf4, 8, 0 },
    { 0, 0, 0 }
};
PacketResourceDescriptor g_aCockpitPrimaryResources_00469d08[8] = {
    { &g_pCockpitPilotShape_0046905c, -1, 4 },
    { &g_pCockpitDamageShape_005a76f4, -1, 7 },
    { &g_pCockpitWeaponShape_005a7564, -1, 9 },
    { &g_apCockpitShapes_005a7c80[0], -1, 0 },
    { &g_apCockpitShapes_005a7c80[1], -1, 1 },
    { &g_apCockpitShapes_005a7c80[2], -1, 2 },
    { &g_apCockpitShapes_005a7c80[3], -1, 3 },
    { 0, 0, 0 }
};
unsigned int g_bIntroSceneResourcesActive_00469d48 = 1;
short g_asConstellationObjectIndices_00469d50[4] = {
    -1, -1, -1, -1
};
int g_bCockpitResourcesActive_00469d58;
int DAT_00469d5c;
const short g_asSceneAnimationLogicalFiles_00469d60[8] = {
    63, 64, 65, 66, 67, 68, 69, 70
};
short g_bSceneAnimationWaitCommand_00469d70;
const char g_szSceneAnimationTextFormat_00469d74[12] = "%X%Y%F%s%P";
Viewport g_stTrainSimPanelViewport_00469da8;
/* This is four zero bytes, not a pointer table.  The original instruction at
 * 0x425E07 uses 0x469DBC as a compiler-generated biased displacement so that
 * index 9 lands on the real pointer table at 0x469DE0. */
unsigned int g_dwHighScoreNameTableBiasPadding_00469dbc;
const ShortRect g_stTrainSimPanelBounds_00469dc0 = {
    0x30, 0x1d, 0x110, 0x6d
};
const char g_szTrainSimTitle_00469dc8[24] = "SQUADRON: TRAINSIM";
unsigned char *g_apszBuiltInHighScoreNames_00469de0[6] = {
    &g_aszBuiltInHighScores_00469e38[0],
    &g_aszBuiltInHighScores_00469e38[8],
    &g_aszBuiltInHighScores_00469e38[16],
    &g_aszBuiltInHighScores_00469e38[24],
    &g_aszBuiltInHighScores_00469e38[32],
    &g_aszBuiltInHighScores_00469e38[40]
};
TitleMenuRegion g_aTrainSimMissionRegions_00469df8[5] = {
    { 1, 47, 29, 67, 49 },
    { 1, 47, 89, 67, 109 },
    { 1, 251, 29, 271, 49 },
    { 1, 251, 89, 271, 109 },
    { -1, 0, 0, 0, 0 }
};
short g_nTrainSimMissionRegionPadding_00469e2a;
int g_nTrainSimActive_00469e2c;
short g_nTrainSimMission_00469e30;
short g_nArcadeWave_00469e34;
unsigned char g_aszBuiltInHighScores_00469e38[48] =
    "BISHOP\0\0"
    "GOBLIN\0\0"
    "JEFFTEP\0"
    "MANGLER\0"
    "THE MAN\0"
    "MONGO\0\0\0";
unsigned int g_dwTrainSimStringPadding_00469e68;
const char g_szTextFlushToken_00469e6c[4] = "%P";
const char g_szNewPilotPrompt_00469e70[80] =
    "CONGRATULATIONS!\n"
    "YOU HAVE A TOP SCORE!\n"
    "PLEASE ENTER YOUR\n"
    "NAME AND CALLSIGN:\n";
const char g_szDefaultPilotName_00469ec0[8] = "Blair";
const char g_szPilotNameLabel_00469ec8[12] = "LAST NAME: ";
const char g_szDefaultCallsign_00469ed4[12] = "Maverick";
const char g_szCallsignLabel_00469ee0[12] = "CALLSIGN : ";
const char g_szCheaterCallsign_00469eec[8] = "CHEATER";
const char g_szHighScoreCongratulations_00469ef4[68] =
    "*******\n"
    "CONGRATULATIONS!\n"
    "YOU HAVE SCORE NUMBER\n"
    ">>>> %d <<<<\n"
    "*******";
const char g_szLowScoreMessage_00469f38[56] =
    "> SORRY <\n\n"
    "YOUR SCORE IS ONLY\n"
    "%ld0\n\n"
    "PLEASE PLAY AGAIN!";
const char g_szHighScoresHeading_00469f70[16] = "%JHIGH SCORES%P";
const char g_szHighScoreNumberFormat_00469f80[8] = "%0ld";
const char g_szHighScoreRowFormat_00469f88[16] =
    "%X%Y%d. %s%X%s0";
const char g_szSelectEnemy_00469f98[16] = "SELECT\nENEMY%P";
int g_nCannedSceneMode_00469fac;
int g_nArcadeState_00469fb0;
short DAT_00469fb4 = 1;
short g_nFrameSkip_00469fb8 = 1;
int g_nSfxVolumeSetting_00469fbc = 0x14;
int g_nMusicVolumeSetting_00469fc0 = 0x14;
int g_bViewportDirty_00469fc4;
int g_anVolumeLevels_00469fc8[11] = {
    0, 40000, 50000, 55000, 60000, 61000,
    61500, 62000, 63000, 63500, 64000
};
int g_nOriginDevUnlock_00469ff4;
int g_bPlayerCollisionsEnabled_00469ff8 = 1;
int DAT_00469ffc = 1;
int DAT_0046a000 = 1;
int DAT_0046a004;
int DAT_0046a008;
int g_nShowMemoryStatus_0046a00c;
short DAT_0046a010 = -1;
short g_nArcadeBonusCountdown_0046a014;
int g_bMouseCursorVisible_0046a018;
unsigned char g_bInputPollingGuard_0046a01c;
InputDeviceSample g_stLastPolledFlightInput_0046a020;
int g_bMouseAfterburnerControl_0046a02c;
const short g_asMouseYawThresholds_0046a030[8] = {
    10, 37, 52, 57, 62, 1070, 0, 0
};
const short g_asMousePitchThresholds_0046a040[8] = {
    5, 18, 27, 35, 38, 1040, 0, 0
};
unsigned int g_dwLastSecondaryButtonPress_0046a04c;
unsigned char g_bFlightRollLatch_0046a050;
unsigned char g_bAfterburnerButtonLatched_0046a054;
short g_nMouseYawInput_0046a058;
short g_nMousePitchInput_0046a05c;
const char g_szSpaceBuffer_0046a1d0[16] = "SPACE BUFFER";
const char g_szAllocateBufferTag_0046a1e0[4] = "AB";
ActiveSoundEntry *g_pActiveSoundHead_0046a438;
ActiveSoundEntry *g_pActiveSoundTail_0046a43c;
int DAT_0046a440;
WaveTableEntry *g_pWaveTableHead_0046a444;
WaveTableEntry *g_pWaveTableTail_0046a448;
const char g_szPlayWaveOpenError_0046a46c[36] =
    "playWAVE Unable to open file '%s'";
unsigned char *g_pDamageDisplayBackground_0046a748;
const ShortPoint g_aDamageDisplayPositions_0046a750[9] = {
    {36, 37}, {36, 28}, {36, 30}, {36, 23}, {36, 19},
    {36, 15}, {36, 24}, {36, 16}, {36, 22}
};
const char *g_apszComponentNames_0046a778[9] = {
    g_szIonDrive_0046a7c4,
    g_szPowerPlant_0046a7d0,
    g_szShieldGenerator_0046a7dc,
    g_szComputerSystem_0046a7ec,
    g_szIntercomUnit_0046a7fc,
    g_szTargetTracking_0046a80c,
    g_szAccelerationAbsorbers_0046a81c,
    g_szEjectorSystem_0046a82c,
    g_szRepairSystems_0046a83c
};
const unsigned char g_abDamageDisplayFrames_0046a7a0[9] = {
    21, 22, 23, 24, 25, 26, 27, 28, 29
};
const char *g_apszDamageSeverityNames_0046a7b0[5] = {
    g_szDamageOk_0046a84c,
    g_szDamageLight_0046a850,
    g_szDamageModerate_0046a858,
    g_szDamageHeavy_0046a864,
    g_szDamageDestroyed_0046a86c
};
short g_nScriptedViewObject_0046a8d0 = -1;
int g_bScriptedView_0046a8d4;
const short g_asConstellationVelocity_0046a8d8[16] = {
    8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1
};
const short g_asConstellationFrame_0046a8f8[16] = {
    0, 16, 16, 0, 4, 4, 20, 20, 24, 8, 8, 24, 28, 12, 12, 28
};
short g_nConstellationDirection_0046a918 = -1;
unsigned char *g_pPacketDecompressionWorkspace_0046a91c;
unsigned short g_wPacketDecompressionInputSizeOverride_0046a920;
unsigned short g_wPacketCompressionFormatFlags_0046a924;
const ShortRect g_aTargetArmorClipRects_0046a928[4] = {
    { 12, -20, 29, 20 },
    { -11, 1, 11, 20 },
    { -11, -20, 11, 0 },
    { -29, -20, -12, 20 }
};
char g_szEmptyTargetDisplayText_0046a948[4] = "";
const char g_szLockedTarget_0046a94c[20] = "   LOCKED TARGET";
const char g_szTextColourStringColourFormat_0046a960[8] = "%F%s%F";
const char g_szAutoTargetting_0046a968[20] = "  AUTO TARGETTING";
const char g_szTextColourStringFormat_0046a97c[8] = "%F%s";
const char g_szTargetLabel_0046a984[12] = "\nTarget:";
const char g_szNoTarget_0046a990[8] = " None";
const char g_szWingmanTargetNameFormat_0046a998[4] = " %s";
const char g_szAceTargetNameFormat_0046a99c[4] = " %s";
const char g_szShipTargetNameFormat_0046a9a0[4] = " %s";
const char g_szRangeLabel_0046a9a4[12] = "\nRange : ";
const char g_szTargetTooFar_0046a9b0[8] = "TOO FAR";
const char g_szRangeMetresSuffix_0046a9b8[4] = " m";
const char g_szTargetOffscreenRange_0046a9bc[8] = "----- m";
short DAT_0046a9f8;
int g_bMusicCommandSuppressed_0046a9fc;
short g_nMusicTrackComplete_0046aa04 = 1;
short g_nFuneralSequenceActive_0046aa10;
int g_nCurrentMusicTrack_0046aa14 = -1;
int g_nMusicStreamSet_0046aa18 = -1;
volatile int g_nInFlightMusicSlotA_0046aa24 = -1;
volatile int g_nInFlightMusicSlotB_0046aa2c = -1;
int g_nWaitForMusicEnabled_0046aa30 = 1;
int g_nFlightSoundEffectsEnabled_0046aa34 = 1;
int g_nInitialFlightMusicPending_0046aa38 = 1;
int g_nCombatMusicActive_0046aa3c;
int g_nInFlightMusicActive_0046aa40;
short g_nPassingShipSoundObject_0046aa48 = -1;
short g_nPassingShipSoundCountdown_0046aa4c;
const unsigned char g_abFireworkSoundDescriptor_0046ab70[7] = {
    0x00, 0x80, 0x40, 0x40, 0x3c, 0x00, 0x00
};
const char g_szSfxWaveFormat_0046ad2c[12] = "sfx%02i.wav";
unsigned char g_abTaggedAllocationPrefix_0046ad88[8] = {
    'j', 'e', 'f', 'f', 0, 0, 0, 0
};
short g_nInputDoubleClickInterval_0046af54 = 1;
short g_nMenuPointerSpeed_0046af58 = 2;
short g_nCommMenuChoiceCount_0046af60 = -1;
short g_nCommMenuReuseMode_0046af64;
signed char g_cPendingCommMenuAction_0046af6c = 1;
unsigned char g_bRadioSilence_0046af70;
unsigned char g_bVideoImagesSuppressed_0046af74;
unsigned char DAT_0046af78 = 1;
signed char g_cCommMenuRecipient_0046afc4 = -1;
short g_nCommSpeakerObject_0046afc8;
short g_nCommSpeakerRating_0046afcc;
short g_nCommPortraitIndex_0046afd0 = -1;
const char g_szConfedCommFormat_0046b150[8] = "%s: %Fs";
const char g_szKilrathiAceCommFormat_0046b158[8] = "%s: %Fs";
const char g_szShipCommFormat_0046b160[8] = "%s: %Fs";
unsigned short DAT_0046b168 = 0xffff;
const GUID g_guidDirectDraw2_00463118 = {
    0xb3a6f3e0, 0x2b43, 0x11cf,
    { 0xa2, 0xde, 0x00, 0xaa, 0x00, 0xb9, 0x33, 0x56 }
};
LPDIRECTDRAW2 g_pDirectDraw2_0046b1a4;
LPDIRECTDRAWSURFACE g_pPrimarySurface_0046b1a8;
LPDIRECTDRAWSURFACE g_pSecondarySurface_0046b1ac;
LPDIRECTDRAWPALETTE g_pDirectDrawPalette_0046b1b0;
int DAT_0046b1b4 = -1;
int DAT_0046b1b8 = 62;
int DAT_0046b1bc;
int DAT_0046b1c4;
int DAT_0046b1c8;
float g_fSpaceFlightFrameRate_0046b1cc = 20.0f;
float g_fCinematicFrameRate_0046b1d0 = 16.0f;
const short g_aasCockpitLightX_0046dca8[5][7] = {
    {101, 101, 189, 137, 265, -99, 203},
    {198, 198, 133, 145, 234, 234, -99},
    {76, 76, 139, 257, 20, 20, -99},
    {194, 194, 96, 137, 96, 96, -99},
    {-99, -99, -99, 179, 133, -99, -99}
};
const short g_aasCockpitLightY_0046dcf0[5][7] = {
    {122, 145, 121, 7, 121, -99, 147},
    {120, 139, 114, 114, 179, 188, -99},
    {164, 183, 22, 181, 162, 178, -99},
    {119, 142, 136, 15, 126, 116, -99},
    {-99, -99, -99, 115, 114, -99, -99}
};
const signed char g_aacCockpitLightOffFrame_0046dd38[5][7] = {
    {14, 14, 14, 11, 13, 14, 14},
    {12, 12, 12, 13, 12, 12, 12},
    {9, 9, 9, 14, 9, 9, 9},
    {15, 15, 13, 11, 13, 13, 13},
    {5, 5, 5, 5, 5, 5, 5}
};
const signed char g_aacCockpitLightOnFrame_0046dd60[5][7] = {
    {3, 3, 3, 0, 2, 3, 3},
    {2, 2, 2, 3, 2, 2, 2},
    {1, 1, 1, 6, 1, 1, 1},
    {4, 4, 2, 0, 2, 2, 2},
    {1, 1, 1, 1, 1, 1, 1}
};
const CockpitBarDefinition g_aaCockpitBars_0046dd88[5][8] = {
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
const CockpitLayout g_stCockpitLayout_0046e000 = {
    {0, 0, 0, 0},
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
unsigned char g_bCurrentKey_0046c014 = 0x80;
signed char g_cPreviousKey_0046c018 = (signed char)0x80;
short g_nCurrentWave_0046c01c = -1;
int g_bIntroSecondaryScene_0046c024;
signed char g_abHazardObjects_0046c028[0x14] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
short g_nEyePitchRate_0046c004 = 1;
short g_nEyeYawRate_0046c008 = 1;
short g_nEyeRollRate_0046c00c = 1;
short DAT_0046c010 = -1;
signed char g_cCurrentObjective_0046c020 = -1;
int DAT_0046c03c = -1;
signed char g_cViewObject_0046c000 = -1;
short g_nExternalViewShip_0046c040 = -1;
short g_nEjectedPilotObject_0046c044;
short g_nClosestVisibleObject_0046c048 = -1;
short g_nYourWingman_0046c04c = -1;
short g_nPlayerCollisionObject_0046c050 = -1;
enum ObjectType g_eSelectedGunType_0046c054 = (enum ObjectType)-1;
int g_nSelectedReleaseWeaponIndex_0046c058 = -1;
short DAT_0046c05c;
short g_bTargetLockReadoutDirty_0046c060;
short g_nTargetLockCountdown_0046c064;
short g_nTargetLockMarkerAngle_0046c068;
signed char g_cTargetDisplayObject_0046c06c = -1;
int g_bTargetLockAcquired_0046c074;
short g_nTargetLockMode_0046c078;
int g_bMissileCameraEnabled_0046c07c;
int g_bEngageAllowed_0046c080;
short g_nAutoEngageTimer_0046c084 = -1;
signed char g_cViableTargetCount_0046c088;
const short g_asIntroCameraSequence_0046c090[20] = {
    0, -1000, 0, -4263, 2, 15, 1, 0, 30, 0,
    3, 15, 4, 30, 1, 13, 14, 400, -1, 0
};
const short g_asCannedSequence32_0046c0b8[] = {
    0, 20, 1, 0, 0, 180, 40, 1, 0, 0, 180, 40,
    2, 0, 400, -1
};
const short g_asCannedSequence33_0046c0d8[] = {
    0, 20, 0, 20, 1, 0, -15, -60, 50, 0, 400, -1
};
const short g_asCannedSequence34_0046c0f0[] = {
    0, 20, 0, 20, 1, 0, 15, 60, 50, 0, 400, -1
};
const short g_asCannedSequence35_0046c108[42] = {
    0, 17, 0, 6, 3, 0, 6, 3, 0, 6, 3, 0, 6, 3,
    0, 6, 3, 0, 6, 3, 0, 6, 3,
    1, 0, 0, 180, 40, 1, 0, 0, 180, 40,
    1, 0, -30, 60, 40, 0, 400, -1
};
const short g_asCannedSequence37_0046c190[] = {
    0, 100, 1, 0, 30, 0, 50, 0, 20, 2, 0, 400, -1
};
const short g_asCannedSequence38_0046c1b0[] = {
    0, 110, 1, 0, 30, 0, 50, 0, 10, 2, 0, 400, -1
};
const short g_asCannedSequence39_0046c1d0[] = {
    0, 10, 2, 0, 400, -1
};
const short g_asCannedSequence40_0046c1e0[] = {
    0, 40,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60,
    3, 1, 0, 0, 90, 60, 3, 1, 0, 0, 90, 60, -1
};
const short g_asCannedSequence41_0046c230[3] = { 0, 400, -1 };
const short g_asCannedSequence42_0046c250[] = {
    0, 120, 1, 0, 0, 180, 50, 3, 1, 0, 0, 180, 50, 3, -1
};
const short g_asCannedSequence43_0046c270[] = {
    0, 120, 1, 0, 0, 180, 50, 3, 1, 0, 0, 180, 50,
    3, 1, 0, 0, 180, 50, 3, -1
};
const short g_asCannedSequence44_0046c2a0[] = {
    0, 5, 3, 0, 5, 3, 0, 5, 3, 0, 5, 3,
    1, 0, 0, 180, 50, 1, 0, 0, 180, 50,
    1, 0, 0, 180, 50, 1, 0, 0, 180, 50, -1
};
const short g_asCannedSequence45_0046c2e8[] = { 0, 20, 2, -1 };

const signed char g_acDirectionShapeFrame_0046db28[
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
const signed char g_acDirectionShapeFlip_0046dbe8[
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

MissionNavPoint g_aMissionNavPoints_0046c2f0[WC1_MISSION_NAV_POINT_COUNT] = {
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_DRALTHI, OBJECT_TYPE_HORNET },
        { 32, 33, 34, 35, -1, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_ASTEROID_FIELD, (enum ObjectType)-1 },
        { 36, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_GRATHA, OBJECT_TYPE_RAPIER },
        { 37, 38, 39, 40, 41, -1, -1, -1, -1, -1 }
    },
    {
        "", 1, { 0, 0, 0 }, 50000,
        { { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
        { OBJECT_TYPE_KRANT, OBJECT_TYPE_SCIMITAR },
        { 42, 43, 44, 45, -1, -1, -1, -1, -1, -1 }
    }
};

MissionShipRecord g_aMissionShips_0046c948[WC1_MISSION_SHIP_COUNT] = {
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { 0, 0, 0 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence32_0046c0b8 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { -154521, -25600, -232012 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence33_0046c0d8 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_DRALTHI, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { -61849, 25600, -232012 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence34_0046c0f0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_HORNET, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      16, { 324582, 0, 696038 }, -155, 0, 0, 0, 40, 3,
      { g_asCannedSequence35_0046c108 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_ASTEROID_FIELD, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE, 17, { 0, 0, 0 }, 0, 0, 0, 0,
      30000, 3, { 0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 102400, -153600, -332800 }, 0, 0, -30, 0, 80, 3,
      { g_asCannedSequence37_0046c190 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -332800 }, 0, 0, 90, 0, 62, 3,
      { g_asCannedSequence38_0046c1b0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_GRATHA, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -153600 }, 0, 0, -30, 0, 80, 3,
      { g_asCannedSequence39_0046c1d0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_RAPIER, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 0, 256000, -819200 }, 0, 0, 30, 0, 60, 3,
      { g_asCannedSequence40_0046c1e0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_RAPIER, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      18, { 102400, -153600, -819200 }, 0, 0, 0, 0, 80, 3,
      { g_asCannedSequence41_0046c230 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      19, { -51200, 76800, -1382400 }, 0, 0, 0, 0, 50, 3,
      { g_asCannedSequence42_0046c250 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      19, { 153600, 0, -1280000 }, 0, 0, 60, 0, 50, 3,
      { g_asCannedSequence43_0046c270 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_KRANT, SIDE_KILRATHI, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE,
      19, { -249856, 0, 1139200 }, 155, 0, 0, 0, 50, 3,
      { g_asCannedSequence44_0046c2a0 }, 0, 0, 0, -1, 0, 0 },
    { OBJECT_TYPE_SCIMITAR, SIDE_IMPERIAL, -1, -1,
      MISSION_TYPE_CANNED_SEQUENCE, 19, { -76800, 0, 768000 },
      155, 0, 0, 0, 50, 3, { g_asCannedSequence45_0046c2e8 },
      0, 0, 0, -1, 0, 0 }
};
int g_aiIntelligenceEvent_0046d368[10];
signed char g_aDefenseManeuversNovice_0046d390[8] = {
    24, 34, 13, 14, -1, 0, 0, 0
};
signed char g_aDefenseManeuversVeteran_0046d398[8] = {
    8, 13, 15, 14, 19, 24, -1, 0
};
signed char g_aDefenseManeuversElite_0046d3a0[16] = {
    8, 15, 17, 23, 19, 9, 20, 34, 14, -1, 0, 0, 0, 0, 0, 0
};
signed char g_aDefenseManeuversAce_0046d3b0[16] = {
    17, 23, 15, 19, 9, 14, 20, 12, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char g_aDefenseManeuversBoss_0046d3c0[16] = {
    15, 19, 12, 11, 17, 23, 7, 35, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char *g_apDefenseManeuvers_0046d3d0[5] = {
    g_aDefenseManeuversNovice_0046d390,
    g_aDefenseManeuversVeteran_0046d398,
    g_aDefenseManeuversElite_0046d3a0,
    g_aDefenseManeuversAce_0046d3b0,
    g_aDefenseManeuversBoss_0046d3c0
};
ManeuverChoice g_aRatedManeuverChoices_0046d3e8[13][9][3] = {
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
ManeuverChoice g_aKilrathiManeuverChoices_0046d808[5][9][3] = {
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
signed char g_acPilotAggression_0046d9a0[24] = {
    3, 3, 3, 2, 2, 3, 2, 3, 0, 3, 2, 5,
    4, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0
};
signed char g_acPilotRecovery_0046d9b8[20] = {
    6, 7, 8, 8, 9, 8, 6, 8, 10, 7,
    8, 9, 7, 0, 7, 7, 8, 8, 0, 0
};
InputEvent *g_pInputEventHead_0046da90;
InputEvent *g_pInputEventTail_0046da94;
int g_bInputEventPoolInitialized_0046da98;
unsigned char *g_pDrawnMouseCursorShape_0046da9c;
int DAT_0046daa0;
short g_nScreenWidth_0046daa4 = 320;
short g_nScreenHeight_0046daa8 = 200;
const ScreenViewportGeometry g_aScreenViewportGeometry_0046dab8[6] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 320, 128, 0, 24, 0, 24, -24576, -1 },
    { 320, 200, 0, 0, 0, 0, -1536, -1 }
};
const short g_asPilotHandOrigins_0046e120[10] = {
    154, 187, 154, 187, 154, 187, 154, 187, 154, 187
};
CampaignDate g_stSavedCampaignDate_0046e188 = {20, 340};
const TalkingHeadOrigin g_aTalkingHeadOrigins_0046e190[11] = {
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
const signed char g_abBriefingSmallCharacterAnimation_0046e1e8[24] = {
    1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0
};
const signed char g_abBriefingLargeCharacterAnimation_0046e200[24] = {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0
};
BriefingCharacterLayout g_aBriefingCharacters_0046e218[8] = {
    { 60, 123,  10, 95, 176, 1, 0, 2,
      g_abBriefingSmallCharacterAnimation_0046e1e8, 0},
    {316, 123, 264, 94, 176, 1, 2, 1,
      g_abBriefingSmallCharacterAnimation_0046e1e8, 0},
    {193, 123, 141, 95, 176, 1, 3, 1,
      g_abBriefingSmallCharacterAnimation_0046e1e8, 0},
    {250, 124, 199, 93, 176, 1, 4, 1,
      g_abBriefingSmallCharacterAnimation_0046e1e8, 0},
    {124, 123,  71, 94, 176, 1, 5, 1,
      g_abBriefingSmallCharacterAnimation_0046e1e8, 0},
    {103, 122,  29, 76, 256, 1, 6, 2,
      g_abBriefingLargeCharacterAnimation_0046e200, 0},
    {191, 122, 118, 76, 256, 1, 8, 1,
      g_abBriefingLargeCharacterAnimation_0046e200, 0},
    {287, 122, 212, 76, 256, 1, 9, 1,
      g_abBriefingLargeCharacterAnimation_0046e200, 0}
};
const short g_asMedalDisplayX_0046e2d0[5] = {191, 199, 207, 216, 228};
const char *g_apszMedalNames_0046e2e0[5] = {
    g_szBronzeStar_0046e594,
    g_szSilverStar_0046e5a0,
    g_szGoldStar_0046e5ac,
    g_szGoldenSun_0046e5b8,
    g_szTerranMedalOfValor_0046e5c4
};
unsigned char *g_pMedalSceneShape_0046e2f4;
const char *g_pszMedalsPilotSummary_0046e2f8 =
    g_szMedalsPilotSummary_0046e5dc;
const signed char g_aBriefingPortraitOffsetX_0046e300[8][12] = {
    {  0,  0, -2, -2, -1, -2, -4, -3, -5, -7, -7, -4},
    { -1,  0, -2, -3, -3, -4, -3, -4, -6, -8, -7, -6},
    {  0,  1,  0,  0,  0,  0,  0, -1, -2, -4, -5, -3},
    {  0, -1, -2, -3, -1, -3, -2, -3, -4, -4, -5, -4},
    {  2,  0,  0,  0,  0,  0,  0,  0, -2, -3, -4, -2},
    {  1,  1,  1,  0,  1,  0, -6, -4, -4, -9, -8, -6},
    { -2, -1, -4, -4, -4, -7, -8, -7, -5, -8, -9, -7},
    {  0,  0,  0,  0,  0, -3, -3, -7, -5, -7, -8, -7}
};
const signed char g_aBriefingPortraitOffsetY_0046e360[8][12] = {
    { -6, -2, -3, -1, -4,-10,-12,-20,-32,-38,-42,-43},
    { -5,  0, -4, -4, -7,-12,-16,-23,-32,-37,-42,-42},
    { -5, -3,  0,  1,  0, -7,-13,-23,-33,-38,-42,-42},
    { -3, -2, -1,  0, -5,-10,-16,-25,-33,-38,-41,-41},
    { -6,  0,  2,  0, -6,-11,-18,-25,-33,-39,-42,-42},
    { -8, -2,  1, -3, -6,-16,-23,-35,-46,-55,-61,-61},
    { -6, -4, -3, -5, -9,-14,-23,-34,-49,-57,-61,-62},
    {-10, -5,  0,  0, -3, -7,-24,-32,-47,-55,-59,-60}
};
const short g_aBriefingPortraitScale_0046e3c0[8][12] = {
    {357, 357, 357, 359, 355,   0, 352, 355, 353, 353, 355,   1},
    {355,   0, 354, 354, 352, 354, 355, 355, 352, 352, 353, 350},
    {354, 354, 359,   0, 358, 357, 357, 356, 357,   0,   0,   2},
    {354, 354, 353, 356, 358, 356, 356, 355, 356,   0, 357, 357},
    {  0,   0,   0,   0, 358,   0, 358,   0,   0,   4,   0, 359},
    {  0, 358, 358,   0, 357,   0, 350, 354, 357, 354,   0, 358},
    {356, 356, 352, 354, 352, 349, 350, 352,   0,   0,   0,   0},
    {  0,   0,   0, 359,   0, 350, 354, 350, 356, 357, 357, 357}
};
const int g_aiBriefingLeftPanelVelocity_0046e480[12] = {
    1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4
};
const int g_aiBriefingPodiumVelocity_0046e4b0[12] = {
    1, 2, 2, 3, 3, 4, 4, 4, 5, 5, 5, 6
};
const int g_aiBriefingRightPanelVelocity_0046e4e0[12] = {
    2, 2, 3, 4, 4, 4, 5, 5, 5, 6, 7, 8
};
const signed char g_abBriefingPodiumFrames_0046e510[40] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 14, 13, 12, 11, 10,  9,
     8,  7,  6,  5,  4,  0,  0,  0,  0,  0
};
const signed char g_abDebriefingEstablishDeltas_0046e538[48] = {
    -2, -2, -1, -1,  0,  0,  1,  0,
     1,  0,  1,  0,  1,  0,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  0,  1,  0,  1,
     0,  1,  0,  1,  0,  0, -1, -1
};
short g_nDebriefingLeftX_0046e56c;
short g_nDebriefingPilotX_0046e570 = 80;
short g_nDebriefingRightX_0046e574 = 278;
short g_nDebriefingOfficerX_0046e578 = 200;
short g_nDebriefingPodiumX_0046e57c = 344;
short g_nConversationCharacter_0046e580 = -1;
short g_nTalkingHeadFace_0046e584 = -1;
short g_nConversationBackdropFrame_0046e588 = -1;
int g_bConversationConstellation_0046e58c;
int g_bConversationOverlay_0046e590;
const char g_szBronzeStar_0046e594[12] = "Bronze Star";
const char g_szSilverStar_0046e5a0[12] = "Silver Star";
const char g_szGoldStar_0046e5ac[12] = "Gold Star";
const char g_szGoldenSun_0046e5b8[12] = "Golden Sun";
const char g_szTerranMedalOfValor_0046e5c4[24] =
    "Terran Medal of Valor";
const char g_szMedalsPilotSummary_0046e5dc[40] =
    "$R $N, aka $C.\n$S system, dateline $D.";
const char g_szViewMedalsTextFormat_0046e604[12] = "%X%Y%F%s%P";
const char g_szMedalChestTextFormat_0046e610[12] = "%X%Y%F%s%P";
const char g_szMedalLongShotTextFormat_0046e61c[12] = "%X%Y%F%s%P";
const char g_szMedalEstablishTextFormat_0046e628[12] = "%X%Y%F%s%P";
const char g_szPinMedalTextFormat_0046e634[12] = "%X%Y%F%s%P";
const char g_szEstablishingShotTextFormat_0046e640[12] = "%X%Y%F%s%P";
const char g_szBriefingReturnTextFormat_0046e64c[12] = "%X%Y%F%s%P";
const char g_szDismissedTextFormat_0046e658[12] = "%X%Y%F%s%P";
const char g_szDebriefEstablishTextFormat_0046e664[12] = "%X%Y%F%s%P";
const char g_szFuneralLongShotTextFormat_0046e670[12] = "%X%Y%F%s%P";
unsigned char g_abRasterPaletteTranslation_0046ff2c[256];
RLETransformVertex g_aRLETransformVertices_0047002c[4];
int g_anRLESourceSteps_0047007c[4];
int g_nRLETransformWorkspaceReserved_0047008c;
const char *g_pszBlankRoomMenuLabel_00470090 =
    g_szBlankRoomMenuLabel_0047052c;
const char *g_apszPilotRankNames_00470098[5] = {
    g_szSecondLieutenantRank_00470530,
    g_szFirstLieutenantRank_00470538,
    g_szCaptainRank_00470540,
    g_szMajorRank_00470548,
    g_szLieutenantColonelRank_00470550
};
CampaignState g_stInitialCampaignState_004700b0 = {
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
PilotRecord g_aInitialPilotRecords_00470108[9] = {
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
signed char g_abSpiritRecRoomAnimation_00470260[48] = {
    0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 3, 4, 3, 3, 4,
    3, 4, 3, 4, 5, 3, 4, 5, 3, 4, 3, 4, 3, 4, 5, 3,
    2, 2, 2, 3, 2, 2, 2, 1, 1, 1, -1, 0, 0, 0, 0, 0
};
signed char g_abHunterRecRoomAnimation_00470290[40] = {
    0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 2, 2,
    3, 3, 4, 4, 5, 3, 3, 4, 4, 5, 5, 5, 2, 2, 1, 1,
    0, 0, -1, 0, 0, 0, 0, 0
};
signed char g_abAngelRecRoomAnimation_004702b8[48] = {
    3, 4, 3, 3, 4, 3, 4, 4, 3, 4, 3, 4, 0, 0, 0, 2,
    0, 2, 0, 1, 0, 2, 0, 0, 2, 0, 1, 0, 2, 3, 3, 4,
    3, 4, 4, 5, 5, 5, 5, 5, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char g_abKnightRecRoomAnimation_004702e8[40] = {
    0, 0, 0, 0, 0, 1, 0, 1, 0, 5, 0, 0, 1, 2, 3, 4,
    4, 3, 2, 2, 2, 3, 4, 4, 5, 5, 0, 0, 1, 0, 1, 2,
    1, 0, 1, -1, 0, 0, 0, 0
};
signed char g_abIcemanRecRoomAnimation_00470310[40] = {
    0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 5, 4, 4, 3,
    3, 2, 2, 1, 0, 0, -1, 0
};
signed char g_abManiacRecRoomAnimation_00470338[24] = {
    0, 1, 2, 0, 1, 2, 0, 3, 2, 0, 1, 1,
    0, 4, 2, 0, 1, 2, 0, 5, 5, 5, 5, -1
};
signed char g_abPaladinRecRoomAnimation_00470350[56] = {
    0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 2, 2, 2, 3, 2,
    2, 3, 2, 3, 2, 3, 2, 4, 4, 2, 3, 2, 3, 2, 3, 2,
    3, 2, 3, 4, 4, 5, 5, 4, 4, 4, 4, 5, 5, 4, 2, 2,
    2, -1, 0, 0, 0, 0, 0, 0
};
signed char g_abBossmanRecRoomAnimation_00470388[48] = {
    3, 3, 3, 2, 3, 2, 3, 2, 3, 3, 2, 3, 1, 3, 1, 3,
    0, 3, 0, 3, 0, 2, 1, 2, 1, 3, 3, 4, 5, 5, 4, 5,
    4, 5, 1, 3, 1, 3, 1, 3, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char g_abShotglassIdleAnimation_004703b8[24] = {
    0, 0, 0, 0, 0, 1, 1, 0, 0, 2, 2, 0,
    0, 3, 3, 3, -1, 0, 0, 0, 0, 0, 0, 0
};
signed char g_abShotglassGlassAnimation_004703d0[32] = {
    3, 6, 7, 8, 8, 9, 9, 10, 10, 10, 12, 11, 10, 12, 11, 10,
    12, 11, 10, 12, 11, 10, 13, 13, 13, 10, 9, 8, 8, 3, 3, -1
};
signed char g_abShotglassPourAnimation_004703f0[80] = {
    3, 3, 6, 6, 7, 8, 8, 14, 15, 15, 16, 17, 18, 18, 18, 18,
    17, 19, 19, 20, 20, 21, 20, 21, 20, 21, 20, 21, 20, 21, 20, 21,
    22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 28, 29, 30, 31,
    31, 31, 31, 31, 32, 32, 31, 31, 31, 33, 33, 32, 34, 34, 35, 35,
    36, 36, 37, 37, 37, 38, 38, 39, 40, 3, 3, 3, -1, 0, 0, 0
};
signed char g_abShotglassWipeAnimation_00470440[24] = {
    3, 4, 3, 4, 5, 3, 4, 3, 5, 6, 3, 3,
    3, 3, 4, 3, 4, 3, -1, 0, 0, 0, 0, 0
};
signed char *g_apRecRoomAnimations_00470458[14] = {
    g_abSpiritRecRoomAnimation_00470260,
    g_abHunterRecRoomAnimation_00470290,
    g_abBossmanRecRoomAnimation_00470388,
    g_abIcemanRecRoomAnimation_00470310,
    g_abAngelRecRoomAnimation_004702b8,
    g_abPaladinRecRoomAnimation_00470350,
    g_abManiacRecRoomAnimation_00470338,
    g_abKnightRecRoomAnimation_004702e8,
    0,
    g_abShotglassIdleAnimation_004703b8,
    g_abShotglassGlassAnimation_004703d0,
    g_abShotglassPourAnimation_004703f0,
    g_abShotglassWipeAnimation_00470440,
    0
};
const ShortPoint g_aRecRoomCharacterOrigins_00470490[3] = {
    {94, 59}, {161, 79}, {202, 79}
};
short g_nRecRoomPanelOriginX_0047049c = 158;
short g_nRecRoomPanelOriginY_0047049e = 28;
TitleMenuRegion g_aRecRoomMenuRegions_004704a0[7] = {
    { 1, 94, 59, 130, 95 },
    { 1, 161, 79, 180, 95 },
    { 1, 210, 79, 240, 95 },
    { 1, 180, 50, 250, 75 },
    { 1, 275, 50, 319, 135 },
    { 1, 0, 100, 120, 190 },
    { -1, 0, 0, 0, 0 }
};
char *g_apszRecRoomBaseLabels_004704e8[3] = {
    g_szTalkToShotglass_0047055c,
    g_szTalkToFirstPilot_00470570,
    g_szTalkToSecondPilot_00470588
};
char *g_apszRecRoomMenuLabels_004704f8[6] = {
    0, 0, 0,
    g_szCheckPilotScores_004705a0,
    g_szEnterBarracks_004705b4,
    g_szFlyTrainingMission_004705c4
};
int DAT_00470510;
CampaignDate g_stChalkBoardDate_00470514 = {-1, -1};
short g_asChalkBoardPilotOrder_00470518[9] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8
};
const char g_szBlankRoomMenuLabel_0047052c[4] = " ";
const char g_szSecondLieutenantRank_00470530[8] = "2ND LT.";
const char g_szFirstLieutenantRank_00470538[8] = "1ST LT.";
const char g_szCaptainRank_00470540[8] = "CAPTAIN";
const char g_szMajorRank_00470548[8] = "MAJOR";
const char g_szLieutenantColonelRank_00470550[12] = "LT. COL.";
char g_szTalkToShotglass_0047055c[20] = "Talk to SHOTGLASS.";
char g_szTalkToFirstPilot_00470570[24] = "Talk to ??????????????";
char g_szTalkToSecondPilot_00470588[24] = "Talk to ??????????????";
char g_szCheckPilotScores_004705a0[20] = "Check pilot scores";
char g_szEnterBarracks_004705b4[16] = "Enter barracks";
char g_szFlyTrainingMission_004705c4[24] = "Fly training mission";
const char g_szRoomMenuLabelFormat_004705dc[12] = "%X%Y%s%P";
const char g_szEmptyRoomMenuLabel_004705e8[4] = "";
const char g_szTalkToPilotFormat1_004705ec[16] = "Talk to %s.";
const char g_szTalkToPilotFormat2_004705fc[16] = "Talk to %s.";
const char g_szChalkBoardHeading_0047060c[32] =
    "%X%YCARRIER - TIGER'S CLAW%P";
const char g_szChalkBoardPilotHeading_0047062c[12] = "%X%YPILOT";
const char g_szChalkBoardScoreHeading_00470638[20] =
    "%X%YSORTIES KILLS";
const char g_szChalkBoardPilotFormat_0047064c[12] = "%X%Y%s %s";
const char g_szChalkBoardScoreFormat_00470658[12] = "%X%Y%d%X%d";
const char g_szKilledInAction_00470664[4] = "KIA";
const char g_szChalkBoardKiaFormat_00470668[16] = "%X%Y     %s";
unsigned char g_abPaletteTranslation_00470678[256] = {
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
unsigned short g_awAbsoluteCosine_00470778[360] = {
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
unsigned short g_awAbsoluteSine_00470a48[360] = {
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
const char g_szInvalidShapeAllocation_00470d18[12] = "not jefftep";
const char g_szBadViewport_00470d24[12] = "bad vport";
const char g_szShapeRLEVersion_00470d30[8] = "1.00";
const char g_szShapeRLEOverflow_00470d38[20] = "qq copy overflow";
const char g_szBadShapeFlip_00470d4c[12] = "bad flip";
const char g_szCopyLine_00470d58[12] = "__copyline";
const char g_szWipeLine_00470d64[12] = "__wipeline";
const char g_szTriangle_00470d70[12] = "triangle";
const char g_szShadedTriangle_00470d7c[16] = "shaded_triangle";
const char g_szShadowDraw_00470d8c[12] = "shadow_draw";
const char g_szFizzleFade_00470d98[12] = "fizzle_fade";
const char g_szSnowViewport_00470da4[16] = "snow_viewport";
int g_nInsertKeyState_00475b68;
int g_nClearedKeyboardState_00475b6c;
int g_nOemPeriodKeyState_00475bf0;
int g_nHomeKeyState_00475bf4;
int g_nPageDownKeyState_00475bf8;
int g_nOemCommaKeyState_00475bfc;
int g_nRightArrowKeyState_00475c00;
int g_nEndKeyState_00475c04;
int g_nClearKeyState_00475c08;
int g_nDeleteKeyState_00475c0c;
int g_nUpArrowKeyState_00475c10;
int g_nDownArrowKeyState_00475c14;
char g_szStreamsPath_00475c18[0x100];
int g_nPageUpKeyState_00475d18;
int g_nLeftArrowKeyState_00475d1c;
char g_szReadDataFileError_00475d20[0x40];
char g_szCreateDataFileError_00475d60[0x40];
char g_szWriteDataFileError_00475da0[0x40];
char g_szSeekDataFileError_00475de0[0x80];
int g_bMonoDebugInstalled_00475e70;
HANDLE g_hMonoDebugDevice_00475e74;
unsigned short DAT_00475e78;
unsigned char g_bCurrentManeuverReroll_00475e7c;
NavMapLabel g_aNavMapLabels_00475e80[20];
ShortRect g_aNavMapExclusionRects_00475f48[21];
unsigned char g_abMouseCursorBackground_00475ff0[0x400];
DebugOverlayConsole *g_pDebugOverlay_004763f0;
int g_nFlightFrameTotalTicks_004764f8;
int g_nFlightCockpitTicks_004764fc;
LARGE_INTEGER g_liFlightAfterInput_00476500;
LARGE_INTEGER g_liFlightFrameEnd_00476508;
int g_nFlightPresentTicks_00476510;
LARGE_INTEGER g_liFlightFrameStart_00476518;
LARGE_INTEGER g_liFlightAfterSimulation_00476520;
int g_nFlightSimulationTicks_00476528;
LARGE_INTEGER g_liFlightAfterCockpit_00476530;
int g_nFlightInputTicks_00476538;
LARGE_INTEGER g_liFlightAfterRender_00476540;
int g_nFlightRenderTicks_00476548;
IxSample *g_pLoopingWaveSample_0047654c;
IxSound *g_pLoopingWaveSound_00476550;
int g_nDisplayedComponentDamage_00476554;
char g_szSfxWavePath_00476558[0xc8];
unsigned char DAT_00476620[32];
unsigned int DAT_00476640;
unsigned int DAT_00476644;
unsigned char *DAT_00476648;
unsigned int DAT_0047664c;
unsigned int DAT_00476650;
unsigned char DAT_00476658[0xfa00];
int DAT_00486058;
int DAT_00486060;
HWND DAT_00486074;
char DAT_00486078[256];
unsigned char DAT_00486110[1024];
float DAT_00486510;
int DAT_00486518;
unsigned char DAT_004865a8[0x1000];
RasterSurface g_stRasterSurface_004875a8;
unsigned char g_abShapeTransformScratch_004875c0[0xfa00];
RasterClip g_stRasterClip_00496fc0;
unsigned char g_abSolidColourTranslation_00497648[256];
unsigned char g_abShapeRLEScratch_00497748[0x100000];
extern unsigned int g_dwStreamerState_00597cd0;
int g_bStreamerAudioPlaying_00597748;
char g_szStreamerPath_00597750[128];
int DAT_00598888;
int DAT_0059888c;
int DAT_00598890;
Viewport g_stRoomScreenViewport_005988a0;
short g_nSavedRoomControllerX_005988b4;
unsigned char *g_pRecRoomRoster_005988b8;
unsigned char *g_pMissionCampaignData_005988bc;
unsigned char *g_apRecRoomCharacterShapes_005988c0[3];
PilotRecord g_aPilotRecords_005988d0[9];
ConstellationObjectDefinition *g_pConstellationDefinitions_00598a28;
PilotRecord *g_apWingmanPilots_00598a30[8];
unsigned char *g_pRecRoomBackgroundShape_00598a50;
Viewport g_stRoomDisplayViewport_00598a60;
Viewport g_stRoomMouseViewport_00598a80;
unsigned char *g_apRecRoomTextData_00598aa0[3];
short g_nRoomMenuCursorFrame_00598ab0;
TitleMenuRegion *g_pRoomMenuRegions_00598ab2;
char **g_ppszRoomMenuLabels_00598ab6;
char *g_pszCurrentRoomMenuLabel_00598aba;
TextContext g_stRoomMenuTextContext_00598abe;
unsigned char *g_apRecRoomSceneData_00598ae0[3];
unsigned char *g_pBriefingPacket_00598aec;
unsigned char *g_pBriefingTextData_00598af0;
short *g_pMouthAnimationCommands_00598af4;
unsigned char *g_pDebriefingSceneData_00598afc;
char g_szTextScratchBuffer_00598b00[256];
unsigned char *g_pBriefingSceneData_00598c00;
unsigned char *g_pConversationBackdropShape_00598c04;
short g_nConversationMedalIndex_00598c08;
short g_nConversationSceneType_00598c0a;
unsigned char *g_pTalkingHeadShape_00598c0c;
short g_nConversationTextColour_00598c10;
unsigned char *g_pBriefingAnimationShape_00598c14;
short *g_pFaceAnimationCommands_00598c18;
unsigned char *g_pBriefingBodyShape_00598c1c;
unsigned char *g_pMedalSceneData_00598c20;
unsigned char *g_pBriefingPortraitShape_00598c24;
unsigned char *g_pDebriefingTextData_00598c28;
unsigned char *g_pBriefingCloseupShape_00598c2c;
unsigned char *g_pConversationOverlayShape_00598c30;
InputEvent g_aInputEventPool_00598c40[0x100];
int g_nMouseCursorSavedY_0059a840;
int g_nMouseCursorSavedX_0059a844;
unsigned char g_bInputMode_0059a848;
int DAT_0059a84c;
short g_nEventManagerActive_0059a850;
short g_nViewCenterX_0059a852;
short g_nViewCenterY_0059a854;
short DAT_0059a856;
short g_nPacketDecompressSourceFile_0059a858;
unsigned char g_abInputKeyState_0059a860[0x80];
int DAT_0059a8e0;
int DAT_0059a8e4;
int g_anObjectDepthPlaced_0059a8f0[WC1_SPACE_OBJECT_COUNT];
signed char g_cScreenViewportMode_0059a9f2;
const ScreenViewportGeometry * volatile
    g_pScreenViewportGeometry_0059a9f4;
int g_anSortedObject_0059aa00[WC1_SPACE_OBJECT_COUNT];
short g_nPacketDecompressInputPosition_0059ab00;
void *g_pPacketDecompressInput_0059ab04;
unsigned short g_wCurrentInputModifiers_0059ab08;
volatile short g_nMouseX_0059ab10;
volatile short g_nMouseY_0059ab12;
unsigned char g_bPrimaryMouseButton_0059ab14;
unsigned char g_bSecondaryMouseButton_0059ab15;
unsigned char g_bInputReserved_0059ab16;
unsigned short g_wInputFlags_0059ab17;
unsigned char * volatile DAT_0059ab19;
unsigned short DAT_0059ab1d;
Viewport * volatile DAT_0059ab23;
unsigned char g_bMouseCursorShapeChanged_0059ab2b;
void (*DAT_0059ab2c)(void);
int g_nPacketDecompressResult_0059ab30;
short DAT_0059ab34;
short g_nPacketDecompressPending_0059ab36;
unsigned short g_wPacketDecompressInputSize_0059ab38;
short g_nPacketDecompressWorkspaceSegment_0059ab3a;
volatile int DAT_0059ab3c;
int DAT_0059ab40;
int DAT_0059ab44;
int DAT_0059ab48;
int DAT_0059ab4c;
short g_nViewportOriginY_0059ab50;
short g_nViewportOriginX_0059ab52;
unsigned int DAT_0059ab54;
unsigned char DAT_0059ab58;
int DAT_0059ab5c;
int DAT_0059ab60;
int g_aiInputEventSlotUsed_0059ab70[0x100];
short g_nHostMouseX_0059af70;
short g_nHostMouseY_0059af72;
unsigned char g_bPreviousPrimaryInputButton_0059af74;
unsigned char g_bPreviousSecondaryInputButton_0059af75;
unsigned char g_bMenuInputReserved_0059af76;
unsigned short g_wPreviousMenuInputFlags_0059af77;
TextContext *g_pCurrentTextContext_0059af8c;
int g_nInputTickScale_0059af90;
FixedVector g_aObjectViewPosition_0059afa0[WC1_SPACE_OBJECT_COUNT];
short g_anObjectPitchRotation_0059b2a0[WC1_SPACE_OBJECT_COUNT];
int g_anShipSpeed_0059b320[64];
short g_nSpaceFrame_0059b420;
unsigned int DAT_0059b430[512];
FixedVector g_vPlayerAcceleration_0059b460;
int g_anShipFuel_0059b470[512];
short g_asObjectDistance_0059b4a0[WC1_SPACE_OBJECT_COUNT];
ShortVector g_aShipFormationOffset_0059b520[10];
enum ObjectType g_aeObjectType_0059b560[96];
short g_asObjectAnimationDelay_0059b660[WC1_SPACE_OBJECT_COUNT];
FixedVector g_aShipRightVector_0059b6e0[64];
FixedVector g_aShipUpVector_0059b9e0[64];
FixedVector g_aShipForwardVector_0059bce0[64];
HazardField *g_pActiveHazardField_0059bfe0;
signed char g_acPlayerComponentDamage_0059bff0[9];
signed char g_abFlightPath_0059c000[WC1_MISSION_OBJECTIVE_COUNT + 1];
FixedVector g_aShipVelocity_0059c010[512];
short g_anYawGoal_0059c310[16];
short g_asObjectCounter_0059c330[512];
short g_nTargetShip_0059c3b0;
enum SpecialManeuver g_aeSpecialManeuver_0059c3c0[WC1_SPACE_OBJECT_COUNT];
enum ShipMissionType g_aeShipMissionType_0059c3f0[512];
short g_asShipCount_0059c420[512];
short g_asShipMaximumSpeed_0059c440[16];
signed char g_acShipDamage_0059c460[10];
signed char g_cMissionObjectiveCount_0059c46a;
short g_asViableTargetDistance_0059c470[16];
FixedVector g_aShipPosition_0059c490[512];
short g_asObjectRadarRadius_0059c790[WC1_SPACE_OBJECT_COUNT];
short g_asShipAfterburnerTimer_0059c810[256];
short g_nShipMissionIndices_0059c830[10];
FixedVector g_vStarFieldMotion_0059c860;
signed char g_cCurrentNavPointIndex_0059c86c;
short g_asObjectFlip_0059c870[WC1_SPACE_OBJECT_COUNT];
short g_nEyeRollGoal_0059c8f0;
FixedVector g_vPreviousStarFieldMotion_0059c900;
short g_nHazardFieldCount_0059c90c;
signed char DAT_0059c910[WC1_SPACE_OBJECT_COUNT];
signed char g_acViableTarget_0059c920[16];
short g_nEyeYawGoal_0059c944;
short g_asObjectScreenScale_0059c950[WC1_SPACE_OBJECT_COUNT];
short g_asObjectAfterburnerVelocity_0059c9d0[WC1_SPACE_OBJECT_COUNT];
CampaignState g_stCampaignState_0059ca50;
unsigned char g_aShipWeapons_0059cab0[16][0x47];
signed char g_acShipRating_0059cd80[16];
short g_asObjectScreenAngle_0059cd90[WC1_SPACE_OBJECT_COUNT];
short g_nTargetRange_0059ce10;
unsigned int DAT_0059ce18[256];
signed char g_acObjectOwner_0059ce20[64];
signed char g_acShipTarget_0059ce60[512];
short g_nPreviousPitchInput_0059ce70;
short g_nPreviousYawInput_0059ce72;
short g_nPreviousRollInput_0059ce74;
short g_anObjectYawRotation_0059ce80[256];
signed char DAT_0059cf00[WC1_SPACE_OBJECT_COUNT];
signed char g_acTurnRegulator_0059cf10[16];
unsigned char DAT_0059cf20[512];
short g_asObjectDrawY_0059cf80[WC1_SPACE_OBJECT_COUNT];
short g_asObjectDrawX_0059d000[WC1_SPACE_OBJECT_COUNT];
short g_asPreviousObjectDistance_0059d080[WC1_SPACE_OBJECT_COUNT];
enum ObjectClass g_aeObjectClass_0059d100[512];
enum ShipObjective g_aeShipObjective_0059d200[512];
short g_asObjectViewFrame_0059d230[WC1_SPACE_OBJECT_COUNT];
int g_nCurrentObjectiveRange_0059d2b0;
signed char g_acWingmanMessageState_0059d2c0[16];
short g_asCollisionCountdown_0059d2d0[16];
unsigned char *g_apObjectShape_0059d2f0[WC1_SPACE_OBJECT_COUNT];
short g_nPitchInput_0059d3f0;
short g_nYawInput_0059d3f2;
short g_nRollInput_0059d3f4;
short g_asShipWingLeader_0059d400[16];
short g_aasShipArmor_0059d420[10][4];
short g_asShipWeaponEnergy_0059d470[16];
signed char g_acFormationMemberList_0059d490[16];
signed char g_acShipIonDriveDamage_0059d4a0[16];
short g_anShipMissionShip_0059d4b0[16];
FixedVector g_vToTarget_0059d4d0;
short g_asCannedCommand_0059d4e0[WC1_SPACE_OBJECT_COUNT];
unsigned short DAT_0059d500[16];
char g_acShipSequence_0059d520[512];
short g_nTargetFacing_0059d52a;
FixedVector g_aShipDestination_0059d530[10];
short g_aasShipShield_0059d5b0[12][2];
enum ShipTactic g_aeShipTactic_0059d5e0[512];
signed char g_abShipExhaustHeat_0059d610[10];
short g_nRenderedSpaceFrame_0059d61a;
short g_nEyePitchGoal_0059d61c;
signed char g_acShipStress_0059d620[16];
short g_anRollGoal_0059d630[16];
enum Side g_aeShipSide_0059d650[512];
FixedVector g_vCollisionDelta_0059d690;
signed char g_acLastCollisionObject_0059d6a0[WC1_SPACE_OBJECT_COUNT];
short g_aasShipMaximumShield_0059d6e0[12][2];
short g_asObjectCollisionRadius_0059d710[64];
signed char g_acShipPointingMode_0059d790[16];
short g_anPitchGoal_0059d7a0[16];
signed char g_abShipNavPointIndex_0059d7c0[512];
short g_anObjectRollRotation_0059d7e0[WC1_SPACE_OBJECT_COUNT];
unsigned char g_abShipTurn_0059d860[WC1_SPACE_OBJECT_COUNT];
HazardField g_aHazardFields_0059d870[7];
short g_nFacingToTarget_0059d920;
short g_asObjectScreenY_0059d930[WC1_SPACE_OBJECT_COUNT];
short g_asObjectScreenX_0059d9b0[WC1_SPACE_OBJECT_COUNT];
short g_asObjectAnimationIndex_0059da30[WC1_SPACE_OBJECT_COUNT];
signed char g_cCockpitView_0059dab0;
MissionObjective g_aMissionObjectives_0059dac0[WC1_MISSION_OBJECTIVE_COUNT];
enum ShipManeuver g_aeShipManeuver_0059dcb0[512];
const short *g_apCannedSequence_0059dce0[WC1_SPACE_OBJECT_COUNT];
FixedVector g_aShipMissionSpot_0059dd10[10];
unsigned char DAT_0059dd88[8];
short g_asCapitalShipViewFrame_0059dd90[16];
signed char g_acObjectCollisionGraceTicks_0059ddb0[
    WC1_SPACE_OBJECT_COUNT];
ObjectResourceSlot g_aObjectResourceSlots_0059ddf0[4] = {
    { -1, 0, 0, 0 }, { -1, 0, 0, 0 },
    { -1, 0, 0, 0 }, { -1, 0, 0, 0 }
};
signed char g_acShipDestroyedWeaponCount_0059de30[16];
short g_asObjectScale_0059de40[WC1_SPACE_OBJECT_COUNT];
char DAT_0059dec0[256];
short g_asShipAccumulatedDamage_0059dee0[WC1_SPACE_OBJECT_COUNT];
short g_nCurrentNavPoint_0059df60;
unsigned char *g_apCommPortraitShapes_0059e180[16];
char g_szHudMessageBuffer_0059e1c0[0x60];
char *g_aapszPilotSpeech_0059e220[14][11];
signed char g_abCommMenuChoiceCommand_0059e488[7];
char *g_pszCommMenuHeading_0059e490;
char g_szCommMenuHeadingBuffer_0059e4a0[0x40];
char *g_apszCommMenuChoiceText_0059e4e0[7];
int g_anInputButton2PressTime_0059e500[2];
short g_asInputButton1DoubleClick_0059e508[2];
short g_asInputButton1Changed_0059e50c[2];
short g_asInputButton2Changed_0059e510[2];
short g_asPreviousInputButtons_0059e514[2];
int g_anInputButton1PressTime_0059e518[2];
short g_asInputButton2DoubleClick_0059e520[2];
int g_aiPacketHandles_0059e530[0x1000];
int g_aiPacketHandleOffsets_005a2530[0x1000];
int g_nPacketHandleCount_005a6530;
int *DAT_005a6538;
int g_aiSoundEffectSourceActive_005a66ec[0x41];
unsigned char g_abSoundPlaybackSlots_005a67f0[0xfc];
int g_nPassingShipSoundCooldown_005a68e8;
int g_nAllocateViewportCalls_005a68ec;
void *g_pLastPacketAllocation_005a68f0;
FireworkState g_aFireworks_005a6900[30];
unsigned char *g_pFireworkShape_005a6a68;
ConstellationStar g_aConstellationStars_005a6a70[10];
Viewport *g_pConstellationViewport_005a6aac;
short g_nConstellationStarCount_005a6ab0;
ConstellationParticle g_aConstellationParticles_005a6ac0[16];
Viewport g_stConstellationViewport_005a6b40;
short g_nConstellationParticleCount_005a6b54;
const short *g_pViewScript_005a6b58;
short g_nExternalViewDistance_005a6b5c;
Viewport DAT_005a6b60;
Viewport DAT_005a6b80;
ScreenViewportPacket *g_pScreenViewportPacket_005a6b94;
Viewport DAT_005a6ba0;
TextContext DAT_005a6bc0;
Viewport DAT_005a6be0;
unsigned char *g_pTargetLockShape_005a6bf4;
unsigned char *g_apTextFonts_005a6c00[4];
FontWorkspace **g_apFontWorkspaces_005a6c10[4];
FixedVector g_aDirectionViewRightVector_005a6c20[WC1_DIRECTION_VIEW_COUNT];
FixedVector g_aDirectionViewUpVector_005a6f10[WC1_DIRECTION_VIEW_COUNT];
FixedVector g_aDirectionViewForwardVector_005a7200[WC1_DIRECTION_VIEW_COUNT];
TextContext DAT_005a74f0;
Viewport DAT_005a7510;
Viewport DAT_005a7530;
Viewport DAT_005a7550;
unsigned char *g_pCockpitWeaponShape_005a7564;
Viewport g_stConversationTextViewport_005a7570;
char g_szDefaultTextBuffer_005a7590[0xcc];
unsigned char *g_pCockpitIndicatorShape_005a7658;
unsigned char *g_pConstellationShape_005a765c;
unsigned char *g_pCommMenuCursorShape_005a7660;
Viewport g_stModalSourceViewport_005a7670;
unsigned char *g_pPilotHandShape_005a7684;
Viewport DAT_005a7690;
Viewport DAT_005a76b0;
short g_aPaletteFadeEntries_005a76d0[6][3];
unsigned char *g_pCockpitDamageShape_005a76f4;
TextContext DAT_005a7700;
TextContext DAT_005a7720;
TextContext g_stDefaultTextContext_005a7740;
TextContext g_stConversationTextContext_005a7760;
short DAT_005a7780[3];
short g_nDamageDisplayTicks_005a7786;
short g_nWeaponDisplayOriginX_005a7788;
short g_nWeaponDisplayOriginY_005a778a;
char g_szDamageStatusText_005a7790[0x48];
short g_nDamageSpriteX_005a77d8;
short g_nDamageSpriteY_005a77da;
signed char g_cDamageDisplayComponent_005a77dc;
signed char g_cDamageDisplayFrame_005a77dd;
signed char g_cDamagedComponentCount_005a77de;
int g_nDamageDisplayState_005a77e0;
int g_nDamageDisplayPhase_005a77e4;
unsigned char *g_pLaunchDoorShape_005a77e8;
unsigned int DAT_005a77ec;
unsigned char g_abPaletteTriplets_005a77f0[256][3];
InputDeviceSample g_stPreviousFlightInput_005a7af0;
short g_bMouseButtonEventQueued_005a7afc;
short g_bKeyboardEventQueued_005a7afe;
short g_bMouseMoveEventQueued_005a7b00;
char *g_pStartupArguments_005a7b10[30];
short g_bJoystickEventQueued_005a7b88;
short g_nArcadeStartupParameter_005a7b8a;
Viewport g_stTrainSimTitleDisplayViewport_005a7b90;
Viewport g_stTrainSimHighScoreBufferViewport_005a7bb0;
int g_nArcadeScore_005a7bc4;
TextContext g_stTrainSimTextContext_005a7bd0;
Viewport g_stTrainSimHighScoreDisplayViewport_005a7bf0;
TextContext g_stTrainSimHighScoreTextContext_005a7c10;
short g_nArcadeTimeRemaining_005a7c2c;
HighScoreEntry g_aHighScoreEntries_005a7c30[6];
int g_nArcadeWaveBonus_005a7c50;
unsigned char *g_pSceneAnimationSceneData_005a7c54;
unsigned char *g_pSceneAnimationPrimaryShape_005a7c58;
unsigned char *g_pSceneAnimationTextData_005a7c5c;
unsigned char *g_pSceneAnimationPacket_005a7c60;
SceneAnimationObject *g_pSceneAnimationObjects_005a7c64;
short g_nSceneAnimationWaitFrames_005a7c68;
unsigned char *g_pSceneAnimationDefinitions_005a7c6c;
unsigned char *g_pSceneAnimationSecondaryShape_005a7c70;
signed char g_cCockpitLogicalFile_005a7c74;
unsigned char *g_apCockpitShapes_005a7c80[4];
unsigned char *g_pCinematicViewBackdrop_005a7c90;
unsigned char *g_pRearViewBackdrop_005a7c94;
int g_bPlayerDestroyed_005a7c98;
short g_nPlayerKillCount_005a7c9c;
short g_asCollisionTime_005a7ca0[10];
int g_nWingmanKilledThisMission_005a7cb4;
short g_nWingmanKillCount_005a7cb8;
short g_nLastFoundShip_005a7cba;
short g_asCollisionPartner_005a7cc0[10];
int g_nMemoryConfiguration_005a7cd4;
unsigned int g_dwOriginalFreeMemory_005a7cd8;
unsigned char *g_pMouseCursorResource_005a7cdc;
int g_nAvailableGameMemory_005a7ce0;
int g_nSceneResourceBudget_005a7ce4;
int g_nAfterburnerSoundDeadline_005a7ce8;
int DAT_005a7cec;
DiskFileRecord *g_pDiskFileRecords_005a7cf0;
Viewport g_stDiskPromptBackgroundViewport_005a7d00;
unsigned char g_abDiskPromptDriveState_005a7d20[2];
Viewport g_stDiskPromptViewport_005a7d40;
int g_bPointerMovedByKeyboard_005a7d54;
TextContext g_stDiskPromptTextContext_005a7d60;
unsigned int g_dwDiskPromptTopLeft_005a7d80;
unsigned int g_dwDiskPromptBottomRight_005a7d84;
short *g_pPaletteTransitionDirection_005a7d88;
short *g_pPaletteTransitionDelta_005a7d8c;
short g_nPaletteTransitionMaxDelta_005a7d90;
short *g_pPaletteTransitionAccumulator_005a7d94;
short g_nPaletteTransitionCountdown_005a7d98;
int DAT_005a7d9c;
signed char g_cCapitalShipLogicalFile_005a7da0;
FixedVector g_vNormalizedToTarget_005a7db0;
short g_nReleaseWeaponDisplayX_005a7dbc;
short g_nReleaseWeaponDisplayY_005a7dbe;
enum ObjectType g_eReleaseWeaponDisplayType_005a7dc0;
unsigned char *g_pScannerMarkerBackground_005a7dc4;
unsigned char g_bStickIndicatorFrame_005a7dc8;
short DAT_005a7dca;
short g_nPendingCockpitDamage_005a7dcc;
HudMessageSlot g_aHudMessageSlots_005a7dd0[2];
signed char g_cPreviousTargetObject_005a7df2;
int g_nSavedMouseCursorY_005a7df4;
int g_nSavedMouseCursorX_005a7df8;
char g_szComponentHitMessage_005a7e00[40];
short g_nTargetLockMarkerY_005a7e28;
short g_nCarrierMissionShipIndex_005a7e2a;
CockpitReadout g_aCockpitReadouts_005a7e30[6];
short g_nScannerCursorX_005a7e6c;
short g_nScannerCursorY_005a7e6e;
unsigned char g_abCockpitLightState_005a7e70[7];
short g_asScannerObjectY_005a7e80[11];
volatile short g_nCockpitExplosionX_005a7e98;
volatile short g_nCockpitExplosionY_005a7e9a;
short g_asScannerObjectX_005a7ea0[11];
unsigned char g_abCockpitLightGoal_005a7eb8[7];
int DAT_005a7ec0;
short g_asScannerBackgroundColour_005a7ed0[10];
unsigned int g_anCockpitDamageState_005a7ef0[4];
unsigned short DAT_005a7f00;
short g_nExternalViewAngle_005a7f06;
unsigned char *g_pTitleShape_005a7f08;
int g_nViewportAllocationCount_005a7f0c;
unsigned char *g_apViewportAllocations_005a7f10[128];
int g_nFreeViewportCalls_005a8110;
short DAT_005a8114;
short g_nMissionMedalScore_005a8116;
short g_nCampaignDataSet_005a8118;
short g_nNavMapMaximumX_005a812c;
short g_nNavMapMinimumX_005a812e;
unsigned short g_awNavObjectiveLabelIndex_005a8130[16];
short g_nNavMapMaximumY_005a8150;
short g_nNavMapCentreX_005a8152;
short g_nNavMapMinimumY_005a8154;
TextContext g_stNavMapTextContext_005a8160;
short g_nNavMapCentreY_005a817c;
TextContext g_stNavLabelTextContext_005a8180;
short g_nActiveInputDevice_005a819c;
int g_nJoystickVerticalDeadZone_005a81a0;
int g_nJoystickHorizontalDeadZone_005a81a4;
int g_nJoystickUpScale_005a81a8;
int g_nJoystickLeftScale_005a81ac;
int g_nJoystickMaximumX_005a81b0;
int g_nJoystickMaximumY_005a81b4;
int g_nJoystickMinimumX_005a81b8;
int g_nJoystickMinimumY_005a81bc;
int g_nJoystickRawX_005a81c0;
int g_nJoystickRawY_005a81c4;
int g_nJoystickVerticalRange_005a81c8;
int g_nJoystickHorizontalRange_005a81cc;
int g_nJoystickRightScale_005a81d0;
int g_nJoystickDownScale_005a81d4;
int g_nJoystickCentreY_005a81d8;
int g_nJoystickCentreX_005a81dc;
int g_nJoystickFailureValue_005a81e0;
InputDeviceSample g_aInputDeviceSamples_005a81f0[4];
unsigned char g_abMissionAuxData_005a8210[0x28];
unsigned char g_abSeriesAuxData_005a8240[0x28];
MissionObjectiveSource g_aMissionObjectiveSources_005a8270[16];
short g_nMissionEntryNavPoint_005a8690;
short g_nHomeMissionShipIndex_005a8692;
short g_nPlayerMissionShipIndex_005a8694;
short g_nInitialMissionShipIndices_005a8696[8];
short DAT_005a86a6;
CampaignDate *g_pCurrentCampaignDate_005a86a8;
CampaignDate *g_pElapsedCampaignDate_005a86ac;
signed char g_cObjectResourceLogicalFile_005a86b0;
Viewport *g_pScrambleViewport_005a86b4;
short g_nScrambleCanopyFrame_005a86b8;
unsigned char *g_pScrambleDetailShape_005a86bc;
short g_nScrambleCockpitScale_005a86c0;
short g_nScrambleCockpitDetailX_005a86c4;
short g_nScrambleCockpitDetailY_005a86c6;
short g_nFuneralCasketX_005a86c8;
short g_nFuneralCasketY_005a86ca;
short g_nScrambleShipDetailCount_005a86cc;
signed char g_acScrambleShipDetailIndices_005a86d0[24];
signed char g_cScrambleWalkTicks_005a86e8;
unsigned char *g_pConversationSpecialShape_005a86ec;
ShortPoint g_aFuneralParticles_005a86f0[7];
unsigned char *g_pScrambleBackgroundShape_005a870c;
short g_nFuneralParticleDistance_005a8710;
short g_nScrambleBackgroundY_005a8712;
short g_nScrambleBackgroundRightX_005a8714;
short g_nFuneralForegroundX_005a8718;
short g_nFuneralForegroundY_005a871a;
short g_nFuneralMainScale_005a871c;
short g_nFuneralRifleFrame_005a871e;
short g_nFuneralBaseX_005a8720;
short g_nFuneralBaseY_005a8722;
short g_nScrambleShipX_005a8724;
short g_nScrambleShipY_005a8726;
short g_nFuneralParticleScale_005a8728;
unsigned char *g_pScrambleHangarShape_005a872c;
unsigned char *g_pScrambleCockpitShape_005a8730;
short g_nScrambleWalkerY_005a8734;
short g_nScrambleCanopyOffset_005a8736;
short g_nFuneralMainDistance_005a8738;
signed char g_bScrambleCanopyClosed_005a873a;
short g_nFuneralGuardFrame_005a873c;
short g_nScrambleOverlayX_005a8740;
short g_nScrambleOverlayY_005a8742;
unsigned char *g_pScrambleOverlayShape_005a8744;
signed char g_cScrambleWalkerPair_005a8748;
unsigned char *g_pScrambleCanopyShape_005a874c;
unsigned char *g_pScrambleShipShape_005a8750;
short g_nTalkingHeadFaceX_005a8754;
short g_nTalkingHeadFaceY_005a8756;
short g_nTalkingHeadMouthY_005a8758;
short g_nTalkingHeadMouthX_005a875a;
char DAT_005a8760[512];
unsigned char *g_pIntroFont_005a8960;
unsigned int DAT_005a8964;
JOYINFO g_aJoystickInfo_005a8970[2];
int g_nHostMouseMessageX_005a8990;
int g_nHostMouseMessageY_005a8994;
int g_bHostPrimaryMouseButton_005a8998;
int g_bHostSecondaryMouseButton_005a899c;
HWND DAT_005a89a0;
HANDLE DAT_005a89a4;
unsigned long g_dwGameClockBase_005a89a8;
GuardedAllocation *g_pGuardedAllocationTail_005a89ac;
char g_szMemoryUsage_005a89b0[0x80];
HDC DAT_005a8a30;
HCURSOR DAT_005a8a34;
unsigned int DAT_005a8a38;
int DAT_005a8a3c;
HINSTANCE DAT_005a8a40;
unsigned int DAT_005a8a44;
unsigned short DAT_005a8a50[0x300];

/* Embedded animation command streams at 0x00466030-0x00466400.  Each
 * command occupies the low word of a four-byte source record. */
static unsigned int g_anAnimExplosion0_00466030[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa000, 0
};
static unsigned int g_anAnimExplosion1_00466060[] = {
    0, 0x406, 1, 0x406, 2, 0x406, 0x406, 3, 0x406, 0x406,
    4, 0x406, 0x406, 0x406, 5, 0x406, 0x406, 0x406, 0x406,
    0x406, 0xa000, 0
};
static unsigned int g_anAnimExplosion2_004660b8[] = {
    0, 0x406, 1, 0x406, 2, 0x406, 3, 0x406, 4, 0x406, 5,
    6, 7, 0xa000
};
static unsigned int g_anAnimLaserSpark_004660f0[] = {
    0, 1, 2, 3, 4, 5, 0xa000, 0
};
static unsigned int g_anAnimBlueSpark_00466110[] = {
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int g_anAnimRedSpark_00466128[] = {
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int g_anAnimSparkTrail_00466140[] = {
    0, 1, 2, 3, 0xa000, 0
};
static unsigned int g_anAnimGirder_00466158[] = {
    0, 1, 2, 3, 4, 5, 0x9000, 0
};
static unsigned int g_anAnimTubing_00466178[] = {
    6, 7, 8, 9, 10, 11, 0x9000, 0
};
static unsigned int g_anAnimGlass_00466198[] = {
    12, 13, 14, 15, 16, 17, 18, 19, 0x92, 0x91, 0x90,
    0x8f, 0x8e, 0x8d, 0x8c, 20, 0x9000, 0
};
static unsigned int g_anAnimORing_004661e0[] = {
    21, 22, 23, 24, 25, 26, 0x9000, 0
};
static unsigned int g_anAnimPipe_00466200[] = {
    27, 28, 29, 30, 31, 32, 0x9000, 0
};
static unsigned int g_anAnimMetalSheet_00466220[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int g_anAnimWing_00466258[] = {
    13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
    27, 28, 0x9000, 0
};
static unsigned int g_anAnimMine_004662a0[] = {
    0, 1, 2, 0x41, 0x9000, 0, 0, 1, 2, 3, 4, 5, 0x9000, 0
};
static unsigned int g_anAnimAsteroidForward_004662d8[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0x9000, 0
};
static unsigned int g_anAnimAsteroidShortForward_00466318[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};
static unsigned int g_anAnimAsteroidReverse_00466350[] = {
    13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000, 0
};
static unsigned int g_anAnimAsteroidShortReverse_00466390[] = {
    12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0x9000
};
static unsigned int g_anAnimEjectedPilot_004663c8[14] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0x9000
};

/* Child/hardpoint offsets at 0x004682F0. */
const ShortVector g_aChildOffsets_004682f0[35] = {
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
    { 0, 0, 500 }, { -200, 0, 250 }
};

const ShortPoint g_aWeaponDisplayPositions_00468440[10] = {
    {64, 28}, {8, 28}, {45, 25}, {27, 25}, {36, 23},
    {20, 32}, {27, 24}, {31, 31}, {36, 22}, {41, 31}
};

const ShortPoint g_aWeaponDisplayOrigins_004684c0[5] = {
    {0, 16}, {0, 16}, {0, 16}, {0, 16}, {0, 16}
};

ObjectTypeData g_aObjectTypeData_00466458[OBJECT_TYPE_COUNT] = {
    /* 0: Hornet */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x00],
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
        &g_aszObjectTypeDisplayNames_004684d4[0x08],
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
        &g_aszObjectTypeDisplayNames_004684d4[0x10],
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
    { &g_aszObjectTypeDisplayNames_004684d4[0x1c] }, /* 3: Raptor */
    { &g_aszObjectTypeDisplayNames_004684d4[0x24] }, /* 4: Venture */
    { &g_aszObjectTypeDisplayNames_004684d4[0x2c] }, /* 5: Dilligent */
    { &g_aszObjectTypeDisplayNames_004684d4[0x38] }, /* 6: Drayman */
    { &g_aszObjectTypeDisplayNames_004684d4[0x40] }, /* 7: Exeter */
    /* 8: Tiger's Claw */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x48],
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
    { &g_aszObjectTypeDisplayNames_004684d4[0x58] }, /* 9: Salthi */
    /* 10: Dralthi */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0x60],
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
        &g_aszObjectTypeDisplayNames_004684d4[0x68],
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
        &g_aszObjectTypeDisplayNames_004684d4[0x70],
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
    { &g_aszObjectTypeDisplayNames_004684d4[0x78] }, /* 13: Jalthi */
    { &g_aszObjectTypeDisplayNames_004684d4[0x80] }, /* 14: Spikeri */
    { &g_aszObjectTypeDisplayNames_004684d4[0x88] }, /* 15: Dorkir */
    { &g_aszObjectTypeDisplayNames_004684d4[0x90] }, /* 16: Lumbari */
    { &g_aszObjectTypeDisplayNames_004684d4[0x98] }, /* 17: Ralari */
    { &g_aszObjectTypeDisplayNames_004684d4[0xa0] }, /* 18: Fralthi */
    { &g_aszObjectTypeDisplayNames_004684d4[0xa8] }, /* 19: Snakeir */
    { &g_aszObjectTypeDisplayNames_004684d4[0xb0] }, /* 20: Sivar */
    { &g_aszObjectTypeDisplayNames_004684d4[0xb8] }, /* 21: Kilrathi base */
    { 0 }, /* 22: asteroid field */
    { 0 }, /* 23: mine field */
    /* 24: laser cannon bolt */
    {
        &g_aszObjectTypeDisplayNames_004684d4[0xc4],
        OBJECT_CLASS_PROJECTILE, 10, 0, 512, 7, 30, 0, 25, 0,
        160, 0, 0, 0, 0, 0, 0, 0, { 0 },
        0, 0, 0, 0, 0, 0, 0, 0
    },
    { &g_aszObjectTypeDisplayNames_004684d4[0xd4],
      OBJECT_CLASS_PROJECTILE, 10, 1, 832, 14, 20, 0, 40, 0,
      140, 0 }, /* 25: neutron particle gun */
    { &g_aszObjectTypeDisplayNames_004684d4[0xe0],
      OBJECT_CLASS_PROJECTILE, 10, 1, 768, 14, 25, 0, 30, 0,
      120, 0 }, /* 26: mass driver cannon */
    { &g_aszObjectTypeDisplayNames_004684d4[0xec],
      OBJECT_CLASS_PROJECTILE, 10, 0, 512, 7, 30, 0, 25, 0,
      160, 0 }, /* 27: turret bolt; resources alias laser */
    { &g_aszObjectTypeDisplayNames_004684d4[0xf0],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 28 */
    { &g_aszObjectTypeDisplayNames_004684d4[0xf8],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 29 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x104],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 30 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x110],
      OBJECT_CLASS_MISSILE, 10, 10, 512, 0, 180, 20, 20, 0,
      80, 60 }, /* 31 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x11c],
      OBJECT_CLASS_MISSILE, 20, 20, 768, 0, 240, 40, 40, 0,
      60, 40 }, /* 32 */
    { &g_aszObjectTypeDisplayNames_004684d4[0x120],
      OBJECT_CLASS_MINE, 20, 5, 768, 110, 120, 4, 100, 10000,
      20, 20, (unsigned char *)g_anAnimMine_004662a0,
      0, 0, 2, 2 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x12c],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidForward_004662d8,
      0, 0, 1, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x130],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidShortForward_00466318,
      0, 0, 1, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x134],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidReverse_00466350,
      0, 0, 1, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x138],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidShortReverse_00466390,
      0, 0, 1, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x13c],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidForward_004662d8,
      0, 0, 2, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x140],
      OBJECT_CLASS_ASTEROID, 100, 300, 640, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidShortForward_00466318,
      0, 0, 2, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x144],
      OBJECT_CLASS_DEBRIS, 10, 4, 192, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimAsteroidForward_004662d8,
      0, 0, 2, 13 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x148],
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimGirder_00466158,
      0, 0, 2, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x14c],
      OBJECT_CLASS_DEBRIS, 10, 1, 2048, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimTubing_00466178,
      0, 0, 2, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x150],
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimMetalSheet_00466220,
      0, 0, 1, 11 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x154],
      OBJECT_CLASS_DEBRIS, 20, 2, 1280, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimWing_00466258,
      0, 0, 1, 15 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x158],
      OBJECT_CLASS_DEBRIS, 20, 2, 768, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimGlass_00466198,
      0, 0, 1, 15 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x15c],
      OBJECT_CLASS_DEBRIS, 2, 1, 1792, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimORing_004661e0,
      0, 0, 1, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x160],
      OBJECT_CLASS_DEBRIS, 6, 1, 1536, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimPipe_00466200,
      0, 0, 1, 5 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x164],
      OBJECT_CLASS_EXPLOSION, 0, 0, 768, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)g_anAnimExplosion0_00466030,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x168],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)g_anAnimExplosion1_00466060,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x16c],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 6000,
      0, 0, (unsigned char *)g_anAnimExplosion2_004660b8,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x170],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimLaserSpark_004660f0,
      0, 0, 1, 0 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x174],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimRedSpark_00466128,
      0, 0, 2, 3 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x178],
      OBJECT_CLASS_EXPLOSION, 0, 0, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimBlueSpark_00466110,
      0, 0, 2, 3 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x17c],
      OBJECT_CLASS_EXPLOSION, 1, 1, 256, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimSparkTrail_00466140,
      0, 0, 2, 3 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x180],
      OBJECT_CLASS_FIXED_OBJECT, 0, 0, 256, 0, 0, 0, -1 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x184],
      OBJECT_CLASS_DEBRIS, 6, 1, 512, 0, 0, 0, -1, 0,
      0, 0, (unsigned char *)g_anAnimEjectedPilot_004663c8,
      0, 0, 1, 12 },
    { &g_aszObjectTypeDisplayNames_004684d4[0x188],
      OBJECT_CLASS_EXPLOSION, 0, 0, 1024, 0, 0, 0, -1, 0,
      0, 0, 0, 0, 0, 1, 0 }
};
unsigned char g_bMessageSpeed_0046af68 = 2;
unsigned char g_abManeuverRerollChance_00465678[47] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 3, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0
};
void (*g_apShipAiManeuverHandlers_004656a8[47])(short, short) = {
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

const char *g_apszKilrathiAceNames_0046af80[4] = {
    &g_aszKilrathiAceNames_0046afd4[0x00],
    &g_aszKilrathiAceNames_0046afd4[0x08],
    &g_aszKilrathiAceNames_0046afd4[0x10],
    &g_aszKilrathiAceNames_0046afd4[0x18]
};

char *g_apszCommMenuText_0046af90[13] = {
    (char *)&g_aszCommMenuText_0046aff8[0x00],
    (char *)&g_aszCommMenuText_0046aff8[0x10],
    (char *)&g_aszCommMenuText_0046aff8[0x24],
    (char *)&g_aszCommMenuText_0046aff8[0x38],
    (char *)&g_aszCommMenuText_0046aff8[0x48],
    (char *)&g_aszCommMenuText_0046aff8[0x58],
    (char *)&g_aszCommMenuText_0046aff8[0x64],
    (char *)&g_aszCommMenuText_0046aff8[0x78],
    (char *)&g_aszCommMenuText_0046aff8[0x8c],
    (char *)&g_aszCommMenuText_0046aff8[0x9c],
    (char *)&g_aszCommMenuText_0046aff8[0xb0],
    (char *)&g_aszCommMenuText_0046aff8[0xc4],
    (char *)&g_aszCommMenuText_0046aff8[0xd8]
};

/* Ordered display names transcribed from the image.  The Sega CD inspector
 * proves that these index ObjectType, not a ship-only class enum. */
const char g_aszObjectTypeDisplayNames_004684d4[0x18c] =
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

const char g_aszKilrathiAceNames_0046afd4[32] =
    "Bhurak\0\0"
    "Dakhath\0"
    "Khajja\0\0"
    "Bakhtosh";

const char g_aszCommMenuText_0046aff8[0xe8] =
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

const char g_szIonDrive_0046a7c4[12] = "Ion drive";
const char g_szPowerPlant_0046a7d0[12] = "Power plant";
const char g_szShieldGenerator_0046a7dc[16] = "Shield gen'r";
const char g_szComputerSystem_0046a7ec[16] = "Computer sys";
const char g_szIntercomUnit_0046a7fc[16] = "InterCom unit";
const char g_szTargetTracking_0046a80c[16] = "Target track";
const char g_szAccelerationAbsorbers_0046a81c[16] = "Accel absorbers";
const char g_szEjectorSystem_0046a82c[16] = "Ejector system";
const char g_szRepairSystems_0046a83c[16] = "Repair systems";
const char g_szDamageOk_0046a84c[4] = "Ok";
const char g_szDamageLight_0046a850[8] = "Light";
const char g_szDamageModerate_0046a858[12] = "Moderate";
const char g_szDamageHeavy_0046a864[8] = "Heavy";
const char g_szDamageDestroyed_0046a86c[12] = "Destroyed";
const char g_szDamageReport_0046a878[16] = "DAMAGE REPORT";
const char g_szNoInternalDamage_0046a888[20] =
    "NO INTERNAL\n\nDAMAGE";
const char g_szDamagedUnitCountFormat_0046a89c[20] =
    "%d Unit%c Damaged";
const char g_szDamageStatusFormat_0046a8b0[16] = "%s\nDamage: %s";
const char g_szDamageStatusFormatHighRes_0046a8c0[16] =
    "%s\nDamage: %s";
