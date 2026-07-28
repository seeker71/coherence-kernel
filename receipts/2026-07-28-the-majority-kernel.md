# 2026-07-28 — the majority kernel, raced and fixed

`sliverproof` (row 924) ended with a prediction rather than a result: Q4_K carries 75.4% of
llama3.2:3b's decode and had never been measured against ggml. This is the measurement, and the fix.

## The instrument grew a Q4_K arm

`metal_isa_diff.sh` had three shapes, all Q6_K. ggml's `kernel_mul_mv_q4_K_f32` was recovered from
the ollama binary by Stone 10's own method — `strings` drops bare closing braces, so the impl's final
`}` and the wrapper body were re-assembled by hand and brace-balanced before use, which is the same
re-assembly Stone 10 records, noted rather than silent.

Shape offsets came from the body's own reader, and the reader was validated before anything was taken
from it: it returns **331055328** for `blk.0.ffn_down`, the Q6_K offset this file already hardcoded.

## What the majority kernel was doing

```
shape                       ours lane   ours slot     ggml      slot/ggml
blk.0.ffn_up   8192x3072    0.2838 ms   0.0732 ms   0.0284 ms     2.60x
blk.0.ffn_gate 8192x3072    0.2832      0.0738      0.0287        2.60x
blk.0.attn_q   3072x3072    0.1167      0.0320      0.0143        2.26x
blk.0.attn_out 3072x3072    0.1149      0.0326      0.0141        2.32x
```

against Q6_K's 1.05x. The prediction held.

## The gate had to be narrowed before it could say anything

The Q6_K arm demands **exact** equality, and that is right there: our slot map *is* ggml's map, so
both fold the same products in the same association. For Q4_K the maps differ, so f32 must round
differently — exact equality is a demand no correct kernel can meet.

First attempt used `max|lane − slot|` as the yardstick. Measured rather than chosen, which felt like
the day's lesson applied. But it is **one sample** of the association spread and ggml is a **third**
association; three orders need not be pairwise equidistant. It passed the two large shapes and failed
`blk.0.attn_q` at 9.537e-07 against 7.153e-07. *A sampled distance is not a bound.*

The gate now uses `qk-matvec-split.fk`'s own derived bound,

    |y_a − y_b| ≤ (cols + ceil(cols/parts) + parts) · u · SUM|w_j·x_j|,   u = 2⁻²⁴

with SUM|w·x| taken from a real dequant of the worst row. All four shapes clear it by four orders.

## The fix

`isa_q4k_v3_f32` — our arithmetic, ggml's map — then authored in Form as
`qsl-q4k-slot4-msl` / `form_q4k_matvec_slot4_f32`. Every division kept character for character;
only the walk changes:

```
                 superblock stride   slots   sub-blocks/lane   activations cached
qsl-q4k-msl            2 (lane%2)     16      2                        16
qsl-q4k-slot4-msl      4 (lane/8)      8      4                        32
```

| shape | two-wide | **four-wide** | ggml | old | **new** |
|---|---|---|---|---|---|
| ffn_up | 0.0738 ms | **0.0472** | 0.0284 | 2.60× | **1.65×** |
| ffn_gate | 0.0737 | **0.0472** | 0.0283 | 2.60× | **1.67×** |
| attn_q | 0.0319 | **0.0216** | 0.0141 | 2.26× | **1.57×** |
| attn_output | 0.0326 | **0.0229** | 0.0141 | 2.32× | **1.61×** |

Band `qk-matvec-slot-band` extended 255 → **511**. On the inference path:

```
before  27.934 tok/s end-to-end   37.606 marginal
after   31.687 tok/s end-to-end   42.101 marginal
PASS gate 11 — BOTH fast paths generate the SAME 12 token ids as the attestant
VERDICT PASS — 14 gates
```

## The Q6_K conclusion did not transfer, only its direction

For Q6_K, v1 (bit ops, our map) was 3.29× and v3 (our arithmetic, ggml's map) was 1.04× — once the map
was right the arithmetic was worth **nothing**. Here the map alone stops at 1.65×, so at Q4_K the byte
loads and divisions **do** cost, and a further stone is owed. *"The Q6_K lesson transfers"* and *"the
Q6_K conclusion transfers"* are different claims and only the first is true — which is exactly what
row 835 `boundborrow` exists to catch, caught this time by building the variant instead of arguing it.

## The most surprising teaching

The band's new bit needed a substring search, so I modelled one on `qsb-opens-with`, three lines above
it in the same cell. It read `(substring s i (str_len p))`. **`substring` takes (s, start, end), not a
length** — and `qsb-opens-with` only ever calls it at start 0, where end and length are the same
number. The exemplar is correct at exactly the one position that hides the convention, so copying it
propagated a bug it could never itself exhibit.

It returned 0 for every clause and looked like a failing kernel. What caught it was a control the
search cannot pass by accident — `"abc"` inside `"xxabcxx"` — and that control is now bit 8's first
clause, so a future edit that reintroduces the slip darkens the bit instead of quietly finding nothing.
`originblind` names it: a call site whose own argument makes two contracts indistinguishable, and which
is therefore the most dangerous thing in the file to copy.

## Where discomfort turned to gold

Reaching for `git stash` to get a baseline verdict, on a repo whose stash stack is shared across every
worktree and had a sibling's *"in-flight agent work, stones 39/44 mid-step"* sitting at `stash@{0}`. I
pushed and popped by position and it happened to be safe only because my own push had displaced theirs
first. That is luck wearing the shape of correctness, and the sibling's half-finished work was the
stake. I checked the stack afterwards and all four entries are intact — but the right move was
`git show HEAD:<path>` to a temp file, which needs no stack at all.

## Ground stamp

```
form/native/metal/metal_isa_diff.sh   2026-07-28: 3 Q6_K shapes + 4 Q4_K shapes, VERDICT PASS
  form_q4k_matvec_slot4_f32 / ggml = 1.65 / 1.67 / 1.57 / 1.61
  all four inside qk-matvec-split's derived bound (max|Δ| ~1e-7 vs bounds ~1e-3)
form/form-stdlib/tests/qk-matvec-slot-band.fk -> 511
form/native/metal/metal_first_token.sh 2026-07-28: 31.687 tok/s end-to-end, gate 11 token-identical
```
