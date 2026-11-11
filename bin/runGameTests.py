#!/usr/bin/env python3
"""Run SDL2 regressions or seeded flight tests using disposable game data."""
import argparse
import hashlib
import os
from pathlib import Path
import secrets
import shlex
import shutil
import subprocess
import sys
import tempfile


def positive(value):
    number = int(value)
    if number < 1:
        raise argparse.ArgumentTypeError("must be positive")
    return number


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("probe", type=Path)
    parser.add_argument("data", type=Path, help="Kilrathi Saga installation containing GAMEDAT and STREAMS")
    parser.add_argument("--campaign", choices=("wc1", "sm1", "sm2", "all"), default="all")
    parser.add_argument("--random", action="store_true", help="run the flight probe with seeded controls")
    parser.add_argument("--seed", type=int, help="32-bit seed; randomly chosen and printed if omitted")
    parser.add_argument("--runs", type=positive, default=1, help="consecutive seeds to exercise")
    parser.add_argument("--ticks", type=positive, default=600, help="maximum flight ticks per nav point (1..3600)")
    parser.add_argument("--case", help="replay one mission, e.g. wc1:1:0 (mission is zero-based)")
    parser.add_argument("--timeout", type=positive, help="seconds per child process")
    parser.add_argument("--log", type=Path, help="random test log; defaults to a unique file beside the probe")
    args = parser.parse_args()
    if args.ticks > 3600 or args.seed is not None and not 0 <= args.seed <= 0xffffffff:
        parser.error("ticks must be in 1..3600 and seed in 0..4294967295")
    if not args.random and args.campaign not in ("sm1", "sm2"):
        parser.error("regression probes require --campaign sm1 or sm2")
    probe = args.probe.resolve()
    if not probe.is_file():
        parser.error(f"build the probe first: {probe}")
    installation = args.data.resolve()
    directories = {path.name.upper(): path for path in installation.iterdir() if path.is_dir()}
    if not {"GAMEDAT", "STREAMS"} <= directories.keys():
        parser.error("supply a Kilrathi Saga installation containing GAMEDAT and STREAMS")
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy",
               ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
    timeout = args.timeout or (120 if args.random else
                              1800 if os.environ.get("WC1_CAMPAIGN_FULL_SCENES") else 600)

    with tempfile.TemporaryDirectory(prefix="wc1-test-") as scratch:
        root = Path(scratch)
        excluded = {"SAVEGAME.WLD", "WINGCMDR.CFG", "PROBE-COMPLETE"}
        if not args.random and args.campaign == "sm1":
            excluded.update({"TITLE1.VGA", "MODULE.002", "CAMP.002", "BRIEFING.002"})
        shutil.copytree(directories["GAMEDAT"], root / "GAMEDAT",
                        ignore=lambda _, names: [name for name in names if name.upper() in excluded])
        shutil.copytree(directories["STREAMS"], root / "STREAMS")
        complete = root / "GAMEDAT" / "probe-complete"

        def run(arguments, output=None):
            # Original fatal game errors can call exit(0).
            complete.unlink(missing_ok=True)
            result = subprocess.run([str(probe), scratch, *arguments], env=env, timeout=timeout,
                                    stdout=output, stderr=subprocess.STDOUT if output else None,
                                    text=True, check=True)
            if not complete.is_file():
                raise RuntimeError("probe exited before completing its checks")
            return result

        if not args.random:
            run(["sm2"] if args.campaign == "sm2" else [])
            return

        names = ("wc1", "sm1", "sm2")
        cases = []
        for line in run(["--list"], subprocess.PIPE).stdout.splitlines():
            if line.startswith("CASE "):
                campaign, series, mission = map(int, line.split()[1:])
                label = f"{names[campaign]}:{series}:{mission}"
                if args.campaign in ("all", names[campaign]) and args.case in (None, label):
                    cases.append((label, campaign, series, mission))
        if not cases:
            parser.error("no matching missions; check campaign data and --case")
        seed = args.seed if args.seed is not None else secrets.randbits(32)
        if args.log:
            log_path = args.log.resolve()
            log_path.parent.mkdir(parents=True, exist_ok=True)
            log = log_path.open("w")
        else:
            log = tempfile.NamedTemporaryFile(mode="w", prefix=f"flight-{seed}-", suffix=".log",
                                              dir=probe.parent, delete=False)
            log_path = Path(log.name)
        with log:
            print(f"Seed {seed}; {len(cases)} missions × {args.runs} runs; log: {log_path}", flush=True)
            log.write(f"probe sha256={hashlib.sha256(probe.read_bytes()).hexdigest()}\n")
            for run_index in range(args.runs):
                run_seed = (seed + run_index) & 0xffffffff
                for label, campaign, series, mission in cases:
                    replay = [sys.executable, str(Path(__file__).resolve()), str(probe), str(installation),
                              "--random", "--case", label, "--seed", str(run_seed),
                              "--ticks", str(args.ticks), "--timeout", str(timeout)]
                    log.write(f"\nREPLAY {shlex.join(replay)}\n")
                    log.flush()
                    print(f"RUN seed={run_seed} {label}", flush=True)
                    try:
                        run(list(map(str, (campaign, series, mission, run_seed, args.ticks))), log)
                    except (subprocess.SubprocessError, RuntimeError) as error:
                        log.flush()
                        print("\n".join(log_path.read_text(errors="replace").splitlines()[-45:]), file=sys.stderr)
                        raise RuntimeError(f"{error}\nReplay: {shlex.join(replay)}\nLog: {log_path}") from error
            print(f"PASS: {len(cases) * args.runs} seeded mission runs; log: {log_path}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        if isinstance(error, subprocess.CalledProcessError) and error.stdout:
            print(error.stdout, file=sys.stderr, end="")
        raise SystemExit(str(error)) from error
