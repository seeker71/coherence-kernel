# 2026-07-17 — the real-data witness: packs proven on the body's own field

## Ground

Urs: "let's build, not just name, and verify end-to-end with real data, not toy
data, actual learning, actual improvements, shown with audio, video, object
classification, native reasoning, second brain librarian." Same checkout as the
three 2026-07-16 sibling receipts; fkwu freshness band 15. The second-brain
covenant (standing grant) grounds the read of the live field stores; everything
below is measured, nothing synthetic, and every person-name is anonymized
before it enters this public artifact (private-circle law).

## The honesty gate first: parity witnessed

The evaluation harnesses are Python carriers (hands); the Form cells are the
law. Before any number counted, `librarian_pack_e2e.py parity` emitted three
real 768-dim vectors into Form programs and ran the actual cell on fkwu:
**carrier pack == cell pack, code-for-code, on all three** (2.3s per vector in
the tree-walker). The carrier earned the right to scale; the cell stayed the
authority. (First parity attempt died on the receipted `fk_fkb` path-prefix
seam — the harness now invokes fkwu by bare relative name from form/.)

## Second-brain librarian — 597 real documents (the body's own receipts)

Embedder: `nomic-embed-text` via ollama (the rag lane's named bootstrap
carrier, already pulled on this machine). Corpus: every receipt in receipts/.

- **Compression: 30.7×** — 3,667,968 bytes of float64 vectors → 119,400 bytes
  of packs (norm-microunits + 2-bit codes).
- **Top-1 agreement 93.0%**, top-5 overlap 76.4%, over 100 real title-derived
  queries: the packed ranker returns the same best document as full precision
  93 times in 100.
- **12 hand-labeled natural-language questions** (written before evaluation
  ran): full precision hit@1 5/12, hit@5 9/12; packed hit@1 4/12, hit@5 7/12.
  The base retrieval is itself modest — 597 receipts contain many
  near-duplicate accounts of the same events — and packing costs 1@1 / 2@5
  against that base. Honest reading: packs preserve most of what full
  precision achieves; absolute quality is bounded by the embedder and the
  shelf's density, not by the packing.

## Audio — the live speaker book (real voiceprints, real people, anonymized)

Store: `~/.coherence-network/speakers/` — the mic fleet's real book, actively
growing under its launchd watcher while this ran. 2,181 human-labeled 256-dim
resemblyzer voiceprints across three speakers (speaker-1 n=331, speaker-2
n=57, speaker-3 n=1,793; majority baseline 82.2%). Leave-one-out nearest-sample
identification, 300 random queries:

- **Full-precision accuracy 99.0%; packed accuracy 99.7%** (both ≈99%; the
  gap is sampling noise, not a claim of improvement).
- **Compression 28.4×** (4,466,688 → 157,032 bytes for the labeled set).
- **Top-1 agreement only 24.0%** — the finding of the night, see below.

## Vision — the face store (real Apple Vision feature-prints)

Store: `~/.coherence-network/face-training/` — 3,330 labeled 768-dim
feature-prints (one profile, the household's own; 670 pooled). One profile
means identification cannot be scored; measured instead: nearest-sample
agreement (100 live queries vs a 1,000-print packed gallery) = **31.0%**,
best-score mean absolute error **0.178**, whole-store compression **30.7×**
(20,459,520 → 666,000 bytes).

## The finding: agreement broke, truth held (veridical, row 778)

The librarian agreed 93%; the speaker book agreed 24% — yet named the right
speaker 99.7% of the time. The difference is **gallery density**: 597 receipts
are mostly distinct, so one nearest document is stable under blur; 1,793
voiceprints of one person are near-duplicates, so WHICH same-person sample
wins top-1 is arbitrary under any perturbation — and the 2-bit blur reshuffles
it freely while the answer to the real question ("who is speaking?") stands.
Exact-neighbor agreement is the wrong fidelity metric on dense galleries;
task-level truth is the veridical one. Toy data could never have taught this —
density is a property real fields carry and synthetic pins do not.

## Floors, named honestly (the ask's remaining words)

- **Video / object classification:** 24,935 real labeled camera frames exist
  in the world store with **zero persisted feature vectors** — the world
  featurizer is the next work order (the face lane proves the
  print-then-pack path end-to-end; vision models llava/moondream are local).
  No video files exist anywhere; the live camera-pull stage is empty and
  waits on the present word, as covenanted.
- **Native reasoning:** not witnessed tonight. The wiring points are now
  named precisely: the Form `nearest-shape` recognition recipe, and the CN
  nearest-neighbor call sites (speaker_profiles.py:123-131,
  face_profiles.py:88-92, vision_train.py:55,76,
  web/lib/form-kernel/recognition-recipe.ts:67-71) — each an insertion point
  for the packed lane; plus form-cli-ask's index for packed grounding.
- **Dialog:** 16 turns — barely seeded; nothing honest to measure yet.
- The recon also re-confirmed the counterfeit `mac-speakers.json` (eight
  1-dimensional "centroids") sitting beside the real 256-dim book — the
  counterfeit/real distinction the memory already carries, now witnessed
  against both files.
- Main landed `.fkb artifact v4 — the value lane molts to 64 bits` (#265)
  while this session ran: the 2026-07-16 receipts' integer-literal-cap claims
  are OWED a re-witness after merge (belief-freshness law).

## What is now in the body

- `form/scripts/librarian_pack_e2e.py` — embed / parity / eval, reproducible
  (cache gitignored, rebuilt in ~32s).
- `form/scripts/recognition_pack_e2e.py` — audio / face witnesses, READ-ONLY
  on the live stores, anonymizing before printing.
- Corpus rows renumbered **775 isotropy, 776 thrift, 777 isometry** (main
  reached 752 the same night) and **778 veridical** landed; corpus band
  re-pinned 135 rows / field code 1351352756, witnessed **511**.

## The most surprising teaching this work left behind

The metric I trusted most — exact top-1 agreement, the one the four-way bands
pin — is the one real data broke first. On the dense speaker book the packed
ranker almost never agreed with full precision about WHICH sample was nearest,
and almost never differed about WHO was speaking. Fidelity lives at the level
of the question being asked, not the ranking beneath it; the same 2-bit blur
is fatal or free depending on what the field around it looks like.

## Where discomfort turned to gold

Reading "top1_agreement: 24%" landed as a small dread — three receipts of
four-way-proven work, and the first real-data number looked like failure. The
pull was to soften it (report only accuracy, bury agreement). Witnessing it
instead — asking WHY agreement broke while accuracy held — produced the
session's deepest finding (density decides what blur costs), the fresh word
that names it (veridical), and a sharper law for every future witness: report
the metric that embarrassed you, then explain it; the embarrassment is
usually the teaching.
