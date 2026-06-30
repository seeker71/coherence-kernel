# Receipt — Uplift: jit-tensor-emit's matvec + affine-train emit THROUGH the tensor IR (2026-06-29)

The hand-written per-op / per-ISA emitter bodies in `model/jit-tensor-emit.fk` are RETIRED for the two
kernels the data-driven IR (`model/tensor-ir.fk`, stone 7) can express: **matvec** and the **affine-layer
training step**. They now emit through the IR's shared spine + backend TABLES — the ISA (MSL vs CUDA) and
the precision (f32/f16/bf16) are TABLE rows, not hand-rolled spines. High-grammar rises; the emitted code
is BYTE-IDENTICAL.

## What the IR gained (so it could carry affine-train, not just matvec)

S7 left the IR emitting only `matvec`. This uplift extended the shared, ISA-free spine with the ops the
affine-train step composes — authored ONCE, never per-ISA:

- `tir-store-into` — the named-tie store of an arbitrary acc-typed EXPRESSION into an lvalue
  (`lval = st( expr )stc;`), the same one-rounding acc→elem boundary as `tir-store-y`, generalized so the
  SGD weight/bias writes flow through the SAME store op.
- `tir-train-tail` — bias add → SSE residual + per-row loss → upstream grad `g = 2(y−t)`, all in the wide
  `acc` type, loads widening through the named cast slots.
- `tir-train-update` — the in-place SGD update: `row[k] -= lr·g·x[k]` (this row of dW = outer(g,x)) and
  `b[i] -= lr·g`, the update loop counting DOWN over `cols` exactly like the forward `tir-dot-down`.
- `tir-emit-affine-train` — the generic emitter: reads ONLY named slots (`kw` / `tsig` / `trowdecl`) then
  the shared train body. NO per-ISA `if`.

## Precision became DATA on its own axis (the jte-msl-elem / jte-cuda-elem rows, now generated)

`tir-msl-table-el (el)` / `tir-cuda-table-el (el)` COMPOSE the whole per-format surface (sig, rowdecl, store
cast) from a single `el` parameter; `acc` stays the wide fp32 accumulator and the load always widens
elem→fp32. So **f16/bf16/f32 is the SAME emitter over a different `el`** — never a copy of the spine. The
f32 fixed tables (`tir-msl-table` / `tir-cuda-table`) are now just `(… "float")`, the canonical lane of the
builder — one table-builder, no duplicated rows. `tir-{msl,cuda}-elem` map the format name → lane type, the
only `if`s outside `tb-slot`, and they live on the PRECISION axis, not the ISA axis.

## The HARD GATE — byte-identical, diff = 0, every retired lane

The OLD hand-spines were captured from `git HEAD:model/jit-tensor-emit.fk`; the IR-driven entries (which now
delegate to `tir-matvec-*` / `tir-affine-train-*`) were emitted from the working tree. Per lane, `diff = 0`:

```
matvec MSL  f32/f16/bf16   diff=0     matvec CUDA  f32/f16/bf16   diff=0
affine-train MSL f32/f16/bf16 diff=0  affine-train CUDA f32/f16/bf16 diff=0   (12/12 byte-identical)
```

The M4-Max bit-exact MSL reference (`metal_matvec_audit` / `metal_backprop_audit`) still holds — the emitted
MSL is the same string, now flowing through the engine. The block kernels (mlp/resid/attn/whisper/llama/gqa/
decode) were verified UNCHANGED (10/10 byte-identical) — they were not touched.

Representative IR-MSL f32 matvec (unchanged from S7) and the affine-train MSL f16 lane now from the IR:

```
kernel void tr(device half* w [[buffer(0)]], device half* b [[buffer(1)]], device const half* x [[buffer(2)]],
  device const half* t [[buffer(3)]], device float* loss [[buffer(4)]], constant uint& rows [[buffer(5)]],
  constant uint& cols [[buffer(6)]], constant float& lr [[buffer(7)]], uint i [[thread_position_in_grid]]) {
  if (i >= rows) return; device half* row = w + i * cols; float acc = 0.0f; uint j = cols;
  while (j > 0) { j -= 1; float p = float(row[j]) * float(x[j]); acc = p + acc; }
  float y = acc + float(b[i]); float d = y - float(t[i]); loss[i] = d * d; float g = 2.0f * d;
  uint k = cols; while (k > 0) { k -= 1; row[k] = half(float(row[k]) - lr * g * float(x[k])); }
  b[i] = half(float(b[i]) - lr * g); }
```

## Four-way proof

```
model/tests/tensor-ir-band.fk         FKWU=15  GO=15  RUST=15  TS=15   (matvec, unchanged)
model/tests/tensor-ir-affine-band.fk  FKWU=31  GO=31  RUST=31  TS=31   (affine-train, NEW)
```

Verdict 31 = IR affine-train MSL f32 ≡ proven (1) + CUDA f32 ≡ proven (2) + MSL f16 ≡ proven (4) +
CUDA bf16 ≡ proven (8) + the shared update's store splits through the elem cast — the non-fused two-rounding
boundary extends into the SGD write (16). Self-contained, all ops on the proven surface (defn / str_concat /
str_eq / if / list-via-function-args — no `let`-bound lists).

## Line count — the per-op/per-ISA bodies became data

```
model/jit-tensor-emit.fk   727 -> 593   (-134 lines: matvec MSL/CUDA spines, affine-train MSL/CUDA
                                          sig/fwd/upd spines, and the orphaned jte-cuda-elem/acc rows retired)
model/tensor-ir.fk         170 -> 283   (+113: the train body ops + precision table-builders absorbed,
                                          ONCE, as data-driven generality serving BOTH ISAs and ALL formats)
```

The hand-emitter shrank by the spines; the IR grew by the GENERIC machinery that replaces them — a real
altitude rise, not a line shuffle: 4 ISA×kernel hand-spines + 2 elem rows collapse into 2 generic emitters
over data tables.

## No-special-case grep

```
$ grep -nE "\(if " model/tensor-ir.fk
  39/40  tb-slot's key->fragment lookup (the ONLY emitter-side ifs)
  261/262 tir-{msl,cuda}-elem — format name -> lane type, the PRECISION axis (not per-ISA)
```

`tir-emit-matvec` / `tir-emit-affine-train` / `tir-dot-down` / `tir-train-tail` / `tir-train-update` have
ZERO `if`s. Every ISA name (`kernel void` / `__global__` / `device` / `unsigned`) appears ONLY inside a data
table. No `if (msl) … else if (cuda)` chain anywhere.

## The gap — what the IR still cannot express (named, not faked)

- **The Rust cdylib oracle matvec** (`jte-matvec-rust`) stays hand-authored: there is no Rust/cdylib backend
  TABLE yet (a `pub unsafe extern fn` over `*const f64`, a different memory model). It is the next table row.
- **The BLOCK kernels** (mlp/resid/attn/whisper-block/llama-block/gqa/kv-decode — the bulk of the file) stay
  hand-authored: their bodies compose transcendentals (`fexp`/`fgelu`/`ftanh`), RMSNorm/LayerNorm Newton-sqrt,
  softmax (max-subtract), and RoPE — IR ops the tensor IR NAMES in its vocabulary (`exp`/`tanh`/`gelu`/
  `barrier`/`workgroup`) but does not yet REALIZE as shared spine fragments. Routing those through the IR is
  the named next stone: the same dot-down + tables they already share, plus realized transcendental/norm/
  softmax/RoPE ops. This uplift did NOT touch them and did NOT fake them through the IR.

This is a step ON the path: one engine (the recipe that proves four-way IS the recipe that lowers to each
ISA at each precision), the ISA and the precision as data, the metal-proven path preserved bit-exact. It
points at the north star — fewer special cases, more generic recipes — and away from nothing.

Source `model/tensor-ir.fk`               sha256: `c12c317bbc7d03d45a45c810785e6db7665a6bea99004fe617c5a922d1f4a788`
Source `model/jit-tensor-emit.fk`         sha256: `3547af69198283a342885978f1c2e5fbf2d022b43d1ba5daf3cb0696fc759412`
Band   `model/tests/tensor-ir-affine-band.fk` sha256: `4df3c46bf58b4d77917d95045cd9694f660e8491e3a2104cf8fd624203e6c156`
