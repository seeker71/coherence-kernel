# 2026-06-30 -- speech model AutoML selector

## What Changed

Added `learn/speech-model-auto-selection.fk`.

The selector makes model choice explicit:

```text
ASR:        prototype-asr                    nearest-l1-wav-feature-prototype
TTS:        formant-vocoder                  source-filter-formant-frames
NL2NL:      closed-set-locale-form           token-overlap-to-neutral-meaning
Audio2Audio prototype-asr-formant-audio2audio ASR -> neutral Form -> formant target
```

It also keeps unselected candidates visible:

```text
small-transformer-nl      trainable, not live-selected for speech yet
diffusion-codec-speech    candidate only, no native executable kernel receipt yet
macos-say-whisper-oracle  local teacher/carrier, not native authority
```

## Witness

```sh
cat form/form-stdlib/somatic-coherence-loop.fk \
    form/form-stdlib/form-cli-router.fk \
    form/form-stdlib/form-cli-sufficiency.fk \
    form/form-stdlib/observed-auto-learning.fk \
    learn/speech-model-auto-selection.fk \
    learn/tests/speech-model-auto-selection-band.fk > /tmp/speech-model-auto-selection-band.fk
./fkwu --src /tmp/speech-model-auto-selection-band.fk
```

Output:

```text
4095
```

## Answer To The Model Question

We are not using diffusion today.

We are training the native speech path with closed-set Form learners: nearest-prototype ASR over Form-read wav
features, neutral Form meaning rows, and a deterministic source-filter formant vocoder. The auto-research layer
keeps transformer and diffusion/codec candidates visible; the AutoML layer selects the current winners from
receipt-backed scores and only promotes challengers through live reversible A/B evidence with fail/undo/timeout
controls. The score penalizes latency, cost, and uncertainty; it does not treat uncertainty as fear.

## Honest Boundary

The stronger native neural ASR/vocoder is still the next target. It should win by adding an executable Form
micro-kernel and beating the current closed-set route in the same observed AutoML window, not by replacing the
receipt with an assertion.
