/*
 *  ============================================================================
 *  |                            WINGLEADER                                    |
 *  |                  The 3D space combat simulator                           |
 *  |                                                                          |
 *  |          A game of interstellar fighter conflict.                        |
 *  |                                                                          |
 *  |          (c)1989,1990 Chris Roberts.  All rights reserved.               |
 *  ============================================================================
 *
 *  Shared declarations for the Wing Commander (Kilrathi Saga, Win32)
 *  reconstruction.  The banner above is reproduced from the leaked original
 *  main-module header; the module name in that file was
 *  "MAIN GAME C SOURCE MODULE (Main loop etc.)".
 *
 *  The core is C, ported from 16-bit DOS sources.  `int` was 16 bits in the
 *  original, so nearly all game state is `short` here.  Using `int` where the
 *  original used a 16-bit type produces 32-bit operations and breaks the
 *  instruction comparison -- see AGENTS.md.
 */
#ifndef WC1_H
#define WC1_H

#ifndef WC1_ANALYSIS
#include <windows.h>
#include <ddraw.h>
#include <mmsystem.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

/* Degrees are the angular unit throughout the game core (the constant lives at
 * DAT_004631b0 in the original); the trig shims convert on the way in. */
#define WC1_DEG2RAD 0.017453292519943295

/* The DirectDraw back end releases every interface through this one shape. */
#define COM_RELEASE(p) \
    do { if ((p) != 0) { (**(void (**)(void *))(*(int *)(p) + 8))(p); (p) = 0; } } while (0)

/* --------------------------------------------------------------------------
 * Original 16-bit-era type spellings.
 * -------------------------------------------------------------------------- */
typedef short          INT16;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;
typedef signed char    INT8;

typedef struct IxSound IxSound;
typedef struct IxSample IxSample;

/* The DOS rasteriser passes this record to every drawing primitive.  Its
 * offsets are fixed by the accesses in the 0x00440C00-0x00441A8F block. */
typedef struct Viewport {
    unsigned char *pixels;          /* +0x00 */
    unsigned short *rowOffsets;     /* +0x04 */
    short left;                     /* +0x08 */
    short top;                      /* +0x0A */
    short right;                    /* +0x0C */
    short bottom;                   /* +0x0E */
    unsigned char *allocation;      /* +0x10 */
} Viewport;

/* The event manager keeps a fixed pool of doubly-linked input records.  The
 * 0x1C-byte stride is fixed by the allocator at 0x004356E0 and the link
 * accesses at 0x00435790-0x004359BF. */
typedef struct InputEvent {
    short type;                       /* +0x00 */
    short x;                          /* +0x02 */
    short y;                          /* +0x04 */
    short value;                      /* +0x06 */
    unsigned int modifiers;           /* +0x08 */
    unsigned int timestamp;           /* +0x0C */
    short primaryButton;              /* +0x10 */
    short secondaryButton;            /* +0x12 */
    struct InputEvent *next;           /* +0x14 */
    struct InputEvent *previous;       /* +0x18 */
} InputEvent;

/* Public event records retain the packed layout of the 16-bit event-manager
 * API.  In particular, both 32-bit fields are intentionally unaligned. */
#pragma pack(push, 1)
typedef struct InputEventState {
    short type;                       /* +0x00 */
    unsigned int value;               /* +0x02 */
    unsigned int timestamp;           /* +0x06 */
    short modifiers;                  /* +0x0A */
    short x;                          /* +0x0C */
    short y;                          /* +0x0E */
} InputEventState;
#pragma pack(pop)

/* One sampled joystick position and its button mask. */
typedef struct InputDeviceSample {
    int x;
    int y;
    unsigned int buttons;
} InputDeviceSample;

/* Linear 8-bit raster target and its active clipping rectangle. */
typedef struct RasterSurface {
    unsigned char *pixels;
    int maximumX;
    int maximumY;
    int field_C;
    int field_10;
} RasterSurface;

typedef struct RasterClip {
    RasterSurface *surface;
    int left;
    int top;
    int right;
    int bottom;
} RasterClip;

/* One scratch glyph bitmap used while drawing the packed game fonts.  The
 * renderer stores a pointer-to-pointer to this record in TextContext because
 * the DOS implementation could move the backing allocation. */
typedef struct FontWorkspace {
    int width;
    int height;
    unsigned char *pixels;
} FontWorkspace;

/* Packed text renderer state.  The Win32 port retained the DOS byte layout:
 * the draw colour is at +0x0C, the optional text pointer at +0x0E and the
 * horizontal alignment byte at +0x16. */
#pragma pack(push, 1)
typedef struct TextContext {
    Viewport *viewport;              /* +0x00 */
    short cursorX;                   /* +0x04 */
    short cursorY;                   /* +0x06 */
    unsigned char *font;             /* +0x08 */
    unsigned char colour;            /* +0x0C */
    unsigned char backgroundColour;  /* +0x0D */
    char *text;                      /* +0x0E */
    char *textCursor;                /* +0x12 */
    unsigned char alignment;         /* +0x16 */
    FontWorkspace **fontWorkspace;   /* +0x17 */
} TextContext;

/* Saved background and text state for the centred modal message panel.  The
 * unaligned Viewports are inherited from the packed DOS layout. */
typedef struct ModalTextPanel {
    TextContext context;              /* +0x00 */
    Viewport savedBackground;         /* +0x1B */
    Viewport viewport;                /* +0x2F */
    TextContext *previousContext;     /* +0x43 */
    short left;                       /* +0x47 */
    short top;                        /* +0x49 */
    short right;                      /* +0x4B */
    short bottom;                     /* +0x4D */
} ModalTextPanel;

/* One flashing cockpit/VDU message.  Two adjacent records begin at
 * 0x005A7DD0; the unaligned text pointer at +0x0D is intentional. */
typedef struct HudMessageSlot {
    TextContext *context;            /* +0x00 */
    short x;                         /* +0x04 */
    short y;                         /* +0x06 */
    unsigned short colour;           /* +0x08 */
    unsigned short drawColour;       /* +0x0A */
    signed char flashCount;          /* +0x0C */
    char *text;                      /* +0x0D */
} HudMessageSlot;
#pragma pack(pop)

/* Runtime wave-cache node.  The name pointer and next link are established by
 * the allocation/free paths at 0x0042B1F0 and 0x0042B300; the sample pointer
 * at +0x08 is established by playWAVE. */
typedef struct WaveTableEntry {
    char *name;
    int field_4;
    void *sample;
    int field_c;
    struct WaveTableEntry *next;
} WaveTableEntry;

/* Metadata for allocations surrounded by the 0x400-byte 0xAB guard regions
 * checked by ReportHeapGuardCorruption. */
typedef struct GuardedAllocation {
    void *block;
    unsigned int size;
    struct GuardedAllocation *next;
} GuardedAllocation;

/* Win32 developer console.  The constructor/destructor and member-call ABI in
 * the shipped image identify this small utility as C++ even though the game
 * itself remains C.  The 0x400-byte formatting area fixes the complete 0x45C
 * object layout used by the 0x0041C760 debug-overlay unit. */
typedef struct DebugOverlayConsole {
    int columns;                         /* +0x000 */
    int rows;                            /* +0x004 */
    int cursorColumn;                    /* +0x008 */
    int cursorRow;                       /* +0x00C */
    HWND window;                         /* +0x010 */
    char formatBuffer[0x400];            /* +0x014 */
    char *textBuffer;                    /* +0x414 */
    unsigned char *dirtyLines;           /* +0x418 */
    int characterWidth;                  /* +0x41C */
    int characterHeight;                 /* +0x420 */
    int busyWait;                        /* +0x424 */
    int reverseVideo;                    /* +0x428 */
    COLORREF textColor;                  /* +0x42C */
    COLORREF backgroundColor;            /* +0x430 */
    int backgroundMode;                  /* +0x434 */
    int field_438;                       /* +0x438 */
    int field_43c;                       /* +0x43C */
    int field_440;                       /* +0x440 */
    int field_444;                       /* +0x444 */
    int field_448;                       /* +0x448 */
    HANDLE mutex;                        /* +0x44C */
    int animationState;                  /* +0x450 */
    int spinnerIndex;                    /* +0x454 */
    char *spinnerCharacters;             /* +0x458 */
#ifdef __cplusplus
    DebugOverlayConsole(HINSTANCE module, HWND targetWindow,
                        int columnCount, int rowCount, int waitMode);
    ~DebugOverlayConsole();
    void Clear(void);
    void Scroll(void);
    void DrawPendingLines(void);
    char WaitForKey(void);
    void EnableReverseVideo(void);
    void DisableReverseVideo(void);
    void SetOverlayTextColor(int red, int green, int blue);
    void SetOverlayBackgroundColor(int red, int green, int blue);
    void SetTransparentBackground(void);
    void SetOpaqueBackground(void);
#endif
} DebugOverlayConsole;

/*
 * The DOS source spelled `BOOLEAN window_colored = FALSE;`, but <windows.h>
 * already provides BOOLEAN (as BYTE) and MSVC 4.2 rejects a redefinition with a
 * different base type.  The Win32 port therefore has to be using the windows.h
 * one, so do not redeclare it here.  TRUE/FALSE likewise come from windows.h.
 */
#ifdef WC1_ANALYSIS
typedef unsigned char BOOLEAN;
#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif
#endif

/* --------------------------------------------------------------------------
 * Enumerations named by the leaked main-module source.  The tag names and the
 * first enumerator of each are original:
 *     GAME_MODE   Game_Mode   = Player;
 *     GAME_STATUS Game_Status = ALIVE;
 * The remaining enumerators are NOT yet recovered -- do not invent values that
 * the assembly has not confirmed.
 * -------------------------------------------------------------------------- */
typedef enum {
    Player = 0
    /* TODO: remaining GAME_MODE enumerators unknown */
} GAME_MODE;

typedef enum {
    ALIVE = 0
    /* TODO: remaining GAME_STATUS enumerators unknown */
} GAME_STATUS;

/* --------------------------------------------------------------------------
 * Shared utility layer.  These were recovered by ranking the call graph by
 * fan-in; the addresses are the originals.
 * -------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C" {
#endif

/* Variadic printers cannot be generated mechanically, so they live here. */
void ShowOnScreenMessage(int flags, short duration, const char *fmt, ...);
void SoundDebugPrintf(const char *fmt, ...);   /* 0x00403DB0 */
void SystemDebugPrintf(const char *fmt, ...);  /* 0x00425BB0 */

#ifndef WC1_ANALYSIS
#include "wcdata.h"
#include "globals.h"
#include "wc1funcs.h"
#include "wc1extern.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* WC1_H */
