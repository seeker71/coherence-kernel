"""Thin Python DTO boundary for kernel-issued Form data.

There is deliberately no parser, evaluator, emitter, interner, or identity
allocator here. The sibling kernels own language semantics and composite
identity. Python may carry their NodeIDs, read/write canonical FORMBIN2, and
read/write the separate human-facing source lens.
"""

from __future__ import annotations

import json
from dataclasses import dataclass, field
from pathlib import Path

from .form_binary import (
    MAGIC,
    FormBinaryComposite,
    FormBinaryFloat64,
    FormBinaryInt64,
    FormBinaryLeaf,
    FormBinaryNode,
    NodeID,
    decode_form_binary,
    dump_form_binary,
    encode_form_binary,
    load_form_binary,
)


@dataclass(frozen=True)
class SourceSpan:
    path: str
    start_offset: int
    end_offset: int
    start_line: int
    start_col: int
    end_line: int
    end_col: int

    @classmethod
    def empty(cls) -> "SourceSpan":
        return cls("", 0, 0, 0, 0, 0, 0)


@dataclass
class Lens:
    """Human symbol/source metadata keyed by kernel-issued NodeID text."""

    entries: dict[str, dict] = field(default_factory=dict)

    def symbol_for(self, nodeid: NodeID) -> str | None:
        entry = self.entries.get(str(nodeid))
        return entry.get("symbol") if entry else None

    def span_for(self, nodeid: NodeID) -> SourceSpan | None:
        entry = self.entries.get(str(nodeid))
        if not entry or "span" not in entry:
            return None
        return SourceSpan(**entry["span"])

    @classmethod
    def load(cls, path: str | Path) -> "Lens":
        source = Path(path)
        if not source.exists():
            return cls()
        return cls(entries=json.loads(source.read_text()))

    def write(self, path: str | Path) -> None:
        Path(path).write_text(json.dumps(self.entries, indent=2, sort_keys=True))


def lens_path_for(fkb_path: str | Path) -> Path:
    return Path(fkb_path).with_suffix(".fkl")


__all__ = [
    "MAGIC",
    "NodeID",
    "FormBinaryLeaf",
    "FormBinaryFloat64",
    "FormBinaryInt64",
    "FormBinaryComposite",
    "FormBinaryNode",
    "encode_form_binary",
    "decode_form_binary",
    "dump_form_binary",
    "load_form_binary",
    "SourceSpan",
    "Lens",
    "lens_path_for",
]
