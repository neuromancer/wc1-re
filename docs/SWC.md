# Super Wing Commander Mac demo

The first SDL2 support slice is an asset preview, built with `make modern-swc`.
It loads the Mac demo's CMF files, decodes its sprites, reads its resource-fork
palette, and displays the cockpit and 13 ship shape sets at 320x240. Left/right
select a ship view, up/down select a ship, C toggles the cockpit, and Esc exits.
Missions, flight controls, HUD animation, audio, and movies are not connected yet.

`src/swc/` contains reusable resource readers. `src/sdl/swc_demo.c` is the temporary
SDL2 presentation/inspection entry point. Neither is linked into `WC1.EXE` or
the existing `wc1-modern` executable. The build needs the host C compiler and
SDL2; it does not require Slint, LZO, the classic Mac runtime, or a Mac compiler.

## Run

Point `SWC_DATA_DIR` at the extracted **SuperWing DEMO** directory containing
`CMFs/`. For example, from the repository root on macOS:

```sh
unar -o data/swc-demo ../releases/mac/SuperWing_DEMO.sit
make run-modern-swc SWC_DATA_DIR="data/swc-demo/SuperWing DEMO"
```

The viewer first tries `SuperWingCommanderDemo.rsrc` inside that directory,
then the native resource fork of `Super Wing Commander Demo`. The palette is
`clut` ID 251, named `pcshipv00`. A stripped PEF executable does not contain it.
To supply an already extracted **raw** resource fork:

```sh
out-modern/swc-demo "/path/to/SuperWing DEMO" \
    --rsrc ../releases/mac/extracted/SuperWingCommanderDemo.rsrc
```

AppleDouble sidecars (`._...`, or `.rsrc` from `unar -forks visible`) are containers,
not raw resource forks, and are not currently supported. On macOS, `unar`'s
default fork-preserving extraction works directly. Paths and filenames currently
use the spelling in the demo archive. Game assets stay outside version control.

`--check` presents one frame and exits. `--snapshot FILE.bmp` also saves that
320x240 frame and exits. The check runs with SDL's dummy video driver:

```sh
make modern-test-swc
make modern-test-swc-data SWC_DATA_DIR="/path/to/SuperWing DEMO" \
    SWC_ARGS="--rsrc /path/to/SuperWingCommanderDemo.rsrc"
SDL_VIDEODRIVER=dummy out-modern/swc-demo "/path/to/SuperWing DEMO" \
    --rsrc /path/to/SuperWingCommanderDemo.rsrc \
    --snapshot out-modern/swc-preview.bmp
```

The data-free tests cover both RLE commands including 128-byte runs, truncation,
output overrun, endianness, frame padding, signed offsets, empty frames, CMF
table bounds, stale handles, and resource-map/color-table bounds. The corpus
check opens every supplied CMF and decodes every compressed chunk as a sprite
set, a property verified for this demo rather than assumed by the resource API.
The known demo contains 23 CMFs, 1,011 chunks, 267 compressed sprite sets,
3,446 frames, and 18,315,140 decoded chunk bytes. Development builds use ASan
and UBSan, like the existing SDL2 target.

## Recovered format contracts

These contracts come from the original 68000 instructions and actual demo data.
Ghidra's C output alone is insufficient: classic Mac A-line traps truncate some
decompilations, and `GetFramePtr` returns its pointer in A0.

| Data | Contract | Original evidence |
| --- | --- | --- |
| CMF header | `CMF1`; LE32 TOC offset/length at 12/16 | CODE_09 `CMOpenFile` +0x26ae |
| CMF TOC | LE32 relative type-table offset; type/count groups; 16-byte entries | CODE_09 `CMReformatTOC` +0x2fac |
| CMF entry | ID, file offset, flags/size, old handle; length mask `0x00ffffff`; compressed bit `0x10000000` | CODE_09 `CMGetChunk` +0x22f6 |
| Compressed chunk | BE32 output length, then byte RLE | CODE_09 `CMDecompressRLE` +0x2b26 |
| RLE | `(control & 127) + 1` bytes; high bit repeats the next byte, otherwise literal copy | CODE_09 +0x2ba4..+0x2bfe |
| Sprite set | BE32 count; each frame has a 14-byte header plus payload padded to a multiple of four | CODE_01 `GetFramePtr` +0x05f4 |
| Frame | BE16 encoding, signed x/y, width/height; BE32 payload length | CODE_01 `GetFramePtr` and `DrawRLEImage` |
| Pixels | Encoding 1 is raw; 2 is byte RLE; palette index 0 is transparent | CODE_01 `DrawRLEImage` +0x17aa |
| Palette | Classic resource-fork `clut`, 16-bit RGB channels reduced to their high byte | Demo resource fork, clut/251 |

The host ignores persisted Mac handles and cache state. Compressed allocations
are limited to 64 MiB and impossible compression ratios are rejected. Frame
payloads borrow the decoded set; owned buffers and CMFs have explicit lifetimes.
The viewer centers extracted ship images for inspection. It does not claim to
reproduce flight projection, sprite hotspots, scaling, or original draw order.

## Binary similarity

Use **binary-similarity** for SWC-specific modules, as requested. WC1's existing
`binary-comp` workflow remains authoritative for the Win32 reconstruction.
The requested SWC tool has not yet been located in this environment, so **no
SWC similarity measurements are available yet**. Passing decoder tests is not
an assembly or semantic similarity score.

[`config/swc-provenance.json`](../config/swc-provenance.json) records reference
hashes, CODE-resource offsets, source symbols, and differences in the host
adaptations. It is an evidence inventory, not a guessed tool configuration.
Keep CODE-resource offsets distinct from WC1 virtual addresses and Ghidra's
temporary import base. Ordinary `Function start:` annotations are reserved for
the WC1 executable so its export and verification tools do not mix binaries.

Once the tool location and interface are known, add its actual configuration
and reproducible report command. Record reference and rebuilt architecture,
compiler/version/flags, binary hashes, compared symbol spans, tool version,
and raw scores. Compare uninstrumented output in a separate build directory;
ASan/UBSan calls would otherwise measure instrumentation as well as game code.
The Mac reference is 68000 big endian and the current host is arm64, so the
tool must support that comparison or a suitable reference/compiler pairing.

The separate uninstrumented build can already be prepared without changing
the normal diagnostic build:

```sh
make modern-swc MODERN_RELEASE=1 MODERN_OUT_DIR=out-modern/swc-similarity
```

The current resource readers are host adaptations with extra bounds checks and
different memory APIs. `SwcDecodeFrame` only uses `DrawRLEImage` as format
evidence; it is not a reconstruction of that whole drawing routine. SDL window,
resource-fork, allocation, and input plumbing have no one-to-one original game
function to score. Preserve those distinctions in reports and do not infer
code reuse percentages from matching names; many WC1 names were already derived
from this Mac executable's MacsBug strings.

## Next support slices

1. Decode `MOD0` mission data explicitly into SWC host state. `LoadMissionData`
   in CODE_03 +0x16d2 copies 24-byte headers, sixteen 88-byte nav records,
   sixteen 68-byte objectives, thirty-two 76-byte ship records, and two
   40-byte blocks. Mission index 4 (series 1, mission 0) is the initial flight
   candidate; that selection has static evidence but has not run in gameplay.
2. Recover SWC object definitions and map the flight code. CODE_08's initialization
   uses 83 records of 204 bytes; WC1 uses 58 records of 135 bytes. SWC also has
   wider numeric fields, 10-byte weapon slots, and 102-byte loadouts. Direct
   copies into WC1 structures would corrupt state. Similarity should guide
   reuse function by function, including confirmed AI behavior differences.
3. Connect campaign flight, navigation, combat, carrier return, and the 320x240
   HUD. The demo explicitly disables its simulator, so campaign flight is the
   useful first playable milestone.
4. Add scenes, AIFF speech/effects, music, and the custom `LMov` movie format.
   The `.dcMov` files are not ordinary QuickTime movies; renaming them is not
   a decoder implementation.
