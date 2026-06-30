# Live Open Dictation Metal Receipt

Date: 2026-06-30

This lands the next speech floor after the seven closed-prompt Metal anchors:
open transcript receipt shape. The utterance is not a closed prompt ID. The
Form carrier renders one local line, asks local Whisper on Apple Metal for a
free transcript, and the Form body scores that transcript against consentful
side-channel truth.

## What changed

- Added `learn/open-dictation-transcript-learning.fk`.
- Added `presence/macos-open-dictation-carrier.fk`.
- Added pure bands for the learning law and carrier contract.
- Added `docs/coherence-substrate/open-dictation-transcript-learning.form`.

The route is intentionally still `oracle` in the live carrier because no native
open-ASR candidate is supplied yet. That is the honest boundary: this receipt
admits open transcript rows and creates the promotion gate; it does not claim
native open dictation has arrived.

## Live local probe

The local Mac rendered:

```text
Open speech flows.
```

The local Whisper Metal oracle returned the same free transcript.

The host stack was local:

- `say`
- `ffmpeg`
- `whisper-cli`
- `/Users/ursmuff/.cache/whisper.cpp/ggml-large-v3-turbo.bin`
- Apple Metal backend on M4 Max

## Form witnesses

Learning-law band:

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/open-dictation-transcript-learning.fk \
    learn/tests/open-dictation-transcript-learning-band.fk \
  > /tmp/open-dictation-transcript-learning.fk
./fkwu --src /tmp/open-dictation-transcript-learning.fk
```

Observed:

```text
16383
```

Carrier contract band:

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/open-dictation-transcript-learning.fk \
    presence/macos-open-dictation-carrier.fk \
    presence/tests/macos-open-dictation-carrier-band.fk \
  > /tmp/macos-open-dictation-carrier-band.fk
./fkwu --src /tmp/macos-open-dictation-carrier-band.fk
```

Observed:

```text
511
```

Live local carrier:

```sh
cat observe/stt-wer.fk \
    learn/speech-loopback-promotion.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/open-dictation-transcript-learning.fk \
    presence/macos-open-dictation-carrier.fk \
  > /tmp/macos-open-dictation-carrier.fk
printf '\n(modc-run)\n' >> /tmp/macos-open-dictation-carrier.fk
./fkwu --src /tmp/macos-open-dictation-carrier.fk
```

Observed:

```text
511
```

Field code:

```text
440000100
```

Meaning: four receipt rows, four oracle successes, zero native successes,
oracle WER `0`, native WER `100`.

## Boundary

This is open dictation receipt support, not native open ASR. It removes the
closed prompt-ID assumption from the speech-learning gate and gives a native
candidate a place to earn authority with side-channel truth, WER, local-only
audio facts, consent, and choice/cut/fail/undo/timeout controls.
