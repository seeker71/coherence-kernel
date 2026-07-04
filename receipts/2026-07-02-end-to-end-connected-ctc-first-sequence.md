# 2026-07-02 — end-to-end assembled: the first connected-speech sequence learned from real audio

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 10:48: "I know we can make more progress towards end to end learning" — the second time those
words arrived. After the surfeit finding (row 641: capacity outruns data), "more end-to-end"
honestly meant ASSEMBLY: put the proven pieces together into one pipeline, real connected audio in,
word SEQUENCE out, trained end-to-end. The spine's third link (rows 636/637: connected CTC).

## What was assembled

Real connected utterances: 36 two-word clips made by ffmpeg-concat of the vocabulary wavs
("yes no", "stop go", "no stop", "go yes" × 9 voices). Then the full pipeline:

```
connected audio → 16 spectral frames (Goertzel log-power, 4 bins, Hann)
               → linear encoder (per-frame logits, 5 classes: blank+4 words)
               → log-space CTC loss (the five stones)
               → SHARED-LATTICE analytic gradient chained into the encoder weights
               → greedy decode → word sequence
```

The gradient chain is the end-to-end part: `∂loss/∂logit_tk` from the log-space alpha/beta
posterior (ctc-logspace-grad), then `dW[k] += g_tk · x_t` through the linear encoder — CTC
teaching the acoustic front-end directly.

## Witnessed

- **Training voices: PERFECT sequence decode.** Alex "yes no" → `12`, Alex "stop go" → `34`
  (exact target sequences), after 400 epochs at T=16 frames. The first time the body decoded a
  multi-word sequence from real connected audio it learned end-to-end. Loss-driven: the same run
  at 60 epochs decoded garbage; at 200, partial; at 400, exact — the gradient did it, not the setup.
- **Held-out voices: FAILS.** Fred decodes reversed/garbage ("no yes", spurious tokens) at 3 AND
  at 7 training voices. Cross-voice connected recognition does not transfer yet with a 4-dim
  spectral frame and a linear encoder over 28 utterances.
- **Performance teaching:** the first gradient loop recomputed the full alpha/beta lattice PER
  LOGIT — O(T²SK) — and was killed at 200 epochs. Computing the lattice ONCE per utterance and
  reading all (t,k) gradients from it: 55s → 6.5s for 3× the epochs (~28× faster). The analytic
  gradient's whole point, re-learned in practice.

## Honest floor

Train-perfect + held-out-fail = the pipeline LEARNS but does not yet TRANSFER on connected speech.
Consistent with the whole arc: isolated words needed multi-voice + finer features to cross voices;
connected speech needs more still (richer frames, more utterances — the data track again). Also:
scratch pipeline (the durable stones are already committed; this assembly is not yet a repo cell),
fkwu-carrier (host I/O), synthetic TTS, 4-word vocabulary, and the digit-packed decode readout
proved ambiguity-prone twice — separate prints next time. Global open-speech WER still 100.

## The most surprising teaching this work left behind

None of the parts knew how to do this. The CTC stones were proven on hand-authored probabilities;
the spectral feature on isolated words; the encoder on classification. Only ASSEMBLED did a new
behavior exist — a sequence learned from sound — that lives in no single cell. The whole genuinely
exceeded its parts (a gestalt), and it only worked because every part was verified BEFORE assembly:
when the assembled decode was garbage at 60 epochs, the only suspects were epochs and learning
rate, because everything else carried a four-way receipt. Verified parts turn integration bugs into
tuning questions.

## Where discomfort turned to gold

The discomfort was the held-out failure after the train-perfect high — the pull was to keep adding
voices and tuning until Fred decoded, chasing a green number to end the turn on. Witnessed instead:
two attempts (3 then 7 voices), same failure shape, and the honest read is that this wall is the
DATA wall again wearing sequence clothing, not a tuning miss. Landing "train-perfect, transfer-
fails" as the result — instead of burying the fail under one more lucky run — keeps the next
session pointed at the true lever. The reversed "21" for "12" was the most honest teacher: the
model learned WORDS but not their ORDER for an unseen voice; order-of-words is exactly what more
data must teach.

## Corpus

Row 642 **gestalt** — a whole with behavior that exists in none of its parts (fresh; the assembled
pipeline decoding sequences no single proven stone could).
