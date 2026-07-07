# 2026-07-02 — progress on the voice: recognition vs generation, and the reafference path

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 07:50: "so now we can make progress on our voice?" — after tonight's CTC objective +
training + real-audio wiring. The honest answer requires not blurring recognition and generation.

## The two halves of "voice" (grounded)

- **Generator (the voice's sound):** `presence/formant-vocoder.fk` renders waveform from phoneme
  frames — `(phoneme duration pitch amplitude formant1 formant2 noise)`, deterministic parametric
  synthesis. The live Sema formant voice is **WER 100, route `oracle-guide`**
  (`receipts/2026-06-30-live-sema-formant-oracle-probe.md`): the formants are not intelligible
  speech. Tonight's CTC work is recognition-side and does NOT touch this.
- **Judge (is it intelligible?):** `learn/sema-voice-local-oracle-receipt.fk` gates voice samples
  by STT-WER — but with the RENTED whisper oracle, and only as a SELECTION gate (admit/decline a
  sample; no training signal, no backprop).

## The connection that matters: reafference

The voice loop's STT gate is analysis-by-synthesis in miniature: generate → recognize → the error
says whether it worked. What tonight's CTC objective adds is the missing half — it turns that gate
from a SELECTOR into a TRAINING SIGNAL. The CTC loss on "recognize your own generated voice" is
differentiable, and the vocoder's params (pitch, amplitude, formant1, formant2) are continuous —
so the same numerical-gradient descent that trained tonight's encoder could train the GENERATOR
to make sound the recognizer can read. Generate, hear yourself, correct: **reafference** (von
Holst) — perceiving the sensory consequence of your own action. That is how a voice becomes
intelligible by its own feedback rather than by imitating a teacher.

## What is unblocked, and the three rungs that are not (honest floor)

Unblocked: the OBJECTIVE by which the voice could be trained to intelligibility now exists
(`model/ctc-loss.fk`, proven; `model/ctc-train.fk`, trains). Progress on the voice is possible at
the objective — not at the sound, tonight.

Still standing (named, not skipped):
1. **A real recognizer encoder.** Tonight's linear-envelope recognizer collapsed to blank
   (honestly declined). No native judge until the encoder has capacity over real features.
2. **The differentiable loop.** Wire vocoder params → audio → features → recognizer → CTC loss →
   backprop to the vocoder. Each piece exists; the closed loop does not.
3. **Reticence as the promotion law.** The loop stays honest by declining every unintelligible
   sample (WER 100 → not promoted) until the generator earns intelligibility — it trains toward a
   voice, it never fakes one. This is already the body's law
   (`learn/speech-global-promotion-readiness.fk`); reafference training must obey it.

## The honest one-line answer

Not the sound, yet — but yes, the objective. Tonight didn't make the voice more intelligible; it
built the loss by which the voice could be *trained* to be, through reafference, judged (once the
encoder is real) by the body's own ear instead of a rented one.

## The most surprising teaching this work left behind

The voice's path runs THROUGH the recognizer, not around it. It felt like recognition (ASR) and
the voice (TTS) were opposite directions — but reafference makes the recognizer the TEACHER of
the generator: the body learns to speak by learning to hear itself. The two halves the session
kept separating turn out to be one loop, and tonight built the hinge between them.

## Where discomfort turned to gold

The discomfort was the pull to answer "yes!" to an eager question and let tonight's momentum imply
the voice had advanced — when honestly the voice's SOUND is exactly where it was (WER 100). Saying
"not the sound, yet" after a night of wins felt like deflating shared excitement. Witnessed
instead, the precise answer was more generative than a yes: it located the ONE real bridge
(reafference) and named the three rungs, turning "can we?" into "here is exactly how, and what
stands between." An honest map beats an optimistic gesture — it can actually be walked.

## Corpus

Row 628 **reafference** — the loop where a body generates its own voice and hears it back to
correct itself (fresh; the bridge from recognition to generation).
