#!/usr/bin/env python3
"""Refresh the README similarity table from `make report` output.

Usage:

    make report | python3 bin/updateReadmeSimilarity.py

The report is grouped by compilation unit and listed in address order, which is
exactly the shape of the collapsed table in README.md, so the section between
`<details>` and `</details>` is rewritten wholesale.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
README = os.path.join(ROOT, 'README.md')

UNIT = re.compile(r'^=== (\S+) ===')
ROW = re.compile(r'^  (\S+)\s+(0x[0-9A-Fa-f]+)\s+([\d.]+)%')
AVERAGE = re.compile(r'Average similarity:\s*([\d.]+)%')


def main():
    report = sys.stdin.read()
    unit = None
    rows = []
    for line in report.split('\n'):
        match = UNIT.match(line)
        if match:
            unit = match.group(1)
            continue
        match = ROW.match(line)
        if match and unit:
            rows.append((unit, match.group(1), match.group(2), match.group(3)))
    if not rows:
        print('no similarity rows on stdin', file=sys.stderr)
        return 1
    average = AVERAGE.search(report)
    if not average:
        print('no average similarity line on stdin', file=sys.stderr)
        return 1

    table = ['| Compilation unit | Function | Address | Similarity |',
             '| --- | --- | --- | ---: |']
    for unit, name, address, score in rows:
        table.append('| `%s` | `%s` | `%s` | %s%% |' % (unit, name, address, score))

    section = ['<details>',
               '<summary>All %s function similarity scores (%s%% average)</summary>'
               % (format(len(rows), ','), average.group(1)),
               '']
    section += table
    section += ['', '</details>']

    text = open(README).read()
    start = text.index('<details>')
    end = text.index('</details>') + len('</details>')
    open(README, 'w').write(text[:start] + '\n'.join(section) + text[end:])
    print('README similarity table: %d rows, %s%% average'
          % (len(rows), average.group(1)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
