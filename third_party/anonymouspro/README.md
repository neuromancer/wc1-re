# Anonymous Pro and SWC HUD Bitmap

`AnonymousPro-Regular.ttf` is the unmodified font from
[Google Fonts](https://github.com/google/fonts/tree/main/ofl/anonymouspro).
Its SHA-256 is
`46d8b9a5f4b38fc9d30f3cdd676d4c6f78a9bef949bb1a8304216cc731eb87f8`.
The copyright and SIL Open Font License 1.1 are in `OFL.txt`.

The derived **SWC HUD Bitmap** in `src/sdl/swc_hud_font.h` is an ASCII subset
rasterized at 9 pixels in monochrome, with a five-pixel advance. Its name is
distinct from the source's reserved font name. It remains under the OFL.
It is a provisional substitute for classic Mac QuickDraw text; it does not
claim Monaco-compatible metrics or reproduce the original SWC font.

Regenerate with `python3 bin/buildSwcHudFont.py` using Pillow. The header records
the Pillow and FreeType versions used. Normal builds and game installations
need neither Pillow nor the TTF: the SDL renderer creates an atlas from the
compiled bitmap. No SDL2_ttf dependency or system-font lookup is involved.

SDL2 release packaging includes `SWC-HUD-FONT-OFL.txt` for the embedded glyphs.
