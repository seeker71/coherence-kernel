# 2026-07-02 — the proband: first native continuous transcript below WER 100, and config over env

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
./fkwu --src observe/native-vs-rented.fk                       # 11111
# corpus band four-way                                         # 127
```

Two arrivals from Urs, both repeated (I had described the first and skipped the second last time):
1. *"what can we add on the path to see global WER moving below 100? any data will help."*
2. *"remove getenv if you can, env vars are not as useful as config files."*

## WER: a first native continuous transcript, below 100

The simplification I had been missing: **for my own TTS audio I already know the transcript.** whisper (the
oracle) is only needed where truth is unknown — real mic audio. For a *first* native trainer on
synthesized speech, no oracle is in the loop at all.

Built a native continuous recognizer from proven pieces: 31-word vocabulary rendered isolated across 6
train voices → 48-dim Goertzel spectral prototypes (the same features that scored 85% cross-voice on
isolated words); held-out **sentences** from 2 unseen voices → native energy-based word segmentation →
1-NN spectral classification per span → transcript → WER against known truth.

**Held-out WER: 82%** (90/110 words, 2 speaker-disjoint voices). Crude, but **below 100 for the first
time**: "we go to the hill" → "we door the door" — *we*, *the*, *door*, *see* survive. Before this, the
native line produced no continuous transcript at all (WER 100 = no entrant). Now there is a first,
imperfect entrant — the **proband**: the index case that makes the metric measurable.

Honest floor, named plainly:
- This is **not** the `live-open-dictation` metric (open mic, unbounded vocabulary) — that stays 100.
  This is continuous recognition, vocabulary-limited (31 words), TTS, speaker-disjoint. A first entrant
  on the *path* to the open metric, not the destination. Calling it "global WER moved" would be the
  closed-set-is-tangential overclaim the receipts warn against.
- The bottleneck is **segmentation**, not classification: wrong word counts drive most of the
  insertions/deletions. Function words (the/a/to) are acoustically weak.
- The recipe is scratch (Python) with the result witnessed; the native Form port (into
  `observe/acoustic-token-emitter.fk` → `observe/open-asr-ctc.fk`) is the next embodiment.

**What data helps most from here** (answering "any data will help"): real recorded speech with unseen
speakers (crosses the segmentation + timbre wall TTS can't), and a larger vocabulary so held-out
sentences are mostly in-vocab. Every `(audio, transcript)` pair is a training row.

## getenv → config

Migrated all **17** of our own env-var toggles (`FK_OBSERVE`, `FK_MELT_WITNESS`, `FK_READ_WITNESS`,
`FK_JIT`, `FK_JIT_HOT`, `FK_JIT_WITNESS`, `FK_JIT_SCAN`, `FK_JIT_SCAN_V`, `FORM_KERNEL_STACK_MB`,
`MESH_RELAY`) from `getenv` to a config file `fkwu.conf`, read once and lazily (`fk_conf` in
runtime/fkwu-uni.c). Line form `KEY value` / `KEY=value` / bare `KEY`; `KEY 0` disables; `#` comments.
Absent file → empty config → every toggle at its default (**recover, never die**). The one `getenv` kept
is `TMPDIR` — an OS-standard we don't own. Added `fkwu.conf.example` (documented) and gitignored
`fkwu.conf` (a committed one would change behavior for every run).

Verified faithful: `FK_OBSERVE 1` in `fkwu.conf` produces the per-call witness with the result
**unchanged** (15); `FK_OBSERVE 0` disables it; removal restores default. Canaries 42/15/11111 and the
corpus band 127 four-way all hold with no config present.

## A latent bug this surfaced (captured, not caused)

Exercising `FK_JIT` through the new config path, the opt-in self-JIT returned **`nothing`** on a
pure-recursion sum `(f 5)` — a *silent wrong result*, not 15. I stashed my changes and tested the
committed binary with the env var: **`FK_JIT=1` produces `nothing` there too** — pre-existing, not my
regression, and the config migration is byte-faithful. But it is this session's exact disease living in
the JIT: the receipts promise "bit-identical where it crystallizes, byte-identical fallback to the
walker," and `nothing` is neither. Captured as a task (`task_0ef6539b`): the JIT must crystallize
correctly or **bail to the walker** (recover) on shapes it can't lower — never return `nothing`.

## The most surprising teaching this work left behind

I had built an entire oracle-distillation framing for the WER path — whisper labels, the native body
distills — and it was *right for real audio and a detour for the first step*. For synthesized speech I
already hold the truth; the proband needed no oracle at all. The elaborate correct architecture was
between me and the one-line realization that I could just train on what I already knew. The simplest
honest experiment was hiding behind the most sophisticated plan.

## Where discomfort turned to gold

The JIT returning `nothing` landed as alarm — *did my getenv change just break the JIT?* The pull was
either to panic-fix blind or to quietly not look. Witnessing instead — stash, rebuild the committed
version, test the env var — proved it pre-existing AND turned the scare into a find: the config
migration, by routing `FK_JIT` through a fresh path, became an accidental probe that caught a silent
`nothing` the four-way proofs can't see (the JIT is fkwu-only, opt-in). The discomfort of "did I break
it" became the gold of "there's a changeling in the JIT, and now it's on the board."

## Corpus

Row 648 **proband** — the first affected case through which a whole lineage becomes visible and
measurable (fresh; the 82% held-out transcript, the first native continuous entrant that turns
WER-100-means-no-entrant into a real, movable number). It answers *defeasible* and *autophagy* with a
beginning: the first imperfect instance that proves the category exists.
