# Wing Commander I-II Double Resolution Cockpit Font

Font by Trov, distributed under CC BY 4.0
([LICENSE.txt](LICENSE.txt), https://creativecommons.org/licenses/by/4.0/).
Contributed in [issue #29](https://github.com/neuromancer/wc1-re/issues/29)
on 2026-11-25 as `wc_2x_cockpit_font.zip` (SHA-256
`84b6fc507a15f222a4f388bde6a4b1c00dfa07bd9ed85b224c7b9dac85c24a20`), whose
`02_2x` is kept unmodified here as `cockpit-font-2x.bin` (SHA-256
`ca7f83cbd2af365bef65be15899e1928591df1346d27ca0b6ef492234306ed6a`).

It redraws FONTS.FNT's section 2, the 6-row cockpit font, at twice its size,
in the layout the game's decompressed fonts use: a 12-row height, ink colour
0xC6 and background 0, then each character's width and offset. Most letters
are twice the original's width; 8 is one column narrower, and X takes
Wing Commander II's shape instead of the first game's broken one.
Characters 0xF2-0xFF keep the original's symbols, doubled.

The hi-res HUD fonts option shows cockpit text in it while the game's own
cockpit font is the original English one. Builds embed it as C data with
`bin/compileHiResFont.py`; game installations need nothing extra.
