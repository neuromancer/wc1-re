/*
 *  Trivial accessors, constant returns and single-call forwarders.
 *
 *  NOT A REAL COMPILATION UNIT.  These functions live at addresses from
 *  0x00402520 to 0x00436970 and therefore belong to many different original
 *  object files.  They are collected here only so each one can be written and
 *  scored individually; they must be redistributed into their real modules once
 *  `make order` establishes the boundaries.  See docs/ORDER.md.
 *
 *  Each body is exactly what the original does -- these were classified as
 *  "body-proven" during analysis precisely because the whole function is a
 *  single statement.
 */
#include "wc1.h"

/* Globals read/written by the accessors below. */
unsigned int  DAT_004650ac;
unsigned int  DAT_005a8a40;
unsigned int  DAT_005a8964;
unsigned short DAT_00468660;
unsigned char DAT_0046af6c;
unsigned int  DAT_00486074;
unsigned char DAT_00465074;
unsigned char DAT_0059b430;

/* Forwarding targets, implemented elsewhere. */
extern void PlaySfxWaveFileByNumber(int n, int a, int b);   /* 0x0042EF30 */
extern void DIBwholePaletteFromTriplets(unsigned char *palette); /* 0x00433060 */
extern void SetMultimediaTimerCallback(int period);         /* 0x0042AFC0 */

/* Function start: 0x402520 */
unsigned int ReadDAT004650ac(void) { return DAT_004650ac; }

/* Function start: 0x402B80 */
unsigned int ReadDAT005a8a40(void) { return DAT_005a8a40; }

/* Function start: 0x403080 */
unsigned int ReadDAT005a8964(void) { return DAT_005a8964; }

/* Function start: 0x40C350 */
unsigned int ReturnConst1(void) { return 1; }

/* Function start: 0x40CBB0 */
void StoreDAT00468660(unsigned short v) { DAT_00468660 = v; }

/* Function start: 0x40FA30 */
unsigned short ReturnConst1v1(void) { return 1; }

/* Function start: 0x414AD0 */
void CallThrough42EF30(void) { PlaySfxWaveFileByNumber(0x18, -1, 0); }

/* Function start: 0x418130 */
unsigned short ReturnConst1v2(void) { return 1; }

/* Function start: 0x41DA00 */
unsigned int ReturnConst0(void) { return 0; }

/* Function start: 0x421FE0 */
unsigned int ReturnConst0v1(void) { return 0; }

/* Function start: 0x421FF0 */
unsigned int ReturnConst0v2(void) { return 0; }

/* Function start: 0x422000 */
unsigned int ReturnConst0v3(void) { return 0; }

/* Function start: 0x422130 */
/* NOT a constant return: `mov eax,0x59b430 / cmp eax,1 / sbb eax,eax / inc eax`
 * is the classic `>= 1` boolean idiom applied to the *address* 0x0059B430,
 * which is always non-zero -- hence Ghidra folding it to `return 1`.  Written
 * the way the original was, so the idiom survives. */
unsigned int ReturnConst1v3(void) { return (unsigned int)&DAT_0059b430 >= 1; }

/* Function start: 0x424CE0 */
unsigned int ReturnConst0v4(void) { return 0; }

/* Function start: 0x424CF0 */
unsigned int ReturnConst0v5(void) { return 0; }

/* Function start: 0x42CDA0 */
unsigned short ReturnConst1v4(void) { return 1; }

/* Function start: 0x42F730 */
unsigned short ReturnConst0v6(void) { return 0; }

/* Forward declarations for the tail-jump thunks below. */
unsigned int ReturnConst3E8000v2(void);
unsigned int ReturnConst3E8000v3(void);

/* Function start: 0x42FB20 */
/* Tail-jump thunk to 0x004362E0, not a constant return -- Ghidra followed the
 * jump and folded the callee's body into the display. */
__declspec(naked) void ReturnConst3E8000(void) { __asm { jmp ReturnConst3E8000v2 } }

/* Function start: 0x42FB30 */
__declspec(naked) void ReturnConst3E8000v1(void) { __asm { jmp ReturnConst3E8000v3 } }

/* Function start: 0x430DB0 */
void StoreDAT0046af6c(unsigned char v) { DAT_0046af6c = v; }

/* Function start: 0x432E00 */
unsigned int ReadDAT00486074(void) { return DAT_00486074; }

/* Function start: 0x434FD0 */
/* __stdcall forwarder that passes its argument through; `ret 4` is callee
 * cleanup.  The Ghidra label claimed a no-argument call, which was wrong. */
void __stdcall CallThrough433060(unsigned char *palette)
{
    DIBwholePaletteFromTriplets(palette);
}

/* Function start: 0x435550 */
unsigned short ReturnConst0v7(void) { return 0; }

/* Function start: 0x4361E0 */
void CallThrough42AFC0(short period) { SetMultimediaTimerCallback((int)period); }

/* Function start: 0x436260 */
unsigned short ReturnConst1v5(void) { return 1; }

/* Function start: 0x4362B0 */
unsigned int ReturnConst0v9(void) { return 0; }

/* Function start: 0x4362E0 */
unsigned int ReturnConst3E8000v2(void) { return 0x3e8000; }

/* Function start: 0x4362F0 */
unsigned int ReturnConst3E8000v3(void) { return 0x3e8000; }

/* Function start: 0x4368E0 */
unsigned short ReturnConst8000(void) { return 0x8000; }
