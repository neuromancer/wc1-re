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

/* Function start: 0x401210 */ /* TODO */ void ReleaseAllSfxSlots(void){}
/* Function start: 0x401E30 */
/* TODO: WinMain -- real one at 0x00401E30 (single-instance semaphore, memory
   check, window creation, main loop). Stubbed so WinMainCRTStartup resolves. */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nShowCmd)
{
    (void)hInstance; (void)hPrevInstance; (void)lpCmdLine; (void)nShowCmd;
    return 0;
}


/* Function start: 0x402110 */ /* TODO */ void AbortToDesktop(void) {}
/* Function start: 0x402320 */ /* TODO */ void PumpWindowMessages(void) {}
/* Function start: 0x403C40 */ /* TODO */ void SplitGameClockToHms(void *p){(void)p;}
/* Function start: 0x403DE0 */ /* TODO */ void MonoDebug_print(const char *s){(void)s;}
/* Function start: 0x406D80 */ /* TODO */ void ShipAiState40(short a,short b){(void)a;(void)b;}
/* Function start: 0x406FB0 */ /* TODO */ void ShipAiState29(short a,short b){(void)a;(void)b;}
/* Function start: 0x407270 */ /* TODO */ void ShipAiRoutine02(short a,unsigned int b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x409F80 */ /* TODO */ void StepShipOrderState(short i){(void)i;}
/* Function start: 0x40A030 */ /* TODO */ void StepShipOrderWithLeader(short i){(void)i;}
/* Function start: 0x40A360 */ /* TODO */ void StepShipOrderForNav(short a) { (void)a; }
/* Function start: 0x40BFF0 */ /* TODO */ void EnterNavPoint(short a){(void)a;}
/* Function start: 0x40F940 */ /* TODO */ void free_viewport(void *p) { (void)p; }
/* Function start: 0x4117D0 */ /* TODO */ void DispatchCommMenuCommand(void *a,int b,short c){(void)a;(void)b;(void)c;}
/* Function start: 0x413CE0 */ /* TODO */ void FatalErrorAndExit(const char *s) { (void)s; }
/* Function start: 0x414180 */ /* TODO */ void ClearHudMessageSlot(int *p){(void)p;}
short DAT_0059d52a;
/* Function start: 0x4142B0 */ /* TODO */ void ShowHudMessageUnlessDuplicate(int a,unsigned short b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x4148A0 */ /* TODO */ void PushSeriesStateWord(short i,int v){(void)i;(void)v;}
/* Function start: 0x414BF0 */ /* TODO */ void ReportComponentHit(char a,char b,char c){(void)a;(void)b;(void)c;}
/* Function start: 0x4154C0 */ /* TODO */ short AnyShipQueuedToNavigate(void){return 0;}
/* Function start: 0x416260 */ /* TODO */ void DrawWrappedCommText(char *a,unsigned char b){(void)a;(void)b;}
char *PTR_s_MISSILE_LOCKED_004691d4[8];
/* Function start: 0x416DE0 */ /* TODO */ void SetHudMessageText(void *a,unsigned short b,unsigned short c){(void)a;(void)b;(void)c;}
/* Function start: 0x416E20 */ /* TODO */ void CombatRoutine02(short a,int b,unsigned int c,int d,int e){(void)a;(void)b;(void)c;(void)d;(void)e;}
/* Function start: 0x4171D0 */ /* TODO */ void ComputeStickIndicatorFrame(void){}
/* Function start: 0x417260 */ /* TODO */ void DrawStickIndicator(void){}
/* Function start: 0x417760 */ /* TODO */ void DrawHudMessageList(void) {}
/* Function start: 0x4184C0 */ /* TODO */ void FixedToShortSaturating(int v){(void)v;}
/* Function start: 0x418780 */ /* TODO */ void MakeRandomVectorFixed(int a,short b,int *p){(void)a;(void)b;(void)p;}
/* Function start: 0x419210 */ /* TODO */ short GetHeadingErrorToPoint(short a,void *p){(void)a;(void)p;return 0;}
/* Function start: 0x419290 */ /* TODO */ void UpdateAimSolution(short a, int *p) { (void)a; (void)p; }
/* Function start: 0x419310 */ /* TODO */ void ShipAiRoutine04(short a, short b) { (void)a; (void)b; }
/* Function start: 0x419660 */ /* TODO */ void AimShipAtPoint(short a,void *p){(void)a;(void)p;}
/* Function start: 0x419810 */ /* TODO */ void AimShipAtPointOffset90(short a,void *p){(void)a;(void)p;}
/* Function start: 0x419950 */ /* TODO */ void NormaliseAndScaleVector(unsigned int *p,int v){(void)p;(void)v;}
/* Function start: 0x419990 */ /* TODO */ void ScaleDeltaToRange(int *a,int *b,short c){(void)a;(void)b;(void)c;}
/* Function start: 0x419BD0 */ /* TODO */ void DestroyShip(short i) { (void)i; }
/* Function start: 0x41C910 */ /* TODO */ void DebugOverlayThreadProc(int p){(void)p;}
/* Function start: 0x41CD40 */
/* TODO: PumpMessagesWhileDebugPaused -- GetMessage/Translate/Dispatch loop. */
void PumpMessagesWhileDebugPaused(unsigned int arg)
{
    (void)arg;
}

/* Forward references still to be implemented; each is a real function. */
/* Function start: 0x41D2E0 */ /* TODO */ void FetchDiskPacketRetrying(short a,int b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x41DAA0 */ /* TODO */ short UiRoutine03(void){return 1;}
/* Function start: 0x41E750 */ /* TODO */ void ShipAiRoutine07(short a, int b) { (void)a; (void)b; }
/* Function start: 0x41E820 */ /* TODO */ void ScaleShipVelocity(short i){(void)i;}
/* Function start: 0x420AA0 */ /* TODO */ void ProcessShipCommQueue(short a){(void)a;}
/* Function start: 0x421100 */ /* TODO */ void FindShipCommEntry(short a, int b) { (void)a; (void)b; }
/* Function start: 0x422560 */ /* TODO */ int ShipAiRoutine18(short s){(void)s;return 0;}
/* Function start: 0x422780 */ /* TODO */ void ClearShipAiOrders(short s) { (void)s; }
/* Function start: 0x422C70 */ /* TODO */ void SetShipOrder(short a, int b) { (void)a; (void)b; }
/* Function start: 0x422D90 */ /* TODO */ void ShipAiRoutine21(short a, int b) { (void)a; (void)b; }
/* Function start: 0x423BA0 */ /* TODO */ void InterpolateClamped(int a,int b,unsigned int c,int d,int e){(void)a;(void)b;(void)c;(void)d;(void)e;}
/* Function start: 0x424490 */ /* TODO */ void ReleaseSceneActors(void){}
/* Function start: 0x424BE0 */ /* TODO */ void ResetSeriesProgress(void){}
/* Function start: 0x4272F0 */ /* TODO */ void LogMemoryUsage(void) {}
/* Function start: 0x4273C0 */ /* TODO */ void ShowMemoryStatusDebug(void) {}
/* Function start: 0x427C50 */ /* TODO */ void RunSimulationTick(void) {}
/* Function start: 0x428EA0 */ /* TODO */ void WaitForKeyAcknowledge(int m){(void)m;}
/* Function start: 0x428F20 */ /* TODO */ void ShowModalMessage(const char *s) { (void)s; }
/* Function start: 0x428FA0 */ /* TODO */ void ShowOnScreenMessage(short f, short d, const char *fmt, ...) { (void)f; (void)d; (void)fmt; }
/* Function start: 0x429DD0 */ /* TODO */ void RunFrameUpdate(void) {}
/* Function start: 0x42A120 */ /* TODO */ short FindNearestNavPoint(int a){(void)a;return 0;}
/* Function start: 0x42AFC0 */
/* TODO: SetMultimediaTimerCallback -- timeSetEvent/timeKillEvent wrapper. */
void SetMultimediaTimerCallback(int period)
{
    (void)period;
}
/* Function start: 0x42B300 */ /* TODO */ void FreeWaveTable(void){}
/* Function start: 0x42B640 */
/* TODO */ void StopAllSounds(void) {}
/* Function start: 0x42E090 */
/* TODO: CheckMcgaVideoMode. Ghidra's prototype shows no parameters, but the call
   site at 0x0042F7E0 passes (viewport*, short, int) -- another wrong DB prototype. */
int CheckMcgaVideoMode(int *p, short a, int b)
{
    (void)p;
    (void)a;
    (void)b;
    return 0;
}
/* Function start: 0x42EF30 */
/* TODO: PlaySfxWaveFileByNumber -- plays sfx%02i.wav. */
void PlaySfxWaveFileByNumber(int n, int a, int b)
{
    (void)n;
    (void)a;
    (void)b;
}

/* Function start: 0x42FA90 */ /* TODO */ unsigned int PushMemoryStackFrameTagged(int a,int b){(void)a;(void)b;return 0;}
/* Function start: 0x42FAE0 */ /* TODO */ void ReleasePacketHandle(int h){(void)h;}
/* Function start: 0x430CB0 */ /* TODO */ void CombatRoutine03(int a, short b) { (void)a; (void)b; }
/* Function start: 0x431470 */ /* TODO */ void EndCommSessionWithWingman(void){}
/* Function start: 0x432410 */ /* TODO */ int DIBcascade(int m,int *e){(void)m;(void)e;return 0;}
/* Function start: 0x4328A0 */ /* TODO */ void DIBdestroyDIB(void){}

/* Function start: 0x432970 */ /* TODO */ void DIBslamReal(void){}
/* Function start: 0x433060 */
/* TODO */ void DIBwholePaletteFromTriplets(unsigned char *palette) { (void)palette; }
/* Function start: 0x433120 */ /* TODO */ void DIBwholePaletteFromWords(void) {}
/* Function start: 0x4331F0 */ /* TODO */ char *DirectDrawResultToText(int hr){(void)hr;return "";}
/* Function start: 0x434F20 */ /* TODO */ long FloatToLongDirect(void){return 0;}
/* Function start: 0x435090 */ /* TODO */ void IsPointInRect(short a,short b,short *p){(void)a;(void)b;(void)p;}
/* Function start: 0x435470 */ /* TODO */ void DosStrcpy(void *d,const char *s){(void)d;(void)s;}
/* Function start: 0x435760 */ /* TODO */ void ClearEventSlotByAddress(int n) { (void)n; }
/* Function start: 0x4359C0 */ /* TODO */ void DispatchPendingEvents(int a) { (void)a; }
/* Function start: 0x4360F0 */ /* TODO */ void FlushPendingScreenText(unsigned char a, int b) { (void)a; (void)b; }
/* Function start: 0x436910 */ /* TODO */ unsigned int JoystickEdgeHook(void){return 0;}
/* Function start: 0x440C00 */ /* TODO */ void ValidateViewportBounds(int *a,int *b,unsigned int *c){(void)a;(void)b;(void)c;}
/* Function start: 0x441A90 */ /* TODO */ void CopyViewportContents(void *a, void *b) { (void)a; (void)b; }
/* Function start: 0x441AE0 */ /* TODO */ unsigned int PresentViewport(int *p,char c){(void)p;(void)c;return 0;}
/* Function start: 0x442460 */ /* TODO */ void Streamer_stop(void) {}
/* Function start: 0x4473F3 */ /* TODO */ void ix_system_shutdown(void){}
/* Function start: 0x447548 */ /* TODO */ void ix_system_delete_all_samples(void){}
/* Function start: 0x447610 */ /* TODO */ void ix_system_delete_all_sounds(void){}
