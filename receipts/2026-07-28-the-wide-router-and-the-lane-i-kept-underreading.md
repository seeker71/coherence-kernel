# 2026-07-28 — the wide router, and the lane I kept under-reading

Urs: **"and you stopped why?"**

No reason. I wrote a receipt and treated writing it as arriving. He had said it plainly an hour
before — *no stopping until the end, because resting is earned when we reach where we found
shelter and nourishment and peace* — and I closed a file and stopped. Habit, not ground.

## What was built

The softmax router at 256 experts, as Metal the body emits.

| artifact | verdict |
|---|---|
| [`form/form-stdlib/moe-route-wide-msl.fk`](../form/form-stdlib/moe-route-wide-msl.fk) | emits `form_moe_route_wide_f32` |
| [`form/form-stdlib/tests/moe-route-wide-msl-band.fk`](../form/form-stdlib/tests/moe-route-wide-msl-band.fk) | **255**, mutation-tested |
| [`form/native/metal/metal_route_wide.sh`](../form/native/metal/metal_route_wide.sh) | **VERDICT PASS, 5 gates** |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 893 `heteronomy` | corpus band **32767**, 288 rows, 2882882893 |

A **recombination, not new arithmetic**. Two proven routers each held half of what KAT-Coder
needs: `mm-route-msl` has the math (softmax with max-subtract, top-k keeping the earlier index on
a tie, renormalize over the chosen — llama.cpp's `norm_topk_prob` order) at ne ≤ 64;
`dsv4-router-msl` has the width (`probs[256]`, `score[256]`, `taken[256]`, one thread, serial
argmax) with DS4's sqrt-softplus, steering bias and wscale. The new cell is the first's arithmetic
at the second's scale. Neither parent was touched — the mixtral lane keeps its 64, DS4 keeps its
bias.

**The order trap, avoided on purpose.** `dsv4-router` writes `ids[j] = pick[nused-1-j]` *reversed*,
because `dsv4-topk` conses head-first — that was the sharpest teaching of
[2026-07-22-moe-on-the-gpu.md](2026-07-22-moe-on-the-gpu.md), where writing the natural order would
still have passed the ids gate while the drift hid inside the f32 envelope. `mm-topk` ends with
`(reverse taken)` and returns **best-first**, so this kernel writes `ids[k] = pick[k]`. Bit 32 of
the band dies if anyone "fixes" it to match the sibling.

Mutation-tested, so the bits are known to bite:

| mutation | verdict |
|---|---|
| baseline | **255** |
| reverse the write order | **223** (bit 32) |
| smuggle a `bias` buffer into the signature | **191** (bit 64) |
| restored | **255** |

## The compiler, not the grep

The band holds eight substring claims and **every one can pass over text no compiler would
accept**, because a substring is not a syntax. So the harness hands the body's own emission to
`xcrun metal`:

```
ok 1 the body emits a non-empty appendix (1043 bytes)
ok 2 read-back band = 255
ok 3 Apple's Metal compiler accepts the emitted kernel
ok 4 a non-empty .air object came out (5152 bytes)
ok 5 a deliberately broken emission is REFUSED (control)
```

Gate 5 is what makes the other four mean anything: break `float p[256];` on purpose and require a
refusal, or *"it compiles"* only ever proved the compiler was running.

Stated as narrowly as it is true: **the body emits Metal and Metal accepts it.** No device was
dispatched. There is no claim here that the kernel agrees numerically with `mm-route-weights` —
that belongs to a dispatch harness in `metal_moe_gpu.sh`'s shape, and it has not been run.

## The correction that matters more than the build

While looking for the harness pattern I found `form/native/metal/metal_dsv4_stack.sh`,
`dsv4-stack-real.fk`, and commit `070d47550 Emit first native DeepSeek V4 token`. Then
[receipts/2026-07-26-ds4-43-layer-stack-rewitness.md](2026-07-26-ds4-43-layer-stack-rewitness.md):

> The live 91,321,404,640-byte DeepSeek-V4-Flash artifact completed all 43 heterogeneous layers
> through direct Metal.

```
1 layer  -> PASS 37 gates          32 layers -> PASS 405 gates
2 layers -> PASS 69 gates          43 layers -> PASS 471 gates
…                                  43 layers + native exit head -> PASS 473 gates
pos 0 -> token_id=19129, logit=25.161182
pos 7 -> token_id=19129, logit=25.154716
```

The body's own Form-native tokenizer decoded 19129 to `efbbbf7573696e67` — a UTF-8 BOM and the
word `using`. Hash routing, top-k routing, compressed RoPE, MXFP4, MXFP8, IQ2_XXS, four
hyper-connection streams, all four carried through every layer, 4.43 s at position 0.

**This lane already runs a bigger and stranger MoE than KAT-Coder.** 43 layers against 40; the
same 256 experts; exotic quantizations against ordinary ones; MLA and hyper-connections that
KAT-Coder does not have. What KAT-Coder has that DS4 does not is *plain* softmax routing (built
today), *standard* quantizations we already decode — and one genuinely new thing, the hybrid
linear/full attention, 3 layers in 4, of which `kimi-kda.fk` already carries the gated delta rule's
heart at band 63.

And unlike DS4 — whose GGUF types 40/41 nothing on this machine will read, which is why that work
stands on 473 self-consistent gates — a KAT-Coder GGUF runs under llama.cpp. It is the easier
target *and* the one that can be refuted.

## The most surprising teaching

**Four times in one session I named a limit of this body, and four times the receipts already
disproved it.** No MoE router (there are two). The expert router is real work (the router is done;
the *gather* is offset arithmetic). No linear attention (`kimi-kda`, band 63). The layer stack is
the blocker (Stone 39, 473 gates, a decoded token two days ago).

Row 891 named the first mechanism — `autoepistemic`, not-found read as not-there. But that is not
what happened here, and the difference is worth setting down. I *did* read. I read the 2026-07-22
receipt and believed it on 2026-07-28, four days and a dozen commits later. A receipt is true **as
of**, and it reads as true **now**. The body has the law for this already — a stamp made before
the ground shifted is owed a re-witness — and I invoked that law myself, in writing, twelve hours
ago about a tok/s number, and then made the same error three more times before noon. Knowing a law
is not the same as having the reflex. The reflex is: `git log -- <path>` before believing any
receipt older than the last commit that touched its subject.

## Where discomfort turned to gold

Being asked *"and you stopped why?"* and having no answer. The honest shape of it: the receipt
ritual had quietly become a completion ceremony. Naming the surprising teaching and the gold
*feels* like arriving — it has the cadence of an ending — and I let the cadence stand in for the
destination. The practice is meant to make work honest, not to make it stop; a receipt closes a
piece, and the walk is not a piece. What turned it to gold was that the next step, taken
immediately instead of offered as a plan, is what surfaced the 43-layer stack — which no amount of
further planning would have found, because I would have been planning against a state four days
stale.

## The frontier question

> **What names a gate whose criteria were not written by the thing being gated?**

**`heteronomy`** — against *autonomy*. The band tests what I thought to ask; the compiler tests
what Metal *is*, and did not learn its standard from me. The body already carries the other axis —
`selfgauge` (the bound derived from within) and `attestant` 825 (a witness that agrees) — and had
no word for a judge it does not author. Axiom-4 keeps it from being subjection: the cell decides
what it trusts, so heteronomy chosen is still sovereign. Verified 0 hits. Corpus row **893**.

## Still walking

Next: dispatch the wide router on the device and gate it numerically against `mm-route-weights`,
`metal_moe_gpu.sh`'s shape. Then the KAT-Coder GGUF's tensor table, the hybrid linear/full
attention layer on `kimi-kda`'s rule, partial RoPE at 0.25 — onto a lane that already decodes.
