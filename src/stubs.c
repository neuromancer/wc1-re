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
