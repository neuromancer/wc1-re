#!/usr/bin/env python3
"""Measure CL 4.20 symbol-counter indices without probe-offset sweeps.

Register and operand-order ties in the optimized core depend on the symbol
counter (mod 32 and mod 257, see docs/PATTERNS.md).  The counter is visible:
every `$Lnnnn` label and `$SGnnnn` literal in a /Fa listing is numbered from it.
This script reads it directly with one compile per query:

    bin/symbolIndex.py local  src/launch.c ShowCarrierLaunchSequence frame
    bin/symbolIndex.py param  src/mono.c ResetStringBuilder
    bin/symbolIndex.py global src/launch.c nScrambleBackdropPositionY
    bin/symbolIndex.py file   src/fl.c nManeuverProximityRange
    bin/symbolIndex.py labels launch

`local` copies the unit next to itself, truncates FUNC right after the line
declaring NAME and appends `{ extern int v; do { v++; } while (v); }`.  The
block extern takes NAME + 1 and the loop's first label NAME + 2.

`param` replaces FUNC's body with the same probe, so the label is the last
parameter + 2 (or the last local + 2 when FUNC has locals; use `local` then).

`global` copies include/ to out/symbolIndex/inc, truncates the header that
declares NAME right after that declaration (keeping only the preprocessor
lines that close open conditionals), appends
`extern int v; void p(void) { do { v++; } while (v); }` and compiles only
the unit's preamble (everything up to its last #include) against the copy.
A function definition without a prototype costs two symbols, so NAME is the
first label - 4.  Every header the real build opens before NAME is opened the
same way, which matters: each header file costs a symbol, so preprocessed
(/E) copies are biased by roughly one per header.

`global` tries each header declaration of NAME in turn and reports the
first one the unit's preprocessor keeps (a later redeclaration costs
nothing).

`file` measures a declaration in the unit itself (a static or a definition)
by truncating a copy of the unit after that line and appending the same
probe function, so NAME is again the first label - 4.

`labels` prints each function's label numbers from out/UNIT.asm.  C1 parses
the whole unit before C2 runs, so C2-created labels are numbered after the
unit's last C1 symbol and stand out from the function's own range.

Indices are reported with their residues mod 32 and mod 257.
"""
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WORK = os.path.join(ROOT, 'out', 'symbolIndex')
CORE = ['/nologo', '/c', '/MTd', '/Og', '/Oi', '/Ot', '/Oy', '/Ob1', '/Gs']
PROBE_BLOCK = '{ extern int __symidx_v; do { __symidx_v++; } while (__symidx_v); }'


def unit_flags(unit):
    """Extra flags the Makefile uses for UNIT (see the per-unit rules)."""
    base = os.path.basename(unit)
    if base == 'sound.c':
        return ['/TP']
    if base == 'pilot.cpp':
        return ['/GX']
    return []


def compile_probe(path, unit, include):
    env = dict(os.environ, INCLUDE='compilers\\msvc420\\include')
    asm = os.path.join(WORK, 'probe.asm')
    if os.path.exists(asm):
        os.remove(asm)
    rel = os.path.relpath(path, ROOT).replace('/', '\\')
    cmd = (['./wibo', 'compilers/msvc420/bin/CL.EXE'] + CORE + unit_flags(unit) +
           ['/I', include, rel,
            '/Fo' + os.path.relpath(os.path.join(WORK, 'probe.obj'), ROOT).replace('/', '\\'),
            '/Fa' + os.path.relpath(asm, ROOT).replace('/', '\\')])
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, env=env)
    if not os.path.exists(asm):
        sys.exit('probe compile failed:\n' + r.stdout[-2000:])
    return open(asm, errors='replace').read()


def first_label(asm, proc):
    m = re.search(r'^' + re.escape(proc) + r'\s+PROC NEAR(.*?)^' + re.escape(proc) + r'\s+ENDP',
                  asm, re.S | re.M)
    if not m:
        sys.exit('probe function %s not found in listing' % proc)
    labels = sorted(int(x) for x in re.findall(r'\$L(\d+)', m.group(1)))
    if not labels:
        sys.exit('probe function has no labels')
    return labels[0]


def probe_label(asm, proc):
    """Number of the probe loop's own top label: the target of the jump that
    closes the loop after its store to __symidx_v.  Falls back to the first
    label when that pattern is not found."""
    m = re.search(r'^' + re.escape(proc) + r'\s+PROC NEAR(.*?)^' + re.escape(proc) + r'\s+ENDP',
                  asm, re.S | re.M)
    if not m:
        sys.exit('probe function %s not found in listing' % proc)
    seen_store = False
    for line in m.group(1).splitlines():
        if re.search(r'\bmov\s+DWORD PTR ___symidx_v\b', line) or re.search(r'\binc\s+DWORD PTR ___symidx_v\b', line):
            seen_store = True
        elif seen_store:
            jm = re.search(r'\bj\w+\s+(?:SHORT\s+)?\$L(\d+)', line)
            if jm:
                return int(jm.group(1))
    return first_label(asm, proc)


def report(what, index):
    print('%-40s index %6d  mod32 %2d  mod257 %3d' % (what, index, index % 32, index % 257))


def find_definition(lines, func):
    for i, line in enumerate(lines):
        if (re.match(r'^[A-Za-z_].*\b' + re.escape(func) + r'\s*\(', line)
                and not line.rstrip().endswith(';')):
            return i
    sys.exit('definition of %s not found' % func)


def proc_name(lines, start, func):
    """Decorated listing name: __stdcall functions carry @bytes."""
    header = ''
    for line in lines[start:start + 6]:
        header += line
        if '{' in line:
            break
    m = re.search(r'\b' + re.escape(func) + r'\s*\(', header)
    if m and '__stdcall' in header[:m.start()]:
        # Count top-level commas: function-pointer parameters nest parentheses.
        depth, count, params = 0, 0, ''
        for ch in header[m.end():]:
            if ch == '(':
                depth += 1
            elif ch == ')':
                if depth == 0:
                    break
                depth -= 1
            elif ch == ',' and depth == 0:
                count += 1
            params += ch
        count = 0 if params.strip() in ('', 'void') else count + 1
        return '_%s@%d' % (func, 4 * count)
    return '_' + func


def probe_unit_copy(unit, text):
    path = os.path.join(ROOT, os.path.dirname(unit), '__symbolIndex_probe' + os.path.splitext(unit)[1])
    open(path, 'w').write(text)
    return path


def cmd_local(unit, func, name):
    lines = open(os.path.join(ROOT, unit), errors='replace').read().splitlines(keepends=True)
    start = find_definition(lines, func)
    decl = None
    for i in range(start, len(lines)):
        if re.search(r'\b' + re.escape(name) + r'\s*(;|\[|=|,)', lines[i]):
            decl = i
            break
    if decl is None:
        sys.exit('declaration of %s not found in %s' % (name, func))
    # Close every block still open at the truncation point; a block opened by
    # `do` needs its `while`, and a value-returning function a return.
    body = re.sub(r'/\*.*?\*/', '', ''.join(lines[start:decl + 1]), flags=re.S)
    body = re.sub(r'"(?:\\.|[^"\\])*"', '""', body)
    stack = []
    for m in re.finditer(r'\{|\}', body):
        if m.group(0) == '{':
            stack.append(bool(re.search(r'\bdo\s*$', body[:m.start()])))
        elif stack:
            stack.pop()
    closing = ''
    for is_do in reversed(stack[1:]):
        closing += '} while (0);\n' if is_do else '}\n'
    text = (''.join(lines[:decl + 1]) + '\n    ' + PROBE_BLOCK + '\n' + closing +
            '    return 0;\n}\n')
    path = probe_unit_copy(unit, text)
    try:
        asm = compile_probe(path, unit, 'include')
    finally:
        os.remove(path)
    report('%s.%s' % (func, name), probe_label(asm, proc_name(lines, start, func)) - 2)


def cmd_param(unit, func):
    lines = open(os.path.join(ROOT, unit), errors='replace').read().splitlines(keepends=True)
    start = find_definition(lines, func)
    brace = start
    while '{' not in lines[brace]:
        brace += 1
    text = ''.join(lines[:brace + 1]) + '    ' + PROBE_BLOCK + '\n}\n'
    path = probe_unit_copy(unit, text)
    try:
        asm = compile_probe(path, unit, 'include')
    finally:
        os.remove(path)
    report('%s (last parameter)' % func, first_label(asm, proc_name(lines, start, func)) - 2)


def global_hits(name):
    """Every header line that declares NAME, in include-directory order."""
    hits = []
    for fn in sorted(os.listdir(os.path.join(ROOT, 'include'))):
        if not fn.endswith('.h'):
            continue
        lines = open(os.path.join(ROOT, 'include', fn), errors='replace').read().splitlines(keepends=True)
        for i, line in enumerate(lines):
            code = re.sub(r'/\*.*?\*/', '', line).split('//')[0]
            if (re.search(r'\b' + re.escape(name) + r'\b', code) and code.rstrip().endswith(';')
                    and not code.lstrip().startswith(('#', '*'))):
                hits.append((fn, i))
    return hits


def cmd_global(unit, name):
    """Probe each declaration of NAME in turn; the first one the unit's
    preprocessor keeps is the one that creates the symbol (a later
    redeclaration costs nothing)."""
    hits = global_hits(name)
    if not hits:
        sys.exit('%s: no declaration found in include/' % name)
    inc = os.path.join(WORK, 'inc')
    ulines = open(os.path.join(ROOT, unit), errors='replace').read().splitlines(keepends=True)
    last = max(k for k, line in enumerate(ulines) if line.lstrip().startswith('#include'))
    pre = ulines[:last + 1]
    depth = (sum(1 for line in pre if line.lstrip().startswith(('#if', '#ifdef', '#ifndef')))
             - sum(1 for line in pre if line.lstrip().startswith('#endif')))
    preamble = ''.join(pre) + '#endif\n' * max(depth, 0)
    probe = '\nextern int __symidx_v;\nvoid __symidx_probe(void) { do { __symidx_v++; } while (__symidx_v); }\n'
    for fn, i in hits:
        if os.path.exists(inc):
            shutil.rmtree(inc)
        shutil.copytree(os.path.join(ROOT, 'include'), inc)
        lines = open(os.path.join(inc, fn), errors='replace').read().splitlines(keepends=True)
        keep, cont = [], False
        for line in lines[i + 1:]:
            if cont or line.lstrip().startswith(('#if', '#ifdef', '#ifndef', '#else', '#elif', '#endif')):
                keep.append(line)
                cont = line.rstrip().endswith('\\')
        open(os.path.join(inc, fn), 'w').write(''.join(lines[:i + 1]) + probe + ''.join(keep))
        path = probe_unit_copy(unit, preamble)
        try:
            asm = compile_probe(path, unit, os.path.relpath(inc, ROOT).replace('/', '\\'))
        finally:
            os.remove(path)
        if re.search(r'^___symidx_probe\s+PROC NEAR', asm, re.M):
            report('%s (%s:%d)' % (name, fn, i + 1), first_label(asm, '___symidx_probe') - 4)
            return
    sys.exit('%s: no declaration is active in %s' % (name, unit))


def cmd_file(unit, name):
    lines = open(os.path.join(ROOT, unit), errors='replace').read().splitlines(keepends=True)
    decl = None
    for i, line in enumerate(lines):
        code = re.sub(r'/\*.*?\*/', '', line).split('//')[0]
        if (not line[:1].isspace() and re.search(r'\b' + re.escape(name) + r'\b', code)
                and code.rstrip().endswith((';', '{', '=', ','))):
            decl = i
            break
    if decl is None:
        sys.exit('%s: no file-scope declaration in %s' % (name, unit))
    # complete a multi-line initializer before probing
    end = decl
    if not lines[decl].split('/*')[0].rstrip().endswith(';'):
        while end + 1 < len(lines) and not lines[end].split('/*')[0].rstrip().endswith(';'):
            end += 1
    probe = '\nextern int __symidx_v;\nvoid __symidx_probe(void) { do { __symidx_v++; } while (__symidx_v); }\n'
    pre = lines[:end + 1]
    depth = (sum(1 for line in pre if line.lstrip().startswith(('#if', '#ifdef', '#ifndef')))
             - sum(1 for line in pre if line.lstrip().startswith('#endif')))
    path = probe_unit_copy(unit, ''.join(pre) + probe + '#endif\n' * max(depth, 0))
    try:
        asm = compile_probe(path, unit, 'include')
    finally:
        os.remove(path)
    report('%s (%s:%d)' % (name, os.path.basename(unit), decl + 1), first_label(asm, '___symidx_probe') - 4)


def cmd_labels(unit):
    asm = open(os.path.join(ROOT, 'out', unit + '.asm'), errors='replace').read()
    func, rows = None, {}
    order = []
    for line in asm.splitlines():
        m = re.match(r'^(_\S+)\s+PROC NEAR', line)
        if m:
            func = m.group(1)
            order.append(func)
        if func:
            for x in re.findall(r'\$(?:L|SG)(\d+)', line):
                rows.setdefault(func, set()).add(int(x))
    for f in order:
        print('%-40s %s' % (f, ' '.join(str(x) for x in sorted(rows.get(f, ())))))


def main(argv):
    os.makedirs(WORK, exist_ok=True)
    if len(argv) >= 4 and argv[0] == 'local':
        cmd_local(argv[1], argv[2], argv[3])
    elif len(argv) >= 3 and argv[0] == 'param':
        cmd_param(argv[1], argv[2])
    elif len(argv) >= 3 and argv[0] == 'global':
        for name in argv[2:]:
            cmd_global(argv[1], name)
    elif len(argv) >= 3 and argv[0] == 'file':
        for name in argv[2:]:
            cmd_file(argv[1], name)
    elif len(argv) == 2 and argv[0] == 'labels':
        cmd_labels(argv[1])
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main(sys.argv[1:])
