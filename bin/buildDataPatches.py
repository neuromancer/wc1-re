#!/usr/bin/env python3
"""Build the curated SDL data deltas for Wing Commander.

No downloads and no changes to either installation. Source CRCs deliberately
pin the reviewed DOS/Kilrathi Saga and 35th Anniversary WCAT sections. Revisit
the comments and selection before accepting a different WCAT revision.

    python3 bin/buildDataPatches.py --original data/dos/GAMEDAT \
        --wcat '/path/to/WCAT/files/GAMEWC1'

Only changed bytes and the short context between nearby edits enter the C
header, not complete replacement assets. Runtime guards use the decoded
section, so LZW compression and outer archive offsets do not affect matching.
SM1 also receives a correction to one malformed debriefing record.
The larger BRIEFING.000 dialogue/lip-sync rewrite
is not included here. WCDX stream corrections are generated from its documented
original chunk headers, using the same delta and checksum format.
"""
import argparse
import difflib
from pathlib import Path
import struct
import zlib


# filename, section, source CRC32, WCAT CRC32, selected RLE frames (None = script),
# explanation. Frame choices exclude recompression with identical pixels and
# unused duplicate frames; each correction remains independently documented.
FIXES = [
    ("BRIEFING.VGA", 8, 0x46e882c5, 0xb477b4b7, [0],
     "Restore Halcyon's missing shoulder insignia in the medal ceremony."),
    ("BRIEFING.VGA", 9, 0x20c89e56, 0xf09cbd18, [8],
     "Remove four opaque colour-255 pixels from the ceremony overlay."),
    ("TALKING.VGA", 4, 0x18170aed, 0x2ce648b4, [0],
     "Remove Iceman's stray portrait pixel at (212, 63)."),
    ("TALKING.VGA", 5, 0x6cbb53d0, 0x959617c9, [0],
     "Remove Angel's stray portrait pixel at (233, 81)."),
    ("WINGMEN.VGA", 2, 0xe9625c5b, 0xb6182465, list(range(5)),
     "Correct Hunter's helmet label (DART) across all five comm portraits."),
    ("WINGMEN.VGA", 7, 0x64ae288f, 0xe79a41e6, list(range(5)),
     "Correct Maniac's helmet label (JOKER) across all five comm portraits."),
    ("RECROOM.VGA", 12, 0xd8f77354, 0xb61e64fc, [0] + list(range(37, 49)),
     "Repair the barracks bucket background and splash edges in frames 37-48."),
    ("PCSHIP.V00", 8, 0xb54d942a, 0x6e26e133, [1],
     "Make the Hornet launch canopy transparent, as on the other fighters."),
    ("MIDGAME.V03", 0, 0xb92a169a, 0xc29f069d, [0],
     "Remove opaque colour-255 speckles from the Rostov cutscene backdrop."),
    ("MIDGAME.V03", 2, 0xfab6e43d, 0xf53c70a7, None,
     "Repair the Rostov loss explosion: remove per-frame LT 1 delays, hold the\n"
     "last image for LT 64, then end with W 0. Relocate subsequent scripts."),
    ("CAMP.000", 1, 0x0c319175, 0x1b75a806, None,
     "Lower three unreachable medal thresholds by 10, matching WCAT's fix for\n"
     "a perfect medal sweep without sacrificing a wingman."),
]


# WCDX dist/patchmusic/src/main.cpp: the two Kilrathi Saga victory fanfares
# must return to trigger 3873 instead of looping through their branch lists.
# These are complete 24-byte original headers, not embedded audio assets.
# https://github.com/Bekenn/wcdx/blob/58da90d/dist/patchmusic/src/main.cpp
STREAM_FIXES = [
    (0x03a1c7bc, (0x01a28da0, 0x01a40058, 0, 2565, 1, 3419),
     "Stop the first victory fanfare (audio chunk 311) from looping."),
    (0x03a1c894, (0x01aeaa68, 0x01b04410, 0, 2565, 1, 3433),
     "Stop the second victory fanfare (audio chunk 320) from looping."),
]


def u32(data, offset=0):
    return struct.unpack_from("<I", data, offset)[0]


def decompress_lzw(data):
    """Origin's little-endian, 9-to-12-bit LZW (as in src/sdl/resources.c)."""
    expected = u32(data)
    data = data[4:]
    position = 0
    width = 9
    dictionary = [bytes([i]) for i in range(256)] + [b"", b""]
    previous = b""
    result = bytearray()
    first = True
    while True:
        start = position // 8
        code = (int.from_bytes(data[start:start + 3], "little") >>
                (position % 8)) & ((1 << width) - 1)
        position += width
        if position > len(data) * 8:
            raise ValueError("Truncated LZW section")
        if first and code != 256:
            raise ValueError("Missing LZW clear code")
        first = False
        if code == 256:
            del dictionary[258:]
            width = 9
            previous = b""
            continue
        if code == 257:
            break
        if code < len(dictionary):
            word = dictionary[code]
        elif code == len(dictionary) and previous:
            word = previous + previous[:1]
        else:
            raise ValueError("Invalid LZW code")
        result.extend(word)
        if len(result) > expected:
            raise ValueError("LZW output exceeds declared length")
        if previous:
            if len(dictionary) >= 4096:
                raise ValueError("LZW dictionary overflow")
            dictionary.append(previous + word[:1])
            if len(dictionary) == 1 << width and width < 12:
                width += 1
        previous = word
    if len(result) != expected:
        raise ValueError("LZW output length mismatch")
    return bytes(result)


def split_packet(data):
    if len(data) < 8 or u32(data) != len(data):
        raise ValueError("Invalid packet length")
    header_size = u32(data, 4) & 0xffffff
    if header_size < 8 or header_size > len(data) or header_size % 4:
        raise ValueError("Invalid packet directory")
    entries = [u32(data, i) for i in range(4, header_size, 4)] + [len(data)]
    sections = []
    for entry, following in zip(entries, entries[1:]):
        start, end = entry & 0xffffff, following & 0xffffff
        if not header_size <= start <= end <= len(data):
            raise ValueError("Invalid packet offsets")
        section = data[start:end]
        sections.append(decompress_lzw(section) if entry >> 24 == 1 else section)
    return header_size, sections


def differences(source, target, offset, comment):
    """Trim unchanged ends before diffing to avoid quadratic large-image work."""
    start = 0
    while start < min(len(source), len(target)) and source[start] == target[start]:
        start += 1
    end_a, end_b = len(source), len(target)
    while end_a > start and end_b > start and source[end_a - 1] == target[end_b - 1]:
        end_a -= 1
        end_b -= 1
    edits = []
    matcher = difflib.SequenceMatcher(None, source[start:end_a],
                                     target[start:end_b], autojunk=False)
    for tag, i, j, k, l in matcher.get_opcodes():
        if tag != "equal":
            i, j, k, l = i + start, j + start, k + start, l + start
            # Keep nearby changes together, including directory words, so
            # deletions/insertions remain understandable instead of byte soup.
            if edits and offset + i - (edits[-1][0] + edits[-1][1]) <= 8:
                old_offset, old_size, literal, _ = edits.pop()
                gap_start = old_offset + old_size - offset
                literal += source[gap_start:i] + target[k:l]
                edits.append((old_offset, offset + j - old_offset, literal, comment))
            else:
                edits.append((offset + i, j - i, target[k:l], comment))
    return edits


def curate(source, target, frames, filename, comment):
    if frames is None:
        if filename == "CAMP.000":
            changes = [(0x188, "Brimstone 2 medal threshold: 100 -> 90."),
                       (0x3ea, "Venice 1 medal threshold: 165 -> 155."),
                       (0x480, "Hell's Kitchen 4 medal threshold: 220 -> 210.")]
            edits = [(offset, 2, target[offset:offset + 2], text)
                     for offset, text in changes]
        else:
            edits = differences(source, target, 0, comment)
        return target, edits
    source_header, original_frames = split_packet(source)
    target_header, fixed_frames = split_packet(target)
    if source_header != target_header:
        raise ValueError("Selected patch changes the frame count")
    chosen = [fixed_frames[i] if i in frames else frame
              for i, frame in enumerate(original_frames)]
    offsets = []
    position = source_header
    for frame in chosen:
        offsets.append(position)
        position += len(frame)
    header = struct.pack("<" + "I" * (len(offsets) + 1), position, *offsets)
    edits = differences(source[:source_header], header, 0,
                        "Update the RLE size and frame offsets after the corrections below.")
    position = source_header
    for i, (original, corrected) in enumerate(zip(original_frames, chosen)):
        edits += differences(original, corrected, position,
                             f"Frame {i}: {comment}")
        position += len(original)
    return header + b"".join(chosen), edits


def emit_patch(lines, filename, location, source, target, edits, comment):
    # Validate both formats with the same splice semantics as the runtime.
    rebuilt = bytearray()
    position = 0
    for offset, removed, literal, _ in edits:
        if offset < position or offset + removed > len(source):
            raise ValueError("Overlapping or out-of-bounds delta")
        rebuilt.extend(source[position:offset])
        rebuilt.extend(literal)
        position = offset + removed
    rebuilt.extend(source[position:])
    if rebuilt != target:
        raise ValueError(f"{filename} #{location}: delta does not reproduce correction")
    name = filename.lower().replace(".", "_") + f"_{location}"
    lines += ["/* " + comment.replace("\n", "\n * ") + " */",
              f"static const unsigned char {name}Bytes[] = {{"]
    literals = b"".join(edit[2] for edit in edits)
    for i in range(0, len(literals), 12):
        lines.append("    " + ", ".join(f"0x{x:02x}" for x in literals[i:i + 12]) + ",")
    lines += ["};", f"static const SdlDataEdit {name}Edits[] = {{"]
    literal_offset = 0
    previous_comment = None
    for offset, removed, literal, explanation in edits:
        if explanation != previous_comment:
            lines.append("    /* " + explanation.replace("\n", "\n     * ") + " */")
            previous_comment = explanation
        lines.append(f"    {{ 0x{offset:04x}, {removed}, {len(literal)}, {literal_offset} }},")
        literal_offset += len(literal)
    lines += ["};", ""]
    return [f'    {{ "{filename}", {location}, {len(source)}, 0x{zlib.crc32(source):08x}U,',
            f"      {len(target)}, 0x{zlib.crc32(target):08x}U,",
            f"      {name}Edits, SDL_arraysize({name}Edits),",
            f"      {name}Bytes, sizeof({name}Bytes) }},"]


def generate(original_root, wcat_root):
    lines = ["/* Generated by bin/buildDataPatches.py from local reference assets",
             " * and WCDX's documented stream headers. No complete assets are embedded.",
             " * WCAT contributor changes: Copyright (c) 2026 Lance Burns.",
             " * See https://alltinker.itch.io/wcat.",
             " * WCDX: Copyright (c) 2014-2020 James Touton.",
             " * See https://github.com/Bekenn/wcdx.",
             " * Edits use offsets in the ORIGINAL decoded packet or stream record;",
             " * all are applied after checking its complete length and CRC32. */", ""]
    descriptors = []
    for filename, section, source_crc, wcat_crc, frames, comment in FIXES:
        source = split_packet((original_root / filename).read_bytes())[1][section]
        target = split_packet((wcat_root / filename).read_bytes())[1][section]
        if zlib.crc32(source) != source_crc or zlib.crc32(target) != wcat_crc:
            raise ValueError(f"{filename} #{section}: unreviewed source or WCAT revision")
        target, edits = curate(source, target, frames, filename, comment)
        descriptors += emit_patch(lines, filename, section, source, target, edits,
                                  f"{filename}, section {section}. {comment}")
    # SM1 has a sentence in testsOffset and mouth syllables in textOffset.
    # Restore the three offsets before the interpreter or translator sees it.
    source = split_packet((original_root / "BRIEFING.001").read_bytes())[1][24]
    if len(source) != 11193 or zlib.crc32(source) != 0xbe94a1f3:
        raise ValueError("BRIEFING.001 #24: unreviewed source revision")
    literal = struct.pack("<3H", 0, 824, 895)
    target = source[:0x126e] + literal + source[0x1274:]
    comment = "SM1 debriefing, scene 1 record 9: restore test, text and mouth offsets."
    descriptors += emit_patch(lines, "BRIEFING.001", 24, source, target,
                              [(0x126e, 6, literal, comment)], comment)
    lines += ["static const SdlDataPatch packetPatches[] = {", *descriptors, "};", ""]
    descriptors = []
    for offset, header, comment in STREAM_FIXES:
        source = struct.pack("<6I", *header)
        target = struct.pack("<6I", header[0], header[1], 1, 3873, 0, header[5])
        edits = [(8, 12, target[8:20],
                  "Enable trigger 3873 and disable the looping branch (WCDX patchmusic).")]
        descriptors += emit_patch(lines, "MISSION.STR", offset, source, target, edits,
                                  f"MISSION.STR, file offset 0x{offset:08x}. {comment}")
    lines += ["static const SdlDataPatch streamPatches[] = {", *descriptors, "};", ""]
    return "\n".join(lines)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, required=True)
    parser.add_argument("--wcat", type=Path, required=True)
    parser.add_argument("--output", type=Path,
                        default=Path(__file__).resolve().parents[1] / "src/sdl/data_patches.h")
    args = parser.parse_args()
    args.output.write_text(generate(args.original, args.wcat), encoding="utf-8")
    print(f"Wrote {len(FIXES) + 1} packet and {len(STREAM_FIXES)} stream patches to {args.output}")
