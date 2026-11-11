#!/usr/bin/env bash
set -euo pipefail

version=${1:?usage: package-flatpak.sh VERSION PORTABLE_DIRECTORY}
portable=${2:?usage: package-flatpak.sh VERSION PORTABLE_DIRECTORY}
case "$version" in
    *[!A-Za-z0-9._-]*) echo "Invalid release version: $version" >&2; exit 1 ;;
esac
test "$(uname -m)" = x86_64
test -x "$portable/wc1-modern"

# Reuse the Linux release payload, including its libraries, catalogs and notices.
# No original game data or recordings are included in that payload.
app_id=io.github.neuromancer.wc1
runtime_version=25.08
dist_dir=${DIST_DIR:-dist}
build_dir="$dist_dir/flatpak-$version"
bundle="$dist_dir/wc1-re-$version-linux-x86_64-experimental.flatpak"
if test -e "$build_dir" || test -e "$bundle"; then
    echo "Flatpak staging path already exists: $version" >&2
    exit 1
fi

flatpak remote-add --user --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
flatpak install --user --noninteractive --assumeyes --arch=x86_64 flathub \
    "org.freedesktop.Platform//$runtime_version" "org.freedesktop.Sdk//$runtime_version"
flatpak build-init --arch=x86_64 "$build_dir" "$app_id" \
    org.freedesktop.Sdk org.freedesktop.Platform "$runtime_version"
files="$build_dir/files"
mkdir -p "$files/lib/wc1" "$files/bin"
cp -a "$portable/." "$files/lib/wc1/"
cat > "$files/bin/wc1-modern" <<'EOF'
#!/bin/sh
exec /app/lib/wc1/wc1-modern "$@"
EOF
chmod +x "$files/bin/wc1-modern"
install -Dm644 packaging/flatpak/app.desktop "$files/share/applications/$app_id.desktop"
install -Dm644 packaging/flatpak/icon.svg "$files/share/icons/hicolor/scalable/apps/$app_id.svg"
mkdir -p "$files/share/metainfo"
sed -e "s/@VERSION@/${version#v}/g" -e "s/@DATE@/$(date -u +%F)/g" \
    packaging/flatpak/metainfo.xml > "$files/share/metainfo/$app_id.metainfo.xml"

# Check the prebuilt executable against the end-user runtime in CI, not the SDK.
flatpak build --runtime "$build_dir" ldd /app/lib/wc1/wc1-modern > "$build_dir/dependencies.txt" 2>&1
cat "$build_dir/dependencies.txt"
if grep -q 'not found' "$build_dir/dependencies.txt"; then
    echo "Linux release has dependencies missing from the Flatpak runtime." >&2
    exit 1
fi

# Home and removable media contain user-selected installations and WC1 saves.
# Device access also supports controllers on older SteamOS Flatpak versions.
flatpak build-finish "$build_dir" --command=wc1-modern \
    --share=ipc --socket=x11 --socket=wayland --socket=pulseaudio --device=all \
    --filesystem=home --filesystem=/run/media --filesystem=/media
flatpak build-export "$build_dir/repo" "$build_dir" experimental
flatpak build-bundle --arch=x86_64 \
    --runtime-repo=https://dl.flathub.org/repo/flathub.flatpakrepo \
    "$build_dir/repo" "$bundle" "$app_id" experimental
test -s "$bundle"
