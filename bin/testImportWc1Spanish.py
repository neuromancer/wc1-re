#!/usr/bin/env python3
"""Data-format regression tests; no copyrighted game files are required."""

import gettext
import io
import json
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile
import unittest

import importWc1Spanish as importer
from importWc1French import french_text, menu_artwork


def packed_codes(codes):
    bits = 0
    value = 0
    for code, width in codes:
        value |= code << bits
        bits += width
    return value.to_bytes((bits + 7) // 8, "little")


def scene(text, duration=10, speaker=1, condition=b"", mouth=b"bimorgames"):
    # Minimal scene with separate condition, dialogue and animation strings.
    strings = b"\0" + condition + b"\0" + text + b"\0" + mouth + b"\0"
    text_offset = len(condition) + 2
    record = struct.pack("<bbb5h", 0, 2, speaker, duration, 1,
                         text_offset, text_offset + len(text) + 1, 0)
    end = struct.pack("<bbb5h", -2, 2, speaker, 0, 0, 0, 0, 0)
    return struct.pack("<II", 8, 34) + record + end + strings


class ImportSpanishTests(unittest.TestCase):
    def test_executable_import_reads_text_not_patch_instructions(self):
        data = ("# seg000:123 aText db 'Tiger',27h,'s Claw',0Ah\n"
                "# seg000:123 db 'Ready',0\n"
                "P001234:Garra de Tigre\\ALista\\0\n"
                "D000001:\\&90\\&90\\X\n").encode('cp1252')
        entries = importer.executable_strings(data)
        self.assertEqual(list(entries), ["Tiger's Claw\nReady"])
        self.assertEqual(entries["Tiger's Claw\nReady"]['text'], 'Garra de Tigre\nLista')

    def test_artwork_delta_preserves_unselected_frames(self):

        def shape(a, b):
            return struct.pack('<III', 12 + len(a) + len(b), 12, 12 + len(a)) + a + b

        source = shape(b'original-one', b'original-two')
        target = shape(b'translated-one-longer', b'unrelated-animation-edit')
        selected = importer.shape_with_frames(source, target, [0])
        self.assertEqual(importer.packets(selected), [b'translated-one-longer', b'original-two'])
        rebuilt, position = bytearray(), 0
        for offset, removed, inserted in importer.delta(source, selected):
            rebuilt.extend(source[position:offset])
            rebuilt.extend(bytes.fromhex(inserted))
            position = offset + removed
        rebuilt.extend(source[position:])
        self.assertEqual(rebuilt, selected)

    def test_midgame_extra_metadata_does_not_become_dialogue(self):
        def midgame(text):
            data = scene(text)
            return struct.pack('<II', 40, 66) + b'original-animation-header'.ljust(32, b'\0') + data[8:]
        entries, issues = [], []
        importer.scene_entries(entries, issues, 'common/MIDGAME.V00/packet/5',
                               midgame(b'Hello'), midgame(b'Hola'), 1)
        self.assertEqual([entry['translation'] for entry in entries], ['Hola'])
        self.assertFalse(issues)

    def test_french_menu_keeps_expansion_frame(self):
        def archive(sections):
            offsets, size = [], 4 * (len(sections) + 1)
            for section in sections:
                offsets.append(size)
                size += len(section)
            return struct.pack('<' + 'I' * (len(sections) + 1), size, *offsets) + b''.join(sections)
        source = archive([b'start', b'continue', b'expansion'])
        target = archive([b'demarrer', b'continuer', b'expansion-change'])
        patch, = menu_artwork(archive([b''] * 4 + [source]),
                              archive([b''] * 4 + [target]))['packets']
        result, position = bytearray(), 0
        for offset, removed, inserted in patch['edits']:
            result.extend(source[position:offset])
            result.extend(bytes.fromhex(inserted))
            position = offset + removed
        result.extend(source[position:])
        self.assertEqual(importer.packets(result), [b'demarrer', b'continuer', b'expansion'])

    def test_lzw_dictionary_reference_and_special_code(self):
        data = packed_codes([(256, 9), (65, 9), (66, 9), (258, 9), (260, 9), (257, 9)])
        self.assertEqual(importer.origin_lzw(data, 7), b"ABABABA")
        with self.assertRaises(ValueError):
            importer.origin_lzw(data, 6)
        with self.assertRaises(ValueError):
            importer.origin_lzw(data[:-1], 7)

    def test_lzw_width_transition(self):
        codes = [(256, 9)] + [(i, 9) for i in range(255)] + [(255, 10), (257, 10)]
        self.assertEqual(importer.origin_lzw(packed_codes(codes), 256), bytes(range(256)))

    def test_packet_empty_sections_and_lzw(self):
        data = packed_codes([(256, 9), (65, 9), (257, 9)])
        section = struct.pack("<I", 1) + data
        archive = struct.pack("<III", 12 + len(section), 0xff00000c, 0x0100000c) + section
        self.assertEqual(importer.packets(archive), [b"", b"A"])
        with self.assertRaises(ValueError):
            importer.packets(archive[:-1])

    def test_installer_decompression_limits(self):
        import zlib
        block = b"\1" + zlib.compress(b"test")
        self.assertEqual(importer.expand_installer_block(block, 4), b"test")
        with self.assertRaises(ValueError):
            importer.expand_installer_block(block, 3)
        with self.assertRaises(ValueError):
            importer.expand_installer_block(block[:-1], 4)

    def test_font_specific_encoding(self):
        encoded = b"ca\xe0\xebn, t\xef, s\xed, \xa1bien!"
        self.assertEqual(importer.decode_text(encoded, True, 1), "cañón, tú, sí, ¡bien!")
        self.assertEqual(importer.decode_text(b"\xe0\xeb\xef", True, 0), "àëï")

    def test_french_import_does_not_require_secret_missions(self):
        def archive(sections):
            offsets, size = [], 4 * (len(sections) + 1)
            for section in sections:
                offsets.append(size)
                size += len(section)
            return struct.pack('<' + 'I' * (len(sections) + 1), size, *offsets) + b''.join(sections)
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = {'BRIEFING.000': archive([b'']),
                      'MODULE.000': archive([b'', b'Waypoint\0'.ljust(77, b'\0'), b'', b'', b'', b'']),
                      'COMMUNIC.DAT': b'Ready\0'.ljust(80, b'\0')}
            for name, data in source.items():
                (root / name).write_bytes(data)
            translated = {'GAMEDAT/' + name: data for name, data in source.items()}
            translated['GAMEDAT/COMMUNIC.DAT'] = 'Prêt'.encode('cp437').ljust(80, b'\0')
            entries, issues, sources = importer.import_resources(
                root, translated, campaigns=('wc1',), decoder=french_text)
            self.assertEqual(entries[-1]['translation'], 'Prêt')
            self.assertEqual(set(sources), set(source))
            self.assertFalse(issues)
        entries, issues = [], []
        importer.scene_entries(entries, issues, 'wc1/test', scene(b'Ready $C'),
                               scene('Prêt, $C'.encode('cp437')), 1, decoder=french_text)
        self.assertEqual(entries[0]['translation'], 'Prêt, $C')
        self.assertNotIn('GPL', importer.po_document(entries, language='fr'))

    def test_scene_offsets_timing_and_script_separation(self):
        entries, issues = [], []
        importer.scene_entries(entries, issues, "test", scene(b"Hello $C"),
                               scene(b"Hola $C", duration=500, mouth=b"different"), 1)
        self.assertEqual(len(entries), 1)
        self.assertEqual(entries[0]["source"], "Hello $C")
        self.assertEqual(entries[0]["translation"], "Hola $C")
        self.assertFalse(issues)

    def test_debug_extraction_uses_original_encoding_without_spanish(self):
        entries, issues = [], []
        original = b"Hello \x8d $C"
        importer.scene_entries(entries, issues, "scene", scene(original), None, 1)
        importer.fixed_entries(entries, issues, "label", original + b"\0", None,
                               len(original) + 1, 0, len(original) + 1)
        self.assertEqual([e["source"] for e in entries], [original.decode("cp437")] * 2)

    def test_changed_speaker_rejected(self):
        with self.assertRaises(ValueError):
            importer.scene_entries([], [], "test", scene(b"Hello"), scene(b"Hola", speaker=2), 1)

    def test_condition_and_token_changes_require_review(self):
        entries, issues = [], []
        importer.scene_entries(entries, issues, "test", scene(b"Hello $C", condition=b"R1"),
                               scene(b"Hola $N", condition=b"R2"), 1)
        self.assertEqual(len(issues), 2)
        self.assertIn("#, fuzzy", importer.po_document(entries))
        self.assertNotIn("#, fuzzy", importer.po_document(entries, template=True))

    def test_fixed_record_requires_terminator(self):
        with self.assertRaises(ValueError):
            importer.fixed_entries([], [], "test", b"abcd", b"hola", 4, 0, 4)

    def test_unrecognized_archive_is_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "fake.zip"
            path.write_bytes(b"not the upstream archive")
            with self.assertRaisesRegex(ValueError, "SHA-256"):
                importer.read_spanish(path)

    @unittest.skipUnless(shutil.which("msgfmt"), "GNU gettext is not installed")
    def test_gettext_contexts_accents_and_fuzzy_fallback(self):
        entries, issues = [], []
        importer.add_entry(entries, issues, "radio", b"Yes", "Sí".encode("cp1252"))
        importer.add_entry(entries, issues, "menu", b"Yes", b"Aceptar")
        importer.add_entry(entries, issues, "review", b"Hello $C", b"Hola $N")
        with tempfile.TemporaryDirectory() as directory:
            po = Path(directory) / "es.po"
            mo = Path(directory) / "es.mo"
            po.write_text(importer.po_document(entries), encoding="utf-8")
            subprocess.run(["msgfmt", "--check", "-o", str(mo), str(po)], check=True, capture_output=True)
            translations = gettext.GNUTranslations(io.BytesIO(mo.read_bytes()))
        self.assertEqual(translations.pgettext("radio", importer.message_key("Yes")), "Sí")
        self.assertEqual(translations.pgettext("menu", importer.message_key("Yes")), "Aceptar")
        key = importer.message_key("Hello $C")
        self.assertEqual(translations.pgettext("review", key), key)


class CompileSpanishTests(unittest.TestCase):
    def setUp(self):
        import compileTranslations
        self.compiler = compileTranslations
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        (self.root / "glyphs.json").write_text('{"schema":1,"fonts":[]}')

    def catalog(self, text="¡Cañón!", source="Cannon!", extra="", font=1):
        import json
        (self.root / "test.po").write_text(
            f"#. Original font set: {font}\n" + extra +
            'msgctxt "test"\nmsgid ' + json.dumps(source) +
            '\nmsgstr ' + json.dumps(text) + '\n', encoding="utf-8")

    def test_encodes_font_one_and_preserves_utf8_source_file(self):
        self.catalog()
        data, count, skipped = self.compiler.build_catalog(self.root)
        self.assertIn("¡Cañón!".encode("cp1252") + b"\0", data)
        self.assertEqual((count, skipped), (1, 0))
        self.assertEqual(data[:4], b"WCT2")
        self.assertNotIn(b"Cannon!", data)

    def test_content_ids_preserve_format_requirements_without_prose(self):
        self.assertEqual(self.compiler.message_key("hello"), "wc1:a430d84680aabd0b")
        original = "\nHello $R $N: %X%d (Y/N)\n"
        key = self.compiler.message_key(original)
        self.assertTrue(key.startswith("\nwc1:"))
        self.assertTrue(key.endswith(" $R $N %X %d (Y/N)\n"))
        self.assertNotIn("Hello", key)
        self.assertEqual(self.compiler.message_key(key), key)
        document = 'msgid "Hello "\n"$C"\nmsgstr "Hola $C"\n'
        converted = self.compiler.keyed_po(document)
        self.assertNotIn("Hello", converted)
        self.assertEqual(self.compiler.keyed_po(converted), converted)

    def test_keyed_and_local_plaintext_inputs_compile_identically(self):
        self.catalog("Hola $C", "Hello $C")
        expected = self.compiler.build_catalog(self.root)
        path = self.root / "test.po"
        path.write_text(self.compiler.keyed_po(path.read_text()))
        self.assertEqual(self.compiler.build_catalog(self.root), expected)

    def test_published_game_catalogs_contain_only_content_ids(self):
        root = Path(__file__).resolve().parent.parent / "translations"
        for path in [*(root / "es").glob("*.po"), *(root / "templates").glob("*.pot")]:
            for entry, *_ in self.compiler.read_po(path):
                self.assertIsNotNone(self.compiler.MESSAGE_ID.fullmatch(entry["msgid"]), path)

    def test_rejects_source_strings_that_cannot_match_at_runtime(self):
        for source in ("Hello\0world", "x" * 32767):
            self.catalog(source=source)
            with self.assertRaisesRegex(ValueError, "Invalid source string"):
                self.compiler.build_catalog(self.root)

    def test_rejects_changed_campaign_tokens(self):
        self.catalog("Hola", "Hello $C")
        with self.assertRaisesRegex(ValueError, "placeholders"):
            self.compiler.build_catalog(self.root)

    def test_encoded_field_and_expanded_workspace_limits(self):
        self.catalog("Demasiado largo", extra="#. Original field capacity: 5 encoded bytes before NUL.\n")
        with self.assertRaisesRegex(ValueError, "exceeds 5"):
            self.compiler.build_catalog(self.root)
        self.catalog("$C " * 20, "$C " * 20)
        with self.assertRaisesRegex(ValueError, "workspace"):
            self.compiler.build_catalog(self.root)

    def test_duplicate_context_even_when_first_is_fuzzy(self):
        self.catalog(extra="#, fuzzy\n")
        (self.root / "second.po").write_text((self.root / "test.po").read_text())
        with self.assertRaisesRegex(ValueError, "Duplicate message key"):
            self.compiler.build_catalog(self.root)

    def test_fuzzy_entries_are_omitted(self):
        self.catalog()
        (self.root / "review.po").write_text('#, fuzzy\nmsgctxt "review"\nmsgid "Hello"\nmsgstr "Hola"\n')
        data, count, skipped = self.compiler.build_catalog(self.root)
        self.assertEqual((count, skipped), (1, 1))
        self.assertNotIn(b"Hola", data)

    def test_gettext_keys_allow_shared_context_and_default_context(self):
        (self.root / "ui.po").write_text(
            'msgid "Cannon"\nmsgstr "cañón"\n\n'
            'msgid "Ready"\nmsgstr "Listo"\n\n'
            'msgctxt "font0"\nmsgid "Cannon"\nmsgstr "cañón"\n', encoding="utf-8")
        data, count, skipped = self.compiler.build_catalog(self.root)
        self.assertEqual((count, skipped), (3, 0))
        self.assertIn(b"ca\xf1\xf3n\0", data)

    def test_shared_menu_and_cockpit_text_uses_same_accent_bytes(self):
        self.catalog("ñóú")
        glyphs = [{"character": code, "width": 1, "pixels": pixels}
                  for code, pixels in ((0xe0, "01"), (0xeb, "02"), (0xef, "03"))]
        (self.root / "glyphs.json").write_text(json.dumps({"schema": 1, "fonts": [
            {"font": 1, "height": 1, "ink": 1, "background": 0,
             "source_crc32": 0, "glyphs": glyphs}]}))
        data, _, _ = self.compiler.build_catalog(self.root)
        self.assertIn(b"\xf1\xf3\xfa\0", data)
        self.assertTrue(data.endswith(bytes((0xe0, 1, 1, 0xeb, 1, 2, 0xef, 1, 3,
                                            0xf1, 1, 1, 0xf3, 1, 2, 0xfa, 1, 3))))

    def test_ui_rejects_printf_type_changes(self):
        (self.root / "ui.po").write_text('msgid "Slot %d"\nmsgstr "Casilla %s"\n')
        with self.assertRaisesRegex(ValueError, "Format directives"):
            self.compiler.build_catalog(self.root)

    def test_ui_preserves_custom_formatter_directives(self):
        (self.root / "ui.po").write_text(
            'msgid "%X%YScore: %s0%P"\nmsgstr "%X%YPuntos: %s0%P"\n')
        data, count, skipped = self.compiler.build_catalog(self.root)
        self.assertEqual((count, skipped), (1, 0))
        self.assertIn(b"%X%YPuntos: %s0%P\0", data)
        (self.root / "ui.po").write_text(
            'msgid "%X%YScore: %s0%P"\nmsgstr "%X%YPuntos: %P%s0"\n')
        with self.assertRaisesRegex(ValueError, "Format directives"):
            self.compiler.build_catalog(self.root)

    def test_ui_key_labels_must_match_input_bindings(self):
        (self.root / "ui.po").write_text('msgid "Quit? (Y/N)"\nmsgstr "¿Salir? (S/N)"\n', encoding="utf-8")
        with self.assertRaisesRegex(ValueError, "Keyboard shortcut"):
            self.compiler.build_catalog(self.root)

    def test_title_sprite_alphabet_encoding(self):
        (self.root / 'ui.po').write_text('msgctxt "intro"\nmsgid "Design"\nmsgstr "Diseño, áéíóú"\n', encoding='utf-8')
        data, count, skipped = self.compiler.build_catalog(self.root)
        self.assertIn(b'Dise^o, [\\]_`\0', data)
        self.assertEqual((count, skipped), (1, 0))
        (self.root / 'ui.po').write_text('msgctxt "intro"\nmsgid "Design"\nmsgstr "¡Diseño!"\n', encoding='utf-8')
        with self.assertRaisesRegex(ValueError, 'title-font'):
            self.compiler.build_catalog(self.root)

    def test_japanese_ui_without_font_patches(self):
        (self.root / 'glyphs.json').unlink()
        (self.root / 'ui.po').write_text('msgid "Ready"\nmsgstr "準備完了"\n', encoding='utf-8')
        data, count, skipped = self.compiler.build_catalog(self.root, 'cp932')
        self.assertEqual((count, skipped), (1, 0))
        self.assertEqual(struct.unpack_from('<I', data, 8)[0], 0)
        self.assertIn('準備完了'.encode('cp932') + b'\0', data)
        (self.root / 'ui.po').write_text('msgid "Ready"\nmsgstr "' + 'あ' * 128 + '"\n', encoding='utf-8')
        with self.assertRaisesRegex(ValueError, 'workspace'):
            self.compiler.build_catalog(self.root, 'cp932')

    def test_french_uses_cp437_without_spanish_font_aliases(self):
        self.catalog('Prêt, déjà là')
        (self.root / 'glyphs.json').write_text(json.dumps({'schema': 1, 'fonts': [
            {'font': 1, 'height': 1, 'ink': 1, 'background': 0, 'source_crc32': 0,
             'glyphs': [{'character': 0xe0, 'width': 1, 'pixels': '01'}]}]}))
        data, count, _ = self.compiler.build_catalog(self.root, 'cp437')
        self.assertEqual(count, 1)
        self.assertIn('Prêt, déjà là'.encode('cp437') + b'\0', data)
        self.assertTrue(data.endswith(bytes((1, 0, 0xe0, 1, 1))))
        (self.root / 'ui.po').write_text('msgctxt "intro"\nmsgid "Ready"\nmsgstr "Prêt"\n', encoding='utf-8')
        with self.assertRaisesRegex(ValueError, 'title-font'):
            self.compiler.build_catalog(self.root, 'cp437')

    def test_french_catalog_scope_and_opaque_source_ids(self):
        root = Path(__file__).resolve().parent.parent / 'translations/fr'
        data, count, skipped = self.compiler.build_catalog(root, 'cp437')
        self.assertGreater(count, 6500)
        self.assertLess(skipped, 20)
        for path in root.glob('*.po'):
            for entry, _, _, _ in self.compiler.read_po(path):
                self.assertTrue(self.compiler.MESSAGE_ID.fullmatch(entry['msgid']))
        for name, expected in (('sm1.po', 1435), ('sm2.po', 1572), ('cinematics.po', 86)):
            entries = self.compiler.read_po(root / name)
            self.assertEqual(len(entries), expected)
            for entry, fuzzy, _, _ in entries:
                self.assertTrue(entry['msgstr'], entry['msgctxt'])
                self.assertFalse(fuzzy, entry['msgctxt'])

    def test_artwork_splices_are_bounded_and_size_checked(self):
        packet = {'file': 'TITLE.VGA', 'section': 1, 'source_size': 8,
                  'source_crc32': 123, 'target_size': 9, 'target_crc32': 456,
                  'edits': [[3, 2, '010203']]}
        path = self.root / 'artwork.json'
        path.write_text(json.dumps({'schema': 1, 'packets': [packet]}))
        self.assertTrue(self.compiler.compile_artwork(path).startswith(b'WCA1'))
        for edits, size in (([[7, 2, '00']], 7), ([[3, 2, '00'], [4, 1, '00']], 7), ([[3, 2, '00']], 12)):
            packet.update(edits=edits, target_size=size)
            path.write_text(json.dumps({'schema': 1, 'packets': [packet]}))
            with self.assertRaises(ValueError):
                self.compiler.compile_artwork(path)

    def test_all_ui_and_cinematic_entries_are_translated(self):
        root = Path(__file__).resolve().parent.parent / 'translations/es'
        for name in ('ui.po', 'cinematics.po'):
            for entry, fuzzy, _, _ in self.compiler.read_po(root / name):
                self.assertTrue(entry['msgstr'], entry['msgid'])
                self.assertFalse(fuzzy, entry['msgid'])

    @unittest.skipUnless(shutil.which('msgfmt'), 'GNU gettext is optional')
    def test_ui_catalog_passes_gettext_format_validation(self):
        for language in ('es', 'ja', 'fr'):
            path = Path(__file__).resolve().parent.parent / 'translations' / language / 'ui.po'
            subprocess.run(['msgfmt', '--check', '-o', str(self.root / 'ui.mo'), str(path)], check=True, capture_output=True)

    @unittest.skipUnless(shutil.which("xgettext"), "GNU gettext is optional")
    def test_ui_template_matches_decorated_source(self):
        import updateUiTranslations
        root = Path(__file__).resolve().parent.parent
        self.assertEqual(updateUiTranslations.extract(root),
                         (root / "translations/templates/ui.pot").read_text())


if __name__ == "__main__":
    unittest.main()
