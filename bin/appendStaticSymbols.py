#!/usr/bin/env python3
"""Add compiler-local data symbols to a copy of LINK's map.

Resolve COFF section bases from public symbols and linked DIR32 relocations.
This supplies address metadata to binary-comp; it never changes the executable
or compares it with the original. Keep LINK's unmodified map as a separate file.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import re
import struct

from binary_comp.core.mapfile import parse_msvc_map_symbols
from binary_comp.core.pe import PEImage


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def recover_layout(path: Path, entries, image: PEImage) -> tuple[dict, dict, dict]:
    """Resolve object section bases independently of original-image contents."""
    data = path.read_bytes()
    machine, count, _, symbol_offset, symbol_count, optional, _ = \
        struct.unpack_from('<HHIIIHH', data)
    require(machine == 0x14C, f'{path}: expected i386 COFF')
    strings = data[symbol_offset + symbol_count * 18:]
    sections = {}
    for index in range(count):
        offset = 20 + optional + index * 40
        name = data[offset:offset + 8].rstrip(b'\0').decode('ascii')
        size, raw, relocations = struct.unpack_from('<III', data, offset + 16)
        relocation_count = struct.unpack_from('<H', data, offset + 32)[0]
        sections[index + 1] = (name, size, raw, relocations, relocation_count)

    symbols = {}
    index = 0
    while index < symbol_count:
        offset = symbol_offset + index * 18
        raw_name = data[offset:offset + 8]
        if raw_name[:4] == b'\0' * 4:
            start = struct.unpack_from('<I', raw_name, 4)[0]
            name = strings[start:].split(b'\0', 1)[0].decode('ascii')
        else:
            name = raw_name.rstrip(b'\0').decode('ascii')
        value, section, _, storage, auxiliaries = \
            struct.unpack_from('<IhHBB', data, offset + 8)
        symbols[index] = (name, value, section, storage)
        index += 1 + auxiliaries

    bases = {}

    def set_base(section: int, base: int) -> None:
        require(section not in bases or bases[section] == base,
                f'{path}: inconsistent addresses for section {section}')
        bases[section] = base

    for name, value, section, storage in symbols.values():
        if section <= 0 or storage != 2:
            continue
        matches = [entry.va for entry in entries
                   if entry.symbol == name and entry.object_file == path.name]
        require(len(matches) == 1, f'{path}: missing or ambiguous public {name}')
        set_base(section, matches[0] - value)

    data_targets = {index for index, (_, _, section, _) in symbols.items()
                    if section > 0 and
                    sections[section][0] in ('.data', '.bss', '.rdata')}
    relocations = image.relocations()
    for source_section, (_, size, raw, start, count) in sections.items():
        if source_section not in bases:
            continue
        for index in range(count):
            offset, target, kind = struct.unpack_from('<IIH', data, start + index * 10)
            if target not in data_targets or kind != 6:  # IMAGE_REL_I386_DIR32
                continue
            require(offset + 4 <= size, f'{path}: relocation outside section')
            site = bases[source_section] + offset
            require(site in relocations, f'{path}: missing linked HIGHLOW at {site:#x}')
            linked = image.read(site, 4)
            require(linked is not None, f'{path}: unreadable relocation at {site:#x}')
            value = struct.unpack('<I', linked)[0] - struct.unpack_from('<I', data, raw + offset)[0]
            _, symbol_value, target_section, _ = symbols[target]
            # Indexed references can have negative addends, encoded as DWORDs.
            set_base(target_section, (value - symbol_value) & 0xFFFFFFFF)

    return sections, symbols, bases


def recover(path: Path, entries, image: PEImage) -> list[tuple[str, int]]:
    sections, symbols, bases = recover_layout(path, entries, image)
    local_names = {index: match.group(1)
                   for index, (name, _, section, storage) in symbols.items()
                   if section > 0 and storage == 3
                   and (match := re.fullmatch(r'(_\w+)\$S\d+', name))}
    result = []
    for index, name in local_names.items():
        _, offset, section, _ = symbols[index]
        require(section in bases, f'{path}: cannot resolve private {name}')
        require(sections[section][0] in ('.data', '.bss'),
                f'{path}: unsupported private storage for {name}')
        require(offset < sections[section][1], f'{path}: private symbol outside section')
        result.append((name, bases[section] + offset))
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--map', required=True)
    parser.add_argument('--rebuilt', required=True)
    parser.add_argument('--output', required=True)
    parser.add_argument('objects', nargs='+', type=Path)
    args = parser.parse_args()
    require(Path(args.map).resolve() != Path(args.output).resolve(),
            'the output must preserve the original LINK map')
    image = PEImage(args.rebuilt)
    entries = parse_msvc_map_symbols(args.map)
    names = {entry.symbol for entry in entries}
    lines = []
    for path in args.objects:
        for name, address in recover(path, entries, image):
            require(name not in names, f'{path}: duplicate map symbol {name}')
            names.add(name)
            section = image.section_for_va(address)
            require(section is not None and section.name == '.data',
                    f'{path}: private data outside linked .data')
            segment = image.sections.index(section) + 1
            lines.append(f' {segment:04x}:{address - section.start:08x} '
                         f'      {name:<30} {address:08x}   {path.name}\n')
    output = Path(args.map).read_text(encoding='latin1')
    output += '\n Private data symbols recovered from COFF and linked relocations\n\n'
    output += ''.join(lines)
    Path(args.output).write_text(output, encoding='latin1')
    print(f'Recovered {len(lines)} private data symbols in {args.output}')


if __name__ == '__main__':
    main()
