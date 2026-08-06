# Live Open-ASR Source Authority

Date: 2026-06-30

The live open-dictation Metal receipt now changes the source-ASR algorithm
instead of remaining a boundary note:

```text
open dictation rows: 4
local oracle successes: 4
native open-ASR successes: 0
oracle WER: 0
native WER: 100
action: train-live-segmented-open-asr-source
candidate: native-segmented-acoustic-learning
authority: oracle-guide
```

The route does not promote native open microphone authority. It names the next
training action for the segmented source path: local oracle text teaches
segmented acoustic token prototypes, which lower through native CTC into open
dictation receipts.

## Witness

```sh
cat learn/live-open-asr-source-authority.fk \
    learn/tests/live-open-asr-source-authority-band.fk \
  > /tmp/live-open-asr-source-authority.fk
./fkwu --src /tmp/live-open-asr-source-authority.fk
```

Observed:

```text
32767
```

Selector witness:

```text
speech-model-auto-selection-band -> 67108863
```

The selector sees `live-open-asr-source-authority` as a live, metal-lowered,
trainable ASR candidate, but `prototype-asr` remains selected until native
open-ASR transcript evidence wins.

## Boundary

This is a learning action, not a promotion. Native open-ASR source authority
requires a local native transcript candidate to pass the same WER, consent,
audio, and control gates.
