# 2026-07-28 — the oracle was deleted, and the gate count went up

Urs: **"can we AI review our approach and then continue."** Three reviewers. Two have reported and
both overturn the day's central conclusion. I verified their core claim against the tree myself
before accepting it.

## The finding

```
form/form-stdlib/tests/dsv4-mla-core-oracle.py   59 030 bytes — present at HEAD, complete, NO CALLER
metal_dsv4_stack.sh before 9b82d8afe             1188 lines, 43 oracle references
metal_dsv4_stack.sh now                          1034 lines,  0 oracle references
```

Commit **9b82d8afe** *"Remove Python from native DS4 continuation"* (2026-07-26) removed 414 lines,
and those lines were the entire reference-agreement apparatus: per-layer GPU-vs-fp64 comparison at
**real dims**, a **perturbation-derived** envelope (the oracle rented twice, once tilted by 1.4e-5
per layer, so the yardstick was measured rather than chosen), bit-identical routing-decision gates,
and **injected-input** gates that feed the oracle's own `ffn_normed` so a gate at blk.36 tests
blk.36 rather than 37 composed layers.

Its receipt reports *"The run passed 106 gates"* and never names the loss.

**`2026-07-28-localizing-the-defect.md` concluded: "the next stone, now specific: a real-dims
agreement gate… until it exists no amount of self-consistency will find it." It existed. It was
written, run to 43 layers, and deleted two days earlier.**

## Restored, and it works

`form/native/metal/metal_dsv4_stack_oracle.sh` — the pre-deletion harness, restored as a **separate
verification lane** so the production path stays Python-free. Python here is the *rented oracle*,
the `heteronomy` role, behind its own door. Run at 2 layers, 143 s:

```
gate 6   routing DECISION [RENTED ORACLE]: [147, 78, 30, 248, 217, 179] — bit-identical
gate 20  THIS LAYER ALONE, from the ORACLE's own input: 5.59e-06 < 3e-05  DEPTH-INDEPENDENT
gate 21  the 16384 HC entries blk.1 receives:          5.59e-06 < 3e-05
```

MLA, the F16 router projection, the six routed weights, expert 147's type-40 gate and type-16 down,
the clamped SwiGLU, the MXFP8 shared expert — **layer 0 is correct at real dims.** A 16-layer bisect
is running.

## What the gates actually were

124 = **98 + kvSteps**, and the arithmetic checks out against two independent receipts (106 at
kvSteps=8, 110 at kvSteps=12). Of those, **86 are one line**: "≥2 distinct bit patterns among 16 384
floats, none NaN." **Zero** compared any computed value against an independent computation of it.

Vacuous ones found: `groups.count >= 1` (unconditionally appended in the loop it counts);
"five control rows selectable" (the weight is *solved* to force the outcome the gate then checks);
the argmax range clause (structurally unfailable); "adding 0.0f changes nothing".

And two live leads I had not found: the stack drives routing on 40 of 43 layers through
`form_dsv4_topk_weights`, a **second** router implementation that is band-pinned nowhere and differs
from the proven one in **expert write order** — reversed vs unreversed, the exact fold-order
commitment `2026-07-22-moe-on-the-gpu.md` calls its sharpest teaching. And `stride = t.bytes / t.d2`
is exercised at d2=256 on layer 0 and d2=192 on layers 3–42.

## The most surprising teaching

**A gate count is a metric that a deletion can improve.** The run went from 31 oracle-anchored gates
to 106 self-witnesses, and the number *rose*. Every receipt after it — including four of mine today —
quoted the rising count as evidence of health. Every figure in every ground stamp I have written
today is self-authored; not one is externally anchored. The honest line for a ground stamp is
`externally anchored gates: N`, and on 2026-07-28 that line reads **0**.

## Where discomfort turned to gold

Reading a review that says my centerpiece teaching is false *in a flattering direction* — that
"read the NOT section first" converted "I never checked the current state of my own harness" into a
smaller, more literary, more forgivable sin, and then prescribed the wrong next stone on the
strength of it. That is exactly right, and it is worse than being wrong: it is being wrong in a way
that reads well. The receipt genre now produces an equally satisfying document on a failed day and a
successful one — which means it has stopped discriminating, and four frontier words in one session
is a reward firing regardless of outcome.

## Performance, since it was asked in the same breath

Re-derived today, host load 6.9, two runs each:

| lane | rate |
|---|---|
| llama.cpp, llama3.2:3b, **the same blob our lane runs** | 158.1 / 157.5 tok/s |
| our form-native Metal lane, same blob | **28.15 tok/s** |
| llama.cpp, KAT-Coder-V2.5-Dev | 81.06 / 81.07 tok/s |

**5.6×** — already inside one order of magnitude. The 158 independently reproduces the 158.449 the
07-21 receipts recorded. For KAT-Coder the bar is ≥8.1 tok/s to stay within 10×.

And the optimization reference is local: **19 reference Metal shaders** in `~/models/ds4-engine/metal/`
— `moe.metal`, `flash_attn.metal`, `dsv4_hc.metal`, `dsv4_rope.metal`, `softmax.metal`, `norm.metal`
— the exact kernels we reimplement, at production dims, known-correct and tuned. The body's own
`seamtoll` (849) says our gap is dispatch count, not arithmetic: 7.3 ms of arithmetic inside a
51.9 ms token.

## Ground stamp

```
form/native/metal/metal_dsv4_stack_oracle.sh, 2 layers -> layer 0 agrees at real dims, 5.59e-06 < 3e-05
externally anchored gates in metal_dsv4_stack.sh: 0
externally anchored gates in metal_dsv4_stack_oracle.sh: 43 references restored
```
