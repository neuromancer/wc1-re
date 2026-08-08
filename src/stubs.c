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

/* Function start: 0x401E30 */
/* TODO: WinMain -- real one at 0x00401E30 (single-instance semaphore, memory
   check, window creation, main loop). Stubbed so WinMainCRTStartup resolves. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd)
{
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nShowCmd;
    return 0;
}

/* Function start: 0x403DB0 */
/* TODO: mono-monitor debug printf in the game core; ix_log_printf calls it.
   Stubbed so the ix modules link. */
void GetG005a8760Fn3DB0(const char *fmt, ...)
{
    (void)fmt;
}

/* Function start: 0x42EF30 */
/* TODO: PlaySfxWaveFileByNumber -- plays sfx%02i.wav. */
void PlaySfxWaveFileByNumber(int n, int a, int b) { (void)n; (void)a; (void)b; }

/* Function start: 0x433060 */
/* TODO: DIBwholePaletteFromTriplets -- 256 packed RGB triplets -> SetEntries. */
void DIBwholePaletteFromTriplets(unsigned char *palette) { (void)palette; }

/* Function start: 0x42AFC0 */
/* TODO: SetMultimediaTimerCallback -- timeSetEvent/timeKillEvent wrapper. */
void SetMultimediaTimerCallback(int period) { (void)period; }

/* Function start: 0x42EE80 */
/* TODO */ void SetShipAiFnEE80(void) {}
/* Function start: 0x42B640 */
/* TODO */ void GetSfxFnB640(void) {}
/* Function start: 0x41CD40 */
/* TODO: PumpMessagesWhileDebugPaused -- GetMessage/Translate/Dispatch loop. */
void PumpMessagesWhileDebugPaused(unsigned int arg) { (void)arg; }

/* Forward references still to be implemented; each is a real function. */
/* Function start: 0x433120 */ /* TODO */ void DIBwholePaletteFromWords(void) {}
/* Function start: 0x407460 */ /* TODO */ void HelperOf4075A0A(short s) { (void)s; }
/* Function start: 0x4060B0 */ /* TODO */ void SelectNewShipAiBehavior(short s) { (void)s; }
/* Function start: 0x402C70 */ /* TODO */ void DoShipAiFn2C70(short a, int b) { (void)a; (void)b; }
/* Function start: 0x40A360 */ /* TODO */ void DispatchTbl0059d5e0FnA360(short a) { (void)a; }
/* Function start: 0x421100 */ /* TODO */ void ScanTbl0059cab0Fn1100(short a, int b) { (void)a; (void)b; }
/* Function start: 0x427C50 */ /* TODO */ void GetShipAiFn7C50(void) {}
/* Function start: 0x409DD0 */ /* TODO */ void GetUiFn9DD0(void) {}
/* Function start: 0x4360F0 */ /* TODO */ void GetUiFn60F0(unsigned char a, int b) { (void)a; (void)b; }
/* Function start: 0x435760 */ /* TODO */ void LoopInputFn5760(int n) { (void)n; }
/* Function start: 0x42F940b */ /* TODO */ void ReleaseTbl005a7f10FnF940(void *p) { (void)p; }
/* Function start: 0x42E330 */  /* TODO */ void DoLocalFnE330(void) {}
/* Function start: 0x4141B0 */ /* TODO */ void DoLocalFn41B0(int *p, int v) { (void)p; (void)v; }
/* Function start: 0x417760 */ /* TODO */ void GetUiFn7760(void) {}
/* Function start: 0x421AE0 */ /* TODO */ void BlitUiFn1AE0(void *p, unsigned int v) { (void)p; (void)v; }
