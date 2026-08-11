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

HEADER = re.compile(r'/\*\s*Function start:\s*0x([0-9A-Fa-f]+)\b')
TODO = re.compile(r'/\*\s*TODO')

IX_LO, CRT_LO = 0x00442750, 0x00449100

# Compiler/linker-only entries have no source body to reconstruct.  Counting
# them as resolved here avoids encouraging a forbidden hand-written jump thunk.
AUTO_COMPLETE = {
    0x0040CB20: "linker JMP thunk to Input at 0x00402AC0",
    0x00434D10: "compiler JMP thunk to CRT rand at 0x00449940",
}


def region(addr):
    if addr >= CRT_LO:
        return 'crt'
    return 'ix' if addr >= IX_LO else 'game'


def main():
    if not os.path.isdir(SRC):
        sys.exit('error: src/ not found')

    total = defaultdict(int)
    inventory = set()
    with open(MAPFILE) as fh:
        for line in fh:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            addr = int(line.split()[0], 16)
            inventory.add(addr)
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

    auto_complete = defaultdict(set)
    for addr in AUTO_COMPLETE:
        if addr in inventory and addr not in done[region(addr)]:
            auto_complete[region(addr)].add(addr)

    print(f"{'region':<8} {'implemented':>12} {'auto':>6} "
          f"{'stubbed':>9} {'total':>7}  progress")
    grand_done = grand_auto = grand_total = 0
    for r in ('game', 'ix'):
        d, a, s, t = (len(done[r]), len(auto_complete[r]),
                      len(stubbed[r]), total[r])
        grand_done += d
        grand_auto += a
        grand_total += t
        pct = ((d + a) / t * 100) if t else 0.0
        bar = '#' * int(pct / 2.5)
        print(f"{r:<8} {d:>12} {a:>6} {s:>9} {t:>7}  "
              f"{pct:5.1f}% {bar}")
    pct = (((grand_done + grand_auto) / grand_total * 100)
           if grand_total else 0.0)
    print(f"{'TOTAL':<8} {grand_done:>12} {grand_auto:>6} {'':>9} "
          f"{grand_total:>7}  {pct:5.1f}%")

    if grand_auto:
        print('\nauto-completed compiler/linker functions:')
        for addr in sorted(AUTO_COMPLETE):
            if addr in auto_complete[region(addr)]:
                print(f"  0x{addr:08X}  {AUTO_COMPLETE[addr]}")

    if per_file:
        print('\nper file (implemented / stubbed):')
        for rel in sorted(per_file):
            impl, stub = per_file[rel]
            print(f"  {rel:<44} {impl:>4} / {stub:<4}")


if __name__ == '__main__':
    main()
