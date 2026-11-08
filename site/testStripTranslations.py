#!/usr/bin/env python3
"""Keep source-browser stripping limited to translation decorations."""

import unittest

from stripTranslations import strip_translations


class StripTranslationsTests(unittest.TestCase):
    def test_all_markers_and_adjacent_literals(self):
        self.assertEqual(
            strip_translations('_("One"); N_("Two"); C_("font0", "Three");\n'
                               'NC_("intro", "First\\n"\n    "Second");'),
            '"One"; "Two"; "Three";\n"First\\n"\n    "Second";',
        )

    def test_expressions_nested_calls_and_context_commas(self):
        self.assertEqual(
            strip_translations('C_("a,b)", _(names[lookup(x, y)]));\n'
                               '_(choose(\')\', "a, (b)", NC_("c", "word")));'),
            'names[lookup(x, y)];\nchoose(\')\', "a, (b)", "word");',
        )

    def test_comments_literals_and_unrelated_macros(self):
        unchanged = ('/* _("comment") */ // C_("x", "comment")\n'
                     '"_(\\"literal\\")"; PREFIX_("text");\n'
                     '#define OTHER(x) (x)\n')
        self.assertEqual(strip_translations(unchanged), unchanged)
        self.assertEqual(strip_translations('_ /* decoration */ ("text")'), '"text"')

    def test_reference_definitions_and_explanatory_comment(self):
        self.assertEqual(strip_translations(
            '/* Gettext-style UI markers.\n * Modern-port explanation. */\n'
            '#define _(text) text\n#define N_(text) text\n'
            '#define C_(context, text) text\n#define NC_(context, text) \\\ntext\n'
            'N_("Original");\n'), '\n"Original";\n')

    def test_malformed_markers_fail_the_build(self):
        for source in ('_("unfinished"', 'C_("context")', '_(a, b)', '_()'):
            with self.subTest(source=source), self.assertRaises(ValueError):
                strip_translations(source)

    def test_stripping_is_idempotent(self):
        source = 'N_("a"); C_("font0", labels[index]);'
        stripped = strip_translations(source)
        self.assertEqual(strip_translations(stripped), stripped)


if __name__ == "__main__":
    unittest.main()
