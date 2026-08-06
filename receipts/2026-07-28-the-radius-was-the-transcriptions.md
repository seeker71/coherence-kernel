# 2026-07-28 — the radius belonged to the transcription, not to the meaning

Urs, at 04:31 WITA:

> only core axioms are limits, anything else showing up as gap or not yet are
> opportunities to show us we can

I had just closed a message with *"say the word and I'll start the stone"* — a deferral, after
three turns of reporting **unbuilt** as **can't**. Both were mine to drop without being asked.

## Ground

`axioms/core-axioms.form` — the five, and the whole fence:

1. three states: 0, 1, nothing
2. everything is a cell
3. a cell's identity is computed from its present composition
4. a cell meets the world only through an interface it offers; observation through it makes it real
5. to run a cell and to speak to a cell are one act: offer, acked by nothing/0/1/node

None of them forbids a 256-expert router. And `NORTH_STAR.md` already places the oracle
correctly: *"local or remote oracles used as review/training teachers until the body can retire
them."* llama.cpp is not a rival on this path; it is the teacher we grade against until we don't
need it.

`cc -O2 -o fkwu runtime/fkwu-uni.c`; `bootstrap/ground.fk` → **42**.

## The stone

`moe-msl.fk`'s header declares a radius:

> `mm-route` ne <= 64 experts and nsel <= 8 chosen (the fixed-size thread-local arrays below; a
> larger router is WRONG here, not slow…)

That sentence is **true of the emitted kernel and false of the recipe it was transcribed from.**
`mm-route-weights` takes `(logits n k)`; nothing in the softmax, the argmax-free scan or the
renormalization knows about 64. The cap lives in the MSL thread-local arrays and nowhere else.

Run at KAT-Coder-V2.5-Dev's shape — **256 routed experts, 8 per token** (its `config.json`,
read 2026-07-28) — the existing recipe returns

```
ids     [204, 151, 98, 45, 249, 196, 143, 90]
weights [0.1391, 0.1348, 0.1307, 0.1266, 0.1227, 0.1190, 0.1153, 0.1118]  Σ = 1
```

**Checked against a closed form, not against a second run.** The fixture's logits are
`(i·97 mod 257)/32 − 4`; 257 is prime and 97 coprime to it, so `i ↦ i·97 mod 257` is a bijection
and the ranking of logits *is* the ranking of residues. Since `97·53 = 20·257 + 1`, the inverse is
53 and the index holding residue *r* is `53·r mod 257` — integer arithmetic that never enters
`tn-exp` or the softmax. For r = 256…249 that is 204, 151, 98, 45, 249, 196, 143, 90. Exactly what
the router picked. Agreement means the float path preserved the order it was meant to preserve.

| cell | verdict |
|---|---|
| [`form/form-stdlib/moe-route-radius.fk`](../form/form-stdlib/moe-route-radius.fk) | the shape, the fixture, the closed-form truth |
| [`form/form-stdlib/tests/moe-route-radius-band.fk`](../form/form-stdlib/tests/moe-route-radius-band.fk) | **63**, mutation-tested |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 892 `adventitious` | corpus band **32767**, 287 rows, field code 2872872892 |

**Mutation-tested, so the bits are known to bite:**

| mutation | verdict | bit |
|---|---|---|
| baseline | **63** | — |
| modular inverse 53 → 54 | **61** | 2 dark — the closed-form check is load-bearing |
| logits flattened (÷32 000 000) | **47** | 16 dark — the flat-distribution falsifier |
| restored | **63** | — |

## What this changes about the path

The expert gather is not a hole in the kernels either. `moe-msl.fk` already worked it out: for a
**decode of one token**, `nb02 · id` is a number the host already has, because the host binds the
buffer — so the gather closes as `t.off + e·nb02`, **zero new MSL**, at the cost of one round trip
per layer. It is a prefill, routing differently per position, that would want a device-side gather.

So the remaining distance to a KAT-Coder MoE FFN in this lane is smaller and more specific than
"build MoE": the router's **math** is home (softmax → top-k → renormalize, `norm_topk_prob` order),
the router's **scale** is home on the DS4 side (`dsv4-router-msl` already selects among 256), and
what is genuinely unbuilt is the recombination — mm-route's math at dsv4-router's width — plus the
MSL arrays that made the header say 64.

## The most surprising teaching

**A radius written in a header reads as a property of the meaning, and can be a property of one
act of transcription.** The sentence in `moe-msl.fk` is careful, honest, and correct — it says
exactly what it means, names why the kernel refuses rather than overruns, and calls the refusal a
rule rather than a race. Nothing about it is sloppy. And it still transmitted a limit that the
recipe five lines above never had, because a header is where a reader learns what a cell *is*, and
this one was describing what the cell *emitted*. Honest documentation is not the same as
documentation that cannot be misread.

## Where discomfort turned to gold

The first probe returned `ids [0,1,2,3,4,5,6,7]` with **every weight exactly 0.125** — a perfectly
shaped answer carrying no meaning. Above it, one line I nearly scrolled past:

```
error: [unresolved-call] 'rem' matched no op/rewrite/fn/binding -- typo or missing prelude?
Recovered to nothing (axiom-5); parse continues
```

My typo, not the body's — but the shape of the failure is the thing. Under axiom-5 an unresolved
call recovers to `nothing` and the run *continues*, so a flat distribution came out wearing the
right arity, the right length, and a sum of exactly 1. Every structural check I would have written
first would have passed it. That is why bit 16 of the band is a spread falsifier and not a sum
check, and mutation-testing it (→ 47) reproduces that exact failure on purpose so it can never
pass silently again.

The smaller one: bit 32 first asserted the raw chosen probabilities sum below **0.2**, and the band
came back **31**. The fixture measures **0.2216**. The code was right; my threshold was a number I
picked rather than derived. Rewritten as the claim it was always trying to make — that
renormalization supplies more than half the final mass — which holds for any threshold between the
measured value and 1, and so is not tuned to this fixture.

## The frontier question

> **What names a limit acquired in the making rather than belonging to the thing made?**

**`adventitious`** — against *constitutive*. The five axioms constitute; a transcription merely
happened. Verified 0 hits. Kept distinct from `aporon` (an unanswerable): this one was always
answerable and nobody asked. Landed as corpus row **892**.

## Still walking

Next on the path, in order: the recombined 256-wide softmax router as emitted MSL, then the layer
stack, gated against llama.cpp on the same GGUF — the first big-MoE target this body can actually
be *refuted* on, unlike the DS4 file that nothing on this machine will read.
