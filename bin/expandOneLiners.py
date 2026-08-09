#!/usr/bin/env python3
"""Expand one-line function definitions onto separate lines.

AGENTS.md forbids `unsigned short f(void) { return 0; }`: the reconstruction is
read side by side with the disassembly, and a body folded onto the signature
line hides how many statements the original actually has.  This rewrites

    void DIBslam(void) { DAT_00486518 = 1; }

as

    void DIBslam(void)
    {
        DAT_00486518 = 1;
    }

Statements are split on top-level `;` only, so `for (i = 0; i < n; i++)` and
string literals containing a semicolon survive.  Nested braces are left alone --
a body with its own block is emitted as one statement and reindented by hand.

    bin/expandOneLiners.py [--check]
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# A definition, not a call or a declaration: ends in `}` with a body between.
ONE_LINER = re.compile(r'^(?P<indent>[ \t]*)(?P<sig>[A-Za-z_][^;{}]*\))[ \t]*\{'
                       r'(?P<body>.*)\}[ \t]*$')
KEYWORDS = ('if', 'for', 'while', 'switch', 'do', 'else', 'return', '__asm',
            'struct', 'union', 'enum', 'typedef')


def split_statements(body):
    """Split on `;` at brace/paren depth 0, outside strings and chars."""
    out, cur, depth, i = [], '', 0, 0
    while i < len(body):
        c = body[i]
        if c in '"\'':
            quote, cur, i = c, cur + c, i + 1
            while i < len(body):
                cur += body[i]
                if body[i] == '\\':
                    i += 1
                    if i < len(body):
                        cur += body[i]
                elif body[i] == quote:
                    i += 1
                    break
                i += 1
            continue
        if c in '({[':
            depth += 1
        elif c in ')}]':
            depth -= 1
        if c == ';' and depth == 0:
            out.append((cur + ';').strip())
            cur = ''
        else:
            cur += c
        i += 1
    if cur.strip():
        out.append(cur.strip())
    return [s for s in out if s and s != ';']


def main():
    check = '--check' in sys.argv
    files = (sorted(glob.glob(os.path.join(ROOT, 'src', '*.c')))
             + sorted(glob.glob(os.path.join(ROOT, 'src', 'ix', '*.cpp'))))
    total = 0
    for path in files:
        lines = open(path).read().split('\n')
        out, changed = [], False
        for line in lines:
            m = ONE_LINER.match(line)
            sig = m.group('sig').strip() if m else ''
            first = sig.split('(')[0].split()[-1].lstrip('*') if sig and '(' in sig else ''
            if not m or first in KEYWORDS or sig.split()[0] in KEYWORDS or '{' in m.group('body'):
                out.append(line)
                continue
            indent = m.group('indent')
            stmts = split_statements(m.group('body'))
            out.append(f'{indent}{sig}')
            out.append(indent + '{')
            for s in stmts:
                out.append(f'{indent}    {s}')
            out.append(indent + '}')
            changed = True
            total += 1
        if changed and not check:
            open(path, 'w').write('\n'.join(out))
    if check:
        print(f'{total} one-line function definition(s)')
        return 1 if total else 0
    print(f'expanded {total} one-line function definition(s)')
    return 0


if __name__ == '__main__':
    sys.exit(main())
