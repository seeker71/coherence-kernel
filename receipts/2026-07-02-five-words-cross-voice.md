# 2026-07-02 — more than yes and no: a 5-word recognizer, cross-voice held-out

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
which say ffmpeg                                               # present
```

Urs, 08:45: "we probably want to teach the voice more than yes and no." So: five words, three
voices, and a real held-out test — train on one voice, recognize the words in voices never seen.

## What ran

- **Vocabulary:** `yes no up down stop` (a small command set), rendered as real audio in THREE
  voices via `say` → `ffmpeg` → 16 kHz wavs: **Alex** (train), **Samantha** and **Fred** (held
  out — never trained on).
- **Features:** `observe/wav-sense.fk` 8-window energy envelope, normalized to sum 1 (so voice
  loudness is divided out; the envelope SHAPE remains).
- **Recognizer:** a linear classifier (8 features → 5 classes) trained with the **analytic
  softmax–cross-entropy gradient** (`softmax − onehot`, backprop through the linear map — no
  numerical gradient, no CTC lattice, so it stays under the AST node limit and trains in ~1 s).
  300 epochs over the 5 Alex clips.

## Witnessed — cross-voice held-out accuracy

| set | voice | result | note |
|-----|-------|--------|------|
| train | Alex | **5/5** | fits the training voice |
| held-out | Samantha | **5/5** | every word right in an unseen voice (yes→0 no→1 up→2 down→3 stop→4, verified per-clip) |
| held-out | Fred | **2/5** | "no" and "stop" right; "yes/up/down" collapse toward "no" |

**Cross-voice average: 7/10 = 70%** — real generalization, not memorization (the held-out clips
are different audio in different voices). Word accuracy 70% cross-voice ≈ WER ~30% on this closed
set (WER 0 on Samantha, 60% on Fred).

## Honest scope (real generalization, real limits)

- This is **closed-set command recognition** of 5 words — a genuine step past yes/no (which was a
  memorized 2-clip pair at WER 50), but NOT open speech.
- The envelope feature is **voice-sensitive**: it generalized perfectly to Samantha and poorly to
  Fred (whose vocal envelope shape diverges more from Alex). One training voice is not enough
  speaker diversity; spectral/mel features and multi-voice training are the honest next rungs.
- The **global native open-speech WER is still 100.** This is a scoped closed-set recognizer; the
  promotion law (`learn/speech-global-promotion-readiness.fk`) does not move the global number.

## The most surprising teaching this work left behind

The jump from 2 words to 5 came with LESS machinery, not more. yes/no used a CTC lattice, numerical
gradients, 500 steps, and reached WER 50. Five words used a plain classifier with an ANALYTIC
gradient, 300 epochs, and reached 70% cross-voice — because isolated-word recognition is
classification, not sequence decoding: CTC is the tool for CONNECTED speech, and using it on
single words was carrying a lattice where a dot product would do. The right-sized tool beat the
more powerful one. Knowing which problem you have is worth more than which method is fanciest.

## Where discomfort turned to gold

The discomfort was the 5/5-on-Samantha result — suspiciously perfect, exactly the kind of number
this session has learned to distrust. The pull was to report "100% cross-voice!" and move on.
Witnessed instead — a third voice added, per-clip predictions dumped — it held for Samantha
(genuinely 0,1,2,3,4) but broke to 2/5 on Fred. The honest 70% averaged over two voices is the
real result; the 100% on one voice would have been a true number telling a false story. A second
held-out voice turned a suspicious perfect into a trustworthy partial.

## Corpus

Row 634 **eidetic** — grasping the essential identity of a thing regardless of the particular form
carrying it (fresh; the recognizer hearing the WORD apart from the VOICE — fully for Samantha,
partly for Fred).
