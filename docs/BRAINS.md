# Using the recovered `BRAINS.C`

## Provenance and authority

The recovered file is [the old demo's `BRAINS.C`](../../BRAINS.C), headed “WINGLEADER” and
“NPC PILOT INTELLIGENCE CODE MODULE”, with copyright dates 1989 and 1990. Its SHA-256 is:

`461232a3399d7c33ec6351be2f52991ae7652318b2b55f0615f46a08255db458`

It is the strongest naming evidence yet found for the game-core AI. It is **not** the source
that produced the Win32 executable. The safe authority order is:

1. Win32 disassembly determines the body, constants, widths, calling convention, and edge
   cases.
2. `BRAINS.C` supplies original identifiers, state-array roles, and a control-flow scaffold.
3. Amiga, FM Towns, and Sega CD diagnostics corroborate enum order and spelling.

Several concrete differences prove that copying the file wholesale would be wrong. The demo
`fail()` only reports an error, while Win32 clears the objective. Demo `arrive_from_warp()`
always resumes patrol, while Win32 chooses `COME_HOME` for Imperial ships. Formation scan
ranges differ, the retail dispatcher contains additional cases, and the demo retains obvious
test code and source-level mistakes. Reimplementation must therefore follow Win32 instruction
flow even when the older source looks cleaner.

## High-confidence Win32 function map

The following block is unusually strong: function order, callers, switch values, constants,
and state accesses all align. These exact source identifiers are now used in `src/map` and in
Ghidra.

| Win32 address | Recovered identifier | Main corroboration |
|---|---|---|
| `0x00409760` | `cruise_home` | destination/mission-spot comparison and return-home thresholds |
| `0x004098C0` | `fail` | common default arm for the same AI switches; retail body evolved |
| `0x004098D0` | `coming_home` | tactic switch over cruise/head-home/none |
| `0x004099C0` | `run_away` | side split and rout behavior |
| `0x00409AC0` | `check_engage_target` | validates or replaces `ship_target[]` |
| `0x00409B10` | `check_destroy_target` | mission ship, damage, and fallback-target logic |
| `0x00409C20` | `maneuvering` | stores target, processes events, calls `perform_maneuver` |
| `0x00409C50` | `formation_burst` | full speed, destination steering, burst counter, engage transition |
| `0x00409D60` | `imperial_formation` | wing leader, sighting wave, engage permission, catch-up burn |
| `0x00409F00` | `formation_break` | `ship_seq[]` staged break followed by engagement |
| `0x00409F80` | `imperial_wingman` | objective dispatcher for the player's wingman |
| `0x0040A030` | `kilrathi_wingman` | leader inheritance and objective mirroring |
| `0x0040A130` | `wingman_mission` | `side[]` dispatch to Imperial or Kilrathi logic |
| `0x0040A160` | `dist_from_home` | distance from `ship_mission_spot[obj]` |
| `0x0040A180` | `scan_and_lock` | scanner result becomes target and tactic |
| `0x0040A1C0` | `patrol_area` | look-out/head-home/approach-target tactic switch |
| `0x0040A360` | `kilrathi_patrol` | patrol objective switch |
| `0x0040A400` | `imperial_wingleader` | forwards to Kilrathi patrol logic, as the source comments describe |
| `0x0040A410` | `cruise_to_destination` | destination following and enemy-dependent cruise speed |
| `0x0040A540` | `prepare_for_jump` | stop-drift state and warp-preparation counter |
| `0x0040A630` | `accelerate_and_jump` | full-speed acceleration timer followed by warp |
| `0x0040A670` | `reach_warp` | cruise/sit-still/warp-out tactic switch |
| `0x0040A710` | `warp_arrival` | warp-in tactic gate |
| `0x0040A740` | `return_to_buddy` | closes escort distance and resumes wander |
| `0x0040A7A0` | `escort_buddy` | matches speed and heading with escorted ship |
| `0x0040A7D0` | `escort_mission` | escort objective dispatcher |
| `0x0040A900` | `check_goal` | missing strike target becomes rout |
| `0x0040A940` | `streak_toward` | steering variation, range test, and afterburner |
| `0x0040A9B0` | `approach_and_engage` | damage/range decision between closing and combat |
| `0x0040AAC0` | `strike_mission` | strike objective dispatcher |
| `0x0040ABB0` | `return_to_master` | defend ship closes on its master and resumes wander |
| `0x0040AC00` | `defend_mission` | threat scan, drift correction, and defend objective switch |
| `0x0040AD80` | `rendezvous_mission` | reach-ship objective becomes defend |
| `0x0040AE80` | `ship_intelligence` | top-level fighter mission-type dispatcher |
| `0x0040AF70` | `mega_ship` | Ralari/Fralthi sphere patrol and turret behavior |
| `0x0040B140` | `capital_ship_intelligence` | top-level capital-ship mission/tactic dispatcher |
| `0x0040B320` | `futurion_intelligence` | restores class from `object_counter[]` when arrival conditions pass |
| `0x0040B3A0` | `mine_intelligence` | proximity scan and detonation |
| `0x0040B430` | `heat_seeking_missile_intelligence` | heat-target reacquisition and homing |
| `0x0040B570` | `FF_missile_intelligence` | friend-or-foe target acquisition and homing |
| `0x0042A950` | `arrive_from_warp` | unwarp, speed restoration, and mission reset |

The source exposes called API names, while the later Mac symbol map refines a few spellings;
all entries below are checked against their Win32 bodies:

| Win32 address | Identifier | Status in reconstruction |
|---|---|---|
| `0x004075D0` | `perform_maneuver` | implemented outside `stubs.c` |
| `0x00418590` | `equ_vector` | implemented outside `stubs.c` |
| `0x004185F0` | `zero_vector` | implemented outside `stubs.c` |
| `0x00419210` | `distance_from_point` | implemented outside `stubs.c` |
| `0x00419260` | `distance_from_object` | implemented outside `stubs.c` |
| `0x00419290` | `get_facing_range_from_point` | implemented outside `stubs.c` |
| `0x00419310` | `get_facing_range_from_object` | implemented outside `stubs.c` |
| `0x00419390` | `ship_vs_point` | implemented outside `stubs.c` |
| `0x004193B0` | `ship_vs_ship` | implemented outside `stubs.c` |
| `0x00419660` | `point_ship_at_point` | implemented outside `stubs.c` |
| `0x004196A0` | `point_ship_at_object` | implemented outside `stubs.c` |
| `0x00419850` | `point_perpendicular` | implemented outside `stubs.c` |
| `0x00419870` | `point_parallel` | implemented outside `stubs.c` |
| `0x00419BD0` | `remove_object` | implemented outside `stubs.c` |
| `0x0041E7C0` | `steady_object` | implemented outside `stubs.c` |
| `0x0041E7F0` | `real_velocity` | implemented outside `stubs.c` |
| `0x0041E820` | `fix_velocity` | implemented outside `stubs.c` |
| `0x00421350` | `fire_afterburner` | implemented outside `stubs.c` |
| `0x00422220` | `normal_speed` | implemented outside `stubs.c` |
| `0x00422560` | `unactive` | implemented; original spelling retained |
| `0x00422710` | `find_ship_index` | implemented outside `stubs.c` |
| `0x00422830` | `no_goal` | implemented |
| `0x00422930` | `detect_enemy_tail` | implemented outside `stubs.c` |
| `0x00422B30` | `init_formation_burst` | implemented outside `stubs.c` |
| `0x00422BE0` | `reset_mission_type` | implemented outside `stubs.c` |
| `0x00422C30` | `change_mission_type` | implemented outside `stubs.c` |
| `0x00422C70` | `reset_objective` | implemented |
| `0x00422CD0` | `reset_tactic` | implemented |
| `0x00422D00` | `alter_tactic` | implemented |
| `0x00422D30` | `reset_maneuver` | exact Mac symbol; implemented body is disassembly-backed |
| `0x00422D90` | `set_special` | implemented |
| `0x00422E10` | `approach_half_speed` | implemented outside `stubs.c` |
| `0x00422E50` | `approach_cruise_speed` | implemented |
| `0x00422E80` | `approach_full_speed` | implemented |
| `0x00422EA0` | `approach_ship_speed` | implemented outside `stubs.c` |
| `0x00422F80` | `scan_for_enemy` | implemented outside `stubs.c` |
| `0x00423070` | `any_enemy` | implemented outside `stubs.c` |
| `0x004232B0` | `attacker_in_range` | implemented outside `stubs.c` |
| `0x00423350` | `in_danger` | implemented outside `stubs.c` |
| `0x004235B0` | `inherit_leader` | implemented outside `stubs.c` |
| `0x00423640` | `gone_ship` | implemented outside `stubs.c` |
| `0x004239D0` | `abandoned` | exact call name; retail behavior has expanded |
| `0x00423A50` | `engage` | implemented outside `stubs.c` |
| `0x00423AC0` | `target_valid` | implemented outside `stubs.c` |
| `0x00423C00` | `evaluate_damage` | implemented outside `stubs.c` |
| `0x00433E50` | `select_target` | implemented outside `stubs.c` |
| `0x00433EC0` | `veer_random` | implemented outside `stubs.c` |
| `0x004344E0` | `maintain_formation` | implemented outside `stubs.c` |

`EMStartUp` at `0x00421AB0` is a separate FM Towns/Win32 diagnostic recovery, not a
`BRAINS.C` mapping.

## Runtime state model recovered

The largest gain is not an individual function name but the separation of fields that had
previously been collapsed into “class”, “order”, or “AI state”.

| Win32 address | Recovered global | Element type/width |
|---|---|---|
| `0x0059B560` | `type[]` / `g_aeObjectType_0059b560` | `ObjectType`, 32-bit |
| `0x0059C310` | `yaw_goal[]` / `g_anYawGoal_0059c310` | signed 16-bit |
| `0x0059C330` | `object_counter[]` / `g_asObjectCounter_0059c330` | signed 16-bit |
| `0x0059C3B0` | `target_ship` / `g_nTargetShip_0059c3b0` | signed 16-bit object index |
| `0x0059C3C0` | `special_maneuver[]` / `g_aeSpecialManeuver_0059c3c0` | `SpecialManeuver`, 32-bit |
| `0x0059C3F0` | `ship_mission_type[]` / `g_aeShipMissionType_0059c3f0` | `ShipMissionType`, 32-bit |
| `0x0059C420` | `ship_count[]` / `g_asShipCount_0059c420` | signed 16-bit |
| `0x0059CD80` | `rating[]` / `g_acShipRating_0059cd80` | `Rating`, 8-bit |
| `0x0059CE10` | `target_range` / `g_nTargetRange_0059ce10` | signed 16-bit distance |
| `0x0059CE60` | `ship_target[]` / `g_acShipTarget_0059ce60` | signed 8-bit; `-1` is none |
| `0x0059D100` | `class[]` / `g_aeObjectClass_0059d100` | `ObjectClass`, 32-bit |
| `0x0059D200` | `ship_objective[]` / `g_aeShipObjective_0059d200` | `ShipObjective`, 32-bit |
| `0x0059D400` | `ship_wingleader[]` / `g_asShipWingLeader_0059d400` | signed 16-bit |
| `0x0059D4B0` | `ship_mission_ship[]` / `g_anShipMissionShip_0059d4b0` | signed 16-bit mission id |
| `0x0059D520` | `ship_seq[]` / `g_acShipSequence_0059d520` | signed 8-bit |
| `0x0059D52A` | `target_facing` / `g_nTargetFacing_0059d52a` | signed 16-bit facing score |
| `0x0059D530` | `destination[]` / `g_aShipDestination_0059d530` | 12-byte `FixedVector` |
| `0x0059D5E0` | `ship_tactic[]` / `g_aeShipTactic_0059d5e0` | `ShipTactic`, 32-bit |
| `0x0059D630` | `roll_goal[]` / `g_anRollGoal_0059d630` | signed 16-bit |
| `0x0059D650` | `side[]` / `g_aeShipSide_0059d650` | `Side`, 32-bit |
| `0x0059D7A0` | `pitch_goal[]` / `g_anPitchGoal_0059d7a0` | signed 16-bit |
| `0x0059D860` | `ship_turn[]` / `g_abShipTurn_0059d860` | 8-bit counter |
| `0x0059D920` | `facing_to_target` / `g_nFacingToTarget_0059d920` | signed 16-bit facing score |
| `0x0059DCB0` | `maneuver[]` / `g_aeShipManeuver_0059dcb0` | `ShipManeuver`, 32-bit |
| `0x0059DD10` | `ship_mission_spot[]` / `g_aShipMissionSpot_0059dd10` | 12-byte `FixedVector` |
| `0x0046C04C` | `your_wingman` / `g_nYourWingman_0046c04c` | signed 16-bit object index |
| `0x0046C080` | engage authorization / `g_bEngageAllowed_0046c080` | 32-bit Boolean |
| `0x0046C084` | `auto_engage_timer` / `g_nAutoEngageTimer_0046c084` | signed 16-bit |
| `0x00465C7C` | `enemy_sighting` / `g_nEnemySighting_00465c7c` | signed 16-bit wave id |
| `0x0046C01C` | `current_wave` / `g_nCurrentWave_0046c01c` | signed 16-bit wave id |

The cross-release tables fill in `ObjectClass`, `ObjectType`, `ShipMissionType`,
`ShipObjective`, `ShipTactic`, `Side`, `Rating`, `ShipManeuver`, and `SpecialManeuver` in
[wcdata.h](../include/wcdata.h). The Win32 maneuver dispatcher has 47 slots; Amiga names
0-43, so 44-46 deliberately remain `UNKNOWN` until direct evidence appears.

The additional goal/mission arrays above are fully named and typed in Ghidra. Some source-side
backing allocations remain deliberately oversized or byte-typed for now because resizing those
tentative declarations moves later globals in the reconstructed image and reduces similarity;
their retail widths, not the temporary C allocation, are authoritative.

## Reconstruction workflow

For any function named by this source:

1. Treat the source routine as a map of intent and state transitions.
2. Recover the retail Win32 body from Ghidra, including changed thresholds and port-specific
   branches.
3. Compile and compare the function against the original before accepting it.
4. If the function currently lives in `src/stubs.c`, move the completed body into its owning
   address-ordered source file and remove the stub in the same change.

The first implementation tranche now includes every high-confidence top-level mapping in the
table above, including `imperial_wingman`, `kilrathi_wingman`, `kilrathi_patrol`,
`mine_intelligence`, both missile-intelligence routines, and `arrive_from_warp`. Their completed
bodies and the called AI/geometry/mission helpers live in their address-ordered source modules,
not in `stubs.c`. The post-pass report compares 604 functions: 355 are exact, 415 are at least
90%, and the whole-image average is 83.91%. A few newly exposed downstream helpers remain
functional approximations with low similarity; these are candidates for later retail-driven
tightening rather than hidden link stubs.
