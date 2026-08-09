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

Two other kinds of evidence carried the rest of the renaming:

- **A routine that prints a path names its compilation unit.** `free_viewport`
  (`0x0040F940`) prints `"free viewport not mcga"`, and `PacketLoad` prints
  `"Library\Source\Pload.c PacketLoad"`.
- **The `ix` module map plus the body.** Every `ix` function sits in a known object file
  (docs/ORDER.md), so its prefix is a fact, not a guess: `ix_streamer_*`, `ix_thread_*`,
  `ix_dsp_*`, `ix_dsps_*`, `ix_dspv_*`, `ix_system_*`, `ix_sound_*`, `ix_sample_*`. The rest
  of the name comes from what the body does — `ix_streamer_hash_name` is a case-insensitive
  rolling hash, `ix_streamer_find_entry` is a binary search over a 16-byte-stride table,
  `ix_dsp_build_pan_tables` fills both 0x80-entry stereo tables.

Where the body proves *what* but not *why*, the name says exactly what is known and no more:
`ix_dsp_set_config_bit1` sets bit `0x02` of `DAT_00597D18` and the meaning of that bit is not
yet established. That is a fact, unlike `GetG00597d18Fn4C27`, and unlike a guess.

**Its one failure mode**: a wrapper that logs on behalf of its callee is misattributed. Always
read the disassembly before adopting a result.

## 0.2 Third-party projects as naming evidence

Three community projects in the parent directory carry facts the binary alone does not, and
each is trustworthy about a different thing:

| Project | Good for | Not usable for |
|---|---|---|
| [WCMissionTools](../../WCMissionTools) | MODULE/CAMP/BRIEFING record layouts and the ShipClass / ShipOrder / Allegiance / Pilot enumerations | runtime layout — the on-disk ship record is 42 bytes against the game's 0x36 stride, and the nav record 77 against 0x1F |
| [WingCommanderArduinoBridge](../../WingCommanderArduinoBridge) | the *order* of fields in the pilot record, since both builds compile the same struct | its addresses, which are DOS-segment relative |
| [wcdx](../../wcdx) | PE layout notes, and confirmation of which code the port patches | naming — its patches are byte diffs by file offset, with no symbols |

**Check them against the image before believing them.** Doing that on `ShipClass` found the
name table at `0x004684D4` listing all 22 classes in exactly the documented order — and
correcting two spellings: index 5 is `"Dilligent"` (sic) and index 14 is `"Spikeri"`, the
developers' own name for the ship the manual calls the Hhriss. The table runs on into weapon
names at `0x00468598`. Likewise the four Kilrathi ace names really are in the image at
`0x0046AFD4`, which confirms the ace block's base index of 14.

`ShipOrder` is corroborated from the other direction: the three order dispatchers
(`0x00409F80`, `0x0040A030`, `0x0040A360`) all switch on the same dword table with cases
4, 5, 6, 8, 9 and -1, and -1 is the enum's `Inactive`. That is what identified
`g_aeShipOrder_0059d200`, and then `g_abShipQueuedOrder_0059c3f0` — written only when the
live order is `ORDER_JUMP_OUT`, so it is the order to apply once the jump finishes.

Everything recovered this way is in [include/wcdata.h](../include/wcdata.h), with each entry
marked verified or not.

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
