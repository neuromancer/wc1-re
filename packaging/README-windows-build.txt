Windows installer packaging
===========================

package-windows.sh compiles an x86_64 setup executable with NSIS 3.13, an
open-source installer system: https://nsis.sourceforge.io/

The installer uses the current user's LocalAppData\Programs\wc1-re folder
by default, adds a Start Menu shortcut, and registers an uninstaller in
Windows Settings. It does not request administrator privileges. Upgrades
run the previous uninstaller before installing the new files. Uninstallers
remove only their packaged files, shortcut, and registry entry; game data,
saves, and launcher preferences remain separate.

The Windows release job installs NSIS 3.13.0 through Chocolatey, builds the
native GUI, and stages its runtime libraries with package-modern.sh. It then
passes that same staging directory to the installer packager:

    packaging/package-windows.sh "$version" x86_64 "dist/wc1-re-$version-windows-x86_64-sdl2"

The payload includes the SWC sound bank, Spanish catalog, and license/readme files
from the portable package, in addition to the executable and runtime DLLs.

MAKENSIS can point to makensis.exe when it is not on PATH. The wrapper runs
under MSYS2 on Windows and also supports native NSIS compilers on other hosts.
It requires Bash and the standard shell tools used by the existing packager.
DIST_DIR overrides the output directory; existing installers are not replaced.

The result is dist/wc1-re-VERSION-windows-x86_64-setup.exe. NSIS treats
compiler warnings as errors. No installer or game is launched during packaging.
The setup executable is unsigned. Game data is not included.

Tagged releases publish the installer and its SHA-256 alongside the portable
ZIPs and macOS DMGs. Manual workflow runs produce unpublished artifacts.
