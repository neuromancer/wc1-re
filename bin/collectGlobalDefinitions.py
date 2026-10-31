#!/usr/bin/env python3
"""Build one parser input from globals distributed across owner units."""

from __future__ import annotations

import argparse
import ast
import re
from pathlib import Path


FUNCTION_MARKER = "/* Function start:"
GAME_INCLUDE = '#include "wc1.h"'
OWNER_INCLUDES = (GAME_INCLUDE, '#include "winmain.h"', '#include "ix.h"')
BLOCK_COMMENT_RE = re.compile(r"/\*.*?\*/", re.DOTALL)
LINE_COMMENT_RE = re.compile(r"//[^\r\n]*")
DEFINE_RE = re.compile(
    r"^[ \t]*#define[ \t]+([A-Za-z_]\w*)[ \t]+([^\r\n]+)",
    re.MULTILINE,
)
ENUM_RE = re.compile(r"\benum(?:\s+[A-Za-z_]\w*)?\s*\{(.*?)\}\s*;", re.DOTALL)
INTEGER_SUFFIX_RE = re.compile(r"(?<=[0-9A-Fa-f])[uUlL]+\b")


def evaluate_integer(expression: str, constants: dict[str, int]) -> int:
    expression = INTEGER_SUFFIX_RE.sub("", expression.strip())
    node = ast.parse(expression, mode="eval")

    def visit(current: ast.AST) -> int:
        if isinstance(current, ast.Expression):
            return visit(current.body)
        if isinstance(current, ast.Constant) and type(current.value) is int:
            return current.value
        if isinstance(current, ast.Name) and current.id in constants:
            return constants[current.id]
        if isinstance(current, ast.UnaryOp):
            value = visit(current.operand)
            if isinstance(current.op, ast.UAdd):
                return value
            if isinstance(current.op, ast.USub):
                return -value
            if isinstance(current.op, ast.Invert):
                return ~value
        if isinstance(current, ast.BinOp):
            left = visit(current.left)
            right = visit(current.right)
            if isinstance(current.op, ast.Add):
                return left + right
            if isinstance(current.op, ast.Sub):
                return left - right
            if isinstance(current.op, ast.Mult):
                return left * right
            if isinstance(current.op, (ast.Div, ast.FloorDiv)):
                quotient = abs(left) // abs(right)
                return -quotient if (left < 0) != (right < 0) else quotient
            if isinstance(current.op, ast.Mod):
                quotient = abs(left) // abs(right)
                if (left < 0) != (right < 0):
                    quotient = -quotient
                return left - quotient * right
            if isinstance(current.op, ast.LShift):
                return left << right
            if isinstance(current.op, ast.RShift):
                return left >> right
            if isinstance(current.op, ast.BitOr):
                return left | right
            if isinstance(current.op, ast.BitAnd):
                return left & right
            if isinstance(current.op, ast.BitXor):
                return left ^ right
        raise ValueError(expression)

    return visit(node)


def header_constants(paths: list[Path]) -> dict[str, int]:
    constants: dict[str, int] = {}
    pending: list[tuple[str, str]] = []

    for path in paths:
        text = BLOCK_COMMENT_RE.sub("", path.read_text(encoding="utf-8"))
        text = LINE_COMMENT_RE.sub("", text)
        pending.extend(DEFINE_RE.findall(text))
        for enum_body in ENUM_RE.findall(text):
            previous_name: str | None = None
            for item in enum_body.split(","):
                item = item.strip()
                if not item:
                    continue
                if "=" in item:
                    name, expression = item.split("=", 1)
                    name = name.strip()
                    expression = expression.strip()
                else:
                    name = item
                    expression = "0" if previous_name is None else f"{previous_name} + 1"
                if re.fullmatch(r"[A-Za-z_]\w*", name):
                    pending.append((name, expression))
                    previous_name = name

    while pending:
        unresolved: list[tuple[str, str]] = []
        changed = False
        for name, expression in pending:
            try:
                constants[name] = evaluate_integer(expression, constants)
                changed = True
            except (SyntaxError, ValueError, ZeroDivisionError):
                unresolved.append((name, expression))
        if not changed:
            break
        pending = unresolved
    return constants


def expand_array_dimensions(text: str, constants: dict[str, int]) -> str:
    expanded: list[str] = []
    index = 0
    while index < len(text):
        if text.startswith("//", index):
            end = text.find("\n", index)
            if end < 0:
                expanded.append(text[index:])
                break
            expanded.append(text[index:end])
            index = end
            continue
        if text.startswith("/*", index):
            end = text.find("*/", index + 2)
            if end < 0:
                expanded.append(text[index:])
                break
            end += 2
            expanded.append(text[index:end])
            index = end
            continue
        if text[index] in {'"', "'"}:
            quote = text[index]
            end = index + 1
            while end < len(text):
                if text[end] == "\\":
                    end += 2
                    continue
                end += 1
                if text[end - 1] == quote:
                    break
            expanded.append(text[index:end])
            index = end
            continue
        if text[index] == "[":
            end = text.find("]", index + 1)
            if end >= 0:
                expression = text[index + 1:end]
                try:
                    value = evaluate_integer(expression, constants)
                except (SyntaxError, ValueError, ZeroDivisionError):
                    pass
                else:
                    expanded.append(f"[{value}]")
                    index = end + 1
                    continue
        expanded.append(text[index])
        index += 1
    return "".join(expanded)


UNREFERENCED_STATIC_RE = re.compile(
    r"^static[ \t]+[^;=()\r\n]*?\b([A-Za-z_]\w*)[ \t]*(?:\[[^\]\r\n]*\][ \t]*)*;"
    r"[ \t]*/\*[ \t]*0x[0-9A-Fa-f]{8}[ \t]*\*/[ \t]*\n",
    re.MULTILINE,
)


def without_unreferenced_statics(block: str, text: str) -> str:
    """Drop uninitialized statics that nothing in their unit references.

    CL still reserves their .bss slots but emits no COFF symbol for them, so
    no map can place them; verifyUnitData.py checks those slots through the
    section sizes and the offsets of the referenced statics around them.
    """
    def keep(match: re.Match[str]) -> str:
        name = match.group(1)
        uses = len(re.findall(rf"(?<![A-Za-z0-9_]){re.escape(name)}(?![A-Za-z0-9_])", text))
        return "" if uses == 1 else match.group(0)

    return UNREFERENCED_STATIC_RE.sub(keep, block)


def later_definitions(text: str, function_at: int) -> str:
    """Keep addressed file-scope storage declared after the first function."""
    from binary_comp.source.cpp import make_cpp_parser, sanitize_source, walk
    from binary_comp.source.globals import DECL_RE, address_from_comment, address_from_name

    source = text.encode("utf-8")
    tree = make_cpp_parser().parse(sanitize_source(source))
    functions = [
        (node.start_byte, node.end_byte)
        for node in walk(tree.root_node)
        if node.type == "function_definition"
    ]
    blocks: list[str] = []
    for match in DECL_RE.finditer(text):
        start = match.start("type")
        if start < function_at:
            continue
        byte_start = len(text[:start].encode("utf-8"))
        if any(first <= byte_start < last for first, last in functions):
            continue
        if (address_from_name(match.group("name")) is not None or
                address_from_comment(match.group("trailing") or "") is not None or
                address_from_comment(match.group("leading") or "") is not None):
            blocks.append(match.group(0).strip() + "\n")
    return "".join(blocks)


def definition_block(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    if path.name == "globals.c":
        return text

    include_positions = [text.find(include) for include in OWNER_INCLUDES]
    include_at = min((pos for pos in include_positions if pos >= 0), default=-1)
    function_at = text.find(FUNCTION_MARKER)
    # Data-only owners, such as the version object, have no function boundary.
    if function_at < 0:
        function_at = len(text)
    if include_at < 0 or function_at <= include_at:
        raise ValueError(f"cannot locate global-definition block in {path}")
    declarations_at = text.find("\n", include_at) + 1
    block = text[declarations_at:function_at].strip() + "\n"
    if function_at < len(text):
        block += later_definitions(text, function_at)
    return without_unreferenced_statics(block, text)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--constants-header", action="append", default=[], type=Path)
    parser.add_argument("sources", nargs="+", type=Path)
    args = parser.parse_args()
    constants = header_constants(args.constants_header)

    parts = [
        "/* Generated by bin/collectGlobalDefinitions.py; do not edit. */\n",
        f"{GAME_INCLUDE}\n",
    ]
    for source in args.sources:
        parts.append(f'\n#line 1 "{source.as_posix()}"\n')
        parts.append(expand_array_dimensions(definition_block(source), constants))
    output = "".join(parts)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    if not args.output.exists() or args.output.read_text(encoding="utf-8") != output:
        args.output.write_text(output, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
