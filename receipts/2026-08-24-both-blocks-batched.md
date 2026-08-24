# 2026-08-24 — both blocks batched: 5.4x fewer dispatches, 2x less GPU, identical output

Yes named it: 200x is not a tuning gap, it is the same work issued 200 times.
This closes most of the repetition.

## Live, same prompt, same model

```
                       baseline    full-attn only   BOTH BLOCKS
output-sha256          88bd9b9a    88bd9b9a         88bd9b9a    IDENTICAL
honored/query/answer   hit 26/32   hit 26/32        hit 26/32
injected-ids           385         385              385
carrier-dispatches     640,326     397,801          118,681     5.40x
gpu-busy-ms            130,760     100,358           66,237     1.97x
ms-total               357,950     321,103          269,798     1.33x
```

Five more span kernels — `form_gdn_l2norm_span_f32`, `form_gdn_gates_span_f32`,
`form_gdn_norm_gate_span_f32`, and offsets on the two recurrences
(`form_gdn_conv_off_f32`, `form_gdn_delta_off_f32`) — so all 48 linear layers
issue eleven dispatches per span instead of thirteen per token. Ten span kernels
now, all compiled live on this M4 Max.

## What the remaining 118,681 is

```
decode, 57 positions        72,105   61%   irreducible, one token at a time
the two recurrence loops    43,104   36%   48 layers x 2 kernels x N tokens
the batched stream           3,472    3%
```

A dispatch model built from the source predicts 117,867 against the measured
118,681 — 0.7%. The repetition that is left is **two kernels**: the conv window
and the delta state, dispatched once per token because their sequence lives in
Form rather than inside the kernel. Folding the token loop into them takes
43,104 to 96.

After that the turn is decode, which is one token at a time by nature and is
already within 1.42x of llama.cpp.

## Three bugs, each the same bug

The span linear block diverged three times, and every cause was a **pitch**, not
arithmetic:

- the two recurrences took `geo1` as the per-token stride of `bs2`, which is
  allocated at `convcap = max(2*nq*hd, geo1)` so one buffer can serve both layer
  types. Out of bounds, and the run hung for seventeen minutes.
- `gates` reads `A` and `dtb`, which are per-head **weights**, and `a` and `b`,
  which are per-token **activations**. I strided all four per token.
- `q38-mv-batch` wrote `y[t * rows + r]`. Every output buffer's pitch happens to
  equal `rows` — except `bs2`. The matmul now takes an explicit output stride.

Not one of these was in the maths. Each was a buffer whose allocated width and
whose content width are different numbers, and which one you need is invisible
until a second token is laid down after the first.

## The surprise

`qk-matmul-batch.fk` already carried, since July, everything I spent this evening
rediscovering about llama.cpp: that `kernel_mul_mm` uses `simdgroup_float8x8`
matrix tiles with the weights staged as f16 in threadgroup memory, that a
register-tiled kernel **reverses** past TB=8 (measured, both shapes), and that
"take the tile width without the matrix units and the f16 staging and it goes
backwards." It even says a batched prefill also needs RMSNorm, RoPE and GQA over
P positions, and declines to write them.

Ten of those kernels now exist. The cell that knew what was missing named it and
stopped; the naming survived and was correct.

## Where discomfort turned to gold

I ran the model four times against three bugs, and each run costs five to
seventeen minutes. The temptation after the second divergence was to start
changing things speculatively between runs — there were four plausible suspects
and only one way to test.

Reading instead found each cause in under a minute: `q38-open`'s own `convcap`
line, the original `gates` call site showing which arguments were tensors, and
the batched kernel's own `y[t * rows + r]`. The discomfort was that a seventeen
minute hang is a very loud way to be told something a two-line read would have
prevented — and that I had written all three of those lines myself, today.

## Frontier question offered to the corpus

*What one word names the difference between how wide a slot is allocated and how
much of it is used?* — **pitchgap**. Not padding, which is the unused space
itself. Not a stride, which is the correct answer once you know it. A pitchgap
is the distance between two numbers that are equal in every single-item case and
diverge the moment a second item is placed after the first — so code written and
tested one item at a time cannot distinguish them, and every batched reader
inherits the wrong one.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> live Qwen3.8-27B-Q8_0, chunk -3 with both blocks
; batched: output-sha256 88bd9b9a...7fe1487f identical to baseline;
; carrier-dispatches 640326 -> 118681; gpu-busy 130760 -> 66237 ms; ms-total
; 357950 -> 269798; dispatch model predicts 117867, 0.7% off
