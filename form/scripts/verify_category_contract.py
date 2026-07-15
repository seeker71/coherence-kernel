#!/usr/bin/env python3
"""Reject category-number drift and collisions across the sibling kernels."""

from __future__ import annotations

import json
import re
import sys
from collections import defaultdict
from pathlib import Path


FORM_DIR = Path(__file__).resolve().parents[1]
CONTRACT_PATH = FORM_DIR / "category-contract.json"

GO_RE = re.compile(r"\bRBasic([A-Za-z0-9]+)\s+uint32\s*=\s*(\d+)")
RUST_RE = re.compile(r"\bconst\s+(?:RB|RBASIC)_([A-Z0-9_]+)\s*:\s*u32\s*=\s*(\d+)")
TS_ALIAS_RE = re.compile(
    r"export\s+const\s+RBasic([A-Za-z0-9]+)\s*=\s*RBasic\.([A-Z0-9_]+)\s*;"
)

NAME_ALIASES = {
    "FN_DEF": "FNDEF",
    "FN_CALL": "FNCALL",
    "LIST_LOCAL": "LIST",
    # The field arm historically called this local constant Resolve.
    "RESOLVE": "FIELD_RESOLVE",
}

REQUIRED_SIBLING = {
    "FORMAT",
    "NUMERIC",
    "QUOTIENT",
    "INDUCTIVE",
    "CONSTRUCTOR",
    "EQUIVALENCE",
}


def _camel_to_contract(name: str) -> str:
    snake = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", name).upper()
    return NAME_ALIASES.get(snake, snake)


def _source_texts(root: Path, suffix: str) -> list[tuple[Path, str]]:
    return [(path, path.read_text()) for path in sorted(root.rglob(f"*{suffix}"))]


def _collect_numeric(
    sources: list[tuple[Path, str]], pattern: re.Pattern[str], *, camel: bool
) -> tuple[dict[str, int], list[str]]:
    found: dict[str, int] = {}
    errors: list[str] = []
    for path, text in sources:
        for raw_name, raw_value in pattern.findall(text):
            name = _camel_to_contract(raw_name) if camel else NAME_ALIASES.get(raw_name, raw_name)
            value = int(raw_value)
            previous = found.get(name)
            if previous is not None and previous != value:
                errors.append(
                    f"{path.relative_to(FORM_DIR)}: {name}={value} conflicts with {previous}"
                )
            found[name] = value
    return found, errors


def _compare(
    implementation: str, observed: dict[str, int], contract: dict[str, int]
) -> list[str]:
    errors: list[str] = []
    for name, value in sorted(observed.items()):
        expected = contract.get(name)
        if expected is None:
            errors.append(f"{implementation}: {name}={value} is absent from category-contract.json")
        elif value != expected:
            errors.append(
                f"{implementation}: {name}={value}, category-contract.json requires {expected}"
            )
    missing = sorted(REQUIRED_SIBLING - observed.keys())
    if missing:
        errors.append(f"{implementation}: missing sibling constants: {', '.join(missing)}")
    return errors


def main() -> int:
    payload = json.loads(CONTRACT_PATH.read_text())
    contract = payload.get("r_basic")
    if not isinstance(contract, dict) or not contract:
        print("FAIL category-contract.json has no non-empty r_basic map", file=sys.stderr)
        return 1
    if any(not isinstance(k, str) or not isinstance(v, int) for k, v in contract.items()):
        print("FAIL category-contract.json r_basic must map names to integers", file=sys.stderr)
        return 1

    errors: list[str] = []
    by_slot: dict[int, list[str]] = defaultdict(list)
    for name, slot in contract.items():
        by_slot[slot].append(name)
    for slot, names in sorted(by_slot.items()):
        if len(names) > 1:
            errors.append(f"category-contract.json: RBasic slot {slot} aliases {', '.join(names)}")

    go, go_errors = _collect_numeric(
        _source_texts(FORM_DIR / "form-kernel-go", ".go"), GO_RE, camel=True
    )
    rust, rust_errors = _collect_numeric(
        _source_texts(FORM_DIR / "form-kernel-rust" / "src", ".rs"),
        RUST_RE,
        camel=False,
    )
    errors.extend(go_errors)
    errors.extend(rust_errors)
    errors.extend(_compare("Go", go, contract))
    errors.extend(_compare("Rust", rust, contract))

    ts_root = FORM_DIR / "form-kernel-ts" / "src"
    ts_sources = _source_texts(ts_root, ".ts")
    kernel_text = (ts_root / "kernel.ts").read_text()
    if 'import CATEGORY_CONTRACT from "../../category-contract.json";' not in kernel_text:
        errors.append("TypeScript: kernel.ts does not import the canonical category contract")
    if "Object.freeze(CATEGORY_CONTRACT.r_basic)" not in kernel_text:
        errors.append("TypeScript: RBasic is not projected from category-contract.json")
    ts_aliases: dict[str, int] = {}
    for path, text in ts_sources:
        for raw_name, target in TS_ALIAS_RE.findall(text):
            name = _camel_to_contract(raw_name)
            if name != target:
                errors.append(
                    f"{path.relative_to(FORM_DIR)}: RBasic{raw_name} aliases {target}, expected {name}"
                )
            if target not in contract:
                errors.append(f"TypeScript: alias target {target} is absent from category contract")
            else:
                ts_aliases[target] = contract[target]
    missing_ts = sorted({"FORMAT", "NUMERIC", "LANGUAGE", "EQUIVALENCE"} - ts_aliases.keys())
    if missing_ts:
        errors.append(f"TypeScript: missing contract aliases: {', '.join(missing_ts)}")

    for error in errors:
        print(f"FAIL {error}", file=sys.stderr)
    if errors:
        return 1
    print(
        "PASS category contract: "
        f"{len(contract)} injective RBasic slots; Go={len(go)}, Rust={len(rust)}, "
        f"TypeScript=contract+{len(ts_aliases)} named aliases"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
