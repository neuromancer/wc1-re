# Original bugs {#original_bugs}

Bugs of the original game and its data. This source keeps them, since it
rebuilds `WC1.EXE` byte for byte; the SDL2 port fixes the ones listed here.

## Planets never appear in the Kilrathi Saga {#bug_planets_as_stars}

draw_sorted_objects_to_buffer() draws each planet with the star field's sprite
instead of the planet's own shape, so space has no planets. Only the Kilrathi
Saga has this bug.

**How to reproduce:** fly any mission. The sky shows stars but no planets, where
the DOS release shows them.

**How it was fixed:** by drawing planets with their own shapes, scaled like
ships.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## Knocked-out displays show no static {#bug_display_static}

When a cockpit display is knocked out, malf_noise() plays the sound of static,
but snow_viewport(), which should draw it, is empty in the Kilrathi Saga, so the
display shows nothing. The DOS game draws the static.

**How to reproduce:** take hits until a cockpit display is knocked out. You hear
static, but the display stays blank.

**How it was fixed:** by drawing noise over the display.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## Cockpit hits draw no explosion {#bug_cockpit_explosion}

cockpit_explosion() unloads its animation on the first frame without an
explosion, and nothing loads it again; the mission loader may also skip it to
fit the DOS game's memory budget. So the explosion that should flash over the
cockpit when it is hit never shows, and only its sound plays. The Kilrathi Saga
has this bug.

**How to reproduce:** take hits until your ship takes internal damage. You hear
the cockpit explosion, but none is drawn.

**How it was fixed:** by always loading the animation and keeping it until the
mission ends.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## A released arrow key keeps steering {#bug_released_arrow}

PollKeyboardState() folds the held arrow keys into one key, and
process_player_input() clears the steering only once no arrow is held, so
letting go of one of two arrows keeps its direction turning. Both the DOS game
and the Kilrathi Saga have this bug.

**How to reproduce:** hold Down and Right, then let go of Down. The ship keeps
turning down and right.

**How it was fixed:** by releasing pitch, yaw and roll each on its own once its
keys are up.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## Joystick calibration keeps only the centre {#bug_joystick_calibration}

LoadJoystickCalibrationFile() keeps only the centre that the joystick
calibration recorded in j.cal, and makes the right and lower sides of each axis
as long as the left and upper ones. On a stick whose centre is not halfway
through its travel, one side reaches its full turn too early or never.

**How to reproduce:** calibrate a joystick whose centre sits off the middle of
its travel, then steer to both sides. One side turns at its full rate before the
stick reaches its end, or never does.

**How it was fixed:** by restoring the extremes the calibration recorded.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Self-repair checks a random component {#bug_self_repair}

repair_internal_damage() first checks that a component is not beyond repair, but
reads the component's number from an uninitialised variable, so it tests a
random entry. Depending on what the stack holds, the ion drive and the shield
generator may never repair themselves, or recover even once destroyed.

**How to reproduce:** fly on with a damaged ion drive or shield generator.
Whether it repairs itself, and whether a destroyed one recovers, can change from
one flight to another.

**How it was fixed:** by checking each component as it is repaired, as Super
Wing Commander does.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Ships evade missiles by the wrong object {#bug_missile_evasion}

When a missile is on a ship's tail, Mkill_missile() steers the ship by
nTargetShip, which missile_on_tail() should set to the missile but leaves at
whatever object was looked at last. So a ship evading a missile turns by an
unrelated object.

**How to reproduce:** hard to see in play: fire a missile at an enemy and watch
it evade. Its turns do not follow the missile.

**How it was fixed:** by having missile_on_tail() note the missile, so the
evasion steers by it.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Briefing shots drawn too high after the nav map {#bug_briefing_nav_map}

Once a briefing has shown its nav map, every later shot in it is drawn 20 lines
higher, nearly touching the top of the screen. The map raises the top of the
screen view from line 24 to line 4. The DOS game puts the view back after the
map, but the Kilrathi Saga's UpdateMap() puts it back before showing the map, so
the change lasts until the next conversation. Only the Kilrathi Saga has this
bug.

**How to reproduce:** start a new campaign and watch the first briefing. After
the nav map, Spirit's question and the shots after it sit 20 lines higher than
the shots before the map.

**How it was fixed:** by putting the view back after the map, as DOS does.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Fighters in the victory sequence launch at random, or never {#bug_victory_launch}

ShowCampaignVictorySequence() counts its fighters' launch timer down before ever
setting it, so whatever an earlier call left on the stack decides whether the
first fighters launch at once, later or never. The DOS game has the same bug.

**How to reproduce:** win the last mission of the campaign and watch the victory
sequence. When the fighters launch, and whether they do, can change from one run
to the next and from one machine to another.

**How it was fixed:** by starting the timer at 0, so the fighters launch as the
Tiger's Claw comes into view, then every eight frames.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## The victory fanfare repeats {#bug_victory_fanfare}

The Kilrathi Saga streams its music from MISSION.STR, where the two victory
fanfares loop through their branches instead of ending, so a won fight's fanfare
plays over and over. Only the Kilrathi Saga has this bug.

**How to reproduce:** win a fight, for example by shooting down a fleeing
Salthi. The victory fanfare plays again and again.

**How it was fixed:** by making both fanfares end instead of looping, as the
stream loads.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## No planets in the skies of Secret Missions 2 {#bug_sm2_planets}

Secret Missions 2 adds its planets in PLANETS1.VGA and numbers them from 12,
after WC1's twelve in PLANETS.VGA. The DOS SM2.EXE loads them from PLANETS1.VGA,
but the Kilrathi Saga's SM2.EXE kept WC1's loader,
InitializeConstellationObject(), which reads only PLANETS.VGA, so none of SM2's
planets load. Only the Kilrathi Saga has this bug.

**How to reproduce:** start Secret Missions 2 from the Kilrathi Saga and launch
any mission. The space backdrops show no planets, where the DOS release shows
them.

**How it was fixed:** by loading them from PLANETS1.VGA, as DOS does.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## The Kilrathi see through the captured Dralthi at once {#bug_sm2_disguise}

In Secret Missions 2 the player flies a captured Dralthi, and in the DOS game
the Kilrathi take it for one of their own until it fires on them. The Kilrathi
Saga's SM2.EXE kept WC1's code, so they attack it at once.

**How to reproduce:** in Secret Missions 2 from the Kilrathi Saga, fly the
captured Dralthi to the nav point it should sneak past. The Kilrathi attack at
once.

**How it was fixed:** by following the DOS SM2.EXE: a sphere linked to itself
disguises the Dralthi until its guns hit a Kilrathi ship.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## The captured Dralthi's lamps are wrong {#bug_sm2_dralthi_lamps}

The Kilrathi Saga's SM2.EXE draws the Dralthi cockpit's autopilot lamp as a flat
red square, from frames its lamp packet does not have, and drops the missile
warning lamp. The DOS SM2.EXE draws both from the lamp frames of PCSHIP.V05,
which both releases ship.

**How to reproduce:** fly the captured Dralthi in Secret Missions 2 from the
Kilrathi Saga. The autopilot lamp is a flat red square, and no missile warning
lamp lights.

**How it was fixed:** by drawing both lamps as the DOS SM2.EXE does.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Secret Missions 2 shows no cockpit damage {#bug_sm2_cockpit_damage}

In the Kilrathi Saga's SM2.EXE, explosion_draw() jumps straight to its end, so
the cockpit never shows damage scars.

**How to reproduce:** take internal damage in Secret Missions 2 from the
Kilrathi Saga. The cockpit shows no scars.

**How it was fixed:** by drawing the scars as WC1 does, at the DOS SM2.EXE's
positions for the Dralthi.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## Secret Missions 2's barracks poster cannot be opened {#bug_sm2_poster}

The DOS SM2.EXE lets the player check out the poster beside the barracks
lockers, which fills the screen with TITLE1.VGA's second image. The Kilrathi
Saga's SM2.EXE dropped that spot from BarracksScreen(), so clicking the poster
does nothing.

**How to reproduce:** in Secret Missions 2 from the Kilrathi Saga, click the
poster beside the barracks lockers. Nothing happens.

**How it was fixed:** by restoring the poster's spot in SM2's barracks, as in
the DOS SM2.EXE.

**Origin:** [wcdx](https://github.com/Bekenn/wcdx)

## The briefing map crashes on an emptied objective {#bug_nav_note_crash}

nav_note() prints the name of the objective the briefing map points at without
checking that it exists. The game's own missions never point past their
objectives, but SM1.5, the fans' port of Super Wing Commander's Covert Missions,
does in its first briefing, and the Kilrathi Saga crashes there.

**How to reproduce:** play SM1.5's first briefing with the Kilrathi Saga.

**How it was fixed:** by giving an empty note for an objective outside the list.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Halcyon's shoulder insignia is missing in the medal ceremony {#bug_halcyon_insignia}

The medal ceremony's image of Colonel Halcyon, in BRIEFING.VGA, lacks his
shoulder insignia. Both releases' data have this bug.

**How to reproduce:** earn a medal and watch the ceremony.

**How it was fixed:** by restoring the insignia in the image as it loads.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## Stray pixels in portraits and cutscenes {#bug_stray_pixels}

A few images carry pixels that do not belong: four that should be transparent in
the medal ceremony's overlay, one on each of Iceman's and Angel's talking
portraits, and specks over the backdrop of the Rostov cutscene. Both releases'
data have this bug.

**How to reproduce:** look closely at the medal ceremony, at Iceman and Angel
when they talk, and at the Rostov cutscene.

**How it was fixed:** by removing those pixels from the images as they load.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## Hunter's and Maniac's helmets carry the wrong labels {#bug_helmet_labels}

In all five of their comm portraits, in WINGMEN.VGA, the labels on Hunter's and
Maniac's helmets are wrong. Both releases' data have this bug.

**How to reproduce:** fly with Hunter or Maniac and watch their portraits when
they talk.

**How it was fixed:** by correcting the labels in the portraits as they load.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## The barracks bucket's splash is damaged {#bug_barracks_bucket}

In frames 37 to 48 of the barracks animation, in RECROOM.VGA, the bucket's
background and the edges of its splash are damaged. Both releases' data have
this bug.

**How to reproduce:** stay in the barracks and watch the bucket.

**How it was fixed:** by repairing those frames as they load.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## The Hornet's launch canopy is opaque {#bug_hornet_canopy}

In the launch sequence, the Hornet's canopy, in PCSHIP.V00, is opaque, where the
other fighters' canopies are see-through. Both releases' data have this bug.

**How to reproduce:** launch in a Hornet.

**How it was fixed:** by making the canopy transparent, as on the other
fighters.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## The Rostov's explosion pauses after every frame {#bug_rostov_cutscene}

The script of the cutscene that shows the Rostov's loss, in MIDGAME.V03, pauses
after every frame of the explosion and does not hold its last image before the
scene ends. Both releases' data have this bug.

**How to reproduce:** lose the Rostov and watch the cutscene that follows.

**How it was fixed:** by removing the pauses and holding the last image before
the scene ends.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## Three medals cannot be won without losing a wingman {#bug_medal_thresholds}

The campaign file, CAMP.000, sets three medals' thresholds 10 points too high,
so winning every medal means sacrificing a wingman. Both releases' data have
this bug.

**How to reproduce:** fly the campaign without losing a wingman. Those three
medals stay out of reach.

**How it was fixed:** by lowering the three thresholds by 10 as the campaign
loads.

**Origin:** [W.C.A.T.](https://alltinker.itch.io/wcat)

## A full weapon list overwrites another ship's hardpoint {#bug_weapon_list_overflow}

When a ship loses a weapon, remove_weapon() shifts the rest of its list and
marks the spare slot after them. With all ten slots in use, that slot lies past
the end of the list, so the write changes the next ship's first hardpoint.

**How to reproduce:** it needs a ship with all ten weapon slots in use; no
effect in play is known.

**How it was fixed:** by marking the spare slot only while the list has room, as
Super Wing Commander does.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## A new cockpit resumes the launch icon in a freed buffer {#bug_launch_icon_buffer}

reset_cockpit() keeps the release-weapon display's launch icon running from the
previous mission, although the buffer behind it was freed with the old cockpit,
so the next cockpit draws the icon through a buffer that may be smaller.

**How to reproduce:** it needs the launch icon still showing when a mission
ends; no effect in play is known.

**How it was fixed:** by stopping the icon once its buffer is gone.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)

## Reads and writes just outside their tables {#bug_harmless_overreads}

Several routines reach just outside their tables: target_locking() reads the
word before the weapon table when no weapon is selected, perform_maneuver() the
byte before its maneuver table, set_up_action_sphere() the type of a nav point
past the table during the Tiger's Claw's escape, and remove_hazard() clears the
entry before its sound table for an empty slot. In the original each finds the
same harmless data every time.

**How to reproduce:** nothing shows in play.

**How it was fixed:** by keeping each result without reaching outside the
table.

**Origin:** [wc1-re](https://github.com/neuromancer/wc1-re)
