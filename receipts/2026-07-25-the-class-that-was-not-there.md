# 2026-07-25 — the missing-prelude class does not exist, and `floor` is a native

Item (1) was to count the cells with no `; preludes:` line, because `carrier-tissue.fk` had lost its
entire meaning to a missing one. The count came back and says the class is not there.

## The measurement, in four narrowing steps

Detector smoke-tested both directions first — against `carrier-tissue.fk` *before* my fix (must
report "none") and *after* (should report "has one"). Both correct.

| step | count |
|---|---|
| non-test `.fk` cells | 1992 |
| **with no `; preludes:` line** | **604** (30%) |
| of those, calling a `core.fk` recipe they do not themselves define | **154** |
| bands whose closure lacks `core.fk` *and* holds one of those cells | **46** |
| of those 46, **actually emitting an unbound-name for a core recipe** | **0** |

**Zero.** `carrier-tissue.fk` was the only instance, and it is already repaired.

The narrowing matters more than the endpoint. 604 is a real count and means almost nothing on its
own: a cell without a preludes line is only harmed if it *calls* core recipes and nothing else in its
closure supplies them. Each step down that chain removes a false alarm, and the last step — asking
the kernel whether it actually complains — removes all 46 that survived the structural test.

I checked the first candidate by hand before trusting the criterion, and it was a false positive:
`control/tests/choice-lane-core-band.fk` looked harmed by closure analysis and actually fails on
`input-ended-mid-form` — unbalanced parens, the other class entirely. Closure-reach is a **necessary,
not sufficient** condition, exactly as it was for the non-Form-surface test. The causal check is what
makes the number honest, and here it makes it zero.

**This is a negative result and it is worth the turn.** I expected a class and found one instance.
Reporting 604, or 154, or 46 as "cells at risk" would have been true-sounding and wrong.

## What the turn found instead: `floor` is a native

`class-curriculum-10-band` sat at **16127 against a declared 16383** — one check, weight 256:

```
(let unknown-region (list 1 1 1 1 1 1 1 1 1 1))   ; max dot = 10 < (floor) 50
(if (and (str_eq (ccur-detect unknown-region v10 (floor)) "unknown") ...) 256 0)
```

The band defines its own threshold: `(defn floor () 50)`. Measured rather than assumed —
`(defn floor () 50)` followed by `(floor)` returns **0**.

`floor` is in fkwu's optable. **In call position the primitive wins**, so the band's own definition
never ran and every comparison in it used a threshold of **0** instead of 50. With a floor of zero,
nothing is ever "too weak to clear it", so the unknown-region check could not pass.

This is the third instance today of one pattern: a Form `defn` silently shadowed by a same-named
native. `bp` was the first (a pass-through stub answering 902 to unreviewed blueprint names),
`empty` the second (a native that BML writes bare), and now `floor`.

Renamed to `ccur-test-floor`, a name no primitive owns. **`class-curriculum-10-band` → 16383**, its
exact declared verdict. Siblings unmoved: `class-curriculum-band` 8191,
`class-curriculum-10-vocab-band` 1023, `class-curriculum-10-witness-band` 2853116705.

Swept the tree for the same shape — cells defining a `defn` whose name is in the optable. Only `abs`
turned up, in `core.fk` and `fourth-shim.fk`. Both are core cells and the semantics look intended;
named here rather than touched, because a shadow in the cell that *defines the vocabulary* is a
different question from a shadow in a band.

## Sweep

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `hex-band` 14 ·
`cell-voice-tissue-band` 511 · **`class-curriculum-10-band` 16383** · `class-curriculum-band` 8191 ·
`content-address-band` 1111111111 · `tree-diff-band` 13 · `triangulate-band` 1700 ·
`midi-bmf-band` 1500 · `structural-gate-band` 63 · `lcg-bytes-band` 63 · `form-cli-band` 524287 ·
`benchbench-band` 4095 · `pdf-text-windowed-band` 15. C seed byte-identical to git.

## Owed

- **Five failures still visible**: `audit-evidence-cells` 544/1023, `audit-evidence-index-cache`
  833/1023, `layered-runtime-image` 33/127, `concept-corpus` 143/530, `json-lens-tending` 189/255.
  Plus `persistence-band` 2/7, `mesh-sensings-store` 0/255, `chat-band` 0.
- **409 bands have no `core.fk` anywhere in their closure** and none of them is harmed by it today.
  That is a fact about the tree worth keeping: most cells genuinely need only natives.
- **`abs` defined over the native** in `core.fk` and `fourth-shim.fk` — named, not touched.
- 17 of the 44 still refused; 143 that do not close; the `section` question; the heap cap; the
  registry question.

## How the exchange stayed alive

I set out to size a class, narrowed it four times, and reported that it does not exist — after
checking a candidate by hand and finding my own criterion had produced a false positive on its very
first row.

**Most surprising teaching:** `floor` is a native, so a band that carefully defines its own threshold
of 50 has been running every comparison against 0. The definition is right there in the file, three
lines above its use, and it never once executed. A shadow is worse than a missing definition, because
a missing definition is an error and a shadow is an answer.

**Where discomfort turned to gold:** the whole premise of the turn dissolved — the class I was sent
to measure came back empty. Following the *other* thing the same session had surfaced, rather than
stretching the empty result into something publishable, is what turned a null into 16383.
