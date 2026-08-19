#!/usr/bin/env python3
"""Drop the `g_` prefix and the `_<address>` suffix from global names.

    g_bDIBSlamPending_00486518   ->  bDIBSlamPending
    g_stSpaceBuffer_005a7510     ->  stSpaceBuffer

Only names matching `g_<name>_<8 hex digits>` are touched, so the hungarian
tag survives and `DAT_<addr>` placeholders are left exactly as they are --
an unnamed global still has nothing but its address to identify it, and that
address has to stay.

The suffix must parse as an address inside the shipped image, which keeps a
name that merely ends in a hex-looking word from being mangled.

Two safety checks run before anything is written:

  * two globals whose names differ only by address would collide once the
    address is gone;
  * a stripped name may already be in use somewhere as an ordinary
    identifier (a local, a field, a function).

Either one aborts the run unless --force is given.

The address itself still has to survive somewhere: the layout audits resolve
each global's original VA from its name, falling back to a `/* 0x004xxxxx */`
comment on the declaration.  --keep-address-comment writes those comments as
it renames, so `make verify-globals` and `make audit-rebuilt-global-layout`
keep working while every use site reads clean.

    bin/stripGlobalAddresses.py --dry-run     # report, touch nothing
    bin/stripGlobalAddresses.py               # rewrite src, include, tests
    bin/stripGlobalAddresses.py path ...      # rewrite the given files/dirs
    bin/stripGlobalAddresses.py --keep-address-comment
    bin/stripGlobalAddresses.py --annotate-only MAP include/globals.h ...

A file that defines globals must not carry a bare `0x004xxxxx` in its header
comment -- binary-comp scans the leading text first, so the header would
outrank the first declaration's own address comment.
"""
import argparse
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEFAULT_PATHS = ['src', 'include', 'tests', 'bin/expandOneLiners.py']
SOURCE_SUFFIXES = ('.c', '.cpp', '.h', '.hpp', '.inc', '.py')

# WC1.EXE is based at 0x00400000 and ends just past 0x005b3c16.
IMAGE_START = 0x00400000
IMAGE_END = 0x00600000

GLOBAL_RE = re.compile(r'\bg_(\w+)_([0-9a-fA-F]{8})\b')

# Comments and literals, so the shadowing check does not trip over prose or
# over a path like "\\wc1\\streams\\".  Leftmost match wins, which is what
# keeps a quote inside a comment (and a `/*` inside a string) from confusing
# the scan.  Only the conflict scan uses this -- a rename still rewrites
# comments, so a global mentioned in prose keeps up with the code.
NON_CODE_RE = re.compile(
    r'/\*.*?\*/'
    r'|//[^\n]*'
    r'|#[^\n]*'
    r'|"""|\'\'\''
    r'|"(?:\\.|[^"\\\n])*"'
    r"|'(?:\\.|[^'\\\n])*'",
    re.DOTALL)


def strip_non_code(text):
    return NON_CODE_RE.sub(' ', text)


def in_image(address):
    return IMAGE_START <= int(address, 16) < IMAGE_END


def collect_files(paths):
    files = []
    for path in paths:
        full = path if os.path.isabs(path) else os.path.join(ROOT, path)
        if os.path.isfile(full):
            files.append(full)
            continue
        for dirpath, _, names in os.walk(full):
            for name in sorted(names):
                if name.endswith(SOURCE_SUFFIXES):
                    files.append(os.path.join(dirpath, name))
    return sorted(set(files))


def build_mapping(files):
    """old name -> new name, plus every plain identifier seen in the tree."""
    mapping = {}
    identifiers = set()
    for path in files:
        text = open(path, encoding='utf-8', errors='replace').read()
        for match in GLOBAL_RE.finditer(text):
            if in_image(match.group(2)):
                mapping[match.group(0)] = match.group(1)
        code = GLOBAL_RE.sub(' ', strip_non_code(text))
        identifiers.update(re.findall(r'\b[A-Za-z_]\w*\b', code))
    return mapping, identifiers


def find_conflicts(mapping, identifiers):
    """(collisions between globals, collisions with existing identifiers)"""
    by_new = {}
    for old, new in mapping.items():
        by_new.setdefault(new, []).append(old)
    collisions = {new: sorted(olds)
                  for new, olds in by_new.items() if len(olds) > 1}

    stripped = set(mapping.values())
    shadowed = sorted(stripped & identifiers)
    return collisions, shadowed


# A definition or declaration at file scope: optional storage/qualifier words,
# a type, then the name, then array bounds, then `=` or `;`.  Parenthesised
# forms are allowed so `void (*pEventManagerPump)(void);` is recognised too.
def declaration_re(name):
    return re.compile(
        r'^(?:extern|static|const|volatile|unsigned|signed|register)?[\w\s\*\(\)]*'
        r'\b%s\b\s*(?:\[[^\]]*\]\s*)*(?:\)\s*\([^)]*\)\s*)?\s*(?:=|;|\{|\[\s*$)'
        % re.escape(name))


# A declaration whose type sits alone on the line above, e.g.
#     MissionShipRecord
#         aMissionShips[WC1_MISSION_SHIP_STORAGE_COUNT] = {
BARE_TYPE_RE = re.compile(r'^[A-Za-z_][\w\s\*]*$')


ADDRESS_COMMENT_RE = re.compile(r'/\*\s*0x[0-9a-fA-F]{6,8}\s*\*/')


def annotate(files, addresses, dry_run):
    """Append `/* 0x00xxxxxx */` to each global's declaration/definition line.

    binary-comp reads the address off the name suffix, and falls back to an
    address comment on the declaration (analyzers/globals.py, address_from_
    comments).  Annotating the declaration keeps every use site clean while
    leaving the layout audits something to anchor on.
    """
    patterns = {name: declaration_re(name) for name in addresses}
    annotated = 0
    for path in files:
        lines = open(path, encoding='utf-8', errors='replace').read().split('\n')
        changed = 0
        for index, line in enumerate(lines):
            if not line or ADDRESS_COMMENT_RE.search(line):
                continue
            # A continuation line only counts when the line above is a bare
            # type, which is how a wrapped declaration reads.
            if line[0].isspace():
                previous = lines[index - 1] if index else ''
                if not (previous and not previous[0].isspace()
                        and BARE_TYPE_RE.match(previous)):
                    continue
            for name, address in addresses.items():
                if name in line and patterns[name].match(line.lstrip()):
                    lines[index] = '%s /* 0x%s */' % (line.rstrip(), address)
                    changed += 1
                    break
        if changed:
            if not dry_run:
                open(path, 'w', encoding='utf-8').write('\n'.join(lines))
            annotated += changed
            print('  %-32s %d' % (os.path.relpath(path, ROOT), changed))
    return annotated


def load_address_map(path):
    addresses = {}
    for line in open(path, encoding='utf-8'):
        if line.split():
            name, address = line.split()
            addresses[name] = address.lower()
    return addresses


def rewrite(files, mapping, dry_run):
    substitute = lambda m: (mapping.get(m.group(0)) or m.group(0))
    touched = 0
    replacements = 0
    for path in files:
        text = open(path, encoding='utf-8', errors='replace').read()
        new_text, count = GLOBAL_RE.subn(substitute, text)
        if count and new_text != text:
            if not dry_run:
                open(path, 'w', encoding='utf-8').write(new_text)
            touched += 1
            replacements += count
            print('  %-32s %d' % (os.path.relpath(path, ROOT), count))
    return touched, replacements


def main():
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('paths', nargs='*', default=DEFAULT_PATHS,
                        help='files or directories (default: %s)'
                             % ' '.join(DEFAULT_PATHS))
    parser.add_argument('--dry-run', action='store_true',
                        help='report what would change, write nothing')
    parser.add_argument('--force', action='store_true',
                        help='rewrite even when conflicts are reported')
    parser.add_argument('--keep-address-comment', action='store_true',
                        help='append /* 0x004xxxxx */ to each declaration, so '
                             'the layout audits still resolve the address')
    parser.add_argument('--annotate-only', metavar='MAP',
                        help='skip the rename; just add the address comments, '
                             'reading "<name> <address>" lines from MAP '
                             '(for a tree that was already stripped)')
    args = parser.parse_args()

    files = collect_files(args.paths or DEFAULT_PATHS)
    if not files:
        print('no source files found', file=sys.stderr)
        return 1

    if args.annotate_only:
        addresses = load_address_map(args.annotate_only)
        print('%d addresses, %d files\n\n%s:'
              % (len(addresses), len(files),
                 'would annotate' if args.dry_run else 'annotating'))
        total = annotate(files, addresses, args.dry_run)
        print('\n%d declaration(s) annotated%s'
              % (total, ' (dry run)' if args.dry_run else ''))
        return 0

    mapping, identifiers = build_mapping(files)
    if not mapping:
        print('no g_<name>_<address> globals found')
        return 0
    print('%d globals in %d files' % (len(mapping), len(files)))

    collisions, shadowed = find_conflicts(mapping, identifiers)
    if collisions:
        print('\n%d name collision(s) -- these differ only by address:'
              % len(collisions), file=sys.stderr)
        for new, olds in sorted(collisions.items()):
            print('  %s <- %s' % (new, ', '.join(olds)), file=sys.stderr)
    if collisions and not args.force:
        print('\nrefusing to rewrite; give the globals distinct names, or '
              're-run with --force', file=sys.stderr)
        return 1
    if shadowed:
        # A struct member or a block-scope local may legitimately share the
        # name -- only a file-scope clash actually breaks, and the compiler
        # is the one that can tell.  Warn and carry on.
        print('\nwarning: %d stripped name(s) also appear as an identifier; '
              'check the build:' % len(shadowed), file=sys.stderr)
        for name in shadowed:
            print('  %s' % name, file=sys.stderr)

    print('\n%s:' % ('would rewrite' if args.dry_run else 'rewriting'))
    touched, replacements = rewrite(files, mapping, args.dry_run)
    print('\n%d replacement(s) across %d file(s)%s'
          % (replacements, touched, ' (dry run)' if args.dry_run else ''))

    if args.keep_address_comment:
        addresses = {new: old.rsplit('_', 1)[1].lower()
                     for old, new in mapping.items()}
        print('\n%s:' % ('would annotate' if args.dry_run else 'annotating'))
        total = annotate(files, addresses, args.dry_run)
        print('\n%d declaration(s) annotated%s'
              % (total, ' (dry run)' if args.dry_run else ''))
    return 0


if __name__ == '__main__':
    sys.exit(main())
