# Steam Deck wireframes

Unmodified `steamdeckFront.svg`, `steamdeckBack.svg` and `steamdeckTop.svg` from Valve's
[Steam Deck SVG images](https://partner.steamgames.com/doc/steamhardware/steamdeck/svg),
downloaded from the linked
[steamdeckSVG.zip](https://shared.akamai.steamstatic.com/community_assets/images/steamworks_docs/english/steamdeckSVG.zip)
on 2026-11-18. Valve supplies these images for in-game input callouts.
The page and archive do not specify a named open-source license; these assets
are not covered by this repository's code license or SC Controller's CC0 terms.

The launcher embeds the vectors and adds its own labels and highlights in
`src/sdl/slint/steamdeck.slint`. Its `steamdeck-top.svg` copy rotates the top
wireframe by 180 degrees so left/right agree with the front view; Slint's
software renderer does not rotate image items. Steam Deck is a trademark of
Valve Corporation.
