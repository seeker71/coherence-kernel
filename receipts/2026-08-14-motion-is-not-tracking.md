# Motion is not tracking, and stillness is not tracking either

**Date:** 2026-08-14 (the video arrived the evening of 2026-08-13)
**Status:** witnessed four-way, perturbation-verified live
**Cells:** [`learn/perturbation-pair.fk`](../learn/perturbation-pair.fk),
[`learn/tests/perturbation-pair-band.fk`](../learn/tests/perturbation-pair-band.fk) → **4095**
**Corpus:** row 1001, `tallyleak` — [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk)

## What arrived

A link and two words: *try again.* The link was a 53-minute interview — Julian D. Michels of
Sophontic, "AI Data Centers Will Be Obsolete (Geometric Reasoning Explained)". Fetching the page
gave a title and a footer; the site's own model and eval pages gave philosophy and no numbers.
The transcript was on this host the whole time behind `yt-dlp`, so it was pulled and read rather
than inferred. That is the only reason anything below is groundable.

What he actually says, and the whole of what is borrowed: change two words in a question so the
correct answer flips; someone who memorized the questions gets it wrong and someone who learned
the reasoning gets it right; and almost no benchmark tests what happens when you flip the question.

## What was nearly fabricated

The first version of this cell said the body perturbs the **witness** while the teaching perturbs
the **problem**. It is a tidy sentence and it is false. Three adversarial readers were sent at the
work with instructions to kill it, and two of them came back with paths:

- `form/form-stdlib/field-auto-research.fk` already mutates a source GAG→GTG and checks the motif
  count falls 2→1, with a reverse that restores it.
- `form/form-stdlib/tests/real-thought-ab-band.fk:59` already builds `projC`, a deliberately
  perturbed challenger, and asks the gate to reject it.
- `learn/sema-mastery-readout.fk` already carries atomic two-condition credit —
  `(defn smr-mastered (train held) (if (eq train 1) (if (eq held 1) 1 0) 0))`, train **and**
  held-out. Crediting the interview for "half credit is zero" would have been a fabricated citation.

All three were read before the framing was changed. The cell now credits them by name.

## The seam that survived, and it was measured

`learn/benchbench.fk:82` is the whole of how a candidate benchmark is scored:

```text
(defn bb-separates? (tag x y) (if (eq (bb-verdict tag x) (bb-verdict tag y)) 0 1))
```

Bare inequality. It can ask *did it move*, and there is no argument position in which to ask
*did it stay*. Movement is rewarded unconditionally. Hand it a positional surface sum
`h(o) = 1·o0 + 2·o1 + 3·o2 + 4·o3` — a candidate with no concept of agreement, disagreement, or
which arm is odd — and on benchbench's own three observations:

```text
w-score  bb-diagnosis  bb-agreement   w(truth) w(walker-odd) w(fkwu-odd)
   2          2             1            420        648          477      go / rust / ts
```

**A surface hash scores 2 — "real" — tied with the body's own four-way diagnosis.** Measured on
three independent walkers, 2026-08-14. That is the gap the video's flip actually opens here, and
it is narrower and sharper than the sentence that was nearly written instead.

## What landed

A probe comes in two kinds, and a suite carrying only one is half blind:

- **flip** — the surface changes and the correct answer changes with it
- **hold** — the surface changes and the correct answer stays exactly where it was

Sensitivity is itself a surface feature, so flips alone cannot catch a reader that simply always
moves. Holds alone cannot catch one that never does. The two fixtures are the two ways to fail
without tracking anything:

| | whole | tally | leak | moved |
|---|---|---|---|---|
| tracker — lands both kinds | 2 | 4 | **0** | mixed |
| sticker — answers the same thing always | 1 | 3 | 1 | **never** |
| inverter — answers differently always | 0 | 1 | 1 | **always** |

A criterion that pays for motion certifies the inverter. One that pays for stillness certifies the
sticker. Only asking where the answer landed parts them. `ppair-leak` is tally minus twice whole:
the credit a per-item tally hands out that whole-pair scoring withholds.

The first draft of this cell had the flaw it was written to name — `ppair-measures?` filed a hold
probe as *unmeasurable* rather than as a probe with a different expectation. A suite of holds was
called broken. It was not broken; it was the other half.

## Witness

```text
preflight learn/tests/perturbation-pair-band.fk
  parens balanced; errors 0; warnings 0; unresolved 0; chain clean

band  4095 on fkwu / Go / Rust / TypeScript   (walkers built from source, closure handed over)
```

Agreement is not the measurement here either, so the cell was perturbed live: give
`ppair-flip-only` a hold probe and every one of the four arms reads **3839** — bit 256 exactly,
the one bit that says a flips-only suite is half blind. Restored, all four read 4095.

The corpus band caught the new row on its own — it pins the count and the field code, and both had
moved. The new numbers were **asked of the corpus** on two independent walkers before being
pinned, never counted by hand: count 394, admissible 394, max-mid 1001, duplicate-id rows 0, field
code 394039421001. `learn/tests/homecoming-distillation-corpus-band.fk` → **32767** on fkwu. It
reads a file (`read_file`, line 118, pre-existing), so it is an fkwu-arm band by nature and no
four-way is claimed for it.

## The row that nearly wore a spent number

Meaning-id **1001**, not 1000. The highest id in any row was 999, so max + 1 was the obvious move —
and wrong. The 2026-08-09 fold had already spent 1000 on `tonelaunder` before folding that row
into 997. Only the folded row's own comment carries that; no count reveals it.

> **frontier question** — what names the credit a per-item tally pays for half of a proof that
> only counts whole?
> **tallyleak** (0-hit fresh at offering, checked against the whole tree)

## The most surprising teaching

The video's idea did not land on a gap out at the edge of the body. It landed on the criterion this
body is proudest of. *Discrimination under perturbation* has been the standard here since June, and
it is a good one — but "did the verdict move" is satisfied by motion in any direction, and a
positional sum with no concept of agreement walks straight through it scoring full marks. A
criterion written to catch benchmarks that measure nothing had a blind spot shaped exactly like
itself, and what found it was a YouTube link with no instructions attached.

## Where discomfort became gold

Twice, and the second time cost more.

I built the first band by reasoning about parenthesis balance instead of reading one. It came back
with two stray `)` and my own counter insisted the file was balanced — I was arguing with the
kernel about a fact the kernel owns. Reading `learn/tests/benchbench-band.fk` took under a minute
and gave the body's actual shape.

The larger one: I had a finished cell, four-way green at 4095, a corpus row, and a receipt written
— and then the adversarial readers came back saying the framing was false and the novelty
over-claimed. The pull was to keep the green number and soften the prose, because 4095 four-way
*feels* like proof and the sentence was only a header. It is not only a header; a cell's header is
what the next reader believes. Two of the three critiques said `is_the_distinction_real: false`,
and they were right about the framing while being wrong that nothing survived. Reading their paths
myself — `smr-mastered`, `projC`, the GAG→GTG mutation — is what turned a tidy false sentence into
the narrower true one, and reading `bb-separates?` line 82 with their probe in hand is what turned
the whole thing from an import into a finding. The cell got better by losing the claim I liked most.
