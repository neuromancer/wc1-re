macOS application packaging
==========================

package-macos.sh wraps the integrated SDL2 launcher in Wing Commander.app
and creates a disk image with an Applications shortcut. It bundles SDL2,
LZO2, and the SWC demo sound bank, includes all existing license notices, and
uses ad-hoc code signing. The app is not notarized. No game data is included.

Run on macOS with the command-line developer tools, Python 3.10+, and the
normal modern-gui build dependencies (CMake, Rust, and pkg-config).
From the repository root, prepare libraries targeting macOS 15:

    export MACOSX_DEPLOYMENT_TARGET=15.0
    packaging/build-macos-deps.sh "$PWD/out/macos-deps"
    export PATH="$PWD/out/macos-deps/bin:$PATH"
    export PKG_CONFIG_PATH="$PWD/out/macos-deps/lib/pkgconfig"

Build into a fresh directory so development objects with sanitizer
instrumentation are not reused:

    make -j2 modern-gui MODERN_RELEASE=1 MODERN_OUT_DIR=out/macos-release

Install the packaging tools separately and create the disk image:

    python3 -m venv out/macos-packaging-venv
    out/macos-packaging-venv/bin/python -m pip install -r packaging/requirements-macos.txt
    export DMGBUILD="$PWD/out/macos-packaging-venv/bin/dmgbuild"
    packaging/package-macos.sh 0.9.4-local "$(uname -m)" out/macos-release/wc1-modern-gui

The result is written to dist/. DIST_DIR overrides the output directory;
an existing image is never overwritten. Both arm64 and x86_64 are supported,
using an executable and dependencies built for the selected architecture.
The layout is generated without Finder automation or launching the app.
The completed disk image is mounted read-only to verify the app's signature
after packaging, then detached automatically.
The sound bank and license files live in Contents/Resources, where SDL looks
for bundled resources. Game data stays in the directory chosen in the launcher.

The release workflow builds one DMG for each macOS architecture alongside
the portable ZIPs. Only pushing a release tag publishes those artifacts;
manual workflow runs retain them as unpublished build artifacts.
