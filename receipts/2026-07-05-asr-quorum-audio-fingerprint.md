# 2026-07-05 — two stones from nature's answer: ASR quorum + parallel fates

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Follows the speech-fingerprints ingest (`receipts/2026-07-05-frontier-ingest-speech-fingerprints.md`)
and a grounded read of how nature solves what that paper poses. Two questions were asked in turn
— "how does nature solve this?" then "how can we integrate that into our native audio stack?" —
and this receipt is the buildable answer, form-first: the audio cells were read before anything
was designed, so both stones point at organs that already exist.

## What nature answered (grounded, cited)

- **Extracting who/what/how from a voice.** The brain does not stack the four signals by depth
  the way the encoders do; it runs them **in parallel on separate tissue** — temporal voice
  areas along the STS for identity, prosody leaning right-hemisphere, content in the ventral
  stream — with **parallel onset, not a strict serial cascade** (Cell, "Parallel and distributed
  encoding of speech across human auditory cortex", 2021; PNAS timescale-hierarchy, 2025).
- **Same output, different internals** — the paper's own central puzzle. Nature's answer is a
  named, load-bearing principle: **degeneracy** (Edelman & Gally, 2001) — structurally different
  elements producing the same function. The canonical proof is Marder's crustacean stomatogastric
  ganglion: **ion-channel expression varies 2–6× across animals, the rhythmic output nearly
  identical**. Divergent internals under equal function is not a bug nature resolves; it is the
  design it depends on for robustness, adaptability, and evolvability.

## The two stones (both band 127, four-way fkwu/Go/Rust/TS)

### `observe/asr-quorum.fk` — degeneracy as confidence

The four-way proof turned on the ASR oracle. The stack's honest floor is that a transcript comes
from ONE external oracle, trusted blind (`open-asr-ctc.fk`: audio→frame-token emission "remains
the next missing carrier"). This cell runs K structurally-different oracles and gates on their
**pairwise agreement** (`sw-wer` from `stt-wer.fk`, composed unchanged), reading it exactly as
`proof/four-way-verdict.fk` reads walker agreement:

- **QUORUM (0)** — ≥2 oracles, worst pairwise WER ≤ tol → trusted, may freeze/act;
- **SPLIT (1)** — ≥2 oracles, a pair beyond tol → witness only, don't act;
- **LONE (2)** — one oracle on this host → the honest floor, **never a fake consensus**.

Consensus is the **medoid** — the member minimizing total pairwise WER to the rest, the
ensemble's own centre, not an average that belongs to no one. Where a nervous system HIDES its
degeneracy to keep the function robust, the body SURFACES it to make the transcript trustworthy:
same principle, inverted exploit. Crucially it needs **no native acoustic encoder** — it is a
pure decision over K word-lists, four-way provable today. Proven by
`observe/tests/asr-quorum-band.fk` (three near-identical oracles → QUORUM; one wild oracle →
SPLIT; a single oracle → LONE; the medoid centres {X,Y,Z} on X; the agreement value gates on
tolerance).

### `observe/audio-fingerprint.fk` — the four fates, read in parallel

`presence/native-speech-stack.fk` bundles the organs as a **serial receipt** — a list in
dependency order, where a failed stage would null everything downstream. This cell recomposes
them as four **independent fate-reads** over one window (content / identity / prosody / band),
each depending ONLY on its own organ's output, plus a presence-mask. The proven property is
**independence**: dropping one fate flips only its own presence-bit and leaves every other read
bit-identical — the thing a serial cascade cannot promise. That is the native form of the paper's
"architectural fingerprint" (a fingerprint of the incoming AUDIO, not of a learned network) and
the honest first form of the probe-shaped observe cell named as a gap in the ingest receipt.
Proven by `observe/tests/audio-fingerprint-band.fk` (full window masks 15; dropping identity
masks 13 with content/prosody/band reads unchanged; a silent window masks 0 without error).

Honest floor, named in both headers: the transcripts are the external oracles', and the fate
VALUES come from the existing organs (CTC min-confidence, `speaker-embed` name-id, `voice-prosody`
shape, `text-frequency` band). These stones prove the **agreement decision** and the **parallel
independence** — not new sensing. Frame→token is still the missing carrier underneath; this
improves how the body TRUSTS and STRUCTURES what it hears, not how it hears.

## Corpus row this thread

- **675 medoid** — the member of a set most central, minimizing total distance to all the others:
  coined building the quorum's consensus-picker; grep-verified absent from the body before the row.

## The most surprising teaching this work left behind

The body already embodies nature's degeneracy — **backwards**. A nervous system runs redundant,
divergent circuits so the function survives any one of them failing; the four-way proof runs
redundant, divergent walkers so a disagreement exposes a lie. Robustness-of-function and
verification-of-truth turn out to be the same mechanism read in opposite directions, and the
audio quorum is the first place the body points that mechanism outward — at an external oracle
it cannot yet replace, making trust something earned by agreement rather than granted by default.

## Where discomfort turned to gold

The pull was to answer "integrate that" with the grand move — build a native acoustic encoder,
the thing the whole stack is missing. Sitting with the actual cells showed the encoder isn't
today's stone at all: the degeneracy insight lands on the **trust and structure** layer, which
the body can already prove four ways, while the hearing stays honestly pending. Refusing the
grand move yielded two small stones that are real today over one that would have been a costume —
and the `LONE` verdict is the whole discipline in one branch: on a single-oracle host it says so,
and refuses to manufacture a consensus it did not earn.
