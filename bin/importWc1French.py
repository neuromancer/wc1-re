#!/usr/bin/env python3
"""Import Greg14's WC1 and Secret Missions translation from a local installation.

Only text, changed font glyphs and menu-label deltas are exported. English IDs are opaque;
use importWc1Spanish.py --debug-english for the ignored debugging inventory.
"""
import argparse
from collections import Counter
import json
from pathlib import Path
import struct
import tempfile
import zlib

from compileTranslations import read_po
from importWc1Spanish import (cinematic_entries, decode_text, delta, digest, font_changes, import_resources,
                             packets, po_document, shape_with_frames, string_at)
from updateUiTranslations import extract


def french_text(data, translated=False, font=0):
    return decode_text(data)  # GAMEDAT uses CP437; executable title captions need review.


def menu_artwork(original, translated):
    # Base-game Start/Continue buttons only; keep expansion art and intro alphabet.
    source, target = packets(original)[4], packets(translated)[4]
    target = shape_with_frames(source, target, [0, 1])
    return {"schema": 1, "packets": [{
        "file": "TITLE.VGA", "section": 4, "frames": [0, 1],
        "source_size": len(source), "source_crc32": zlib.crc32(source),
        "target_size": len(target), "target_crc32": zlib.crc32(target),
        "edits": delta(source, target),
    }]}


def executable_ui(original, translated, template):
    # This patch retains the DOS executable's layout, relocating some strings
    # through near MOV immediates and far-pointer tables. Never execute it.
    if digest(original) != "c37d085909175abc95b4a51ffc884913c7d4ea52ba66ca932619ce1f67fa91ca":
        raise ValueError("Unsupported English WC.EXE for French UI pairing")
    if len(original) != len(translated) or original[:28] != translated[:28]:
        raise ValueError("French WC.EXE layout differs")
    base, end, segment = 0x13510, 0x209b4, b"1\x12"
    entries = []
    for entry, *_ in read_po(template):
        source, context = entry["msgid"], entry.get("msgctxt", "")
        candidates = set()
        needle = source.rstrip("\n").encode("cp437") + b"\0"
        start = base
        while (offset := original.find(needle, start, end)) >= 0:
            start = offset + 1
            if original[offset - 1]:
                continue
            pointers = set()
            pointer = struct.pack("<H", offset - base)
            position = 0x1200
            while (position := original.find(pointer, position)) >= 0:
                site = position
                position += 1
                near = not base <= site < end and 0xb8 <= original[site - 1] <= 0xbf
                far = original[site + 2:site + 4] == segment
                if near or far:
                    target = base + struct.unpack_from("<H", translated, site)[0]
                    if target != offset:
                        pointers.add(target)
            for target in pointers or {offset}:
                if not base <= target < end or translated[target - 1]:
                    continue
                try:
                    text = french_text(string_at(translated, target, end - target))
                except ValueError:
                    continue
                if text and text != source.rstrip("\n"):
                    candidates.add(text + ("\n" if source.endswith("\n") and not text.endswith("\n") else ""))
        entries.append({"context": context, "source": source,
                        "translation": next(iter(candidates)) if len(candidates) == 1 else "",
                        "font": int(context[-1]) if context.startswith("font") else 1,
                        "capacity": None, "warnings": ["Review executable text and display width"],
                        "notes": ["Source: Greg14 WC.EXE; review moved/truncated DOS strings."]})
    return entries


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--english", type=Path, required=True, help="Original DOS GAMEDAT")
    parser.add_argument("--english-exe", type=Path, required=True, help="Original DOS WC.EXE")
    parser.add_argument("--french", type=Path, required=True, help="Greg14 installation directory")
    parser.add_argument("--output", type=Path, required=True, help="New catalog directory")
    args = parser.parse_args()
    try:
        if args.output.exists():
            raise ValueError("Output already exists; preserve reviewed catalogs")
        root = args.english / "GAMEDAT" if (args.english / "GAMEDAT").is_dir() else args.english
        names = ([f"{kind}.00{i}" for kind in ("BRIEFING", "MODULE") for i in range(3)]
                 + [f"MIDGAME.V0{i}" for i in range(9)]
                 + ["COMMUNIC.DAT", "COMMSM2.DAT", "FONTS.FNT", "TITLE.VGA"])
        french = {"GAMEDAT/" + name: (args.french / "GAMEDAT" / name).read_bytes() for name in names}
        entries, issues, sources = import_resources(root, french, decoder=french_text)
        captions, caption_sources = cinematic_entries(root, french, decoder=french_text)
        sources.update(caption_sources)
        # Unchanged proper names/labels need no translation entry.
        entries = [entry for entry in entries if entry["translation"] != entry["source"]]
        with tempfile.TemporaryDirectory() as directory:
            template = Path(directory) / "ui.pot"
            template.write_text(extract(Path(__file__).resolve().parent.parent, plain=True), encoding="utf-8")
            ui = executable_ui(args.english_exe.read_bytes(), (args.french / "WC.EXE").read_bytes(), template)
        fonts = font_changes((root / "FONTS.FNT").read_bytes(), french["GAMEDAT/FONTS.FNT"])
        used = set("".join(e["translation"] for e in entries + captions + ui).encode("cp437"))
        for font in fonts["fonts"]:
            font["glyphs"] = [g for g in font["glyphs"] if g["character"] in used]
        fonts["fonts"] = [f for f in fonts["fonts"] if f["glyphs"]]
        title = (root / "TITLE.VGA").read_bytes()
        artwork = menu_artwork(title, french["GAMEDAT/TITLE.VGA"])
        sources["TITLE.VGA"] = {"english_sha256": digest(title),
                                "french_sha256": digest(french["GAMEDAT/TITLE.VGA"])}
        report = {
            "language": "fr", "scope": "WC1, SM1 and SM2", "authors": ["Greg14"],
            "author_url": "https://www.wcnews.com/chatzone/members/greg14.11738/",
            "permission": "Maintainer confirmed Greg14's permission to convert and use this translation on 2026-11-09.",
            "encoding": "cp437", "domains": dict(Counter(e["context"].split("/")[0] for e in entries)),
            "cinematic_captions": len(captions),
            "sources": {name: {k.replace("spanish", "french"): v for k, v in hashes.items()}
                        for name, hashes in sources.items()},
            "issues": [{k.replace("spanish", "french"): v for k, v in issue.items()} for issue in issues],
            "not_imported": ["original English prose", "game binaries", "artwork other than base-game menu-label deltas"],
        }
        for name in ("WC.EXE", "GAMEDAT/FONTS.FNT"):
            report["sources"][name] = {"french_sha256": digest((args.french / name).read_bytes())}
        documents = {domain + ".po": po_document([e for e in entries if e["context"].startswith(domain + "/")], language="fr")
                     for domain in ("wc1", "sm1", "sm2", "common")}
        documents["cinematics.po"] = po_document(captions, language="fr")
        documents["ui.po"] = po_document(ui, language="fr")
        documents["glyphs.json"] = json.dumps(fonts, separators=(",", ":")) + "\n"
        documents["artwork.json"] = json.dumps(artwork, separators=(",", ":")) + "\n"
        documents["provenance.json"] = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
        args.output.mkdir(parents=True)
        for name, text in documents.items():
            (args.output / name).write_text(text, encoding="utf-8")
        print(f"Imported {len(entries)} campaign entries, {len(captions)} captions and {len(ui)} UI candidates; review warnings before compiling.")
    except (OSError, ValueError, KeyError) as error:
        parser.exit(1, f"Import failed: {error}\n")


if __name__ == "__main__":
    main()
