#!/usr/bin/env python3
"""Import L@Zar0/darkpadawan's WC1 Spanish v1.1 resources into UTF-8 PO.

Reads the installer as an archive; never executes it or copies game binaries.
English strings come from the user's own DOS GAMEDAT.
Resource layouts follow wcdata.h, cmpgn.c, screens.c and mission.c.
"""

import argparse
import bz2
import difflib
from collections import Counter
import hashlib
import io
import json
from pathlib import Path
import re
import struct
import sys
import zipfile
import zlib

from compileTranslations import read_po


SPANISH_SHA256 = "b4b5bccb3eb6489b8694333c32b7a164bfa74734a9c88bcfbe917653088e9f9d"
SPANISH_URL = "https://www.mediafire.com/file/dqqj341ayh0jsj2/WC1_SMIyII_V1.1.zip/file"
MAX_RESOURCE = 16 * 1024 * 1024
FONT_ONE = str.maketrans({"à": "ñ", "ë": "ó", "ï": "ú"})
CAMPAIGNS = ("wc1", "sm1", "sm2")


def digest(data):
    return hashlib.sha256(data).hexdigest()


def unpack(data, fmt, offset=0):
    size = struct.calcsize(fmt)
    if offset < 0 or offset + size > len(data):
        raise ValueError("Truncated record")
    return struct.unpack_from(fmt, data, offset)


def expand_installer_block(data, expected):
    if not data or not 0 <= expected <= MAX_RESOURCE:
        raise ValueError("Invalid installer block size")
    method = data[0]
    if method == 0:
        output = data[1:1 + expected]
    elif method == 1:
        decoder = zlib.decompressobj()
        output = decoder.decompress(data[1:], expected + 1)
        if not decoder.eof:
            raise ValueError("Incomplete or oversized deflate block")
    elif method == 2:
        decoder = bz2.BZ2Decompressor()
        output = decoder.decompress(data[1:], max_length=expected + 1)
        if not decoder.eof:
            raise ValueError("Incomplete or oversized bzip2 block")
    else:
        raise ValueError(f"Unsupported installer compression {method}")
    if len(output) != expected:
        raise ValueError("Installer block length mismatch")
    return output


def read_spanish(path):
    """Read the pinned Clickteam 2.0.0.30 package without running its stub.

    Format references: Bioruebe/cicdec Program.cs (TryParse30), and ScummVM's
    common/compression/clickteam.cpp. This is a purpose-specific reader for the
    verified v1.1 ZIP, not a general-purpose installer extractor.
    """
    raw = path.read_bytes()
    if digest(raw) != SPANISH_SHA256:
        raise ValueError("Expected WC1_SMIyII_V1.1.zip; SHA-256 does not match")
    with zipfile.ZipFile(io.BytesIO(raw)) as archive:
        names = [n for n in archive.namelist() if n.lower().endswith(".exe")]
        if len(names) != 1:
            raise ValueError("Expected one Spanish installer")
        installer = archive.read(names[0])
        readme = archive.read("Leeme.txt")
    position = installer.find(bytes.fromhex("777767542948"))
    if position < 0:
        raise ValueError("Missing Clickteam overlay")
    position += 6
    file_lists = []
    file_data = None
    while position + 8 <= len(installer):
        tag, flags, length = unpack(installer, "<HHI", position)
        position += 8
        if position + length > len(installer):
            raise ValueError("Installer block exceeds archive")
        block = installer[position:position + length]
        if tag == 0x143a:
            expected, = unpack(block, "<I")
            file_lists.append(expand_installer_block(block[4:], expected))
        elif tag == 0x7f7f:
            file_data = block
            break
        position += length
    if len(file_lists) != 2 or file_data is None:
        raise ValueError("Expected two installer variants and a data block")
    result = {"Leeme.txt": readme}
    # Both variants have the same game resources. Ignore executable variants.
    table = file_lists[0]
    count, = unpack(table, "<I")
    position = 4
    for _ in range(count):
        length, kind = unpack(table, "<HH", position)
        if length < 4 or position + length > len(table):
            raise ValueError("Invalid installer file entry")
        record = table[position:position + length]
        position += length
        if kind != 0:
            continue
        name = record[68:].split(b"\0", 1)[0].decode("cp1252").replace("\\", "/")
        wanted = re.fullmatch(
            r"GAMEDAT/(?:BRIEFING\.00[012]|MODULE\.00[012]|COMMUNIC\.DAT|"
            r"COMMSM2\.DAT|FONTS\.FNT|MIDGAME\.V0[0-8]|TITLE1?\.VGA|"
            r"COCKPIT\.VGA|RECROOM\.VGA|SERIES\.VGA)|"
            r"Tools/(?:charmap|nomenclaturaWC1)\.txt|"
            r"Tools/_EXE_WORK(?:SM2)?/patch_work(?:sm2)?\.txt",
            name,
        )
        if not wanted:
            continue
        offset, compressed, _, expected = unpack(record, "<4I", 6)
        start = offset + 4
        if start + compressed > len(file_data):
            raise ValueError(f"Installer file exceeds data block: {name}")
        result[name] = expand_installer_block(file_data[start:start + compressed], expected)
    if position != len(table):
        raise ValueError("Unexpected trailing installer file records")
    return result


def origin_lzw(data, expected):
    """WC1's LSB-first 9-to-12-bit LZW, matching src/sdl/resources.c."""
    if not 0 < expected <= MAX_RESOURCE:
        raise ValueError("Invalid LZW output size")
    bit = 0
    width = 9
    table = None
    previous = b""
    output = bytearray()
    while bit + width <= len(data) * 8:
        start = bit // 8
        code = (int.from_bytes(data[start:start + 3], "little") >> (bit % 8)) & ((1 << width) - 1)
        bit += width
        if code == 256:
            table = [bytes([i]) for i in range(256)] + [b"", b""]
            previous = b""
            width = 9
            continue
        if code == 257:
            if len(output) != expected:
                raise ValueError("LZW output size mismatch")
            return bytes(output)
        if table is None or code > len(table) or (code == len(table) and not previous):
            raise ValueError("Invalid LZW code")
        entry = previous + previous[:1] if code == len(table) else table[code]
        output.extend(entry)
        if len(output) > expected:
            raise ValueError("LZW output exceeds declared size")
        if previous:
            if len(table) >= 4096:
                raise ValueError("LZW dictionary overflow")
            table.append(previous + entry[:1])
            if len(table) == 1 << width and width < 12:
                width += 1
        previous = entry
    raise ValueError("Missing LZW stop code")


def packets(data):
    length, first = unpack(data, "<II")
    directory = first & 0xffffff
    if length != len(data) or directory < 8 or directory > length or directory % 4:
        raise ValueError("Invalid Origin packet directory")
    entries = unpack(data, "<" + "I" * (directory // 4 - 1), 4)
    output = []
    for index, entry in enumerate(entries):
        start = entry & 0xffffff
        end = entries[index + 1] & 0xffffff if index + 1 < len(entries) else length
        if not directory <= start <= end <= length:
            raise ValueError("Invalid Origin packet bounds")
        section = data[start:end]
        if section and entry >> 24 == 1:
            expected, = unpack(section, "<I")
            section = origin_lzw(section[4:], expected)
        output.append(section)
    return output


def string_at(data, offset, limit=None):
    end = len(data) if limit is None else min(len(data), offset + limit)
    if not 0 <= offset < end:
        raise ValueError("Text offset outside resource")
    terminator = data.find(b"\0", offset, end)
    if terminator < 0:
        raise ValueError("Unterminated text record")
    return data[offset:terminator]


def decode_text(data, spanish=False, font=0):
    text = data.decode("cp1252" if spanish else "cp437")
    if spanish and font == 1:
        text = text.translate(FONT_ONE)
    if any(ord(c) < 32 and c not in "\n\r\t" for c in text):
        raise ValueError("Non-text control byte in text record")
    return text


def tokens(text):
    return Counter(re.findall(r"\$[A-Z](?:[0-9])?", text))


def add_entry(entries, issues, context, english, spanish, font=0, capacity=None):
    source = decode_text(english)
    translated = decode_text(spanish, True, font)
    if not source:
        if translated:
            raise ValueError(f"Spanish text has no English source: {context}")
        return
    warnings = []
    if tokens(source) != tokens(translated):
        warnings.append("Campaign substitution tokens differ; review before use")
    if not translated:
        warnings.append("Empty upstream translation")
    for warning in warnings:
        issues.append({"context": context, "warning": warning})
    entries.append({"context": context, "source": source, "translation": translated,
                    "font": font, "capacity": capacity, "warnings": warnings})


def scene_entries(entries, issues, context, english, spanish, pairs):
    for pair in range(pairs):
        es, et = unpack(english, "<II", pair * 8)
        ss, st = unpack(spanish, "<II", pair * 8)
        if not (pairs * 8 <= es <= et <= len(english) and pairs * 8 <= ss <= st <= len(spanish)):
            raise ValueError(f"Invalid scene bounds: {context}/{pair}")
        if (et - es) % 13 or (st - ss) % 13:
            raise ValueError(f"Scene records do not align: {context}/{pair}")
        changed_scene = (context == "sm2/BRIEFING.002/packet/4" and pair == 4
                         and et - es == 6 * 13 and st - ss == 14 * 13)
        if st - ss != et - es and not changed_scene:
            raise ValueError(f"Scene records do not align: {context}/{pair}")
        if changed_scene:
            # v1.1 replaces the stock Eight-Ball exchange with a different
            # conversation. These are not translations of the same sentences.
            issues.append({
                "context": f"{context}/scene/{pair}",
                "warning": "Upstream replaces the Eight-Ball scene; not paired with unrelated English dialogue",
                "unmapped_spanish": [decode_text(string_at(spanish, st + unpack(
                    spanish, "<bbb5h", ss + i * 13)[5]), True) for i in range(13)],
            })
        for index in range((et - es) // 13):
            original = unpack(english, "<bbb5h", es + index * 13)
            key = f"{context}/scene/{pair}/record/{index}"
            if original[0] == -2:
                continue
            if changed_scene:
                add_entry(entries, issues, key, string_at(english, et + original[5]), b"")
                continue
            localized = unpack(spanish, "<bbb5h", ss + index * 13)
            if (original[0], original[2]) != (localized[0], localized[2]):
                raise ValueError(f"Scene shot/speaker differs: {key}")
            if original[1] != localized[1]:
                issues.append({"context": key, "warning": "Upstream text colour differs; import text only"})
            # Test expressions and animation strings are scripts, not dialogue.
            original_test = string_at(english, et + original[4])
            localized_test = string_at(spanish, st + localized[4])
            if key == "sm1/BRIEFING.001/packet/24/scene/1/record/9" and original_test and not localized_test:
                # The English resource points textOffset at mouth-animation
                # syllables and testsOffset at the actual spoken sentence.
                add_entry(entries, issues, key, original_test,
                          string_at(spanish, st + localized[5]))
                warning = "English data has misplaced text/test offsets; source taken from testsOffset"
                entries[-1]["warnings"].append(warning)
                issues.append({"context": key, "warning": warning})
                continue
            add_entry(entries, issues, key,
                      string_at(english, et + original[5]),
                      string_at(spanish, st + localized[5]))
            if original_test != localized_test:
                warning = "Scene conditions differ between editions; text pairing requires review"
                entries[-1]["warnings"].append(warning)
                issues.append({"context": key, "warning": warning,
                               "english_condition": original_test.decode("ascii"),
                               "spanish_condition": localized_test.decode("ascii")})


def fixed_entries(entries, issues, context, english, spanish, stride, offset, capacity, per_group=None, group="mission"):
    if len(english) != len(spanish) or len(english) % stride:
        raise ValueError(f"Fixed records do not align: {context}")
    for index in range(len(english) // stride):
        position = index * stride + offset
        key = f"{context}/record/{index}"
        if per_group:
            key = f"{context}/{group}/{index // per_group}/record/{index % per_group}"
        add_entry(entries, issues, key, string_at(english, position, capacity),
                  string_at(spanish, position, capacity), font=1, capacity=capacity - 1)


def import_resources(english_root, spanish):
    entries = []
    issues = []
    sources = {}

    def load(name):
        original = (english_root / name).read_bytes()
        translated = spanish["GAMEDAT/" + name]
        sources[name] = {"english_sha256": digest(original), "spanish_sha256": digest(translated)}
        return original, translated

    for campaign, suffix in zip(CAMPAIGNS, ("000", "001", "002")):
        name = f"BRIEFING.{suffix}"
        original, translated = map(packets, load(name))
        if len(original) != len(translated):
            raise ValueError(f"Packet counts differ: {name}")
        for index, (en, es) in enumerate(zip(original, translated)):
            if not en and not es:
                continue
            count = 7 if index == 0 else 1 if index < 3 else 5
            scene_entries(entries, issues, f"{campaign}/{name}/packet/{index}", en, es, count)
        name = f"MODULE.{suffix}"
        original, translated = map(packets, load(name))
        if len(original) != 6 or len(translated) != 6:
            raise ValueError(f"Expected six module packets: {name}")
        for packet, label, stride, offset, capacity, per_mission in (
            (1, "nav", 77, 0, 30, 16),
            (2, "objective", 64, 4, 60, 16),
            (4, "mission-name", 40, 0, 40, None),
            (5, "series-name", 40, 0, 40, None),
        ):
            fixed_entries(entries, issues, f"{campaign}/{name}/{label}",
                          original[packet], translated[packet], stride, offset, capacity, per_mission)
    for name, domain in (("COMMUNIC.DAT", "common"), ("COMMSM2.DAT", "sm2")):
        original, translated = load(name)
        fixed_entries(entries, issues, f"{domain}/{name}", original, translated, 80, 0, 80, 11, "speaker")
    return entries, issues, sources


def quote(text):
    return json.dumps(text, ensure_ascii=False)


def font_changes(english, spanish):
    """Keep only modified glyphs, never a complete replacement font resource."""
    original = packets(english)
    translated = packets(spanish)
    if len(original) != 4 or len(translated) != 4:
        raise ValueError("Expected four WC1 fonts")
    fonts = []
    for index, (source, target) in enumerate(zip(original, translated)):
        if source[:4] != target[:4] or len(source) < 772 or len(target) < 772:
            raise ValueError("Font header differs")
        height, = unpack(source, "<H")
        glyphs = []
        for character in range(256):
            bitmaps = []
            widths = []
            for font in (source, target):
                width = font[4 + character]
                offset = font[260 + character] + 256 * font[516 + character]
                if width and (offset < 772 or offset + width * height > len(font)):
                    raise ValueError("Glyph exceeds font resource")
                widths.append(width)
                bitmaps.append(font[offset:offset + width * height])
            if widths[0] != widths[1] or bitmaps[0] != bitmaps[1]:
                glyphs.append({"character": character, "width": widths[1], "pixels": bitmaps[1].hex()})
        fonts.append({"font": index, "height": height, "ink": source[2],
                      "background": source[3], "source_crc32": zlib.crc32(source), "glyphs": glyphs})
    return {"schema": 1, "fonts": fonts}


# Only translated images. Other upstream VGA changes fix animation/pixels and
# belong to the independent data-patch system, not to the language setting.
ART = {"TITLE.VGA": {1: list(range(26, 32)), 4: [0, 1, 2]},
       "TITLE1.VGA": {0: [0, 1]}, "COCKPIT.VGA": {7: [0]},
       "RECROOM.VGA": {0: [0]}, "SERIES.VGA": {0: [0]}}


def executable_strings(data):
    """Parse English IDA db comments paired with text-only B/M/P directives."""
    source = None
    result = {}
    for number, line in enumerate(data.decode("cp1252").splitlines(), 1):
        match = re.match(r"#\s*seg\w*:[0-9A-Fa-f]+\s+(?:(\w+)\s+)?db\s+(.+)$", line)
        if match:
            if match[1] is not None:
                source = ""
            if source is not None:
                # Strip assembler comments before accepting numeric db items.
                for word, value in re.findall(r"'([^']*)'|\b([0-9A-F]+h|[0-9]+)\b", match[2].split(';')[0], re.I):
                    if word:
                        source += word
                    elif value:
                        byte = int(value[:-1], 16) if value.lower().endswith('h') else int(value)
                        if byte:
                            source += chr(byte)
            continue
        match = re.match(r'[BMP][0-9A-Fa-f]+:(.*)', line)
        if match and source:
            text = match[1].split('\\0')[0].replace('\\A', '\n').replace('\\D', '\r')
            if '\\&' not in text:
                result[source] = {"text": text, "line": number}
            source = None
        elif re.match(r'^[A-Z][0-9A-Fa-f]+:', line):
            source = None
    return result


def ui_translation(record, context):
    text = record['text']
    if context == 'intro':
        return text.translate(str.maketrans('[\\]^_`', 'áéíñóú'))
    # The executable patch occasionally uses normal Unicode in font-1 fields,
    # too. Decode only the three explicit alternate glyph slots.
    return text.translate(str.maketrans('àëï', 'ñóú'))


def shape_with_frames(source, target, selected):
    originals, translations = packets(source), packets(target)
    if len(originals) != len(translations):
        raise ValueError('Artwork frame count changed')
    frames = [translations[i] if i in selected else frame for i, frame in enumerate(originals)]
    offsets, size = [], (len(frames) + 1) * 4
    for frame in frames:
        offsets.append(size)
        size += len(frame)
    return struct.pack('<' + 'I' * (len(frames) + 1), size, *offsets) + b''.join(frames)


def delta(source, target):
    """Store insertions/replacements only; all equal bytes stay in GAMEDAT."""
    # Strip common ends before SequenceMatcher to bound work on large images.
    start = 0
    while start < min(len(source), len(target)) and source[start] == target[start]:
        start += 1
    a, b = len(source), len(target)
    while a > start and b > start and source[a - 1] == target[b - 1]:
        a -= 1
        b -= 1
    edits = []
    for tag, i, j, k, l in difflib.SequenceMatcher(None, source[start:a], target[start:b], autojunk=False).get_opcodes():
        if tag != 'equal':
            edits.append([start + i, j - i, target[start + k:start + l].hex()])
    return edits


def import_extras(english, spanish, template):
    inventory, sources = {}, {}
    for name in ('Tools/_EXE_WORK/patch_work.txt', 'Tools/_EXE_WORKSM2/patch_worksm2.txt'):
        sources[name] = digest(spanish[name])
        for source, record in executable_strings(spanish[name]).items():
            inventory.setdefault(source, {**record, 'file': name})
    ui = []
    for entry, _, _, _ in read_po(template):
        context, source = entry.get('msgctxt', ''), entry['msgid']
        record = inventory.get(source) or inventory.get(source.rstrip('\n'))
        translation = ui_translation(record, context) if record else ''
        if translation and source.endswith('\n') and not translation.endswith('\n'):
            translation += '\n'
        ui.append({'context': context, 'source': source,
                   'translation': translation,
                   'font': int(context[-1]) if context in ('font0', 'font2', 'font3') else 1,
                   'capacity': None, 'warnings': [],
                   'notes': [f"Source: {record['file']}:{record['line']}" if record
                             else 'Translation: wc1-re contributors.']})
    entries, issues = [], []
    for i in range(9):
        name = f'MIDGAME.V0{i}'
        source = (english / name).read_bytes()
        target = spanish['GAMEDAT/' + name]
        sources[name] = {'english_sha256': digest(source), 'spanish_sha256': digest(target)}
        original, translated = packets(source), packets(target)
        for section in (5, 6):
            if original[section]:
                scene_entries(entries, issues, f'common/{name}/packet/{section}',
                              original[section], translated[section], 1)
    assets = []
    for name, sections in ART.items():
        source = (english / name).read_bytes()
        target = spanish['GAMEDAT/' + name]
        sources[name] = {'english_sha256': digest(source), 'spanish_sha256': digest(target)}
        original, translated = packets(source), packets(target)
        for section, frames in sections.items():
            before = original[section]
            after = shape_with_frames(before, translated[section], frames)
            assets.append({'file': name, 'section': section, 'frames': frames,
                           'source_size': len(before), 'source_crc32': zlib.crc32(before),
                           'target_size': len(after), 'target_crc32': zlib.crc32(after),
                           'edits': delta(before, after)})
    artwork = json.dumps({'schema': 1, 'packets': assets}, indent=2)
    artwork = re.sub(r'\[\s+(\d+),\s+(\d+),\s+("[0-9a-f]*")\s+\]',
                     r'[\1, \2, \3]', artwork)
    return {'es/ui.po': po_document(ui), 'es/cinematics.po': po_document(entries),
            'es/artwork.json': artwork + '\n'}, sources


def po_document(entries, template=False):
    lines = [
        "# WC1 and Secret Missions text imported from Spanish version 1.1 (2021).",
        "# Translation: L@Zar0. Text revision: darkpadawan.",
        "# Upstream declares GPL v3.0; see upstream/Leeme.txt for its complete notice.",
        'msgid ""', 'msgstr ""',
    ]
    for line in (
        "Project-Id-Version: wc1-re\n", f"Language: {'' if template else 'es'}\n",
        "Last-Translator: L@Zar0; text revision by darkpadawan\n",
        "Language-Team: Spanish\n",
        "PO-Revision-Date: 2021-04-13 00:00+0000\n",
        "MIME-Version: 1.0\n", "Content-Type: text/plain; charset=UTF-8\n",
        "Content-Transfer-Encoding: 8bit\n", "Plural-Forms: nplurals=2; plural=(n != 1);\n",
    ):
        lines.append(quote(line))
    for entry in entries:
        lines.append("")
        lines.extend("# " + note for note in entry.get("notes", []))
        if entry["capacity"] is not None:
            lines.append(f"#. Original field capacity: {entry['capacity']} encoded bytes before NUL.")
        for warning in entry["warnings"]:
            lines.append(f"#. {warning}")
        if entry["warnings"] and not template:
            lines.append("#, fuzzy")
        lines.extend(["msgctxt " + quote(entry["context"]),
                      "msgid " + quote(entry["source"]),
                      "msgstr " + quote("" if template else entry["translation"])])
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--english", type=Path, required=True, help="Original DOS installation or GAMEDAT directory")
    parser.add_argument("--spanish", type=Path, required=True, help="Unmodified WC1_SMIyII_V1.1.zip")
    parser.add_argument("--output", type=Path, required=True, help="New catalog directory; must not exist")
    args = parser.parse_args()
    try:
        if args.output.exists():
            raise ValueError("Output already exists; use a new directory to protect edited catalogs")
        root = args.english / "GAMEDAT" if (args.english / "GAMEDAT").is_dir() else args.english
        spanish = read_spanish(args.spanish)
        entries, issues, sources = import_resources(root, spanish)
        contexts = [entry["context"] for entry in entries]
        if len(contexts) != len(set(contexts)):
            raise ValueError("Duplicate catalog contexts")
        template = Path(__file__).resolve().parent.parent / "translations/templates/ui.pot"
        documents, extra_sources = import_extras(root, spanish, template)
        sources.update(extra_sources)
        for domain in ("common",) + CAMPAIGNS:
            selected = [e for e in entries if e["context"].startswith(domain + "/")]
            documents[f"es/{domain}.po"] = po_document(selected)
        for name in ("Leeme.txt", "Tools/charmap.txt", "Tools/nomenclaturaWC1.txt"):
            documents["upstream/" + Path(name).name] = "\n".join(line.rstrip() for line in spanish[name].decode("cp1252").splitlines()) + "\n"
        fonts = font_changes((root / "FONTS.FNT").read_bytes(), spanish["GAMEDAT/FONTS.FNT"])
        documents["es/glyphs.json"] = json.dumps(fonts, indent=2) + "\n"
        report = {
            "schema": 1, "language": "es", "upstream_version": "1.1",
            "upstream_date": "2021-04-13", "authors": ["L@Zar0", "darkpadawan"],
            "upstream_url": SPANISH_URL, "upstream_zip_sha256": SPANISH_SHA256,
            "catalog_entries": len(entries),
            "review_required": sum(bool(e["warnings"]) for e in entries),
            "domains": dict(Counter(e["context"].split("/")[0] for e in entries)),
            "sources": sources, "issues": issues,
            "font_sources": {"english_sha256": digest((root / "FONTS.FNT").read_bytes()),
                             "spanish_sha256": digest(spanish["GAMEDAT/FONTS.FNT"])},
            "not_imported": ["intro copy-protection questions", "DOS diagnostics", "installer/transfer utilities"],
        }
        documents["provenance.json"] = json.dumps(report, ensure_ascii=False, indent=2) + "\n"
        for name, text in documents.items():
            destination = args.output / name
            destination.parent.mkdir(parents=True, exist_ok=True)
            destination.write_text(text, encoding="utf-8")
        print(f"Imported {len(entries)} entries; {report['review_required']} require review. Output: {args.output}")
    except (OSError, ValueError, KeyError, zipfile.BadZipFile, zlib.error, EOFError) as error:
        parser.exit(1, f"Import failed: {error}\n")


if __name__ == "__main__":
    main()
