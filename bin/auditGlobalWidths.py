#!/usr/bin/env python3
"""Report scalar globals whose declared width the original never uses.

The original always accesses a global at its real width, so a global declared
`int` that the shipped code only ever reads with `mov ax, word ptr [addr]` is
declared two bytes too wide.  Every call site of such a global emits the wrong
operand size, so one declaration fix moves many functions at once.

Reads include/globals.h for `extern <type> <name>_00<addr>;` declarations and
code-full/*.disassembled.txt for the widths the original actually uses.

    python3 bin/auditGlobalWidths.py
"""
import collections
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HEADER = os.path.join(ROOT, 'include', 'globals.h')
CODE = os.path.join(ROOT, 'code-full')

SCALAR_SIZES = {
    'char': 1, 'signed char': 1, 'unsigned char': 1,
    'short': 2, 'signed short': 2, 'unsigned short': 2,
    'int': 4, 'signed int': 4, 'unsigned int': 4,
    'long': 4, 'unsigned long': 4, 'float': 4,
}
OPERAND_SIZES = {'byte': 1, 'word': 2, 'dword': 4, 'qword': 8}

DECL = re.compile(
    r'extern\s+(.+?)\s+(\**)((?:g_\w+|DAT)_00([0-9a-fA-F]{6}))\s*;\s*$')
ACCESS = re.compile(r'\b(byte|word|dword|qword) ptr \[0x([0-9a-f]{6})\]')


def declared_scalars():
    scalars = {}
    for line in open(HEADER):
        match = DECL.match(line.strip())
        if not match:
            continue
        base, stars, name, address = match.groups()
        if stars:
            size = 4
        else:
            size = SCALAR_SIZES.get(
                base.replace('const ', '').replace('volatile ', '').strip())
        if size is None:
            continue
        scalars[int(address, 16)] = (name, base + stars, size)
    return scalars


def original_widths():
    widths = collections.defaultdict(collections.Counter)
    for name in os.listdir(CODE):
        if not name.endswith('.disassembled.txt'):
            continue
        for line in open(os.path.join(CODE, name)):
            for match in ACCESS.finditer(line.lower()):
                widths[int(match.group(2), 16)][match.group(1)] += 1
    return widths


def main():
    scalars = declared_scalars()
    widths = original_widths()
    suspect = []
    for address, (name, base, size) in sorted(scalars.items()):
        used = widths.get(address)
        if not used:
            continue
        if size not in set(OPERAND_SIZES[kind] for kind in used):
            suspect.append((address, name, base, size, dict(used),
                            sum(used.values())))
    suspect.sort(key=lambda entry: -entry[5])
    print('%d scalar global(s) declared at a width the original never uses'
          % len(suspect))
    for address, name, base, size, used, total in suspect:
        print('  0x%06x %-52s decl=%-16s size=%d orig=%s'
              % (address, name, base, size, used))
    return 1 if suspect else 0


if __name__ == '__main__':
    sys.exit(main())
