/*
 *  Link stubs.
 *
 *  Placeholders that exist only so the project links while the reconstruction is
 *  incomplete -- binary-comp's `compare`, `report` and `order` all need a real
 *  linked executable and its map.
 *
 *  Every stub here is a TODO: replace it with the recovered implementation and
 *  delete the stub.  Nothing in this file is a claim about the original code.
 */
#include "wc1.h"

/* Function start: 0x406D80 */ /* TODO */ void ShipAiState40(short a,short b){(void)a;(void)b;}
/* Function start: 0x406FB0 */ /* TODO */ void ShipAiState29(short a,short b){(void)a;(void)b;}
/* Function start: 0x407270 */ /* TODO */ void ShipAiRoutine02(short a,unsigned int b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x41C910 */ /* TODO */ void DebugOverlayThreadProc(int p){(void)p;}

/* Forward references still to be implemented; each is a real function. */
/* Function start: 0x41DAA0 */ /* TODO */ short UiRoutine03(void){return 1;}
/* Function start: 0x4212A0 */
/* TODO: drop_mine -- create a mine at a ship weapon hardpoint. */
int drop_mine(short obj, short weapon, enum ObjectType type, short lifetime)
{
    (void)obj;
    (void)weapon;
    (void)type;
    (void)lifetime;
    return -1;
}

/* Function start: 0x42B640 */
/* TODO */ void StopAllSounds(void) {}
/* Function start: 0x42EF30 */
/* TODO: PlaySfxWaveFileByNumber -- plays sfx%02i.wav. */
void PlaySfxWaveFileByNumber(int n, int a, int b)
{
    (void)n;
    (void)a;
    (void)b;
}

/* Function start: 0x431470 */ /* TODO */ void EndCommSessionWithWingman(void){}
/* Function start: 0x4331F0 */ /* TODO */ char *DirectDrawResultToText(int hr){(void)hr;return "";}
/* Function start: 0x4359C0 */ /* TODO */ void DispatchPendingEvents(int a) { (void)a; }
/* Function start: 0x442460 */ /* TODO */ void Streamer_stop(void) {}
/* Function start: 0x4473F3 */ /* TODO */ void ix_system_shutdown(void){}
/* Function start: 0x447548 */ /* TODO */ void ix_system_delete_all_samples(void){}
/* Function start: 0x447610 */ /* TODO */ void ix_system_delete_all_sounds(void){}
