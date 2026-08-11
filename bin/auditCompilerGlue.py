#!/usr/bin/env python3
"""Reject hand-written assembly used only to force compiler glue.

Bare tail calls and calling-convention adapters must be expressed in C and
emitted by the compiler. Inline assembly remains valid for routines whose
original implementation was itself hand-written assembly.
"""
import os
import re
import sys


ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_ROOTS = (os.path.join(ROOT, 'src'), os.path.join(ROOT, 'include'))
SOURCE_SUFFIXES = ('.c', '.cpp', '.h', '.hpp', '.inc')

ASM_BLOCK = re.compile(r'\b__asm\s*\{(?P<body>[^{}]*)\}', re.DOTALL)
JUMP_GLUE = re.compile(r'jmp\s+[^\s;]+\s*;?\s*\Z', re.IGNORECASE)
CALL_GLUE = re.compile(
    r'call\s+[^\s;]+\s*;?\s*ret(?:\s+(?:0x)?[0-9a-f]+)?\s*;?\s*\Z',
    re.IGNORECASE,
)


def strip_comments(text):
    def preserve_lines(match):
        return re.sub(r'[^\n]', ' ', match.group(0))

    text = re.sub(r'/\*.*?\*/', preserve_lines, text, flags=re.DOTALL)
    return re.sub(r'//[^\n]*', preserve_lines, text)


def source_files():
    for source_root in SOURCE_ROOTS:
        for directory, _subdirectories, filenames in os.walk(source_root):
            for filename in sorted(filenames):
                if filename.endswith(SOURCE_SUFFIXES):
                    yield os.path.join(directory, filename)


def main():
    violations = []

    for path in source_files():
        source = open(path).read()
        stripped = strip_comments(source)
        for match in ASM_BLOCK.finditer(stripped):
            body = match.group('body').strip()
            if not (JUMP_GLUE.fullmatch(body) or CALL_GLUE.fullmatch(body)):
                continue
            line = source.count('\n', 0, match.start()) + 1
            relative_path = os.path.relpath(path, ROOT)
            instruction = ' '.join(body.split())
            violations.append((relative_path, line, instruction))

    if not violations:
        print('no manually forced compiler glue found')
        return 0

    for path, line, instruction in violations:
        print(f'{path}:{line}: manually forced compiler glue: {instruction}')
    print(
        f'\n{len(violations)} violation(s): express the forwarding operation in C '
        'and let the compiler emit the glue'
    )
    return 1


if __name__ == '__main__':
    sys.exit(main())
