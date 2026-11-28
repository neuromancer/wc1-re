# Original bugs {#original_bugs}

Bugs of the original game. This source keeps them, since it rebuilds `WC1.EXE`
byte for byte; the SDL2 port fixes the ones listed here.

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
