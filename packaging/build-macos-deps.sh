#!/usr/bin/env bash
set -euo pipefail

# Keep the macOS 15 runtime baseline when compiling on a newer CI image.
# Pin SDL2 and LZO because newer Homebrew bottles can require the runner's
# OS or replace SDL2 with an SDL3 shim.
prefix=${1:?usage: build-macos-deps.sh ABSOLUTE_PREFIX}
case "$prefix" in
    /*) ;;
    *) echo "The dependency prefix must be absolute." >&2; exit 1 ;;
esac
test "$(uname -s)" = Darwin
export MACOSX_DEPLOYMENT_TARGET=${MACOSX_DEPLOYMENT_TARGET:-15.0}
export CC=${MODERN_CC:-clang}
export CXX=${MODERN_CXX:-clang++}

mkdir -p "$prefix/build"
cd "$prefix/build"
curl -fsSL --retry 3 --connect-timeout 15 --max-time 180 \
    -o SDL2-2.32.10.tar.gz \
    https://github.com/libsdl-org/SDL/releases/download/release-2.32.10/SDL2-2.32.10.tar.gz
curl -fsSL --retry 3 --connect-timeout 15 --max-time 180 \
    -o lzo-2.10.tar.gz \
    https://www.oberhumer.com/opensource/lzo/download/lzo-2.10.tar.gz
shasum -a 256 -c <<'CHECKSUMS'
5f5993c530f084535c65a6879e9b26ad441169b3e25d789d83287040a9ca5165  SDL2-2.32.10.tar.gz
c0f892943208266f9b6543b3ae308fab6284c5c90e627931446fb49b4221a072  lzo-2.10.tar.gz
CHECKSUMS

tar -xzf SDL2-2.32.10.tar.gz
tar -xzf lzo-2.10.tar.gz
(
    cd SDL2-2.32.10
    ./configure --prefix="$prefix" --enable-shared --disable-static \
        --enable-hidapi --without-x
    make -j2
    make install
)
(
    cd lzo-2.10
    ./configure --prefix="$prefix" --enable-shared --disable-static \
        --disable-dependency-tracking
    make -j2
    make install
)
