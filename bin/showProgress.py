#!/usr/bin/env python3
"""Reimplementation progress summary.

Counts functions carrying a `/* Function start: 0x... */` header in src/ and
compares that against the developer-function inventory in src/map.
"""
import os
import re
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'src')
MAPFILE = os.path.join(ROOT, 'src', 'map')

HEADER = re.compile(r'/\*\s*Function start:\s*0x([0-9A-Fa-f]+)\s*\*/')
TODO = re.compile(r'/\*\s*TODO')

IX_LO, CRT_LO = 0x00442750, 0x00449100


def region(addr):
    if addr >= CRT_LO:
        return 'crt'
    return 'ix' if addr >= IX_LO else 'game'


def main():
    if not os.path.isdir(SRC):
        sys.exit('error: src/ not found')

    total = defaultdict(int)
    with open(MAPFILE) as fh:
        for line in fh:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            addr = int(line.split()[0], 16)
            total[region(addr)] += 1

    done = defaultdict(set)
    stubbed = defaultdict(set)
    per_file = {}
    for dirpath, _dirs, files in os.walk(SRC):
        for name in sorted(files):
            if not name.endswith(('.c', '.cpp')):
                continue
            path = os.path.join(dirpath, name)
            with open(path, errors='replace') as fh:
                lines = fh.readlines()
            impl = stub = 0
            for i, line in enumerate(lines):
                m = HEADER.search(line)
                if not m:
                    continue
                addr = int(m.group(1), 16)
                # a header immediately followed by a TODO comment is a stub
                nxt = lines[i + 1] if i + 1 < len(lines) else ''
                if TODO.search(line) or TODO.search(nxt):
                    stubbed[region(addr)].add(addr)
                    stub += 1
                else:
                    done[region(addr)].add(addr)
                    impl += 1
            rel = os.path.relpath(path, ROOT)
            if impl or stub:
                per_file[rel] = (impl, stub)

    print(f"{'region':<8} {'implemented':>12} {'stubbed':>9} {'total':>7}  progress")
    grand_done = grand_total = 0
    for r in ('game', 'ix'):
        d, s, t = len(done[r]), len(stubbed[r]), total[r]
        grand_done += d
        grand_total += t
        pct = (d / t * 100) if t else 0.0
        bar = '#' * int(pct / 2.5)
        print(f"{r:<8} {d:>12} {s:>9} {t:>7}  {pct:5.1f}% {bar}")
    pct = (grand_done / grand_total * 100) if grand_total else 0.0
    print(f"{'TOTAL':<8} {grand_done:>12} {'':>9} {grand_total:>7}  {pct:5.1f}%")

    if per_file:
        print('\nper file (implemented / stubbed):')
        for rel in sorted(per_file):
            impl, stub = per_file[rel]
            print(f"  {rel:<44} {impl:>4} / {stub:<4}")


if __name__ == '__main__':
    main()
