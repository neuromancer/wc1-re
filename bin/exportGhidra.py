#!/usr/bin/env python3
"""Export per-function disassembly from the live Ghidra MCP server into
code-full/, in the layout binary-comp expects:

    code-full/<NAME>.disassembled.txt
        Function: <NAME>
        Address: 0x<addr>

        <one instruction per line>

Usage:
    bin/exportGhidra.py                 # export every developer function
    bin/exportGhidra.py 0x41d0c0 ...    # export just these addresses
"""
import json
import os
import sys
import urllib.parse
import urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, 'code-full')
BASE = os.environ.get('GHIDRA_URL', 'http://127.0.0.1:8089')
CRT_LO = 0x00449100


def get(path, **params):
    url = f"{BASE}/{path}?{urllib.parse.urlencode(params)}"
    with urllib.request.urlopen(url, timeout=180) as r:
        return r.read().decode('utf-8', 'replace')


def get_json(path, **params):
    return json.loads(get(path, **params))


def clean(text):
    """Strip the tool's framing and keep instruction lines."""
    lines = []
    for raw in text.splitlines():
        s = raw.strip()
        if not s:
            continue
        # tool output is usually "<addr>: <mnemonic> <operands>"
        if ':' in s:
            head, _, tail = s.partition(':')
            h = head.strip().lower().replace('0x', '')
            if len(h) >= 6 and all(c in '0123456789abcdef' for c in h):
                s = tail.strip()
        if s:
            lines.append(s)
    return lines


def main():
    os.makedirs(OUT, exist_ok=True)
    funcs = get_json('list_functions_enhanced', limit=3000)['functions']
    by_addr = {f['address'].lower(): f['name'] for f in funcs}

    if len(sys.argv) > 1:
        wanted = [a.lower().replace('0x', '').rjust(8, '0') for a in sys.argv[1:]]
    else:
        wanted = sorted(a for a in by_addr if int(a, 16) < CRT_LO)

    done = skipped = 0
    for addr in wanted:
        name = by_addr.get(addr)
        if name is None:
            print(f"  ?? no function at {addr}", file=sys.stderr)
            skipped += 1
            continue
        try:
            text = get('disassemble_function', address='0x' + addr)
        except Exception as exc:
            print(f"  !! {addr} {name}: {exc}", file=sys.stderr)
            skipped += 1
            continue
        body = clean(text)
        if not body:
            skipped += 1
            continue
        path = os.path.join(OUT, f"{name}.disassembled.txt")
        with open(path, 'w') as fh:
            fh.write(f"Function: {name}\n")
            fh.write(f"Address: 0x{addr}\n\n")
            fh.write("\n".join(body) + "\n")
        done += 1
        if done % 200 == 0:
            print(f"  {done}/{len(wanted)}", flush=True)
    print(f"exported {done} functions to code-full/ ({skipped} skipped)")


if __name__ == '__main__':
    main()
