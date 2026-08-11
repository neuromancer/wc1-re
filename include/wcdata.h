/*
 *  Game data enumerations and on-disk record layouts.
 *
 *  Sources, in descending order of trust:
 *
 *  1. THIS BINARY.  Anything marked "verified" was read out of WC1.EXE itself,
 *     usually from an ordered string table.  Where the community documentation
 *     and the binary disagree, the binary wins and the difference is noted.
 *  2. WCMissionTools (../WCMissionTools), a MODULE/CAMP/BRIEFING parser built
 *     from the data files.  It names the fields the game loads; it does not
 *     know what the code calls them.
 *  3. WingCommanderArduinoBridge (../WingCommanderArduinoBridge), which reads
 *     the DOS build's memory live.  Its offsets are DOS-segment relative and do
 *     NOT apply here, but the field ORDER within the pilot record does, because
 *     both builds compile the same struct.
 *
 *  The MODULE record layouts below are the ON-DISK format.  The in-memory
 *  tables are different sizes -- the per-object table at 0x0046C97A has a
 *  0x36-byte stride against the 42-byte disk ship record, and the nav table has
 *  a 0x1F-byte stride against the 77-byte disk nav record -- so a disk field
 *  offset is a hint about what exists, never about where it lives at runtime.
 */
#ifndef WC1_DATA_H
#define WC1_DATA_H

#include <stddef.h>

/* --------------------------------------------------------------------------
 * Original object model.  BRAINS.C uses the fields class[] and type[]; the FM
 * Towns debug table supplies the ObjectClass order, while the Sega CD inspector
 * preserves the original ObjectType identifiers.  The Win32 type-name strings
 * at 0x004684D4 corroborate ordinals 0-32 ("Star post" is the display name for
 * the internal KILRATHI_BASE identifier).
 * -------------------------------------------------------------------------- */
enum ObjectClass {
    OBJECT_CLASS_NULL         = 0,
    OBJECT_CLASS_FUTURION     = 1,
    OBJECT_CLASS_STAR         = 2,
    OBJECT_CLASS_PLANET       = 3,
    OBJECT_CLASS_DUST         = 4,
    OBJECT_CLASS_EXPLOSION    = 5,
    OBJECT_CLASS_DEBRIS       = 6,
    OBJECT_CLASS_FIXED_OBJECT = 7,
    OBJECT_CLASS_PROJECTILE   = 8,
    OBJECT_CLASS_ASTEROID     = 9,
    OBJECT_CLASS_MINE         = 10,
    OBJECT_CLASS_MISSILE      = 11,
    OBJECT_CLASS_SHIP         = 12,
    OBJECT_CLASS_CAPITAL_SHIP = 13
};

enum ObjectType {
    OBJECT_TYPE_HORNET                    = 0,
    OBJECT_TYPE_RAPIER                    = 1,
    OBJECT_TYPE_SCIMITAR                  = 2,
    OBJECT_TYPE_RAPTOR                    = 3,
    OBJECT_TYPE_VENTURE                   = 4,
    OBJECT_TYPE_DILLIGENT                 = 5,  /* original misspelling */
    OBJECT_TYPE_DRAYMAN                   = 6,
    OBJECT_TYPE_EXETER                    = 7,
    OBJECT_TYPE_TIGERS_CLAW               = 8,
    OBJECT_TYPE_SALTHI                    = 9,
    OBJECT_TYPE_DRALTHI                   = 10,
    OBJECT_TYPE_KRANT                     = 11,
    OBJECT_TYPE_GRATHA                    = 12,
    OBJECT_TYPE_JALTHI                    = 13,
    OBJECT_TYPE_SPIKERI                   = 14,
    OBJECT_TYPE_DORKIR                    = 15,
    OBJECT_TYPE_LUMBARI                   = 16,
    OBJECT_TYPE_RALARI                    = 17,
    OBJECT_TYPE_FRALTHI                   = 18,
    OBJECT_TYPE_SNAKEIR                   = 19,
    OBJECT_TYPE_SIVAR                     = 20,
    OBJECT_TYPE_KILRATHI_BASE             = 21,
    OBJECT_TYPE_ASTEROID_FIELD            = 22,
    OBJECT_TYPE_MINE_FIELD                = 23,
    OBJECT_TYPE_LASER_CANNON              = 24,
    OBJECT_TYPE_NEUTRON_PARTICLE_GUN      = 25,
    OBJECT_TYPE_MASS_DRIVER_CANNON        = 26,
    OBJECT_TYPE_TURRET                    = 27,
    OBJECT_TYPE_DUMB_FIRE_MISSILE         = 28,
    OBJECT_TYPE_HEAT_SEEKING_MISSILE      = 29,
    OBJECT_TYPE_FF_MISSILE                = 30,
    OBJECT_TYPE_IMAGE_RECOGNITION_MISSILE = 31,
    OBJECT_TYPE_TORPEDO                   = 32,
    OBJECT_TYPE_SPACE_MINE                = 33,
    OBJECT_TYPE_ASTEROID1                 = 34,
    OBJECT_TYPE_ASTEROID2                 = 35,
    OBJECT_TYPE_ASTEROID3                 = 36,
    OBJECT_TYPE_ASTEROID4                 = 37,
    OBJECT_TYPE_ASTEROID5                 = 38,
    OBJECT_TYPE_ASTEROID6                 = 39,
    OBJECT_TYPE_ROCK_CHUNK                = 40,
    OBJECT_TYPE_DEBRIS_SHIP_GIRDER_CHUNK  = 41,
    OBJECT_TYPE_DEBRIS_SHIP_TUBING        = 42,
    OBJECT_TYPE_DEBRIS_METAL_SHEET        = 43,
    OBJECT_TYPE_DEBRIS_WING               = 44,
    OBJECT_TYPE_DEBRIS_GLASS              = 45,
    OBJECT_TYPE_DEBRIS_O_RING             = 46,
    OBJECT_TYPE_DEBRIS_PIPE               = 47,
    OBJECT_TYPE_EXPLOSION0                = 48,
    OBJECT_TYPE_EXPLOSION1                = 49,
    OBJECT_TYPE_EXPLOSION2                = 50,
    OBJECT_TYPE_LASER_SPARK               = 51,
    OBJECT_TYPE_RED_SPARK                 = 52,
    OBJECT_TYPE_BLUE_SPARK                = 53,
    OBJECT_TYPE_SPARK_TRAIL               = 54,
    OBJECT_TYPE_THRUSTERS                 = 55,
    OBJECT_TYPE_EJECTED_PILOT             = 56,
    OBJECT_TYPE_HYPERSPACE_JUMP_FLASH     = 57,
    OBJECT_TYPE_TYPES                     = 58, /* original enum terminator */
    OBJECT_TYPE_COUNT                     = OBJECT_TYPE_TYPES,
    OBJECT_TYPE_SPACE_DUST                = 59,
    OBJECT_TYPE_DEBRIS_DUST               = 60
};

/* BRAINS.C switch bodies line up exactly with these three Amiga debug tables. */
enum ShipMissionType {
    MISSION_TYPE_NONE              = -1,
    MISSION_TYPE_PATROL            = 0,
    MISSION_TYPE_ESCORT            = 1,
    MISSION_TYPE_STRIKE            = 2,
    MISSION_TYPE_DEFEND            = 3,
    MISSION_TYPE_WINGMAN           = 4,
    MISSION_TYPE_ROUT              = 5,
    MISSION_TYPE_GOTO_WARP         = 6,
    MISSION_TYPE_WARP_ARRIVE       = 7,
    MISSION_TYPE_CANNED_SEQUENCE   = 8,
    MISSION_TYPE_RENDEZVOUS        = 9,
    MISSION_TYPE_COME_HOME         = 10,
    MISSION_TYPE_BOGUS_AVOID_CRASH = 11
};

enum ShipObjective {
    OBJECTIVE_NONE            = -1,
    OBJECTIVE_NAV_POINT       = 0,
    OBJECTIVE_HOME_BASE       = 1,
    OBJECTIVE_GUARD           = 2,
    OBJECTIVE_REACH_SHIP      = 3,
    OBJECTIVE_DESTROY_SHIP    = 4,
    OBJECTIVE_WANDER          = 5,
    OBJECTIVE_ENGAGE_ENEMY    = 6,
    OBJECTIVE_EVADE_ENEMY     = 7,
    OBJECTIVE_HOLD_FORMATION  = 8,
    OBJECTIVE_BREAK_FORMATION = 9
};

enum ShipTactic {
    TACTIC_NONE            = -1,
    TACTIC_CRUISE          = 0,
    TACTIC_SIT_STILL       = 1,
    TACTIC_SCOUT_AHEAD     = 2,
    TACTIC_LAG_BEHIND      = 3,
    TACTIC_RAM             = 4,
    TACTIC_AVOID_OBJECT    = 5,
    TACTIC_WARP_OUT        = 6,
    TACTIC_WARP_IN         = 7,
    TACTIC_HEAD_HOME       = 8,
    TACTIC_CHASE           = 9,
    TACTIC_LOOK_OUT        = 10,
    TACTIC_APPROACH_TARGET = 11,
    TACTIC_TARGETTING      = 12, /* original spelling */
    TACTIC_SHAKE_ENEMY     = 13,
    TACTIC_ZIP_AWAY        = 14,
    TACTIC_RETREAT         = 15,
    TACTIC_SELF_DEFENSE    = 16,
    TACTIC_PICK_ATTACK     = 17
};

/* Side is a 32-bit enum in the Win32 image; do not collapse side[] to bytes. */
enum Side {
    SIDE_IMPERIAL = 0,
    SIDE_KILRATHI = 1,
    SIDE_NEUTRAL  = 2
};

/* Sega CD's inspector identifies this as the original Rating enum, not a pilot
 * id.  The four humorous tail identifiers are source placeholders; Win32 maps
 * its separate Bhurak/Dakhath/Khajja/Bakhtosh display-name table onto them. */
enum Rating {
    RATING_PROVINCIAL = 0,
    RATING_LINE       = 1,
    RATING_CRACK      = 2,
    RATING_ELITE      = 3,
    RATING_FANATICAL  = 4,
    RATING_ACE_SPIRIT = 5,
    RATING_ACE_HUNTER = 6,
    RATING_ACE_BOSSMAN = 7,
    RATING_ACE_ICEMAN = 8,
    RATING_ACE_ANGEL  = 9,
    RATING_ACE_PALADIN = 10,
    RATING_ACE_MANIAC = 11,
    RATING_ACE_KNIGHT = 12,
    RATING_ACE_PLAYER = 13,
    RATING_ACE_HEWEY  = 14,
    RATING_ACE_LEWEY  = 15,
    RATING_ACE_DEWEY  = 16,
    RATING_ACE_DAFFY  = 17
};

/* Amiga's live AI inspector preserves this dispatch-table order.  Win32 has
 * three additional slots (44-46); their identifiers are not present in any
 * recovered release, so they remain explicitly unknown. */
enum ShipManeuver {
    MANEUVER_NONE             = -1,
    MANEUVER_WARPING_IN       = 0,
    MANEUVER_WARPING_OUT      = 1,
    MANEUVER_VEER_AWAY        = 2,
    MANEUVER_DRIFT            = 3,
    MANEUVER_FULL_AHEAD       = 4,
    MANEUVER_THINKING         = 5,
    MANEUVER_RAM_MISSILE      = 6,
    MANEUVER_KICK_STOP        = 7,
    MANEUVER_TIGHT_LOOP       = 8,
    MANEUVER_HARD_BRAKE       = 9,
    MANEUVER_SIT_N_SPIN       = 10,
    MANEUVER_TURN_N_SPIN      = 11,
    MANEUVER_BURNOUT          = 12,
    MANEUVER_WABBLE           = 13,
    MANEUVER_ROLL_OVER        = 14,
    MANEUVER_HARD_TURN        = 15,
    MANEUVER_FISH_HOOK        = 16,
    MANEUVER_SPLIT_LEFT       = 17,
    MANEUVER_SIT_N_FIRE       = 18,
    MANEUVER_KICKIT           = 19,
    MANEUVER_TURN_N_KICK      = 20,
    MANEUVER_OUTA_HERE        = 21,
    MANEUVER_DROP_A_MINE      = 22,
    MANEUVER_SPLIT_RIGHT      = 23,
    MANEUVER_ZIG_ZAG          = 24,
    MANEUVER_GLOAT            = 25,
    MANEUVER_TAIL_FIRE        = 26,
    MANEUVER_TARGET_LASER     = 27,
    MANEUVER_TARGET_MISSILE   = 28,
    MANEUVER_STRAFE_ENEMY     = 29,
    MANEUVER_STRAFE_N_ROLL    = 30,
    MANEUVER_KILL_MISSILE     = 31,
    MANEUVER_SUICIDE_RUN      = 32,
    MANEUVER_ZIG_ZAG_PITCH    = 33,
    MANEUVER_SAFE_BRAKE       = 34,
    MANEUVER_TURN_N_FIRE      = 35,
    MANEUVER_GET_DISTANCE     = 36,
    MANEUVER_CORKSCREW        = 37,
    MANEUVER_INTERCEPT        = 38,
    MANEUVER_TRY2TAIL         = 39,
    MANEUVER_ZIP_PAST         = 40,
    MANEUVER_BUZZ_DEBRIS      = 41,
    MANEUVER_LINE_UP_DROP     = 42,
    MANEUVER_CHILL            = 43,
    MANEUVER_UNKNOWN_44       = 44,
    MANEUVER_UNKNOWN_45       = 45,
    MANEUVER_UNKNOWN_46       = 46
};

enum SpecialManeuver {
    SPECIAL_MANEUVER_NONE          = -1,
    SPECIAL_MANEUVER_NORMAL        = 0,
    SPECIAL_MANEUVER_AFTERBURNER   = 1,
    SPECIAL_MANEUVER_BOGUS_LOOP    = 2,
    SPECIAL_MANEUVER_SUPER_BRAKE   = 3,
    SPECIAL_MANEUVER_BOGUS_PUSH    = 4,
    SPECIAL_MANEUVER_KILL_ENGINES  = 5,
    SPECIAL_MANEUVER_STOP_DRIFT    = 6,
    SPECIAL_MANEUVER_LOST_CONTROL  = 7,
    SPECIAL_MANEUVER_BLOWING_UP    = 8,
    SPECIAL_MANEUVER_UNKNOWN_9     = 9
};

/* --------------------------------------------------------------------------
 * Comm menu.  VERIFIED: the pointer table at 0x0046AF90 indexes these six
 * strings, laid out consecutively from 0x0046AFF8.
 * -------------------------------------------------------------------------- */
enum CommMenuEntry {
    COMM_NEVER_MIND      = 0,     /* "Never mind..."     */
    COMM_ATTACK_TARGET   = 1,     /* "Attack my target!" */
    COMM_HELP_ME_OUT     = 2,     /* "Help me out here"  */
    COMM_RETURN_TO_BASE  = 3,     /* "Return to base."   */
    COMM_DIE_FURBALL     = 4,     /* "Die furball!"      */
    COMM_SLAG_OFF        = 5      /* "Slag off!"         */
};

/* --------------------------------------------------------------------------
 * MODULE on-disk records (WCMissionTools).  Reproduced as sizes and field
 * offsets rather than structs: the loader reads these into differently shaped
 * runtime tables, so declaring a C struct over them would assert a layout the
 * code does not use.
 *
 *   Nav record, 77 bytes
 *     0   30  name
 *     30   1  nav type (0 hidden, 1 visible, 2-5 follow-up waves)
 *     31   1  sphere radius, x1000
 *     32  12  X, Y, Z as 3-byte signed integers
 *     45   8  four (trigger type, target nav) pairs; 0 off, 1 on, 255 none
 *     53   2  preload ship class indices (at 53 and 55)
 *     57  20  ten ship indices, 2 bytes each, 255 empty
 *
 *   Ship record, 42 bytes
 *     0    1  ship class          22   6  rotation X, Y, Z (int16)
 *     2    1  allegiance          28   2  speed x10, or hazard-field radius
 *     4    1  leader index        30   1  AI level 0-4
 *     6    1  order               32   1  pilot id
 *     8    1  formation slot      39   1  secondary target index
 *     10  12  X, Y, Z position    40   1  formation group
 *                                 41   1  primary target index
 *
 *   Map / flight-plan record, 64 bytes
 *     0    1  objective icon (0 square, 1 home triangle, 2 cross,
 *                             3 green circle, 4 red circle)
 *     1    1  target nav index
 *     2   61  description text
 *
 * Coordinates are 3-byte little-endian signed: uint16 low word plus uint8 high
 * byte, minus 16777216 when the high byte is >= 128.
 * -------------------------------------------------------------------------- */
#define WC1_NAV_RECORD_BYTES   77
#define WC1_SHIP_RECORD_BYTES  42
#define WC1_MAP_RECORD_BYTES   64

typedef struct FixedVector {
    int x;
    int y;
    int z;
} FixedVector;

/* One of the 30 fireworks animated by TheEndFireWorks.  The 0x0C-byte stride
 * is explicit in the Win32 accesses at 0x0042D270-0x0042D384. */
typedef struct FireworkState {
    short frame;                    /* +0x00: -1 while the slot is free */
    short x;                        /* +0x02 */
    short y;                        /* +0x04 */
    short variant;                  /* +0x06: one of three 8-frame runs */
    int soundHandle;                /* +0x08 */
} FireworkState;

typedef char FireworkState_size_must_be_0x0c[
    sizeof(FireworkState) == 0x0c ? 1 : -1];

/* Runtime object-type records.  The 0x87-byte stride and the named fields are
 * fixed by accesses throughout the ship/object code; unknown slots retain
 * offset names until their purpose is established. */
#pragma pack(push, 1)
typedef struct ObjectTypeData {
    const char *displayName;          /* +0x00 */
    enum ObjectClass objectClass;     /* +0x04 */
    short collisionRadius;            /* +0x08 */
    short radarRadius;                /* +0x0A */
    short scale;                      /* +0x0C */
    short animationDelay;             /* +0x0E */
    short lifetime;                   /* +0x10 */
    short weaponDamage;               /* +0x12 */
    short damageCapacity;             /* +0x14 */
    short explosionDamage;            /* +0x16 */
    short maximumVelocity;            /* +0x18 */
    short cruiseVelocity;             /* +0x1A */
    unsigned char *animation;         /* +0x1C */
    int acceleration;                 /* +0x20 */
    short pitchRate;                  /* +0x24 */
    short yawRate;                    /* +0x26 */
    short rollRate;                   /* +0x28 */
    short afterburnerVelocity;        /* +0x2A */
    unsigned char weaponLoadout[0x47];/* +0x2C */
    short shieldFore;                 /* +0x73 */
    short shieldAft;                  /* +0x75 */
    short armorFront;                 /* +0x77 */
    short armorRear;                  /* +0x79 */
    short armorLeft;                  /* +0x7B */
    short armorRight;                 /* +0x7D */
    unsigned char *shapeSet;          /* +0x7F */
    unsigned char *shape;             /* +0x83 */
} ObjectTypeData;
#pragma pack(pop)

typedef char ObjectTypeData_size_must_be_0x87[
    sizeof(ObjectTypeData) == 0x87 ? 1 : -1];
typedef char ObjectTypeData_objectClass_must_be_at_0x04[
    offsetof(ObjectTypeData, objectClass) == 0x04 ? 1 : -1];
typedef char ObjectTypeData_weaponLoadout_must_be_at_0x2c[
    offsetof(ObjectTypeData, weaponLoadout) == 0x2c ? 1 : -1];

/* Each ship weapon entry is seven packed bytes inside its 0x47-byte loadout. */
#pragma pack(push, 1)
typedef struct ShipWeaponSlot {
    enum ObjectType type;             /* +0x00 */
    short hardpoint;                  /* +0x04 */
    signed char disabled;             /* +0x06 */
} ShipWeaponSlot;
#pragma pack(pop)

typedef struct ShortPoint {
    short x;
    short y;
} ShortPoint;

typedef struct CockpitBarDefinition {
    short direction;
    short left;
    short top;
    short right;
    short bottom;
    short length;
    short filledFrame;
    short emptyFrame;
} CockpitBarDefinition;

/* Compact three-axis offset used by the formation tables.  Unlike a
 * FixedVector, each component is a 16-bit distance. */
typedef struct ShortVector {
    short x;
    short y;
    short z;
} ShortVector;

/* Inclusive screen-space bounds used by the cockpit target-box renderer. */
typedef struct ShortRect {
    short left;
    short top;
    short right;
    short bottom;
} ShortRect;

/* One cockpit scanner's centre and inclusive cursor limits. */
typedef struct CockpitScannerGeometry {
    short centerX;
    short centerY;
    short minimumX;
    short minimumY;
    short maximumX;
    short maximumY;
} CockpitScannerGeometry;

/* The 144-word cockpit table at 0x0046E000.  Five ship cockpits use the
 * viewport/scanner/hand rectangles; the four readout origins also contain a
 * sixth sentinel entry used by the original table indexing. */
typedef struct CockpitLayout {
    short reserved[4];
    ShortPoint readoutOrigins[4][6];
    ShortRect leftVduBounds[5];
    ShortRect rightVduBounds[5];
    CockpitScannerGeometry scanner[5];
    short scannerPadding[2];
    ShortRect pilotHandBounds[5];
} CockpitLayout;

/* Header shared by the static and packet-backed view geometries.  fadeData is
 * a variable-length sequence in loaded packets, but four words are sufficient
 * for each built-in geometry at 0x0046DAB8. */
typedef struct ScreenViewportGeometry {
    short width;
    short height;
    short originX;
    short originY;
    short fadeData[4];
} ScreenViewportGeometry;

#pragma pack(push, 1)
/* Packet 6 in each PCSHIP file starts with a count followed by offsets to the
 * four cockpit view geometries. */
typedef struct ScreenViewportPacket {
    unsigned short geometryCount;
    short geometryOffsets[4];
} ScreenViewportPacket;

/* INSTALL.DAT uses fixed 16-byte records.  The last byte is the logical-file
 * id used to build the runtime lookup table. */
typedef struct DiskFileRecord {
    char name[13];
    signed char diskNumber;
    unsigned char fileClass;
    unsigned char logicalFile;
} DiskFileRecord;
#pragma pack(pop)

/* The leading offsets in every BRIEFING.xxx mission packet. */
typedef struct BriefingPacketHeader {
    unsigned int briefingScene;
    unsigned int briefingText;
    unsigned int debriefingScene;
    unsigned int debriefingText;
    unsigned int recRoomScene0;
    unsigned int recRoomText0;
    unsigned int recRoomScene2;
    unsigned int recRoomText2;
    unsigned int recRoomScene1;
    unsigned int recRoomText1;
} BriefingPacketHeader;

/* Caller-owned storage accepted by DecompressPacketSection. */
typedef struct PacketDecompressionWorkspace {
    unsigned char decoder[0x3020];
    unsigned char input[0x400];
} PacketDecompressionWorkspace;

/* Typed views over provisional oversized globals.  These preserve the current
 * backing allocations while giving recovered subregions proper field names. */
typedef struct ShipTimerStateView {
    unsigned char beforeCommunicator[0x40];
    signed char communicator[16];
    unsigned char beforeActionCount[0xd0];
    short actionCount[16];
} ShipTimerStateView;

typedef struct PilotRuntimeStateView {
    unsigned char unknown[0x10];
    int pilotLevel[12];
    short targetListRange[16];
} PilotRuntimeStateView;

typedef struct ShipSideRuntimeStateView {
    enum Side side[12];
    signed char aiCooldown[16];
} ShipSideRuntimeStateView;

typedef struct ShipNavRuntimeStateView {
    signed char navPointIndex[16];
    signed char turnInterval[16];
} ShipNavRuntimeStateView;

typedef struct ShipSpawnRuntimeStateView {
    signed char historyIndex[16];
    signed char spawnNavPoint[16];
} ShipSpawnRuntimeStateView;

typedef char CockpitLayout_size_must_be_0x120[
    sizeof(CockpitLayout) == 0x120 ? 1 : -1];
typedef char ScreenViewportGeometry_size_must_be_0x10[
    sizeof(ScreenViewportGeometry) == 0x10 ? 1 : -1];
typedef char DiskFileRecord_size_must_be_0x10[
    sizeof(DiskFileRecord) == 0x10 ? 1 : -1];
typedef char BriefingPacketHeader_size_must_be_0x28[
    sizeof(BriefingPacketHeader) == 0x28 ? 1 : -1];
typedef char ShipTimerStateView_communicator_must_be_at_0x40[
    offsetof(ShipTimerStateView, communicator) == 0x40 ? 1 : -1];
typedef char ShipTimerStateView_actionCount_must_be_at_0x120[
    offsetof(ShipTimerStateView, actionCount) == 0x120 ? 1 : -1];
typedef char PilotRuntimeStateView_pilotLevel_must_be_at_0x10[
    offsetof(PilotRuntimeStateView, pilotLevel) == 0x10 ? 1 : -1];
typedef char PilotRuntimeStateView_targetListRange_must_be_at_0x40[
    offsetof(PilotRuntimeStateView, targetListRange) == 0x40 ? 1 : -1];
typedef char ShipSideRuntimeStateView_aiCooldown_must_be_at_0x30[
    offsetof(ShipSideRuntimeStateView, aiCooldown) == 0x30 ? 1 : -1];
typedef char ShipNavRuntimeStateView_turnInterval_must_be_at_0x10[
    offsetof(ShipNavRuntimeStateView, turnInterval) == 0x10 ? 1 : -1];
typedef char ShipSpawnRuntimeStateView_spawnNavPoint_must_be_at_0x10[
    offsetof(ShipSpawnRuntimeStateView, spawnNavPoint) == 0x10 ? 1 : -1];

#pragma pack(push, 1)
typedef struct NavMapLabel {
    short x;
    short y;
    unsigned short colour;
    const char *text;
} NavMapLabel;
#pragma pack(pop)

typedef char NavMapLabel_size_must_be_0x0a[
    sizeof(NavMapLabel) == 0x0a ? 1 : -1];

/* Marker geometry and palette slots for one mission-objective type. */
typedef struct NavMapObjectiveStyle {
    short markerType;
    short markerSize;
    unsigned char *unvisitedColour;
    unsigned char *markerColour;
    unsigned char *labelColour;
} NavMapObjectiveStyle;

typedef char NavMapObjectiveStyle_size_must_be_0x10[
    sizeof(NavMapObjectiveStyle) == 0x10 ? 1 : -1];

/* One selectable title-screen image and its inclusive mouse hit bounds. */
typedef struct TitleMenuRegion {
    short frame;
    short left;
    short top;
    short right;
    short bottom;
} TitleMenuRegion;

/* One packet-backed pointer slot in a scene resource list. */
typedef struct PacketResourceDescriptor {
    unsigned char **resource;
    short logicalFile;
    short section;
} PacketResourceDescriptor;

/* Six packed TrainSim ranking records begin at 0x005A7C30.  The five-byte
 * stride is explicit in every getter, setter, and ranking-table scan. */
#pragma pack(push, 1)
typedef struct HighScoreEntry {
    signed char pilotIndex;
    unsigned int score;
} HighScoreEntry;
#pragma pack(pop)

/* Campaign pilot records are copied from the nine-entry template at
 * 0x00470108 into 0x005988D0.  The high-score code addresses the callsign at
 * +0x0E, while PostMission updates the mission and kill totals at +0x20 and
 * +0x22. */
#pragma pack(push, 1)
typedef struct PilotRecord {
    char name[14];                    /* +0x00 */
    char callsign[14];                /* +0x0E */
    short portrait;                   /* +0x1C */
    short rank;                       /* +0x1E */
    short missions;                   /* +0x20 */
    short kills;                      /* +0x22 */
    short personality;                /* +0x24 */
} PilotRecord;

typedef struct CampaignDate {
    short day;
    short year;
} CampaignDate;

/* One packed command in BRIEFING.000's scene stream.  The Win32 director
 * advances these records with a 13-byte stride retained from the DOS data. */
typedef struct ConversationSceneRecord {
    signed char shot;                 /* +0x00 */
    signed char textColour;           /* +0x01 */
    signed char talker;               /* +0x02 */
    short duration;                   /* +0x03 */
    short testsOffset;                /* +0x05 */
    short textOffset;                 /* +0x07 */
    short mouthAnimationOffset;       /* +0x09 */
    short faceAnimationOffset;        /* +0x0B */
} ConversationSceneRecord;

/* One actor in the briefing-room long shot.  The packed 18-byte records at
 * 0x0046E218 carry both the seated-body origin and the independently scaled
 * portrait animation used by DrawBriefingCharacter. */
typedef struct BriefingCharacterLayout {
    short bodyX;                      /* +0x00 */
    short bodyY;                      /* +0x02 */
    short portraitX;                  /* +0x04 */
    short portraitY;                  /* +0x06 */
    short scale;                      /* +0x08 */
    signed char visible;              /* +0x0A */
    signed char firstPortraitFrame;   /* +0x0B */
    signed char portraitFrameCount;   /* +0x0C */
    const signed char *animation;     /* +0x0D */
    signed char animationPhase;       /* +0x11, mutable scene state */
} BriefingCharacterLayout;

/* One runtime object in the packet-driven scene-animation interpreter.  The
 * packet stores a 0x36-byte array for each scene, followed by bytecode streams
 * that animate position, rotation, scale and frame. */
typedef struct SceneAnimationObject {
    short layer;                      /* +0x00: 0 tiles frames horizontally */
    short scriptOffset;               /* +0x02: offset into definition packet */
    signed char *scriptStart;         /* +0x04 */
    signed char *scriptCursor;        /* +0x08 */
    signed char *repeatCursor;        /* +0x0C */
    unsigned short goalFlags;         /* +0x10 */
    short delay;                      /* +0x12 */
    unsigned char *shape;             /* +0x14 */
    short x;                          /* +0x18 */
    short y;                          /* +0x1A */
    short rotation;                   /* +0x1C */
    short scale;                      /* +0x1E */
    short frame;                      /* +0x20 */
    short deltaX;                     /* +0x22 */
    short deltaY;                     /* +0x24 */
    short deltaRotation;              /* +0x26 */
    short deltaScale;                 /* +0x28 */
    short deltaFrame;                 /* +0x2A */
    short goalX;                      /* +0x2C */
    short goalY;                      /* +0x2E */
    short goalRotation;               /* +0x30 */
    short goalScale;                  /* +0x32 */
    short goalFrame;                  /* +0x34 */
} SceneAnimationObject;

/* SCRAMBLE.VGA uses five compact actors while the pilot approaches and
 * boards the selected fighter.  The two pointers are deliberately unaligned:
 * the retail Win32 table at 0x004657B0 has a packed 0x18-byte stride. */
typedef struct ScrambleAnimationActor {
    short x;                          /* +0x00 */
    short y;                          /* +0x02 */
    short deltaX;                     /* +0x04 */
    short deltaY;                     /* +0x06 */
    signed char baseFrame;            /* +0x08 */
    signed char animationFrame;       /* +0x09; -1 disables the script */
    signed char animationState;       /* +0x0A */
    const unsigned short *animation;  /* +0x0B */
    unsigned char *shape;             /* +0x0F */
    short angle;                      /* +0x13 */
    short scale;                      /* +0x15 */
    signed char flip;                 /* +0x17 */
} ScrambleAnimationActor;

/* Per-fighter decorative sprite positions used by the scramble/landing
 * renderer.  Each of the four player fighters owns 32 packed entries. */
typedef struct ScrambleShipDetail {
    signed char frame;                /* +0x00 */
    short x;                          /* +0x01 */
    short y;                          /* +0x03 */
} ScrambleShipDetail;

enum CampaignBadgeIndex {
    CAMPAIGN_BADGE_FIRST_MISSION = 2,
    CAMPAIGN_BADGE_SHIP_TYPE_BASE = 3,
    CAMPAIGN_BADGE_FIVE_KILLS = 7,
    CAMPAIGN_BADGE_TWENTY_FIVE_KILLS = 8,
    CAMPAIGN_BADGE_FIVE_MISSIONS = 9,
    CAMPAIGN_BADGE_TEN_MISSIONS = 10,
    CAMPAIGN_BADGE_FIFTEEN_MISSIONS = 11
};

/* The complete persistent campaign record copied by ResetCampaignData.  The
 * four bytes at +0x44 are the ace-state flags; the following eight bytes are
 * two dates, rather than a single twelve-byte flag array as the old placeholder
 * declaration implied. */
typedef struct CampaignState {
    PilotRecord *currentPilot;        /* +0x00 */
    enum ObjectType playerShipType;   /* +0x04 */
    unsigned char medals[5];          /* +0x08 */
    unsigned char badges[12];         /* +0x0D */
    signed char currentMission;       /* +0x19 */
    signed char currentSeries;        /* +0x1A */
    signed char seriesHistoryCount;   /* +0x1B */
    signed char seriesHistory[8];     /* +0x1C */
    int personalityDeathMission[8];   /* +0x24 */
    unsigned char aceFlags[4];        /* +0x44 */
    CampaignDate currentDate;         /* +0x48 */
    CampaignDate elapsedDate;         /* +0x4C */
    short promotionScore;             /* +0x50 */
    short missionScore;               /* +0x52 */
    short seriesScore;                /* +0x54 */
    short campaignIndex;              /* +0x56 */
} CampaignState;

/* SAVEGAME.WLD stores the two leading 32-bit campaign values and the eight
 * personality-death mission values as signed 16-bit quantities.  The remaining
 * fields retain their packed runtime order. */
typedef struct SaveGameDiskCampaignState {
    short currentPilot;                /* +0x00 */
    short playerShipType;              /* +0x02 */
    unsigned char medals[5];           /* +0x04 */
    unsigned char badges[12];          /* +0x09 */
    signed char currentMission;        /* +0x15 */
    signed char currentSeries;         /* +0x16 */
    signed char seriesHistoryCount;    /* +0x17 */
    signed char seriesHistory[8];      /* +0x18 */
    short personalityDeathMission[8];  /* +0x20 */
    unsigned char aceFlags[4];         /* +0x30 */
    CampaignDate currentDate;          /* +0x34 */
    CampaignDate elapsedDate;          /* +0x38 */
    short promotionScore;              /* +0x3C */
    short missionScore;                /* +0x3E */
    short seriesScore;                 /* +0x40 */
    short campaignIndex;               /* +0x42 */
} SaveGameDiskCampaignState;

/* One open packet section.  OpenPacketSection fills this record and the packet
 * reader advances position while leaving the containing data file open. */
typedef struct PacketSectionHandle {
    short file;
    short finalSection;
    short sectionCount;
    short compression;
    unsigned int dataOffset;
    unsigned int position;
    unsigned int dataSize;
} PacketSectionHandle;
#pragma pack(pop)

typedef char PilotRecord_size_must_be_0x26[
    sizeof(PilotRecord) == 0x26 ? 1 : -1];
typedef char CampaignState_size_must_be_0x58[
    sizeof(CampaignState) == 0x58 ? 1 : -1];
typedef char SaveGameDiskCampaignState_size_must_be_0x44[
    sizeof(SaveGameDiskCampaignState) == 0x44 ? 1 : -1];
typedef char ConversationSceneRecord_size_must_be_0x0d[
    sizeof(ConversationSceneRecord) == 0x0d ? 1 : -1];
typedef char BriefingCharacterLayout_size_must_be_0x12[
    sizeof(BriefingCharacterLayout) == 0x12 ? 1 : -1];
typedef char SceneAnimationObject_size_must_be_0x36[
    sizeof(SceneAnimationObject) == 0x36 ? 1 : -1];
typedef char ScrambleAnimationActor_size_must_be_0x18[
    sizeof(ScrambleAnimationActor) == 0x18 ? 1 : -1];
typedef char ScrambleShipDetail_size_must_be_0x05[
    sizeof(ScrambleShipDetail) == 0x05 ? 1 : -1];
typedef char PacketSectionHandle_size_must_be_0x14[
    sizeof(PacketSectionHandle) == 0x14 ? 1 : -1];

/* Polar form used by the original 3D orientation routines. */
typedef struct SphericalVector {
    int radius;
    short yaw;
    short pitch;
} SphericalVector;

/* Runtime asteroid/mine-field descriptor.  The packed 0x16-byte stride is
 * visible in the Win32 hazard-field scans and agrees with the Mac `hazar`
 * compilation unit. */
#pragma pack(push, 1)
typedef struct HazardField {
    enum ObjectType type;             /* +0x00 */
    FixedVector center;                /* +0x04 */
    short innerRadius;                 /* +0x10 */
    short outerRadius;                 /* +0x12 */
    short density;                     /* +0x14 */
} HazardField;
#pragma pack(pop)

/* Weighted pair in the retail maneuver-selection tables. */
typedef struct ManeuverChoice {
    signed char threshold;
    signed char primary;
    signed char secondary;
} ManeuverChoice;

/* Coordinates for the independently animated face and mouth layers in a
 * talking-head shape packet. */
typedef struct TalkingHeadOrigin {
    short faceX;
    short faceY;
    short mouthX;
    short mouthY;
} TalkingHeadOrigin;

/* The constellation renderer keeps ten static stars and sixteen horizontally
 * moving particles. */
typedef struct ConstellationStar {
    short x;
    short y;
    short frame;
} ConstellationStar;

typedef struct ConstellationParticle {
    short x;
    short y;
    short velocity;
    short frame;
} ConstellationParticle;

/* One of four optional planet sprites placed around a campaign
 * constellation.  The angles orient a temporary reference frame before the
 * object is positioned at the fixed constellation radius. */
typedef struct ConstellationObjectDefinition {
    short shapePacket;
    short yaw;
    short pitch;
    short roll;
} ConstellationObjectDefinition;

typedef char TalkingHeadOrigin_size_must_be_0x08[
    sizeof(TalkingHeadOrigin) == 0x08 ? 1 : -1];
typedef char ConstellationStar_size_must_be_0x06[
    sizeof(ConstellationStar) == 0x06 ? 1 : -1];
typedef char ConstellationParticle_size_must_be_0x08[
    sizeof(ConstellationParticle) == 0x08 ? 1 : -1];
typedef char ConstellationObjectDefinition_size_must_be_0x08[
    sizeof(ConstellationObjectDefinition) == 0x08 ? 1 : -1];

/* Runtime mission-nav record.  The loader expands each 3-byte disk coordinate
 * to a 32-bit fixed-point value, producing the 0x51-byte stride observed at
 * 0x0046C2F0. */
#pragma pack(push, 1)
typedef struct MissionNavPoint {
    char name[0x1e];                 /* +0x00 */
    signed char type;                /* +0x1E: 1 is an active nav point */
    FixedVector position;            /* +0x1F */
    unsigned short proximityRadius;  /* +0x2B */
    signed char triggers[4][2];      /* +0x2D: type, target nav point */
    enum ObjectType preloadObjectTypes[2]; /* +0x35 */
    short missionShips[10];          /* +0x3D */
} MissionNavPoint;
#pragma pack(pop)

/* The +0x28 slot has two proven interpretations.  Loaded campaign records use
 * it as the pilot/personality value, while the built-in opening-sequence
 * records (32-45) store a pointer to a canned command stream there. */
typedef union MissionShipBehaviour {
    const short *cannedSequence;
    int pilot;
} MissionShipBehaviour;

/* The expanded runtime mission-ship record at 0x0046C948.  The first 32
 * records are overwritten by LoadMissionData; the recovered opening
 * sequence follows them in records 32-45. */
#pragma pack(push, 1)
typedef struct MissionShipRecord {
    enum ObjectType type;             /* +0x00 */
    enum Side side;                   /* +0x04 */
    signed char leader;               /* +0x08 */
    signed char field_9;              /* +0x09 */
    enum ShipMissionType missionType; /* +0x0A */
    signed char navPoint;             /* +0x0E */
    FixedVector position;             /* +0x0F */
    short pitch;                      /* +0x1B */
    short yaw;                        /* +0x1D */
    short roll;                       /* +0x1F */
    signed char formationSpot;        /* +0x21 */
    short speed;                      /* +0x22 */
    int rating;                       /* +0x24 */
    MissionShipBehaviour behaviour;  /* +0x28 */
    short field_2c;                   /* +0x2C */
    int field_2e;                     /* +0x2E */
    signed char state;                /* +0x32 */
    signed char leaderMissionIndex;   /* +0x33 */
    signed char formationIndex;       /* +0x34 */
    signed char targetMissionIndex;   /* +0x35 */
} MissionShipRecord;

/* Expanded copy of one 64-byte objective/map record. */
typedef struct MissionObjectiveSource {
    int type;                         /* +0x00: sign-extended disk short */
    short index;                      /* +0x04 */
    char description[60];             /* +0x06 */
} MissionObjectiveSource;

/* Four packed resource-cache entries at 0x0059DDF0. */
typedef struct ObjectResourceSlot {
    signed char type;                 /* +0x00 */
    unsigned char *shapeSet;          /* +0x01: archive section 0 */
    unsigned char *animation;         /* +0x05: archive section 2 */
    unsigned char *shape;             /* +0x09: archive section 1 */
} ObjectResourceSlot;
#pragma pack(pop)

/* Runtime mission-objective records use the 0x1F-byte stride visible in every
 * objective scan.  The coordinate pair leads each record at 0x0059DAC0; the
 * former declaration incorrectly began five bytes later at the type field. */
#pragma pack(push, 1)
typedef struct MissionObjective {
    short mapX;                       /* +0x00 */
    short mapY;                       /* +0x02 */
    unsigned char field_4;            /* +0x04 */
    int type;                         /* +0x05 */
    signed char index;                /* +0x09 */
    unsigned char flags;              /* +0x0A */
    const char *displayName;          /* +0x0B */
    char *name;                       /* +0x0F: mission description */
    FixedVector position;             /* +0x13 */
} MissionObjective;

/* One objective in SAVEGAME.WLD.  The runtime integer and two pointer-sized
 * fields are retained as signed words by the inherited DOS file format. */
typedef struct SaveGameDiskObjective {
    short mapX;                       /* +0x00 */
    short mapY;                       /* +0x02 */
    unsigned char field_4;            /* +0x04 */
    short type;                       /* +0x05 */
    signed char index;                /* +0x07 */
    unsigned char flags;              /* +0x08 */
    short displayName;                /* +0x09 */
    short name;                       /* +0x0B */
    FixedVector position;             /* +0x0D */
} SaveGameDiskObjective;

typedef struct SaveGameRecord {
    char description[17];             /* +0x000 */
    unsigned char occupied;           /* +0x011 */
    PilotRecord pilots[9];            /* +0x012 */
    CampaignState campaign;           /* +0x168 */
    MissionObjective objectives[16];  /* +0x1C0 */
} SaveGameRecord;

typedef struct SaveGameDiskRecord {
    char description[17];                    /* +0x000 */
    unsigned char occupied;                  /* +0x011 */
    PilotRecord pilots[9];                   /* +0x012 */
    SaveGameDiskCampaignState campaign;      /* +0x168 */
    SaveGameDiskObjective objectives[16];    /* +0x1AC */
} SaveGameDiskRecord;

typedef struct BarracksBunkState {
    short occupied;                   /* +0x00 */
    short animationFrame;             /* +0x02 */
    short animationPeriod;            /* +0x04 */
    int animationTick;                /* +0x06 */
} BarracksBunkState;

typedef struct BarracksAnimationState {
    BarracksBunkState bunks[8];       /* +0x00 */
    short fallingDelay;               /* +0x50 */
    int fallingY;                     /* +0x52 */
    int fallingVelocity;              /* +0x56 */
    short impactFrame;                /* +0x5A */
    short blinkDelay;                 /* +0x5C */
    short eyesOpen;                   /* +0x5E */
    short animationTick;              /* +0x60 */
    char *menuLabel;                  /* +0x62 */
    short field_66;                   /* +0x66 */
} BarracksAnimationState;
#pragma pack(pop)

typedef char MissionNavPoint_size_must_be_0x51[
    sizeof(MissionNavPoint) == 0x51 ? 1 : -1];
typedef char MissionShipRecord_size_must_be_0x36[
    sizeof(MissionShipRecord) == 0x36 ? 1 : -1];
typedef char MissionObjectiveSource_size_must_be_0x42[
    sizeof(MissionObjectiveSource) == 0x42 ? 1 : -1];
typedef char MissionObjective_size_must_be_0x1f[
    sizeof(MissionObjective) == 0x1f ? 1 : -1];
typedef char SaveGameDiskObjective_size_must_be_0x19[
    sizeof(SaveGameDiskObjective) == 0x19 ? 1 : -1];
typedef char SaveGameRecord_size_must_be_0x3b0[
    sizeof(SaveGameRecord) == 0x3b0 ? 1 : -1];
typedef char SaveGameDiskRecord_size_must_be_0x33c[
    sizeof(SaveGameDiskRecord) == 0x33c ? 1 : -1];
typedef char BarracksBunkState_size_must_be_0x0a[
    sizeof(BarracksBunkState) == 0x0a ? 1 : -1];
typedef char BarracksAnimationState_size_must_be_0x68[
    sizeof(BarracksAnimationState) == 0x68 ? 1 : -1];

#define WC1_SPACE_OBJECT_COUNT 64
#define WC1_SPACE_LAST_MOVING_OBJECT 60
#define WC1_EYE_OBJECT 61
#define WC1_DIRECTION_VIEW_COUNT 62
#define WC1_DIRECTION_SHAPE_TABLE_COUNT 3
#define WC1_MISSION_SHIP_COUNT 64
#define WC1_ACTIVE_MISSION_SHIP_COUNT 32
#define WC1_MISSION_NAV_POINT_COUNT 20
#define WC1_ACTIVE_MISSION_NAV_POINT_COUNT 16
#define WC1_MISSION_OBJECTIVE_COUNT 16

/* --------------------------------------------------------------------------
 * Pilot record field order, from the DOS build's live memory layout
 * (WingCommanderArduinoBridge).  The DOS offsets themselves do not apply to
 * this build, but both builds compile the same struct, so the ORDER holds:
 *
 *     +0x00  first name          +0x1E  rank
 *     +0x0E  callsign            +0x20  sorties flown
 *                                +0x22  total kills
 *
 * In-mission kills are a separate global, not part of this record.  The
 * defaults are in the executable at 0x00469EC0: first name "Blair", callsign
 * "Maverick", and "CHEATER" -- the callsign the game substitutes once the
 * cheat flag is set (see ReadCheaterFlagFromRegistry, 0x00442600).
 * -------------------------------------------------------------------------- */

#endif /* WC1_DATA_H */
