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

/* Compact three-axis offset used by the formation tables.  Unlike a
 * FixedVector, each component is a 16-bit distance. */
typedef struct ShortVector {
    short x;
    short y;
    short z;
} ShortVector;

/* Weighted pair in the retail maneuver-selection tables. */
typedef struct ManeuverChoice {
    signed char threshold;
    signed char primary;
    signed char secondary;
} ManeuverChoice;

/* Runtime mission-nav record.  The loader expands each 3-byte disk coordinate
 * to a 32-bit fixed-point value, producing the 0x51-byte stride observed at
 * 0x0046C2F0. */
#pragma pack(push, 1)
typedef struct MissionNavPoint {
    char name[0x1e];                 /* +0x00 */
    unsigned char type;              /* +0x1E: 1 is an active nav point */
    FixedVector position;            /* +0x1F */
    short proximityRadius;           /* +0x2B */
    unsigned char missionData[0x24]; /* +0x2D */
} MissionNavPoint;
#pragma pack(pop)

#define WC1_MISSION_NAV_POINT_COUNT 16

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
