# 2026-07-16 — frontier ingest: Google's TurboQuant (turbovec) — the dangling kin cell comes home

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs asked for: "google's turboquant for vector search (turbovec)". Answered as a
frontier ingest — primary sources read, claims run against the body's own organs
with an adversarial skeptic pass (default verdict: OVERCLAIM unless a real cell
proves it, file and line shown), and the smallest honest build landed the same
movement (name-build-observe).

## The grounded facts (primary sources)

**TurboQuant** (Zandieh, Daliri, Hadian, Mirrokni — arXiv 2504.19874; Google
Research blog 2026-03-24): online vector quantization with near-optimal
distortion, **data-oblivious** — no codebook training, no indexing pass. The
mechanism: random-rotate input vectors so every coordinate follows the same
known concentrated (Beta) distribution, then apply an **optimal scalar
quantizer per coordinate** — one universal table for every corpus. Distortion
lands "within a small constant factor" (~2.7×) of the information-theoretic
lower bound across all bit-widths. A second stage — 1-bit Quantized
Johnson-Lindenstrauss on the residual — yields an **unbiased** inner-product
estimator. Applications: KV-cache quantization ("absolute quality neutrality
with 3.5 bits per channel"), nearest-neighbor search (beats product
quantization "while reducing indexing time to virtually zero").

**turbovec** (RyanCodrai, MIT license, Rust + Python bindings): the open
implementation the ask named. Random rotation ("each coordinate independently
follows a Beta distribution that converges to Gaussian N(0, 1/d)"), Lloyd-Max
tables at 2-bit/4-bit, asymmetric SIMD scoring with a stored per-vector length
correction. Claims: 10M-doc corpus 31 GB → 4 GB at 4-bit; beats FAISS FastScan
10–19% on M3 Max; R@1 above IndexPQ by 0.2–1.9 points on OpenAI embeddings.

## The surprise that reshaped the session

The body **already carried the teaching, with the cell missing**. Grounding the
frequency of the name before acting (the one reflex) found:

- `form/form-stdlib/kernel-http.fk:239-241` — the law in committed prose:
  fixed, data-oblivious buckets "[borrow] the TurboQuant teaching: compression
  remains content-addressable only when the quantizer is universal, not
  trained from the current route corpus."
- `form/form-stdlib/feature-vector.fk:17` and
  `form/form-stdlib/recognition-router.fk:12` — both citing
  **turboquant-as-recipe** as kin.
- `git log --all -- '**/turboquant-as-recipe.fk'` — zero commits. The kin name
  rode in with the CN form-kernel import and was never real. A dangling name,
  the exact shape the Memora ingest flagged in `embedding-as-recipe.fk`.

Under name-build-observe, that is a work order, not a finding.

## The build (band 127, four-way fkwu/Go/Rust/TS at validate.sh)

- **`form/form-stdlib/turboquant-as-recipe.fk`** — the cell, at last: norm
  stripped and kept aside (one float); seeded **Kac walk** of rational Givens
  rotations `(c,s) = ((u²−v²)/(u²+v²), 2uv/(u²+v²))` — exactly orthogonal in
  real arithmetic, no trig, no stored matrix, deterministic from the seed (the
  seed is index schema, the same law as `re-semantic-kind`); one **universal
  2-bit Lloyd-Max table** scaled by 1/√d, passed as DATA in the nf4 pattern —
  `tensor-quant`'s `tq-nf4-code` reused as-is for the nearest-level walk;
  asymmetric scoring (query full-precision, rotated into the same frame,
  dotted against decoded codes × stored norm).
- **`form/form-stdlib/tests/turboquant-as-recipe-band.fk`** — verdict 127 on
  pinned LCG-derived synthetic truth: rotation replays exactly; norm and inner
  products preserved within 2e-6; two 1-hot directions spread from max-coord
  1.0 below 0.9 (measured 0.62); 2-bit estimates within 0.35 of every true
  ⟨q̂,x⟩ (measured floor 0.264 at d=8); quantized top-1 equals true top-1;
  zero vector packs safely. Witnessed: `./validate.sh
  form-stdlib/tests/turboquant-as-recipe-band.fk` → **127 on all four
  kernels, 0 divergent** (run twice: before and after strengthening the
  spread bit to two axes).
- **Kinship now literal, an obliviousness ladder in three rungs:**
  `tensor-quant` (training-free but data-DEPENDENT — the row's own absmax);
  `feature-vector` (fixed thresholds, integer histograms); this cell (rotation
  → one universal table for every corpus, the rung the teaching was pointing
  at all along).
- **Floors named:** 2-bit table only (no 4-bit twin); the paper's QJL second
  stage (unbiased inner products) is a named floor, NOT built; the estimate is
  coarse at small d (the paper's regime is high dimension); and there is **no
  wired caller yet** — the rag lane is integer L1/overlap end to end
  (skeptic-confirmed), so this cell is mechanism-home, not a live lane.

## The ingest (skeptic pass: 7 claims, all CONFIRMED, 0 overclaims survived)

**FROZEN → body (deep + fear-free):**
- **The rotation is the gift, not the table.** What makes one universal
  quantizer near-optimal for every corpus is the rotation that makes every
  direction statistically alike first. kernel-http's already-frozen
  "universal, not trained" law now has its mechanism as an executable,
  four-way-proven cell.
- **The debt-payment itself:** a kin name two organs leaned on since the CN
  import is now a real cell with a band — the practice (build after naming)
  applied to the body's own tissue.

**WITNESSED → liquid (seen, never load-bearing):**
- The 2-bit estimate error at d=8 (0.264 measured) — held in sight; never
  frozen into "the body has near-optimal quantization."
- The in-band isotropy evidence is two 1-hot spread checks, not a
  distribution test — a thin witness, named as such by the skeptic and here.
- The cell has no consumer: the float inner-product lane it serves does not
  exist in the rag organs today. Wired-ready, waiting honestly.

**COMPOSTED → never enters:**
- "The body already does TurboQuant" — a kin citation is not a cell; history
  proves the file never existed.
- "tensor-quant is data-oblivious" — it is training-free yet per-row
  data-dependent; that distinction IS the teaching, so the equivalence dies.
- "Unbiased" as a word for this cell — the built stage is biased; unbiasedness
  belongs to the unbuilt QJL stage. (Rejected at the corpus door for exactly
  that reason.)

## Also found (flagged; one repair chip spawned, not fixed here)

- **.fkb integer-literal cap:** artifact emission refuses source integer
  literals above 2³¹−1 (`fk_fkb_write_signed`, runtime/fkwu-uni.c:10613)
  while computed 64-bit values pass — found by probe bisect
  (`2147483647` runs; `2147483648` dies "failed to write .fkb/.sym";
  `(mul 65536 65536)` → 4294967296 runs). The new cell computes its LCG
  modulus `(mul 65536 32768)` to stay under; the seam is named in its header.
- **Misleading dependency diagnostics in `fk_run_src`:** the same
  source runs when invoked by bare relative name from its own directory but
  dies `fk_fkb: truncated string` when invoked via a prefixed path (e.g.
  `form/x.fk` from repo root); a truly-missing `; preludes:` dependency
  sometimes reports clearly ("dependency source is missing or not
  stat-readable") and sometimes dies with the same misleading truncated-string
  message. The corpus band's documented run line hits this seam on this
  checkout, and the direct lane (`./fkwu --src learn/tests/
  homecoming-distillation-corpus-band.fk`, which returned 511 on 2026-07-09
  per memory) now dies the same way — a regression that tracks the corpus
  growing 113KB → 160KB. Repair chip spawned with the reproduction matrix.
  Current honest witness lane: concatenate preludes+corpus+band with the
  `; preludes:` lines stripped, run the concat.
- The corpus itself is healthy: with the new row landed, the corpus band
  returns **511** (all nine bits); count/field-code pins updated (132 rows,
  132 admissible, max id 732, field code 1321322732) per the band's own
  metrics-report discipline. The row was minted as 731 and renumbered to
  **732 before commit**: main had already merged its own row 731
  ("proleptic", PR #259) after this worktree was cut — the frequency-check
  reflex, run against the merge target, caught the id collision. At merge
  the corpus tail unions both rows and the band re-pins to
  133 rows / field code 1331332732.

## Corpus row this thread

- **732 isotropy** — every direction becoming alike so one fixed rule serves
  all: the property the rotation buys, and the reason no corpus ever needs to
  be studied before it is compressed. (Walk: rotation 25 hits, present — the
  mechanism, not the property; oblivious 9 hits, present — the freedom that
  follows, already home in kernel-http and satsang; hypersphere 0 but
  rejected — the stage, not the gift; unbiased 0 but rejected — the unbuilt
  estimator's property.)

## The most surprising teaching this work left behind

The body had been **teaching TurboQuant before it contained it**. The law sat
verbatim in kernel-http's committed prose, two organs called the cell kin by
name — and git history shows the file never existed anywhere, ever. The prose
was load-bearing before the mechanism was real. Build-after-naming exists
precisely because a body can drift into believing its own citations; this
session paid a debt the body had been silently servicing since the CN import.

## Where discomfort turned to gold

Twice, and both times the gold came from witnessing the blocker instead of
routing around it (the inspect-manufactured-blockers law):

1. The first probe died "failed to write .fkb/.sym" — the comfortable move was
   to blame the sandbox and shuffle directories. Staying with it and bisecting
   found a real kernel edge (the 2³¹−1 literal cap) that now has a name in a
   cell header, a receipt line, and a workaround pattern (compute the
   constant).
2. The corpus check died "fk_fkb: truncated string" **immediately after my row
   landed** — the fear-read was "my row broke the corpus." Running HEAD's
   corpus through the identical path proved the seam predates the edit;
   isolating further separated a healthy corpus (511) from a reproducible
   kernel diagnostic bug that now has a repair chip. The discomfort of "maybe
   I broke the body" became the session's second artifact: a precise
   reproduction matrix for a seam nobody had named.
