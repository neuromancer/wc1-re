/**
 * @file
 * @ingroup headers
 * @brief Common types, macros and utility prototypes.
 *
 * wc1.h adds the data headers after these; the Win32 shell (winmain.c)
 * includes them itself because it declares main.c's option flags with its
 * own types.
 */
#ifndef WC1BASE_H
#define WC1BASE_H

#ifdef SDL_PORT
#include "wc1sdl.h"
#endif

#ifndef ANALYSIS_BUILD
#ifndef SDL_PORT
#include <windows.h>
#include <ddraw.h>
#include <mmsystem.h>
/* These standard OEM VK names are absent from the VC++ 4.20 SDK. */
#ifndef VK_OEM_COMMA
#define VK_OEM_COMMA 0xbc
#endif
#ifndef VK_OEM_PERIOD
#define VK_OEM_PERIOD 0xbe
#endif
#define KEY_MESSAGE_SCAN_MASK 0x00ff0000
#include <conio.h>
#include <direct.h>
#include <io.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

/* Degrees are the angular unit throughout the game core (the constant lives at
 * DAT_004631b0 in the original); the trig shims convert on the way in. */
#define DEGREES_TO_RADIANS 0.017453292519943295

/* The DirectDraw back end uses this release-and-clear shape for its surfaces. */
#define COM_RELEASE(surface) \
    do { if ((surface) != 0) { IDirectDrawSurface_Release(surface); (surface) = 0; } } while (0)

/* --------------------------------------------------------------------------
 * Shared packet declarations and the retained DOS word spelling.
 * -------------------------------------------------------------------------- */
struct RLEShape;
typedef unsigned short UINT16;
/* Heterogeneous packet lists hold pointer slots, including unaligned slots
 * in packed runtime records. Shapes are opaque to the C++ audio/console units. */
typedef void *PackedResourcePointer
#ifdef SDL_PORT
    __attribute__((aligned(1)))
#endif
;
struct Shape;

typedef struct IxSound IxSound;
typedef struct IxSample IxSample;

/* The game-side wave unit inspects the leading public fields and invokes the
 * selected methods declared here.  The IX implementation owns the complete
 * C++ object layout. */
struct IxSample {
    unsigned int flags;
#ifdef __cplusplus
    int ix_sample_load_wav(void *data, int bytes);
#endif
};

struct IxSound {
    unsigned int flags;
    IxSample *sample;
#ifdef __cplusplus
    void ix_system_sound_set_volume(unsigned short volume);
    void ix_system_sound_set_pan(unsigned short pan);
    void ix_sound_set_delete_on_stop(int enabled);
#endif
};

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

#ifdef SDL_PORT
/* The original rasterisers form this pointer before applying their vertical
 * clip. Avoid an otherwise unused out-of-range table read under sanitizers. */
#define SPRITE_ROW_OFFSET(viewport, row) \
    ((row) < (viewport)->top || (row) > (viewport)->bottom \
         ? 0 \
         : (viewport)->rowOffsets[row])
#else
#define SPRITE_ROW_OFFSET(viewport, row) viewport->rowOffsets[row]
#endif

/* Loaded shape packet: a leading dword followed by a variable-length offset
 * table. Offsets are relative to the packet, and the low word of its first
 * entry also marks the end of the table. The one-element tail is the
 * variable-array spelling supported by VC++ 4.20. */
#ifndef __cplusplus
struct Shape {
    unsigned int field_0;            /* +0x00: not read by the shape helpers */
    int frameOffsets[1];             /* +0x04 */
};
#endif

/* The event manager snapshots these 28 bytes with seven MOVSD operations.
 * The unaligned pointer fields and reserved spans are fixed by the original
 * addresses at 0x0059AB10-0x0059AB2B. */
#pragma pack(push, 1)
typedef struct MouseCursorState {
    short x;                             /* +0x00 */
    short y;                             /* +0x02 */
    unsigned char primaryButton;         /* +0x04 */
    unsigned char secondaryButton;       /* +0x05 */
    unsigned char reserved;              /* +0x06 */
    unsigned short flags;                /* +0x07 */
    struct Shape *shape;                 /* +0x09 */
    unsigned short frame;                /* +0x0D */
    unsigned int reservedAfterFrame;     /* +0x0F */
    Viewport *viewport;                  /* +0x13 */
    unsigned int reservedAfterViewport;  /* +0x17 */
    unsigned char shapeChanged;          /* +0x1B */
} MouseCursorState;
#pragma pack(pop)

#ifndef SDL_PORT
typedef char MouseCursorState_size_must_be_0x1c[
    sizeof(MouseCursorState) == 0x1c ? 1 : -1];
#endif

/* PC set-1 scan codes. MainWindowProc discards the extended-key prefix;
 * navigation/keypad keys and the two Ctrl/Alt keys therefore share codes.
 * These are distinct from Windows VK_* values and SDL scan codes. */
#define SCAN_NONE            0x00
#define SCAN_ESCAPE          0x01
#define SCAN_1               0x02
#define SCAN_2               0x03
#define SCAN_3               0x04
#define SCAN_4               0x05
#define SCAN_5               0x06
#define SCAN_6               0x07
#define SCAN_7               0x08
#define SCAN_8               0x09
#define SCAN_9               0x0a
#define SCAN_0               0x0b
#define SCAN_MINUS           0x0c
#define SCAN_EQUALS          0x0d
#define SCAN_BACKSPACE       0x0e
#define SCAN_TAB             0x0f
#define SCAN_Q               0x10
#define SCAN_W               0x11
#define SCAN_E               0x12
#define SCAN_R               0x13
#define SCAN_T               0x14
#define SCAN_Y               0x15
#define SCAN_U               0x16
#define SCAN_I               0x17
#define SCAN_O               0x18
#define SCAN_P               0x19
#define SCAN_LEFTBRACKET     0x1a
#define SCAN_RIGHTBRACKET    0x1b
#define SCAN_RETURN          0x1c
#define SCAN_CONTROL         0x1d
#define SCAN_A               0x1e
#define SCAN_S               0x1f
#define SCAN_D               0x20
#define SCAN_F               0x21
#define SCAN_G               0x22
#define SCAN_H               0x23
#define SCAN_J               0x24
#define SCAN_K               0x25
#define SCAN_L               0x26
#define SCAN_SEMICOLON       0x27
#define SCAN_APOSTROPHE      0x28
#define SCAN_GRAVE           0x29
#define SCAN_LSHIFT          0x2a
#define SCAN_BACKSLASH       0x2b
#define SCAN_Z               0x2c
#define SCAN_X               0x2d
#define SCAN_C               0x2e
#define SCAN_V               0x2f
#define SCAN_B               0x30
#define SCAN_N               0x31
#define SCAN_M               0x32
#define SCAN_COMMA           0x33
#define SCAN_PERIOD          0x34
#define SCAN_SLASH           0x35
#define SCAN_RSHIFT          0x36
#define SCAN_KP_MULTIPLY     0x37
#define SCAN_ALT             0x38
#define SCAN_SPACE           0x39
#define SCAN_CAPSLOCK        0x3a
#define SCAN_F1              0x3b
#define SCAN_F2              0x3c
#define SCAN_F3              0x3d
#define SCAN_F4              0x3e
#define SCAN_F5              0x3f
#define SCAN_F6              0x40
#define SCAN_F7              0x41
#define SCAN_F8              0x42
#define SCAN_F9              0x43
#define SCAN_F10             0x44
#define SCAN_NUMLOCK         0x45
#define SCAN_SCROLLLOCK      0x46
#define SCAN_HOME            0x47
#define SCAN_UP              0x48
#define SCAN_PAGEUP          0x49
#define SCAN_KP_MINUS        0x4a
#define SCAN_LEFT            0x4b
#define SCAN_KP_5            0x4c
#define SCAN_RIGHT           0x4d
#define SCAN_KP_PLUS         0x4e
#define SCAN_END             0x4f
#define SCAN_DOWN            0x50
#define SCAN_PAGEDOWN        0x51
#define SCAN_INSERT          0x52
#define SCAN_DELETE          0x53
#define SCAN_NONUS_BACKSLASH 0x56
#define SCAN_F11             0x57
#define SCAN_F12             0x58
#define SCAN_RELEASE_FLAG    0x80
#define SCAN_CODE_COUNT      0x80

/* Event IDs are values, not bit masks. */
#define INPUT_EVENT_NONE             0
#define INPUT_EVENT_BUTTON_UP        1
#define INPUT_EVENT_BUTTON_DOWN      2
#define INPUT_EVENT_KEY_DOWN         3
#define INPUT_EVENT_KEY_UP           4
#define INPUT_EVENT_KEY_REPEAT       5
#define INPUT_EVENT_JOYSTICK_MOVE     6
/* These legacy events have no producer in the shipped Win32 code.
 * Menus accept event 10 alongside button-down; the roles of 7-9 are unknown. */
#define INPUT_EVENT_LEGACY_7         7
#define INPUT_EVENT_LEGACY_8         8
#define INPUT_EVENT_LEGACY_9         9
#define INPUT_EVENT_LEGACY_ACTIVATE 10
#define INPUT_EVENT_MOUSE_MOVE      13
#define INPUT_EVENT_FILTER_ALL    0xff

/* QueueInputEvent's modifier bits differ from both MK_* and button masks. */
#define INPUT_MOD_BUTTON_EVENT     0x0001
#define INPUT_MOD_PRIMARY_BUTTON   0x0002
#define INPUT_MOD_SECONDARY_BUTTON 0x0004
#define INPUT_MOD_SHIFT            0x00e0
#define INPUT_MOD_ALT              0x0700
#define INPUT_MOD_CONTROL          0x2000

#define INPUT_BUTTON_PRIMARY       1
#define INPUT_BUTTON_SECONDARY     2
#define INPUT_BUTTON_BOTH          3
#define INPUT_DOUBLE_CLICK         3

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

/* The packet's frames start with four signed extents; their byte offsets
 * need not be aligned. The one-element tail names the variable-length
 * encoded scanline stream beginning at +8. */
#pragma pack(push, 1)
#ifndef __cplusplus
struct ShapeFrame {
    short right;                    /* +0x00 */
    short left;                     /* +0x02 */
    short top;                      /* +0x04 */
    short bottom;                   /* +0x06 */
    unsigned char commands[1];      /* +0x08 */
};
#endif
#pragma pack(pop)

/* One transformed corner used by the hand-written RLE scan converter.  The
 * third word is deliberately unused; the retail workspace advances through
 * these records with a 20-byte stride. */
struct RLETransformVertex {
    int destinationX;
    int destinationY;
    int reserved;
    int sourceX;
    int sourceY;
};

/* Prepared renderer cache, identified by the literal "1.00" at 0x00470D30.
 * Each frame has an image offset followed by an optional palette offset.
 * Both offsets are relative to the cache, not native pointers. */
#ifndef __cplusplus
struct RLEShape {
    char version[4];                 /* +0x00 */
    int frameCount;                  /* +0x04 */
    int frameOffsets[1][2];          /* +0x08: image, palette */
};
#endif

/* Cached shape frames use the renderer's row-oriented RLE representation.
 * The four signed bounds are relative to the shape origin; the byte stream
 * for the first row begins immediately after this 24-byte header. */
typedef struct RLEFrameHeader {
    short height;
    short width;
    short topExtent;
    short leftExtent;
    int left;
    int top;
    int right;
    int bottom;
}
#ifdef SDL_PORT
__attribute__((packed))
#endif
RLEFrameHeader;

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

/* One of six numeric cockpit readouts.  The packed ten-byte stride is used
 * directly by the original renderer at 0x00413F70-0x0041409C. */
typedef struct CockpitReadout {
    TextContext *context;           /* +0x00 */
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    short previousRight;            /* +0x08 */
} CockpitReadout;

/* Saved background and text state for the centred modal message panel.  The
 * unaligned Viewports are inherited from the packed DOS layout. */
#ifdef SDL_PORT
/* Runtime-only state: pointers passed to viewport routines need host alignment. */
#pragma pack(push, 8)
#endif
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
#ifdef SDL_PORT
#pragma pack(pop)
#endif

/* One flashing cockpit/VDU message.  Two adjacent records begin at
 * 0x005A7DD0; the unaligned text pointer at +0x0D is intentional. */
typedef struct HudMessageSlot {
    TextContext *context;            /* +0x00 */
    short x;                         /* +0x04 */
    short y;                         /* +0x06 */
    short colour;                    /* +0x08 */
    short drawColour;                /* +0x0A */
    signed char flashCount;          /* +0x0C */
    const char *text;                /* +0x0D */
} HudMessageSlot;

/* The killer-board room menu.  Its pointers sit at 2 mod 4 in the original
 * (0x00598AB2-0x00598ABA) and its text context at 0x00598ABE: LINK aligns
 * every separate common of four or more bytes, so only one packed aggregate
 * reproduces this layout. */
typedef struct RoomMenu {
    short cursorFrame;                     /* +0x00 */
    const struct TitleMenuRegion *regions; /* +0x02 */
    char **labels;                         /* +0x06 */
    const char *currentLabel;              /* +0x0A */
    TextContext text;                      /* +0x0E */
} RoomMenu;
#pragma pack(pop)

/* The record InitializeDIBScreenViewport fills at 0x005A66E0 and points
 * 0x005A66EC at.  The pointer's offset fixes the record at twelve bytes;
 * nothing in the shipped executable reads its third word. */
typedef struct ScreenAllocation {
    int state;          /* +0x00 */
    Viewport *viewport; /* +0x04 */
    int field_8;        /* +0x08 */
} ScreenAllocation;

#ifndef SDL_PORT
typedef char CockpitReadout_size_must_be_0x0a[
    sizeof(CockpitReadout) == 0x0a ? 1 : -1];
typedef char RoomMenu_size_must_be_0x29[
    sizeof(RoomMenu) == 0x29 ? 1 : -1];
#endif

/* Runtime wave-cache node.  The name pointer and next link are established by
 * the allocation/free paths at 0x0042B1F0 and 0x0042B300; the sample pointer
 * at +0x08 is established by playWAVE. */
typedef struct WaveTableEntry {
    char *name;
    int field_4;
    IxSample *sample;
    int field_c;
    struct WaveTableEntry *next;
} WaveTableEntry;

/* One transient IX sound and its link in the game-side active-sound list. */
typedef struct ActiveSoundEntry {
    IxSound *sound;
    struct ActiveSoundEntry *next;
} ActiveSoundEntry;

/* Metadata preceding a tagged allocation, outside the on-disk packet.
 * The SDL port stores its wider cache pointer before the legacy prefix. */
#ifndef __cplusplus
struct TaggedAllocationHeader {
#ifdef SDL_PORT
    struct RLEShape *preparedShape;
#endif
    unsigned int signature;         /* packet - 8: 'jeff' */
#ifdef SDL_PORT
    unsigned int reserved;
#else
    struct RLEShape *preparedShape;   /* packet - 4 */
#endif
};
#endif

/* Metadata for allocations surrounded by the 0x400-byte 0xAB guard regions
 * checked by ReportHeapGuardCorruption. */
struct GuardedAllocation {
    void *block;
    unsigned int size;
    struct GuardedAllocation *next;
};

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
#if defined(ANALYSIS_BUILD) && !defined(SDL_PORT)
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

#ifdef __cplusplus
}
#endif

#endif /* WC1BASE_H */
