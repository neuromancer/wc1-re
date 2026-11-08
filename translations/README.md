# Spanish translation

Choose **Game language → Español** or use `--language=es`; `--language=en`
selects English. Restart to change language. Supports WC1 and Secret Missions
with DOS or Kilrathi Saga data; Super Wing Commander remains English.

Translation: **L@Zar0**, revision: **darkpadawan**, version 1.1 (2021-04-13).
[Upstream notice](upstream/Leeme.txt) declares GPLv3 and includes an additional
no-charge term. [provenance.json](provenance.json) records the source URL, hashes,
and import findings. PO comments identify local edits and executable sources.
Only translation text, changed glyphs, and sparse artwork edits are included.

Edit `es/*.po` with gettext or a translation service. Preserve `msgctxt`,
`msgid`, `$C`-style substitutions, format directives, and displayed key names.
One malformed SM1 record remains fuzzy; unknown entries fall back to English.
Proper names and artwork without translations retain their original wording.

Mark C strings with `_()` or `C_(context, text)`. Use `N_()`/`NC_()` for static
initializers, then translate at display time. `intro` selects the title sprite
alphabet; other text uses CP1252 with compatible menu/cockpit accent glyphs.
The reference build retains the original English expressions.

```sh
make update-translations  # extract UI markers and merge es/ui.po; needs gettext
make modern modern-gui   # compile catalogs automatically; Python only
make check-translations MODERN_RUN_DIR=/path/to/WC1
```

Checks cover tokens, buffer limits, glyphs, artwork CRCs, dialogue layout, and
simulator displays. The probe writes `out-modern/spanish-preview.bmp` and needs
original WC1/Secret Missions data. It is not a full campaign playthrough.
Packages include `translations/es.dat`; source game files are never rewritten.

To reimport the pinned upstream archive into a new directory:

```sh
python3 bin/importWc1Spanish.py --english /path/to/DOS/GAMEDAT \
  --spanish /path/to/WC1_SMIyII_V1.1.zip --output /tmp/wc1-spanish
```

The importer reads installer resources and executable text patches without
running them. Merge its output manually to preserve reviewed local wording.
