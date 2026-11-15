#!/usr/bin/env python3
"""Run a flight fuzzer (flight-fuzzer or swc-flight-fuzzer) on a player's game data.

A Kilrathi Saga installation is copied to a disposable directory; an SWC directory is only read.
Corpus, artifacts and logs stay beside the fuzzer. libFuzzer flags follow --.
"""
import argparse
import hashlib
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
import tempfile

CAMPAIGNS = ("wc1", "sm1", "sm2", "sim")
# The fuzzer and runner select these; WC1_FLIGHT_KEYS and WC1_FLIGHT_TRACE pass through.
CONTROLLED = ("WC1_FLIGHT_DATA", "WC1_FLIGHT_CAMPAIGN", "WC1_FLIGHT_CASE", "WC1_FLIGHT_LIST",
              "WC1_FLIGHT_SEEDS", "WC1_FLIGHT_TICKS")


def positive(value):
    number = int(value)
    if number < 1:
        raise argparse.ArgumentTypeError("must be positive")
    return number


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("fuzzer", type=Path)
    parser.add_argument("data", type=Path,
                        help="Kilrathi Saga installation (GAMEDAT and STREAMS) or SWC directory (CMFs)")
    parser.add_argument("--campaign", choices=(*CAMPAIGNS, "all"), default="all",
                        help="sim is the training simulator; SWC's campaign data sets 0-2 are wc1, sm1 and sm2")
    parser.add_argument("--ticks", type=positive, default=600, help="maximum flight ticks per input (1..3600)")
    parser.add_argument("--case", help="restrict to one mission, e.g. wc1:1:0 or sim:0:2 (mission is zero-based)")
    parser.add_argument("--timeout", type=positive, default=120, help="seconds per input")
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
    fuzzer = args.fuzzer.resolve()
    if not fuzzer.is_file():
        parser.error(f"build the fuzzer first: {fuzzer}")
    installation = args.data.resolve()
    directories = ({path.name.upper(): path for path in installation.iterdir() if path.is_dir()}
                   if installation.is_dir() else {})
    swc = "CMFS" in directories
    if not swc and not {"GAMEDAT", "STREAMS"} <= directories.keys():
        parser.error("supply a Kilrathi Saga installation (GAMEDAT and STREAMS) or an SWC directory (CMFs)")
    env = {name: value for name, value in os.environ.items() if name not in CONTROLLED}
    env.update(SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", ASAN_OPTIONS="detect_leaks=0",
               UBSAN_OPTIONS="halt_on_error=0:print_stacktrace=1",
               WC1_FLIGHT_CAMPAIGN=str(CAMPAIGNS.index(args.campaign) if args.campaign != "all" else -1),
               WC1_FLIGHT_TICKS=str(args.ticks))
    if args.case:
        try:
            campaign, series, mission = args.case.split(":")
            series, mission = int(series), int(mission)
            if campaign not in CAMPAIGNS or not 0 <= mission < 256:
                raise ValueError
            if campaign == "sim" and (series != 0 or mission > 3):
                raise ValueError
            if campaign != "sim" and not 1 <= series <= 13:
                raise ValueError
            if args.campaign not in ("all", campaign):
                raise ValueError
        except ValueError:
            parser.error("--case must be campaign:series:mission, matching --campaign")
        env["WC1_FLIGHT_CASE"] = f"{CAMPAIGNS.index(campaign)}:{series}:{mission}"
    if args.replay and not args.replay.is_file():
        parser.error(f"replay input does not exist: {args.replay}")

    with tempfile.TemporaryDirectory(prefix="wc1-fuzz-") as scratch:
        if swc:
            env["WC1_FLIGHT_DATA"] = str(installation)
        else:
            # The game writes its saves and settings beside the data.
            excluded = {"SAVEGAME.WLD", "WINGCMDR.CFG"}
            shutil.copytree(directories["GAMEDAT"], Path(scratch) / "GAMEDAT",
                            ignore=lambda _, names: [name for name in names if name.upper() in excluded])
            shutil.copytree(directories["STREAMS"], Path(scratch) / "STREAMS")
            env["WC1_FLIGHT_DATA"] = scratch
        inventory = subprocess.run([str(fuzzer)], env=dict(env, WC1_FLIGHT_LIST="1"),
                                   timeout=args.timeout, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                   text=True, check=True)
        lines = inventory.stdout.splitlines()
        # Original fatal game errors can call exit(0).
        if "COMPLETE" not in lines:
            raise RuntimeError("flight inventory exited before completing its checks:\n" + inventory.stdout)
        installed = {int(line.split()[1]) for line in lines if line.startswith("INSTALLED ")}
        cases = [tuple(map(int, line.split()[1:])) for line in lines if line.startswith("CASE ")]
        selected = range(len(CAMPAIGNS)) if args.campaign == "all" else [CAMPAIGNS.index(args.campaign)]
        if args.campaign != "all" and not installed.issuperset(selected):
            parser.error(f"{args.campaign} not detected in {installation}")
        print("Flight missions: " + ", ".join(
            f"{CAMPAIGNS[index]} {sum(case[0] == index for case in cases)}" if index in installed
            else f"{CAMPAIGNS[index]} not detected" for index in selected), flush=True)
        if not cases:
            parser.error("no matching missions; check --case")
        name = fuzzer.name.removesuffix("-fuzzer")
        corpus = (args.corpus or fuzzer.parent / f"{name}-corpus").resolve()
        artifacts = fuzzer.parent / f"{name}-artifacts"
        artifacts.mkdir(parents=True, exist_ok=True)
        if args.replay:
            if tuple(args.replay.read_bytes()[:3]) not in cases:
                parser.error("replay input does not match the selected installed missions")
            inputs = [str(args.replay.resolve())]
        else:
            corpus.mkdir(parents=True, exist_ok=True)
            # The fuzzer adds small seeds for every selected mission and start leg.
            env["WC1_FLIGHT_SEEDS"] = str(corpus)
            inputs = [str(corpus)]
        command = [str(fuzzer), *inputs, f"-max_len={8 + 2 * args.ticks}",
                   f"-timeout={args.timeout}", "-detect_leaks=0", f"-artifact_prefix={artifacts}{os.sep}",
                   *fuzz_flags]
        replay = [sys.executable, str(Path(__file__).resolve()), str(fuzzer), str(installation),
                  "--ticks", str(args.ticks), "--replay", str(artifacts / "crash-<hash>")]
        with tempfile.NamedTemporaryFile(mode="w", encoding="utf-8", prefix=f"{name}-", suffix=".log",
                                         dir=fuzzer.parent, delete=False) as log:
            print(f"Full flight log: {log.name}", flush=True)
            for line in (f"Build SHA256: {hashlib.sha256(fuzzer.read_bytes()).hexdigest()}",
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
