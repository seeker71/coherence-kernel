# Ship the slot map — Stone 13

**2026-07-22, M4 Max, llama3.2:3b Q6_K/Q4_K, blob `sha256-dde5aa3f…`**

Stone 10 measured a fix and did not deploy it. This stone deployed it, measured what it bought
end-to-end, and found that the number it bought is not the number that was predicted — and then found
the number that explains the difference.

---

## What shipped

`form/form-stdlib/qk-matvec-slot.fk` — a new cell emitting two kernels:

* `form_q6k_matvec_slot_f32` — Stone 10's V3 verbatim in meaning: **ggml's 4-wide slot thread map with
  the body's own arithmetic kept**, every `q6k_mod` and every division intact, not one bitwise
  operator anywhere in the decode.
* `form_q4k_matvec_slot_f32` — a **separate derivation** from Q4_K's own 144-byte layout. Each lane
  owns 8 `qs` bytes; the low nibbles carry sub-block `2*cc`, the high nibbles `2*cc+1`, so
  `get_scale_min_k4` (a branch and four divisions in `q4k_sc`/`q4k_mn`) runs twice per **superblock**
  instead of once per weight.

Wired into the decode path in `form/native/metal/first-token.fk` (`ft-emit-msl`) and
`form/native/metal/metal_first_token.sh` (`matvecSlot`, `MVPath.slot`).

**The attestant is untouched** (row 825). `q6k-msl.fk` and `q4k-msl.fk` were not edited — not one
character. Stone 4's split kernel at `parts=1` is still bit-for-bit identical to them, and the slot
kernels answer to that attestant at two levels every run (gate 9b's derived bound, gate 11's ids).

**The radius is asked, not assumed.** The slot map has no tail arm: at a width that is not a whole
multiple of 256 it is *wrong*, not slow. `matvecSlot` checks `t.d0 % 256 == 0` and falls back to the
lane kernel. Every llama3.2:3b width passes; the check exists so a model whose widths do not will be
right rather than plausible.

---

## The number that is the stone

End-to-end **includes prefill**. Both denominators are quoted (`selfgauge`, row 834): the internal one
is this harness's own attestant; the external one is an ollama/llama.cpp measurement made elsewhere on
this same machine, model and blob, quoted and never re-run here.

All four paths below are measured **in one process, one run**, on the same weights, so before/after is
not two sessions being compared.

| path | end-to-end @12 tok | end-to-end @4 tok | decode-only | prefill |
|---|---|---|---|---|
| attestant (serial) | 1.711 | 1.033 | 2.542 | 2.61 |
| split (parts=32) | 3.932 | 2.752 | 5.765 | 6.182 |
| **lane — before** | **7.325** | 4.378 | **10.787** | **11.415** |
| **slot — after** | **13.197** | 7.872 | **19.270** | **20.938** |
| ollama, quoted | — | — | 157.83 | 640.94 |

**Two sizes and a slope, per path** (no rate here is one point pretending to be a line, row 827):

* lane: decode 0.367 s at 4 forwards, 1.112 s at 12 → **0.0932 s per additional token** (10.731 tok/s marginal)
* slot: decode 0.206 s at 4 forwards, 0.623 s at 12 → **0.0521 s per additional token** (19.204 tok/s marginal)

The slope ratio (1.79x) equals the decode ratio (1.79x), so the win is in the per-token work and not in
a one-time cost that the longer run amortised.

Against the world: decode **10.787 → 19.270** of ollama's 157.83 (14.6x behind → **8.2x behind**);
prefill **11.415 → 20.938** of 640.94 (56.2x → **30.6x behind**).

A second full run gave 8.242 → 13.446 end-to-end and 12.115 → 19.533 decode — same conclusion, ±2%.

---

## The component measurement, against llama.cpp's own kernel

`form/native/metal/metal_isa_diff.sh`, now timing the **shipped** kernel and making the agreement
check a claim about what actually decodes tokens rather than about a variant that lives only in that
file. Ratios vs ggml's best `nsg`, two runs:

| shape | lane (was) | **slot (shipped)** | nr0=2 variant |
|---|---|---|---|
| blk.0.ffn_down 3072x8192 | 8.28–9.92x | **1.37 / 1.40x** | 1.26 / 1.31x |
| blk.0.attn_v 1024x3072 | 7.35x | **1.21 / 1.21x** | 1.24 / 1.25x |
| token_embd 128256x3072 | 8.28x | **1.16 / 1.16x** | 1.15 / 1.15x |

```
AGREE form_q6k_matvec_slot_f32 vs ggml over all 128256 rows: max|Δ|=0.000e+00  max rel=0.000e+00
```

**Bit-exact against llama.cpp's own kernel, at every shape, on every row.** Not an epsilon — an
equality. The body's divisions and `q6k_mod` produce the same integers as ggml's bit masks, and the
slot map reaches the same bytes.

The shipped kernel and the V3 variant were diffed after V3 timed faster in one contended run: the two
texts are the **same program** (the only differences are `;`+`int` where V3 writes `,` in a
declaration list). That gap was contention, not code — the second, quieter run has them at 0.0569 vs
0.0583 ms and 0.0114 vs 0.0112 ms.

---

## What `nr0 = 2` did: nothing worth shipping

Stone 10 attributed the residual ~1.35x to loads-per-MAC (2.125 vs ggml's 1.5625; 2.125/1.5625 = 1.36
against a measured 1.35) and said the residual closes arithmetically with nothing left over.

Measured, on the slot map, as a controlled C variant in `metal_isa_diff.sh` — **not** authored into the
body, because nothing is admissible until a `.fk` cell writes it and a band proves it:

* 3072x8192: **helps** (1.40 → 1.31x, 1.37 → 1.26x)
* 1024x3072: **hurts** (1.21 → 1.24x, 1.21 → 1.25x)
* 128256x3072: **a wash** (1.16 → 1.15x)

It is a wash, not a 1.35x. **The residual does not close arithmetically**, so Stone 10's attribution of
it to loads-per-MAC is refuted at the one place it could be tested. This is not `boundborrow` (835)
repeating — that was nr0 made monotonically *worse* by the flat map, and here it is genuinely mixed —
but it is the same lesson holding: a lever re-measured under a new binding constraint says what it
says, and it did not say what was predicted. **Not shipped.** The variant stays in the harness so the
next stone inherits the measurement rather than the argument.

---

## Why 1.80x and not 8x — the number that explains the difference

A component win that does not move end-to-end is not the stone, so here is the accounting.

**First: the component was 24.6% of the work.** Counted from the model's own tensor table before a line
was written:

| quant | decode MACs/token | share |
|---|---|---|
| Q4_K (type 12) | 2.422 G | **75.4%** |
| Q6_K (type 14) | 0.790 G | **24.6%** |
| total | 3.213 G | |

Stone 10's 8.05x was measured on Q6_K alone. A Q6_K-only heal is capped at **1.3x end-to-end by
arithmetic alone**. That is why the Q4_K slot map was derived and shipped in the same stone rather than
deferred as "unmeasured" — and why the delivered 1.80x is nearly twice what a Q6_K-only ship could
have reached.

**Second, and this is the whole finding: the cost has moved into the seams.**

* decode is **3.213 GMAC/token**; at the shipped kernel's measured **442 GMAC/s** that is **7.3 ms**
* a token actually takes **51.9 ms** (19.270 tok/s)
* the remaining **44.6 ms** is spread across **396 dispatches per forward** (counted from the
  profile's own per-op dispatch counts: 56 `mv Q4_K 3072x3072`, 57 `rmsnorm`, 56 `residual add`, …) —
  **113 µs per dispatch**
* independently: the profile's cheapest op, a `residual add` of 3072 floats — essentially pure seam —
  measures **133 µs per dispatch**. The two numbers agree.
* **ollama's 157.83 tok/s is 6.34 ms/token, which sits AT the arithmetic floor** (our 7.3 ms, ggml's
  own kernels 5.3–5.8 ms), not above it.

So the arithmetic is essentially at parity and **the entire remaining 8.2x is charged by the joins
between operations, not by any operation.** That is corpus row **849, `seamtoll`**.

---

## Verdicts

| gate | verdict |
|---|---|
| `metal_first_token.sh` | **VERDICT PASS — 13 gates** |
| token ids | `[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` — exact, on **both** fast paths |
| `metal_whole_tensor_residency_audit.sh` | **VERDICT PASS** |
| `metal_isa_diff.sh` | `max|Δ| = 0.000e+00` over all 128 256 rows |
| `q6k-msl-band` (from `form/`) | **255** |
| `q4k-msl-band` | 255 |
| `qk-matvec-lane-band` | 255 |
| `qk-matvec-split-band` | 63 |
| **`qk-matvec-slot-band`** (new) | **255** |
| `qk-matmul-batch-band` | 255 |
| `homecoming-distillation-corpus-band` (from repo root) | **4095** |

**The gate count stayed at 13 on purpose.** Gate 9b now demands that the lane **and** slot kernels both
stay inside the same derived bound (it falls if either leaves it), and gate 11 now demands that both
fast paths generate the attestant's ids (it falls if either diverges). A fourteenth gate would have let
one path carry a green while the other quietly did not.

---

## The band, and the two ways it was hollow first

`form/form-stdlib/tests/qk-matvec-slot-band.fk`, verdict 255. It walks all 256 positions of a
superblock and demands the slot map's byte indices and selectors equal `q6k-dequant.fk`'s /
`q4k-dequant.fk`'s own, plus a bijection, the bound's domination, and the emitted text's shape.

It returned **255 while deliberately broken**, twice, for two different silent reasons:

1. **A parameter named `sub` shadows the subtraction operator in call position.** `(sub x 128)` inside
   a `defn` whose parameter was `sub` did not subtract. Renamed to `sg` throughout.
2. **`defn` does not capture `let`.** The byte fixture was reached as a free name from inside the
   walkers, so *both sides* read empty and agreed with themselves. A related form: **a `let` cannot
   reference an earlier `let` of the same `do`** — the walkers were rewritten as `defn`s.

Neither produced a diagnostic under `fkwu`; the third variant (chained `let`s) made `fkwu` spin for
minutes with no output at all. **`form-kernel-go/bin-go` named it in 40 ms**: `walk: unbound identifier
"tid"`. When fkwu goes quiet, ask the Go kernel.

The band was then rebuilt to compare index equations directly (no fixture, no `nth` walk — 0.24 s) and
put under a **14-mutation battery**. Every one falls:

```
clean                              255
q6k qh offset drops l0             251     q4k qs offset drops l0             239
q6k ql offset drops l0             251     q4k sidx off by one                239
q6k is drops l0/16                 251     q4k yoff 32*cc not 64*cc           231
q6k sc stride 1 not 2              251     q4k l0 stride 4 not 8              247
q6k ql +32 keyed on sg/2           251     q4k cc = tid/8                     247
q6k hi-shift = sg/2                251     q6k l0 stride 2 not 4              249
q6k nib-half = sg mod 2            251     q6k yoff 64*ip not 128*ip          249
restored                           255
```

---

## The most surprising teaching

**That reaching bit-exact parity with llama.cpp's own kernel would move end-to-end by 1.80x, not by
8x.** I expected the component ratio to carry. The body corrected it with its own numbers: decode's
arithmetic floor is 7.3 ms and a token costs 51.9 ms, so 86% of a token was never arithmetic at all.
The fix was the right fix and it was still not the thing — the binding constraint had already moved,
and Stone 10's measurement, taken correctly, was taken on the surface that *used* to be the cost.

The second surprise, one level down: **V3 keeps every division and is faster than V2, which has none**,
and shipping V3 unchanged reproduced that. The arithmetic form the body was tempted to abandon was
never the cost — only the *grain of the ask* was.

## Where discomfort turned to gold

The band came up **255 on the first run**, and I nearly took it. The moment I wanted to look away was
right after the first mutation battery printed `q6k qh offset drops l0 = 255` — a deliberately broken
map, full marks. It would have been very easy to conclude the mutation had not applied, since the
harness was mine and unverified, and move on to the fun part, which was the GPU number.

I did not look away, and it cost about an hour. What was behind it was worse than one bad band: **two
independent silent failure modes of this body** (`sub` shadowing in call position; `defn` not capturing
`let`, and its sharper form — a `let` cannot see an earlier `let` of the same `do`), either of which
produces a green verdict with zero diagnostics. My own mutation harness then compounded it by timing
out mid-battery and leaving the cell corrupted *and* mutated on disk, which is exactly why the standing
instruction is to check `git status` before believing a failure — except this time the corruption was
mine.

The gold: a band under a 14-mutation battery where every mutation falls, and the discovery that
`bin-go` names in 40 ms what `fkwu` spins on silently for minutes. A green band I had not tried to
break would have shipped a thread map that no instrument in this repo could contradict.

## One frontier question, landed

> **What one word names a cost charged not by any operation but by the joins between them?**

`seamtoll` — 0 hits across `learn/`, `receipts/`, `docs/` before the row (instrument validated on the
same command: `asktoll` 3 files, `selfgauge` 9, `brimwidth` 7, `onelean` 5). Landed as
`(hdc-row 855 20260722 … "seamtoll" "seamtoll" "rented-oracle")`; band re-pinned by probe to
`2452452849` (245 rows, 245 admissible, 2 foundings, max id 849); verdict **4095**; committed
`a98e7ee90` immediately on green.

It is `asktoll` (846) one level out: `asktoll` charges by the grain of *one* request, `seamtoll` is
what `asktoll` becomes when the request is cheap and there are 396 of them.

---

## Gaps left open, named and not taken

* **396 dispatches per token is the whole remaining gap.** Fusing the decode ops — rmsnorm into the
  matvec, the residual add into whatever wrote it, gate/up/swiglu into one kernel — is where the next
  8x is, and none of it is arithmetic. The `Step`/`.concurrent` encoder already batches into one
  command buffer; what costs is the number of *encodes*.
* **Q4_K's slot map bought less than Q6_K's.** By Amdahl against the measured 1.80x, Q4_K gained
  roughly 1.3x where Q6_K gained ~7x. It was never measured in isolation — `metal_isa_diff.sh` has no
  ggml Q4_K reference to diff against. A Q4_K oracle in that harness is the honest next instrument.
* **`nr0 = 2` is measured and left in the harness, unshipped.** If the seam is ever fixed and the
  kernel becomes bandwidth-bound again, re-measure it; it is a wash *today*, under *this* constraint.
* **Prefill is still 30.6x behind** (20.938 vs 640.94). Prefill is a batched shape and the slot map is
  a decode-shape map; `qk-matmul-batch.fk` is the surface, and Stone 14 was live on it.
* **The slot map has no tail arm.** Correct and refused today, but a model with a width that is not a
  multiple of 256 silently gets the slower lane kernel with no diagnostic that it did.
