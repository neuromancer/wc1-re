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
#if 0
    MISSION_TYPE_CANNED_SEQUENCE   = 8,
    MISSION_TYPE_RENDEZVOUS        = 9,
    MISSION_TYPE_COME_HOME         = 10,
    MISSION_TYPE_BOGUS_AVOID_CRASH = 11
#else
    MISSION_TYPE_RENDEZVOUS        = 8,
    MISSION_TYPE_CANNED_SEQUENCE   = 10,
    MISSION_TYPE_BOGUS_AVOID_CRASH = 11,
    MISSION_TYPE_COME_HOME         = 12
#endif
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
}
#ifdef WC1_SDL
__attribute__((packed))
#endif
FixedVector;

/* Descriptor used by the legacy near-memory allocator.  Descriptors grow
 * downward from the end of the managed region. */
typedef struct NearHeapBlock {
    int address;
    unsigned int sizeAndFlags;
} NearHeapBlock;

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

/* One of the paired projectiles crossing the campaign-victory scene. */
#pragma pack(push, 2)
typedef struct CampaignVictoryProjectile {
    int x;                           /* +0x00 */
    int y;                           /* +0x04 */
    int depth;                       /* +0x08 */
    short screenX;                   /* +0x0C */
    short screenY;                   /* +0x0E */
    short flip;                      /* +0x10 */
    int scale;                       /* +0x12: -1 while inactive */
} CampaignVictoryProjectile;
#pragma pack(pop)

typedef char CampaignVictoryProjectile_size_must_be_0x16[
    sizeof(CampaignVictoryProjectile) == 0x16 ? 1 : -1];

/* WC2 runtime object-type records.  The 0xF3-byte stride and the named fields
 * are fixed by accesses throughout the ship/object code; unknown slots retain
 * offset names until their purpose is established. */
#pragma pack(push, 1)
typedef struct ObjectTypeData {
    char displayName[20];              /* +0x00 */
    short objectClass;                 /* +0x14 */
    short field_16;                    /* +0x16 */
    short field_18;                    /* +0x18 */
    short collisionRadius;             /* +0x1A */
    short radarRadius;                 /* +0x1C */
    short scale;                       /* +0x1E */
    short animationDelay;              /* +0x20 */
    short lifetime;                    /* +0x22 */
    short weaponDamage;                /* +0x24 */
    short damageCapacity;              /* +0x26 */
    short explosionDamage;             /* +0x28 */
    short maximumVelocity;             /* +0x2A */
    short cruiseVelocity;              /* +0x2C */
    unsigned char *animation;          /* +0x2E */
    int acceleration;                  /* +0x32 */
    short pitchRate;                   /* +0x36 */
    short yawRate;                     /* +0x38 */
    short rollRate;                    /* +0x3A */
    short afterburnerVelocity;         /* +0x3C */
    unsigned char weaponLoadout[0xA1]; /* +0x3E */
    short shieldFore;                  /* +0xDF */
    short shieldAft;                   /* +0xE1 */
    short armorFront;                  /* +0xE3 */
    short armorRear;                   /* +0xE5 */
    short armorLeft;                   /* +0xE7 */
    short armorRight;                  /* +0xE9 */
    unsigned char *shapeSet;           /* +0xEB */
    unsigned char *shape;              /* +0xEF */
} ObjectTypeData;
#pragma pack(pop)

#ifndef WC1_SDL
typedef char ObjectTypeData_size_must_be_0xf3[
    sizeof(ObjectTypeData) == 0xf3 ? 1 : -1];
typedef char ObjectTypeData_objectClass_must_be_at_0x14[
    offsetof(ObjectTypeData, objectClass) == 0x14 ? 1 : -1];
typedef char ObjectTypeData_weaponLoadout_must_be_at_0x3e[
    offsetof(ObjectTypeData, weaponLoadout) == 0x3e ? 1 : -1];
#endif

/* WC2 stores sixteen ten-byte weapon entries after a signed count byte. */
#pragma pack(push, 1)
typedef struct ShipWeaponSlot {
    short type;                        /* +0x00: spawned object type */
    short hardpoint;                   /* +0x02 */
    short field_4;                     /* +0x04 */
    short field_6;                     /* +0x06 */
    signed char disabled;              /* +0x08 */
    signed char weaponType;            /* +0x09: object-data index */
} ShipWeaponSlot;
#pragma pack(pop)

typedef char ShipWeaponSlot_size_must_be_0x0a[
    sizeof(ShipWeaponSlot) == 0x0a ? 1 : -1];

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

/* The 140-word cockpit table at 0x0046E008.  Five ship cockpits use the
 * viewport/scanner/hand rectangles; the four readout origins also contain a
 * sixth sentinel entry used by the original table indexing. */
typedef struct CockpitLayout {
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

#pragma pack(push, 1)
typedef struct InputManagerState {
    unsigned char field_0[0x0d];
    void *cursorShape;
    unsigned char field_11[0x10];
} InputManagerState;
#pragma pack(pop)

typedef struct InputPumpContext {
    void (*pump)(void);
    Viewport *viewport;
} InputPumpContext;

#pragma pack(push, 1)
typedef struct CannedSceneBufferHeader {
    unsigned int byteCount;
    short nextFrame;
} CannedSceneBufferHeader;
#pragma pack(pop)

typedef char InputManagerState_size_must_be_0x21[
    sizeof(InputManagerState) == 0x21 ? 1 : -1];

#pragma pack(push, 1)
typedef struct MusicResource {
    void *packet;
    short loaded;
} MusicResource;
#pragma pack(pop)

typedef char MusicResource_size_must_be_6[
    sizeof(MusicResource) == 6 ? 1 : -1];

typedef char CockpitLayout_size_must_be_0x118[
    sizeof(CockpitLayout) == 0x118 ? 1 : -1];
typedef char ScreenViewportGeometry_size_must_be_0x10[
    sizeof(ScreenViewportGeometry) == 0x10 ? 1 : -1];
typedef char DiskFileRecord_size_must_be_0x10[
    sizeof(DiskFileRecord) == 0x10 ? 1 : -1];
typedef char BriefingPacketHeader_size_must_be_0x28[
    sizeof(BriefingPacketHeader) == 0x28 ? 1 : -1];
#pragma pack(push, 1)
typedef struct NavMapLabel {
    short x;
    short y;
    short colour;
    const char *text;
} NavMapLabel;
#pragma pack(pop)

#ifndef WC1_SDL
typedef char NavMapLabel_size_must_be_0x0a[
    sizeof(NavMapLabel) == 0x0a ? 1 : -1];
#endif

/* Marker geometry and palette slots for one mission-objective type. */
typedef struct NavMapObjectiveStyle {
    short markerType;
    short markerSize;
    unsigned char *unvisitedColour;
    unsigned char *markerColour;
    unsigned char *labelColour;
} NavMapObjectiveStyle;

#ifndef WC1_SDL
typedef char NavMapObjectiveStyle_size_must_be_0x10[
    sizeof(NavMapObjectiveStyle) == 0x10 ? 1 : -1];
#endif

/* One selectable title-screen image and its inclusive mouse hit bounds. */
typedef struct TitleMenuRegion {
    short frame;
    short left;
    short top;
    short right;
    short bottom;
} TitleMenuRegion;

/* One packet-backed pointer slot in a scene resource list. */
#ifdef WC1_SDL
typedef unsigned char *Wc1PackedResourcePointer
    __attribute__((aligned(1)));
#endif
#pragma pack(push, 1)
typedef struct PacketResourceDescriptor {
#ifdef WC1_SDL
    Wc1PackedResourcePointer *resource;
#else
    unsigned char **resource;
#endif
#if 0
    short logicalFile;
#else
    char *fileName;
#endif
    short section;
} PacketResourceDescriptor;

/* A decoded WC2 scene chunk.  The retail code addresses the allocation at
 * +2 and the number of entries at +6, so this record is byte packed. */
typedef struct SceneResourceTable {
    short type;
    void *data;
    short count;
} SceneResourceTable;

typedef struct ScenePacketHeader {
    int formOffset;
} ScenePacketHeader;

typedef struct SceneHotspot {
    short left;
    short top;
    short right;
    short bottom;
    unsigned char selection;
} SceneHotspot;
#pragma pack(pop)

#ifndef WC1_SDL
typedef char PacketResourceDescriptor_size_must_be_0x0a[
    sizeof(PacketResourceDescriptor) == 0x0a ? 1 : -1];
typedef char SceneResourceTable_size_must_be_0x08[
    sizeof(SceneResourceTable) == 0x08 ? 1 : -1];
typedef char SceneHotspot_size_must_be_0x09[
    sizeof(SceneHotspot) == 0x09 ? 1 : -1];
#endif

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

/* WC2 keeps the current/default pilot identity and campaign cursor in one
 * packed 0x60-byte record.  The string widths and the late series/mission
 * fields are fixed by the accesses in the startup controller and personnel
 * database.  Fields whose purpose is not yet established retain offset names. */
#pragma pack(push, 1)
typedef struct Wc2PilotProfile {
    char firstName[25];               /* +0x00 */
    char lastName[25];                /* +0x19 */
    char callsign[13];                /* +0x32 */
    short field_3f;                   /* +0x3F */
    short field_41;                   /* +0x41 */
    short field_43;                   /* +0x43 */
    unsigned char field_45[0x11];     /* +0x45 */
    short series;                     /* +0x56 */
    short mission;                    /* +0x58 */
    unsigned char field_5a[4];        /* +0x5A */
    short field_5e;                   /* +0x5E */
} Wc2PilotProfile;

/* Variable-length WC2 campaign globals.  The on-disk word count determines
 * the saved byte size; all 32 pilot-status words nevertheless have fixed
 * offsets in the retail image. */
typedef struct Wc2CampaignGlobals {
    unsigned short wordCount;         /* +0x00 */
    short campaignSlot;               /* +0x02 */
    short series;                     /* +0x04 */
    short mission;                    /* +0x06 */
    short field_08;                   /* +0x08 */
    unsigned char field_0a[8];        /* +0x0A */
    short arcadeState;                /* +0x12 */
    unsigned char field_14[0x8a];     /* +0x14 */
    short pilotCount;                 /* +0x9E */
    short pilotStatus[32];            /* +0xA0 */
} Wc2CampaignGlobals;
#pragma pack(pop)

typedef char Wc2PilotProfile_size_must_be_0x60[
    sizeof(Wc2PilotProfile) == 0x60 ? 1 : -1];
typedef char Wc2CampaignGlobals_pilot_status_must_start_at_0xa0[
    offsetof(Wc2CampaignGlobals, pilotStatus) == 0xa0 ? 1 : -1];

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
#ifndef WC1_SDL
typedef char CampaignState_size_must_be_0x58[
    sizeof(CampaignState) == 0x58 ? 1 : -1];
#endif
typedef char SaveGameDiskCampaignState_size_must_be_0x44[
    sizeof(SaveGameDiskCampaignState) == 0x44 ? 1 : -1];
typedef char ConversationSceneRecord_size_must_be_0x0d[
    sizeof(ConversationSceneRecord) == 0x0d ? 1 : -1];
#ifndef WC1_SDL
typedef char BriefingCharacterLayout_size_must_be_0x12[
    sizeof(BriefingCharacterLayout) == 0x12 ? 1 : -1];
typedef char SceneAnimationObject_size_must_be_0x36[
    sizeof(SceneAnimationObject) == 0x36 ? 1 : -1];
typedef char ScrambleAnimationActor_size_must_be_0x18[
    sizeof(ScrambleAnimationActor) == 0x18 ? 1 : -1];
#endif
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

/* Runtime asteroid/mine-field descriptor.  WC2 retained the original
 * 16-bit object type, producing the 0x14-byte stride visible in its scans. */
#pragma pack(push, 1)
typedef struct HazardField {
    short type;                        /* +0x00 */
    FixedVector center;                /* +0x02 */
    short innerRadius;                 /* +0x0e */
    short outerRadius;                 /* +0x10 */
    short density;                     /* +0x12 */
} HazardField;
#pragma pack(pop)

typedef char HazardField_size_must_be_0x14[
    sizeof(HazardField) == 0x14 ? 1 : -1];

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

/* Runtime WC2 mission-nav record.  The loader expands each 3-byte disk
 * coordinate to a 32-bit fixed-point value.  The three resource columns and
 * the ten mission-ship indices produce the 0x65-byte stride at 0x00491E98. */
#pragma pack(push, 1)
typedef struct MissionNavPoint {
    char name[0x1e];                 /* +0x00 */
    signed char type;                /* +0x1E: 1 is an active nav point */
    FixedVector position;            /* +0x1F */
    short proximityRadius;           /* +0x2B */
#if 0
    signed char triggers[4][2];      /* +0x2D: type, target nav point */
    enum ObjectType preloadObjectTypes[2]; /* +0x35 */
    short missionShips[10];          /* +0x3D */
#else
    signed char systemIndex;         /* +0x2D */
    signed char field_2e;            /* +0x2E */
    signed char triggers[4][2];      /* +0x2F: type, target nav point */
    signed char field_37[8];         /* +0x37 */
    short preloadObjectTypes[3];     /* +0x3F */
    short preloadObjectClasses[3];   /* +0x45 */
    short preloadLogicalFiles[3];    /* +0x4B */
    short missionShips[10];          /* +0x51 */
#endif
} MissionNavPoint;
#pragma pack(pop)

/* Direct 0x18-byte header copied from WC2 campaign packet 0. */
#pragma pack(push, 1)
typedef struct MissionHeader {
    short entryNavPoint;              /* +0x00 */
    short wingmanMissionShip;         /* +0x02 */
    short homeMissionShip;            /* +0x04 */
    short playerMissionShip;          /* +0x06 */
    short initialMissionShips[8];     /* +0x08 */
} MissionHeader;
#pragma pack(pop)

#if 0
/* WC1 expanded the on-disk mission record and reused the +0x28 slot for the
 * built-in opening-sequence command streams. */
typedef union MissionShipBehaviour {
    const short *cannedSequence;
    int pilot;
} MissionShipBehaviour;

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
#else
/* WC2 copies sixteen packed 0x3c-byte mission records directly from packet 3
 * into the table at 0x00492290. */
#pragma pack(push, 1)
typedef struct MissionShipRecord {
    char name[19];                    /* +0x00 */
    signed char field_13;             /* +0x13 */
    signed char type;                 /* +0x14: loaded ship-resource type */
    short objectType;                 /* +0x15: world-object/hazard type */
    short side;                       /* +0x17 */
    signed char field_19;             /* +0x19 */
    signed char field_1a;             /* +0x1A */
    short missionType;                /* +0x1B */
    signed char navPoint;             /* +0x1D */
    FixedVector position;             /* +0x1E */
    short pitch;                      /* +0x2A */
    short yaw;                        /* +0x2C */
    short roll;                       /* +0x2E */
    signed char formationSpot;        /* +0x30 */
    short speed;                      /* +0x31 */
    short rating;                     /* +0x33 */
    short pilot;                      /* +0x35 */
    signed char state;                /* +0x37 */
    signed char leaderMissionIndex;   /* +0x38 */
    signed char formationIndex;       /* +0x39 */
    signed char systemIndex;          /* +0x3A */
    signed char portrait;             /* +0x3B */
} MissionShipRecord;
#endif

/* WC1 expanded the first short to an int after loading; WC2 retains the
 * packet's packed 64-byte representation in memory. */
typedef struct MissionObjectiveSource {
#if 0
    int type;                         /* +0x00: sign-extended disk short */
    short index;                      /* +0x04 */
    char description[60];             /* +0x06 */
#else
    short type;                       /* +0x00 */
    short index;                      /* +0x02 */
    char description[60];             /* +0x04 */
#endif
} MissionObjectiveSource;

/* Five packed WC2 ship-resource cache entries at 0x00493398. */
typedef struct ObjectResourceSlot {
#if 0
    signed char type;                 /* +0x00: WC1 layout */
    unsigned char *shapeSet;          /* +0x01 */
    unsigned char *animation;         /* +0x05 */
    unsigned char *shape;             /* +0x09 */
#else
    short type;                       /* +0x00 */
    short logicalFile;                /* +0x02 */
    short objectClass;                /* +0x04 */
    unsigned char *shapeSet;          /* +0x06: archive section 0 */
    unsigned char *animation;         /* +0x0A: archive section 2 */
    unsigned char *shape;             /* +0x0E: archive section 1 */
    unsigned char *field_12;          /* +0x12 */
#endif
} ObjectResourceSlot;
typedef char ObjectResourceSlot_size_must_be_0x16[
    sizeof(ObjectResourceSlot) == 0x16 ? 1 : -1];
#pragma pack(pop)

/* WC2 runtime mission-objective records use the 0x1E-byte stride visible in
 * every objective scan.  The coordinate pair leads each record at 0x4932A8. */
#pragma pack(push, 1)
typedef struct MissionObjective {
    short mapX;                       /* +0x00 */
    short mapY;                       /* +0x02 */
    unsigned char field_4;            /* +0x04 */
#if 0
    int type;                         /* +0x05 */
    signed char index;                /* +0x09 */
    unsigned char flags;              /* +0x0A */
    const char *displayName;          /* +0x0B */
    char *name;                       /* +0x0F: mission description */
    FixedVector position;             /* +0x13 */
#else
    short type;                       /* +0x05 */
    signed char index;                /* +0x07 */
    unsigned char field_8;            /* +0x08 */
    signed char flags;                /* +0x09 */
    const char *displayName;          /* +0x0A */
    char *name;                       /* +0x0E: mission description */
    FixedVector position;             /* +0x12 */
#endif
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
    const char *menuLabel;            /* +0x62 */
    short field_66;                   /* +0x66 */
} BarracksAnimationState;
#pragma pack(pop)

#if 0
typedef char MissionNavPoint_size_must_be_0x51[
    sizeof(MissionNavPoint) == 0x51 ? 1 : -1];
#else
typedef char MissionNavPoint_size_must_be_0x65[
    sizeof(MissionNavPoint) == 0x65 ? 1 : -1];
#endif
#ifndef WC1_SDL
typedef char MissionShipRecord_size_must_be_0x3c[
    sizeof(MissionShipRecord) == 0x3c ? 1 : -1];
#endif
typedef char MissionHeader_size_must_be_0x18[
    sizeof(MissionHeader) == 0x18 ? 1 : -1];
typedef char MissionObjectiveSource_size_must_be_0x40[
    sizeof(MissionObjectiveSource) == 0x40 ? 1 : -1];
#ifndef WC1_SDL
#if 0
typedef char MissionObjective_size_must_be_0x1f[
    sizeof(MissionObjective) == 0x1f ? 1 : -1];
#else
typedef char MissionObjective_size_must_be_0x1e[
    sizeof(MissionObjective) == 0x1e ? 1 : -1];
#endif
#endif
typedef char SaveGameDiskObjective_size_must_be_0x19[
    sizeof(SaveGameDiskObjective) == 0x19 ? 1 : -1];
#ifndef WC1_SDL
#if 0
typedef char SaveGameRecord_size_must_be_0x3b0[
    sizeof(SaveGameRecord) == 0x3b0 ? 1 : -1];
#else
typedef char SaveGameRecord_size_must_be_0x3a0[
    sizeof(SaveGameRecord) == 0x3a0 ? 1 : -1];
#endif
#endif
typedef char SaveGameDiskRecord_size_must_be_0x33c[
    sizeof(SaveGameDiskRecord) == 0x33c ? 1 : -1];
typedef char BarracksBunkState_size_must_be_0x0a[
    sizeof(BarracksBunkState) == 0x0a ? 1 : -1];
#ifndef WC1_SDL
typedef char BarracksAnimationState_size_must_be_0x68[
    sizeof(BarracksAnimationState) == 0x68 ? 1 : -1];
#endif

#define WC1_SPACE_OBJECT_COUNT 64
#define WC2_SPACE_OBJECT_COUNT 70
#define WC1_SPACE_LAST_MOVING_OBJECT 60
#define WC2_SPACE_LAST_MOVING_OBJECT 66
#define WC1_EYE_OBJECT 61
#define WC2_EYE_OBJECT 67
#define WC1_DIRECTION_VIEW_COUNT 62
#define WC1_DIRECTION_SHAPE_TABLE_COUNT 3
#define WC2_MISSION_SHIP_COUNT 16
#define WC1_MISSION_SHIP_STORAGE_COUNT 48
#define WC1_MISSION_SHIP_SCAN_LIMIT 64
#define WC1_ACTIVE_MISSION_SHIP_COUNT 32
#define WC1_MISSION_NAV_POINT_COUNT 20
#define WC1_ACTIVE_MISSION_NAV_POINT_COUNT 16
#define WC2_MISSION_OBJECTIVE_COUNT 8
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
