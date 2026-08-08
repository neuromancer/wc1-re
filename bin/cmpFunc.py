#!/usr/bin/env python3
"""Compare one rebuilt function against the original disassembly.

Reads our compiler's listing from out/**.asm, the original from
code-full/<name>.disassembled.txt, normalises both to a comparable instruction
form, and prints a similarity score plus a side-by-side diff.

This is the fast inner loop; `make report` (binary-comp) remains the
authoritative whole-program check.

    bin/cmpFunc.py MinShort
    bin/cmpFunc.py MinShort MaxShort
"""
import difflib
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'out')
CODE = os.path.join(ROOT, 'code-full')

# MSVC 4.2 /Fa emits plain assembly: instructions are tab-indented, labels sit
# at column 0, and `;` lines are source-line markers.
LISTING = re.compile(r'^[ \t]+(\S.*?)\s*$')
LABEL = re.compile(r'^\S+:')
# assembler directives and alignment padding are not instructions
SKIP = re.compile(r'^(npad|align|public|extrn|extern|include|end|title|'
                  r'\.\w+|_\w+\s+(segment|ends|proc|endp)|\w+\s+(segment|ends|equ|db|dw|dd))\b',
                  re.I)


REGS = {'eax','ebx','ecx','edx','esi','edi','esp','ebp',
        'ax','bx','cx','dx','si','di','sp','bp',
        'al','bl','cl','dl','ah','bh','ch','dh',
        'st','fs','gs','cs','ds','es','ss'}

BRANCH = re.compile(r'^(j\w+|call|loop\w*)$')

# Ghidra and MASM spell the same opcode differently (JZ vs je, ...).
ALIAS = {'je': 'jz', 'jne': 'jnz', 'jc': 'jb', 'jnae': 'jb', 'jnb': 'jae',
         'jnc': 'jae', 'jna': 'jbe', 'jnbe': 'ja', 'jnge': 'jl', 'jnl': 'jge',
         'jng': 'jle', 'jnle': 'jg', 'jpe': 'jp', 'jpo': 'jnp',
         'sete': 'setz', 'setne': 'setnz', 'setc': 'setb', 'setnc': 'setae',
         'repz': 'repe', 'repnz': 'repne', 'cbw': 'cbw', 'iret': 'iretd'}


def norm(ins):
    """Normalise one instruction to `mnemonic operand-shape`.

    The two sides spell identical operands differently: MSVC's listing uses
    symbolic stack slots (`_b$[esp-4]`) where Ghidra prints the resolved form
    (`[ESP + 0x8]`), and offsets legitimately differ because our stack frame is
    not laid out identically.  So memory offsets and literals collapse to K and
    branch/call targets to T.

    Register WIDTH is preserved on purpose: `mov eax,ecx` must NOT compare equal
    to `mov ax,cx` -- that distinction is exactly what we are trying to match.
    """
    s = ins.strip().lower()
    s = re.sub(r';.*$', '', s).strip()
    if not s:
        return ''
    parts = s.split(None, 1)
    mnem = ALIAS.get(parts[0], parts[0])
    ops = parts[1] if len(parts) > 1 else ''

    # cdecl `ret 0` in the MSVC listing is Ghidra's bare `RET`
    if mnem == 'ret':
        return 'ret'
    # branch/call targets always differ between the two builds
    if BRANCH.match(mnem):
        return mnem + ' T'

    ops = re.sub(r'\b(?:dword|word|byte|qword|tbyte|fword)\s+ptr\b', ' ', ops)
    ops = re.sub(r'\bshort\b|\boffset\b|\bflat:', ' ', ops)
    ops = re.sub(r'[\w@?]+\$(?=\[)', '', ops)        # _b$[esp-4] -> [esp-4]

    def mem(m):
        """Normalise a [..] operand: drop spaces, collapse displacements."""
        inner = re.sub(r'\s+', '', m.group(1))
        inner = re.sub(r'\*(0x)?1\b', '', inner)      # [edi+edx*1] == [edi+edx]
        # absolute address, or a bare symbol (not a register) -> [S]
        if re.fullmatch(r'-?(0x)?[0-9a-f]+h?', inner):
            return '[S]'
        if re.fullmatch(r'_?[a-z_][\w@?$.]*', inner) and inner not in REGS:
            return '[S]'
        # Stack displacements legitimately differ: our locals are not laid out
        # identically, and MSVC prints them symbolically (_n$[esp]) anyway.
        # Compare the base register only, like binary-comp's --no-offsets.
        base = re.match(r'(esp|ebp)\b', inner)
        if base and '*' not in inner:
            return '[' + base.group(1) + ']'
        inner = re.sub(r'0x[0-9a-f]+|\b[0-9a-f]+h\b|\b\d+\b', 'D', inner)
        inner = re.sub(r'[-+]D', '+D', inner)
        return '[' + inner + ']'

    ops = re.sub(r'\[([^\]]*)\]', mem, ops)

    # a bare memory operand in the MSVC listing (`DWORD PTR _g_x`) is what Ghidra
    # prints as `dword ptr [0x...]`; both must reduce to [S]
    parts_ops = [o.strip() for o in ops.split(',')]
    for i, o in enumerate(parts_ops):
        if re.fullmatch(r'_?[a-z_][\w@?$.]{2,}', o) and o not in REGS:
            parts_ops[i] = '[S]'
    ops = ','.join(p for p in parts_ops if p)

    ops = re.sub(r'-?0x[0-9a-f]+', 'K', ops)
    ops = re.sub(r'-?\b[0-9a-f]+h\b', 'K', ops)
    ops = re.sub(r'(?<![\w$\[])-?\d+\b', 'K', ops)
    ops = re.sub(r'(?<![\[\w])_?[a-z_][\w@?$.]{2,}',
                 lambda m: m.group(0) if m.group(0) in REGS else 'S', ops)
    ops = re.sub(r'\s+', '', ops)
    return (mnem + ' ' + ops).strip()


def load_original(name):
    path = os.path.join(CODE, f'{name}.disassembled.txt')
    if not os.path.isfile(path):
        return None
    out = []
    for line in open(path).read().splitlines()[2:]:
        s = line.strip()
        if s:
            out.append(s)
    return out


def load_rebuilt(name):
    """Find `name` in any out/**/*.asm listing and return its instructions."""
    for dirpath, _d, files in os.walk(OUT):
        for fn in files:
            if not fn.endswith('.asm'):
                continue
            path = os.path.join(dirpath, fn)
            lines = open(path, errors='replace').read().splitlines()
            start = None
            for i, l in enumerate(lines):
                if re.match(rf'^_?{re.escape(name)}\b.*PROC', l, re.I):
                    start = i
                    break
            if start is None:
                continue
            body = []
            for l in lines[start + 1:]:
                if re.match(rf'^_?{re.escape(name)}\b.*ENDP', l, re.I):
                    return body, path
                if l.lstrip().startswith(';'):
                    continue
                m = LISTING.match(l)
                if m:
                    t = m.group(1)
                    if not LABEL.match(t) and not SKIP.match(t):
                        body.append(t)
            return body, path
    return None, None


def compare(name):
    orig = load_original(name)
    if orig is None:
        print(f'{name}: no code-full/{name}.disassembled.txt '
              f'(run bin/exportGhidra.py <addr>)')
        return None
    rebuilt, src = load_rebuilt(name)
    if rebuilt is None:
        print(f'{name}: not found in any out/**/*.asm (build it first)')
        return None

    a = [norm(x) for x in orig]
    b = [norm(x) for x in rebuilt]
    ratio = difflib.SequenceMatcher(None, a, b).ratio() * 100

    print(f'\n=== {name}  ({ratio:.1f}%)   original {len(orig)} instrs, '
          f'rebuilt {len(rebuilt)}   [{os.path.relpath(src, ROOT)}]')
    if ratio < 100.0:
        sm = difflib.SequenceMatcher(None, a, b)
        print(f"    {'ORIGINAL':<40} | REBUILT")
        for tag, i1, i2, j1, j2 in sm.get_opcodes():
            if tag == 'equal':
                for k in range(i1, i2):
                    print(f"    {orig[k]:<40} | {rebuilt[j1 + k - i1]}")
            else:
                for k in range(i1, i2):
                    print(f"  - {orig[k]:<40} |")
                for k in range(j1, j2):
                    print(f"  + {'':<40} | {rebuilt[k]}")
    return ratio


def main():
    names = sys.argv[1:]
    if not names:
        sys.exit(__doc__)
    scores = [s for s in (compare(n) for n in names) if s is not None]
    if len(scores) > 1:
        print(f'\nmean similarity over {len(scores)}: {sum(scores)/len(scores):.1f}%')


if __name__ == '__main__':
    main()
