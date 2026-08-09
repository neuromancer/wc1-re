/*
 *  Packet loader.
 *
 *  Address range 0x42b000-0x42b3ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN: PacketLoad prints "Library\\Source\\Pload.c PacketLoad".
 */
#include "wc1.h"

/* Function start: 0x42B1B0 */
void ServiceAudioStream(void)
{
    if (DAT_00465058 != 0 && DAT_0046a440 != 0) {
        LoopG0059861cFn7610();
        LoopAudioSoundFn7548();
        GetAudioVoiceFn73F3();
        LoopAudioStreamFnB300();
        DAT_0046a440 = 0;
    }
}

/* Function start: 0x42B3F0 */
int *FindWaveTableEntry(int key)
{
    int *p = DAT_0046a438;

    while (p != 0) {
        if (*(int *)(*p + 4) == key)
            return p;
        p = (int *)p[1];
    }
    return 0;
}
