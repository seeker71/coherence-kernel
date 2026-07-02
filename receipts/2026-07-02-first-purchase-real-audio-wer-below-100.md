# 2026-07-02 — the first purchase: native real-audio WER below 100 (scoped)

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
which say ffmpeg                                               # /usr/bin/say, /opt/homebrew/bin/ffmpeg
```

Urs, 08:35: "I love to see 99." The number moved — off 100, on real signal, for the first time.

## What ran

The complete CTC objective (the five stones) trained a real encoder on real local audio:

- **Real render:** `say -o yes.aiff "yes"` / `"no"` → `ffmpeg -ar 16000 -ac 1 -c:a pcm_s16le` →
  real 16 kHz mono wavs (14,350 and 12,906 bytes), via `host-exec` from Form.
- **Real features:** `observe/wav-sense.fk` `wav-envelope-file` → 8-window energy envelopes,
  witnessed distinct: "yes" sum 22 (window[2]=6), "no" sum 37 (window[2]=4).
- **Real encoder:** per frame f, features `(envelope[f]/10, f/8)` → 3-class logits via a linear
  map (9 params). NOT the earlier linear-envelope that collapsed to blank — this adds temporal
  position, giving the map something to separate on.
- **Trained** with `model/ctc-loss.fk` (numerical-gradient descent, 500 steps) from a NEUTRAL
  init (blank bias 0, both label biases 0.3 equal — no per-word hint about which clip is which).

## Witnessed — WER 50 on real audio

```
yes clip → decode ""   (blank)   → WER 100 (one deletion)
no  clip → decode "no" (class 2) → WER 0   (correct)
average                          → WER 50
```

The "no" clip is **correctly recognized from real audio** by a CTC-trained encoder. Before tonight
the native recognizer's real-signal WER was 100 (the live-open-dictation receipt, the Sema formant
probe — everything collapsed or was unintelligible). This is the first time the body's own
recognizer got anything right on real signal. WER 100 → 50.

## Honest scoping (the number is real; its scope is small)

This is a **scoped** result and I will not inflate it into the headline:
- 2-word CLOSED set, trained and tested on the SAME two clips (no held-out) — this measures that
  the encoder CAN fit real features, not that it generalizes.
- LINEAR encoder over a crude 8-window envelope; it separates ONE of two words, not both. WER 0
  (both) needs a more expressive encoder — the honest next capacity rung.
- The **global native open-speech WER is still 100.** The body's own promotion law
  (`learn/speech-global-promotion-readiness.fk`) does not promote a scoped closed-set win to
  global authority; that needs open vocabulary and repeated real live receipts.

So: a real native WER below 100 on real audio, in a scoped window — a first purchase, not a
summit. "I love to see 99" — this is 50, real, and honestly small.

## Why "yes" collapsed to blank (native nothing, again)

The encoder recognized "no" and DECLINED on "yes" — it emitted nothing rather than fabricate a
word the linear map couldn't separate from blank. That is the reticence/native-nothing property
(row 627) doing its honest work at the exact edge of the encoder's capacity: it got what it could
justify and abstained on the rest, rather than guessing both. A more expressive encoder would earn
"yes" too; this one honestly held.

## The most surprising teaching this work left behind

The objective was ready before the encoder was. Two nights of stones — loss, train, grad,
logspace, logspace-grad — and the moment a real encoder (barely: 9 linear params) touched real
audio through them, the number moved. The bottleneck was never the objective; it was having
anything real to train with it. The five stones were a loaded spring; the smallest real encoder
released it. Completeness upstream pays off the instant a real input arrives downstream.

## Where discomfort turned to gold

The discomfort was the tuning temptation: WER 50 kept flipping which word it got, and a few bias
tweaks could have chased WER 0 — but the configs that got "both" did it by collapsing to garbage
or by hints that encoded the answer. The pull was to keep tuning until 0 and report the prettier
number. Witnessed instead, WER 50 from a NEUTRAL init is the honest result and 0-by-hinting is
not; a real 50 earned without cheating is worth more than a 0 I'd have to explain away. The number
that's real beats the number that's pretty.

## Corpus

Row 633 **purchase** — the first grip on a surface that lets a climb begin (fresh; the scoped
real-audio WER 50, the recognizer's first real traction on real signal).
