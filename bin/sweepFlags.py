#!/usr/bin/env python3
"""Search compiler flag combinations for the best match on a set of functions.

Calibration tool: the original's flags are not documented anywhere, so we
recover them by compiling one representative file many ways and scoring each
result against the original disassembly.

    bin/sweepFlags.py src/mathutil.c MinShort MaxShort
"""
import importlib.util
import itertools
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
spec = importlib.util.spec_from_file_location('cmpfunc', os.path.join(ROOT, 'bin', 'cmpFunc.py'))
cmpfunc = importlib.util.module_from_spec(spec)
spec.loader.exec_module(cmpfunc)

BASE = ['/nologo', '/c', '/MTd', '/I', 'include']
TMP = os.path.join(ROOT, 'out', 'sweep')

# Candidate optimizer sets. MSVC 4.2 presets plus hand-built combinations.
CANDIDATES = [
    ['/Od'],
    ['/O1'], ['/O2'],
    ['/Ox'],
    ['/Og'], ['/Og', '/Os'], ['/Og', '/Ot'],
    ['/Og', '/Oi', '/Ot', '/Oy', '/Ob1'],
    ['/Og', '/Oi', '/Os', '/Oy', '/Ob1'],
    ['/Og', '/Oi', '/Ot', '/Oy', '/Ob1', '/Gs'],
    ['/Og', '/Oi', '/Os', '/Oy', '/Ob1', '/Gs'],
    ['/O2', '/Oy'], ['/O1', '/Oy'],
    ['/Ox', '/Oy'],
    ['/Og', '/Oy'], ['/Os', '/Oy'], ['/Ot', '/Oy'],
    ['/Oa', '/Og', '/Oi', '/Ot', '/Oy', '/Ob1'],
    ['/Ow', '/Og', '/Oi', '/Ot', '/Oy', '/Ob1'],
]


def build(src, flags):
    os.makedirs(TMP, exist_ok=True)
    stem = os.path.splitext(os.path.basename(src))[0]
    asm = os.path.join(TMP, stem + '.asm')
    for p in (asm, os.path.join(TMP, stem + '.obj')):
        if os.path.exists(p):
            os.remove(p)
    cmd = ['./wibo', 'compilers/msvc420/bin/CL.EXE'] + BASE + flags + [
        src, f'/Fo{TMP}/{stem}.obj', f'/Fa{asm}']
    env = dict(os.environ, INCLUDE=r'compilers\msvc420\include')
    r = subprocess.run(cmd, cwd=ROOT, env=env, capture_output=True, text=True)
    return (asm if os.path.exists(asm) else None), r


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    src, names = sys.argv[1], sys.argv[2:]

    saved_out = cmpfunc.OUT
    results = []
    for flags in CANDIDATES:
        asm, r = build(src, flags)
        if asm is None:
            results.append((0.0, flags, 'BUILD FAILED: ' + (r.stdout or r.stderr)[:60]))
            continue
        cmpfunc.OUT = TMP
        scores = []
        for n in names:
            orig = cmpfunc.load_original(n)
            rebuilt, _ = cmpfunc.load_rebuilt(n)
            if orig is None or rebuilt is None:
                continue
            import difflib
            a = [cmpfunc.norm(x) for x in orig]
            b = [cmpfunc.norm(x) for x in rebuilt]
            scores.append(difflib.SequenceMatcher(None, a, b).ratio() * 100)
        cmpfunc.OUT = saved_out
        mean = sum(scores) / len(scores) if scores else 0.0
        detail = ' '.join(f'{n}={s:.0f}%' for n, s in zip(names, scores))
        results.append((mean, flags, detail))

    results.sort(key=lambda x: -x[0])
    print(f'{"mean":>7}  {"flags":<44} detail')
    for mean, flags, detail in results:
        print(f'{mean:6.1f}%  {" ".join(flags):<44} {detail}')
    print(f'\nbest: {" ".join(results[0][1])}')


if __name__ == '__main__':
    main()
