#!/usr/bin/env python3
"""Compile editable UTF-8 PO catalogs and glyph changes for the SDL port.

The runtime file uses WC1's byte encodings, so neither the DOS text formatter
nor its fixed-size fields ever receive UTF-8. No gettext runtime is needed.
"""
import argparse
import ast
from collections import Counter
import json
from pathlib import Path
import re
import struct


def compile_artwork(path):
    data = json.loads(path.read_text())
    if data.get("schema") != 1 or not 1 <= len(data["packets"]) <= 256:
        raise ValueError("Invalid artwork schema/count")
    output = bytearray(b"WCA1" + struct.pack("<I", len(data["packets"])))
    seen = set()
    for packet in data["packets"]:
        name, section = packet["file"], packet["section"]
        if not re.fullmatch(r"[A-Z0-9_]+\.[A-Z0-9]{3}", name) or not 0 <= section < 32768 or (name, section) in seen:
            raise ValueError("Invalid or duplicate artwork packet")
        seen.add((name, section))
        source, target = packet["source_size"], packet["target_size"]
        edits = packet["edits"]
        if not 0 < source <= 16 * 1024 * 1024 or not 0 < target <= 16 * 1024 * 1024 or not 0 < len(edits) <= 65536:
            raise ValueError("Invalid artwork size")
        encoded = name.encode("ascii") + b"\0"
        output.extend(struct.pack("<HH5I", len(encoded), section, source, packet["source_crc32"], target, packet["target_crc32"], len(edits)))
        output.extend(encoded)
        position, result_size = 0, source
        for offset, removed, text in edits:
            literal = bytes.fromhex(text)
            if not position <= offset <= source or not 0 <= removed <= source - offset:
                raise ValueError("Overlapping or out-of-bounds artwork edit")
            position = offset + removed
            result_size += len(literal) - removed
            output.extend(struct.pack("<3I", offset, removed, len(literal)))
            output.extend(literal)
        if result_size != target:
            raise ValueError("Artwork result size differs")
    return output


def read_po(path):
    entries = []
    entry = {}
    field = None
    fuzzy = False
    font = None
    capacity = None
    for line in path.read_text(encoding="utf-8").splitlines() + [""]:
        if not line.strip():
            if entry.get("msgid"):
                if "msgstr" not in entry:
                    raise ValueError(f"Missing translation in {path}")
                entries.append((entry, fuzzy, font, capacity))
            entry, field, fuzzy, font, capacity = {}, None, False, None, None
        elif line.startswith("#~"):
            continue
        elif line.startswith("#, "):
            fuzzy |= "fuzzy" in line[3:].split(", ")
        elif line.startswith("#. Original font set: "):
            font = int(line.rsplit(" ", 1)[1])
        elif line.startswith("#. Original field capacity: "):
            capacity = int(line.split(": ", 1)[1].split()[0])
        elif line.startswith("#"):
            continue
        else:
            match = re.fullmatch(r'(msgctxt|msgid|msgstr) (".*")', line)
            if match:
                field = match[1]
                if field in entry:
                    raise ValueError(f"Duplicate PO field in {path}")
                entry[field] = ast.literal_eval(match[2])
            elif line.startswith('"') and field:
                entry[field] += ast.literal_eval(line)
            else:
                raise ValueError(f"Unsupported PO syntax in {path}: {line[:80]}")
    return entries


def build_catalog(directory):
    messages = {}
    skipped = 0
    seen_keys = set()
    for path in sorted(directory.glob("*.po")):
        for entry, fuzzy, font, capacity in read_po(path):
            context = entry.get("msgctxt", "")
            key = (context.encode("ascii"), entry["msgid"].encode("cp437"))
            if key in seen_keys:
                raise ValueError(f"Duplicate message key: {context!r}, {entry['msgid']!r}")
            seen_keys.add(key)
            if font is None:
                font = int(context[-1]) if context in ("font0", "font2", "font3") else 1 if path.name == "ui.po" else 0
            if fuzzy or not entry["msgstr"]:
                skipped += 1
                continue
            source, translated = entry["msgid"], entry["msgstr"]
            if path.name == "ui.po":
                # Both printf and the WC1 formatter consume arguments in order.
                # Keep exact directives, including WC1's %X/%Y/%F/%P commands.
                keys = r"\([A-Z](?:/[A-Z])?\)"
                if Counter(re.findall(keys, source)) != Counter(re.findall(keys, translated)):
                    raise ValueError(f"Keyboard shortcut labels differ: {source!r}")
                directives = r"%[-+ #0]*[0-9]*(?:\.[0-9]+)?(?:hh|ll|[hljztL])?[A-Za-z%]"
                if re.findall(directives, source) != re.findall(directives, translated):
                    raise ValueError(f"Format directives differ: {source!r}")
                if re.search(r"%[0-9]+\$|%[^A-Za-z%]*\*", translated):
                    raise ValueError(f"Unsupported positional/dynamic format: {source!r}")
            if Counter(re.findall(r"\$[A-Z](?:[0-9])?", source)) != Counter(re.findall(r"\$[A-Z](?:[0-9])?", translated)):
                raise ValueError(f"Campaign placeholders differ: {context}")
            # AddPCName/ExpandCommMessageTokens share a 256-byte workspace.
            # These bounds cover full pilot fields, fixed mission names and
            # numeric expansions; font encoding uses one byte per character.
            token_limits = {"A": 32, "C": 13, "N": 13, "P": 13, "R": 16,
                            "S": 39, "W": 13, "D": 11, "E": 11,
                            "T": 9, "K": 6, "L": 6}
            expanded_length = len(translated)
            for marker in re.findall(r"\$[A-Z](?:[0-9])?", translated):
                if marker[1] not in token_limits:
                    raise ValueError(f"Unsupported placeholder {marker}: {context}")
                expanded_length += token_limits[marker[1]] - len(marker)
            # szHudMessageBuf is 96 bytes. Reserve 13 for the longest pilot
            # callsign, two for ": ", and one for NUL after token expansion.
            if expanded_length > (80 if "/speaker/" in context else 255):
                raise ValueError(f"Expanded text exceeds game workspace: {context}")
            if context == "intro":
                # TITLE.VGA's sprite alphabet has six Spanish glyphs in the
                # ASCII gap between uppercase and lowercase letters.
                translated = translated.translate(str.maketrans("áéíñóú", "[\\]^_`"))
                if re.search(r"[^A-Za-z0-9\[\\\]^`_., \n]", translated):
                    raise ValueError(f"Unsupported title-font character: {source!r}")
            elif font not in (0, 1, 2, 3):
                raise ValueError(f"Unsupported text font {font}: {context}")
            fields = (context.encode("ascii"), source.encode("cp437"), translated.encode("cp1252"))
            if capacity is not None and len(fields[2]) > capacity:
                raise ValueError(f"Translation exceeds {capacity} encoded bytes: {context}")
            if any(b"\0" in field or len(field) > 32766 for field in fields):
                raise ValueError(f"Invalid catalog string: {context}")
            messages[key] = fields
    if not messages:
        raise ValueError("No translations found")
    fonts = json.loads((directory / "glyphs.json").read_text())
    if fonts.get("schema") != 1:
        raise ValueError("Unsupported glyph schema")
    output = bytearray(b"WCT1" + struct.pack("<II", len(messages), len(fonts["fonts"])))
    for key in sorted(messages):
        fields = tuple(field + b"\0" for field in messages[key])
        output.extend(struct.pack("<3H", *(len(field) for field in fields)))
        output.extend(b"".join(fields))
    for font in fonts["fonts"]:
        if not 0 <= font["font"] < 4 or not 1 <= font["height"] <= 32:
            raise ValueError("Invalid font index or height")
        glyphs = list(font["glyphs"])
        if font["font"] == 1:
            # Resource names and UI strings are shared between menu font 1
            # and cockpit font 2. Give font 1 the same CP1252 accent slots as
            # the other fonts, while retaining the upstream alternate slots.
            destinations = {0xe0: 0xf1, 0xeb: 0xf3, 0xef: 0xfa}
            present = {glyph["character"] for glyph in glyphs}
            glyphs.extend(dict(glyph, character=destinations[glyph["character"]])
                          for glyph in font["glyphs"]
                          if glyph["character"] in destinations and
                          destinations[glyph["character"]] not in present)
        output.extend(struct.pack("<4BIH", font["font"], font["height"], font["ink"],
                                  font["background"], font["source_crc32"], len(glyphs)))
        seen = set()
        for glyph in glyphs:
            character, width = glyph["character"], glyph["width"]
            pixels = bytes.fromhex(glyph["pixels"])
            if character in seen or not 0 <= character <= 255 or not 0 <= width <= 32 or len(pixels) != width * font["height"]:
                raise ValueError("Invalid glyph")
            seen.add(character)
            output.extend(struct.pack("<BB", character, width) + pixels)
    artwork = directory / "artwork.json"
    if artwork.exists():
        output.extend(compile_artwork(artwork))
    return bytes(output), len(messages), skipped


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path("translations/es"))
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    try:
        output, count, skipped = build_catalog(args.source)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(output)
        print(f"Compiled {count} Spanish messages; {skipped} untranslated/review entries fall back to English.")
    except (OSError, ValueError, KeyError, SyntaxError, UnicodeError, struct.error) as error:
        parser.exit(1, f"Translation compilation failed: {error}\n")


if __name__ == "__main__":
    main()
