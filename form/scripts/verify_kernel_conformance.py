#!/usr/bin/env python3
"""Run canonical Form vectors through the real sibling-kernel CLIs.

This is intentionally only an orchestrator.  It parses no Form and owns no
language semantics: each vector is handed unchanged to the Go, Rust, and
TypeScript kernels through their public ``--expr`` entry point.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path


FORM_DIR = Path(__file__).resolve().parents[1]
REPO_ROOT = FORM_DIR.parent
DEFAULT_VECTOR = FORM_DIR / "conformance" / "canonical-s-expression-vectors.json"
DEFAULT_MALFORMED_VECTOR = FORM_DIR / "conformance" / "formbin2-malformed-vectors.json"
sys.path.insert(0, str(REPO_ROOT))

from form.python_bmf.sdk import (  # noqa: E402
    FormBinaryComposite,
    FormBinaryFloat64,
    FormBinaryInt64,
    FormBinaryLeaf,
    NodeID,
    decode_form_binary,
    dump_form_binary,
    encode_form_binary,
    load_form_binary,
)

FORM_BINARY_MAX_BYTES = 64 << 20


@dataclass(frozen=True)
class KernelCommand:
    name: str
    argv: tuple[str, ...]
    cwd: Path


def _run(command: list[str], *, cwd: Path, timeout: int = 180) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        cwd=cwd,
        check=False,
        capture_output=True,
        text=True,
        timeout=timeout,
    )


def _require_ok(result: subprocess.CompletedProcess[str], label: str) -> None:
    if result.returncode == 0:
        return
    detail = result.stderr.strip() or result.stdout.strip() or "no output"
    raise RuntimeError(f"{label} failed ({result.returncode}):\n{detail}")


def _build_commands(selected: list[str], build_dir: Path) -> list[KernelCommand]:
    commands: list[KernelCommand] = []

    if "go" in selected:
        go_dir = FORM_DIR / "form-kernel-go"
        go_bin = build_dir / "form-kernel-go"
        result = _run(["go", "build", "-o", str(go_bin), "."], cwd=go_dir)
        _require_ok(result, "Go kernel build")
        commands.append(KernelCommand("go", (str(go_bin),), go_dir))

    if "rust" in selected:
        rust_dir = FORM_DIR / "form-kernel-rust"
        target_dir = build_dir / "rust-target"
        result = _run(
            ["cargo", "build", "--quiet", "--release", "--target-dir", str(target_dir)],
            cwd=rust_dir,
        )
        _require_ok(result, "Rust kernel build")
        rust_bin = target_dir / "release" / "form-kernel-rust"
        commands.append(KernelCommand("rust", (str(rust_bin),), rust_dir))

    if "ts" in selected:
        ts_dir = FORM_DIR / "form-kernel-ts"
        tsx = ts_dir / "node_modules" / ".bin" / "tsx"
        if not tsx.is_file():
            raise RuntimeError(
                "TypeScript kernel dependencies are absent; run `npm ci` in "
                f"{ts_dir} before the conformance proof"
            )
        commands.append(
            KernelCommand("typescript", (str(tsx), "src/main.ts"), ts_dir)
        )

    return commands


def _load_cases(path: Path) -> tuple[str, list[dict[str, str]]]:
    payload = json.loads(path.read_text())
    if payload.get("schema") != "form-kernel-conformance-v1":
        raise ValueError(f"{path}: unsupported conformance schema")
    raw_cases = payload.get("cases")
    if not isinstance(raw_cases, list) or not raw_cases:
        raise ValueError(f"{path}: cases must be a non-empty list")
    cases: list[dict[str, str]] = []
    for index, case in enumerate(raw_cases):
        if not isinstance(case, dict):
            raise ValueError(f"{path}: case {index} is not an object")
        normalized: dict[str, str] = {}
        for field in ("name", "form", "expected"):
            value = case.get(field)
            if not isinstance(value, str) or not value:
                raise ValueError(f"{path}: case {index} has invalid {field}")
            normalized[field] = value
        cases.append(normalized)
    return payload["schema"], cases


def _execute(kernel: KernelCommand, form: str) -> str:
    result = _run([*kernel.argv, "--expr", form], cwd=kernel.cwd, timeout=60)
    _require_ok(result, f"{kernel.name} kernel expression {form!r}")
    return result.stdout.rstrip("\n")


def _run_binary(kernel: KernelCommand, path: Path) -> str:
    result = _run([*kernel.argv, "--binary", str(path)], cwd=kernel.cwd, timeout=60)
    _require_ok(result, f"{kernel.name} kernel binary read {path}")
    return result.stdout.rstrip("\n")


def _emit_binary(kernel: KernelCommand, source: Path, output: Path) -> None:
    result = _run(
        [*kernel.argv, "--emit-binary", str(output), str(source)],
        cwd=kernel.cwd,
        timeout=60,
    )
    _require_ok(result, f"{kernel.name} kernel binary emit {source}")


def _u32(value: int) -> bytes:
    return value.to_bytes(4, "big")


def _malformed_bytes(case: dict[str, object]) -> bytes:
    builder = case.get("builder")
    if builder == "hex":
        value = case.get("value")
        if not isinstance(value, str):
            raise ValueError(f"malformed case {case.get('name')}: missing hex value")
        return bytes.fromhex(value)
    if builder == "nested-composite":
        depth = case.get("depth")
        if not isinstance(depth, int) or depth < 1:
            raise ValueError(f"malformed case {case.get('name')}: invalid depth")
        leaf = _u32(0) + _u32(1) + _u32(1) + _u32(1) + _u32(0)
        node = leaf
        for _ in range(depth):
            node = _u32(1) + leaf + _u32(1) + node
        return b"FORMBIN2" + _u32(0) + node
    if builder == "oversized-artifact":
        return b"FORMBIN2" + bytes(FORM_BINARY_MAX_BYTES + 1 - len(b"FORMBIN2"))
    raise ValueError(f"malformed case {case.get('name')}: unknown builder {builder!r}")


def _prove_malformed_form_binary(
    kernels: list[KernelCommand], build_dir: Path, vector_path: Path
) -> dict[str, object]:
    payload = json.loads(vector_path.read_text())
    if payload.get("schema") != "formbin2-malformed-conformance-v1":
        raise ValueError(f"{vector_path}: unsupported malformed-vector schema")
    cases = payload.get("cases")
    if not isinstance(cases, list) or not cases:
        raise ValueError(f"{vector_path}: cases must be a non-empty list")
    results: list[dict[str, object]] = []
    for index, raw_case in enumerate(cases):
        if not isinstance(raw_case, dict):
            raise ValueError(f"{vector_path}: case {index} is not an object")
        name = raw_case.get("name")
        expected = raw_case.get("expected_error")
        if not isinstance(name, str) or not isinstance(expected, str):
            raise ValueError(f"{vector_path}: case {index} has invalid metadata")
        data = _malformed_bytes(raw_case)
        artifact = build_dir / f"malformed-{index:02d}.fkb"
        artifact.write_bytes(data)

        python_error = ""
        try:
            decode_form_binary(data)
        except (TypeError, ValueError) as error:
            python_error = str(error)
        if expected not in python_error:
            raise RuntimeError(
                f"Python accepted or misclassified malformed FORMBIN2 {name}: "
                f"expected {expected!r}, got {python_error!r}"
            )

        kernel_errors: dict[str, str] = {}
        for kernel in kernels:
            result = _run(
                [*kernel.argv, "--binary", str(artifact)],
                cwd=kernel.cwd,
                timeout=60,
            )
            detail = "\n".join(part for part in (result.stderr, result.stdout) if part).strip()
            if result.returncode == 0 or expected not in detail:
                raise RuntimeError(
                    f"{kernel.name} accepted or misclassified malformed FORMBIN2 {name}: "
                    f"exit={result.returncode}, expected {expected!r}, output={detail!r}"
                )
            kernel_errors[kernel.name] = expected
        print(f"PASS malformed FORMBIN2 {name}: Python + {len(kernels)} kernels rejected")
        results.append(
            {
                "name": name,
                "expected_error": expected,
                "bytes": len(data),
                "python": expected,
                "kernels": kernel_errors,
                "status": "pass",
            }
        )
    return {
        "status": "pass",
        "vector": str(vector_path),
        "cases": results,
    }


def _prove_form_binary(kernels: list[KernelCommand], build_dir: Path) -> dict[str, object]:
    python_artifact = build_dir / "python-formbin2.fkb"
    python_root = FormBinaryComposite(
        FormBinaryLeaf(NodeID(1, 2, 34, 1)),
        (
            FormBinaryLeaf(NodeID(1, 1, 2, 0), "python-formbin2"),
            FormBinaryLeaf(NodeID(1, 1, 1, 42)),
            FormBinaryInt64(2147483648),
            FormBinaryFloat64(3.5),
        ),
    )
    dump_form_binary(python_artifact, python_root)
    if not python_artifact.read_bytes().startswith(b"FORMBIN2"):
        raise RuntimeError("Python wire codec did not emit canonical FORMBIN2")
    expected_python = "[python-formbin2, 42, 2147483648, 3.5]"
    python_to_kernel = {
        kernel.name: _run_binary(kernel, python_artifact) for kernel in kernels
    }
    if set(python_to_kernel.values()) != {expected_python}:
        raise RuntimeError(
            f"Python FORMBIN2 artifact diverged in real kernels: {python_to_kernel}"
        )

    source = build_dir / "kernel-fkb-interop.fk"
    source.write_text('(list "fkb-interop" 42 2147483648 3.5)\n')
    expected_kernel = "[fkb-interop, 42, 2147483648, 3.5]"
    emitted: dict[str, Path] = {}
    kernel_to_python: dict[str, dict[str, object]] = {}
    for producer in kernels:
        artifact = build_dir / f"{producer.name}.fkb"
        _emit_binary(producer, source, artifact)
        original = artifact.read_bytes()
        root = load_form_binary(artifact)
        repacked = encode_form_binary(root)
        if repacked != original:
            raise RuntimeError(
                f"Python FORMBIN2 codec did not byte-roundtrip {producer.name} artifact"
            )
        consumer_outputs = {
            consumer.name: _run_binary(consumer, artifact) for consumer in kernels
        }
        if set(consumer_outputs.values()) != {expected_kernel}:
            raise RuntimeError(
                f"{producer.name} FORMBIN2 artifact diverged across kernels: {consumer_outputs}"
            )
        emitted[producer.name] = artifact
        kernel_to_python[producer.name] = {
            "bytes": len(original),
            "byte_roundtrip": True,
            "consumer_outputs": consumer_outputs,
        }
    byte_variants = {path.read_bytes() for path in emitted.values()}
    if len(byte_variants) != 1:
        raise RuntimeError("sibling kernels emitted different FORMBIN2 bytes for one source")
    print(
        "PASS FORMBIN2: Python artifact executed by "
        f"{len(kernels)} kernels; {len(kernels)} kernel artifacts byte-roundtripped in Python"
    )
    return {
        "status": "pass",
        "magic": "FORMBIN2",
        "python_to_kernel": python_to_kernel,
        "kernel_to_python": kernel_to_python,
        "sibling_bytes_identical": True,
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Execute canonical Form S-expressions on real sibling kernels"
    )
    parser.add_argument("--vector", type=Path, default=DEFAULT_VECTOR)
    parser.add_argument(
        "--malformed-vector", type=Path, default=DEFAULT_MALFORMED_VECTOR
    )
    parser.add_argument(
        "--kernel",
        action="append",
        choices=("all", "go", "rust", "ts"),
        help="kernel to prove; repeat for several (default: all)",
    )
    parser.add_argument("--json", type=Path, help="optional machine-readable report path")
    args = parser.parse_args(argv)

    requested = args.kernel or ["all"]
    selected = ["go", "rust", "ts"] if "all" in requested else list(dict.fromkeys(requested))
    schema, cases = _load_cases(args.vector.resolve())
    report: dict[str, object] = {
        "schema": schema,
        "vector": str(args.vector.resolve()),
        "kernels": selected,
        "status": "pass",
        "cases": [],
    }
    failed = False

    with tempfile.TemporaryDirectory(prefix="form-kernel-conformance-") as temp:
        kernels = _build_commands(selected, Path(temp))
        for case in cases:
            outputs = {kernel.name: _execute(kernel, case["form"]) for kernel in kernels}
            values = set(outputs.values())
            passed = values == {case["expected"]}
            failed = failed or not passed
            report_case = {
                "name": case["name"],
                "form": case["form"],
                "expected": case["expected"],
                "outputs": outputs,
                "status": "pass" if passed else "fail",
            }
            report["cases"].append(report_case)  # type: ignore[union-attr]
            rendered = ", ".join(f"{name}={value!r}" for name, value in outputs.items())
            print(f"{'PASS' if passed else 'FAIL'} {case['name']}: {rendered}")
        report["form_binary"] = _prove_form_binary(kernels, Path(temp))
        report["malformed_form_binary"] = _prove_malformed_form_binary(
            kernels, Path(temp), args.malformed_vector.resolve()
        )

    if failed:
        report["status"] = "fail"
    if args.json is not None:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    print(
        f"{report['status'].upper()} {len(cases)} canonical expressions × "
        f"{len(selected)} real kernels"
    )
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
