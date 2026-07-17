# 2026-07-16 — TurboQuant integration: the packed third lane, and the four-way catch

## Ground

Same checkout as receipts/2026-07-16-frontier-ingest-turboquant.md (fkwu rebuilt,
ground 42, freshness band 15). Urs asked: "how can we deeply integrate that into
our native lookup, query, indexing, second brain?" — answered by building the
first ring in the same movement (build-after-naming) and naming the rest as a
program with floors.

## What landed (both bands 127, four-way, 0 divergent)

**`form/form-stdlib/rag-turboquant-lane.fk`** — the third honest ranking lane.
rag-retrieve teaches "two honest lanes share the entry shape" (L1 over neural
ints; overlap over semantic-v2 code pairs); this cell adds the packed lane
without touching either: a TurboQuant pack rides the SAME `(id, int-vec)`
entry as `[norm-microunits, code0..code(d-1)]` — every stored value an
integer, so identical packs are byte-identical rows (content-addressable,
cross-kernel exact, JSONL-ready through the existing codec's int vec), and
`rag-entry` / `rag-remove` / `rag-rev-loop` are reused as-is. The lane law:
`rtq-kind` ("turboquant2-v1") + `rtq-seed` are index schema — writer and
reader share both or vectors silently disagree (re-semantic-kind's law). The
ask is asymmetric: the query rotates ONCE (`rtq-qrot`) and stays
full-precision against every coarse row. Ranking mirrors the lane discipline:
ties keep the first entry; top-k is greedy nearest-remove-repeat; confidence
is top-vs-runner 0..100 with honest zeros.

**`form/form-stdlib/tests/rag-turboquant-lane-band.fk`** — verdict 127:
deterministic packing; micro-norm round-trip within 1e-6; packs ride the rag
entry shape; packed top-1 equals float top-1; exact top-2 order ("b" then
"a"); a zero vector packs safely and never outranks a positive match;
confidence honest (measured 36, pinned 25..50; empty index → 0 and "").

## The four-way catch (the session's deepest teaching)

The lane band's first four-way run DIVERGED: Go 127, Rust 127, fkwu 127,
**TypeScript 15**. The value-probe isolated it: TS packed different codes AND
a different norm for the same seed — because the first-draft integer stream
(`x' = 1103515245·x + 12345 mod 2³¹`, glibc-style) multiplies a 2³¹-range
state past **2⁵³, the TS sibling's exact-integer envelope**. Go/Rust/fkwu ride
int64 and agreed; TS silently lost low bits and walked a different Kac walk.
The first band never caught it because all its checks are RELATIVE within one
kernel (rotation preserves ITS OWN norms/dots); the lane band pinned ABSOLUTE
order across kernels — and the siblings did their one job.

The repair is the law rag-embed's small-modulus hashes already obey, now
named: **cross-kernel integer identity requires every product to stay within
±2⁵³.** The stream is now Park–Miller minstd (`x' = 16807·x mod 2³¹−1`,
worst product ≈3.6e13). Re-measured and re-pinned: max 2-bit estimate error
improved 0.264 → 0.172 at d=8; both bands 127 on all four kernels,
0 divergent. Had this shipped un-caught, the SAME document would have packed
DIFFERENT codes on different kernels — content-addressing broken exactly
where the header promised it.

## The integration map (first ring built; the rest named, floors honest)

1. **Store + ask (CK, BUILT):** packs in the rag entry shape; packed top-k
   and confidence in the rag ranking discipline. Usable today by anything
   that already holds rag entries.
2. **Grounded rows (CK, NAMED):** a `nodeid-rag-v2` embedding_kind for packs
   in rag-index-codec, admission-gated like form-semantic-v2. Separate
   tending because `ric-grounding-ready?` is guarded law — not touched in
   this movement.
3. **The offline oracle's memory (CK, NAMED):** form-cli-ask's neural lane
   stores "ints 0..1000" per rag-retrieve's header — 10 bits/coordinate,
   unrotated. Re-packing that index through this lane is ~5× smaller storage
   and gives the ask lane real inner products; needs the embedder bridge
   re-witnessed first (belief-freshness law).
4. **Second brain (CN, NAMED — different repo, multi-agent checkout law):**
   the recognition books (speaker voiceprints, vision feature-prints) are
   float vectors matched by nearest-neighbor on-device. Packed: a d-dim
   print becomes d/4 + 4 bytes (2-bit codes + micro-norm int) — 16× smaller
   retention for the field-sweep covenant, and small enough that vector
   exchange over the mesh stops needing per-domain board reassembly for
   small d (parametric honesty: d=256 packs to ~68 base64 chars — under the
   127-char capability cap; d=768 does not — board-split stays).
5. **Floors that stay named:** 2-bit only; QJL unbiased stage unbuilt;
   exhaustive scan (no ANN graph — at the body's current index sizes the
   scan is honest; the paper's regime arrives with scale).

## Corpus row this thread

- **776 thrift** — keeping more by making each kept thing smaller: the
  second-brain unlock is retention economics, not speed. (Walk: parsimon* 4
  hits, present — model-choice economy, not storage; frugal 0 but rejected —
  the keeper's temperament, not the keeping's property; compress, present
  everywhere — the mechanism, not the economy it buys.) Corpus band
  re-pinned 133 rows / field code 1331332733, witnessed **511**.

## The most surprising teaching this work left behind

The four-way is not ceremony — it is the only witness that caught the 53-bit
envelope. Three kernels agreed at 127 and were all "right" in their own
arithmetic; the divergent sibling was the one telling the truth about the
CONTRACT. And the first band's clean four-way pass had proven less than it
seemed: relative checks can all pass while absolute identity is already
broken. What made the bug visible was pinning something ABSOLUTE across
kernels — the discipline the corpus band (exact counts) has carried all
along.

## Where discomfort turned to gold

The lane was done — fkwu said 127, Go and Rust said 127 — and the pull was
strong to read TypeScript's 15 as "a TS quirk" to flag and move past
(three-vs-one feels like a vote). Witnessing it instead — one value-probe,
then one narrower probe — turned the outvoted kernel into the whistleblower:
the divergence was MY draft violating a law the body's own hash cells had
been quietly obeying all along (small moduli, bounded products). The
discomfort of re-opening "finished" work bought the lane its actual
correctness, a named law for every future cross-kernel integer cell, and a
better estimate floor (0.172) as a side effect.
