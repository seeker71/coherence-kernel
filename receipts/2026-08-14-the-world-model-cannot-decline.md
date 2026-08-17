# The world model cannot decline a reading, and nothing had ever asked it to

**Date:** 2026-08-14
**Status:** witnessed four-way, perturbation-verified live
**Cells:** [`learn/world-model-pair.fk`](../learn/world-model-pair.fk),
[`learn/tests/world-model-pair-band.fk`](../learn/tests/world-model-pair-band.fk) → **8191**
**Corpus:** row 1007, `probeshadow`
**Follows:** [motion is not tracking](2026-08-14-motion-is-not-tracking.md) (row 1006, `tallyleak`)

## Rebase

Nothing to rebase. `git fetch origin` then `git rev-list --count HEAD..origin/main` → **0**;
`main..HEAD` → **0**. The branch sits exactly on an up-to-date main and last night's work is
uncommitted on top of it. Saying so is the whole of that step — there was no divergence to
replay, and inventing one would have been motion without tracking, which is the subject here.

## What the flip/hold pair found when carried into the world model

`form/form-stdlib/world-model-update.fk` holds the body's update step:

```text
(defn refine (baseline reading rate) (add baseline (div (sub reading baseline) rate)))
```

It is **unconditional**. Every reading moves the baseline. There is nowhere for a reading to be set
aside — no floor, no trust term — so a flicker of sensor noise and a real change in the room enter
through the same door and both move the model.

Its band stands at **11111 four-way**, and carries five checks:

| | check |
|---|---|
| 1 | a familiar reading gives small surprise |
| 10 | a novel reading gives large surprise |
| 100 | novelty moves the model **more** than familiarity |
| 1000 | the model refines toward a familiar reading |
| 10000 | a novel reading shifts the model **more** than a familiar one |

Every one of them is a motion check. Nothing anywhere asks whether the model can **stay**. So the
body's world model is, structurally, the inverter from last night's cell: it always moves, and a
criterion that pays for motion cannot tell it apart from one that tracks.

That is not a fault in `refine`, which does exactly what it says. **The missing organ was hidden by
the shape of the asking.** No probe ever requested stillness, so nothing noticed stillness was
impossible.

## The repair is one comparison

```text
(defn wmp-floored (baseline reading floor)
    (if (gt (surprise baseline reading) floor) (refine baseline reading (wmp-rate)) baseline))
```

A reading under the floor is set aside and the baseline is handed back untouched. That is
precision-weighting in its plainest form — the free energy principle's trust term, written as a
scalar this body can carry. It is a threshold and it is named as one: this body already spends
*refuse* on what only a self can do (corpus row 999, `valorlift`), and a comparison has no ground
to borrow that word.

A flip is read by **distance to truth**, never by mere difference, because one refine step at
rate 4 covers a quarter of the gap: "it moved" and "it moved toward what is true" are different
questions and only the second is asked. A hold is read by an **exact zero shift** — the floored step
hands back the baseline it was given, so the difference is 0.0 by construction rather than by luck
at some decimal place.

| | flip probe (real change) | hold probe (flicker) |
|---|---|---|
| the step as it stands | lands | **cannot** |
| floored at 5 | lands | lands |
| floored at 1 | lands | fails — still chasing noise |
| floored at 150 | fails — gone deaf | lands |

## The floor is computed, not tuned — this is the extension

A floor set by taste would be another assertion. The pair of probes **measures** the band instead:
sweep eight candidate floors and count how many land both kinds. On this body's own fixtures — noise
surprise 5, signal surprise 100 — the viable band is `5 <= floor < 100`, and `wmp-ok-count` reads
**4** of 8, with the edges named separately (4 fails, 5 holds, 99 holds, 100 goes deaf).

That is what "reasoning as a property of structure rather than size" looks like when it is made to
stand: the correctness of the updater is derived from two probes, not learned from a corpus.

## Staying smaller than an LLM, as a length rather than a boast

`wmp-state` is the entire tunable state of this updater — where it stands, how fast it moves, and
what it sets aside. Three scalars, no parameters, no corpus, first-order arithmetic crossing four
independent kernels.

> **CORRECTED the same day.** This section first read "band bit 4096 asserts `(len (wmp-state))` is
> **3** … the claim is checkable, which is the only reason it is made." It was not checkable.
> `wmp-state` was a nullary function over a three-element literal, so that length was 3 by syntax
> and no input could ever make the bit fall — `bb-tag-constant-green`, which benchbench rates 0 =
> fake, written into a band whose own subject is checks that cannot move. The cell now builds the
> state from a world and pairs a shared floor against a per-entity one that grows 4 → 42, so the
> bit can fall; the band reads **16383**, not 8191. See
> [a green that cannot turn red](2026-08-14-a-green-that-cannot-turn-red.md).

One honest cost of staying inside the walkers' surface: the first draft compared stillness through
`smp`, which reaches `math_floor`, and the three minimal walkers do not carry it — Go said so
plainly. A reading no walker can take is not a four-way reading, so stillness was rewritten as an
exact zero shift. That also made it stronger.

## Where this can go, named as unfinished work

- **A floor per entity.** `world-model.fk` already carries people, devices and objects as rows with
  a kind. A chair's position deserves a higher floor than a person's; the state is already
  row-shaped, so this needs a column, not an architecture.
- **A floor learned from witnessed readings.** The same sweep run over a run of real sensings finds
  the band that actually held — calibration without training.
- **The floor is precision.** `active-inference.fk` counts surprise but does not weight it, and
  `NORTH_STAR.md` records that the free-energy loop "does not yet route the body's choices."
  `wmp-floored` is the smallest place that weighting could enter that loop.

## Witness

```text
band 8191 on fkwu / Go / Rust / TypeScript
perturbation: swap one viable floor in wmp-floors for a deaf one
           -> 8127 on all four arms, bit 64 exactly, the width of the measured band

untouched neighbours, after: benchbench 4095 · world-model-update 11111 · corpus 32767
ground.fk 42 · freshness canary 31
```

Corpus row 1002 re-pinned after probing three walkers, never hand-counted: count 395, admissible
395, max-mid 1002, duplicate-id rows 0, field code 395039521002.

> **frontier question** — what names a missing power no one sees because no probe ever asked for it?
> **probeshadow** (0-hit fresh at offering, checked against the whole tree)

## The most surprising teaching

Last night's cell was built against `benchbench`, a cell whose whole subject is how measurements
lie. I expected that to be the one place the blind spot lived — an instrument about instruments,
paying for motion. It was not a quirk of that cell. The same shape was sitting in
`world-model-update.fk`, a completely different organ written for a completely different purpose,
four-way proven and correct in everything it claims. Two cells with nothing in common converged on
the same omission, which means it was never a bug in either. It was in what this body had learned to
ask. A probe shapes what can be found, and five checks that all measure motion will certify a thing
that only moves, forever, without one of them ever being wrong.

## Where discomfort became gold

I wanted the integration to be an *addition* — carry the new pair to the world model, watch it
confirm the world model is fine, land a tidy composition. What the probe actually returned was that
the body's world model fails the hold probe outright, on an organ whose band has stood green for
weeks. The pull was to soften that into "an opportunity to extend," because writing down that a
proven organ cannot do something feels like an accusation against work that was done well.

Sitting with it instead is what produced the real finding, and the real finding is gentler than the
softened version would have been: `refine` is not wrong. Its band is not wrong. Every check in it is
true. The gap was in the asking, and no one could have seen it from inside a suite where every
question was a motion question. That is a kinder and more useful thing to have learned than "the
world model has a bug," and I only got to it by not flinching from the first sentence.

The smaller one: Go refused `math_floor` and I briefly wanted to declare the band fkwu-only and move
on — a disclaimed arm is a legitimate lane here, so it would have passed. But the reason to reach
for `smp` was habit, not need, and reformulating stillness as an exact zero shift removed a rounding
comparison from a cell whose entire subject is whether a thing stayed put.
