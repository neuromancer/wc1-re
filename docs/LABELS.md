# How much to trust a Ghidra name

All 1,450 developer functions in the WC Ghidra project are named, but the names are of two
very different kinds. Treating an operational label as a statement of intent will send you
down the wrong path.

The `label_kind` column in `../wc1_function_evidence.csv` separates them.

## 0. Naming policy for this reconstruction

**No reimplemented function keeps an operational label.** Once a function is written, it gets
a name that says what it does, chosen in this order of preference:

1. **The developer's own name**, when the binary states it. Preserve their spelling and case
   exactly, including the inconsistent ones: `DIBslamReal`, `playWAVE`, `shadow_draw`,
   `snow_viewport`, `exit_squadron`, `PacketLoad`, `Streamer_open`, `SetMusBreakpt`.
   `bin/nameOracle.py` finds these (see below).
2. **A `<Verb><Object>` description of the observable behaviour**, when 1 is unavailable:
   `SetTextCursor`, `FreeAllTrackedAllocations`, `DosStrchr`, `ClearWingmanSlots`.
3. **A `…Hook` name** for functions that are genuinely empty in the original. These are
   compiled-out DOS-era operations, not stubs on our side; the suffix marks that fact so a
   future reader does not go looking for a body: `SceneEnterHook`, `TimerResetHook`.

Never allowed in a function name:

- the address (`DoLocalFn5450`) — `bin/auditAddresses.py` enforces the annotation instead;
- Ghidra's relationship labels (`HelperOf430FC0C`, `CallThrough433060`);
- a bare `vN` counter (`ReturnConst0v5`) where any distinguishing fact exists.

Globals are the exception: they **must** keep the address, as `g_<hungarian><Name>_<address>`
or, until identified, `DAT_<address>` (AGENTS.md).

## 0.1 The name oracle

WC1 shipped as a debug build, so its diagnostic printers still carry their format strings, and
many of those formats are the developer's own name for the routine doing the printing.

    bin/nameOracle.py --top      # rank printers by string-argument call sites
    bin/nameOracle.py            # dump the six developer-facing ones

It scans `.text` for CALL sites and takes the last string literal pushed before each call,
never looking back past the start of the enclosing function. Two independent controls confirm
the method: it re-derives `StopMusic` (`0x0042E350`) and the whole `DIB*` family, both of which
were already evidence-named by other means.

What it produced, with the label it replaced:

| Address | Was | Is | Printed string |
|---|---|---|---|
| `0x0042E320` | `DoUiFnE320` | `FadeMusic` | `"FadeMusic\n"` |
| `0x0042E330` | `DoLocalFnE330` | `SetMusicOn` | `"SetMusicOn %d\n"` |
| `0x0042E380` | `DoMusicFnE380` | `SetMusBreakpt` | `"SetMusBreakpt\n"` |
| `0x0042E3A0` | `DoLocalFnE3A0` | `FlushSoundEffect` | `"FlushSoundEffect\n"` |
| `0x0042E3C0` | `DoLocalFnE3C0` | `FlushSoundEffects` | `"FlushSoundEffects\n"` |
| `0x0042EF00` | `DoLocalFnEF00` | `SoundFxTick` | `"soundFX"` |

**Its one failure mode**: a wrapper that logs on behalf of its callee is misattributed. Always
read the disassembly before adopting a result.

## 1. Evidence-named — 437 functions — trust these

Derived from something the binary actually states:

- **Exact original developer names (23)** — the game logs its own function names in error
  strings, so these are ground truth: `DIBcascade`, `DIBmakeDIB`, `DIBdestroyDIB`,
  `DIBslamReal`, `DIBupdate`, `DIBramPalette`, `DIBsetPalette`, `DIBinstall`, `DIBreInstall`,
  `MonoDebug_install`, `MonoDebug_print`, `MonoDebug_remove`, `playWAVE`, `exit_squadron`,
  `shadow_draw`, `fizzle_fade`, `snow_viewport`, `sound_effect`, `PacketLoad`,
  `Streamer_open`, `Streamer_play`, `Streamer_stop`, `Streamer_trigger`, `Streamer_close`.
  Plus `main`, identified from the leaked source screenshot.
- **`ix` assert messages** — e.g. `ix_sample_load_aiff` / `ix_sample_load_wav` come from the
  `FORM`/`AIFF` vs `RIFF`/`WAVE` checks named in their own assert text.
- **Distinctive strings or API usage** — `CalibrateJoystickInteractive`
  ("Move stick to the UPPER LEFT"), `LoadVolumeSettingsFromRegistry`, `ShowDamageReport`.
- **Bodies that prove the semantics** — `MinShort`, `MaxShort`, `RandomBelowOrEqual`,
  `ReadDAT*`/`StoreDAT*` accessors, `CallThrough*` forwarders, `ReturnConst*`.

Two caveats even here:

- `ShipAiState00`..`ShipAiState44` are **structural**: the number is the dispatch-table index,
  not a behaviour. 42 distinct handlers fill 47 slots; there is no `ShipAiState01/20/36/45/46`
  because those ids alias other handlers. See the plate comment on `RunShipAiBehaviorTick`.
- `DIBwholePaletteFromTriplets` / `DIBwholePaletteFromWords` — both functions emit an
  identical `"DIBsetWholePalette   SetEntries"` literal, so only one can own the original
  name. They are distinguished by palette element stride (3 vs 6 bytes).

## 2. Operational-signature — 1,013 functions — mechanism only

Form: `<Verb><Object>Fn<addr>`, e.g. `ScanTbl0046C028Fn1210`.

This is a real, verified claim — that function loops over the array at `0x0046C028` — but it
is **not** a claim about what the loop is for. Rename freely once understood.

| Verb | Means (verified) |
|---|---|
| `Blit` | calls the DirectDraw surface blit/restore path |
| `Draw` | calls the sprite / primitive rasterisers |
| `Palette` | calls the palette setters |
| `Sound` / `Stream` | calls the SFX-music entry points / the `Streamer_*` wrappers |
| `Load` / `Release` | calls the packet-disk loaders / the free-release path |
| `Text` | calls `sprintf`, the on-screen printer, or the string-table lookup |
| `Input` / `Ai` | calls the input pollers / the AI selector or tick |
| `Win32` | calls Win32 imports directly |
| `Calc` | uses floating point |
| `Scan` | loops over an indexed global table |
| `Dispatch` / `Loop` | switch-based dispatch / loop with no indexed table |
| `Set` / `Get` | writes more global slots than it reads / reads only |
| `Do` | straight-line code, no dominant pattern |

`Object` is the global table indexed most often (`Tbl0059D710` = `&DAT_0059d710`), else the
subsystem hypothesis, else the dominant global. `Fn<addr>` is the function's own address.

### Working efficiently with these

Group by the `Object` token. Every function whose name contains the same `Tbl…` works on the
same array, so understanding one array typically explains a dozen functions at once. That is
the highest-value order of attack.

## Subsystem hypotheses are NOT names

Two independent classifiers were built — shared-global co-access clustering (66% coverage) and
call-graph propagation from named callers (18%). **They agree only 39% of the time**, mostly
because `HandleDebugCheatKeys` calls into every subsystem and poisons graph propagation.

So no subsystem was written into any function name. The guesses live in Ghidra **function
tags** instead, where they are explicitly marked:

| Tag | Meaning |
|---|---|
| `wc-hypo-data-<Subsystem>` | subsystem hypothesis — unverified |
| `wc-module-ix-*` / `wc-module-gamecore` | module, exact for `ix` |
| `wc-ported16bit` / `wc-native32bit` | 16-bit-operand density above/below 0.05 |
| `wc-role-getter/setter/forwarder/constret/emptystub` | proven from the body |
| `wc-reach-nocaller` / `wc-reach-tableorptr` | 161 functions have no direct caller |
| `wc-has-assert-anchor`, `wc-calls-win32api`, `wc-refs-string` | evidence present |

Query them in Ghidra with `search_functions_by_tag`.

## Functions that only exist because of the reloc table

96 functions were created during analysis that Ghidra had not found at all, because a data
table was their only reference — 33 from reloc-proven code pointers (the AI dispatch table)
and 63 from sweeping orphaned code after MSVC's 16-byte alignment padding. All 63 landed on
16-byte boundaries, confirming they are genuine entry points. If a function seems to have no
callers, check the `wc-reach-*` tag before assuming it is dead.
