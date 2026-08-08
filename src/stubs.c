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
/* Function start: 0x407030 */ /* TODO */ void ShipAiState30(short a, short b) { (void)a; (void)b; }
/* Function start: 0x409BD0 */ /* TODO */ void ReleaseTbl0046c028Fn9BD0(short i) { (void)i; }
/* Function start: 0x419290 */ /* TODO */ void SetTbl0059d710Fn9290(short a, int *p) { (void)a; (void)p; }
/* Function start: 0x406310 */ /* TODO */ void ShipAiRoutine04(short a, short b) { (void)a; (void)b; }
/* Function start: 0x4075A0b */ /* TODO */ void ShipAiRoutine21(short a, int b) { (void)a; (void)b; }
/* Function start: 0x406910 */ /* TODO */ void ShipAiRoutine07(short a, int b) { (void)a; (void)b; }
/* Function start: 0x402320 */ /* TODO */ void PumpWindowMessages(void) {}
/* Function start: 0x4359C0 */ /* TODO */ void DispatchTbl0046da90Fn59C0(int a) { (void)a; }
/* Function start: 0x421A90 */ /* TODO */ void DoLocalFn1A90(void *a, void *b) { (void)a; (void)b; }
/* Function start: 0x4273C0 */ /* TODO */ void ShowMemoryStatusDebug(void) {}
/* Function start: 0x428F20 */ /* TODO */ void DoLocalFn8F20(const char *s) { (void)s; }
/* Function start: 0x428FA0b */ /* TODO */ void HelperOf428F80A(const char *s) { (void)s; }
/* Function start: 0x428FA0 */ /* TODO */ void ShowOnScreenMessage(short f, short d, const char *fmt, ...) { (void)f; (void)d; (void)fmt; }
/* Function start: 0x430C00 */ /* TODO */ void CombatRoutine03(int a, short b) { (void)a; (void)b; }
char *PTR_s_Never_mind____0046af90[32];
/* Function start: 0x403BA0 */ /* TODO */ void DoLocalFn3BA0(int a,int b,unsigned int c,int d,int e){(void)a;(void)b;(void)c;(void)d;(void)e;}
/* Function start: 0x406200 */ /* TODO */ void ShipAiRoutine02(short a,unsigned int b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x440C00 */ /* TODO */ void ValidateViewportBounds(int *a,int *b,unsigned int *c){(void)a;(void)b;(void)c;}
/* Function start: 0x443020 */ /* TODO */ void SetTbl00486112Fn3020(short a,unsigned short *b){(void)a;(void)b;}
/* Function start: 0x4272F0 */ /* TODO */ void Helper4272F0(void) {}
/* Function start: 0x402110 */ /* TODO */ void AbortToDesktop(void) {}
