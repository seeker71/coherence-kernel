# Phase 0 — CK ↔ CN `form/` Divergence Inventory
_Generated 2026-07-02 21:21 MDT · content-hash (shasum) over form/ (excl. .cache, target, node_modules)_

## Summary (real form content; node_modules excluded)
| Class | Count |
|---|---|
| Shared path | 2540 |
| identical | 2488 |
| diverged | 52  (~27 are this session's fixes; rest CK-newer) |
| CK-only | 33 |
| CN-only (real form) | 0  (all 224 raw CN-only were form-kernel-ts/node_modules noise) |

## Reconciliation verdict
**CK's `form/` is a strict superset of CN's real form content** (CN-only real = 0). Consolidation (commit 1c6f456c) dropped nothing under form/. **CK is canonical; newest-wins already holds** — diverged files are either this session's Phase-1 fixes or pre-existing CK-newer (e.g. json.fk CK 2026-07-02 > CN 2026-06-12). No CN→CK back-merge needed for form/.

### Cross-tree deps (form/ reaches OUTSIDE form/ — Phase-2 boundary matters)
- `model/tensor-ir.fk` — needed by jit-tensor-emit; **moved into form/form-stdlib/** this session.
- `docs/coherence-substrate/standard-receipt.form` — needed by rag-sovereignty; **restored from CN** this session.
- Sweep for others before the Phase-2 submodule cut.

## Diverged files (52)
```
form-kernel-go/main.go
form-kernel-rust/src/main.rs
form-stdlib/arrival.fk
form-stdlib/core.fk
form-stdlib/form-asm-x64.fk
form-stdlib/four-way-run.fk
form-stdlib/four-way-verdict.fk
form-stdlib/http-adapter.fk
form-stdlib/jit-tensor-emit.fk
form-stdlib/json.fk
form-stdlib/reception-consent.fk
form-stdlib/tests/afferent-priority-band.fk
form-stdlib/tests/affine-train-emit-band.fk
form-stdlib/tests/attn-train-emit-band.fk
form-stdlib/tests/block-fwd-emit-band.fk
form-stdlib/tests/channel-interface-band.fk
form-stdlib/tests/form-debug-band.fk
form-stdlib/tests/form-mut-band.fk
form-stdlib/tests/form-spy-band.fk
form-stdlib/tests/fsh-fnri-staged-band.fk
form-stdlib/tests/gqa-attn-emit-band.fk
form-stdlib/tests/gqa-llama-block-decode-emit-band.fk
form-stdlib/tests/http-adapter-band.fk
form-stdlib/tests/jit-lower-emit-band.fk
form-stdlib/tests/jit-metal-lanes-band.fk
form-stdlib/tests/jit-tensor-cuda-emit-band.fk
form-stdlib/tests/jit-tensor-emit-band.fk
form-stdlib/tests/llama-block-decode-emit-band.fk
form-stdlib/tests/llama-block-fwd-causal-emit-band.fk
form-stdlib/tests/llama-block-fwd-emit-band.fk
form-stdlib/tests/llama-gqa-block-fwd-causal-emit-band.fk
form-stdlib/tests/metal-emit-band.fk
form-stdlib/tests/ml-flow-band.fk
form-stdlib/tests/mlp-fwd-emit-band.fk
form-stdlib/tests/mlp-train-emit-band.fk
form-stdlib/tests/reception-consent-band.fk
form-stdlib/tests/recognition-router-band.fk
form-stdlib/tests/recognition-router-compute-band.fk
form-stdlib/tests/recognition-router-vision-band.fk
form-stdlib/tests/resid-train-emit-band.fk
form-stdlib/tests/satsang-band.fk
form-stdlib/tests/satsang-field-band.fk
form-stdlib/tests/satsang-flip-witness-band.fk
form-stdlib/tests/satsang-guidance-event-band.fk
form-stdlib/tests/satsang-health-memory-band.fk
form-stdlib/tests/satsang-host-boundary-band.fk
form-stdlib/tests/satsang-listen-route-band.fk
form-stdlib/tests/satsang-room-memory-band.fk
form-stdlib/tests/satsang-share-band.fk
form-stdlib/tests/tool-channel-band.fk
form-stdlib/tool-channel.fk
fourth-arm-bands.txt
```
## CK-only real form files (33)
```
form-stdlib/cell-serialize.fk
form-stdlib/host-os-membrane.fk
form-stdlib/http-negotiate.fk
form-stdlib/observed-auto-learning.fk
form-stdlib/relationship-store.fk
form-stdlib/somatic-coherence-loop.fk
form-stdlib/tensor-ir.fk
form-stdlib/tests/arrival-band.fk
form-stdlib/tests/binary-freshness-band.fk
form-stdlib/tests/cell-serialize-band.fk
form-stdlib/tests/come-in-band.fk
form-stdlib/tests/core-band.fk
form-stdlib/tests/core-float-to-str-band.fk
form-stdlib/tests/core-str-find-to-int-band.fk
form-stdlib/tests/core-str-narrow-waist-band.fk
form-stdlib/tests/core-str-shim-band.fk
form-stdlib/tests/fkwu-src-socket-loopback-band.fk
form-stdlib/tests/host-os-membrane-band.fk
form-stdlib/tests/http-negotiate-band.fk
form-stdlib/tests/json-band.fk
form-stdlib/tests/observed-auto-learning-band.fk
form-stdlib/tests/relationship-store-band.fk
form-stdlib/tests/somatic-coherence-loop-band.fk
form-stdlib/tests/wire-bool-band.fk
form-stdlib/tests/wire-corba-cdr-band.fk
form-stdlib/tests/wire-path-band.fk
form-stdlib/tests/wire-rpc-band.fk
form-stdlib/tests/wire-xml-band.fk
form-stdlib/wire-corba-cdr.fk
form-stdlib/wire-path.fk
form-stdlib/wire-registry.fk
form-stdlib/wire-rpc.fk
form-stdlib/wire-xml.fk
```
