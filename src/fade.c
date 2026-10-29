/*
 *  Incremental palette transitions and the adjacent DOS string append entry.
 *
 *  Address range 41c510-41c75f (see docs/ORDER.md). The initialization flag
 *  follows the barracks literal pool. Ownership of the string entry remains
 *  provisional within this range.
 */
#define WC1_FADE_SOURCE
#include "wc1.h"

short nPaletteTransitionInitialise = 1; /* 0x00469640 */
short *pPaletteTransDirection; /* 0x005a7d88 */
short *pPaletteTransitionDelta; /* 0x005a7d8c */
short nPaletteTransMaxDelta; /* 0x005a7d90 */
short *pPaletteTransitionAccumulatorPtr; /* 0x005a7d94 */
short nPaletteTransitionCountdown; /* 0x005a7d98 */

/* Function start: 0x41C510 */
unsigned short __stdcall StepPaletteTransition(short *current,
                                                const short *target,
                                                short componentCount)
{
    unsigned int byteCount;
    short index;
    short difference;
    short *palette;
    short *accumulator;

    if (nPaletteTransitionInitialise != 0) {
        byteCount = (unsigned int)(componentCount * 2);
        pPaletteTransitionAccumulatorPtr =
            AllocateTaggedMemory(byteCount, 0);
        pPaletteTransitionDelta =
            AllocateTaggedMemory(byteCount, 0);
        pPaletteTransDirection =
            AllocateTaggedMemory(byteCount, 0);
        if (pPaletteTransitionAccumulatorPtr == 0 ||
            pPaletteTransitionDelta == 0 ||
            pPaletteTransDirection == 0) {
            if (pPaletteTransitionAccumulatorPtr != 0)
                ReleasePacketHandle(pPaletteTransitionAccumulatorPtr);
            if (pPaletteTransitionDelta != 0)
                ReleasePacketHandle(pPaletteTransitionDelta);
            if (pPaletteTransDirection != 0)
                ReleasePacketHandle(pPaletteTransDirection);
            return 0;
        }

        nPaletteTransMaxDelta = 0;
        palette = current;
        for (index = 0; index < componentCount; index++) {
            difference = (short)(palette[index] - target[index]);
            if (difference < 0) {
                difference = (short)-difference;
                pPaletteTransDirection[index] = 4;
            } else {
                pPaletteTransDirection[index] = -4;
            }
            pPaletteTransitionDelta[index] = difference;
            if (nPaletteTransMaxDelta < difference)
                nPaletteTransMaxDelta = difference;
        }

        difference = (short)(nPaletteTransMaxDelta / 4);
        for (index = 0; index < componentCount; index++) {
            pPaletteTransitionAccumulatorPtr[index] = difference;
        }
        nPaletteTransitionInitialise = 0;
        nPaletteTransitionCountdown =
            (short)(nPaletteTransMaxDelta / 4);
    } else {
        palette = current;
    }

    if (nPaletteTransitionCountdown-- == 0) {
        ReleasePacketHandle(pPaletteTransitionAccumulatorPtr);
        ReleasePacketHandle(pPaletteTransitionDelta);
        ReleasePacketHandle(pPaletteTransDirection);
        nPaletteTransitionInitialise = 1;
        return 0;
    }

    for (index = 0; index < componentCount; index++) {
        pPaletteTransitionAccumulatorPtr[index] =
            (short)(pPaletteTransitionAccumulatorPtr[index] +
                    pPaletteTransitionDelta[index]);
        accumulator = &pPaletteTransitionAccumulatorPtr[index];
        if (*accumulator >
            nPaletteTransMaxDelta) {
            *accumulator =
                (short)(*accumulator -
                        nPaletteTransMaxDelta);
            palette[index] =
                (short)(palette[index] +
                        pPaletteTransDirection[index]);
        }
    }
    return 1;
}

/* Function start: 0x41C740 */
char *__stdcall DosStrcat(char *destination, const char *source)
{
    char *end;

    end = DosStrchr(destination, 0);
    DosStrcpy(end, source);
    return destination;
}
