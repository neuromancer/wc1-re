#!/usr/bin/env python3
"""Compile voice assignments; optionally validate a local WAV directory. Never copy audio."""
import argparse
from pathlib import Path, PurePosixPath
import re
import struct
import wave

from compileTranslations import MESSAGE_ID, read_po


def compile_voices(source, root, translations):
    aliases = {}
    for path in sorted(translations.glob("*.po")):
        for entry, fuzzy, *_ in read_po(path):
            if not fuzzy and entry["msgstr"]:
                aliases[(entry.get("msgctxt", ""), entry["msgid"].strip()[:20])] = entry["msgstr"]
    messages = {}
    skipped = 0
    checked = set()
    for entry, fuzzy, *_ in read_po(source):
        context, key, filename = entry.get("msgctxt", ""), entry["msgid"], entry["msgstr"]
        if not re.fullmatch(r"wc1/BRIEFING\.000/packet/\d+/scene/\d+/record/\d+|"
                            r"common/COMMUNIC\.DAT/speaker/(?:[0-9]|1[0-3])/record/(?:[0-9]|10)(?:/rank/[0-4])?", context) or not MESSAGE_ID.fullmatch(key):
            raise ValueError("Expected a WC1 scene or radio context and opaque text ID")
        if fuzzy or not filename:
            skipped += 1
            continue
        path = PurePosixPath(filename)
        if path.is_absolute() or ".." in path.parts or not re.fullmatch(r"[A-Za-z0-9_./ -]+\.wav", filename, re.I):
            raise ValueError(f"Invalid relative WAV path: {filename}")
        if root is not None and filename not in checked:
            resolved = (root / path).resolve()
            if not resolved.is_relative_to(root.resolve()):
                raise ValueError(f"WAV is outside the voice directory: {filename}")
            with wave.open(str(resolved)) as audio:
                if (audio.getcomptype() != "NONE" or audio.getnchannels() not in (1, 2) or
                        audio.getsampwidth() not in (1, 2) or not 4000 <= audio.getframerate() <= 96000 or
                        not 0 < audio.getnframes() <= audio.getframerate() * 120):
                    raise ValueError(f"Unsupported PCM WAV: {filename}")
                size = audio.getnframes() * audio.getnchannels() * audio.getsampwidth()
                if len(audio.readframes(audio.getnframes())) != size:
                    raise ValueError(f"Truncated WAV: {filename}")
        checked.add(filename)
        ids = [key.strip()[:20]]
        translated = aliases.get((re.sub(r"/rank/\d+$", "", context), key.strip()[:20]))
        if translated:
            value = 14695981039346656037
            for byte in translated.encode("cp1252"):
                value = ((value ^ byte) * 1099511628211) & 0xffffffffffffffff
            ids.append(f"wc1:{value:016x}")
        for text_id in set(ids):
            if (context, text_id) in messages:
                raise ValueError(f"Duplicate voice entry: {context}")
            messages[context, text_id] = filename
    if not messages or len(messages) > 16384:
        raise ValueError("Expected 1..16384 voice mappings")
    directory = (str(root.resolve()) if root is not None else ".").encode("utf-8") + b"\0"
    if len(directory) > 4096:
        raise ValueError("Voice directory path is too long")
    output = bytearray(b"WCV1" + struct.pack("<IH", len(messages), len(directory)) + directory)
    for (context, key), filename in sorted(messages.items()):
        fields = [s.encode("ascii") + b"\0" for s in (context, key, filename)]
        if any(len(s) > 4096 for s in fields):
            raise ValueError("Voice mapping field is too long")
        output.extend(struct.pack("<3H", *(len(s) for s in fields)))
        output.extend(b"".join(fields))
    return output, len(checked), skipped


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    repo = Path(__file__).resolve().parent.parent
    parser.add_argument("--source", type=Path, default=repo / "voices/segacd.po")
    parser.add_argument("--voice-root", type=Path, help="validate local WAVs and pin their directory; otherwise build a portable mapping")
    parser.add_argument("--translations", type=Path, default=repo / "translations/es")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    try:
        data, recordings, skipped = compile_voices(args.source, args.voice_root, args.translations)
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(data)
        print(f"Mapped {recordings} recording filenames; {skipped} unmapped/review entries remain silent. No audio copied.")
    except (OSError, ValueError, wave.Error, EOFError) as error:
        parser.exit(1, f"Voice compilation failed: {error}\n")


if __name__ == "__main__":
    main()
