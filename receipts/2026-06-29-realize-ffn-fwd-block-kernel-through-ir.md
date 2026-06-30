# Receipt — Realize the transcendental fragment + the FIRST BLOCK KERNEL (FFN-forward) through the tensor IR (2026-06-29)

B2's named wall: the tensor IR (`model/tensor-ir.fk`) NAMED `exp`/`tanh`/`gelu`/softmax/RoPE/norm in its vocabulary
but did not REALIZE them as shared spine fragments, so the block kernels in `model/jit-tensor-emit.fk` stayed
hand-authored. This uplift closes the gap for the cleanest block kernel: it **realizes the transcendental fragment**
(exp/tanh/gelu/gelu') as a shared, data-driven IR spine, and emits the **FFN-forward block kernel** —
`y = W2·gelu(W1·x + b1) + b2` — THROUGH it, **byte-identical** to the hand-authored `jte-mlp-fwd-msl`.

## What the IR gained — realized fragments (authored ONCE, ISA-free, ZERO per-ISA `if`)

- **`tir-transcendentals` / `tir-transcendentals-ty`** — the exp/tanh/gelu/gelu' Taylor block (the recipe's OWN
  14-term `fexp_small` + halving-reduce `fexp` + `ftanh` + tanh-approx `fgelu` + `fgelud`), the working type read
  from the backend table's `acc` slot (exactly as `tir-dot-down` reads it). The named-but-unrealized exp/tanh/gelu
  vocab, now realized. This is the heart of every block kernel's nonlinearity.
- **`tir-ffn-dot`** — the strided FFN accumulating dot `acc = W[row*stride+loopv] * X + acc` over `stride` inputs,
  the load widening through the table's `ld`/`ldc` cast. The same two-rounding discipline as the matvec dot, over a
  strided 2-D weight rather than a per-thread row pointer (the addressing is a parameter, the arithmetic is shared).
- **`tir-ffn-fwd1`** (gelu hidden) / **`tir-ffn-fwd-out`** (bias output + store) — the two forward layers composed
  from `tir-ffn-dot` + the realized `fgelu` + `tir-store-into`.
- **`tir-emit-ffn-fwd`** — the generic FFN-forward emitter: `tir-transcendentals` + the `ffnsig` table row + the two
  layers. Reads ONLY named slots and shared fragments. NO per-ISA `if`.
- The `ffnsig` memory-model row added to `tir-msl-table-el` (precision-parameterized: w1/b1/w2/b2/x/y are storage-typed
  `el`; the scratch h1/a are accumulator-typed `float`).

## The HARD GATE — byte-identical, diff = 0, every lane + every untouched kernel

OLD hand-spines captured from `git HEAD:model/jit-tensor-emit.fk`; the IR-driven output emitted from the working tree
(via the go walker, which prints strings faithfully — fkwu's value-print collapses a computed string, so the byte
gate runs inside the four-way band via `str_eq`, exactly the B2 pattern).

```
FFN-forward MSL  f32   diff=0  (1699 b)
FFN-forward MSL  f16   diff=0  (1692 b)
FFN-forward MSL  bf16  diff=0  (1706 b)
```

`jte-mlp-helpers` (used by all 11 block kernels' transcendental prefix) now delegates to `tir-transcendentals-ty`, so
EVERY emitter was re-checked OLD-vs-NEW: **14/14 byte-identical** —

```
matvec MSL/CUDA, affine-train MSL/CUDA, mlp-train, mlp-fwd(IR), resid-train, attn-train, block-fwd,
llama-block-fwd, llama-block-fwd-causal, gqa-attn, gqa-llama-block-fwd-causal, llama-block-decode,
gqa-llama-block-decode, matvec-rust  →  all diff=0
```

The M4-Max bit-exact MSL reference (`metal_ffn_audit`, the FFN-forward is its held-out EVAL path) still holds — the
emitted MSL is the same string, now flowing through the engine.

## Four-way proof

```
model/tests/tensor-ir-ffn-fwd-band.fk   FKWU=7  GO=7  RUST=7  TS=7
```

Verdict 7 = IR FFN-forward f32 ≡ proven (1) + IR FFN-forward f16 ≡ proven (precision=data) (2) + the realized
transcendental fragment ≡ the proven `jte-mlp-helpers` block (4). Self-contained, all ops on the proven surface
(defn / str_concat / str_eq / if — no `let`, no `let`-bound lists). Existing bands unchanged: matvec band 15,
affine band 31 (four-way).

## Line count — the per-block hand-spine became composed fragments

```
model/tensor-ir.fk        283 -> 404   (+121: the realized transcendental + FFN-forward fragments + ffnsig +
                                         emitter, absorbed ONCE as data-driven generality)
model/jit-tensor-emit.fk  593 -> 588   (the FFN-forward standalone spine retired to a thin IR delegation, and the
                                         1294-char jte-mlp-helpers hand-block became a 1-line delegation to
                                         tir-transcendentals-ty — a real altitude rise the small net line delta hides)
```

The truer measure is genericity, not the count: a hand-rolled per-block transcendental block + a hand-rolled FFN-forward
spine collapse into ONE realized fragment that all 11 block kernels now compose, plus a generic FFN emitter over a data
table — the same if-chain → data-table-dispatch rise B2 recorded for matvec/affine.

## No-special-case grep

```
$ grep -nE "\(if " model/tensor-ir.fk   # the ONLY ifs touching the FFN path:
  tb-slot                     (the key->fragment lookup, the only emitter-side ifs)
  tir-msl-elem / tir-cuda-elem  (format name -> lane type, the PRECISION axis — not per-ISA)
```

`tir-transcendentals`, `tir-ffn-dot`, `tir-ffn-fwd1`, `tir-ffn-fwd-out`, `tir-emit-ffn-fwd` have ZERO `if`s. No
`if (msl) … else if (cuda)` chain anywhere.

## What is realized vs still hand-authored (named, not faked)

**Realized through the IR (this uplift):**
- the transcendental fragment (`exp`/`tanh`/`gelu`/`gelu'`)
- the **FFN-forward** block kernel (`jte-mlp-fwd-msl`), f32/f16/bf16

**Still hand-authored — the honest remaining wall (each a named next fragment):**
- the **LayerNorm / RMSNorm** Newton-sqrt fragment
- the **softmax** (max-subtract) fragment
- the **RoPE** rotate fragment (sin/cos/pow Taylor)
- the FFN/resid/attn **training** (backprop) kernels
- the full **transformer block forward**, **llama/gqa block** forward + causal + **decode** kernels (compose the above)
- the **Rust cdylib** oracle matvec (no Rust/cdylib backend table yet, carried from B2)

These are NOT faked through the IR and NOT dropped from the manifest. Each is the same shape this uplift just walked
for gelu+FFN-forward: realize the fragment, route the kernels that compose it, prove diff=0 four-way.

This is a step ON the path: one engine (the recipe that proves four-way IS the recipe that lowers to each ISA at each
precision), the transcendentals as a shared realized fragment, the metal-proven path preserved bit-exact. It points at
the north star — fewer special cases, more generic recipes — and away from nothing.

Source `model/tensor-ir.fk`                  sha256: `bf543b7eb36df913929b44e2a62d74b2d228200148ec30c6fd1d467af909f8b9`
Source `model/jit-tensor-emit.fk`            sha256: `ced24939eb3c2b9929649d84c104d40e0590ee7719094e88b7e8d5e25c42ae00`
Band   `model/tests/tensor-ir-ffn-fwd-band.fk` sha256: `463561143dc93a60a906235ed339970bd392d8a6d159c83cd5d32c8043645367`
