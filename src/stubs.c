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
/* Function start: 0x40BFF0 */ /* TODO */ void EnterNavPoint(short a){(void)a;}
/* Function start: 0x4117D0 */ /* TODO */ void DispatchCommMenuCommand(void *a,int b,short c){(void)a;(void)b;(void)c;}
/* Function start: 0x414180 */ /* TODO */ void ClearHudMessageSlot(int *p){(void)p;}
short g_nTargetFacing_0059d52a;
/* Function start: 0x4142B0 */ /* TODO */ void ShowHudMessageUnlessDuplicate(int a,unsigned short b,int c){(void)a;(void)b;(void)c;}
/* Function start: 0x414BF0 */ /* TODO */ void ReportComponentHit(char a,char b,char c){(void)a;(void)b;(void)c;}
/* Function start: 0x416260 */ /* TODO */ void DrawWrappedCommText(char *a,unsigned char b){(void)a;(void)b;}
char *PTR_s_MISSILE_LOCKED_004691d4[8];
/* Function start: 0x416DE0 */ /* TODO */ void SetHudMessageText(void *a,unsigned short b,unsigned short c){(void)a;(void)b;(void)c;}
/* Function start: 0x416E20 */ /* TODO */ void CombatRoutine02(short a,int b,unsigned int c,int d,int e){(void)a;(void)b;(void)c;(void)d;(void)e;}
/* Function start: 0x417260 */ /* TODO */ void DrawStickIndicator(void){}
/* Function start: 0x417760 */ /* TODO */ void DrawHudMessageList(void) {}
/* Function start: 0x41C910 */ /* TODO */ void DebugOverlayThreadProc(int p){(void)p;}
/* Function start: 0x41CD40 */
/* TODO: PumpMessagesWhileDebugPaused -- GetMessage/Translate/Dispatch loop. */
void PumpMessagesWhileDebugPaused(unsigned int arg)
{
    (void)arg;
}

/* Forward references still to be implemented; each is a real function. */
/* Function start: 0x41DAA0 */ /* TODO */ short UiRoutine03(void){return 1;}
/* Function start: 0x421100 */ /* TODO */ void FindShipCommEntry(short a, int b) { (void)a; (void)b; }
/* Function start: 0x422780 */ /* TODO */ void ClearShipAiOrders(short s) { (void)s; }
/* Function start: 0x424490 */ /* TODO */ void ReleaseSceneActors(void){}
/* Function start: 0x424BE0 */ /* TODO */ void ResetSeriesProgress(void){}
/* Function start: 0x4272F0 */ /* TODO */ void LogMemoryUsage(void) {}
/* Function start: 0x4273C0 */ /* TODO */ void ShowMemoryStatusDebug(void) {}
/* Function start: 0x427C50 */ /* TODO */ void RunSimulationTick(void) {}
/* Function start: 0x428EA0 */ /* TODO */ void WaitForKeyAcknowledge(int m){(void)m;}
/* Function start: 0x428F20 */ /* TODO */ void ShowModalMessage(const char *s) { (void)s; }
/* Function start: 0x428FA0 */ /* TODO */ void ShowOnScreenMessage(short f, short d, const char *fmt, ...) { (void)f; (void)d; (void)fmt; }
/* Function start: 0x429DD0 */ /* TODO */ void RunFrameUpdate(void) {}
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
