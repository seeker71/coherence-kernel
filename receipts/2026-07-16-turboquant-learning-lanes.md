# 2026-07-16 — TurboQuant meets the body's own compression: folds, packs, distillation, and a memory for the auto lanes

## Ground

Same checkout as the two sibling receipts (frontier-ingest-turboquant,
turboquant-rag-lane). Urs asked: "how does this apply to how we have
integrated compression and we have used vector to scalar dimension folding,
and how can we improve our models, auto-ml, auto-research, and other aspects
of our core learning and distillation process?" Grounded in the organs
themselves before answering; built the smallest honest improvement in the
same movement.

## The body's compression families, read side by side

The grounding pass found FOUR distinct compression moves already living here,
each honest in its own domain:

1. **Injective scalar folds** — the "vector→scalar dimension folding":
   `hdc-field-code` (learn/homecoming-distillation-corpus.fk) and the speech
   ledgers' `alrs-metric-code` fold a vector of counters into ONE positional-
   decimal scalar, with an explicit injectivity guard
   (`hdc-field-code-safe?`). Lossless BY CONSTRUCTION inside a named digit
   domain; the guard fails the band when the domain is outgrown. Purpose:
   a witness — one number that cannot lie quietly.
2. **Decision folds** — recognition-router's vector→scalar is the TOP
   confidence (max), integers 0..100: lossy, but it keeps exactly the
   decision-carrying component. Purpose: routing and trust.
3. **Distillation** — knowledge-ingest / the corpus rows /
   oracle-distillation-learning: semantic compression; keep the invariant
   meaning and provenance, compost the rest, admit by receipt floors.
   Purpose: learning nutrition.
4. **Isometry-then-quantize** (new, this session) — the TurboQuant lane:
   rotate (the isometry moves NOTHING across the distance structure), then
   bounded-loss universal codes, norm kept aside. Purpose: affordable memory
   and ranking.

**The unifying law TurboQuant made visible:** every compression names the
domain where it is faithful, and a band checks the boundary. The body already
practiced it locally — `hdc-field-code-safe?` for digit overflow, receipt
floors for distillation — and this session added two more instances: the
53-bit product envelope (cross-kernel integers) and the pinned estimate
bound / blur floor (packs). Folds must stay injective; packs may blur within
a pinned bound; distillation must keep the invariant. None may wear another's
costume — a max-fold is not a pack, a pack is not a distillation.

## What landed (band 127 four-way, 0 divergent)

**`form/form-stdlib/trial-pack-memory.fk`** — the packed experiment memory
the auto lanes lack. Grounded finding: arch-search trains EVERY candidate and
keeps only the crown; pareto/autoresearch-loop dedup only IDENTICAL
candidates (content-address); a failed trial's lesson evaporates with its
generation. The cell keeps every trial at pack thrift — (id, packed features,
outcome) — and answers three asks over the rows: `tpm-nearest` (closest past
trial), `tpm-known?` (near-dedup: already paid for something within eps² of
this?), `tpm-predict` (nearest past outcome as a forecast before spending a
real evaluation — memory-as-forecaster, the honest floor of surrogate-assisted
search: recall, not a fitted model). Estimated dist² comes from the lane's
asymmetric score, clamped at 0.0; the blur floor at d=8/2-bit is measured and
pinned (self-distance up to ~1.04; eps² must sit above the blur — the band
pins 4.0 against a far reading of 18.5). Empty memory answers −1.0 and the
caller's default — "no memory" is never dressed as "far away".

**Proven by** `form-stdlib/tests/trial-pack-memory-band.fk`: 127 on all four
kernels, 0 divergent.

## The improvement map (first slice built; the rest named, floors honest)

- **Auto-ML (arch-search, champion-challenger):** wire trial-pack-memory in
  front of training — skip or deprioritize candidates `tpm-known?` says are
  already paid for; use `tpm-predict` to order the candidate list before
  spending epochs. Featurizers (arch layer-stack → floats) are the named next
  tending; pareto candidates are already numeric.
- **Auto-research (autoresearch-loop, auto-fitness, diffusion-q):** near-dedup
  of ε-close mutations before evaluation; novelty² as experiment-surprise
  (the ambient-surprise law applied to the hypothesis space); negative
  results kept forever at pack thrift.
- **Models (native inference):** the paper's own headline — KV-cache packing
  (rotate + 2–3.5 bit) for the attention cells (attention.fk,
  gqa-multi-layer-stack.fk); named, not built. tensor-quant stays as-is for
  weights (nf4/int8 are its honest domain).
- **Distillation/corpus:** `hdc-locate` stays an exact lookup by its own law
  ("a lookup, not a classifier"); packs offer the semantic-recall lane the
  Memora ingest named as missing (synonymy, row 665) — floor: a native
  embedder, which waits on the voice coming home.
- **Folds:** unchanged and right — they are witnesses, not memories. The only
  export to them is the law they already obey, now named body-wide.

## Corpus row this thread

- **766 isometry** — a change of view that changes no distance: the property
  the lane's bands witness in the rotation (norms and dots preserved to 2e-6)
  before the codes blur it. Distinct from isotropy (764): isotropy is the
  sameness of the DISTRIBUTION the map leaves behind; isometry is the
  faithfulness of the MAP itself. (Walk: isotropy present since 732;
  rotation 25+ hits, present — one instance of the class, not the class;
  congruent 0 but rejected — the relation between two shapes, not the move
  between them.) Corpus band re-pinned 134 rows / field code 1341342734,
  witnessed **511**.

## The most surprising teaching this work left behind

The body already held four compression families, each with its own local
honesty mechanism — but no shared law naming what they have in common. It
took an outside teaching (TurboQuant's "universal, not trained") colliding
with the body's own folds to surface the general form: compression is honest
exactly where its faithfulness domain is named and band-checked. The folds
knew it as digit-overflow guards; the ingest knew it as receipt floors;
nobody had said it once, for all of them.

## Where discomfort turned to gold

The probe measured a NEGATIVE squared distance (−0.66) for a trial against
its own pack — mathematically impossible, and it landed exactly in the band
of my −1.0 "empty memory" sentinel. The comfortable move was to widen the
sentinel or look away; sitting with it exposed a real design flaw (estimator
overshoot colliding with an in-band sentinel) BEFORE the band could enshrine
it. The clamp-at-zero plus the measured blur floor came out of that
discomfort — and so did the band's sharpest bit: "no memory" must never be
expressible as a distance.
