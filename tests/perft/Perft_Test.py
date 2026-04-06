# generate from ChatGPT
from __future__ import annotations

import argparse
import subprocess
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List


@dataclass
class PerftCase:
    name: str = ""
    fen: str = ""
    test_depth: int = 0
    nodes: List[int] = field(default_factory=list)
    captures: List[int] = field(default_factory=list)
    enPassants: List[int] = field(default_factory=list)
    castles: List[int] = field(default_factory=list)
    promotions: List[int] = field(default_factory=list)
    checks: List[int] = field(default_factory=list)


def parse_int_list(text: str) -> List[int]:
    return [int(x) for x in text.split()]


def load_perft_file(path: Path) -> List[PerftCase]:
    cases: List[PerftCase] = []
    current: PerftCase | None = None

    with path.open("r", encoding="utf-8") as f:
        for lineno, raw in enumerate(f, start=1):
            line = raw.strip()

            if not line:
                continue

            if line.startswith("#"):
                if current is not None:
                    cases.append(current)
                current = PerftCase(name=line[1:].strip())
                continue

            if current is None:
                raise ValueError(f"{path}:{lineno}: content found before any test header")

            if ":" not in line:
                raise ValueError(f"{path}:{lineno}: invalid line: {line}")

            key, value = line.split(":", 1)
            key = key.strip()
            value = value.strip()

            if key == "fen":
                current.fen = value
            elif key == "testDepth":
                current.test_depth = int(value)
            elif key == "nodes":
                current.nodes = parse_int_list(value)
            elif key == "captures":
                current.captures = parse_int_list(value)
            elif key == "enPassants":
                current.enPassants = parse_int_list(value)
            elif key == "castles":
                current.castles = parse_int_list(value)
            elif key == "promotions":
                current.promotions = parse_int_list(value)
            elif key == "checks":
                current.checks = parse_int_list(value)
            else:
                raise ValueError(f"{path}:{lineno}: unknown key: {key}")

    if current is not None:
        cases.append(current)

    return cases


def validate_case(case: PerftCase) -> None:
    need = case.test_depth + 1

    if not case.name:
        raise ValueError("missing case name")
    if not case.fen:
        raise ValueError(f"{case.name}: missing fen")
    if case.test_depth <= 0:
        raise ValueError(f"{case.name}: invalid testDepth: {case.test_depth}")

    fields = [
        ("nodes", case.nodes),
        ("captures", case.captures),
        ("enPassants", case.enPassants),
        ("castles", case.castles),
        ("promotions", case.promotions),
        ("checks", case.checks),
    ]

    for field_name, values in fields:
        if len(values) != need:
            raise ValueError(
                f"{case.name}: field {field_name} expects {need} values, got {len(values)}"
            )


def fen_to_arg(fen: str) -> str:
    if fen == "startpos":
        return "startpos"
    return fen


def parse_key_value_output(stdout: str) -> Dict[str, int]:
    result: Dict[str, int] = {}

    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if not line:
            continue

        if "=" not in line:
            # 非 key=value 的行直接忽略
            # 如果你之後想更嚴格，也可以改成 raise
            continue

        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()

        # 只收我們關心的欄位
        if key in {"nodes", "captures", "enPassants", "castles", "promotions", "checks"}:
            result[key] = int(value)

    return result


def run_engine_stats(engine_path: Path, fen: str, depth: int) -> Dict[str, int]:
    cmd = [
        str(engine_path),
        "--fen",
        fen_to_arg(fen),
        "--stats",
        str(depth),
    ]

    completed = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )

    if completed.returncode != 0:
        raise RuntimeError(
            "engine failed\n"
            f"cmd: {' '.join(cmd)}\n"
            f"returncode: {completed.returncode}\n"
            f"stdout:\n{completed.stdout}\n"
            f"stderr:\n{completed.stderr}"
        )

    parsed = parse_key_value_output(completed.stdout)

    required_keys = {"nodes", "captures", "enPassants", "castles", "promotions", "checks"}
    missing = required_keys - parsed.keys()
    if missing:
        raise RuntimeError(
            "engine output missing required keys\n"
            f"cmd: {' '.join(cmd)}\n"
            f"missing: {sorted(missing)}\n"
            f"stdout:\n{completed.stdout}\n"
            f"stderr:\n{completed.stderr}"
        )

    return parsed


def expected_stats(case: PerftCase, depth: int) -> Dict[str, int]:
    return {
        "nodes": case.nodes[depth],
        "captures": case.captures[depth],
        "enPassants": case.enPassants[depth],
        "castles": case.castles[depth],
        "promotions": case.promotions[depth],
        "checks": case.checks[depth],
    }


def compare_stats(expected: Dict[str, int], got: Dict[str, int]) -> List[str]:
    diffs: List[str] = []
    for key in ["nodes", "captures", "enPassants", "castles", "promotions", "checks"]:
        if expected[key] != got[key]:
            diffs.append(f"{key}: expected {expected[key]}, got {got[key]}")
    return diffs


def run_all_tests(engine_path: Path, perft_file: Path, max_depth: int | None) -> int:
    cases = load_perft_file(perft_file)

    for case in cases:
        validate_case(case)

    total = 0
    failed = 0

    print(f"Loaded {len(cases)} cases from {perft_file}")
    print(f"Engine: {engine_path}")
    print()

    for case in cases:
        depth_limit = case.test_depth
        if max_depth is not None:
            depth_limit = min(depth_limit, max_depth)

        print(f"=== {case.name} ===")
        for depth in range(1, depth_limit + 1):
            total += 1

            expected = expected_stats(case, depth)
            got = run_engine_stats(engine_path, case.fen, depth)

            diffs = compare_stats(expected, got)

            if diffs:
                failed += 1
                print(f"[FAIL] depth={depth}")
                for diff in diffs:
                    print(f"  - {diff}")
            else:
                print(f"[ OK ] depth={depth}")

        print()

    print(f"Summary: {total - failed}/{total} passed")

    return 1 if failed else 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Run perft tests against engine CLI")
    parser.add_argument(
        "--engine",
        required=True,
        help="Path to engine executable, e.g. build/Hynobius.exe",
    )
    parser.add_argument(
        "--file",
        required=True,
        help="Path to perft test file, e.g. tests/perft.txt",
    )
    parser.add_argument(
        "--max-depth",
        type=int,
        default=None,
        help="Optional cap on depth for all cases",
    )

    args = parser.parse_args()

    engine_path = Path(args.engine)
    perft_file = Path(args.file)

    if not engine_path.exists():
        raise FileNotFoundError(f"engine not found: {engine_path}")
    if not perft_file.exists():
        raise FileNotFoundError(f"perft file not found: {perft_file}")

    return run_all_tests(engine_path, perft_file, args.max_depth)


if __name__ == "__main__":
    raise SystemExit(main())