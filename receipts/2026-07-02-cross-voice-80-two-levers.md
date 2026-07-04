# 2026-07-02 — 5 words at 80% cross-voice: two levers, data and acuity

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 08:55: "yes please" — close the Fred gap from the 70% cross-voice 5-word recognizer.

## What ran, and the honest progression

Three configs measured on the SAME held-out voices (Samantha, Fred), 5 words, real audio:

| config | training | feature | Samantha | Fred | cross-voice |
|--------|----------|---------|----------|------|-------------|
| 1 (prior) | Alex only | 8-window, clamped 0-9 | 5/5 | 2/5 | 70% (uneven) |
| 2 | 5 voices (Alex/Daniel/Karen/Moira/Tessa) | 8-window, clamped | 4/5 | 3/5 | 70% (uniform) |
| 3 | 5 voices | **16-window, RAW energy** (no clamp) | 4/5 | 4/5 | **80%** |

Two levers, each doing a distinct thing:
- **More training voices** (config 1→2): did NOT raise the average (70%→70%), but made it UNIFORM
  — traded a lucky 5/5-on-Samantha for a robust 4/5+3/5. The Alex-only peak was partly Samantha
  happening to resemble Alex; multi-voice removed the luck, which is what generalizes to NEW voices.
- **Finer, unclamped features** (config 2→3): RAISED the ceiling, 70%→80%. The 8-window feature
  was clamped to integers 0-9 and quantized (`/800`) — lossy. Sixteen raw `wav-abs-sum` windows,
  sum-normalized, carry more of the word's shape, and Fred went 3/5→4/5.

**80% cross-voice held-out on 5 words ≈ WER 20% on this closed set.** Real generalization across
unseen voices, not memorization.

## Honest floor (named)

- Still **closed-set command recognition** of 5 words — not open speech.
- Still **raw energy** features (temporal envelope), not spectral/mel. Energy carries loudness
  shape; it does NOT carry the formant/spectral structure that distinguishes phonemes robustly
  across voices. `model/mel-frame.fk` exists (log-mel over an FFT) and is the identified NEXT
  lever — that is what would push cross-voice past 80% and, eventually, toward open vocabulary.
- The **global native open-speech WER is still 100.** This is scoped; the promotion law is unmoved.

## The most surprising teaching this work left behind

The "feature is the bottleneck" hypothesis was HALF right, and finding which half was the work.
More data alone plateaued (confirming data wasn't the limit), but the fix wasn't a whole new
feature TYPE — just finer resolution of the same energy feature (raw vs clamped) bought 10 points.
The bottleneck had two locks: the training was one-voice-lucky (data lock) and the feature was
quantized-lossy (resolution lock). Neither alone was "the" bottleneck; the plateau lifted only
when BOTH were addressed. A single-cause story would have been wrong; two small honest levers,
each measured separately, told the true one.

## Where discomfort turned to gold

The discomfort was the config-2 result: 70%→70%, more voices "didn't help," the exact moment to
declare "feature is the bottleneck, need mel" and stop. That tidy single-cause conclusion was
tempting and would have been wrong. Witnessed instead — one more cheap lever tried (finer
features) — it jumped to 80%, revealing the plateau was two locks, not one. Sitting with a
"failed" flat result long enough to try one more thing turned a premature conclusion into the
real two-lever finding. The flat number was not the end of the story; it was the middle of it.

## Corpus

Row 635 **acuity** — sharpness of discrimination; the resolving power a finer feature gives
(fresh; the 10 points that raw 16-window energy bought over the clamped 8-window).
