#!/usr/bin/env python3
"""Run expansion integration checks with disposable user-supplied assets."""
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

probe, installation = map(Path, sys.argv[1:3])
sm2 = sys.argv[3:] == ["sm2"]
assets = installation / "GAMEDAT"
if not assets.is_dir():
    assets = installation / "gamedat"
if not assets.is_dir():
    raise SystemExit("Supply a Kilrathi Saga installation using SM1_DATA/SM2_DATA=...")

with tempfile.TemporaryDirectory(prefix="wc1-sm2-" if sm2 else "wc1-sm1-") as scratch:
    # Keep player saves/configuration untouched; SM2 must not be required.
    shutil.copytree(assets, Path(scratch) / "GAMEDAT", ignore=lambda _, names: [
        name for name in names if name.upper() in {"SAVEGAME.WLD", "WINGCMDR.CFG"} or
        not sm2 and name.upper() in {"TITLE1.VGA",
            "MODULE.002", "CAMP.002", "BRIEFING.002",
        }
    ])
    streams = next(path for path in installation.iterdir() if path.name.upper() == "STREAMS")
    shutil.copytree(streams, Path(scratch) / "STREAMS")
    subprocess.run([str(probe.resolve()), scratch, *sys.argv[3:]], check=True,
                   timeout=1800 if os.environ.get("WC1_CAMPAIGN_FULL_SCENES") else 600,
                   env=dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy",
                            ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1"))
    # Original fatal game errors can call exit(0); require the final check too.
    if not (Path(scratch) / "GAMEDAT" / "probe-complete").is_file():
        raise SystemExit("Expansion probe exited before completing its checks")
