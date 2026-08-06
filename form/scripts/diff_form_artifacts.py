#!/usr/bin/env python3
"""Compare two real FORMBIN2 recipe trees structurally."""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO_ROOT))

from form.python_bmf.sdk import (  # noqa: E402
    FormBinaryComposite,
    FormBinaryFloat64,
    FormBinaryInt64,
    FormBinaryLeaf,
    load_form_binary,
)


def _projection(node):
    if isinstance(node, FormBinaryComposite):
        return ["composite", _projection(node.category), [_projection(c) for c in node.children]]
    if isinstance(node, FormBinaryFloat64):
        return ["float64-le", struct.pack("<d", node.value).hex()]
    if isinstance(node, FormBinaryInt64):
        return ["int64", node.value]
    if isinstance(node, FormBinaryLeaf):
        if node.string_value is not None:
            return ["string", node.string_value]
        nid = node.nodeid
        return ["leaf", nid.pkg, nid.level, nid.type, nid.inst]
    raise TypeError(type(node).__name__)


def _node_count(node) -> int:
    if not isinstance(node, FormBinaryComposite):
        return 1
    return 1 + _node_count(node.category) + sum(_node_count(c) for c in node.children)


def _hash(projection) -> str:
    payload = json.dumps(projection, separators=(",", ":"), ensure_ascii=False).encode()
    return hashlib.sha256(payload).hexdigest()


def diff_artifacts(source_path: str, target_path: str) -> dict:
    source = load_form_binary(source_path)
    target = load_form_binary(target_path)
    source_projection = _projection(source)
    target_projection = _projection(target)
    return {
        "schema": "formbin2-structural-diff-v1",
        "source": source_path,
        "target": target_path,
        "source_node_count": _node_count(source),
        "target_node_count": _node_count(target),
        "source_structural_sha256": _hash(source_projection),
        "target_structural_sha256": _hash(target_projection),
        "structurally_equal": source_projection == target_projection,
        "byte_equal": Path(source_path).read_bytes() == Path(target_path).read_bytes(),
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Structurally compare canonical FORMBIN2")
    parser.add_argument("source")
    parser.add_argument("target")
    parser.add_argument("--json", type=Path)
    args = parser.parse_args(argv)
    report = diff_artifacts(args.source, args.target)
    rendered = json.dumps(report, indent=2, sort_keys=True) + "\n"
    if args.json is not None:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(rendered)
    else:
        print(rendered, end="")
    return 0 if report["structurally_equal"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
