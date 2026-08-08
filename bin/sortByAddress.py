#!/usr/bin/env python3
"""Sort reimplemented functions by original address within each source file.

MSVC emits functions in source order, so keeping each file address-sorted is what
exposes incorrect compilation-unit ownership. This rewrites files in place,
reordering the blocks delimited by `/* Function start: 0x... */` headers.

Everything before the first header (includes, struct definitions, file comment)
is preserved as the file preamble and never moved.

    bin/sortByAddress.py                # check only, report files out of order
    bin/sortByAddress.py --write        # rewrite files in place
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'src')
HEADER = re.compile(r'^\s*/\*\s*Function start:\s*0x([0-9A-Fa-f]+)\s*\*/')


def split_blocks(lines):
    """Return (preamble, [(addr, block_lines), ...])."""
    starts = [i for i, l in enumerate(lines) if HEADER.match(l)]
    if not starts:
        return lines, []
    preamble = lines[:starts[0]]
    blocks = []
    for n, i in enumerate(starts):
        end = starts[n + 1] if n + 1 < len(starts) else len(lines)
        addr = int(HEADER.match(lines[i]).group(1), 16)
        blocks.append((addr, lines[i:end]))
    return preamble, blocks


def main():
    write = '--write' in sys.argv
    unsorted_files = []
    for dirpath, _dirs, files in os.walk(SRC):
        for name in sorted(files):
            if not name.endswith(('.c', '.cpp')):
                continue
            path = os.path.join(dirpath, name)
            with open(path, errors='replace') as fh:
                lines = fh.readlines()
            preamble, blocks = split_blocks(lines)
            if len(blocks) < 2:
                continue
            addrs = [a for a, _ in blocks]
            if addrs == sorted(addrs):
                continue
            rel = os.path.relpath(path, ROOT)
            unsorted_files.append(rel)
            if write:
                blocks.sort(key=lambda b: b[0])
                with open(path, 'w') as fh:
                    fh.writelines(preamble)
                    for _addr, block in blocks:
                        fh.writelines(block)

    if not unsorted_files:
        print('all source files are address-sorted')
        return 0
    verb = 'reordered' if write else 'OUT OF ORDER (run with --write to fix)'
    for rel in unsorted_files:
        print(f'{verb}: {rel}')
    return 0 if write else 1


if __name__ == '__main__':
    sys.exit(main())
