#!/usr/bin/env python3
"""Run SDL2 regressions or libFuzzer flight tests using disposable game data."""
import argparse
import hashlib
import os
from pathlib import Path
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
    parser.add_argument("--campaign", choices=("wc1", "sm1", "sm2", "sim", "all"), default="all",
                        help="sim is the training simulator")
    parser.add_argument("--fuzz", action="store_true", help="run libFuzzer; pass its flags after --")
    parser.add_argument("--ticks", type=positive, default=600, help="maximum flight ticks per input (1..3600)")
    parser.add_argument("--case", help="restrict to one mission, e.g. wc1:1:0 or sim:0:2 (mission is zero-based)")
    parser.add_argument("--timeout", type=positive, help="seconds per input or regression process")
    parser.add_argument("--corpus", type=Path, help="persistent input corpus; defaults beside the fuzzer")
    parser.add_argument("--replay", type=Path, help="replay a saved libFuzzer input without mutations")
    arguments = sys.argv[1:]
    split = arguments.index("--") if "--" in arguments else len(arguments)
    args = parser.parse_args(arguments[:split])
    fuzz_flags = arguments[split + 1:]
    if any(not flag.startswith("-") for flag in fuzz_flags):
        parser.error("use --corpus or --replay for input paths; only libFuzzer flags follow --")
    if args.ticks > 3600:
        parser.error("ticks must be in 1..3600")
    if not args.fuzz and (args.campaign not in ("sm1", "sm2") or fuzz_flags or args.replay or args.corpus or args.case):
        parser.error("regression probes require --campaign sm1 or sm2; flight options require --fuzz")
    probe = args.probe.resolve()
    if not probe.is_file():
        parser.error(f"build the probe first: {probe}")
    installation = args.data.resolve()
    directories = {path.name.upper(): path for path in installation.iterdir() if path.is_dir()}
    if not {"GAMEDAT", "STREAMS"} <= directories.keys():
        parser.error("supply a Kilrathi Saga installation containing GAMEDAT and STREAMS")
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy",
               ASAN_OPTIONS="detect_leaks=0", UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
    timeout = args.timeout or (120 if args.fuzz else
                              1800 if os.environ.get("WC1_CAMPAIGN_FULL_SCENES") else 600)
    names = ("wc1", "sm1", "sm2", "sim")
    if args.fuzz:
        env["UBSAN_OPTIONS"] = "halt_on_error=0:print_stacktrace=1"
        env.pop("WC1_FLIGHT_CASE", None)
        env["WC1_FLIGHT_CAMPAIGN"] = str(names.index(args.campaign) if args.campaign != "all" else -1)
        env["WC1_FLIGHT_TICKS"] = str(args.ticks)
        if args.case:
            try:
                campaign, series, mission = args.case.split(":")
                series, mission = int(series), int(mission)
                if campaign not in names or not 0 <= mission < 256:
                    raise ValueError
                if campaign == "sim" and (series != 0 or mission > 3):
                    raise ValueError
                if campaign != "sim" and not 1 <= series <= 13:
                    raise ValueError
                if args.campaign not in ("all", campaign):
                    raise ValueError
            except ValueError:
                parser.error("--case must be campaign:series:mission, matching --campaign")
            env["WC1_FLIGHT_CASE"] = f"{names.index(campaign)}:{series}:{mission}"
        if args.replay and not args.replay.is_file():
            parser.error(f"replay input does not exist: {args.replay}")

    with tempfile.TemporaryDirectory(prefix="wc1-test-") as scratch:
        root = Path(scratch)
        excluded = {"SAVEGAME.WLD", "WINGCMDR.CFG", "PROBE-COMPLETE"}
        if not args.fuzz and args.campaign == "sm1":
            excluded.update({"TITLE1.VGA", "MODULE.002", "CAMP.002", "BRIEFING.002"})
        shutil.copytree(directories["GAMEDAT"], root / "GAMEDAT",
                        ignore=lambda _, names: [name for name in names if name.upper() in excluded])
        shutil.copytree(directories["STREAMS"], root / "STREAMS")
        complete = root / "GAMEDAT" / "probe-complete"

        if not args.fuzz:
            # Original fatal game errors can call exit(0).
            subprocess.run([str(probe), scratch, *(["sm2"] if args.campaign == "sm2" else [])],
                           env=env, timeout=timeout, check=True)
            if not complete.is_file():
                raise RuntimeError("probe exited before completing its checks")
            return

        env["WC1_TEST_ROOT"] = scratch
        inventory = subprocess.run([str(probe)], env=dict(env, WC1_FLIGHT_LIST="1"),
                                   timeout=timeout, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                   text=True, check=True)
        if not complete.is_file():
            raise RuntimeError("flight inventory exited before completing its checks")
        lines = inventory.stdout.splitlines()
        installed = {int(line.split()[1]) for line in lines if line.startswith("INSTALLED ")}
        cases = [tuple(map(int, line.split()[1:])) for line in lines if line.startswith("CASE ")]
        selected = range(len(names)) if args.campaign == "all" else [names.index(args.campaign)]
        if args.campaign != "all" and not installed.issuperset(selected):
            parser.error(f"{args.campaign} not detected: GAMEDAT needs its files and INSTALL.DAT entries")
        print("Flight missions: " + ", ".join(
            f"{names[index]} {sum(case[0] == index for case in cases)}" if index in installed
            else f"{names[index]} not detected" for index in selected), flush=True)
        if not cases:
            parser.error("no matching missions; check --case")
        env.pop("WC1_FLIGHT_LIST", None)
        corpus = (args.corpus or probe.parent / "flight-corpus").resolve()
        artifacts = probe.parent / "flight-artifacts"
        artifacts.mkdir(parents=True, exist_ok=True)
        if args.replay:
            header = args.replay.read_bytes()[:3]
            if tuple(header) not in cases:
                parser.error("replay input does not match the selected installed missions")
            inputs = [str(args.replay.resolve())]
        else:
            corpus.mkdir(parents=True, exist_ok=True)
            # The probe adds small seeds for every selected mission and start leg.
            env["WC1_FLIGHT_SEEDS"] = str(corpus)
            inputs = [str(corpus)]
        command = [str(probe), *inputs, f"-max_len={8 + 2 * args.ticks}",
                   f"-timeout={timeout}", "-detect_leaks=0", f"-artifact_prefix={artifacts}{os.sep}",
                   *fuzz_flags]
        replay = [sys.executable, str(Path(__file__).resolve()), str(probe), str(installation),
                  "--fuzz", "--ticks", str(args.ticks), "--replay", str(artifacts / "crash-<hash>")]
        with tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", prefix="flight-", suffix=".log",
                                         dir=probe.parent, delete=False) as log:
            print(f"Full flight log: {log.name}", flush=True)
            for line in (f"Build SHA256: {hashlib.sha256(probe.read_bytes()).hexdigest()}",
                         f"UBSAN_OPTIONS={env['UBSAN_OPTIONS']}",
                         f"Replay with the same build/data: {shlex.join(replay)}", shlex.join(command)):
                print(line, flush=True)
                log.write(line + "\n")
            log.write(inventory.stdout)
            with subprocess.Popen(command, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                  text=True, encoding="utf-8", errors="replace") as process:
                for line in process.stdout:
                    print(line, end="", flush=True)
                    log.write(line)
                    log.flush()
                status = process.wait()
            log.write(f"\nExit status: {status}\n")
            if status:
                raise RuntimeError(f"Flight fuzzer exited with status {status}; full report: {log.name}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        if isinstance(error, subprocess.CalledProcessError) and error.stdout:
            print(error.stdout, file=sys.stderr, end="")
        raise SystemExit(str(error)) from error
