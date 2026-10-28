/*
 *  Video resource shutdown and palette allocation.
 *
 *  Address range 4318f0-431a0f (see docs/ORDER.md).
 */
#include "wc1.h"
#ifndef SDL_PORT
#include <dos.h>
#endif
#include <sys/types.h>

short nVideoMode = -1; /* 0x0046b168 */


/* Linker commons defined here; LINK orders them by identifier hash. */
int aiPilotLevel[12]; /* 0x0059cf30 */
short asTargetListRangesArray[16]; /* 0x0059cf60 */
short asObjectDrawPosY[SPACE_OBJECT_COUNT]; /* 0x0059cf80 */
short asObjectDrawPosX[SPACE_OBJECT_COUNT]; /* 0x0059d000 */
short asPreviousObjectDistance[SPACE_OBJECT_COUNT]; /* 0x0059d080 */
enum ObjectClass aeObjectClass[SPACE_OBJECT_COUNT]; /* 0x0059d100 */
enum ShipObjective aeShipObjectiveList[12]; /* 0x0059d200 */
short asObjectViewFrames[SPACE_OBJECT_COUNT]; /* 0x0059d230 */
int nCurObjectiveRangeValue; /* 0x0059d2b0 */
signed char acWingmanMessageStatesArray[16]; /* 0x0059d2c0 */
short asCollisionDelayList[16]; /* 0x0059d2d0 */

/* Function start: 0x4318F0 */
void __stdcall ShutdownVideoHook(short mode)
{
    ReleaseVideoResourcesHook();
}

/* Function start: 0x431900 */
short __stdcall ReserveContiguousPaletteEntries(short entryCount)
{
    short entry;
    short freeEntries;
    short firstEntry;

    firstEntry = 0;
    freeEntries = 0;
    entry = 0;
    for (;;) {
        if (awPalEntryAllocation[entry] != 0) {
            freeEntries = 0;
            firstEntry = (short)(entry + 1);
        } else
            freeEntries++;
        if (freeEntries == entryCount)
            break;
        entry++;
        if (entry >= 256)
            return -1;
    }

    freeEntries = 0;
    while (freeEntries < entryCount) {
        awPalEntryAllocation[firstEntry + freeEntries] =
            entryCount;
        freeEntries++;
    }
    return firstEntry;
}

/* Function start: 0x431970 */
void __stdcall ReleaseContiguousPaletteEntries(short firstEntry)
{
    short entry;
    short entryCount;

    entry = 0;
    entryCount = awPalEntryAllocation[firstEntry];
    while (entry < entryCount) {
        awPalEntryAllocation[firstEntry + entry] = 0;
        entry++;
    }
}

/* Function start: 0x4319B0 */
void PrintPaletteAllocationMap(void)
{
    short index;
    short row;
    short column;

    index = 0;
    row = 4;
    do {
        column = 0x40;
        do {
#if !defined(SDL_PORT) || defined(SDL_PORT_LEGACY_DEBUG_OUTPUT)
            printf("%c", awPalEntryAllocation[index++] < 1 ?
                   '_' : '.');
#else
            index++;
#endif
            column--;
        } while (column != 0);
#if !defined(SDL_PORT) || defined(SDL_PORT_LEGACY_DEBUG_OUTPUT)
        printf("\n");
#endif
        row--;
    } while (row != 0);
}
