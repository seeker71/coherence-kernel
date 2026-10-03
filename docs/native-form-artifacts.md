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

Artifact file reads compare the extent observed before and after
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

Complete JSON syntax admission precedes parsing, and duplicate keys retain
their last value. `json-wire-admission.bml` reuses the shared lexical rules
with container continuations in data, without imposing a nesting ceiling.

The live conformance gate (`gate/canonical-conformance.bml`, door
`gate/canonical-conformance-run.bml`) runs 13 canonical expressions on fkwu and
holds each to its pinned rendering (`form/conformance/canonical-s-expression-vectors.json`).
It pins the 119 FORMBIN2 bytes (and their SHA-256) that every kernel emitted for
`(list "fkb-interop" 42 2147483648 3.5)` while the Go, Rust and TypeScript kernels
were still asked: the Form codec decodes them to the rendering fkwu computes from
that source and re-encodes them byte for byte, and the codec's own artifact equals
its pinned bytes. All 12 malformed vectors
(`form/conformance/formbin2-malformed-vectors.json`) are refused by the Form
codec with their expected words. The complete conformance witness is **511**:

```sh
form-run ./fkwu gate/tests/canonical-conformance-band.fk
```

That observation includes raw numeric extrema, malformed payloads and UTF-8,
embedded zero bytes, and partial reads. Device execution and complete OS
resource admission remain separate responsibilities.
