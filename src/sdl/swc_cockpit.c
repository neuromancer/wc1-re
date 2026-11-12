/* SDL presentation of the retail SWC cockpits.
 * CODE_13 and the expanded DATA/0 tables provide the layout and sprite
 * selection. WC1 supplies flight state, speed readouts and scanner maths.
 * QuickDraw Helvetica is supplied by the host text adapter. */
#include "wc1.h"
#include "swc.h"

#include <string.h>

char szObjectiveStatusMessage[28] = "Wait for ??????????????????";

typedef struct SwcCockpitFrame {
    SDL_Texture *texture;
    SDL_Rect bounds;
    SwcRasterImage image;
} SwcCockpitFrame;

typedef struct SwcCockpitShape {
    SwcCockpitFrame *frames;
    uint32_t count;
} SwcCockpitShape;

enum SwcCockpitInstrument {
    SWC_PLAYER_STATUS,
    SWC_WEAPON_ENERGY,
    SWC_FUEL,
    SWC_THROTTLE,
    SWC_SCANNER_GRID,
    SWC_SCANNER_CONTACTS,
    SWC_TARGET_SHIELDS,
    SWC_RETICLE,
    SWC_AUTOPILOT_LIGHT,
    SWC_DAMAGE_LIGHT,
    SWC_MISSILE_LIGHT,
    SWC_COCKPIT_DAMAGE,
    SWC_COCKPIT_SPARKS,
    SWC_PILOT_HAND,
    SWC_SPACE_GLARE,
    SWC_TARGET_BRACKETS,
    SWC_MISSILE_LOCK,
    SWC_COCKPIT_INSTRUMENT_COUNT
};

/* Retail expanded DATA/0: six cockpit rows, selected by initialize_cockpit.
 * Models 0..3 are Hornet/Rapier/Scimitar/Raptor, 4 simulator, 5 captured Dralthi. */
static int swcCockpitModel;
SDL_Point swcCockpitViewCenter = {160, 100};
CockpitScannerGeometry stSwcCockpitScanner = {159, 171, 148, 160, 169, 181};
static const CockpitScannerGeometry swcScannerLayouts[6] = {
    {159, 171, 148, 160, 169, 181},
    {159, 171, 148, 160, 169, 181},
    {159, 171, 148, 160, 170, 182},
    {159, 173, 148, 162, 169, 183},
    {159, 171, 148, 160, 169, 181},
    {163, 190, 152, 179, 174, 201},
};
/* DATA/0 +0x03e0. */
static const SDL_Point swcScannerOrigins[6] = {
    {139, 151}, {139, 151}, {139, 151}, {139, 153}, {139, 151}, {143, 170}
};
/* DATA/0 +0x0428. */
static const SDL_Point swcDamageLightOrigins[6] = {
    {156, 45}, {157, 30}, {156, 45}, {157, 44}, {156, 45}, {0, 0}
};
/* DATA/0 +0x06b6. */
static const SDL_Point swcActualSpeedOrigins[6] = {
    {96, 32}, {103, 47}, {105, 162}, {64, 19}, {96, 32}, {99, 17}
};
/* DATA/0 +0x06e6. */
static const SDL_Point swcCommandSpeedOrigins[6] = {
    {205, 32}, {200, 47}, {195, 162}, {239, 19}, {205, 32}, {201, 17}
};
/* DATA/0 +0x0776. */
static const SDL_Point swcStatusTextOrigins[6] = {
    {51, 84}, {41, 89}, {51, 84}, {51, 84}, {51, 84}, {51, 84}
};
/* DATA/0 +0x07a6. */
static const SDL_Point swcTargetHullOrigins[6] = {
    {230, 136}, {230, 132}, {230, 133}, {236, 134}, {230, 136}, {230, 136}
};
/* DATA/0 +0x07e2. */
static const SDL_Point swcViewCenters[6] = {
    {160, 100}, {160, 90}, {160, 80}, {160, 70}, {160, 100}, {160, 100}
};
/* DATA/0 +0x0812. */
static const SDL_Point swcPilotOrigins[6] = {
    {0, 0}, {133, 172}, {130, 175}, {0, 0}, {0, 0}, {0, 0}
};
/* DATA/0 +0x0842: radar, left VDU, right VDU. */
static const SDL_Point swcDamagePositions[6][3] = {
    {{142, 153}, {33, 134}, {231, 132}},
    {{141, 154}, {31, 132}, {230, 131}},
    {{0, 0}, {0, 0}, {0, 0}},
    {{141, 156}, {24, 135}, {235, 133}},
    {{142, 153}, {33, 134}, {231, 132}},
    {{146, 172}, {33, 134}, {231, 132}},
};
/* DATA/0 +0x0476/+0x05f6/+0x0536: left, right and communication VDUs. */
static const SDL_Point swcVduPolygons[6][3][4] = {
    {
        {{33, 134}, {90, 132}, {90, 188}, {33, 189}},
        {{231, 132}, {288, 134}, {288, 189}, {231, 188}},
        {{231, 132}, {287, 135}, {287, 188}, {231, 189}},
    },
    {
        {{34, 134}, {88, 133}, {89, 185}, {34, 186}},
        {{232, 132}, {288, 133}, {287, 186}, {232, 185}},
        {{232, 132}, {286, 134}, {286, 184}, {232, 185}},
    },
    {
        {{35, 134}, {89, 132}, {89, 184}, {35, 185}},
        {{232, 134}, {285, 136}, {285, 186}, {232, 185}},
        {{231, 134}, {285, 136}, {285, 184}, {231, 186}},
    },
    {
        {{25, 135}, {84, 133}, {84, 190}, {25, 191}},
        {{236, 133}, {295, 135}, {295, 191}, {236, 190}},
        {{234, 132}, {297, 134}, {297, 194}, {234, 196}},
    },
    {
        {{33, 134}, {90, 132}, {90, 188}, {33, 189}},
        {{231, 132}, {288, 134}, {288, 189}, {231, 188}},
        {{231, 132}, {287, 135}, {287, 188}, {231, 189}},
    },
    {
        {{34, 133}, {90, 132}, {90, 187}, {34, 187}},
        {{232, 133}, {287, 134}, {287, 187}, {232, 186}},
        {{231, 133}, {286, 134}, {286, 186}, {231, 189}},
    },
};

static SwcCockpitShape swcCockpitShapes[SWC_COCKPIT_INSTRUMENT_COUNT];
static SwcCockpitShape swcTargetShapes[OBJECT_TYPE_COUNT];
static SwcCockpitShape swcDeadVduShapes[2];
static SwcCockpitShape swcCommShapes[14];
static const SwcCmf *swcCockpitCmf;
static SDL_Renderer *swcCockpitRenderer;
static SDL_Texture *swcCockpitCanvas; /* Borrowed from the flight host. */
static SwcTextFont swcCockpitFont;
static SDL_Color swcCockpitColors[256];
static int swcCockpitDrawing;
static int swcCockpitDrawResult;
static short swcMalfunctionTicks[9];
static int swcVduDead[2];
static int swcVduStaticFrame[2];
static int swcAutopilotLight;
static int swcMissileLight;
static int swcDamageLight;
static int swcScannerFrames[10];
static int swcScannerEnemy = -1;
static int swcHostileArrow = -1;
static int swcDamageComponent;
static int swcDamageDisplayTicks;
static int swcSparkFrame = -1;
static int swcSparkRegion;
static short swcSparkStartFrame;
static SDL_Point swcSparkPosition;
static int swcIttsActive;
static SDL_Point swcIttsPosition = {0x7fff, 0};
static int swcPilotHand = -1;
static SDL_Point swcCockpitDrawOffset;
SDL_Point swcCockpitOffset;
int anSwcWeaponNoticeTicks[2]; /* SWC DATA/A5+0x4848/+0x484c. */
static int swcWeaponNoticeVisible[2];
int nSwcSpaceGlareTicks; /* SWC DATA/A5+0x8c62. */
int bSwcExplosionGlare; /* SWC DATA/A5+0x8c6a. */
static int swcSpaceGlareColor;
static SDL_Point swcSpaceGlareOffset;

typedef struct SwcCockpitMessage {
    char text[35];
    unsigned char color;
    int flash;
    int voice;
} SwcCockpitMessage;

static SwcCockpitMessage swcMessages[16];
static SwcCockpitMessage swcCurrentMessage;
static unsigned int swcMessageRead;
static unsigned int swcMessageCount;
static int swcMessageTicks;
static int swcMessageFlashTicks;
static int swcMessageVisible;
static int swcMessageActive;
static int swcMessageVoiceStarted;
static int swcCommSound = -1;
static int swcCommStarted;
static int swcCommTicks;
static int swcCommMouth;
static int swcPlayerVoice = -1;
static int swcPlayerVoiceStarted;
static int swcCommNavPoint = -1;
static int swcKilrathiVoice;
static signed char swcPreviousCommMessage[10];
static unsigned int swcPreviousCommTick[10];

short SdlSwcMessageShowing(void)
{
    /* CODE_06 message_showing +0x061e includes cockpit computer speech. */
    return swcCommSound >= 0 ||
        (swcMessageActive && swcCurrentMessage.voice >= 0);
}

/* CODE_06 Vocalize_Message +0x045c: player orders are snd/(482+command). */
void SdlSwcVocalizeMessage(int command)
{
    if (swcPlayerVoice == -1 && command >= 1 && command <= 12) {
        swcPlayerVoice = 482 + command;
        swcPlayerVoiceStarted = 0;
    }
}

/* CODE_06 Chosen_communicate_option +0x0276. The Actions menu's message
 * items 13..16 and SetGInputForChar's B, R, H and E keys (CODE_01 +0x3ec4,
 * dispatched by process_player_input +0x3e62..+0x3ea2) send one order each,
 * with no recipient menu: 0 break/attack, 1 request landing from a targeted
 * Tiger's Claw or else form on my wing, 2 help me out, 3 taunt the target.
 * The Mac reads the wingman's objective and the target's side before
 * checking either index; test the indices first. */
void SdlSwcCommunicate(int choice)
{
    short wingman = nYourWingman;
    short recipient = acShipTargetTable[0];
    short command = -1;

    if (nTrainSimActive)
        return;
    switch (choice) {
    case 0:
        if (wingman_dead() || acShipRating[wingman] == -1 || !any_enemy(0, 14000))
            break;
        command = aeShipObjectiveList[wingman] == OBJECTIVE_HOLD_FORMATION ? 7 : 1;
        recipient = wingman;
        break;
    case 1:
        if (recipient == -1)
            break;
        if (aeObjectTypeList[recipient] == OBJECT_TYPE_TIGERS_CLAW && !bLandingAuthorized) {
            command = 12;
            break;
        }
        if (wingman_dead() || aeShipObjectiveList[wingman] == OBJECTIVE_HOLD_FORMATION ||
            acShipRating[wingman] == -1)
            break;
        command = 9;
        recipient = wingman;
        break;
    case 2:
        if (wingman_dead() || acShipRating[wingman] == -1)
            break;
        recipient = wingman;
        if (any_enemy(0, 14000))
            command = 2;
        break;
    case 3:
        /* Random3DO(0,3) excludes its upper bound: taunts 4..6. */
        if (recipient != -1 && aeShipSide[recipient] == SIDE_KILRATHI && !unactive(recipient))
            command = 4 + RandomInRange(0, 2);
        break;
    }
    if (command == -1)
        return;
    SdlSwcVocalizeMessage(command);
    request(0, recipient, command);
}

/* CODE_06 vid_equiv +0x000c, talk_equiv +0x00c4 and real_vid_transmit
 * +0x049e. Enyo 1 uses the base-game faces; later expansion substitutions
 * stay outside the currently supported fresh-demo mission. */
void SdlSwcTransmit(short object, short message)
{
    /* DATA/A5+0x2cb4, eleven entries per speaker. Generic Kilrathi rows
       have ten lines; message 10 is not a valid entry for those speakers. */
    static const short sounds[25][11] = {
        {90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100},
        {101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111},
        {112, 113, 114, 115, 121, 122, 123, 129, 130, 131, 132},
        {138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148},
        {149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159},
        {160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170},
        {171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181},
        {182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192},
        {193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 0},
        {203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 0},
        {213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 0},
        {223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 0},
        {233, 234, 235, 236, 237, 238, 239, 245, 246, 247, 248},
        {249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259},
        {260, 261, 262, 263, 269, 270, 271, 272, 278, 279, 280},
        {281, 282, 283, 284, 290, 291, 292, 293, 299, 300, 301},
        {302, 303, 305, 306, 307, 308, 309, 310, 311, 312, 313},
        {314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324},
        {325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 0},
        {335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 0},
        {345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 0},
        {355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 0},
        {365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 0},
        {375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 0},
        {385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 0}
    };
    int rating;
    int face;
    int talk;
    int sound;

    if (object <= 0 || object >= 10 || message < 0 || message > 10 ||
        SdlSwcMessageShowing() || nCannedSceneMode || nTrainSimActive ||
        nCameraViewMode != 0 || aeShipSide[object] == SIDE_NEUTRAL)
        return;
    rating = acShipRating[object];
    if (rating == -1) {
        face = aeShipSide[object] == SIDE_IMPERIAL ? 12 : 13;
        talk = face == 12 || swcKilrathiVoice == 7 ? face : 18 + swcKilrathiVoice;
    } else {
        face = rating - (aeShipSide[object] == SIDE_KILRATHI);
        talk = face;
    }
    if (face < 0 || face >= 14 || talk < 0 || talk >= 25)
        return;
    sound = sounds[talk][message];
    if (sound == 0)
        return;
    /* Paladin's rank-addressed lines, and the carrier's destination reply. */
    if ((face == 2 && (message == 3 || message == 6 || message == 10)) ||
        (face == 12 && message == 6))
        sound += stCampaignState.currentPilot->rank + 1;
    nCommSpeakerObject = object;
    nCommSpeakerRating = rating;
    nCommPortraitIndex = face;
    swcCommSound = sound;
    swcCommStarted = 0;
    swcCommTicks = 40;
    if (get_mode(1) != 6)
        push_mode(1, 6);
}

/* CODE_06 check_message +0x0640 and CODE_13 npc_communication +0x3d2e.
 * Playback completion controls portrait lifetime; paused audio retains it. */
static void SwcUpdateCommunication(void)
{
    int playing;
    int object;
    int message;

    if (swcCommNavPoint != nCurrentNavigationPoint) {
        swcCommNavPoint = nCurrentNavigationPoint;
        /* CODE_04 set_up_action_sphere +0x0cb6: Random3DO(0,8). */
        swcKilrathiVoice = RandomInRange(0, 7);
    }
    if (swcPlayerVoice >= 0) {
        playing = swcPlayerVoiceStarted ? SdlSwcSoundPlaying(swcPlayerVoice) :
            SdlPlaySwcSound(swcPlayerVoice, 255);
        if (playing > 0)
            swcPlayerVoiceStarted = 1;
        else if (playing < 0 || swcPlayerVoiceStarted)
            swcPlayerVoice = -1;
    }
    if (swcCommSound >= 0) {
        playing = swcCommStarted ? SdlSwcSoundPlaying(swcCommSound) :
            SdlPlaySwcSound(swcCommSound, 255);
        if (playing > 0)
            swcCommStarted = 1;
        if ((playing == 0 && swcCommStarted) ||
            (playing < 0 && --swcCommTicks <= 0) ||
            aeShipSide[nCommSpeakerObject] == SIDE_NEUTRAL) {
            SdlStopSwcSound(swcCommSound);
            swcCommSound = -1;
            nCommSpeakerObject = nCommSpeakerRating = nCommPortraitIndex = -1;
            if (get_mode(1) == 6)
                pop_mode(1);
        }
    }
    if (swcPlayerVoice >= 0 || nCannedSceneMode || nTrainSimActive)
        return;
    for (object = 1; object < 10 && !SdlSwcMessageShowing(); object++) {
        if (aeObjectClass[object] < OBJECT_CLASS_SHIP)
            continue;
        message = acWingmanMessageStatesArray[object];
        if (message == -1)
            continue;
        acWingmanMessageStatesArray[object] = -1;
        if (message == swcPreviousCommMessage[object] && (message == 4 || message == 10) &&
            (nTickCount60Hz - swcPreviousCommTick[object]) < 30U * 60U)
            continue;
        swcPreviousCommMessage[object] = message;
        swcPreviousCommTick[object] = nTickCount60Hz;
        SdlSwcTransmit(object, message);
    }
    if (nCommSpeakerObject == -1 && RandomBelowOrEqual(4999) > 4950) {
        for (object = 1; object < 10; object++) {
            if (aeObjectClass[object] >= OBJECT_CLASS_SHIP &&
                aeShipSide[object] == SIDE_KILRATHI &&
                (aeShipObjectiveList[object] == OBJECTIVE_ENGAGE_ENEMY ||
                 aeShipObjectiveList[object] == OBJECTIVE_DESTROY_SHIP) &&
                (acShipRating[object] != -1 || RandomBelowOrEqual(99) < 20)) {
                acWingmanMessageStatesArray[object] = RandomInRange(2, 4);
                break;
            }
        }
    }
}

/* SWC CODE_13 CockpitMessage +0x01aa: copied, 34-character messages in a
 * sixteen-entry ring; deduplicate queued voice IDs or unvoiced text. */
void SdlSwcCockpitMessage(const char *text, unsigned char color, int flash, int voice)
{
    unsigned int index;
    SwcCockpitMessage *message;

    for (index = 0; index < swcMessageCount; index++) {
        message = &swcMessages[(swcMessageRead + index) % 16];
        if ((voice >= 0 && message->voice == voice) ||
            (voice < 0 && strncmp(message->text, text, 34) == 0))
            return;
    }
    /* Bound host storage if more messages arrive than can be spoken. */
    if (swcMessageCount == 16)
        return;
    message = &swcMessages[(swcMessageRead + swcMessageCount++) % 16];
    SDL_strlcpy(message->text, text, sizeof(message->text));
    message->color = color == 0 ? 0xfd : color;
    message->flash = flash;
    message->voice = voice;
}

/* CODE_13 UpdateMessage +0x000c and CODE_06 check_message +0x0640.
 * Voices use snd/(79+voice); unvoiced text lasts forty simulation frames. */
static void SwcUpdateMessage(void)
{
    int playing;

    if (swcMessageActive) {
        if (swcMessageFlashTicks >= 0)
            swcMessageFlashTicks--;
        if (swcMessageTicks >= 0)
            swcMessageTicks--;
        if (swcCurrentMessage.flash && swcMessageFlashTicks < 0) {
            swcMessageFlashTicks = 20;
            swcMessageVisible = !swcMessageVisible;
        }
        if (swcCurrentMessage.voice >= 0) {
            playing = swcMessageVoiceStarted ?
                SdlSwcSoundPlaying(swcCurrentMessage.voice + 79) :
                SdlPlaySwcSound(swcCurrentMessage.voice + 79, 255);
            if (playing > 0) {
                swcMessageVoiceStarted = 1;
                return;
            }
            if (playing == 0 && !swcMessageVoiceStarted)
                return;
            if (playing < 0) {
                swcCurrentMessage.voice = -1;
                swcMessageTicks = 40;
            }
        }
        if (swcMessageTicks >= 0 || swcCommSound >= 0)
            return;
        swcMessageActive = 0;
    }
    if (swcMessageCount == 0 || swcCommSound >= 0)
        return;
    swcCurrentMessage = swcMessages[swcMessageRead];
    swcMessageRead = (swcMessageRead + 1) % 16;
    swcMessageCount--;
    swcMessageActive = 1;
    swcMessageVisible = 1;
    swcMessageVoiceStarted = 0;
    swcMessageTicks = swcCurrentMessage.voice < 0 ? 40 : 0;
    if (swcCurrentMessage.voice >= 0) {
        playing = SdlPlaySwcSound(swcCurrentMessage.voice + 79, 255);
        swcMessageVoiceStarted = playing > 0;
        if (playing < 0) {
            swcCurrentMessage.voice = -1;
            swcMessageTicks = 40;
        }
    }
}

unsigned char bSwcWaitingForEscort; /* SWC DATA/A5+0x46ec */

/* SWC CODE_13 +0x1732: auto_pilot_valid. The Mac checks hazards first,
 * then enemies, the objective-arrival escort latch, and destination range.
 * CockpitMessage's voice IDs 1..7 address the original snd bank. */
short SdlSwcAutopilotValid(short showReason)
{
    const char *reason = NULL;
    short ship;
    enum ObjectType type;
    int voice = -1;

    if (cMissionObjectiveCountValue == 0 || cCurrentObjective < 0 ||
        cCurrentObjective >= cMissionObjectiveCountValue)
        return 0;
    if (pActiveHazardField != NULL) {
        reason = "Hazard Near";
        voice = 3;
    } else if (kilrathi_near(0, 16000)) {
        reason = "Enemy Near";
        voice = 2;
    } else if (bSwcWaitingForEscort) {
        if (!showReason)
            return 0;
        /* +0x179a indexes the object-type array with this stored ship ID.
           Only types 6, 4, 7 and 5 have a wait message in the original. */
        ship = anShipMissionShips[0];
        if (ship >= 0 && ship < SPACE_OBJECT_COUNT) {
            type = aeObjectTypeList[ship];
            if (type == OBJECT_TYPE_DRAYMAN || type == OBJECT_TYPE_VENTURE ||
                type == OBJECT_TYPE_EXETER || type == OBJECT_TYPE_DILLIGENT) {
                SDL_snprintf(szObjectiveStatusMessage, sizeof(szObjectiveStatusMessage),
                             "Wait for %s", aObjectTypeData[type].displayName);
                voice = type == OBJECT_TYPE_DRAYMAN ? 4 :
                    type == OBJECT_TYPE_VENTURE ? 5 : type == OBJECT_TYPE_EXETER ? 6 : 7;
                SdlSwcCockpitMessage(szObjectiveStatusMessage, 0x84, 1, voice);
            }
        }
        return 0;
    } else if (distance_from_point(0,
                   &aMissionObjectivesArray[cCurrentObjective].position) < 8000) {
        reason = "Already Near";
        voice = 1;
    }
    if (showReason && reason != NULL)
        SdlSwcCockpitMessage(reason, 0x84, 1, voice);
    return reason == NULL;
}

/* SWC CODE_13 +0x212e: flag_reached. Every call clears the escort latch;
 * reaching the selected objective ahead of its escort sets it again. */
void SdlSwcFlagReached(short objective, short reached)
{
    short carrierMissionShip = anShipMissionShips[0];
    short objectiveType = aMissionObjectivesArray[objective].type;
    short carrierObject = find_ship_index(carrierMissionShip);
    short markVisited = objective != cCurrentObjective;
    short advanceDestination = 0;

    bSwcWaitingForEscort = 0;
    if (objective == cCurrentObjective) {
        if (reached == 0 && escorting_a_ship() && carrierObject != -1 &&
            aMissionObjectivesArray[objective].index != carrierMissionShip) {
            if (objectiveType != 1 || aMissionShips[carrierMissionShip].state != 1) {
                SDL_snprintf(szObjectiveStatusMessage, sizeof(szObjectiveStatusMessage),
                             "Wait for %s", aObjectTypeData[aeObjectTypeList[carrierObject]].displayName);
                SdlSwcCockpitMessage(szObjectiveStatusMessage, 0x84, 1, -1);
                bSwcWaitingForEscort = 1;
            }
        } else {
            /* Unlike WC1, the Mac suppresses arrival text for type 1. */
            if (objectiveType != 1) {
                SdlSwcCockpitMessage(visited(objective) ? "Already Visited" : "Objective Reached",
                                    0x84, 0, visited(objective) ? -1 : 0);
            }
            advanceDestination = 1;
            markVisited = 1;
        }
    }
    if (objectiveType != 1 && markVisited) {
        if (!visited(objective) && carrierObject != -1 &&
            aMissionObjectivesArray[objective].index == carrierMissionShip &&
            aeObjectTypeList[carrierObject] != OBJECT_TYPE_TIGERS_CLAW)
            send_message(carrierObject, 6);
        flag_objective(objective, 1);
    }
    if (advanceDestination)
        set_next_destination();
}

static void SwcFreeCockpitShape(SwcCockpitShape *shape)
{
    uint32_t frame;

    if (shape->frames != NULL) {
        for (frame = 0; frame < shape->count; frame++) {
            SDL_DestroyTexture(shape->frames[frame].texture);
            SdlFreeSwcRasterImage(&shape->frames[frame].image);
        }
        SDL_free(shape->frames);
    }
    memset(shape, 0, sizeof(*shape));
}

static int SwcLoadCockpitShape(const SwcCmf *cmf, const char *type,
                               uint32_t chunk, uint32_t count,
                               SwcCockpitShape *shape)
{
    SwcBuffer set = {0};
    SwcFrame frame;
    SDL_Color maskColors[256];
    uint32_t image;
    int result = -1;

    /* MacDrawColor2 replaces every nonzero source pixel with the ink.
       A white alpha mask lets SDL modulation perform that exact recolor. */
    for (image = 0; image < SDL_arraysize(maskColors); image++)
        maskColors[image] = (SDL_Color){255, 255, 255, 255};
    if (shape->frames != NULL)
        return 0;
    if (SwcCMGetChunk(cmf, type, chunk, &set) != 0)
        return -1;
    if (SwcGetFrameCount(&set, &shape->count) != 0)
        goto done;
    if (shape->count != count) {
        SDL_SetError("Unexpected SWC cockpit frame count in %s/%u", type, chunk);
        goto done;
    }
    shape->frames = SDL_calloc(shape->count, sizeof(*shape->frames));
    if (shape->frames == NULL) {
        SDL_OutOfMemory();
        goto done;
    }
    for (image = 0; image < shape->count; image++) {
        if (SwcGetFramePtr(&set, image, &frame) != 0)
            goto done;
        shape->frames[image].bounds = (SDL_Rect){
            frame.x, frame.y, frame.width, frame.height
        };
        if (frame.width == 0 || frame.height == 0)
            continue;
        shape->frames[image].texture =
            SdlCreateSwcTexture(swcCockpitRenderer, &set, image,
                shape == &swcCockpitShapes[SWC_SPACE_GLARE] ||
                shape == &swcCockpitShapes[SWC_TARGET_BRACKETS] ? maskColors : swcCockpitColors);
        if (shape->frames[image].texture == NULL ||
            SdlLoadSwcRasterImage(&set, image, &shape->frames[image].image) != 0)
            goto done;
    }
    result = 0;
done:
    SDL_free(set.data);
    if (result != 0)
        SwcFreeCockpitShape(shape);
    return result;
}

void SdlFreeSwcCockpit(void)
{
    size_t shape;

    for (shape = 0; shape < SDL_arraysize(swcCockpitShapes); shape++)
        SwcFreeCockpitShape(&swcCockpitShapes[shape]);
    for (shape = 0; shape < SDL_arraysize(swcTargetShapes); shape++)
        SwcFreeCockpitShape(&swcTargetShapes[shape]);
    for (shape = 0; shape < SDL_arraysize(swcDeadVduShapes); shape++)
        SwcFreeCockpitShape(&swcDeadVduShapes[shape]);
    for (shape = 0; shape < SDL_arraysize(swcCommShapes); shape++)
        SwcFreeCockpitShape(&swcCommShapes[shape]);
    SDL_DestroyTexture(swcCockpitFont.texture);
    memset(&swcCockpitFont, 0, sizeof(swcCockpitFont));
    swcCockpitRenderer = NULL;
    swcCockpitCanvas = NULL;
    swcCockpitCmf = NULL;
    swcCockpitDrawing = 0;
    memset(swcMalfunctionTicks, 0, sizeof(swcMalfunctionTicks));
    memset(swcVduDead, 0, sizeof(swcVduDead));
    memset(swcVduStaticFrame, 0, sizeof(swcVduStaticFrame));
    memset(swcScannerFrames, 0xff, sizeof(swcScannerFrames));
    swcAutopilotLight = swcMissileLight = swcDamageLight = 0;
    swcScannerEnemy = swcHostileArrow = -1;
    swcDamageComponent = swcDamageDisplayTicks = 0;
    swcSparkFrame = -1;
    swcIttsActive = 0;
    swcIttsPosition.x = 0x7fff;
    swcPilotHand = -1;
    swcCockpitOffset = (SDL_Point){0, 0};
    swcCockpitDrawOffset = (SDL_Point){0, 0};
    memset(anSwcWeaponNoticeTicks, 0, sizeof(anSwcWeaponNoticeTicks));
    memset(swcWeaponNoticeVisible, 0, sizeof(swcWeaponNoticeVisible));
    nSwcSpaceGlareTicks = bSwcExplosionGlare = swcSpaceGlareColor = 0;
    swcSpaceGlareOffset = (SDL_Point){0, 0};
    swcMessageRead = swcMessageCount = 0;
    swcMessageActive = swcMessageVisible = 0;
    swcMessageTicks = swcMessageFlashTicks = 0;
    swcMessageVoiceStarted = 0;
    swcCommSound = swcPlayerVoice = -1;
    swcCommStarted = swcPlayerVoiceStarted = swcCommTicks = swcCommMouth = 0;
    nCommSpeakerObject = nCommSpeakerRating = nCommPortraitIndex = -1;
    swcCommNavPoint = -1;
    swcKilrathiVoice = 0;
    memset(swcPreviousCommMessage, -1, sizeof(swcPreviousCommMessage));
    memset(swcPreviousCommTick, 0, sizeof(swcPreviousCommTick));
}

int SdlInitSwcCockpit(SDL_Renderer *renderer, const SwcCmf *cockpit,
                       const SwcCmf *space, const SDL_Color colors[256], SDL_Texture *canvas,
                       int model)
{
    /* Retail LoadShapeSet lists, DATA/0 +0x42f0/+0x4398. Bar maxima are
       DATA/0 +0x470/+0x7d6/+0x7dc; the captured fighter has a throttle hand. */
    static const uint32_t chunks[] = {10, 15, 16, 17, 18, 31, 9, 1, 12, 13, 14, 7, 6, 6, 19, 36, 5};
    uint32_t counts[] = {7, 11, 14, 14, 1, 13, 2, 14, 1, 1, 2, 3, 8, 16, 1, 4, 4};
    char chunkType[5];
    size_t index;
    int shared;
    int result = -1;

    if (model < 0 || model >= 6)
        return SDL_SetError("Invalid SWC cockpit model %d", model);
    SdlFreeSwcCockpit();
    swcCockpitModel = model;
    stSwcCockpitScanner = swcScannerLayouts[model];
    swcCockpitViewCenter = swcViewCenters[model];
    SDL_snprintf(chunkType, sizeof(chunkType), "PC%02d", model == 4 ? 0 : model);
    if (model != 0 && model != 4) {
        counts[SWC_WEAPON_ENERGY] = model == 5 ? 11 : 10;
        counts[SWC_FUEL] = counts[SWC_THROTTLE] = 15;
        counts[SWC_MISSILE_LIGHT] = 1;
        counts[SWC_PILOT_HAND] = model == 5 ? 24 : 16;
    }
    swcCockpitRenderer = renderer;
    swcCockpitCanvas = canvas;
    swcCockpitCmf = cockpit;
    memcpy(swcCockpitColors, colors, sizeof(swcCockpitColors));
    for (index = 0; index < SDL_arraysize(chunks); index++) {
        shared = index == SWC_SCANNER_CONTACTS || index == SWC_RETICLE ||
            index == SWC_COCKPIT_SPARKS || index == SWC_TARGET_BRACKETS ||
            index == SWC_MISSILE_LOCK;
        if (SwcLoadCockpitShape(shared ? space : cockpit, shared ? "CKPT" : chunkType,
                               chunks[index], counts[index], &swcCockpitShapes[index]) != 0)
            goto done;
    }
    /* CODE_03 init_vdus +0x0914: shared VDUS/1 and /2, four static frames. */
    for (index = 0; index < SDL_arraysize(swcDeadVduShapes); index++) {
        if (SwcLoadCockpitShape(space, "VDUS", (uint32_t)index + 1, 4,
                               &swcDeadVduShapes[index]) != 0)
            goto done;
    }
    if (SdlCreateSwcTextFont(renderer, &swcCockpitFont, 9) != 0)
        goto done;
    result = 0;
done:
    return result;
}

/* CODE_01 MacDraw2 +0x0ef4 adds frame x/y to the draw origin. Unlike the
   centred ship images, these offsets place each gauge in the cockpit. */
static void SwcDrawCockpitShape(SwcCockpitShape *shape, int frame, int x, int y)
{
    SDL_Rect destination;

    if (swcCockpitDrawResult != 0)
        return;
    if (frame < 0 || (uint32_t)frame >= shape->count) {
        swcCockpitDrawResult = SDL_SetError("Invalid SWC instrument frame %d", frame);
        return;
    }
    destination = shape->frames[frame].bounds;
    if (shape->frames[frame].texture == NULL)
        return;
    destination.x += x + swcCockpitDrawOffset.x;
    destination.y += y + swcCockpitDrawOffset.y;
    swcCockpitDrawResult = SDL_RenderCopy(swcCockpitRenderer,
        shape->frames[frame].texture, NULL, &destination);
}

static void SwcDrawInstrument(enum SwcCockpitInstrument instrument,
                               int frame, int x, int y)
{
    SwcDrawCockpitShape(&swcCockpitShapes[instrument], frame, x, y);
}

static void SwcDrawCockpitText(int x, int baseline, unsigned char color,
                                const char *text)
{
    if (swcCockpitDrawResult != 0)
        return;
    swcCockpitDrawResult = SdlDrawSwcProportionalText(swcCockpitRenderer, &swcCockpitFont,
        x + swcCockpitDrawOffset.x, baseline + swcCockpitDrawOffset.y,
        swcCockpitColors[color], text);
}

void SdlDrawSwcCockpitReadout(signed char slot, const char *text)
{
    char padded[16];
    SDL_Point position;

    /* Only the shared speed routine draws here. Other WC1 readout calls can
       occur while changing objectives, outside cockpit composition. */
    if (!swcCockpitDrawing || (slot != 2 && slot != 3))
        return;
    SDL_snprintf(padded, sizeof(padded), "%04d", SDL_atoi(text));
    position = slot == 3 ? swcActualSpeedOrigins[swcCockpitModel] :
                          swcCommandSpeedOrigins[swcCockpitModel];
    SwcDrawCockpitText(position.x, position.y, 0x84, padded);
}

static int SwcCockpitTarget(void)
{
    int target = acShipTargetTable[0];

    return target > 0 && target < 10 &&
        aeObjectClass[target] >= OBJECT_CLASS_SHIP &&
        aeSpecialManeuvers[target] != SPECIAL_MANEUVER_UNKNOWN_9 ? target : -1;
}

/* CODE_13 lock_off +0x3708. The Mac owns two sound IDs and a persistent
 * acquired flag; WC1's text-message and random-angle lock path is separate. */
void SdlSwcLockOff(void)
{
    if (nTargetLockCountdown >= 0) {
        bTargetLockReadoutDirty = 1;
        SdlStopSwcSound(21);
    } else {
        SdlStopSwcSound(22);
    }
    bTargetLockAcquired = 0;
    nTargetLockCountdown = -1;
}

/* CODE_13 target_locking +0x37c0, start/starting_lock +0x36b2/+0x36d6
 * and decrement_lock_time +0x3752. WC2 cockpt.c (0x43c048) supplies the
 * common structure; SWC retains 18/32 updates, its sounds and malfunction
 * stall, without WC2's torpedo, target-camera or damage-eligibility branches. */
static void SwcUpdateTargetLock(void)
{
    const ShipWeaponSlot *weapons = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    int target = SwcCockpitTarget();
    enum ObjectType weapon;
    int duration;
    int x;
    int y;

    if (target == -1 || aeShipSide[target] == aeShipSide[0] ||
        acPlayerComponentDamageList[5] >= 4) {
        swcIttsActive = 0;
        SdlSwcLockOff();
        return;
    }
    x = asObjectScreenXList[target];
    if (x == (short)0x8001)
        return;
    if (nTargetLockCountdown < -1) {
        nTargetLockCountdown++;
    } else {
        y = asObjectScreenYArray[target];
        if (x * x + y * y > 3600 || nSelectedReleaseWeaponIndex < 0 ||
            nSelectedReleaseWeaponIndex >= aShipWeaponsTable[0][0]) {
            SdlSwcLockOff();
            return;
        }
        weapon = weapons[nSelectedReleaseWeaponIndex].type;
        if (weapon == OBJECT_TYPE_HEAT_SEEKING_MISSILE) {
            get_facing_range_from_object(0, target);
            if (nTargetFacingValue > -65) {
                SdlSwcLockOff();
                return;
            }
            duration = 18;
        } else if (weapon == OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE) {
            duration = 32;
        } else {
            SdlSwcLockOff();
            return;
        }
        if (nTargetLockCountdown == -1) {
            nTargetLockCountdown = duration;
            bTargetLockReadoutDirty = 0;
        } else if (nTargetLockCountdown > 0) {
            /* +0x3766 explicitly passes signed projected X * 2 / 5 as
             * play_sound's volume word. The SDL mixer bounds its volume. */
            SdlStopSwcSound(21);
            SdlPlaySwcSound(21, x * 2 / 5);
            if (!malf(5)) {
                nTargetLockCountdown--;
                bTargetLockAcquired = nTargetLockCountdown == 0;
            }
        }
    }
    if (bTargetLockAcquired)
        SdlPlaySwcSound(22, 255);
}

/* CODE_13 check_target +0x3aa4. Adapt WC2 cockpt.c's check_target
 * (0x43ddfc): the Mac retains a listed target or takes the nearest entry,
 * then enables lock mode automatically. It has no WC2 target camera or
 * eight-frame lock-malfunction check. build_your_target_list and its sort
 * already match CODE_13 +0x38f0 / CODE_05 +0x0b52 below 12,000 units. */
static void SwcCheckTarget(void)
{
    int oldTarget = SwcCockpitTarget();
    int index;
    short hasEnemy;

    if (acShipTargetTable[0] != oldTarget) {
        acShipTargetTable[0] = oldTarget;
        nTargetLockCountdown = -1;
    }
    if (oldTarget == -1 || (nTargetLockMode == 0 &&
        (asObjectScreenXList[oldTarget] == (short)0x8001 ||
         aeShipSide[oldTarget] == aeShipSide[0]))) {
        if (oldTarget == -1)
            nTargetLockMode = 0;
        build_your_target_list(&hasEnemy);
        if (cViableTargetCount == 0) {
            acShipTargetTable[0] = -1;
            if (oldTarget != -1)
                swcScannerEnemy = oldTarget;
        } else if (!(hasEnemy == 0 && oldTarget != -1 &&
                     aeShipSide[oldTarget] == aeShipSide[0] &&
                     (nTargetLockMode != 0 ||
                      asObjectScreenXList[oldTarget] != (short)0x8001))) {
            for (index = 0; index < cViableTargetCount &&
                 acViableTargetList[index] != oldTarget; index++)
                ;
            acShipTargetTable[0] = acViableTargetList[index % cViableTargetCount];
            nTargetLockMode = 1;
        }
        if (acShipTargetTable[0] != oldTarget) {
            if (oldTarget != -1 && acShipTargetTable[0] == -1)
                nTargetLockMode = 0;
            nTargetLockCountdown = -1;
        }
    }
    if (acShipTargetTable[0] == -1 || aeShipSide[acShipTargetTable[0]] == aeShipSide[0]) {
        nTargetLockMode = 0;
        swcIttsActive = 0;
    }
}

/* CODE_13 itts_range_check +0x2bce. WC2's
   HasInRangeGunForTargetLead (cockpt.c, 0x43ce8f) has the same loop. */
static int SwcIttsRangeCheck(short targetRange)
{
    const ShipWeaponSlot *weapons = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    int remaining = aShipWeaponsTable[0][0];

    while (remaining-- > 0) {
        const ObjectTypeData *type;

        if (weapons[remaining].disabled)
            continue;
        type = &aObjectTypeData[weapons[remaining].type];
        if (type->objectClass == OBJECT_CLASS_PROJECTILE &&
            type->maximumVelocity * type->lifetime > targetRange)
            return 1;
    }
    return 0;
}

static void SwcUpdateItts(void)
{
    /* CODE_13 position_itts_crosshair +0x2c48, overlay +0x2f56.
       Reuse WC2 UpdateTargetLeadIndicator (0x43cf5a)'s velocity prediction,
       retaining SWC's forward cone, 160-pixel projection and sprite origin. */
    const ShipWeaponSlot *weapons = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    int target = SwcCockpitTarget();
    int weapon;
    int projectileSpeed = 100;
    int interceptFrames;
    int distance;
    FixedVector combinedVelocity;
    FixedVector projectileVelocity;
    FixedVector targetOffset;
    FixedVector interceptPoint;
    FixedVector relative;
    FixedVector eyeRelative;

    swcIttsPosition.x = 0x7fff;
    if (target == -1 || nTargetLockMode == 0 ||
        aeShipSide[target] == aeShipSide[0] ||
        aeObjectClass[target] >= OBJECT_CLASS_CAPITAL_SHIP ||
        acPlayerComponentDamageList[5] >= 4) {
        swcIttsActive = 0;
        return;
    }
    if (!swcIttsActive) {
        if (!SwcIttsRangeCheck(distance_from_object(0, target)))
            return;
        swcIttsActive = 1;
        /* Mac set_global_message +0x3f4e only writes DATA+0x4712/+0x4716.
         * Neither global is read anywhere in this executable. */
    }
    weapon = aShipWeaponsTable[0][0];
    while (weapon-- > 0) {
        const ObjectTypeData *type;

        if (weapons[weapon].disabled)
            continue;
        type = &aObjectTypeData[weapons[weapon].type];
        if (type->objectClass == OBJECT_CLASS_PROJECTILE &&
            projectileSpeed < type->maximumVelocity)
            projectileSpeed = type->maximumVelocity;
    }
    vector_component_in_dir(&aShipVelocity[0], &aShipFwdVec[0],
                            &combinedVelocity);
    ScaleFixedVector(&aShipFwdVec[0], projectileSpeed << 8,
                     &projectileVelocity);
    AddFixedVectors(&projectileVelocity, &combinedVelocity, &combinedVelocity);
    projectileSpeed = (Vector_magnitude(&combinedVelocity) >> 8) + 1;
    interceptFrames = distance_from_object(0, target) / projectileSpeed;
    zero_vector(&targetOffset);
    if (interceptFrames > 0)
        ScaleFixedVector(&aShipVelocity[target], interceptFrames << 8,
                         &targetOffset);
    AddFixedVectors(&aShipPositionsArray[target], &targetOffset, &interceptPoint);
    if (!SwcIttsRangeCheck(distance_from_point(0, &interceptPoint))) {
        swcIttsActive = 0;
        return;
    }
    ComputeVectorDelta(&aShipPositionsArray[0], &interceptPoint, &relative);
    distance = Vector_magnitude(&relative);
    if (distance <= asObjectCollisionRadius[EYE_OBJECT] * 256)
        return;
    transform_to_objects_frame(&relative, &eyeRelative, EYE_OBJECT);
    if (eyeRelative.z < asObjectCollisionRadius[0] * 256 ||
        DivideFixed(eyeRelative.z, distance) < 0x94)
        return;
    swcIttsPosition.x = (int)(((int64_t)eyeRelative.x * 0xa000 /
                               eyeRelative.z) >> 8) + 0x91;
    swcIttsPosition.y = (int)(((int64_t)eyeRelative.y * 0xa000 /
                               eyeRelative.z) >> 8) + 0x55;
    if (swcIttsPosition.x < 10 || swcIttsPosition.x > 310 ||
        swcIttsPosition.y < 5 || swcIttsPosition.y > 230)
        swcIttsPosition.x = 0x7fff;
}

static void SwcUpdateCockpitScanner(void)
{
    FixedVector relative;
    FixedVector rotated;
    SphericalVector spherical;
    unsigned short color;
    short object;
    int frame;
    int target = SwcCockpitTarget();
    int enemyVisible = 0;
    int x;
    int y;

    memset(swcScannerFrames, 0xff, sizeof(swcScannerFrames));
    swcHostileArrow = -1;
    if (anCockpitDamageState[0])
        return;
    /* CODE_13 +0x285a retains the last hostile target, otherwise chooses
       the first enemy ship. Friendly targets never supply a threat arrow. */
    if (target != -1)
        swcScannerEnemy = target;
    if (swcScannerEnemy != -1 &&
        (aeObjectClass[swcScannerEnemy] < OBJECT_CLASS_SHIP ||
         aeSpecialManeuvers[swcScannerEnemy] == SPECIAL_MANEUVER_UNKNOWN_9 ||
         aeShipSide[swcScannerEnemy] == aeShipSide[0]))
        swcScannerEnemy = -1;
    for (object = 1; object < 10; object++) {
        if (swcScannerEnemy == -1 && aeObjectClass[object] >= OBJECT_CLASS_SHIP &&
            aeSpecialManeuvers[object] != SPECIAL_MANEUVER_UNKNOWN_9 &&
            aeShipSide[object] != aeShipSide[0])
            swcScannerEnemy = object;
        if (!get_color(object, &color))
            continue;
        /* WC1 get_color and SWC CODE_13 +0x27ac classify the same contacts.
           The Mac routine returns CKPT/31 frame indices instead of colors. */
        if (color == cRedColour)
            frame = 11;
        else if (color == cBlueColour)
            frame = 9;
        else if (color == cOrangeColour)
            frame = 5;
        else if (color == cDarkGreyColour)
            frame = 1;
        else if (color == cYellowColour)
            frame = 7;
        else
            frame = 3;
        /* CODE_13 +0x2940..+0x295c transforms each contact directly. The scene's
           projected-position cache can omit objects close to the player. */
        ComputeVectorDelta(&aShipPositionsArray[0], &aShipPositionsArray[object], &relative);
        transform_to_objects_frame(&relative, &rotated, EYE_OBJECT);
        rectangular_to_spherical(&rotated, &spherical);
        /* SWC's scanner cutoff is 30,000; WC1's draw_3d_scanner uses 60,000. */
        if (spherical.radius >= 30000 * 256)
            continue;
        rotational_pos_to_scanner_pos(object, &spherical);
        if (object == swcScannerEnemy)
            enemyVisible = 1;
        if (target == object) {
            if (!(nCurrentSpaceFrameValue & 1))
                continue;
            frame++;
        }
        swcScannerFrames[object] = frame;
    }
    if (enemyVisible) {
        /* Keep the threat sectors relative to each ship's scanner center. */
        x = asScannerObjectPositionX[swcScannerEnemy] - stSwcCockpitScanner.centerX + 159;
        y = asScannerObjectY[swcScannerEnemy] - stSwcCockpitScanner.centerY + 171;
        if (y < 169)
            swcHostileArrow = 0;
        if (x > 163)
            swcHostileArrow = 2;
        if (y > 175)
            swcHostileArrow = 4;
        if (x < 157)
            swcHostileArrow = 6;
        if (x > 163 && y < 169)
            swcHostileArrow = 1;
        if (x > 163 && y > 175)
            swcHostileArrow = 3;
        if (x < 157 && y > 175)
            swcHostileArrow = 5;
        if (x < 157 && y < 169)
            swcHostileArrow = 7;
    }
}

static void SwcDrawCockpitScanner(void)
{
    int object;

    for (object = 1; object < 10; object++) {
        if (swcScannerFrames[object] != -1)
            SwcDrawInstrument(SWC_SCANNER_CONTACTS, swcScannerFrames[object],
                             asScannerObjectPositionX[object], asScannerObjectY[object]);
    }
    if (nNavPointerObject != -1 && cCurrentObjective >= 0 &&
        cCurrentObjective < cMissionObjectiveCountValue) {
        SwcDrawInstrument(SWC_SCANNER_CONTACTS, 0,
                             asScannerObjectPositionX[10] - 3, asScannerObjectY[10] - 3);
    }
    SwcDrawInstrument(SWC_SCANNER_GRID, 0,
        swcScannerOrigins[swcCockpitModel].x, swcScannerOrigins[swcCockpitModel].y);
}

short SdlSwcComponentMalfunction(char component)
{
    int index = (unsigned char)component;
    int damage;

    if (index >= (int)SDL_arraysize(swcMalfunctionTicks))
        return 0;
    /* CODE_13 malf +0x1a28: damage-squared test and a countdown.
       SWC Random3DO excludes the upper bound, unlike WC1 RandomInRange. */
    if (swcMalfunctionTicks[index] > 0) {
        swcMalfunctionTicks[index]--;
        return 1;
    }
    damage = acPlayerComponentDamageList[index];
    if ((unsigned short)RandomInRange(0, 14) < damage * damage) {
        swcMalfunctionTicks[index] = RandomInRange(5, 9);
        return 1;
    }
    return 0;
}

void SdlSwcCockpitDamage(short region)
{
    /* CODE_13 cockpit_explosion +0x4222. A running spark sequence blocks
       another impact; an already damaged region can spark again later. */
    if (region < 0 || region >= 3 ||
        nCameraViewMode != 0 || nTrainSimActive != 0 || swcSparkFrame != -1)
        return;
    anCockpitDamageState[region] = 1;
    swcSparkRegion = region;
    swcSparkPosition = swcDamagePositions[swcCockpitModel][region];
    swcSparkFrame = 0;
    swcSparkStartFrame = nCurrentSpaceFrameValue;
    SdlPlaySwcSound(24, 255); /* CODE_13 malf_sound +0x19e4. */
}

void SdlUpdateSwcCockpit(void)
{
    /* CODE_09 handle_space_glare +0x0128, DATA/0 +0x1cb0/+0x1cba. */
    static const unsigned char impactColors[5] = {0, 253, 236, 212, 191};
    static const unsigned char explosionColors[9] = {0, 237, 192, 176, 161, 144, 140, 131, 129};
    int vdu;
    int mode;
    int count;
    int offset;
    int horizontal;
    int vertical;

    SwcCheckTarget();
    if (nCameraViewMode == 0)
        SwcUpdateTargetLock();
    swcSpaceGlareColor = 0;
    /* handle_space_glare precedes this frame's handle_cockpit_banking. */
    swcSpaceGlareOffset = swcCockpitOffset;
    if (nCameraViewMode <= 3 && nSwcSpaceGlareTicks > 0) {
        swcSpaceGlareColor = bSwcExplosionGlare ?
            explosionColors[SDL_min(nSwcSpaceGlareTicks, 8)] :
            impactColors[SDL_min(nSwcSpaceGlareTicks, 4)];
        if (--nSwcSpaceGlareTicks == 0)
            bSwcExplosionGlare = 0;
    }
    /* CODE_13 handle_cockpit_banking +0x18f2: move one pixel toward
       six times pitch/yaw velocity, clamped to +/-6 vertically, +/-8 horizontally. */
    horizontal = SDL_clamp(anObjectYawRotation[0] * 6, -8, 8);
    vertical = SDL_clamp(anObjPitchRot[0] * 6, -6, 6);
    swcCockpitOffset.x += (swcCockpitOffset.x < horizontal) -
        (swcCockpitOffset.x > horizontal);
    swcCockpitOffset.y += (swcCockpitOffset.y < vertical) -
        (swcCockpitOffset.y > vertical);
    /* determine_pilot_hand +0x14e2: horizontal input wins over vertical.
       The first sixteen PCnn/6 frames follow the steering input. */
    horizontal = SDL_clamp(stRotationInput.y, -8, 8) / 2;
    vertical = SDL_clamp(stRotationInput.x, -8, 8) / 2;
    swcPilotHand = horizontal > 0 ? horizontal - 1 : horizontal < 0 ? 3 - horizontal :
        vertical < 0 ? 11 - vertical : vertical > 0 ? 7 + vertical : -1;

    /* These are simulation updates, never render-frame random draws. */
    for (vdu = 0; vdu < 2; vdu++) {
        swcWeaponNoticeVisible[vdu] = anSwcWeaponNoticeTicks[vdu] > 0;
        if (swcWeaponNoticeVisible[vdu])
            anSwcWeaponNoticeTicks[vdu]--;
    }
    if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCountValue)
        set_objective_range(1);
    if (nCurrentSpaceFrameValue % 7 == 0) {
        swcAutopilotLight = cCurrentObjective >= 0 &&
            cCurrentObjective < cMissionObjectiveCountValue && auto_pilot_valid(0);
        swcMissileLight = missile_on_tail(0);
        swcDamageLight = calculate_damage_level() >= 3 &&
            aasShipShield[0][0] + aasShipShield[0][1] < 10;
    }
    if (swcDamageLight && nCurrentSpaceFrameValue % 30 == 0)
        SdlPlaySwcSound(32, 255); /* update_lights +0x04fc. */
    for (vdu = 0; vdu < 2; vdu++) {
        swcVduDead[vdu] = 0;
        if (anCockpitDamageState[vdu + 1])
            continue;
        mode = get_mode(vdu);
        swcVduDead[vdu] = mode == 0 ||
            (vdu == 0 && mode == 1 && malf(3)) ||
            (vdu == 1 && mode == 3 && malf(5)) ||
            (vdu == 1 && mode == 6 && malf(4));
        if (swcVduDead[vdu])
            swcVduStaticFrame[vdu] = RandomInRange(0, 2);
    }
    if (swcVduDead[0] || swcVduDead[1])
        SdlPlaySwcSound(23, 199); /* update_VDUs +0x0b4e. */
    else
        SdlStopSwcSound(23);
    /* Preserve shared damage notices in the Mac status queue. CODE_05
     * fire_missile +0x38b0 rejects an unlocked shot silently; CODE_13's
     * lock/ITTS paths never draw WC1's two VDU message slots. */
    if (aCockpitHudMessageSlotsTable[0].text != NULL && aCockpitHudMessageSlotsTable[0].flashCount != 0 &&
        strcmp(aCockpitHudMessageSlotsTable[0].text, "Need Lock") != 0)
        SdlSwcCockpitMessage(aCockpitHudMessageSlotsTable[0].text,
            aCockpitHudMessageSlotsTable[0].colour == cRedColour ? 0xd4 : 0x84, 1, -1);
    ClearHudGunReadouts();
    SwcUpdateMessage();
    SwcUpdateCommunication();
    /* CODE_13 update_vid_disp +0x0d7e: one-in-four mouth toggle. */
    if (swcCommSound >= 0 && get_mode(1) == 6 && !swcVduDead[1] &&
        !anCockpitDamageState[2] && RandomBelowOrEqual(3) == 0)
        swcCommMouth ^= 1;
    /* CODE_13 show_weapon_disp +0x0fc8 cycles nine damaged components. */
    if (!anCockpitDamageState[1] && !swcVduDead[0] && get_mode(0) == 1 &&
        --swcDamageDisplayTicks <= 0) {
        swcDamageDisplayTicks = 30;
        for (count = 0; count < 9; count++) {
            swcDamageComponent = (swcDamageComponent + 1) % 9;
            if (acPlayerComponentDamageList[swcDamageComponent] != 0)
                break;
        }
    }
    SwcUpdateCockpitScanner();
    if (nCameraViewMode == 0)
        SwcUpdateItts();
    /* CODE_13 explosion_draw +0x40a0: eight frames, occasional repeat
       around the damaged panel. Frame zero is retained for the impact tick. */
    if (swcSparkFrame >= 0 && swcSparkStartFrame != nCurrentSpaceFrameValue &&
        ++swcSparkFrame >= 8) {
        if ((unsigned short)RandomInRange(0, 9) < 3) {
            swcSparkFrame = 0;
            offset = RandomInRange(0, 9);
            swcSparkPosition.x = swcDamagePositions[swcCockpitModel][swcSparkRegion].x +
                RandomInRange(0, 19) - offset;
            offset = RandomInRange(0, 6);
            swcSparkPosition.y = swcDamagePositions[swcCockpitModel][swcSparkRegion].y +
                RandomInRange(0, 14) - offset;
        } else {
            swcSparkFrame = -1;
        }
    }
}

static void SwcDrawTargetStatus(void)
{
    int target = SwcCockpitTarget();
    SDL_Point origin = swcTargetHullOrigins[swcCockpitModel];
    enum ObjectType objectType;
    const ObjectTypeData *type;
    SwcCockpitShape *shape;
    char chunk[5];
    int capital;

    if (target == -1 || swcCockpitDrawResult != 0)
        return;
    objectType = aeObjectTypeList[target];
    type = &aObjectTypeData[objectType];
    capital = type->objectClass == OBJECT_CLASS_CAPITAL_SHIP;
    SDL_snprintf(chunk, sizeof(chunk), "%s%02d", capital ? "SH" : "ST", objectType);
    shape = &swcTargetShapes[objectType];
    /* CODE_03 cockpit resource loading: fighter STnn/2, capital SHnn/38. */
    swcCockpitDrawResult = SwcLoadCockpitShape(swcCockpitCmf, chunk,
                                               capital ? 38 : 2, 5, shape);
    SwcDrawCockpitShape(shape, 4, origin.x, origin.y);
    if (nCurrentSpaceFrameValue % 10 < 5) {
        if (aasShipArmor[target][0] < type->armorFront / 2)
            SwcDrawCockpitShape(shape, 0, origin.x, origin.y);
        if (aasShipArmor[target][3] < type->armorRight / 2)
            SwcDrawCockpitShape(shape, 1, origin.x, origin.y);
        if (aasShipArmor[target][2] < type->armorLeft / 2)
            SwcDrawCockpitShape(shape, 2, origin.x, origin.y);
        if (aasShipArmor[target][1] < type->armorRear / 2)
            SwcDrawCockpitShape(shape, 3, origin.x, origin.y);
    }
    if (type->shieldFore > 0 && aasShipShield[target][0] * 11 / type->shieldFore > 0)
        SwcDrawInstrument(SWC_TARGET_SHIELDS, 0, 0, 0);
    if (type->shieldAft > 0 && aasShipShield[target][1] * 11 / type->shieldAft > 0)
        SwcDrawInstrument(SWC_TARGET_SHIELDS, 1, 0, 0);
}

/* CODE_01 MacDrawcel2 maps the complete frame into the VDU quadrilateral. */
static void SwcDrawVduShape(int polygon, SwcCockpitShape *shape, int frame)
{
    const SDL_Point *corners = swcVduPolygons[swcCockpitModel][polygon];
    SDL_Point vertices[4];
    int corner;

    if (swcCockpitDrawResult != 0)
        return;
    for (corner = 0; corner < 4; corner++) {
        vertices[corner].x = (corners[corner].x + swcCockpitDrawOffset.x) * 256;
        vertices[corner].y = (corners[corner].y + swcCockpitDrawOffset.y) * 256;
    }
    swcCockpitDrawResult = SdlDrawSwcPolygon(swcCockpitRenderer, swcCockpitCanvas,
        &shape->frames[frame].image, swcCockpitColors, vertices);
}

static void SwcDrawPlayerStatus(void)
{
    const ObjectTypeData *type = &aObjectTypeData[aeObjectTypeList[0]];

    /* CODE_13 show_weapon_disp +0x0e54: hull silhouette, flashing armor
       below half strength, and fore/aft shield presence at 1/11 strength. */
    SwcDrawInstrument(SWC_PLAYER_STATUS, 2, 0, 0);
    if (nCurrentSpaceFrameValue % 10 < 5) {
        if (aasShipArmor[0][0] < type->armorFront / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 3, 0, 0);
        if (aasShipArmor[0][3] < type->armorRight / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 4, 0, 0);
        if (aasShipArmor[0][2] < type->armorLeft / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 5, 0, 0);
        if (aasShipArmor[0][1] < type->armorRear / 2)
            SwcDrawInstrument(SWC_PLAYER_STATUS, 6, 0, 0);
    }
    if (type->shieldFore > 0 && aasShipShield[0][0] * 11 / type->shieldFore > 0)
        SwcDrawInstrument(SWC_PLAYER_STATUS, 0, 0, 0);
    if (type->shieldAft > 0 && aasShipShield[0][1] * 11 / type->shieldAft > 0)
        SwcDrawInstrument(SWC_PLAYER_STATUS, 1, 0, 0);
}

int SdlDrawSwcCockpit(void)
{
    const ObjectTypeData *type = &aObjectTypeData[aeObjectTypeList[0]];
    TextContext *savedTextContext = pCurTextContext;
    int fuelCapacity;
    int frame;
    int maximum;

    swcCockpitDrawResult = 0;
    swcCockpitDrawOffset = swcCockpitOffset;
    /* CODE_13 update_lights +0x0408, DATA/0 +0x3d8. */
    if (swcAutopilotLight)
        SwcDrawInstrument(SWC_AUTOPILOT_LIGHT, 0, 0, 0);
    if (swcMissileLight)
        SwcDrawInstrument(SWC_MISSILE_LIGHT, 0, 0, 0);
    if (swcDamageLight && nCurrentSpaceFrameValue % 30 != 0)
        SwcDrawInstrument(SWC_DAMAGE_LIGHT, 0,
            swcDamageLightOrigins[swcCockpitModel].x, swcDamageLightOrigins[swcCockpitModel].y);
    if (!anCockpitDamageState[0])
        SwcDrawCockpitScanner();
    /* CODE_13 update_VDUs +0x0aa8. Physical damage suppresses its VDU;
       mode zero or a component malfunction replaces the live instruments.
       Navigation, damage and info pages are empty in the original demo. */
    if (!anCockpitDamageState[1]) {
        if (get_mode(0) == 0 || swcVduDead[0])
            SwcDrawVduShape(0, &swcDeadVduShapes[0], swcVduStaticFrame[0]);
        else if (get_mode(0) == 1)
            SwcDrawPlayerStatus();
    }
    if (!anCockpitDamageState[2]) {
        if (get_mode(1) == 0 || swcVduDead[1])
            SwcDrawVduShape(1, &swcDeadVduShapes[1], swcVduStaticFrame[1]);
        else if (get_mode(1) == 3)
            SwcDrawTargetStatus();
        else if (get_mode(1) == 6 && nCommPortraitIndex >= 0 && nCommPortraitIndex < 14) {
            SwcCockpitShape *portrait = &swcCommShapes[nCommPortraitIndex];

            swcCockpitDrawResult = SwcLoadCockpitShape(swcCockpitCmf, "WGMN",
                (uint32_t)nCommPortraitIndex + 1, 2, portrait);
            if (swcCockpitDrawResult == 0) {
                SwcDrawVduShape(2, portrait, 0);
                if (swcCommMouth)
                    SwcDrawVduShape(2, portrait, 1);
            }
        }
    }
    /* CODE_13 update_bars +0x0520. The original uses separate animated
       shape sets for energy, commanded throttle and fuel, not WC1's bars. */
    maximum = swcCockpitShapes[SWC_WEAPON_ENERGY].count - 1;
    frame = SDL_clamp(asShipWpnEnergyList[0] * maximum / 100, 0, maximum);
    SwcDrawInstrument(SWC_WEAPON_ENERGY, frame, 0, 0);
    maximum = swcCockpitShapes[SWC_THROTTLE].count - 1;
    frame = asShipMaximumSpeedArray[0] > 0 ?
        (anShipSpeed[0] >> 8) * maximum / asShipMaximumSpeedArray[0] : 0;
    SwcDrawInstrument(SWC_THROTTLE, SDL_clamp(frame, 0, maximum), 0, 0);
    fuelCapacity = type->lifetime;
    maximum = swcCockpitShapes[SWC_FUEL].count - 1;
    frame = anShipFuelsTable[0] < 0 || fuelCapacity <= 0 ? 0 :
        (int)((int64_t)anShipFuelsTable[0] * maximum / fuelCapacity) + 1;
    SwcDrawInstrument(SWC_FUEL, SDL_clamp(frame, 0, maximum), 0, 0);
    swcCockpitDrawing = 1;
    update_digital_readouts();
    swcCockpitDrawing = 0;
    SetTextContext(savedTextContext);
    return swcCockpitDrawResult;
}

int SdlDrawSwcCockpitDamage(void)
{
    int region;
    int frame;
    SDL_Point pilot = swcPilotOrigins[swcCockpitModel];

    swcCockpitDrawResult = 0;
    swcCockpitDrawOffset = swcCockpitOffset;
    /* CODE_13 place_damage_on_cockpit +0x41ae and explosion_draw +0x40a0
       run after instruments and text; damaged panels remain visible. */
    for (region = 0; region < 3; region++) {
        if (anCockpitDamageState[region])
            SwcDrawInstrument(SWC_COCKPIT_DAMAGE, region,
                swcDamagePositions[swcCockpitModel][region].x,
                swcDamagePositions[swcCockpitModel][region].y);
    }
    /* CODE_13 animate_pilot +0x1574 follows panel damage, before sparks. */
    if (swcPilotHand >= 0)
        SwcDrawInstrument(SWC_PILOT_HAND, swcPilotHand, pilot.x, pilot.y);
    /* Retail animate_pilot +0x15be: the Dralthi also animates the throttle hand. */
    if (swcCockpitModel == 5) {
        frame = asShipMaximumSpeedArray[0] > 0 ?
            (int)((int64_t)anShipSpeed[0] * 7 / (asShipMaximumSpeedArray[0] * 256)) : 0;
        SwcDrawInstrument(SWC_PILOT_HAND, 16 + SDL_clamp(frame, 0, 7), pilot.x, pilot.y);
    }
    if (swcSparkFrame >= 0)
        SwcDrawInstrument(SWC_COCKPIT_SPARKS, swcSparkFrame,
            swcSparkPosition.x - 20, swcSparkPosition.y - 20);
    return swcCockpitDrawResult;
}

/* CODE_09 draws glare before sorted space objects and the cockpit, using
 * the selected PCnn/19 mask at the banking origin. */
int SdlDrawSwcSpaceGlare(void)
{
    SDL_Color color;

    if (swcSpaceGlareColor == 0 || nCameraViewMode > 3)
        return 0;
    color = swcCockpitColors[swcSpaceGlareColor];
    swcCockpitDrawResult = SDL_SetTextureColorMod(
        swcCockpitShapes[SWC_SPACE_GLARE].frames[0].texture, color.r, color.g, color.b);
    swcCockpitDrawOffset = swcSpaceGlareOffset;
    SwcDrawInstrument(SWC_SPACE_GLARE, 0, 0, 0);
    return swcCockpitDrawResult;
}

/* CODE_13 draw_target_box +0x3114: colored CKPT/36 corners identify a
 * selected ship or current speaker. CKPT/5 is a separate missile-lock
 * overlay, closing inward by one pixel per countdown update. */
static void SwcDrawTargetBox(int object, const SDL_Rect *bounds,
                              unsigned char ink, int missileLock, SDL_Point center)
{
    SDL_Color color;
    SDL_Point corners[4];
    int index;
    int left;
    int top;
    int right;
    int bottom;
    int x;
    int y;

    if (object <= 0 || object >= 10 || bounds->w == 0 ||
        asObjectScreenXList[object] == (short)0x8001 || swcCockpitDrawResult != 0)
        return;
    if (ink == 0xd4 && aeShipSide[object] == aeShipSide[0])
        ink = 0xef;
    color = swcCockpitColors[ink];
    left = bounds->x - (ink == 0x81 ? 5 : 3);
    top = bounds->y - (ink == 0x81 ? 5 : 3);
    right = bounds->x + bounds->w - (ink == 0x81 ? 1 : 3);
    bottom = bounds->y + bounds->h - (ink == 0x81 ? 1 : 3);
    corners[0] = (SDL_Point){left, top};
    corners[1] = (SDL_Point){right, top};
    corners[2] = (SDL_Point){right, bottom};
    corners[3] = (SDL_Point){left, bottom};
    for (index = 0; index < 4 && swcCockpitDrawResult == 0; index++) {
        swcCockpitDrawResult = SDL_SetTextureColorMod(
            swcCockpitShapes[SWC_TARGET_BRACKETS].frames[index].texture,
            color.r, color.g, color.b);
        SwcDrawInstrument(SWC_TARGET_BRACKETS, index, corners[index].x, corners[index].y);
    }
    if (!missileLock || nTargetLockCountdown < 0 ||
        (bTargetLockAcquired && !(nCurrentSpaceFrameValue & 1)))
        return;
    x = center.x + asObjectScreenXList[object] - 14;
    y = center.y + asObjectScreenYArray[object] - 14;
    SwcDrawInstrument(SWC_MISSILE_LOCK, 0, x, y - nTargetLockCountdown);
    SwcDrawInstrument(SWC_MISSILE_LOCK, 1, x + nTargetLockCountdown, y);
    SwcDrawInstrument(SWC_MISSILE_LOCK, 2, x, y + nTargetLockCountdown);
    SwcDrawInstrument(SWC_MISSILE_LOCK, 3, x - nTargetLockCountdown, y);
}

int SdlDrawSwcSpaceHud(const SDL_Rect objectBounds[10], int cockpitless)
{
    int target = SwcCockpitTarget();
    int object = nNavPointerObject;
    int reticle = 0;
    SDL_Point center = cockpitless ? (SDL_Point){160, 100} : swcCockpitViewCenter;

    swcCockpitDrawResult = 0;
    swcCockpitDrawOffset = (SDL_Point){0, 0};
    /* CODE_13 overlay_head_up_display +0x2f56: ship-specific scene center,
       normal reticle at (center.x-16, center.y-4), including frame offsets.
       update_cockpit draws this layer before the opaque cockpit artwork. */
    if (swcIttsActive && target != -1 && nTargetLockMode != 0 &&
        swcIttsPosition.x != 0x7fff) {
        SwcDrawInstrument(SWC_RETICLE, 5, swcIttsPosition.x + center.x - 160,
            swcIttsPosition.y + center.y - 100);
        if (swcIttsPosition.x + 20 >= 155 && swcIttsPosition.x + 10 <= 163 &&
            swcIttsPosition.y + 20 >= 107 && swcIttsPosition.y + 10 <= 115)
            reticle = 4;
    }
    if (target != -1)
        SwcDrawTargetBox(target, &objectBounds[target], 0xd4, 1, center);
    if (SdlSwcMessageShowing() && nCommSpeakerObject > 0 && nCommSpeakerObject < 10)
        SwcDrawTargetBox(nCommSpeakerObject, &objectBounds[nCommSpeakerObject], 0x81, 0, center);
    swcCockpitDrawOffset = swcCockpitOffset;
    SwcDrawInstrument(SWC_RETICLE, reticle, center.x - 16, center.y - 4);
    if (!anCockpitDamageState[0] && swcHostileArrow != -1)
        SwcDrawInstrument(SWC_RETICLE, 6 + swcHostileArrow, center.x - 16, center.y - 4);
    swcCockpitDrawOffset = (SDL_Point){0, 0};
    if (object >= 0 && asObjectScreenXList[object] != (short)0x8001) {
        const SDL_Rect *bounds = &swcCockpitShapes[SWC_RETICLE].frames[3].bounds;

        /* The shared nav pointer is a centered space object, not an
           instrument placed by MacDraw2's sprite-origin offsets. */
        SwcDrawInstrument(SWC_RETICLE, 3,
            center.x + asObjectScreenXList[object] - bounds->x - bounds->w / 2,
            center.y + asObjectScreenYArray[object] - bounds->y - bounds->h / 2);
    }
    return swcCockpitDrawResult;
}

int SdlDrawSwcHud(void)
{
    /* CODE_13 update_status_text +0x102e: one proportional text cursor
       at DATA/0 +0x726, with eleven-pixel line advances. */
    static const char *const guns[] = {"Lasers", "Neutron", "Mass Drv"};
    static const char *const weapons[] = {
        "DF Missile", "HS Missile", "FF Missile", "IR Missile", "Torpedo", "Mine"
    };
    const ShipWeaponSlot *loadout = (ShipWeaponSlot *)&aShipWeaponsTable[0][1];
    enum ObjectType weapon = (enum ObjectType)-1;
    int target = SwcCockpitTarget();
    int gun = (int)eSelectedGunType;
    int index;
    int range;
    int textX = swcStatusTextOrigins[swcCockpitModel].x;
    int baseline = swcStatusTextOrigins[swcCockpitModel].y;
    int damage = SDL_clamp(acPlayerComponentDamageList[swcDamageComponent], 0, 4);
    char text[64];

    swcCockpitDrawResult = 0;
    swcCockpitDrawOffset = swcCockpitOffset;
    if (damage != 0) {
        /* DATA/0 +0x886/+0x89a/+0x8ae, original status names and colours. */
        static const unsigned char colors[5] = {0xc2, 0x84, 0x9f, 0xd4, 0xb0};
        static const char *const conditions[5] = {"Ok", "Light", "Moderate", "Heavy", "Destroyed"};
        static const char *const components[9] = {
            "Ion drv", "Pwr plant", "Shld gen'r", "Computer", "InterCom",
            "Targetting", "Accel", "Ejector", "Repair"
        };

        SDL_snprintf(text, sizeof(text), "%s %s", components[swcDamageComponent], conditions[damage]);
        SwcDrawCockpitText(textX, baseline, colors[damage], text);
        baseline += 11;
    }
    if (swcWeaponNoticeVisible[0]) {
        SwcDrawCockpitText(textX, baseline, 0x81, "GUNS: ");
        SwcDrawCockpitText(textX + SdlSwcTextWidth(&swcCockpitFont, "GUNS: "), baseline, 0xb0,
            gun == -1 ? "NONE" : (gun & 0x80) ? "FULL" :
            gun >= OBJECT_TYPE_LASER_CANNON && gun <= OBJECT_TYPE_MASS_DRIVER_CANNON ?
                guns[gun - OBJECT_TYPE_LASER_CANNON] : "NONE");
        baseline += 11;
    }
    if (swcWeaponNoticeVisible[1]) {
        if (nSelectedReleaseWeaponIndex >= 0 &&
            nSelectedReleaseWeaponIndex < aShipWeaponsTable[0][0])
            weapon = loadout[nSelectedReleaseWeaponIndex].type;
        SwcDrawCockpitText(textX, baseline, 0x81, "WEAP: ");
        SwcDrawCockpitText(textX + SdlSwcTextWidth(&swcCockpitFont, "WEAP: "), baseline, 0xb0,
            weapon >= OBJECT_TYPE_DUMB_FIRE_MISSILE && weapon <= OBJECT_TYPE_SPACE_MINE ?
                weapons[weapon - OBJECT_TYPE_DUMB_FIRE_MISSILE] : "NONE");
        baseline += 11;
    }
    if (swcMessageActive && swcMessageVisible && swcCurrentMessage.color != 0xfd)
        SwcDrawCockpitText(textX, baseline, swcCurrentMessage.color, swcCurrentMessage.text);

    if (target != -1) {
        /* CODE_13 update_digital_readouts +0x0660; ace strings are in
         * DATA/A5+0x2ca4. Named fighters display the pilot, other ships
         * display their class. Series 6 retains its Valkyrie override. */
        static const char *const aces[] = {"Bhurak", "Dakhath", "Khajja", "Bakhtosh"};
        const char *name = aObjectTypeData[aeObjectTypeList[target]].displayName;
        int rating = acShipRating[target];

        if (aeObjectClass[target] == OBJECT_CLASS_SHIP) {
            if (rating >= 0 && rating < 8 && apWingmanPilotsList[rating] != NULL) {
                name = apWingmanPilotsList[rating]->callsign;
                if (rating == 0 && nCampaignDataSet == 0 &&
                    stCampaignState.currentSeries == 6 && stCampaignState.currentMission == 0)
                    name = "Valkyrie";
            } else if (rating >= 9 && rating <= 12) {
                name = aces[rating - 9];
            }
        }
        SwcDrawCockpitText(200, 110, 0x81, "Target: ");
        SwcDrawCockpitText(200 + SdlSwcTextWidth(&swcCockpitFont, "Target: "),
            110, 0x84, name);
        range = distance_from_object(0, target);
        SwcDrawCockpitText(200, 121, 0x81, "Range:");
        SDL_snprintf(text, sizeof(text), " %d", range);
        SwcDrawCockpitText(200 + SdlSwcTextWidth(&swcCockpitFont, "Range:"),
            121, 0x84, range < 32000 ? text : " FAR");
    } else if (cCurrentObjective >= 0 && cCurrentObjective < cMissionObjectiveCountValue) {
        set_objective_range(0);
        /* Original destination/range layer at DATA/0 +0x6f6, including FAR
           at 32,000. Name visibility and distance remain WC1 core logic. */
        SwcDrawCockpitText(200, 94, 0x81, "Dest: ");
        SwcDrawCockpitText(200 + SdlSwcTextWidth(&swcCockpitFont, "Dest: "),
            94, 0x84, objective_name(cCurrentObjective));
        /* CODE_06 tprintf +0x2214 advances by font size + 2 (9 + 2). */
        SwcDrawCockpitText(200, 105, 0x81, "Range:");
        if (nCurObjectiveRangeValue < 32000)
            SDL_snprintf(text, sizeof(text), " %d", nCurObjectiveRangeValue);
        else
            SDL_strlcpy(text, " FAR", sizeof(text));
        SwcDrawCockpitText(200 + SdlSwcTextWidth(&swcCockpitFont, "Range:"), 105, 0x84, text);
    }
    if (IsCommChoiceMenuOpen()) {
        SDL_Rect panel = {10, 12, 300, 34 + 11 * SDL_min(nCommMenuChoiceCount, 7)};
        const char *heading = pszCommMenuHeading;
        int selection = SdlGetCommunicationMenuSelection();
        size_t length;

        /* WC1 owns recipients, command availability and request dispatch.
           Only the DOS text/VDU drawing is replaced by this SDL overlay. */
        if (swcCockpitDrawResult == 0)
            swcCockpitDrawResult = SDL_SetRenderDrawColor(swcCockpitRenderer, 0, 0, 0, 255);
        if (swcCockpitDrawResult == 0)
            swcCockpitDrawResult = SDL_RenderFillRect(swcCockpitRenderer, &panel);
        baseline = 23;
        while (heading != NULL && *heading != 0 && baseline < 45) {
            length = strcspn(heading, "\r\n");
            if (length != 0) {
                SDL_snprintf(text, sizeof(text), "%.*s", (int)SDL_min(length, sizeof(text) - 1), heading);
                SwcDrawCockpitText(16, baseline, 0x81, text);
                baseline += 11;
            }
            heading += length;
            while (*heading == '\r' || *heading == '\n')
                heading++;
        }
        for (index = 0; index < nCommMenuChoiceCount && index < 7; index++) {
            SDL_snprintf(text, sizeof(text), "%d. %s", index + 1, apszCommMenuChoiceTextTable[index]);
            SwcDrawCockpitText(16, 45 + 11 * index,
                index == selection ? 0x81 : 0x84, text);
        }
        nCommMenuReuseMode = 1;
    }
    if (bCurrentPlayerDestroyed || nArcadeState == 4)
        SwcDrawCockpitText(125, 65, 0xd4, "SHIP DESTROYED");
    return swcCockpitDrawResult;
}
