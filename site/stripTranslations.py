#!/usr/bin/env python3
"""Restore reference string expressions in the staged source browser files."""

import re
import sys
from pathlib import Path


MARKERS = {"_": 1, "N_": 1, "C_": 2, "NC_": 2}
TOKENS = re.compile(
    r'(?P<comment>/\*.*?\*/|//[^\n]*)'
    r'|(?P<literal>"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\')'
    r'|(?P<definition>^[ \t]*#[ \t]*define[ \t]+(?:_|N_|C_|NC_)\('
    r'(?:\\\n|[^\n])*(?:\n|$))'
    r'|[A-Za-z_]\w*|[^\s]', re.DOTALL | re.MULTILINE,
)


def strip_translations(source):
    tokens, edits = [], []
    for token in TOKENS.finditer(source):
        if token.lastgroup == "definition" or (
            token.lastgroup == "comment"
            and token.group().startswith("/* Gettext-style UI markers.")
        ):
            edits.append((token.start(), token.end(), ""))
        elif token.lastgroup != "comment":
            tokens.append(token)

    index = 0
    while index < len(tokens) - 1:
        token = tokens[index]
        marker = token.group()
        if marker not in MARKERS or tokens[index + 1].group() != "(":
            index += 1
            continue
        index += 1
        start = tokens[index].end()
        arguments, depth = [], 1
        while depth and index + 1 < len(tokens):
            index += 1
            part = tokens[index]
            if part.group() == "(":
                depth += 1
            elif part.group() == ")":
                depth -= 1
            if depth == 0 or (depth == 1 and part.group() == ","):
                arguments.append(source[start:part.start()].strip())
                start = part.end()
        if depth or len(arguments) != MARKERS[marker] or not all(arguments):
            raise ValueError(f"Malformed {marker} marker near offset {token.start()}")
        edits.append((token.start(), part.end(), strip_translations(arguments[-1])))
        index += 1

    for start, end, replacement in sorted(edits, reverse=True):
        source = source[:start] + replacement + source[end:]
    return source


if __name__ == "__main__":
    for name in sys.argv[1:]:
        path = Path(name)
        path.write_text(strip_translations(path.read_text(encoding="utf-8")), encoding="utf-8")
