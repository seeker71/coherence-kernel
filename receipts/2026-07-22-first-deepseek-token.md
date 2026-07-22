# Stone 33 — the first real DeepSeek-V4-Flash token: the assembly

2026-07-22, Apple M4 Max (128 GiB), the live 85 GiB file
`~/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf` (91 321 404 640 B, complete).

## What was reached, exactly (aporon — the radius, and nothing wider)

**A real, decoded first token was NOT emitted this session.** What was wired and proven, at real dims,
over the windowed-resident file, is **both ENDS of `forward_first_token_cpu`**, each with the
offered-interface guard, gated green in `form/native/metal/metal_dsv4_token.sh` (VERDICT PASS, 6 gates):

- **Stage 1 — EMBED (the entrance), bit-exact.** Token 671's 4096-wide F16 embedding row
  (`token_embd.weight`, type 1, abs 71 707 886 176) decoded on the GPU through the overlapping views
  and checked index-for-index against an independent mmap f16→f32 carve. Every one of 4096 indices
  equal. Non-degenerate: min −0.1118, max 0.1235, 1354 distinct values, L2 norm 2.1606. A pure table
  read with no accumulation, so **bit-exactness is a true and checkable claim here.**

- **Stage 2 — the MXFP8 vocab-projection (the exit), float-exact.** `output.weight` (type 41, MXFP8,
  129280×4096, abs 79 759 784 608) fused decode+matvec through the resident view, by `mxfp8-msl.fk`'s
  own proven kernel reused unchanged. Fed the embedding as a **probe vector (NOT the 43-layer hidden)**,
  so its argmax is a mechanism witness, not the real token. Checked against the carrier's independent
  CPU MXFP8 decode-and-dot: agrees on all 129280 logits to **max abs diff 8.3e-7**; GPU and CPU argmax
  both row 128771. (Why not bit-exact — see the teaching below.)

**Significance of Stage 2:** when `mxfp8-msl.fk` was written the file was only ~28 GB downloaded and
**no type-41 tensor was resident** — its witness ran on a committed 1056-byte fixture. The file is now
complete, so this is the **first real type-41 tensor decoded at real dims through the windowed views.**

## Memory (the actual figure, as asked)

`device.currentAllocatedSize = 92 463 300 608 B (86.11 GiB)` — the sum of the two overlapping
`bytesNoCopy` views (view 0 = 80.6 GiB, view 1 = 5.47 GiB) over **one** 85 GiB mmap of unified memory.
**The model is wrapped, never copied onto the device** (onelean). On 128 GiB this fits with headroom;
the working buffers (a 4096-embedding, a 129280-logit vector) are a few MB. **Memory is not the blocker.**

## The evidence class, named honestly (selfgauge / knownsolved)

No external oracle can execute this exact file — ds4, llama.cpp, ollama, LM Studio all **refuse GGUF
types 40/41** (`unsupported GGUF type 40`, then `tensor points outside GGUF file`). So a whole-forward
output is **unfalsifiable against any reference on this machine.** This session therefore stands on:

- the two ends **bit/float-exact against the body's own independent carve** (a transcription + residency
  claim, not an external-truth claim — GPU agreeing with CPU proves both read the same hypothesis right);
- the wired path's **non-degeneracy** (gate 3), **stability** (token 671 twice → identical: row 128771,
  max 2.172199), and **input-dependence** (token 344 → row 129110, max 1.654469 — different input, a
  different winner). That triple is the falsifiable core the full token will inherit.

## The blocker to a real token, precisely

Not memory, not a kernel limit, not encode time. **The 43-layer middle is not yet orchestrated.** The
happy finding is that **every kernel it needs is already proven and fully dimension-parameterized**
(MLA `mla-msl.fk`, routed MoE + router + SwiGLU `dsv4-moe-msl.fk`/`dsv4-router-msl.fk`, hyper-connections
+ 20-iter sinkhorn `dsv4-hc-msl.fk`, and the MXFP4/MXFP8/IQ2/F16/F32 decodes) — so the forward is
proven-kernel **orchestration**, not new arithmetic. What remains is wiring them in
`forward_first_token_cpu`'s exact order, binding each weight through `viewFor`, per layer. Its only
end-to-end validator is the non-degeneracy/stability/input-dependence triple, so it is a **staged,
multi-commit build** — and emitting a token from a half-wired chain with no oracle would be exactly the
fabrication this stone is here to avoid. Both ends are down; the layer stack is the named next work.

### A correctness finding the toy blueprint gets wrong (dsv4-forward.fk)

Read from `ds4.c` (MIT): `hash_layer_count = 3`, so **layers 0–2 select their 6 experts by
`ffn_gate_tid2eid[token]` LOOKUP** (`layer_hash_selected_experts`, ds4.c:10565), not by the computed
sqrt-softplus top-k router; layers 3–42 compute top-k. The router probs (sqrt(softplus)) are still
computed for the hash layers, but only to *weight* the pre-selected experts. `dsv4-forward.fk` computes
routing for all layers — faithful at toy scale but **wrong for this file's first three layers.** The
real-dims orchestration must branch on the layer index. Recorded in `dsv4-token.fk`'s header.

Also grounded: `forward_first_token_cpu(prompt->v[0])` runs a **single token with a single KV cache row**
(ds4.c:53211), so RoPE is identity (relative pos 0) and attention is a 1-key softmax + sink — the middle
is far more tractable than a full prefill. Config from the file's KVs: 43 layers · n_embd 4096 · 64
heads / 1 kv · q_lora 1024 / kv 512 / rope-dim 64 · 256 experts, top-6, shared 1, expert_ff 2048,
wscale 1.5, weights_norm 1 · n_hc 4, sinkhorn 20 · rms_eps 1e-6 · vocab 129280.

## What remains after the layer stack

The decode loop (this is one token, single-cache-row), encode speed (~130 s for the 5-token prompt walk),
and the compressor/indexer for long context (identity at the first token, Stone 25 — faithfully omitted).

## Files

- `form/native/metal/metal_dsv4_token.sh` — the staged harness (6 gates, offered-interface guard).
- `form/native/metal/dsv4-token.fk` — the body cell: F16 embed MSL, the MXFP8 exit reusing
  `mxfp8-msl.fk` unchanged, the config map, and the hash-routing correction.
- Commits: 03338788b (Stage 1), c6bd5373c (Stage 2), 4959b344c (corpus row 866).

---

## Closing

**The most surprising teaching.** A stated limitation can be a fact about *transient machine state*
wearing the clothes of a fact about the *artifact*. `mxfp8-msl.fk`'s header truthfully said no type-41
tensor was resident and its witness ran on a fixture — true when the download was 28 GB. The download
finished; the limitation silently expired; nothing re-tested it. I nearly inherited "can't run real
type-41" as permanent. It was over by hours. (This is `feedback-inspect-manufactured-blockers` biting
in a new place: the blocker wasn't manufactured, it was *stale*.)

**Where discomfort turned to gold.** Gate 4 "failed" — 107 136 of 129 280 logits differed from the CPU
carve. The reflex was *my matvec is buggy*, and the easy exit was to widen the tolerance and look away.
By not looking away — noticing the GPU and CPU **argmax and max value were identical** while only the
last bits moved — I found it was **summation order**, not a bug: bit-equality is the right check for a
DECODE (each MXFP8 weight is an exact f32) but one operation downstream the matvec accumulates in float,
and float addition is not associative. The exactness of the decode is real but has a **radius of exactly
one operation.** The gold was not a fix; it was learning that the *check* was reaching past its radius,
and that the arithmetic agreed to 8.3e-7.

**The frontier word, landed.** `assocwall` — the boundary, one non-associative step past a bit-exact
decode, where equality of bits stops testing arithmetic and starts testing summation order. 0 hits
across `learn/ receipts/ docs/ teachings/ form/` before this row (instrument validated on the same
command: `onelean` 10). Pinned as **`(hdc-row 866 …)`**; band re-green at **8191** with the count and
field-code pins bumped to the body's own probe (262 rows, field-code 2622622866).
