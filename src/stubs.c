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
/* Function start: 0x4359C0 */ /* TODO */ void DispatchPendingEvents(int a) { (void)a; }
/* Function start: 0x442460 */ /* TODO */ void Streamer_stop(void) {}
