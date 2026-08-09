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

## 0.2 External naming evidence

The recovered releases and adjacent projects carry facts that the Win32 image alone does not.
Each source is authoritative only within a narrow boundary:

| Source | Good for | Not usable for |
|---|---|---|
| [BRAINS.C](../../BRAINS.C) | exact AI function/API names, original state-array names, and mission/objective/tactic control-flow topology | copying bodies or constants wholesale; this is an older demo revision and several branches differ from retail Win32 |
| [Amiga analysis](../../releases/amiga/AMIGA_ANALYSIS.md) | AI objective, tactic, maneuver, and special-maneuver identifier tables | Win32 addresses and the three Win32-only maneuver slots |
| [FM Towns symbols](../../releases/fm-towns/FMTOWNS_SYMBOLS.md) | object class/type identifiers and original spellings | Win32 addresses and runtime element widths |
| [Sega CD symbols](../../releases/segacd/SEGACD_SYMBOLS.md) | full `ObjectType`, `Side`, mission-type, and `Rating` identifiers | Win32 addresses and structure layouts |
| [Mac symbols](../../releases/mac/MAC_SYMBOLS.md) | compilation-unit names and surviving function names/order, especially the `smart`, `logic`, `targ`, and `select` runs | function bodies or exact per-platform ordering; Super Wing Commander is a later code base |
| [WCMissionTools](../../WCMissionTools) | MODULE/CAMP/BRIEFING record layouts | runtime layout — the on-disk ship record is 42 bytes against the game's 0x36 stride, and the nav record 77 against 0x1F |
| [WingCommanderArduinoBridge](../../WingCommanderArduinoBridge) | the *order* of fields in the pilot record, since both builds compile the same struct | its addresses, which are DOS-segment relative |
| [wcdx](../../wcdx) | PE layout notes, and confirmation of which code the port patches | naming — its patches are byte diffs by file offset, with no symbols |

**Check every external claim against the Win32 image.** That process corrected the former
`ShipClass`/`ShipOrder` model. `BRAINS.C` cleanly separates six runtime layers:

| Address | Original field | Win32 representation |
|---|---|---|
| `0x0059B560` | `type[]` | 32-bit `ObjectType` |
| `0x0059D100` | `class[]` | 32-bit `ObjectClass` |
| `0x0059C3F0` | `ship_mission_type[]` | 32-bit `ShipMissionType` |
| `0x0059D200` | `ship_objective[]` | 32-bit `ShipObjective` |
| `0x0059D5E0` | `ship_tactic[]` | 32-bit `ShipTactic` |
| `0x0059DCB0` | `maneuver[]` | 32-bit `ShipManeuver` |
| `0x0059C3C0` | `special_maneuver[]` | 32-bit `SpecialManeuver` |

Disassembly supplies the widths that the old source cannot: `side[]` is also a 32-bit enum,
while `rating[]`, `ship_target[]`, and `ship_seq[]` are byte arrays; `ship_wingleader[]`,
`ship_count[]`, `ship_mission_index[]`, and `object_counter[]` use 16-bit elements. The Win32 name block at
`0x004684D4` independently preserves `Dilligent` and `Spikeri`, while the ace-name block at
`0x0046AFD4` corroborates the rating table's tail.

Everything recovered this way is in [include/wcdata.h](../include/wcdata.h), with each entry
marked verified or not.

### Mac `brain` unit mapped onto Win32

CODE 2 preserves 45 consecutive symbols belonging to the `brain` source unit. Their checked
retail Win32 counterparts form one uninterrupted semantic run at
`0x00409760`–`0x0040B66F`. The only Win32 insertion is
`heat_seeking_missile_intelligence`, independently named by `BRAINS.C`; it is not attributed
to the Mac symbol list.

| Win32 address | Exact name | Win32 address | Exact name |
|---|---|---|---|
| `0x00409760` | `cruise_home` | `0x004098C0` | `fail` |
| `0x004098D0` | `coming_home` | `0x004099C0` | `run_away` |
| `0x00409AC0` | `check_engage_target` | `0x00409B10` | `check_destroy_target` |
| `0x00409C20` | `maneuvering` | `0x00409C50` | `formation_burst` |
| `0x00409CE0` | `disallow_engage` | `0x00409CF0` | `allow_engage` |
| `0x00409D10` | `try2allow_engage` | `0x00409D60` | `imperial_formation` |
| `0x00409F00` | `formation_break` | `0x00409F80` | `imperial_wingman` |
| `0x0040A030` | `kilrathi_wingman` | `0x0040A130` | `wingman_mission` |
| `0x0040A160` | `dist_from_home` | `0x0040A180` | `scan_and_lock` |
| `0x0040A1C0` | `patrol_area` | `0x0040A360` | `kilrathi_patrol` |
| `0x0040A400` | `imperial_wingleader` | `0x0040A410` | `cruise_to_destination` |
| `0x0040A540` | `prepare_for_jump` | `0x0040A630` | `accelerate_and_jump` |
| `0x0040A670` | `reach_warp` | `0x0040A710` | `warp_arrival` |
| `0x0040A740` | `return_to_buddy` | `0x0040A7A0` | `escort_buddy` |
| `0x0040A7D0` | `escort_mission` | `0x0040A900` | `check_goal` |
| `0x0040A940` | `streak_toward` | `0x0040A9B0` | `approach_and_engage` |
| `0x0040AAC0` | `strike_mission` | `0x0040ABB0` | `return_to_master` |
| `0x0040AC00` | `defend_mission` | `0x0040AD80` | `rendezvous_mission` |
| `0x0040AE80` | `ship_intelligence` | `0x0040AF70` | `orbit_sphere` |
| `0x0040B010` | `tanker_intelligence` | `0x0040B0C0` | `destroyer_intelligence` |
| `0x0040B110` | `stationary_intelligence` | `0x0040B140` | `capital_ship_intelligence` |
| `0x0040B320` | `futurion_intelligence` | `0x0040B3A0` | `mine_intelligence` |
| `0x0040B430` | `heat_seeking_missile_intelligence` | | Win32-only `BRAINS.C` helper |
| `0x0040B570` | `FF_missile_intelligence` | | final exact Mac `brain` symbol |

### Mac `smart` unit mapped onto Win32

The Mac release preserves a `smart` compilation unit between the `3d` and `rand` units.
Retail Win32 has the same semantic run at `0x00433AC0`–`0x00434C70`; its next function,
`RandomBelow` at `0x00434CD0`, starts the random-number tranche. The Mac names are used only
after checking each Win32 body. Two Win32-only split helpers and one local function-order swap
explain why this is a semantic sequence rather than a blind one-for-one positional match.

| Win32 address | Recovered name | Mac evidence |
|---|---|---|
| `0x00433AC0` | `steer_away_from_object` | Win32 split helper used by collision prevention |
| `0x00433B90` | `steer_away_from_predicted_object` | Win32 split helper used by collision prevention |
| `0x00433C80` | `prevent_collision` | exact Mac symbol |
| `0x00433D90` | `handle_collisions` | exact Mac symbol |
| `0x00433DE0` | `regulate_turn` | exact Mac symbol |
| `0x00433E50` | `select_target` | exact Mac symbol |
| `0x00433EC0` | `veer_random` | exact Mac symbol |
| `0x00433F50` | `offset_location` | exact Mac symbol |
| `0x00433FF0` | `compute_formation_destination` | Win32 split helper for formation offsets |
| `0x00434040` | `control_speed` | exact Mac symbol; locally precedes `chase_location` in Win32 |
| `0x004340F0` | `chase_location` | exact Mac symbol |
| `0x004342C0` | `goto_location` | exact Mac symbol |
| `0x00434360` | `goto_formation` | exact Mac symbol |
| `0x004344E0` | `maintain_formation` | exact Mac symbol |
| `0x00434550` | `reset_stress` | exact Mac symbol |
| `0x004345D0` | `stress_morale` | exact Mac symbol |
| `0x004345F0` | `any_defense` | exact Mac symbol |
| `0x00434630` | `pick_regular_maneuver` | exact Mac symbol |
| `0x00434800` | `pick_from_list` | exact Mac symbol |
| `0x004348A0` | `pick_kilrathi_maneuver` | exact Mac symbol |
| `0x00434900` | `process_maneuver_node` | exact Mac symbol |
| `0x00434980` | `handle_stress` | exact Mac symbol |
| `0x00434A80` | `intelligence_events` | exact Mac symbol |
| `0x00434C70` | `chase_speed` | exact Mac symbol and final `smart` routine |

The associated retail data fixes the types as well: a `ManeuverChoice` is the packed
three-byte tuple `(threshold, primary, secondary)`, with rated choices at `0x0046D3E8`
(`13 × 9 × 3`) and Kilrathi choices at `0x0046D808` (`5 × 9 × 3`). Those tables are copied
byte-for-byte into `src/globals.c`; they are not synthetic defaults.

### Mac `targ` unit mapped onto Win32

CODE 4's final source unit is `targ`. Its seven symbols map in the same order onto one
uninterrupted retail Win32 range, and the disassembly confirms every body. The next Win32
function at `0x0042AD00` starts the separate weapon-selection sequence, fixing the end of the
unit at `0x0042ACFF`.

| Win32 address | Recovered name | Checked behavior |
|---|---|---|
| `0x0042A8F0` | `find_objective` | scans the packed mission-objective table by type and optional index |
| `0x0042A950` | `arrive_from_warp` | visits the arrival objective, places the ship, and restores cruise state |
| `0x0042AA10` | `unwarp` | clears the view and creates the arrival hyperspace flash |
| `0x0042AAF0` | `warp` | clears the view and creates the departure hyperspace flash |
| `0x0042ABD0` | `drop_player_mine` | selects the first enabled mine hardpoint and drops it |
| `0x0042AC50` | `personality_killed` | records wingman losses or ace kills and adjusts both scores |
| `0x0042ACC0` | `clean_up_cockpit` | clears player/wingman targets and resets the HUD gun readouts |

### Mac `select` unit mapped onto Win32

CODE 15 preserves a four-symbol source unit named `select`. The same four routines are
contiguous and in the same order in retail Win32, where their disassembly confirms the
weapon-selection behavior. The next function, at `0x0042AFA0`, is the timer/debug step,
fixing the unit at `0x0042AD00`–`0x0042AF9F`.

| Win32 address | Recovered name | Checked behavior |
|---|---|---|
| `0x0042AD00` | `find_next_gun` | cycles distinct projectile types and then the all-guns selection |
| `0x0042ADA0` | `select_guns` | enables projectile hardpoints matching the selected type or all-guns mode |
| `0x0042AE10` | `select_new_gun` | advances the gun selection and refreshes the weapon display when visible |
| `0x0042AE50` | `select_new_release_weapon` | selects or cycles distinct non-projectile hardpoints and refreshes the display |

The same bodies identify `0x0046C054` as the selected gun type and `0x0046C058` as the
selected release-weapon slot. Both globals now have typed reconstruction names.

### Mac `logic` unit mapped onto Win32

CODE 5 ends with a 71-symbol ship-logic run from `ace_status` through `mine_available`.
Every name below was adopted only after its retail Win32 body was checked. Together they
fix an exact nested unit at `0x00422010`–`0x00423CDF`; the next Win32 function at
`0x00423CE0` starts unrelated resource/UI loading. The Mac and Win32 builds are not blindly
position-matched: Win32 reverses the local placement of `are_alive` and `trim_goals` and
introduces private helpers between preserved operations.

| Win32 address | Exact Mac name | Win32 address | Exact Mac name |
|---|---|---|---|
| `0x00422010` | `ace_status` | `0x00422030` | `unflag_ace` |
| `0x00422050` | `flag_ace` | `0x00422060` | `kill_ace` |
| `0x00422090` | `ace_greeting` | `0x004220D0` | `prepare_ace` |
| `0x004220F0` | `signed_random` | `0x00422110` | `alert_flag` |
| `0x00422140` | `set_alert` | `0x00422160` | `clear_alert` |
| `0x00422180` | `start_collision_alert` | `0x004221E0` | `try2end_collision_alert` |
| `0x00422220` | `normal_speed` | `0x00422260` | `real_crash_time` |
| `0x00422440` | `clear_crash_cache` | `0x00422460` | `crash_time` |
| `0x004224F0` | `detect_collisions` | `0x00422560` | `unactive` |
| `0x004225C0` | `trim_goals` | `0x00422590` | `are_alive` |
| `0x00422710` | `find_ship_index` | `0x00422780` | `try2rout` |
| `0x00422830` | `no_goal` | `0x00422860` | `being_tailed` |
| `0x004228A0` | `any_enemy_tail` | `0x00422930` | `detect_enemy_tail` |
| `0x004229F0` | `missile_on_tail` | `0x00422A70` | `build_squad_list` |
| `0x00422AC0` | `find_squad_center` | `0x00422B30` | `init_formation_burst` |
| `0x00422BE0` | `reset_mission_type` | `0x00422C30` | `change_mission_type` |
| `0x00422C70` | `reset_objective` | `0x00422CA0` | `alter_objective` |
| `0x00422CD0` | `reset_tactic` | `0x00422D00` | `alter_tactic` |
| `0x00422D30` | `reset_maneuver` | `0x00422D60` | `try2reset_maneuver` |
| `0x00422D90` | `set_special` | `0x00422DD0` | `approach_zero_speed` |
| `0x00422DF0` | `approach_min_speed` | `0x00422E10` | `approach_half_speed` |
| `0x00422E50` | `approach_cruise_speed` | `0x00422E80` | `approach_full_speed` |
| `0x00422EA0` | `approach_ship_speed` | `0x00422EC0` | `get_front_spot` |
| `0x00422F60` | `close_behind` | `0x00422F80` | `scan_for_enemy` |
| `0x00423070` | `any_enemy` | `0x00423210` | `fire_when_ready` |
| `0x004232B0` | `attacker_in_range` | `0x00423350` | `in_danger` |
| `0x00423440` | `build_target_list` | `0x00423530` | `inherit_leader_mission` |
| `0x004235B0` | `inherit_leader` | `0x00423610` | `dead_ship` |
| `0x00423640` | `gone_ship` | `0x00423670` | `skill_rating` |
| `0x004236B0` | `skill_check` | `0x004236F0` | `find_ships_sphere` |
| `0x00423780` | `locate_ship` | `0x00423820` | `get_follow_point` |
| `0x00423930` | `get_first_follow_point` | `0x00423970` | `hostile_sphere` |
| `0x004239D0` | `abandoned` | `0x00423A50` | `engage` |
| `0x00423AC0` | `target_valid` | `0x00423B00` | `triumph` |
| `0x00423BA0` | `find_ratio` | `0x00423C00` | `evaluate_damage` |
| `0x00423CD0` | `mine_available` | | |

Retail Win32 additionally splits out `HasSpeechBuffer`, `report_kilrathi_rout`,
`is_ship_tailing_player_target`, `select_weighted_value`, `get_rear_spot`,
`nearest_enemy_range`, `ships_within_range`, `target_within_range`, and
`select_safe_target`. Their names describe checked retail behavior rather than claiming a
Mac symbol match. The comparison also corrected the signatures and data model:
`build_target_list` returns a 16-bit count, maneuver identifiers are passed to
`reset_maneuver` as shorts, and `scan_for_enemy` takes an unsigned 16-bit range.
`MissionNavPoint.type` is a signed byte with ten 16-bit `missionShips` entries at offset
`+0x3D`, while the runtime ship table at `0x0059C830` holds ten 16-bit mission indices.

The same symbol resource also corroborates nearby helpers after their Win32 bodies are
checked: `get_ship_slot` and `find_vacant_3d_object` in the preceding `3d` unit, plus
`initialize_object` and `drop_mine` in CODE 5's `ship` unit. Only reconstructed bodies
receive the `wc-implemented` Ghidra tag; `initialize_object` and `drop_mine` retain linked
TODO stubs until their own unit pass.

## 1. Evidence-named — trust these at their documented confidence

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
- **Recovered source, checked against Win32** — `perform_maneuver`, `ship_intelligence`,
  `imperial_formation`, and the surrounding AI family listed in [BRAINS.md](BRAINS.md).
  Ghidra tags these with `wc-evidence-BRAINS-C`; later Mac symbols supply names such as
  `reset_mission_type`, `reset_objective`, and `reset_maneuver`.
- **Bodies that prove the semantics** — `MinShort`, `MaxShort`, `RandomBelowOrEqual`,
  `ReadDAT*`/`StoreDAT*` accessors, `CallThrough*` forwarders, `ReturnConst*`.

Two caveats even here:

- `ShipAiState00`..`ShipAiState44` remain **structural function labels**: Amiga identifies
  maneuver ids 0-43, but shared handlers mean an enum identifier is not always a unique
  function name. There is no `ShipAiState01/20/36/45/46` because those ids alias other
  handlers. See the plate comment on `perform_maneuver`.
- `DIBwholePaletteFromTriplets` / `DIBwholePaletteFromWords` — both functions emit an
  identical `"DIBsetWholePalette   SetEntries"` literal, so only one can own the original
  name. They are distinguished by palette element stride (3 vs 6 bytes).

## 2. Operational-signature — mechanism only

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
