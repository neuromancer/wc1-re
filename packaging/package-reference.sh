#!/usr/bin/env bash
set -euo pipefail

version=${1:?usage: package-reference.sh VERSION}
case "$version" in
    *[!A-Za-z0-9._-]*)
        echo "Invalid release version: $version" >&2
        exit 1
        ;;
esac

test -f WC1.EXE
archive="wc1-re-${version}-win32-msvc420"
dist_dir=${DIST_DIR:-dist}
stage_dir="${dist_dir}/${archive}"

if test -e "$stage_dir" || test -e "${dist_dir}/${archive}.zip"; then
    echo "Release staging path already exists: $archive" >&2
    exit 1
fi

mkdir -p "$stage_dir"
cp WC1.EXE "$stage_dir/WC1.EXE"
cp LICENSE "$stage_dir/LICENSE.txt"
cp packaging/README-win32.txt "$stage_dir/README.txt"

(
    cd "$stage_dir"
    zip -9 -r "../${archive}.zip" .
)
test -s "${dist_dir}/${archive}.zip"
