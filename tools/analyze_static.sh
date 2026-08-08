#!/usr/bin/env bash
set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/analysis/out"
MODE="${1:-all}"

usage() {
  cat <<'EOF'
Usage:
  tools/analyze_static.sh [all]
  tools/analyze_static.sh src/main.c [src/Other.cpp ...]

Runs Clang Static Analyzer over the reconstructed source using analysis-only
Win32/MSVC compatibility headers. Generated logs are written to analysis/out/
and are intentionally ignored by git.
EOF
}

case "$MODE" in
  -h|--help|help)
    usage
    exit 0
    ;;
esac

mkdir -p "$OUT"

COMMON_FLAGS=(
  --analyze
  -w
  -fms-extensions
  -DWC1_ANALYSIS
  -D'__declspec(x)='
  -I"$ROOT/analysis/include"
  -iquote "$ROOT/src" -iquote "$ROOT/include"
  -include "$ROOT/analysis/include/analysis_prelude.h"
  -Xclang -analyzer-output=text
)

case "$MODE" in
  all)
    FILES=()
    while IFS= read -r file; do
      FILES+=("$file")
    done < <(find "$ROOT/src" -maxdepth 2 \( -name '*.cpp' -o -name '*.c' \) | sort)
    ;;
  *)
    FILES=("$@")
    ;;
esac

if [ "${#FILES[@]}" -eq 0 ]; then
  printf 'error: no input files\n' >&2
  exit 1
fi

LOG_NAME="${MODE//[^A-Za-z0-9_.-]/_}"
LOG="$OUT/static-$LOG_NAME.log"
: > "$LOG"

ok=0
fail=0

for file in "${FILES[@]}"; do
  if [ ! -f "$file" ]; then
    file="$ROOT/$file"
  fi
  if [ ! -f "$file" ]; then
    printf 'error: input file not found: %s\n' "$file" | tee -a "$LOG" >&2
    fail=$((fail + 1))
    continue
  fi

  rel="${file#$ROOT/}"
  printf '[static] %s\n' "$rel" | tee -a "$LOG"
  input="$file"
  if grep -q '^[[:space:]]*#pragma[[:space:]]\+data_seg' "$file"; then
    tmp="$OUT/tmp/${rel//\//_}"
    mkdir -p "$(dirname "$tmp")"
    awk -v src="$file" 'BEGIN { printf("#line 1 \"%s\"\n", src) }
      /^[[:space:]]*#pragma[[:space:]]+data_seg/ { print ""; next }
      { print }' "$file" > "$tmp"
    input="$tmp"
  fi
  CXX_OR_CC=clang++; LANG_FLAGS=(-std=c++98)
  case "$input" in *.c) CXX_OR_CC=clang; LANG_FLAGS=(-std=c89);; esac
  if "$CXX_OR_CC" "${COMMON_FLAGS[@]}" "${LANG_FLAGS[@]}" "$input" >> "$LOG" 2>&1; then
    ok=$((ok + 1))
  else
    fail=$((fail + 1))
  fi
done

printf '\nchecked=%d ok=%d failed=%d log=%s\n' "$((ok + fail))" "$ok" "$fail" "$LOG" | tee -a "$LOG"

if [ "$fail" -ne 0 ]; then
  exit 1
fi
