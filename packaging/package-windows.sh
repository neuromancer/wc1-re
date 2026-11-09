#!/usr/bin/env bash
# Compile an NSIS installer from package-modern.sh's Windows staging directory.
# See the main README for dependencies and usage.
set -euo pipefail

version=${1:?usage: package-windows.sh VERSION ARCH PACKAGE_DIRECTORY}
architecture=${2:?usage: package-windows.sh VERSION ARCH PACKAGE_DIRECTORY}
package_dir=${3:?usage: package-windows.sh VERSION ARCH PACKAGE_DIRECTORY}
if [[ "$version" =~ ^v?([0-9]+\.[0-9]+\.[0-9]+)([-.][A-Za-z0-9.-]+)?$ ]]; then
    numeric_version=${BASH_REMATCH[1]}.0
elif [[ "$version" =~ ^ci-[0-9]+-[0-9]+$ ]]; then
    numeric_version=0.0.0.0
else
    echo "Expected a release version or a ci-RUN-ATTEMPT artifact version." >&2
    exit 1
fi
if test "$architecture" != x86_64; then
    echo "The Windows installer currently supports x86_64 only." >&2
    exit 1
fi
package_dir=$(cd "$package_dir" && pwd)
for required in wc1-modern.exe SDL2.dll liblzo2-2.dll swc-music.sf2 \
                translations/es.dat translations/ja.dat voices/segacd.dat SPANISH-TRANSLATION-NOTICE.txt; do
    if test ! -f "$package_dir/$required"; then
        echo "Missing Windows release file: $package_dir/$required" >&2
        exit 1
    fi
done

root=$(cd "$(dirname "$0")/.." && pwd)
compiler=${MAKENSIS:-makensis}
script="$root/packaging/windows-installer.nsi"
option_prefix=-
path_separator=/
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*)
        compiler=$(cygpath -u "$compiler")
        script=$(cygpath -m "$script")
        option_prefix=/
        path_separator='\'
        ;;
esac
if ! command -v "$compiler" >/dev/null; then
    echo "Install NSIS 3.13 or set MAKENSIS to the compiler executable." >&2
    exit 1
fi
dist_dir=${DIST_DIR:-"$root/dist"}
mkdir -p "$dist_dir"
dist_dir=$(cd "$dist_dir" && pwd)
installer="$dist_dir/wc1-re-${version}-windows-${architecture}-setup.exe"
if test -e "$installer"; then
    echo "Installer already exists: $installer" >&2
    exit 1
fi
work=$(mktemp -d "$dist_dir/.windows-package.XXXXXX")
cleanup() {
    rm -rf "$work"
}
trap cleanup EXIT

mkdir "$work/payload"
cp "$package_dir/wc1-modern.exe" "$package_dir/"*.dll "$work/payload/"
cp "$package_dir/"*.txt "$package_dir/swc-music.sf2" "$work/payload/"
cp -R "$package_dir/translations" "$work/payload/"
mkdir "$work/payload/voices"
cp "$package_dir/voices/segacd.dat" "$work/payload/voices/"

# Generate exact install/uninstall lists from the same payload. Never remove
# arbitrary DLLs, game data, saves, or launcher preferences during uninstall.
for file in "$work/payload/"* "$work/payload/translations/"* "$work/payload/voices/"*; do
    test -f "$file" || continue
    name=${file#"$work/payload/"}
    case "$name" in
        *[!A-Za-z0-9._+/-]*)
            echo "Unsupported package filename: $name" >&2
            exit 1
            ;;
    esac
    windows_name=${name//\//\\}
    # dirname on the build host does not recognize Windows separators.
    case "$name" in
        */*) windows_directory=${windows_name%\\*} ;;
        *) windows_directory= ;;
    esac
    source_name=${name//\//$path_separator}
    printf 'SetOutPath "$INSTDIR\\%s"\n' "$windows_directory" >> "$work/install-files.nsh"
    printf 'File "payload%s%s"\n' "$path_separator" "$source_name" >> "$work/install-files.nsh"
    printf 'Delete "$INSTDIR\\%s"\n' "$windows_name" >> "$work/uninstall-files.nsh"
done
# Remove only empty directories, after checking deletion errors. User-added
# translation files are preserved and must not make uninstall fail.
printf 'RMDir "$INSTDIR\\translations"\n' >> "$work/uninstall-directories.nsh"
printf 'RMDir "$INSTDIR\\voices"\n' >> "$work/uninstall-directories.nsh"
installed_size=$(du -sk "$work/payload" | awk '{print $1}')
(
    cd "$work"
    # Native NSIS receives Windows options and paths without MSYS rewriting.
    MSYS2_ARG_CONV_EXCL='*' "$compiler" \
        "${option_prefix}NOCONFIG" "${option_prefix}NOCD" \
        "${option_prefix}WX" "${option_prefix}V3" \
        "${option_prefix}DWC1_VERSION=$version" \
        "${option_prefix}DWC1_NUMERIC_VERSION=$numeric_version" \
        "${option_prefix}DWC1_INSTALLED_SIZE=$installed_size" \
        "$script"
)
test -s "$work/installer.exe"
mv "$work/installer.exe" "$installer"
echo "Created: $installer"
