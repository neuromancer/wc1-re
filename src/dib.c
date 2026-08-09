/*
 *  DirectDraw back end.
 *
 *  Address range 0x432000-0x4333ff (provisional -- see docs/ORDER.md).
 *  Boundary evidence: PROVEN: every routine prints its own name, "DIBinstall", "DIBslamReal", ....
 */
#include "wc1.h"

/* Function start: 0x4320E0 */
void SetFpsCapEnabled(void)
{
    long v;

    DAT_0046b1c8 = 1;
    v = _ftol();
    DAT_0046b1bc = 0;
    DAT_0046b1b8 = (int)v;
}

/* Function start: 0x432110 */
void SetFpsCapDisabled(void)
{
    long v;

    DAT_0046b1c8 = 0;
    v = _ftol();
    DAT_0046b1bc = 0;
    DAT_0046b1b8 = (int)v;
}

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
    DIBslam();
    DIBslamReal();
}

/* Function start: 0x432680 */
/* Full teardown: destroy the DIB, drop the clipper and palette, restore the
 * display mode (vtable +0x4c) and release DirectDraw itself (+0x08).
 * DIBmakeDIB proper is at 0x004326E0 and is not yet reimplemented. */
void DIBunInstall(void)
{
    DIBdestroyDIB();
    COM_RELEASE(DAT_0046b1ac);
    COM_RELEASE(DAT_0046b1a8);
    (**(void (**)(void *))(*DAT_0046b1a4 + 0x4c))(DAT_0046b1a4);
    (**(void (**)(void *))(*DAT_0046b1a4 + 8))(DAT_0046b1a4);
}

/* Function start: 0x432960 */
void DIBslam(void) { DAT_00486518 = 1; }

/* Function start: 0x432DE0 */
unsigned int GetDIBSurfacePitch(void) { return DAT_00476648; }

/* Function start: 0x432E00 */
unsigned int GetDIBSurfaceBase(void) { return DAT_00486074; }

/* Function start: 0x433020 */
void SetPaletteEntryFromWords(short i, unsigned short *rgb)
{
    extern unsigned char DAT_00486110[];
    int k = i * 4;

    rgb[0] = DAT_00486110[k + 2];
    rgb[1] = DAT_00486110[k + 1];
    rgb[2] = DAT_00486110[k];
}

/* Function start: 0x4331E0 */
void ReleaseDirectDrawPaletteAgain(void)
{
    (**(void (**)(void *, int, int))(*DAT_0046b1a4 + 0x58))(DAT_0046b1a4, 1, 0);
}
