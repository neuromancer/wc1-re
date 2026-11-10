#!/usr/bin/env python3
"""Regression checks for file-scope storage between reconstructed functions."""

import tempfile
import unittest
from pathlib import Path

from collectGlobalDefinitions import definition_block, header_constants


class DefinitionBlockTests(unittest.TestCase):
    def test_native_constants_exclude_sdl_branches(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "constants.h"
            path.write_text('''enum ObjectType {
    OBJECT_TYPES = 58,
#ifdef SDL_PORT
    OBJECT_COUNT = 83,
#else
    OBJECT_COUNT = OBJECT_TYPES,
#endif
};
#ifdef SDL_PORT
#define COCKPIT_COUNT 11
#else
#define COCKPIT_COUNT 5
#endif
''', encoding="utf-8")
            constants = header_constants([path])
        self.assertEqual(constants["OBJECT_COUNT"], 58)
        self.assertEqual(constants["COCKPIT_COUNT"], 5)

    def collect(self, source: str) -> str:
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "owner.c"
            path.write_text(source, encoding="utf-8")
            return definition_block(path)

    def test_storage_between_functions_preserves_initializers(self):
        block = self.collect('''/* Non-ASCII prefix: © */
#include "wc1.h"
short first = 1; /* 0x00465000 */
/* Function start: 0x00401000 */
void First(void)
{
    static short localCounter = 2; /* 0x00465004 */
    const char *text = "} static short misleading; {";
    localCounter++;
}
static short formationCount = 0; /* 0x00465008 */
/* 0x00465010 */
short coordinates[2][2] = {
    { 1, 2 }, { 3, 4 }
};
/* Function start: 0x00401010 */
void Second(void)
{
    short localValue = 3; /* 0x00465020 */
    formationCount += localValue;
}
''')
        self.assertIn("short first = 1; /* 0x00465000 */", block)
        self.assertIn("static short formationCount = 0; /* 0x00465008 */", block)
        self.assertIn("short coordinates[2][2] = {\n    { 1, 2 }, { 3, 4 }\n};", block)
        self.assertIn("/* 0x00465010 */", block)
        for excluded in ("localCounter", "localValue", "misleading", "void First", "void Second"):
            self.assertNotIn(excluded, block)

    def test_unused_uninitialized_static_still_has_no_audit_symbol(self):
        block = self.collect('''#include "wc1.h"
/* Function start: 0x00401000 */
void First(void) {}
static short unused; /* 0x00465000 */
static short used; /* 0x00465004 */
/* Function start: 0x00401010 */
void Second(void) { used++; }
''')
        self.assertNotIn("unused", block)
        self.assertIn("static short used; /* 0x00465004 */", block)

    def test_data_only_owner(self):
        declaration = 'const char *name = "WINGLEADER"; /* 0x00465000 */\n'
        self.assertEqual(self.collect('#include "wc1.h"\n' + declaration), declaration)


if __name__ == "__main__":
    unittest.main()
