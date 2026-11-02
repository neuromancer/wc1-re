#!/usr/bin/env python3
"""Extract the hand-written raster library from the retail executable as MASM source.

The raster library (0x00439C40-0x0043F635 in .text, 0x0046E69C-0x0047008B in
.data) was assembled, not compiled: its shifts by one use the D1/D0 encodings
CL 4.20 never emits, and it keeps tables in its code segment (docs/COMPILER.md).
This script disassembles it from the original executable into src/raster.asm,
with symbolic labels for branch targets, functions and data, and verifies with
UASM that the generated source assembles to the original bytes and
relocations.  Jumps are left for the assembler to size, as MASM 6 did; a jump
UASM sized differently would get an explicit size, and any other instruction
it encoded differently would be emitted as bytes.  Neither happens today.

    bin/extractRasterLibrary.py            # regenerate and verify src/raster.asm
    bin/extractRasterLibrary.py --check    # verify the committed source only

Names and comments come from NAMES and DATA_LABELS below, C declarations from
include/wc1funcs.h.  Regenerating discards manual edits to src/raster.asm, so
make lasting changes here.
"""
from __future__ import annotations

import argparse
import os
import re
import struct
import subprocess
import sys
import tempfile
import textwrap

import capstone
from capstone import x86
import pefile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ORIGINAL = os.path.join(ROOT, 'data', 'full', 'WC1.ORI.EXE')
PROTOTYPES = os.path.join(ROOT, 'include', 'wc1funcs.h')
OUTPUT = os.path.join(ROOT, 'src', 'raster.asm')
UASM = os.path.join(ROOT, 'out', 'uasm', 'uasm')  # make out/uasm/uasm

CODE = (0x00439C40, 0x0043F636)
DATA = (0x0046E69C, 0x0047008C)

# Functions: address -> (name, comment lines).  .model flat, c adds the
# leading underscore.
NAMES = {
    0x00439C40: ('CopyRasterDriverName', 'Unreferenced. Copies the raster driver name out of its callback table.'),
    0x00439C69: ('InstallRasterDriverCallbacks', 'Unreferenced. Copies thirteen driver callbacks into the library.'),
    0x00439C88: ('SetRasterClipPixel', 'Writes one pixel through a clip, with explicit segment setup.'),
    0x00439D63: ('ReadRasterClipPixel', 'Reads one pixel through a clip.'),
    0x00439E39: ('DrawClippedLine', 'Draws a clipped line: mode 0 writes a colour, mode 1 translates through\na colour table, and higher modes call the callback passed as the colour.'),
    0x0043A83B: ('FillRasterClipCheckerboard', 'Fills a clip with a two-colour checkerboard.'),
    0x0043A974: ('DrawRLEImage', 'Draws a clipped RLE frame.'),
    0x0043AD78: ('DrawRLEImageUnclipped', 'Draws an RLE frame that lies entirely inside the clip.'),
    0x0043AE3F: ('SetPaletteTranslationTable', 'Selects the colour translation table for the colour RLE blitters.'),
    0x0043AE5E: ('DrawRLEImageColor', 'Draws a clipped RLE frame through the colour translation table.'),
    0x0043B336: ('DrawRLEImageColorUnclipped', 'Draws an unclipped RLE frame through the colour translation table.'),
    0x0043B469: ('RotateRLEImage', 'Draws an RLE frame rotated and scaled through the transform vertices.'),
    0x0043C015: ('GetRLEFrameBounds', 'Measures the bounds of an RLE frame.'),
    0x0043C18D: ('EncodeRasterClipToRLEFrame', 'Encodes a clip into an RLE frame.'),
    0x0043C410: ('TranslateRLEFramePalette', 'Remaps the colours of an RLE frame in place.'),
    0x0043C4A2: ('EncodeRLEScanline', 'Classifies one scanline into RLE runs for EncodeRasterClipToRLEFrame.'),
    0x0043C62B: ('EmitRLEScanlineRun', 'Emits one RLE run for EncodeRLEScanline.'),
    0x0043C808: ('FillRasterClip', 'Fills a clip with one colour.'),
    0x0043C8E7: ('BlitRasterClip', 'Copies one clip into another.'),
    0x0043CC83: ('ScrollRasterClipWrapped', 'Scrolls a clip, wrapping the rows that leave it.'),
    0x0043CE80: ('DrawRasterEllipse', 'Draws an ellipse outline.'),
    0x0043D1C1: ('FillRasterEllipse', 'Fills an ellipse.'),
    0x0043E2D3: ('GetRLETransformTrig', 'Looks up sine and cosine in the quarter-wave table.'),
    0x0043E38B: ('CalculateRoundedRLEFixedProduct', 'Multiplies two fixed-point values with rounding.'),
    0x0043E3B1: ('TransformRLEPoint', 'Rotates and scales one point for RotateRLEImage.'),
    0x0043E478: ('GetRawImageHeight', 'Returns the height of a raw image.'),
    0x0043E48B: ('GetRawFrameWidth', 'Returns the width of a raw image frame.'),
    0x0043E4AB: ('BlitRawFrame', 'Draws a raw image frame through a clip.'),
    0x0043E63E: ('BlitSelectedRawFrames', 'Draws several raw image frames.'),
    0x0043E675: ('BlitRawScanline', 'Draws one raw scanline.'),
    0x0043E784: ('FindIFFChunkData', 'Finds a chunk in an IFF image.'),
    0x0043E7C6: ('DecodeIFFImage', 'Decodes an IFF ILBM image.'),
    0x0043E98D: ('CopyILBMPalette', 'Copies the palette of an ILBM image.'),
    0x0043E9BE: ('GetILBMImageSize', 'Returns the size of an ILBM image.'),
    0x0043E9EB: ('DecodePCXImage', 'Decodes a PCX image.'),
    0x0043EA6D: ('CopyPCXPaletteFromFileTail', 'Copies the palette stored at the end of a PCX file.'),
    0x0043EA98: ('GetPCXImageSize', 'Returns the size of a PCX image.'),
    0x0043EABF: ('ResetGIFLZWDictionary', 'Resets the LZW dictionary of the GIF decoder.'),
    0x0043EB07: ('ReadGIFDataSubBlockByte', 'Reads one byte from the GIF data sub-blocks.'),
    0x0043EB20: ('ReadGIFLZWCode', 'Reads one LZW code from a GIF stream.'),
    0x0043EB66: ('AppendGIFLZWDictionaryEntry', 'Appends one entry to the LZW dictionary.'),
    0x0043EBAC: ('EmitGIFDecodedPixel', 'Stores one decoded GIF pixel, following the interlace order.'),
    0x0043EC29: ('ExpandGIFLZWImage', 'Expands the LZW image data of a GIF.'),
    0x0043EE42: ('CopyGIFPalette', 'Copies the palette of a GIF image.'),
    0x0043EEA3: ('GetGIFImageSize', 'Returns the size of a GIF image.'),
    0x0043EEDB: ('GetRLEFrameDimensions', 'Returns the dimensions of an RLE frame.'),
    0x0043EEFD: ('GetRLEFrameExtents', 'Returns the extents of an RLE frame.'),
    0x0043EF20: ('GetRLEImageSize', 'Returns the size of one RLE image frame.'),
    0x0043EF54: ('GetRLEImageOrigin', 'Returns the origin of one RLE image frame.'),
    0x0043EF7E: ('ApplyRLEFramePalette', 'Applies the palette stored with an RLE frame.'),
    0x0043EFC9: ('CopyRLEFramePalette', 'Copies the palette stored with an RLE frame.'),
    0x0043F011: ('SetRLEFramePalette', 'Stores a palette with an RLE frame.'),
    0x0043F05B: ('GetRLEFrameCount', 'Returns the number of frames in an RLE image.'),
    0x0043F06E: ('CollectUniqueRLEImageFrames', 'Lists the distinct frames of an RLE image.'),
    0x0043F0D0: ('CollectUniqueRLEPaletteFrames', 'Lists the distinct palettes of an RLE image.'),
    0x0043F1AE: ('DissolveRasterClip', 'Unreferenced. Copies pixels from one clip to another in the order of a\nmaximal-length LFSR, a dissolve that a caller can resume from the\nreturned register state.  The tap table before it holds one feedback\nmask per register width.'),
    0x0043F425: ('FadeRasterPaletteToPalette', 'Fades a surface palette towards another in fixed point, through the\ndriver callbacks.'),
    0x0043F5A9: ('CollectRasterClipColours', 'Unreferenced. Collects the set of colours used in a clip.'),
}

# Data: address -> (symbol, unit size, public?, comment).  Spans run to the
# next label; unnamed gaps are emitted as bytes.
DATA_LABELS = {
    0x0043D4BF: ('anRLEQuarterCosine', 4, True, 'Quarter-wave cosine table, 901 entries, 16.16 fixed point.'),
    0x0043E778: ('abIffBmhdChunkId', 1, True, 'IFF chunk identifiers.'),
    0x0043E77C: ('abIffCmapChunkId', 1, True, None),
    0x0043E780: ('abIffBodyChunkId', 1, True, None),
    0x0043F132: ('adwRasterDissolveTaps', 4, False, 'Maximal-length LFSR feedback masks for DissolveRasterClip, indexed by\nregister width from 2.'),
    0x0046E69C: ('apRasterDriverCallbacks', 4, True, None),
    0x0046E6D0: ('szRasterDriverName', 1, True, None),
    0x0046E6DD: ('pRLEEncodeBuffer', 4, True, None),
    0x0046E6E1: ('nRLEPendingSkip', 4, True, None),
    0x0046E6E5: ('pRLEScanlineStart', 4, True, None),
    0x0046E6E9: ('pRLEScanCursor', 4, True, None),
    0x0046E6ED: ('pRLEOutputCursor', 4, True, None),
    0x0046E6F1: ('pRLELiteralStart', 4, True, None),
    0x0046E6F5: (None, 1, False, 'Unreferenced.'),
    0x0046E705: ('nRLEEncodedMinimumX', 4, True, None),
    0x0046E709: ('nRLEEncodedMinimumY', 4, True, None),
    0x0046E70D: ('nRLEEncodedMaximumX', 4, True, None),
    0x0046E711: ('nRLEEncodedMaximumY', 4, True, None),
    0x0046E715: ('apRasterDissolveSourceRows', 4, False, 'Source row addresses for DissolveRasterClip.'),
    0x0046EC15: ('abRasterDecodeBuffer', 1, True, 'Image decode buffer; DissolveRasterClip keeps its target row\naddresses at +700h.'),
    0x0046F915: ('abRasterPaletteUsageScratch', 1, True, None),
    0x0046FC15: ('abRasterFadeAccumulator', 1, True, None),
    0x0046FF15: ('abGifLZWCodeMasks', 1, True, None),
    0x0046FF1E: ('abGifInterlaceRowSteps', 1, True, None),
    0x0046FF23: ('abGifInterlaceRowStarts', 1, True, None),
    0x0046FF28: ('pGifRasterTarget', 4, True, None),
    0x0046FF2C: ('abRasterPaletteTranslation', 1, True, None),
    0x0047002C: ('aRLETransformVertices', 4, True, None),
    0x0047007C: ('anRLESourceSteps', 4, True, None),
}

STRING_OPS = {
    'movsb', 'movsw', 'movsd', 'stosb', 'stosw', 'stosd', 'lodsb', 'lodsw', 'lodsd',
    'scasb', 'scasw', 'scasd', 'cmpsb', 'cmpsw', 'cmpsd',
}
BRANCHES = {'call', 'loop', 'loope', 'loopne', 'jecxz', 'jcxz'}
# Capstone spellings MASM does not accept.
MNEMONICS = {'pushal': 'pushad', 'popal': 'popad'}
SIZE_NAMES = {1: 'byte', 2: 'word', 4: 'dword', 6: 'fword', 8: 'qword', 10: 'tbyte'}


def hexnum(value: int) -> str:
    if value < 0:
        return '-' + hexnum(-value)
    if value < 10:
        return str(value)
    digits = f'{value:X}'
    return ('0' if digits[0].isalpha() else '') + digits + 'h'


class Image:
    def __init__(self, path: str):
        pe = pefile.PE(path)
        self.base = pe.OPTIONAL_HEADER.ImageBase
        self.data = pe.get_memory_mapped_image()
        self.relocs = set()
        for block in pe.DIRECTORY_ENTRY_BASERELOC:
            for entry in block.entries:
                if entry.type == 3:
                    self.relocs.add(entry.rva + self.base)

    def read(self, address: int, size: int) -> bytes:
        return bytes(self.data[address - self.base:address - self.base + size])

    def dword(self, address: int) -> int:
        return struct.unpack('<I', self.read(address, 4))[0]


def data_spans():
    """In-code data ranges, as (start, end) pairs."""
    code_labels = sorted(a for a in DATA_LABELS if CODE[0] <= a < CODE[1])
    spans = []
    for address in code_labels:
        following = [a for a in list(NAMES) + code_labels if a > address]
        spans.append((address, min(following)))
    merged = []
    for start, end in spans:
        if merged and merged[-1][1] == start:
            merged[-1] = (merged[-1][0], end)
        else:
            merged.append((start, end))
    return merged


class Generator:
    def __init__(self, image: Image, raw: set[int], sized: set[int]):
        self.image = image
        self.raw = raw
        self.sized = sized
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True
        self.instructions = []
        self.spans = data_spans()
        cursor = CODE[0]
        for start, end in self.spans + [(CODE[1], CODE[1])]:
            address = cursor
            while address < start:
                insn = next(self.md.disasm(image.read(address, 16), address), None)
                if insn is None or address + insn.size > start:
                    raise SystemExit(f'cannot decode code at {address:#x}')
                self.instructions.append(insn)
                address += insn.size
            cursor = end
        self.starts = {insn.address for insn in self.instructions}
        self.labels = {}
        for insn in self.instructions:
            target = self.branch_target(insn)
            if target is not None:
                if target not in self.starts:
                    raise SystemExit(f'branch into an instruction at {target:#x}')
                self.labels.setdefault(target, f'L{target:06X}')
        for address, (symbol, *_rest) in NAMES.items():
            self.labels[address] = symbol

    @staticmethod
    def branch_target(insn):
        if insn.mnemonic in BRANCHES or insn.mnemonic.startswith('j'):
            op = insn.operands[0]
            if op.type == x86.X86_OP_IMM:
                return op.imm
        return None

    def symbolize(self, value: int) -> str:
        """Express an absolute address as label+offset."""
        candidates = [(a, s) for a, (s, *_r) in DATA_LABELS.items() if s and a <= value]
        candidates += [(a, s) for a, s in self.labels.items() if a <= value and a in self.starts]
        address, symbol = max(candidates)
        offset = value - address
        if offset and address in DATA_LABELS:
            next_label = min((a for a in DATA_LABELS if a > address), default=None)
            limit = next_label if next_label is not None else (DATA[1] if address >= DATA[0] else CODE[1])
            if value >= limit:
                raise SystemExit(f'address {value:#x} beyond {symbol}')
        return symbol + (('+' if offset > 0 else '') + hexnum(offset) if offset else '')

    def tables_before(self, value: int) -> str:
        # The dissolve function indexes its tap table from width 2: table-8.
        for address, (symbol, *_r) in DATA_LABELS.items():
            if symbol and 0 < address - value <= 8 and CODE[0] <= address < CODE[1]:
                return f'{symbol}-{address - value}'
        return self.symbolize(value)

    def reloc_fields(self, insn):
        return [r for r in range(insn.address, insn.address + insn.size) if r in self.image.relocs]

    def memory(self, insn, op, relocated: str | None) -> str:
        mem = op.mem
        parts = []
        if mem.base:
            parts.append(insn.reg_name(mem.base))
        if mem.index:
            parts.append(insn.reg_name(mem.index) + (f'*{mem.scale}' if mem.scale != 1 else ''))
        text = '+'.join(parts)
        if relocated:
            text = (text + '+' if text else '') + relocated
        elif mem.disp or not text:
            disp = mem.disp
            if not text:
                disp &= 0xFFFFFFFF
            text += ('+' if disp >= 0 and text else '') + hexnum(disp) if text else hexnum(disp)
        segment = ''
        if mem.segment and insn.reg_name(mem.segment) not in ('ds',):
            segment = insn.reg_name(mem.segment) + ':'
        elif not mem.base and not mem.index and not relocated:
            segment = 'ds:'
        size = SIZE_NAMES.get(op.size)
        return (f'{size} ptr ' if size else '') + f'{segment}[{text}]'

    def format(self, insn) -> str | None:
        """MASM text for an instruction, or None to emit its bytes."""
        if insn.address in self.raw:
            return None
        mnemonic = MNEMONICS.get(insn.mnemonic, insn.mnemonic)
        prefix_bytes = [p for p in insn.prefix if p]
        base = mnemonic.split()[-1]
        if base in STRING_OPS:
            if any(p in (0x26, 0x2E, 0x36, 0x3E, 0x64, 0x65) for p in prefix_bytes):
                return None
            return mnemonic
        target = self.branch_target(insn)
        if target is not None:
            label = self.labels[target]
            if mnemonic == 'call':
                return f'call {label}'
            if mnemonic in ('loop', 'loope', 'loopne', 'jecxz', 'jcxz') or insn.address not in self.sized:
                return f'{mnemonic} {label}'
            size = 'short' if insn.size == 2 else 'near ptr'
            return f'{mnemonic} {size} {label}'
        fields = self.reloc_fields(insn)
        disp_at = insn.address + insn.disp_offset if insn.disp_offset else None
        imm_at = insn.address + insn.imm_offset if insn.imm_offset else None
        operands = []
        for op in insn.operands:
            if op.type == x86.X86_OP_REG:
                operands.append(insn.reg_name(op.reg))
            elif op.type == x86.X86_OP_IMM:
                if imm_at in fields:
                    operands.append('offset ' + self.symbolize(self.image.dword(imm_at)))
                else:
                    value = op.imm
                    if op.size and value < 0 and mnemonic not in ('push',):
                        value &= (1 << (8 * op.size)) - 1
                        if value >= 1 << (8 * op.size - 1) and op.size == 1:
                            value -= 1 << 8
                    operands.append(hexnum(op.imm if op.imm >= -0x80 and op.imm < 0 else value))
            elif op.type == x86.X86_OP_MEM:
                relocated = None
                if disp_at in fields:
                    relocated = self.tables_before(self.image.dword(disp_at))
                operands.append(self.memory(insn, op, relocated))
            else:
                return None
        consumed = {f for f in (disp_at, imm_at) if f in fields}
        if set(fields) - consumed:
            return None
        if mnemonic in ('test', 'xchg') and all(op.type == x86.X86_OP_REG for op in insn.operands):
            # The original assembler put the first register in ModRM.rm, UASM
            # puts it in ModRM.reg; both instructions are symmetric.
            operands.reverse()
        text = mnemonic
        if operands:
            text += ' ' + ', '.join(operands)
        return text


def emit_bytes(data: bytes, unit: int) -> list[str]:
    lines = []
    if not any(data):
        if unit == 4 and len(data) % 4 == 0:
            count, directive = len(data) // 4, 'dd'
        else:
            count, directive = len(data), 'db'
        return [f'{directive} 0' if count == 1 else f'{directive} {hexnum(count)} dup (0)']
    if unit == 4 and len(data) % 4 == 0:
        values = [struct.unpack('<I', data[i:i + 4])[0] for i in range(0, len(data), 4)]
        for i in range(0, len(values), 8):
            lines.append('dd ' + ', '.join(hexnum(v) for v in values[i:i + 8]))
        return lines
    printable = all(32 <= b < 127 and b not in (39,) for b in data.rstrip(b'\0')) and data.rstrip(b'\0')
    if printable:
        text = data.rstrip(b'\0')
        zeros = len(data) - len(text)
        line = f"db '{text.decode()}'"
        if zeros:
            line += f', {hexnum(zeros)} dup (0)' if zeros > 1 else ', 0'
        return [line]
    for i in range(0, len(data), 12):
        lines.append('db ' + ', '.join(hexnum(b) for b in data[i:i + 12]))
    return lines


def c_declarations() -> dict[str, str]:
    """The C declaration of each library function, from include/wc1funcs.h."""
    text = re.sub(r'/\*.*?\*/', ' ', open(PROTOTYPES).read(), flags=re.S)
    text = '\n'.join(line for line in text.split('\n') if not line.lstrip().startswith('#'))
    declarations = {}
    for statement in text.split(';'):
        statement = ' '.join(statement.split())
        match = re.search(r'(\w+)\s*\(', statement)
        if match and match.group(1) in {symbol for symbol, _text in NAMES.values()}:
            declarations[match.group(1)] = statement.replace('( ', '(') + ';'
    return declarations


def comment(text: str | None, indent: str = '') -> list[str]:
    return [f'{indent}; {line}'.rstrip() for line in text.split('\n')] if text else []


def render(generator: Generator) -> str:
    image = generator.image
    out = [
        '; @file',
        '; @ingroup graphics',
        '; @brief Hand-written raster library: RLE sprites, clipped lines, fills',
        '; and ellipses, and IFF, PCX and GIF image decoding.',
        ';',
        '; Assembled rather than compiled: its shifts by one use the D1/D0',
        '; encodings and it keeps tables in its code segment. It was linked',
        '; between screens.obj and killbrd.obj: code 439c40-43f635, data',
        '; 46e69c-47008b. FillRasterBytes, just before it, is compiled C at the',
        '; end of src/screens.c.',
        ';',
        '; Extracted from the retail executable by bin/extractRasterLibrary.py,',
        '; which also checks that UASM reproduces its bytes and relocations.',
        '; Regenerate rather than edit: the script holds the names and comments.',
        '',
        '        .386',
        '        .model  flat, c',
        '        option  casemap:none',
        '',
    ]
    publics = [s for a, (s, *_r) in sorted(NAMES.items())]
    publics += [s for a, (s, _u, public, _c) in sorted(DATA_LABELS.items()) if s and public]
    for symbol in publics:
        out.append(f'        public  {symbol}')
    out += ['', '        .code', '']
    span_starts = {start: end for start, end in generator.spans}
    by_address = {insn.address: insn for insn in generator.instructions}
    declarations = c_declarations()
    current = None
    address = CODE[0]
    while address < CODE[1]:
        if address in NAMES:
            if current:
                out += [f'{current} endp', '']
            symbol, text = NAMES[address]
            out.append(f'; Function start: 0x{address:08X}')
            if symbol in declarations:
                declaration = declarations[symbol]
                indent = ' ' * (declaration.index('(') + 1)
                out += ['; ' + line for line in textwrap.wrap(declaration, 76, subsequent_indent=indent)]
            out += comment(text)
            out.append(f'{symbol} proc')
            current = symbol
        if address in span_starts:
            if current:
                out += [f'{current} endp', '']
                current = None
            end = span_starts[address]
            labels = sorted(a for a in DATA_LABELS if address <= a < end)
            for i, label_address in enumerate(labels):
                label_end = labels[i + 1] if i + 1 < len(labels) else end
                symbol, unit, _public, text = DATA_LABELS[label_address]
                out += comment(text)
                lines = emit_bytes(image.read(label_address, label_end - label_address), unit)
                out.append(f'{symbol} ' + lines[0])
                out += [' ' * (len(symbol) + 1) + line for line in lines[1:]]
            out.append('')
            address = end
            continue
        insn = by_address[address]
        if address in generator.labels and address not in NAMES:
            out.append(f'{generator.labels[address]}:')
        text = generator.format(insn)
        if text is None:
            text = 'db ' + ', '.join(hexnum(b) for b in insn.bytes)
            text = f'{text:<40}; {insn.mnemonic} {insn.op_str}'.rstrip()
        out.append('        ' + text)
        address += insn.size
    if current:
        out += [f'{current} endp', '']
    out += ['        .data', '']
    labels = sorted(a for a in DATA_LABELS if DATA[0] <= a < DATA[1])
    for i, label_address in enumerate(labels):
        label_end = labels[i + 1] if i + 1 < len(labels) else DATA[1]
        symbol, unit, _public, text = DATA_LABELS[label_address]
        out += comment(text)
        lines = emit_bytes(image.read(label_address, label_end - label_address), unit)
        prefix = f'{symbol} ' if symbol else '        '
        out.append(prefix + lines[0])
        out += [' ' * len(prefix) + line for line in lines[1:]]
    out += ['', '        end', '']
    return '\n'.join(out)


def coff_sections(path: str):
    data = open(path, 'rb').read()
    count = struct.unpack_from('<H', data, 2)[0]
    symtab, nsyms = struct.unpack_from('<II', data, 8)
    optional = struct.unpack_from('<H', data, 16)[0]
    strings = symtab + nsyms * 18
    symbols = []
    index = 0
    while index < nsyms:
        entry = data[symtab + index * 18:symtab + (index + 1) * 18]
        value, section, _type, _klass, aux = struct.unpack_from('<IhHBB', entry, 8)
        symbols.append((value, section))
        symbols.extend([(0, 0)] * aux)
        index += 1 + aux
    sections = {}
    for i in range(count):
        off = 20 + optional + i * 40
        name = data[off:off + 8].rstrip(b'\0').decode()
        size, raw_ptr, reloc_ptr = struct.unpack_from('<III', data, off + 16)
        nreloc = struct.unpack_from('<H', data, off + 32)[0]
        raw = data[raw_ptr:raw_ptr + size]
        relocs = []
        for r in range(nreloc):
            vaddr, symbol, kind = struct.unpack_from('<IIH', data, reloc_ptr + r * 10)
            relocs.append((vaddr, symbol, kind))
        sections[name] = {'index': i + 1, 'raw': raw, 'relocs': relocs}
    return sections, symbols


def verify(image: Image, obj: str) -> set[int]:
    """Return the instruction addresses whose assembled bytes differ."""
    sections, symbols = coff_sections(obj)
    origins = {sections['.text']['index']: CODE[0], sections['.data']['index']: DATA[0]}
    mismatched = set()
    for name, (lo, hi) in (('.text', CODE), ('.data', DATA)):
        section = sections[name]
        built = bytearray(section['raw'])
        if len(built) != hi - lo:
            raise SystemExit(f'{name}: assembled {len(built)} bytes, original {hi - lo}')
        expected = bytearray(image.read(lo, hi - lo))
        resolved = set()
        for vaddr, symbol, kind in section['relocs']:
            if kind != 6:  # IMAGE_REL_I386_DIR32
                raise SystemExit(f'{name}: unexpected relocation type {kind}')
            value, target_section = symbols[symbol]
            addend = struct.unpack_from('<i', built, vaddr)[0]
            address = origins[target_section] + value + addend
            if lo + vaddr not in image.relocs or image.dword(lo + vaddr) != address:
                mismatched.add(lo + vaddr)
            resolved.update(range(vaddr, vaddr + 4))
        original_relocs = {r - lo for r in image.relocs if lo <= r < hi}
        for r in original_relocs:
            if r not in resolved:
                mismatched.add(lo + r)
        for offset in range(hi - lo):
            if offset not in resolved and built[offset] != expected[offset]:
                mismatched.add(lo + offset)
    return mismatched


def assemble(source: str, obj: str) -> None:
    if not os.path.exists(UASM):
        raise SystemExit(f'{os.path.relpath(UASM, ROOT)} is missing; run make out/uasm/uasm')
    directory = os.path.dirname(obj)
    path = os.path.join(directory, 'raster.asm')
    with open(path, 'w') as handle:
        handle.write(source)
    # Run beside the source so an error file stays in the temporary directory.
    result = subprocess.run([UASM, '-q', '-coff', f'-Fo{obj}', path], capture_output=True, text=True,
                            cwd=directory)
    if result.returncode:
        raise SystemExit(re.sub(r'\x1b\[[0-9;]*m', '', result.stdout + result.stderr))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    parser.add_argument('--check', action='store_true', help='verify src/raster.asm only')
    args = parser.parse_args()
    image = Image(ORIGINAL)
    with tempfile.TemporaryDirectory() as tmp:
        obj = os.path.join(tmp, 'raster.obj')
        if args.check:
            assemble(open(OUTPUT).read(), obj)
            bad = verify(image, obj)
            print(f'{len(bad)} differing bytes' if bad else 'src/raster.asm matches the original')
            return 1 if bad else 0
        # Jumps are written unsized, as MASM 6 sizes them itself; a jump UASM
        # sizes differently gets an explicit size, any other differing
        # instruction is emitted as bytes.
        raw: set[int] = set()
        sized: set[int] = set()
        for _round in range(8):
            generator = Generator(image, raw, sized)
            source = render(generator)
            assemble(source, obj)
            bad = verify(image, obj)
            owners = {insn.address: insn for insn in generator.instructions
                      if any(insn.address <= b < insn.address + insn.size for b in bad)}
            if not bad:
                open(OUTPUT, 'w').write(source)
                print(f'wrote {os.path.relpath(OUTPUT, ROOT)}: {len(generator.instructions)} instructions, '
                      f'{len(sized)} sized jumps, {len(raw)} emitted as bytes')
                return 0
            jumps = {a for a, insn in owners.items()
                     if generator.branch_target(insn) is not None and a not in sized}
            if jumps:
                sized |= jumps
                continue
            if not set(owners) - raw:
                print(f'{len(bad)} differing bytes outside instructions: {sorted(hex(b) for b in bad)[:10]}')
                return 1
            raw |= set(owners)
    print('did not converge')
    return 1


if __name__ == '__main__':
    sys.exit(main())
