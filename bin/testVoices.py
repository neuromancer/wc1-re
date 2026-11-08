#!/usr/bin/env python3
"""Voice catalog checks using generated PCM, never game recordings."""
import json
import math
import os
import shutil
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest
import wave

from compileTranslations import message_key, read_po, MESSAGE_ID
from compileVoices import compile_voices


class VoiceTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.translations = self.root / "translations"
        self.translations.mkdir()
        for name, value in (("one.wav", 192), ("two.wav", 64)):
            with wave.open(str(self.root / name), "wb") as audio:
                audio.setparams((1, 1, 16000, 0, "NONE", "not compressed"))
                audio.writeframes(bytes([value]) * 1600)
        self.source = self.root / "voices.po"
        self.entries = [self.entry(0, "one.wav"), self.entry(1, "two.wav")]
        self.write_catalog()
        (self.translations / "test.po").write_text(self.entry(0, "Hola $C"), encoding="utf-8")

    def entry(self, record, value):
        return (f'msgctxt "wc1/BRIEFING.000/packet/1/scene/0/record/{record}"\n'
                f'msgid "{message_key("Hello $C").split()[0]}"\n'
                'msgstr ' + json.dumps(value) + '\n')

    def write_catalog(self):
        self.source.write_text("\n".join(self.entries), encoding="utf-8")

    def test_catalog_has_only_ids_paths_and_local_root(self):
        data, count, skipped = compile_voices(self.source, self.root, self.translations)
        self.assertEqual((count, skipped), (2, 0))
        self.assertEqual(data[:4], b"WCV1")
        self.assertNotIn(b"Hello", data)
        self.assertNotIn(b"Hola", data)
        self.assertNotIn(b"RIFF", data)
        self.assertEqual(struct.unpack_from("<I", data, 4)[0], 3)

    def test_portable_mapping_needs_no_recordings(self):
        (self.root / "one.wav").unlink()
        (self.root / "two.wav").unlink()
        data, count, skipped = compile_voices(self.source, None, self.translations)
        self.assertEqual((count, skipped), (2, 0))
        self.assertEqual(data[8:12], b"\x02\0.\0")
        self.assertNotIn(str(self.root).encode(), data)

    def test_duplicate_and_unsafe_paths_are_rejected(self):
        for entry in (self.entries[0], self.entry(2, "../one.wav"), self.entry(2, "/one.wav")):
            self.source.write_text("\n".join(self.entries + [entry]))
            with self.assertRaises(ValueError):
                compile_voices(self.source, self.root, self.translations)

    def test_unmapped_and_fuzzy_entries_stay_silent(self):
        self.entries.extend([self.entry(2, ""), "#, fuzzy\n" + self.entry(3, "missing.wav")])
        self.write_catalog()
        _, count, skipped = compile_voices(self.source, self.root, self.translations)
        self.assertEqual((count, skipped), (2, 2))

    def test_missing_or_truncated_audio_is_rejected(self):
        (self.root / "one.wav").write_bytes((self.root / "one.wav").read_bytes()[:-1])
        with self.assertRaisesRegex(ValueError, "Truncated WAV"):
            compile_voices(self.source, self.root, self.translations)
        (self.root / "one.wav").unlink()
        with self.assertRaises(OSError):
            compile_voices(self.source, self.root, self.translations)

    def test_shipped_po_contains_only_ids_and_relative_wav_names(self):
        for entry, *_ in read_po(Path(__file__).resolve().parent.parent / "voices/segacd.po"):
            self.assertIsNotNone(MESSAGE_ID.fullmatch(entry["msgid"]))
            if entry["msgstr"]:
                self.assertTrue(entry["msgstr"].lower().endswith(".wav"))
                self.assertNotIn("..", entry["msgstr"])

    def radio_entry(self, speaker, suffix, value):
        return (f'msgctxt "common/COMMUNIC.DAT/speaker/{speaker}/record/0{suffix}"\n'
                f'msgid "{message_key("Hello $R $C").split()[0]}"\n'
                'msgstr ' + json.dumps(value) + '\n')

    def test_invalid_radio_context_is_rejected(self):
        for speaker, suffix in ((14, ""), (-1, ""), (0, "/rank/5")):
            self.entries = [self.radio_entry(speaker, suffix, "one.wav")]
            self.write_catalog()
            with self.assertRaises(ValueError):
                compile_voices(self.source, self.root, self.translations)

    def scene_fixtures(self):
        paths = []
        for language, text in (("english", b"Hello $C"), ("spanish", b"Hola $C")):
            record = struct.pack("<bbb5h", 0, 0, 0, 1, 0, 1, 0, 0)
            end = struct.pack("<bbb5h", -2, 0, 0, 0, 0, 0, 0, 0)
            packet = struct.pack("<II", 8, 47) + record * 2 + end + b"\0" + text + b"\0"
            path = self.root / (language + ".bin")
            path.write_bytes(packet)
            paths.append(str(path))
        return paths

    @unittest.skipUnless(os.environ.get("WC1_VOICE_PROBE"), "run make check-voices for playback checks")
    def test_runtime_playback_and_spanish_aliases(self):
        self.entries.extend([self.radio_entry(0, "", "one.wav"), self.radio_entry(1, "", "two.wav"),
                             self.radio_entry(2, "/rank/0", "one.wav"), self.radio_entry(2, "/rank/4", "two.wav")])
        self.write_catalog()
        (self.translations / "radio.po").write_text(self.radio_entry(2, "", "Hola $R $C"), encoding="utf-8")
        data, _, _ = compile_voices(self.source, self.root, self.translations)
        catalog = self.root / "voices.dat"
        catalog.write_bytes(data)
        paths = self.scene_fixtures()
        for channels, width, rate in ((1, 1, 16000), (1, 2, 44100), (2, 2, 48000)):
            with self.subTest(channels=channels, width=width, rate=rate):
                speech = self.root / "speech.wav"
                pcm = bytearray()
                for sample in range(rate * 8 // 10):
                    t = sample / rate
                    tone = math.sin(2 * math.pi * 220 * t) if 0.1 <= t < 0.3 or 0.5 <= t < 0.7 else 0
                    for channel in range(channels):
                        value = tone * (1 if channel == 0 else -1)
                        pcm.extend(bytes([132 + round(48 * value)]) if width == 1 else
                                   struct.pack("<h", 512 + round(12000 * value)))
                with wave.open(str(speech), "wb") as audio:
                    audio.setparams((channels, width, rate, 0, "NONE", "not compressed"))
                    audio.writeframes(pcm)
                subprocess.run([os.environ["WC1_VOICE_PROBE"], str(catalog), *paths, str(speech)], check=True)

    @unittest.skipUnless(os.environ.get("WC1_VOICE_PROBE"), "run make check-voices for playback checks")
    def test_installation_detection_and_portable_playback(self):
        binary = self.root / "bin" / Path(os.environ["WC1_VOICE_PROBE"]).name
        (binary.parent / "voices").mkdir(parents=True)
        shutil.copy2(os.environ["WC1_VOICE_PROBE"], binary)
        self.entries[0] = self.entry(0, "Angel/rig0000.wav")
        self.write_catalog()
        data, _, _ = compile_voices(self.source, None, self.translations)
        (binary.parent / "voices/segacd.dat").write_bytes(data)
        paths = self.scene_fixtures()
        layouts = (("Juego español", "Juego español/Data/Voice"),
                   ("nested", "nested/WingLoader/Data/Voice"),
                   ("loader/Games/WC1", "loader/Data/Voice"),
                   ("lower/games/wc1", "lower/dAtA/vOiCe"))
        for game, voices in layouts:
            directory = self.root / game
            directory.mkdir(parents=True, exist_ok=True)
            subprocess.run([str(binary), "--detect", str(directory), "0"], check=True)
            voice_root = self.root / voices
            (voice_root / "aNgEl").mkdir(parents=True)
            marker = voice_root / "aNgEl/RIG0000.WAV"
            marker.write_bytes(b"not a wave file")
            subprocess.run([str(binary), "--detect", str(directory), "0"], check=True)
            shutil.copyfile(self.root / "one.wav", marker)
            shutil.copyfile(self.root / "two.wav", voice_root / "two.wav")
            subprocess.run([str(binary), "--detect", str(directory), "1"], check=True)
            subprocess.run([str(binary), "--installed", str(directory), *paths], check=True)
        for game in ("loader/Games/WC2", "loader/random"):
            directory = self.root / game
            directory.mkdir(parents=True)
            subprocess.run([str(binary), "--detect", str(directory), "0"], check=True)
        moved = self.root / "Moved game"
        (self.root / "Juego español").rename(moved)
        subprocess.run([str(binary), "--installed", str(moved), *paths], check=True)


if __name__ == "__main__":
    unittest.main()
