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
 * Ship classes.  VERIFIED: the name table at 0x004684D4 lists these in exactly
 * this order.  Two of the spellings differ from the community documentation and
 * the binary's are used here: index 5 is "Dilligent" (sic) and index 14 is
 * "Spikeri", which is the developers' own name for the ship the manual calls
 * the Hhriss.  The table continues past the ships into weapon names
 * ("Laser cannon", "Neutron gun", "Mass driver" at 0x00468598).
 * -------------------------------------------------------------------------- */
enum ShipClass {
    SHIP_HORNET         = 0,
    SHIP_RAPIER         = 1,
    SHIP_SCIMITAR       = 2,
    SHIP_RAPTOR         = 3,
    SHIP_VENTURE        = 4,
    SHIP_DILLIGENT      = 5,      /* spelled with two Ls in the binary */
    SHIP_DRAYMAN        = 6,
    SHIP_EXETER         = 7,
    SHIP_TIGERS_CLAW    = 8,
    SHIP_SALTHI         = 9,
    SHIP_DRALTHI        = 10,
    SHIP_KRANT          = 11,
    SHIP_GRATHA         = 12,
    SHIP_JALTHI         = 13,
    SHIP_SPIKERI        = 14,     /* the manual's "Hhriss" */
    SHIP_DORKIR         = 15,
    SHIP_LUMBARI        = 16,
    SHIP_RALARI         = 17,
    SHIP_FRALTHI        = 18,
    SHIP_SNAKEIR        = 19,
    SHIP_SIVAR          = 20,
    SHIP_STARPOST       = 21,
    SHIP_ASTEROID_FIELD = 22,
    SHIP_MINE_FIELD     = 23,
    SHIP_NONE           = 255
};

/* --------------------------------------------------------------------------
 * Ship AI orders.  From WCMissionTools, and corroborated here: the three order
 * dispatchers (StepShipOrderState 0x00409F80, StepShipOrderWithLeader
 * 0x0040A030, StepShipOrderForNav 0x0040A360) all switch on the same dword
 * table with cases 4, 5, 6, 8, 9 and -1, and -1 is the enum's Inactive.
 * -------------------------------------------------------------------------- */
enum ShipOrder {
    ORDER_ATTACK        = 0,
    ORDER_PATROL        = 1,
    ORDER_ATTACK_TARGET = 2,
    ORDER_ESCORT        = 3,
    ORDER_FOLLOW        = 4,
    ORDER_DEFEND        = 5,
    ORDER_JUMP_OUT      = 6,
    ORDER_JUMP_IN       = 7,
    ORDER_GO_HOME       = 8,
    ORDER_AUTOPILOT     = 9,
    ORDER_NAVIGATE      = 10,
    ORDER_INACTIVE      = 255
};

enum Allegiance {
    ALLEGIANCE_CONFED   = 0,
    ALLEGIANCE_KILRATHI = 1,
    ALLEGIANCE_NEUTRAL  = 2
};

/* --------------------------------------------------------------------------
 * Pilots.  0-4 are the unnamed difficulty tiers; 5-13 the Confed wingmen;
 * 14-17 the Kilrathi aces.  PARTIALLY VERIFIED: the four ace names really are
 * in the executable, in this order, at 0x0046AFD4 ("Bhurak", "Dakhath",
 * "Khajja", "Bakhtosh") on an 8-byte stride, so the ace block's ordering and
 * base index are confirmed.  The Confed names live in the MODULE files, not in
 * the executable, so 5-13 come from WCMissionTools alone.
 * -------------------------------------------------------------------------- */
enum PilotId {
    PILOT_GENERIC_0     = 0,      /* easiest */
    PILOT_GENERIC_4     = 4,      /* hardest */
    PILOT_SPIRIT        = 5,
    PILOT_HUNTER        = 6,
    PILOT_BOSSMAN       = 7,
    PILOT_ICEMAN        = 8,
    PILOT_ANGEL         = 9,
    PILOT_PALADIN       = 10,
    PILOT_MANIAC        = 11,
    PILOT_KNIGHT        = 12,
    PILOT_BLAIR         = 13,
    PILOT_ACE_FIRST     = 14,     /* index into g_aszKilrathiAceNames_0046afd4 */
    PILOT_BHURAK        = 14,
    PILOT_DAKHATH       = 15,
    PILOT_KHAJJA        = 16,
    PILOT_BAKHTOSH      = 17
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
