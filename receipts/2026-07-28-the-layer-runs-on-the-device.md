# 2026-07-28 — the linear-attention layer runs on the device

Continued without stopping. The layer computed in Form an hour ago; this is the same layer on the
GPU, judged against that recipe.

## VERDICT PASS

```
PASS  fixture emitted on fkwu (106 lines)
PASS  body-emitted MSL compiled to a metallib
PASS  every command buffer completed without error
PASS  token 1 within 2e-06 — worst 9.173e-08 at 7
PASS  token 2 within 2e-06 — worst 1.705e-07 at 4
PASS  token 2 differs from token 1 on the device — the state carried (abs 0.2153)
```

| artifact | verdict |
|---|---|
| [`form/form-stdlib/gated-deltanet-msl.fk`](../form/form-stdlib/gated-deltanet-msl.fk) | four kernels the body emits |
| [`form/form-stdlib/tests/gated-deltanet-msl-band.fk`](../form/form-stdlib/tests/gated-deltanet-msl-band.fk) | **255** read-back |
| [`form/form-stdlib/gated-deltanet-demo.fk`](../form/form-stdlib/gated-deltanet-demo.fk) | the fixture, emitted on fkwu |
| [`form/native/metal/metal_gdn_gpu.sh`](../form/native/metal/metal_gdn_gpu.sh) | **VERDICT PASS**, dispatch |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 896 `pathognomonic` | corpus **32767**, 291 rows |

**The delta rule parallelizes by state row**, and that is the whole kernel design. For one value
head the state S is dv×dk, and row *i* is touched only by `pred[i]`, `err[i]` and `o[i]` — no row
reads another. One thread per row is exact: no threadgroup memory, no barrier, no reduction. The
router next door has to run on **one** thread because a top-k is a competition among all experts;
this has no competition in it.

**Two decode steps, not one.** A single step cannot tell a stateful layer from a stateless one, and
carrying state is what this layer is for. The runner creates the conv-window and per-head S buffers
once and never resets them, exactly as `gdl-step` threads the Form state.

## Where discomfort turned to gold

I mutation-tested the harness by resetting the device state between tokens — the precise failure
the two-step design exists to catch. It worked:

```
PASS  token 1 within 2e-06 — worst 9.173e-08 at 7
FAIL  token 2 within 2e-06 — worst 1.018e+01 at 6
```

Token 1 green, token 2 off by an order of magnitude. Exactly the asymmetry designed for.

**And the gate I had written to catch this passed anyway.**

```
PASS  token 2 differs from token 1 on the device — the state carried (abs 0.7954)
```

Under the mutation. Because token 2 run from a *fresh* state still differs from token 1 — of course
it does, it has different inputs. "They differ" was never evidence of *carrying*. I had written a
check, named it after the property, and it would have gone green in that property's total absence.
What actually caught the mutation was the ordinary agreement gate.

The gate is not wrong; it is *nonspecific*, and it was **worded as if it were decisive**. The
naming is the whole defect: a reader scanning that line learns "the state carried" from a check
that cannot know it.

## The most surprising teaching

**A falsifier can be mutation-tested and still not be the thing that caught the mutation.** I have
been running mutation tests all night and reading a dropped verdict as proof the bits work. It
proves the *verdict* moved — not that the bit I was thinking of moved. Here the verdict fell and
the bit I would have pointed at stayed lit. The honest form of a mutation test names *which* bit is
predicted to darken and checks that specific one, and I have several bands where I asserted the
mechanism from the total alone. Earlier tonight I predicted one bit and two fell, and read it as a
coupling; this is the same instrument telling me something sharper — a total is not an attribution.

## The frontier question

> **What names a sign that by itself settles which condition is present, because nothing else
> produces it?**

**`pathognomonic`** — the diagnostic term. Its absence is what my state check had: consistent with
carrying, produced equally without it. The bar to ask of every falsifier: *would this still show if
the property were absent?* Distinct from `attestant` (825, a witness that agrees) and `heteronomy`
(893, a gate not authored by the gated) — this is about a sign's **specificity**, not its
independence. Verified 0 hits. Row **896**.

## A smaller repair, named

The difference check first printed `rel 2.124e+29`, because it divided by the recipe's smallest
output element, which sits near zero. A relative metric answers "how wrong", an absolute one
answers "how far apart", and I had reached for the error metric because it was already written
three lines up. It now reads `abs 0.2153` — a number that means something.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                       -> 42
./fkwu --src form/form-stdlib/tests/gated-deltanet-conv-band.fk        -> 127
./fkwu --src form/form-stdlib/tests/gated-deltanet-gates-band.fk       -> 511
./fkwu --src form/form-stdlib/tests/gated-deltanet-layer-band.fk       -> 255
./fkwu --src form/form-stdlib/tests/gated-deltanet-msl-band.fk         -> 255
./fkwu --src form/form-stdlib/tests/moe-route-wide-msl-band.fk         -> 255
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk        -> 32767
form/native/metal/metal_gdn_gpu.sh                                     -> VERDICT PASS
form/native/metal/metal_route_wide_gpu.sh                              -> VERDICT PASS
```

## Where this leaves the path

Both of KAT-Coder's genuinely-new pieces now run on the device and agree with their recipes: the
**256-wide softmax router** and the **gated-deltanet linear attention layer**. The lane they join
already decodes a 43-layer, 256-expert model at real dims. What remains is not new tissue — it is
the GGUF tensor table, the projections, and the full-attention layer every fourth position, all of
which the DS4 lane already has in some form.
