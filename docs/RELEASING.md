# Releases

GitHub Actions builds release archives only for a pushed semantic-version tag.
Ordinary branch pushes and commits do not run the release workflow.

To test all release builds without publishing anything, run the **Release**
workflow manually from the Actions page. A manual run keeps its ZIP files as
short-lived workflow artifacts and never creates a GitHub Release, even when
the selected ref is a tag.

To publish a release, create and push a strict `vMAJOR.MINOR.PATCH` tag:

```sh
git tag -a v0.1.0 -m "v0.1.0"
git push origin v0.1.0
```

After every platform build succeeds, the workflow creates one release for the
existing tag, generates release notes, attaches SHA-256 checksums, and uploads:

- the reconstructed 32-bit `WC1.EXE` built with MSVC 4.20;
- the SDL2 port for Windows x86-64;
- the SDL2 port for Linux x86-64;
- the SDL2 port for macOS x86-64; and
- the SDL2 port for macOS arm64.

No retail executable, disc image, installed game data, save file, or local
configuration is included. SDL2 development builds retain the project's
AddressSanitizer and UndefinedBehaviorSanitizer instrumentation; release jobs
use `MODERN_RELEASE=1` and verify that published binaries do not contain or
link sanitizer runtimes.
