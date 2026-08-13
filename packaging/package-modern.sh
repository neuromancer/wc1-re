#!/usr/bin/env bash
set -euo pipefail

version=${1:?usage: package-modern.sh VERSION PLATFORM ARCH BINARY}
platform=${2:?usage: package-modern.sh VERSION PLATFORM ARCH BINARY}
architecture=${3:?usage: package-modern.sh VERSION PLATFORM ARCH BINARY}
binary=${4:?usage: package-modern.sh VERSION PLATFORM ARCH BINARY}

case "$version" in
    *[!A-Za-z0-9._-]*)
        echo "Invalid release version: $version" >&2
        exit 1
        ;;
esac
case "$platform" in
    linux|macos|windows) ;;
    *)
        echo "Unsupported release platform: $platform" >&2
        exit 1
        ;;
esac
case "$architecture" in
    x86_64|arm64) ;;
    *)
        echo "Unsupported release architecture: $architecture" >&2
        exit 1
        ;;
esac

test -f "$binary"
archive="wc1-re-${version}-${platform}-${architecture}-sdl2"
dist_dir=${DIST_DIR:-dist}
stage_dir="${dist_dir}/${archive}"

if test -e "$stage_dir" || test -e "${dist_dir}/${archive}.zip"; then
    echo "Release staging path already exists: $archive" >&2
    exit 1
fi

mkdir -p "$stage_dir"
cp LICENSE "$stage_dir/LICENSE.txt"
cp packaging/README-modern.txt "$stage_dir/README.txt"
cp packaging/THIRD-PARTY-NOTICES.txt \
    "$stage_dir/THIRD-PARTY-NOTICES.txt"
cp third_party/ymfm/LICENSE "$stage_dir/YMFM-LICENSE.txt"

if test "$platform" = windows; then
    executable="$stage_dir/wc1-modern.exe"
else
    executable="$stage_dir/wc1-modern"
fi
cp "$binary" "$executable"
chmod +x "$executable"

if test "$platform" = linux; then
    while read -r name marker path remainder; do
        if test "$marker" != "=>" || test ! -f "$path"; then
            continue
        fi
        case "$name" in
            libSDL2*.so*|liblzo2*.so*|libasan*.so*|libubsan*.so*|\
            libclang_rt.asan*.so*|libstdc++.so*|libgcc_s.so*)
                cp -L "$path" "$stage_dir/$name"
                ;;
        esac
    done < <(ldd "$binary")

    compgen -G "$stage_dir/libSDL2*.so*" >/dev/null
    compgen -G "$stage_dir/liblzo2*.so*" >/dev/null
    patchelf --force-rpath --set-rpath '$ORIGIN' "$executable"
elif test "$platform" = macos; then
    compiler=${MODERN_CXX:-c++}
    runtime_dir=$($compiler --print-runtime-dir)
    while read -r dependency; do
        name=$(basename "$dependency")
        case "$name" in
            libSDL2*.dylib|liblzo2*.dylib|libclang_rt.asan*.dylib)
                source=$dependency
                case "$source" in
                    @rpath/*) source="$runtime_dir/$name" ;;
                esac
                test -f "$source"
                cp -L "$source" "$stage_dir/$name"
                install_name_tool -change "$dependency" \
                    "@executable_path/$name" "$executable"
                install_name_tool -id "@executable_path/$name" \
                    "$stage_dir/$name"
                codesign --force --sign - "$stage_dir/$name"
                ;;
        esac
    done < <(otool -L "$binary" | tail -n +2 | awk '{print $1}')

    compgen -G "$stage_dir/libSDL2*.dylib" >/dev/null
    compgen -G "$stage_dir/liblzo2*.dylib" >/dev/null
    compgen -G "$stage_dir/libclang_rt.asan*.dylib" >/dev/null
    codesign --force --sign - "$executable"
else
    : "${MINGW_PREFIX:?MINGW_PREFIX is required for Windows packaging}"
    while IFS= read -r dependency; do
        test -n "$dependency" || continue
        dependency=$(cygpath -u "$dependency")
        case "$dependency" in
            "$MINGW_PREFIX"/bin/*.dll)
                cp -L "$dependency" "$stage_dir/$(basename "$dependency")"
                ;;
        esac
    done < <(
        ntldd -R "$binary" |
            awk '/=>/ { sub(/^.*=> /, ""); sub(/ \(0x.*$/, ""); print }'
    )

    test -f "$stage_dir/SDL2.dll"
    test -f "$stage_dir/liblzo2-2.dll"
    compgen -G "$stage_dir/*asan*.dll" >/dev/null
fi

(
    cd "$stage_dir"
    zip -9 -r "../${archive}.zip" .
)
test -s "${dist_dir}/${archive}.zip"
