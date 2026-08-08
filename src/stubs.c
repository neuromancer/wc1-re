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


/* Function start: 0x42EF30 */
/* TODO: PlaySfxWaveFileByNumber -- plays sfx%02i.wav. */
void PlaySfxWaveFileByNumber(int n, int a, int b) { (void)n; (void)a; (void)b; }

/* Function start: 0x433060 */
/* TODO */ void DIBwholePaletteFromTriplets(unsigned char *palette) { (void)palette; }
/* Function start: 0x4331F0 */ /* TODO */ char *DirectDrawResultToText(int hr){(void)hr;return "";}
/* Function start: 0x432970 */ /* TODO */ void DIBslamReal(void){}
/* Function start: 0x432410 */ /* TODO */ int DIBcascade(int m,int *e){(void)m;(void)e;return 0;}
/* Function start: 0x4328A0 */ /* TODO */ void DIBdestroyDIB(void){}

/* Function start: 0x42AFC0 */
/* TODO: SetMultimediaTimerCallback -- timeSetEvent/timeKillEvent wrapper. */
void SetMultimediaTimerCallback(int period) { (void)period; }
/* Function start: 0x42B640 */
/* TODO */ void GetSfxFnB640(void) {}
/* Function start: 0x41CD40 */
/* TODO: PumpMessagesWhileDebugPaused -- GetMessage/Translate/Dispatch loop. */
void PumpMessagesWhileDebugPaused(unsigned int arg) { (void)arg; }

/* Forward references still to be implemented; each is a real function. */
/* Function start: 0x433120 */ /* TODO */ void DIBwholePaletteFromWords(void) {}
/* Function start: 0x407460 */ /* TODO */ void HelperOf4075A0A(short s) { (void)s; }
/* Function start: 0x402C70 */ /* TODO */ void DoShipAiFn2C70(short a, int b) { (void)a; (void)b; }
/* Function start: 0x40A360 */ /* TODO */ void DispatchTbl0059d5e0FnA360(short a) { (void)a; }
/* Function start: 0x421100 */ /* TODO */ void ScanTbl0059cab0Fn1100(short a, int b) { (void)a; (void)b; }
/* Function start: 0x427C50 */ /* TODO */ void GetShipAiFn7C50(void) {}
/* Function start: 0x409DD0 */ /* TODO */ void GetUiFn9DD0(void) {}
/* Function start: 0x4360F0 */ /* TODO */ void GetUiFn60F0(unsigned char a, int b) { (void)a; (void)b; }
/* Function start: 0x435760 */ /* TODO */ void LoopInputFn5760(int n) { (void)n; }
/* Function start: 0x42F940b */ /* TODO */ void ReleaseTbl005a7f10FnF940(void *p) { (void)p; }
/* Function start: 0x417760 */ /* TODO */ void GetUiFn7760(void) {}
/* Function start: 0x421AE0 */ /* TODO */ void BlitUiFn1AE0(void *p, unsigned int v) { (void)p; (void)v; }
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
/* Function start: 0x419210 */ /* TODO */ void GetTbl0059d710Fn9210(short a,int *p){(void)a;(void)p;}
/* Function start: 0x434F20 */ /* TODO */ long CalcVideoFn4F20(void){return 0;}
/* Function start: 0x4184C0 */ /* TODO */ void DoLocalFn84C0(int v){(void)v;}
/* Function start: 0x4117D0 */ /* TODO */ void DispatchTbl0059b9e0Fn17D0(void *a,int b,short c){(void)a;(void)b;(void)c;}
/* Function start: 0x416260 */ /* TODO */ void DispatchG004691e2Fn6260(char *a,unsigned char b){(void)a;(void)b;}
char *PTR_s_MISSILE_LOCKED_004691d4[8];
/* Function start: 0x418780 */ /* TODO */ void DoLocalFn8780(int a,short b,int *p){(void)a;(void)b;(void)p;}
/* Function start: 0x419660 */ /* TODO */ void GetG0059c490Fn9660(short a,void *p){(void)a;(void)p;}
/* Function start: 0x419810 */ /* TODO */ void DoShipAiFn9810(short a,void *p){(void)a;(void)p;}
/* Function start: 0x419950 */ /* TODO */ void DoLocalFn9950(unsigned int *p,int v){(void)p;(void)v;}
/* Function start: 0x414BF0 */ /* TODO */ void ReportComponentHit(char a,char b,char c){(void)a;(void)b;(void)c;}
/* Function start: 0x428280 */ /* TODO */ void GetTbl0059b470Fn8280(short a,int b){(void)a;(void)b;}
/* Function start: 0x419990 */ /* TODO */ void DoLocalFn9990(int *a,int *b,short c){(void)a;(void)b;(void)c;}
/* Function start: 0x40AA00 */ /* TODO */ void ScanTbl0059cab0Fn0AA0(short a){(void)a;}
/* Function start: 0x4148A0 */ /* TODO */ void SetTbl0059dec0Fn48A0(short i,int v){(void)i;(void)v;}
/* Function start: 0x407060 */ /* TODO */ int ShipAiRoutine18(short s){(void)s;return 0;}
/* Function start: 0x435090 */ /* TODO */ void DoLocalFn5090(short a,short b,short *p){(void)a;(void)b;(void)p;}
/* Function start: 0x427440 */ /* TODO */ unsigned int GameNoBodyStubv20(void){return 0;}
/* Function start: 0x442460 */ /* TODO */ void Streamer_stop(void) {}
/* Function start: 0x403DE0 */ /* TODO */ void MonoDebug_print(const char *s){(void)s;}
/* Function start: 0x406FB0 */ /* TODO */ void ShipAiState29(short a,short b){(void)a;(void)b;}
/* Function start: 0x406D80 */ /* TODO */ void ShipAiState40(short a,short b){(void)a;(void)b;}
/* Function start: 0x414180 */ /* TODO */ void DoLocalFn4180(int *p){(void)p;}
short DAT_0059d52a;
/* Function start: 0x4171D0 */ /* TODO */ void SetCombatFn71D0(void){}
/* Function start: 0x417260 */ /* TODO */ void DrawCombatFn7260(void){}
/* Function start: 0x417A00 */ /* TODO */ void CombatRoutine02(short a,int b,unsigned int c,int d,int e){(void)a;(void)b;(void)c;(void)d;(void)e;}
/* Function start: 0x429E30 */ /* TODO */ short UiRoutine03(void){return 1;}
/* Function start: 0x41D2E0b */ /* TODO */ void FetchDiskPacketRetrying(short a,int b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x42A120 */ /* TODO */ short LoopNavFnA120(int a){(void)a;return 0;}
/* Function start: 0x40BFF0 */ /* TODO */ void ReleaseTbl0046c35fFnBFF0(short a){(void)a;}
/* Function start: 0x431470 */ /* TODO */ void GetPilotFn1470(void){}
/* Function start: 0x406160 */ /* TODO */ void SetInputFn6160(void){}
/* Function start: 0x40E820 */ /* TODO */ void GetTbl0059b320FnE820(short i){(void)i;}
