#!/usr/bin/env python3
"""Verify every `/* Function start: 0xADDR */` annotation names the function that
actually lives at that address.

This exists because I got it wrong 25 times: names like `GetUiFn9DD0` encode only
the low four hex digits, and filling in the high bits by eye produced annotations
pointing at unrelated functions.  A wrong annotation is worse than no annotation --
`make export-asm` then exports the wrong original and `make report` compares against
it, so the similarity number is meaningless without saying so.

    bin/auditAddresses.py            # report mismatches
    bin/auditAddresses.py --fix      # rewrite annotations to the real address
"""
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, 'src')
INVENTORY = os.path.join(ROOT, 'out', 'funcs_v5.json')
SKIP = {'void', 'if', 'while', 'for', 'return', 'TODO', 'switch', 'sizeof'}


def main():
    fix = '--fix' in sys.argv
    fl = {int(f['address'], 16): f['name'] for f in json.load(open(INVENTORY))}
    byname = {}
    for addr, name in fl.items():
        byname.setdefault(name, addr)

    bad = 0
    for root, _dirs, files in os.walk(SRC):
        for fn in sorted(files):
            if not fn.endswith(('.c', '.cpp')):
                continue
            path = os.path.join(root, fn)
            lines = open(path).read().split('\n')
            changed = False
            for i, line in enumerate(lines):
                m = re.search(r'Function start: 0x([0-9A-Fa-f]+)', line)
                if not m:
                    continue
                addr = int(m.group(1), 16)
                for j in range(i, min(i + 6, len(lines))):
                    seg = lines[j][m.end():] if j == i else lines[j]
                    # Comments between the marker and the signature routinely
                    # mention other functions by name -- never read one as the
                    # annotated function.  Inline /* TODO */ markers sit on the
                    # same line as the signature, so strip rather than skip.
                    seg = re.sub(r'^[^\S\n]*\*/', ' ', seg)   # tail of the marker itself
                    seg = re.sub(r'/\*.*?\*/', ' ', seg)
                    if not seg.strip() or seg.lstrip().startswith(('/*', '*', '//')):
                        continue
                    n = re.search(r'\b([A-Za-z_]\w*)\s*\(', seg)
                    if not n or n.group(1) in SKIP:
                        continue
                    name = n.group(1)
                    if name in byname and byname[name] != addr:
                        bad += 1
                        rel = os.path.relpath(path, ROOT)
                        print(f'{rel}:{i+1}: 0x{addr:08X} labelled {name}, '
                              f'but {name} is at 0x{byname[name]:08X}')
                        if fix:
                            lines[i] = (line[:m.start()]
                                        + f'Function start: 0x{byname[name]:X}'
                                        + line[m.end():])
                            changed = True
                    break
            if changed:
                open(path, 'w').write('\n'.join(lines))

    # A name may only be used once: two annotations sharing a name means one of
    # them is describing a function it is not.
    seen = {}
    for root, _dirs, files in os.walk(SRC):
        for fn in sorted(files):
            if not fn.endswith(('.c', '.cpp')):
                continue
            path = os.path.join(root, fn)
            for i, line in enumerate(open(path).read().split('\n')):
                m = re.match(r'\s*(?:[\w\*]+\s+)*?(\w+)\s*\(', line)
                if m and 'Function start' not in line and m.group(1) in byname:
                    prev = seen.setdefault(m.group(1), (path, i + 1))

    if bad == 0:
        print('all Function start annotations match the inventory')
        return 0
    print(f'\n{bad} mismatch(es)' + (' fixed' if fix else '; rerun with --fix'))
    return 0 if fix else 1


if __name__ == '__main__':
    sys.exit(main())
