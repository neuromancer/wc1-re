# Producing `code-full/`

`code-full/` holds the original disassembly that every binary-comp comparison reads. Files
are named by **entry address**, not by symbol:

```
code-full/FUN_0041D0C0.disassembled.txt
    Function: MinShort
    Address: 0x0041D0C0

    MOV CX,word ptr [ESP + 0x8]
    ...
```

Internal branch targets appear as `LAB_%08X:` labels, direct calls as `CALL 0x%08X`, and
in-body jumps as `Jxx      LAB_%08X`.

There are two ways to produce it, and they are complementary.

## 1. `make export-asm` — from the PE, no Ghidra needed

```sh
make export-asm
```

binary-comp disassembles the original executable with Capstone and writes exports for every
function it can identify. By default it takes its targets from the
`/* Function start: 0xADDR */` annotations already present in `src/`, which is exactly what
you want while implementing: annotate a function, run `make export-asm`, and its export
appears.

Useful flags via `EXPORT_ASM_FLAGS`:

```sh
make export-asm EXPORT_ASM_FLAGS=--discover   # also find starts from calls/jumps/prologues
make export-asm EXPORT_ASM_FLAGS=--clean      # drop stale exports first
make export-asm EXPORT_ASM_FLAGS="--map WC1.ORIGINAL.map"   # if an original map ever surfaces
```

Limitation: it only knows addresses, so the `Function:` header carries whatever name the
source annotation implies. It has no access to the 1,450 names in the Ghidra database.

## 2. `ExportToCompile.java` — from Ghidra, with names

The canonical exporter ships with binary-comp:

```
binary-comp/ghidra_scripts/ExportToCompile.java
```

Run it from Ghidra's **Script Manager** with `WC1.EXE` open (add
`binary-comp/ghidra_scripts` to the script directories first). It exports, for every
function:

- `FUN_XXXXXXXX.disassembled.txt` — disassembly, with the Ghidra function name in the header
- `FUN_XXXXXXXX.decompiled.txt` — decompiler output, used by the call checks
- `globals.h` — the global data inventory `verify-globals` compares against
- `strings.txt` — the address-to-string map

Point its output at this repo's `code-full/`.

**This is the export you want for real work**, because it carries the recovered names and
because `verify-globals` / `verify-calls` need `globals.h` and the decompiled text, which
`export-asm` does not produce.

Note: the Ghidra MCP server in this environment has script execution disabled
(`GHIDRA_MCP_ALLOW_SCRIPTS` is unset), so the script has to be run from Ghidra's UI rather
than driven remotely.

## Which comparisons need what

| Command | Needs |
|---|---|
| `make compare-func FUNC=X` | `FUN_<addr>.disassembled.txt`, linked `WC1.EXE` + `WC1.map` |
| `make report` | same, for every annotated function |
| `make order` | same |
| `make verify-calls` | additionally the `.decompiled.txt` exports |
| `make verify-globals` | additionally `code-full/globals.h` |
| `make compare` / `compare-functions` | `data/full/WC1.ORI.EXE` and a linked `WC1.EXE` |

Everything above also requires the project to **link**. That is why `src/stubs.c` exists: a
`WinMain` stub so `WinMainCRTStartup` resolves and a real PE + map get produced from day one.
Replace stubs with recovered implementations as they land; do not add new ones without need.
