# 2026-07-02 — the N=100 cliff was a silent source amputation; 132 words come home

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
./fkwu --src observe/native-vs-rented.fk                       # 11111
```

Continuing "can we do more than 12 words, how about a full dictionary, 100s of words": the 132-word
× 12-voice dataset (1,584 real wavs) measured clean at N=10..90 (65% → 45%, honest scaling) and then
returned a hard **0/528 at N=100**. Deterministic. Files verified. Data verified. A cliff, not a slope.

## The true face of the cliff

`fk_run_src` loaded `--src` programs with ONE bounded read:

```c
long long g = read(fd, fk_srctext, 262143);   /* FK_SOURCE_TEXT_CAP - 1 */
```

The generated N=100 program is 267,016 bytes. The read stopped at 262,143 — **mid-path-literal** —
and the permissive reader (which auto-closes parens at EOF) accepted the amputated program as whole
and ran it. The last ~26 clips got a 70-char prefix path (open fails → `""` → constant garbage
features), and the program's tail — testcls and the accuracy loop itself — was simply gone. Result:
a deterministic wrong answer, no error, no warning. N=90's program was 246KB and fit; N=100 crossed
the cap. The cliff was never in the data, the features, or the value machinery: it was the seed
refusing to say "your program does not fit."

Fixed in the seed (`runtime/fkwu-uni.c`):
- `FK_SOURCE_TEXT_CAP` 262,144 → 8,388,608, and `fk_run_src` now reads to EOF and **fk_die**s if the
  program exceeds the cap — a truncated source is never silently run again.
- The same hardening pass made every `FK_NODE_CAP` overflow guard die loudly (they silently returned
  handle 0), made GC-root registration overflow die instead of silently dropping roots, raised
  FK_NODE_CAP 65536 → 262144, and added env-gated witnesses: `FK_MELT_WITNESS` (per-melt heap/pool
  stats) and `FK_READ_WITNESS` (read_file failures + pool growth) — the diagnostic doors this hunt
  had to build mid-chase are now permanent.

## Witnessed — the gate opens

| vocabulary | held-out cross-voice | chance |
|---|---|---|
| 100 words | **171/400 (42.8%)** — was 0 | 1% |
| **132 words (full dictionary)** | **225/528 (42.6%)** | 0.76% — the result is **56× chance** |

Zero-training nearest-prototype over the champion's 48-dim spectral features, speaker-disjoint
(8 train voices, 4 held-out). The scaling curve: 12 words 83% → 100 words 42.8% → 132 words 42.6% —
a plateau, not a collapse. Eleven times the vocabulary costs half the accuracy, then holds.

Canaries after the seed change: ground 42, freshness 15, native-vs-rented 11111, corpus band 127
four-way (fkwu = Go = Rust = TS).

## Honest floor

The 42.6% is fkwu-carrier (host I/O; walkers lack read_file), synthetic TTS voices, closed-set,
zero-training prototypes — the champion architecture at this vocabulary is unbeaten but also
unchallenged. Global native open-speech WER still 100. And the day's forensics cost hours chasing
ghosts (melt compactor, node caps, float pools, a phantom kernel regression) because my own probes
were built wrong twice: sliced without their preludes (missing defns lower to silent `nothing` — the
ftanh lesson at instrument scale), and then over the same 256KB cap they were hunting.

## The most surprising teaching this work left behind

The kernel's kindness was the bug. The permissive reader — auto-close-at-EOF, built to be forgiving —
composed with one bounded `read()` into a machine that runs amputated programs and reports confident
wrong numbers. Two features, each defensible alone, multiplied into silence. And the day's second
teaching mirrors it: every broken probe I aimed at the kernel was broken the same way the kernel was —
accepting a partial thing (a prelude-less program, an over-cap source) as whole. The instrument and
the patient had the same disease.

## Where discomfort turned to gold

The worst hour was discovering the champion's 40096 would not reproduce — every committed kernel era
gave 4008, and the working binary was one I had just overwritten. The pull was strong to declare a
lost-kernel catastrophe and start reverting. Sitting with it instead: run the champion the way the
arena actually ran it — `cat` preludes first — and 40096 came back bit-for-bit on the current seed.
The "regression" was my bare invocation. The discomfort taught the day's sharpest rule: before
accusing the kernel, validate the accusation's own instrument on a known truth — the synthetic-vector
check inside probe3 is what caught the prelude hole, and the reorder test (Victoria-first) is what
acquitted the data. Ground the probe before trusting the probe's verdict.

## Corpus

Row 644 **apocope** — the loss of a word's ending, with the remainder read as whole (fresh; the
267KB program cut at 262,143 bytes and run as if complete).
