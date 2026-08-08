#!/usr/bin/env python3
"""Search compiler-flag combinations, scored by binary-comp.

The original's flags are not documented anywhere, so they get recovered by
building the project many ways and asking binary-comp which build matches best.
Scoring is always binary-comp's `report`; this script only drives the build.

    bin/sweepFlags.py                     # sweep CFLAGS_CORE
    bin/sweepFlags.py --ix                # sweep CFLAGS_IX instead
    bin/sweepFlags.py --filter mathutil   # restrict the report to one file
"""
import argparse
import re
import subprocess
import sys
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

CORE_CANDIDATES = [
    '/Od',
    '/O1', '/O2', '/Ox',
    '/Og', '/Og /Os', '/Og /Ot',
    '/Og /Oi /Ot /Oy /Ob1',
    '/Og /Oi /Ot /Oy /Ob1 /Gs',
    '/Og /Oi /Os /Oy /Ob1',
    '/O2 /Oy', '/O1 /Oy', '/Ox /Oy',
    '/Oa /Og /Oi /Ot /Oy /Ob1',
    '/Ow /Og /Oi /Ot /Oy /Ob1',
]
IX_CANDIDATES = ['/Od', '/Od /Gs', '/Og', '/O1', '/O2', '/Zi /Od']

SUMMARY = re.compile(r'Average similarity:\s*([\d.]+)%')
HUNDRED = re.compile(r'^\s*100%:\s*(\d+)', re.M)
TOTAL = re.compile(r'^\s*Total compared:\s*(\d+)', re.M)


def run(flags, var, filt):
    subprocess.run(['make', 'clean'], cwd=ROOT, capture_output=True)
    build = subprocess.run(['make', 'WC1.EXE', f'{var}={flags}'],
                           cwd=ROOT, capture_output=True, text=True)
    if build.returncode != 0:
        return None, 'BUILD FAILED'
    cmd = ['make', 'report', f'{var}={flags}']
    if filt:
        cmd.append(f'FILTER={filt}')
    rep = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    out = rep.stdout + rep.stderr
    m = SUMMARY.search(out)
    if not m:
        return None, 'NO REPORT'
    h = HUNDRED.search(out)
    t = TOTAL.search(out)
    return float(m.group(1)), f"{h.group(1) if h else '?'}/{t.group(1) if t else '?'} exact"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--ix', action='store_true', help='sweep CFLAGS_IX instead of CFLAGS_CORE')
    ap.add_argument('--filter', default=None, help='restrict the report to matching files')
    args = ap.parse_args()

    var = 'CFLAGS_IX' if args.ix else 'CFLAGS_CORE'
    base = '/nologo /c /MTd /I include'
    cands = IX_CANDIDATES if args.ix else CORE_CANDIDATES

    results = []
    for opt in cands:
        score, note = run(f'{base} {opt}', var, args.filter)
        results.append((score if score is not None else -1.0, opt, note))
        print(f'  {opt:<34} {("%.2f%%" % score) if score is not None else note}', flush=True)

    results.sort(key=lambda r: -r[0])
    print(f'\n{"mean":>9}  {"flags":<34} detail')
    for score, opt, note in results:
        s = f'{score:8.2f}%' if score >= 0 else '   n/a  '
        print(f'{s}  {opt:<34} {note}')
    print(f'\nbest {var}: {results[0][1]}')
    subprocess.run(['make', 'clean'], cwd=ROOT, capture_output=True)


if __name__ == '__main__':
    main()
