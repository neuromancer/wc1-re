#!/usr/bin/env python3
"""Report functions whose reimplementation disagrees with the original's RET.

`RET <imm>` in the original means the callee pops the arguments, so the function
is `__stdcall` (or, in src/ix, an implicit `__thiscall` member).  A cdecl
reimplementation of a stdcall original mismatches at every return and at every
call site, and the tail-call shape changes as well, so the fix is usually worth
several instructions per function plus the caller stack cleanup.

C++ member functions and Win32 callbacks declared through CALLBACK/WINAPI are
skipped: their `RET <imm>` is expected and already correct.

    python3 bin/auditCallingConventions.py
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CODE = os.path.join(ROOT, 'code-full')
SOURCE_DIRS = ('src', os.path.join('src', 'ix'))

RET = re.compile(r'RET(?: (0x[0-9a-f]+|\d+))?\s*$')
SIGNATURE = re.compile(r'([A-Za-z_][\w:]*)\s*\(')
CALLEE_CLEANS = ('__stdcall', 'CALLBACK', 'WINAPI', 'PASCAL')


def original_returns():
    returns = {}
    for entry in sorted(os.listdir(CODE)):
        if not entry.endswith('.disassembled.txt'):
            continue
        name = None
        sizes = set()
        for line in open(os.path.join(CODE, entry)):
            if line.startswith('Function: '):
                name = line.split(': ', 1)[1].strip()
            match = RET.match(line.strip())
            if match:
                sizes.add(int(match.group(1), 0) if match.group(1) else 0)
        if name:
            returns[name] = sizes
    return returns


def reimplementations():
    found = {}
    for relative in SOURCE_DIRS:
        directory = os.path.join(ROOT, relative)
        for entry in sorted(os.listdir(directory)):
            if not entry.endswith(('.c', '.cpp')):
                continue
            lines = open(os.path.join(directory, entry)).read().split('\n')
            for index, line in enumerate(lines):
                if '/* Function start:' not in line:
                    continue
                signature = ''
                for follow in lines[index + 1:index + 6]:
                    signature += follow + ' '
                    if '(' in follow:
                        break
                match = SIGNATURE.search(signature)
                if match:
                    found[match.group(1)] = (
                        os.path.join(relative, entry), index + 2, signature)
    return found


def main():
    returns = original_returns()
    ours = reimplementations()
    suspect = []
    for name, sizes in returns.items():
        if name.startswith(('FUN_', 'LAB_')) or '::' in name:
            continue
        if name not in ours:
            continue
        path, line, signature = ours[name]
        # Only a token in front of the function name decorates the function
        # itself; one further right belongs to a function-pointer parameter.
        prefix = signature[:signature.index(name)]
        callee_cleans = any(token in prefix for token in CALLEE_CLEANS)
        if any(size > 0 for size in sizes) != callee_cleans:
            suspect.append((name, sorted(sizes), callee_cleans, path, line))
    suspect.sort()
    print('%d function(s) whose convention disagrees with the original RET'
          % len(suspect))
    for name, sizes, callee_cleans, path, line in suspect:
        print('  %-44s orig RET=%s ours_callee_cleans=%s  %s:%d'
              % (name, sizes, callee_cleans, path, line))
    return 1 if suspect else 0


if __name__ == '__main__':
    sys.exit(main())
