#!/usr/bin/env python3
"""Recover the developers' own function names from the shipped debug build.

WC1 was released as a debug build, so its diagnostic printers are still live and
still carry their format strings.  Many of those formats are the developer's own
name for the routine doing the printing:

    0x0042E320  ->  "FadeMusic\\n"
    0x0042E350  ->  "StopMusic "                 (already evidence-named: a control)
    0x00432970  ->  "DIBslamReal   secondary->Lock"

That makes every such call site a primary-source naming fact, which beats any
operational label.  This script finds them by scanning .text for CALL sites and
taking the last string literal pushed before each call -- the argument closest to
the call, i.e. the format string -- never looking back past the start of the
enclosing function.

    bin/nameOracle.py                  # the six developer-facing printers
    bin/nameOracle.py --top            # rank every callee by string-arg sites
    bin/nameOracle.py 0x432140 ...     # a specific printer

Cross-check before believing a result: a routine that prints another routine's
name (a wrapper logging its callee) will be misattributed.  The controls above
are there so a change in the extraction can be caught.
"""
import argparse
import bisect
import csv
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, 'data', 'full', 'WC1.ORI.EXE')
CSV = os.path.join(ROOT, '..', 'wc1_function_evidence.csv')

# The printers whose format strings name their caller, found with --top.
DEFAULT_LOGGERS = [
    0x00403DB0,   # SoundDebugPrintf   -- music/SFX channel
    0x00425BB0,   # SystemDebugPrintf  -- "[SYSTEM] : ..." channel
    0x00413C40,   # on-screen text printer (%X%Y cursor codes)
    0x00413C70,   # on-screen text printer, paged variant (%P)
    0x00432140,   # DIBerror           -- names every DIB* routine
    0x00428FA0,   # ShowOnScreenMessage
]


def load_image():
    raw = open(EXE, 'rb').read()
    pe = struct.unpack_from('<I', raw, 0x3c)[0]
    nsec = struct.unpack_from('<H', raw, pe + 6)[0]
    optsz = struct.unpack_from('<H', raw, pe + 20)[0]
    imgbase = struct.unpack_from('<I', raw, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + optsz + i * 40
        vsz, va, rsz, ro = struct.unpack_from('<IIII', raw, o + 8)
        secs.append((raw[o:o + 8].rstrip(b'\0').decode('latin1'),
                     imgbase + va, max(vsz, rsz), ro, rsz))
    return raw, secs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('loggers', nargs='*', help='printer addresses (hex)')
    ap.add_argument('--top', action='store_true',
                    help='rank callees by how many string-argument sites they have')
    args = ap.parse_args()

    raw, secs = load_image()

    def off(va):
        for _, base, vsz, ro, rsz in secs:
            if base <= va < base + vsz:
                d = va - base
                return ro + d if d < rsz else None
        return None

    def cstr(va, maxlen=200):
        o = off(va)
        if o is None:
            return None
        e = raw.find(b'\0', o, o + maxlen)
        if e < 0 or e == o:
            return None
        s = raw[o:e]
        if not all(32 <= c < 127 or c in (9, 10, 13) for c in s):
            return None
        return s.decode('latin1')

    funcs = sorted((int(r['address'], 16), r['name'])
                   for r in csv.DictReader(open(CSV)))
    starts = [f[0] for f in funcs]

    def enclosing(va):
        i = bisect.bisect_right(starts, va) - 1
        return funcs[i] if i >= 0 else (0, '?')

    text = next(s for s in secs if s[0] == '.text')
    tbase, toff, tsize = text[1], text[3], text[2]

    calls = {}
    i, end = 0, min(tsize, len(raw) - toff)
    while i < end - 5:
        if raw[toff + i] != 0xE8:
            i += 1
            continue
        site = tbase + i
        target = site + 5 + struct.unpack_from('<i', raw, toff + i + 1)[0]
        fstart = enclosing(site)[0]
        last, j = None, max(0, i - 64)
        if tbase + j < fstart:
            j = fstart - tbase
        while j < i:
            if raw[toff + j] == 0x68 and j + 5 <= i:
                s = cstr(struct.unpack_from('<I', raw, toff + j + 1)[0])
                if s:
                    last = s
                j += 5
            else:
                j += 1
        if last:
            calls.setdefault(target, []).append((site, last))
        i += 5

    if args.top:
        for target, lst in sorted(calls.items(), key=lambda kv: -len(kv[1]))[:25]:
            print(f'{len(lst):4d} sites  0x{target:08X}  {enclosing(target)[1]}')
        return 0

    for logger in [int(a, 16) for a in args.loggers] or DEFAULT_LOGGERS:
        print(f'\n##### printer 0x{logger:08X}  {enclosing(logger)[1]}')
        byfn = {}
        for site, s in sorted(calls.get(logger, [])):
            byfn.setdefault(enclosing(site), []).append(s)
        for (fa, fn), ss in sorted(byfn.items()):
            uniq = list(dict.fromkeys(ss))[:4]
            print(f'  {fa:08X} {fn:<32} ' + ' | '.join(repr(u) for u in uniq))
    return 0


if __name__ == '__main__':
    sys.exit(main())
