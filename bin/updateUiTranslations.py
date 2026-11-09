#!/usr/bin/env python3
"""Extract gettext-style C markers, then merge the editable UI catalog.

Requires GNU gettext only for maintenance; normal game builds use the checked
in catalogs and the Python compiler. --check verifies source/template coverage
without modifying catalogs. Never fuzzily pair a changed format string.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

from compileTranslations import keyed_po


def extract(root, plain=False):
    sources = sorted(path.relative_to(root).as_posix()
                     for path in (root / "src").rglob("*")
                     if path.suffix in (".c", ".cpp", ".h"))
    with tempfile.TemporaryDirectory() as directory:
        output = Path(directory) / "ui.pot"
        subprocess.run([
            "xgettext", "--language=C++", "--from-code=UTF-8",
            "--keyword", "--keyword=_", "--keyword=N_",
            "--keyword=C_:1c,2", "--keyword=NC_:1c,2",
            "--add-comments=TRANSLATORS:", "--no-wrap", "--sort-by-file",
            "--omit-header", "--output", str(output), *sources,
        ], cwd=root, check=True)
        body = output.read_text(encoding="utf-8")
    header = ('# UI strings extracted from _(), N_(), C_() and NC_().\n'
              'msgid ""\nmsgstr ""\n'
              '"Project-Id-Version: wc1-re\\n"\n'
              '"MIME-Version: 1.0\\n"\n'
              '"Content-Type: text/plain; charset=UTF-8\\n"\n'
              '"Content-Transfer-Encoding: 8bit\\n"\n\n')
    return header + body if plain else keyed_po(header + body)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    try:
        text = extract(root)
        template = root / "translations/templates/ui.pot"
        if args.check:
            if not template.exists() or template.read_text(encoding="utf-8") != text:
                parser.exit(1, "UI template is stale; run make update-translations.\n")
        else:
            template.write_text(text, encoding="utf-8")
            for language in ("es", "fr"):
                catalog = root / "translations" / language / "ui.po"
                if catalog.exists():
                    subprocess.run(["msgmerge", "--update", "--backup=none",
                                    "--no-fuzzy-matching", "--no-wrap", str(catalog), str(template)], check=True)
        print("UI translation markers and template agree." if args.check else "Updated UI translation template and merged existing translations.")
    except (OSError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"UI extraction failed: {error}\n")


if __name__ == "__main__":
    main()
