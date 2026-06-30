# 2026-06-30 -- fourth metal live pair anchor

## What Changed

Added `presence/macos-en-fr-speech-roundtrip-variant.fk` and updated
`learn/metal-live-pair-anchors.fk`.

The base macOS carrier still owns the live local TTS -> wav -> Whisper Metal
oracle -> Form wav feature loop. The new variant loads after it and overrides
only the locale pair, prompts, and voices:

```text
pair: en <-> fr
train voices: Samantha / Jacques
eval voices: Eddy English US / Eddy French France
prompts:
  The sky is blue.      <-> Le ciel est bleu.
  The house is calm.    <-> La maison est calme.
  Thank you everyone.   <-> Merci tout le monde.
```

## Live Metal Witness

The live variant returned:

```text
masr-run -> 511
base-field-code -> 12100000000000000
field-code -> 12101008301000066
```

The base field code carries:

```text
count=12
oracle_ok=10
native_ok=0
native_rate=0
A->B rate=0
B->A rate=0
```

The trained field code carries:

```text
count=12
oracle_ok=10
native_ok=10
native_rate=83
A->B rate=100
B->A rate=66
```

## Anchor Set Witness

```sh
cat learn/metal-observed-sweep-bridge.fk \
    learn/metal-live-pair-anchors.fk \
    learn/tests/metal-live-pair-anchors-band.fk > /tmp/metal-live-pair-anchors.fk
./fkwu --src /tmp/metal-live-pair-anchors.fk
```

Output:

```text
32767
```

## Honest Boundary

The live Metal anchor count is now `4/5`: `en<->de`, `en<->es`, `en<->id`,
and `en<->fr`. The route remains `metal-anchored-native-guide`. One more live
reciprocal pair anchor is still needed before `full-metal-native` is an honest
claim.

The French prompt text is ASCII on purpose. The current live carrier's WER
tokenizer is byte/ASCII oriented, so accented and non-Latin scripts need a
native Unicode token lane before Chinese, Arabic, or accent-rich French anchors
can be measured honestly.
