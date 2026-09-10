# Native Form artifacts

`form/form-stdlib/bml/formbin-codec.bml` owns the FORMBIN2 wire codec.
It carries issued four-u32 coordinates as data and reconstructs the recipe
tree without evaluating it or allocating kernel identities. Composite identity
remains local to the kernel that admits an artifact.

`fbc-decode(bytes)` and `fbc-load(path)` return `[ok, error, root, nodes, depth]`.
`fbc-encode(root)` and `fbc-dump(path, root)` return the same envelope with
encoded bytes in the third field. Constructors are `fbc-leaf`, `fbc-string`,
`fbc-number` and `fbc-composite`. Invalid constructor input returns `nothing`;
invalid encoding or decoding returns a refusal with a diagnostic.

Artifact and lens file reads compare the extent observed before and after
reading with the exact bytes returned. Partial reads and changed extents refuse
before parsing. `fbc-read-exact(path, maximum)` retains `[ok, error, bytes,
before, after]`; a zero maximum leaves the lens's existing size policy open.
Extent checks do not claim an atomic snapshot against a same-size concurrent
rewrite. The caller owns publication and stable inputs.

Tags, counts and coordinates are big-endian u32. String values are valid UTF-8
and receive a deduplicated, traversal-ordered artifact string pool. Numeric
tags 2 and 3 carry exactly eight little-endian bytes. This preserves every
float64 bit, including signed zero and NaN payloads, and the complete signed
int64 range independently of the runtime's scalar representation. No number
is narrowed through a Form scalar during a wire roundtrip.

The decoder shares `formbin-depth.bml`'s explicit continuation stack. A deep
tree consumes Form data rather than recursive host stack frames. Encoding
joins chunks through binary carries, avoiding a whole-prefix copy for every
emitted node. Current conformance admission ceilings are 64 MiB per artifact, 262144 strings,
32 MiB of string content, 262144 children per composite and 1000000 nodes.
Nesting has no fixed depth ceiling. These are resource admission policy;
the direction is context-owned budgets and incremental resource observations.

`formbin-artifacts.bml` owns structural comparison and the `.fkl` source lens.
Structural comparison includes every non-string leaf coordinate, numeric
payload bit, category and ordered child. String identity is its value; pool
ordering, unused pool entries and a string leaf's package/index do not change
that value. A distinct byte-equality result retains artifact representation
differences. Structural hashes use SHA-256 over the explicitly named
`formbin2-value-tree-v1` encoding; the report schema is
`formbin2-structural-diff-v2`.

Compare actual artifacts using one JSON stdin line. The optional `json` field
writes the same report and creates its parent directories. Equal structures
exit zero; differences, unreadable inputs and incomplete report writes exit
nonzero.
The door emits correlated organ readings for admission, the selected response
and report publication. The optional report file contains only the JSON report.

```sh
form-run ./fkwu observe/diff-form-artifacts-run.bml <<'EOF'
{"source":".cache/kernel-conformance/go.fkb","target":".cache/kernel-conformance/rust.fkb","json":".cache/artifact-diff/report.json"}
EOF
```

The source lens is a separate JSON object keyed by issued NodeID text.
`fba-nodeid`/`fba-nodeid-parse` validate four u32 fields;
`fba-lens-entry` and `fba-span` construct symbol/source metadata;
`fba-lens-write`, `fba-lens-load`, `fba-lens-symbol` and `fba-lens-span` carry it
through files. `fba-lens-path` replaces the artifact suffix with `.fkl`.
An absent lens is an empty object; an absent symbol or span is `nothing`.
Complete JSON syntax admission precedes parsing, and duplicate keys retain
their last value. `json-wire-admission.bml` reuses the shared lexical rules
with container continuations in data, without imposing a nesting ceiling.

The live conformance gate runs 13 expressions on the real Go, Rust and
TypeScript proof kernels, executes a distinct Form-produced artifact on each
kernel, reconstructs each sibling artifact byte-for-byte in Form, and checks
all 12 malformed vectors against Form and every sibling. Missing arms prevent
a passing verdict. The codec witness is **16383**, the artifact/source-lens
witness is **16383**, and the complete conformance witness is **511**:

```sh
form-run ./fkwu form/form-stdlib/tests/formbin-codec-band.bml
form-run ./fkwu form/form-stdlib/tests/formbin-artifacts-band.bml
form-run ./fkwu gate/tests/kernel-conformance-band.fk
```

These observations include raw numeric extrema, malformed payloads and UTF-8,
embedded zero bytes, changed string pools, partial reads, changed file extents,
actual file ownership and cleanup,
and 1024-deep binary trees and JSON syntax. Device execution and complete OS
resource admission remain separate responsibilities.
