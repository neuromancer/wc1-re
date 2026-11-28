/* Shared libFuzzer driver of the flight fuzzers: wc1Flight.c flies WC1, SM1
 * and SM2 from a Kilrathi Saga installation and swcFlight.c Super Wing
 * Commander. runFuzzer.py prepares the data and runs either one.
 *
 * Input: campaign (0-2: WC1, SM1 and SM2, or SWC's three campaign data sets;
 * 3: the training simulator), series (1-based; 0 for the simulator), mission
 * (0-based), start (bits 0-3: flight-path legs travelled before control
 * begins), little-endian PRNG seed (4 bytes), then pairs of action and hold
 * bytes. Action bits 0-5 select one of the game's controls; bit 6 keeps the
 * controls already held. Hold bits 0-5 give 1..64 ticks and bits 6-7
 * multiply them by 1, 2, 4 or 8.
 *
 * Environment: WC1_FLIGHT_DATA is the game directory (a disposable copy for
 * WC1; SWC's is only read). WC1_FLIGHT_CAMPAIGN and WC1_FLIGHT_CASE
 * (campaign:series:mission) select missions, and WC1_FLIGHT_TICKS limits the
 * ticks per input (1..3600, default 600). WC1_FLIGHT_SEEDS names a corpus to
 * add seed inputs to, WC1_FLIGHT_KEYS changes key bindings as the launcher
 * does (src/port/input/keymap.c), WC1_FLIGHT_LIST prints the installed campaigns and
 * the selected missions instead of fuzzing, and WC1_FLIGHT_TRACE prints where
 * each input started and ended. */
#ifndef FUZZ_FLIGHT_H
#define FUZZ_FLIGHT_H

#include "wc1.h"

#define FLIGHT_SIMULATOR 3

enum { ACTION_NONE, ACTION_KEY, ACTION_POINTER, ACTION_BUTTONS, ACTION_WHEEL, ACTION_EJECT };

typedef struct FlightAction {
    unsigned char kind;
    Sint32 key; /* a scancode in WC1, a keycode in SWC */
    short x;    /* key modifiers, pointer x, button mask or wheel step */
    short y;
} FlightAction;

/* One action of the seed inputs, with its hold byte. */
typedef struct FlightSeedStep {
    FlightAction action;
    unsigned char keepHeld;
    unsigned char hold;
} FlightSeedStep;

/* flight.c */
void Require(int condition, const char *message);

/* The game's fuzzer: its controls and the seed inputs' actions. */
extern const FlightAction flightActions[];
extern const size_t flightActionCount;
extern const FlightSeedStep flightSeed[];
extern const size_t flightSeedLength;

/* Set up the game in the current directory, which is WC1_FLIGHT_DATA. */
void StartFlightGame(void);
/* Fill the mission count of each series (1-13) of an installed campaign
 * (0-2), or return 0 when it is missing. */
int LoadFlightCampaign(int campaign, unsigned char missionCounts[14]);
/* Start a mission as the game does before its first flight tick. */
void BeginFlight(int campaign, int series, int mission);
int FlightContinues(void);
/* One flight tick. frame counts the ticks of the current action, or is -1
 * while the flight path is travelled. */
void FlightStep(int frame);
void ApplyFlightAction(const FlightAction *action);
void ReleaseFlightControls(void);
/* Show the space view at a nav point the flight path has just reached. */
void ViewFlightArrival(void);
/* Leave the mission so that the next input can start another. */
void EndFlight(void);

#endif
