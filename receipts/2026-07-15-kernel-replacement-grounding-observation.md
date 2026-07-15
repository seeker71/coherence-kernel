# Kernel replacement, grounding, and observation receipt

Date: 2026-07-15

## What landed

- `form/form-stdlib` is the sole Form CLI, RAG, and shell source surface; the
  parallel top-level `form-cli/` and `cognition/rag-*` copies are removed.
- Browser and Node consumers share `form/form-kernel-ts/src/kernel.ts` through
  explicit browser and host entry points. The former synthetic TypeScript
  conformance implementation is removed.
- Python is a wire boundary only: strict DTOs plus FORMBIN2 encode/decode. The
  emitted Python parser, evaluator, compiler, decompiler, and rule templates are
  removed.
- `form_table_text`, `string_bytes`, and `string_byte_fold` are registered by
  the Go, Rust, TypeScript, and c-bootstrap siblings. SHA-256 and HMAC consume
  exact UTF-8 bytes through the streaming carrier.
- Grounded RAG results carry a structural NodeID and a computed frequency
  reading. The sufficiency gate can therefore return the complete native trust
  row: native, grounded, frequency-attuned, sufficient, observed.

## Runtime repair and shrink obligation

The committed `form/form-stdlib/bootstrap/fkwu-uni.c` checkout seed grew while
making the new carriers reachable in a fresh clone. This is an explicit
short-lived bootstrap repair, not a new home for semantics. The canonical
meaning remains in Form recipes and the shared native-op manifest; Go, Rust,
and TypeScript carry independent implementations. The seed also gained an
explicit evaluator stack, tail CALL replacement, stable float literals, cached
list lengths, and linear table serialization after the old recursive carrier
exhausted host stack on valid compiler programs.

Shrink obligation: replace emitted C carrier bodies with native walker cells as
the source runner absorbs tags 205-207, then regenerate the seed smaller. Do not
add a second implementation beside this bridge.

## Machine witness

The following commands passed on the changed tree:

- `go test ./...` — Go suite green.
- `cargo test -- --test-threads=1` — 58 Rust tests green.
- `npm run proof` — TypeScript check, browser proof, and Node-host proof green.
- `python3 scripts/verify_kernel_conformance.py` — 12 canonical expressions
  executed by all three real kernels; FORMBIN2 cross-roundtrip and malformed
  rejection matrix green.
- `./scripts/form_cli_bootstrap_proof.sh form-stdlib/bootstrap/form-cli-darwin-arm64`
  — identity, v1/v2 index, exact bytes, 1546-row ranking, batching/caps, request
  HMAC, and replay behavior green.
- Four-way bands green: `string-byte-fold-band`, `sha256-hmac-stream-band`,
  `trust-row-band`, `rag-ask-grounded-band`, `form-cli-request-band`, and
  `form-cli-staged-trace-band`.
- `python3 -m unittest discover -s form/python_bmf/tests -v` — five Python
  boundary and no-duplicate tests green.

The whole historical Form manifest was also read once: 1167 bands agreed and
121 diverged. Those divergences include pre-existing strict blueprint and
ontology mismatches and are not represented here as green. The changed
grounding, crypto, compiler-carrier, browser, and boundary surfaces above are
the scoped witness for this replacement.
