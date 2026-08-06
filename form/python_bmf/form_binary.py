"""Strict DTOs and the canonical FORMBIN2 wire codec.

This module never issues identities. ``NodeID`` is data read from a kernel or
supplied as an already-issued registered/trivial tuple. Composite identity is
artifact-scoped and is reconstructed by the kernel when FORMBIN2 is loaded.
"""

from __future__ import annotations

import struct
from dataclasses import dataclass
from pathlib import Path


MAGIC = b"FORMBIN2"
FORM_BINARY_LEAF = 0
FORM_BINARY_COMPOSITE = 1
FORM_BINARY_FLOAT64 = 2
FORM_BINARY_INT64 = 3
TRIVIAL_LEVEL = 1
TRIVIAL_STRING = 2
U32_MAX = 2**32 - 1
FORM_BINARY_MAX_BYTES = 64 << 20
FORM_BINARY_MAX_STRINGS = 262_144
FORM_BINARY_MAX_STRING_BYTES = 32 << 20
FORM_BINARY_MAX_CHILDREN = 262_144
FORM_BINARY_MAX_NODES = 1_000_000
FORM_BINARY_MAX_DEPTH = 256


@dataclass(frozen=True, order=True)
class NodeID:
    """A kernel-issued four-u32 identity carried as inert data."""

    pkg: int
    level: int
    type: int
    inst: int

    def __post_init__(self) -> None:
        for name, value in (
            ("pkg", self.pkg),
            ("level", self.level),
            ("type", self.type),
            ("inst", self.inst),
        ):
            if not isinstance(value, int) or isinstance(value, bool):
                raise TypeError(f"NodeID {name} must be an integer")
            if value < 0 or value > U32_MAX:
                raise ValueError(f"NodeID {name} must be in u32 range")

    def __str__(self) -> str:
        return f"@{self.pkg}.{self.level}.{self.type}.{self.inst}"

    @classmethod
    def parse(cls, text: str) -> "NodeID":
        parts = text.removeprefix("@").split(".")
        if len(parts) != 4:
            raise ValueError(f"NodeID expects 4 parts, got {text!r}")
        return cls(*(int(part) for part in parts))


@dataclass(frozen=True)
class FormBinaryLeaf:
    nodeid: NodeID
    string_value: str | None = None


@dataclass(frozen=True)
class FormBinaryFloat64:
    value: float


@dataclass(frozen=True)
class FormBinaryInt64:
    value: int

    def __post_init__(self) -> None:
        if self.value < -(2**63) or self.value > 2**63 - 1:
            raise ValueError("FORMBIN2 int64 value is out of range")


@dataclass(frozen=True)
class FormBinaryComposite:
    category: "FormBinaryNode"
    children: tuple["FormBinaryNode", ...]


FormBinaryNode = (
    FormBinaryLeaf | FormBinaryFloat64 | FormBinaryInt64 | FormBinaryComposite
)


@dataclass
class _DecodeBudget:
    nodes: int = 0

    def enter(self, depth: int) -> None:
        if depth > FORM_BINARY_MAX_DEPTH:
            raise ValueError("form binary: maximum node depth exceeded")
        self.nodes += 1
        if self.nodes > FORM_BINARY_MAX_NODES:
            raise ValueError("form binary: maximum node count exceeded")


def _u32(value: int) -> bytes:
    return struct.pack(">I", value)


def _read_u32(data: bytes, position: int) -> tuple[int, int]:
    if position + 4 > len(data):
        raise ValueError("form binary: truncated u32")
    return struct.unpack_from(">I", data, position)[0], position + 4


def _collect_strings(
    node: FormBinaryNode, strings: list[str], indexes: dict[str, int]
) -> None:
    if isinstance(node, FormBinaryComposite):
        _collect_strings(node.category, strings, indexes)
        for child in node.children:
            _collect_strings(child, strings, indexes)
    elif isinstance(node, FormBinaryLeaf) and node.string_value is not None:
        if node.nodeid.level != TRIVIAL_LEVEL or node.nodeid.type != TRIVIAL_STRING:
            raise ValueError("form binary: string value requires a Triv.STRING leaf")
        if node.string_value not in indexes:
            indexes[node.string_value] = len(strings)
            strings.append(node.string_value)


def _encode_node(node: FormBinaryNode, indexes: dict[str, int]) -> bytes:
    if isinstance(node, FormBinaryComposite):
        encoded = bytearray(_u32(FORM_BINARY_COMPOSITE))
        encoded += _encode_node(node.category, indexes)
        encoded += _u32(len(node.children))
        for child in node.children:
            encoded += _encode_node(child, indexes)
        return bytes(encoded)
    if isinstance(node, FormBinaryFloat64):
        return _u32(FORM_BINARY_FLOAT64) + struct.pack("<d", node.value)
    if isinstance(node, FormBinaryInt64):
        return _u32(FORM_BINARY_INT64) + struct.pack("<q", node.value)
    if not isinstance(node, FormBinaryLeaf):
        raise TypeError(f"form binary: unsupported node {type(node).__name__}")
    nid = node.nodeid
    if nid.level == TRIVIAL_LEVEL and nid.type == TRIVIAL_STRING:
        if node.string_value is None:
            raise ValueError("form binary: Triv.STRING leaf requires its artifact value")
        inst = indexes[node.string_value]
    elif node.string_value is not None:
        raise ValueError("form binary: only Triv.STRING leaves carry string values")
    else:
        inst = nid.inst
    return b"".join(
        (_u32(FORM_BINARY_LEAF), _u32(nid.pkg), _u32(nid.level), _u32(nid.type), _u32(inst))
    )


def encode_form_binary(root: FormBinaryNode) -> bytes:
    strings: list[str] = []
    indexes: dict[str, int] = {}
    _collect_strings(root, strings, indexes)
    encoded = bytearray(MAGIC)
    encoded += _u32(len(strings))
    for value in strings:
        raw = value.encode("utf-8")
        encoded += _u32(len(raw)) + raw
    encoded += _encode_node(root, indexes)
    return bytes(encoded)


def _decode_node(
    data: bytes,
    position: int,
    strings: tuple[str, ...],
    budget: _DecodeBudget,
    depth: int,
) -> tuple[FormBinaryNode, int]:
    budget.enter(depth)
    tag, position = _read_u32(data, position)
    if tag == FORM_BINARY_FLOAT64:
        if position + 8 > len(data):
            raise ValueError("form binary: truncated float64")
        return FormBinaryFloat64(struct.unpack_from("<d", data, position)[0]), position + 8
    if tag == FORM_BINARY_INT64:
        if position + 8 > len(data):
            raise ValueError("form binary: truncated int64")
        return FormBinaryInt64(struct.unpack_from("<q", data, position)[0]), position + 8
    if tag == FORM_BINARY_LEAF:
        fields: list[int] = []
        for _ in range(4):
            value, position = _read_u32(data, position)
            fields.append(value)
        nodeid = NodeID(*fields)
        string_value = None
        if nodeid.level == TRIVIAL_LEVEL and nodeid.type == TRIVIAL_STRING:
            if nodeid.inst >= len(strings):
                raise ValueError(f"form binary: bad string index {nodeid.inst}")
            string_value = strings[nodeid.inst]
        return FormBinaryLeaf(nodeid, string_value), position
    if tag != FORM_BINARY_COMPOSITE:
        raise ValueError(f"form binary: unknown node tag {tag}")
    category, position = _decode_node(data, position, strings, budget, depth + 1)
    count, position = _read_u32(data, position)
    if count > FORM_BINARY_MAX_CHILDREN:
        raise ValueError("form binary: maximum child count exceeded")
    children: list[FormBinaryNode] = []
    for _ in range(count):
        child, position = _decode_node(data, position, strings, budget, depth + 1)
        children.append(child)
    return FormBinaryComposite(category, tuple(children)), position


def decode_form_binary(data: bytes) -> FormBinaryNode:
    if len(data) > FORM_BINARY_MAX_BYTES:
        raise ValueError("form binary: maximum artifact size exceeded")
    if not data.startswith(MAGIC):
        raise ValueError(f"form binary: bad magic {data[:8]!r}")
    position = len(MAGIC)
    count, position = _read_u32(data, position)
    if count > FORM_BINARY_MAX_STRINGS:
        raise ValueError("form binary: maximum string count exceeded")
    strings: list[str] = []
    total_string_bytes = 0
    for _ in range(count):
        length, position = _read_u32(data, position)
        total_string_bytes += length
        if total_string_bytes > FORM_BINARY_MAX_STRING_BYTES:
            raise ValueError("form binary: maximum string bytes exceeded")
        if length > len(data) - position:
            raise ValueError("form binary: truncated string")
        end = position + length
        try:
            strings.append(data[position:end].decode("utf-8"))
        except UnicodeDecodeError as error:
            raise ValueError("form binary: invalid utf8") from error
        position = end
    root, position = _decode_node(data, position, tuple(strings), _DecodeBudget(), 0)
    if position != len(data):
        raise ValueError("form binary: trailing bytes")
    return root


def dump_form_binary(path: str | Path, root: FormBinaryNode) -> None:
    Path(path).write_bytes(encode_form_binary(root))


def load_form_binary(path: str | Path) -> FormBinaryNode:
    return decode_form_binary(Path(path).read_bytes())


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
]
