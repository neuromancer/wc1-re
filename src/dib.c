/*
 *  DirectDraw video layer -- the DIB* family.
 *
 *  LIKELY A REAL COMPILATION UNIT: 0x00432140 - 0x0043324F, 15 contiguous
 *  functions, and most carry their ORIGINAL names (recovered from their own
 *  diagnostic strings, e.g. "DIBcascade   CreatePalette").  This is the first
 *  file here that is probably a genuine module rather than a scratch grouping.
 *
 *  Interfaces are used through raw vtable slots exactly as the original does:
 *  IUnknown::Release is +0x08, IDirectDraw2::RestoreDisplayMode +0x4C,
 *  SetCooperativeLevel +0x50, SetDisplayMode +0x54, WaitForVerticalBlank +0x58.
 */
#include "wc1.h"

/* COM interface pointers (raw, so the vtable offsets stay visible). */
int  *DAT_0046b1a4;      /* IDirectDraw2   */
int  *DAT_0046b1a8;      /* primary surface */
int  *DAT_0046b1ac;      /* secondary surface */
int   DAT_0046b1b4;      /* cascade depth */
char  DAT_00486078[256]; /* error text scratch */
extern unsigned int DAT_00486074;   /* HWND passed to DIBinstall */
extern unsigned char DAT_00465074;

extern char *DirectDrawResultToText(int hr);   /* 0x004331F0 */
extern void  DoVideoFn2960(void);
extern void  DIBslamReal(void);                /* 0x00432970 */
extern int   DIBcascade(int mode, int *err);   /* 0x00432410 */
extern void  DIBdestroyDIB(void);              /* 0x004328A0 */

/* Release through IUnknown vtable slot +0x08, then clear -- the original
 * repeats this inline at every site rather than calling a helper. */
#define COM_RELEASE(p) \
    do { if ((p) != 0) { (**(void (**)(void *))(*(int *)(p) + 8))(p); (p) = 0; } } while (0)

/* Function start: 0x432140 */
void DIBerror(const char *tag, int hr)
{
    char *text = DirectDrawResultToText(hr);

    sprintf(DAT_00486078, "ERROR: %s - (%s)", tag, text);
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
}

/* Function start: 0x432230 */
void DIBpositionWindow(void)
{
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
    (**(void (**)(void *))(*DAT_0046b1a4 + 0x4c))(DAT_0046b1a4);  /* RestoreDisplayMode */
    (**(void (**)(void *))(*DAT_0046b1a4 + 8))(DAT_0046b1a4);     /* Release */
}

/* Function start: 0x4322B0 */
void DIBreInstall(void)
{
    int err;

    if (DAT_00465074 == 0) {
        DAT_0046b1b4 = -1;
    } else {
        (**(void (**)(void *, unsigned int, int))(*DAT_0046b1a4 + 0x50))
            (DAT_0046b1a4, DAT_00486074, 0x13);                   /* SetCooperativeLevel */
        if (DIBcascade(-2, &err) == 0)
            DIBerror("DIBreInstall   DIBcascade Failure", err);
    }
    DoVideoFn2960();
    DIBslamReal();
}

/* Function start: 0x432680 */
void GetVideoFn2680(void)
{
    DIBdestroyDIB();
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
    (**(void (**)(void *))(*DAT_0046b1a4 + 0x4c))(DAT_0046b1a4);
    (**(void (**)(void *))(*DAT_0046b1a4 + 8))(DAT_0046b1a4);
}

/* Palette shadow written by the DIBset*Palette family (R at +2, G at +1, B at +0). */
unsigned char DAT_00486110[1024];
