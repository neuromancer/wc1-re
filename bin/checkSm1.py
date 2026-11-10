#!/usr/bin/env python3
"""Run SM1 integration checks with disposable copies of user-supplied assets."""
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

probe, installation = map(Path, sys.argv[1:])
assets = installation / "GAMEDAT"
if not assets.is_dir():
    assets = installation / "gamedat"
if not assets.is_dir():
    raise SystemExit("Supply a Kilrathi Saga installation with SM1 using SM1_DATA=...")

with tempfile.TemporaryDirectory(prefix="wc1-sm1-") as scratch:
    # Keep player saves/configuration untouched; SM2 must not be required.
    shutil.copytree(assets, Path(scratch) / "GAMEDAT", ignore=lambda _, names: [
        name for name in names if name.upper() in {
            "SAVEGAME.WLD", "WINGCMDR.CFG", "TITLE1.VGA",
            "MODULE.002", "CAMP.002", "BRIEFING.002",
        }
    ])
    streams = next(path for path in installation.iterdir() if path.name.upper() == "STREAMS")
    shutil.copytree(streams, Path(scratch) / "STREAMS")
    subprocess.run([str(probe.resolve()), scratch], check=True, timeout=600,
                   env=dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy",
                            ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="halt_on_error=1"))
