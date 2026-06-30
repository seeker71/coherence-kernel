# 2026-06-30 -- fifth metal live pair anchor

## What Changed

Added `presence/macos-en-it-speech-roundtrip-variant.fk` and updated
`learn/metal-live-pair-anchors.fk`.

The base macOS carrier still owns the live local TTS -> wav -> Whisper Metal
oracle -> Form wav feature loop. The new variant loads after it and overrides
only the locale pair, prompts, and voices:

```text
pair: en <-> it
train voices: Samantha / Alice
eval voices: Eddy English US / Eddy Italian Italy
prompts:
  The sun shines.     <-> Il sole brilla.
  The river runs.     <-> Il fiume scorre.
  Thank you friend.   <-> Grazie amico.
```

## Live Metal Witness

The live variant returned:

```text
masr-run -> 511
base-field-code -> 12120000000000000
field-code -> 12121210001000100
```

The base field code carries:

```text
count=12
oracle_ok=12
native_ok=0
native_rate=0
A->B rate=0
B->A rate=0
```

The trained field code carries:

```text
count=12
oracle_ok=12
native_ok=12
native_rate=100
A->B rate=100
B->A rate=100
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

The live Metal anchor count is now `5/5`: `en<->de`, `en<->es`, `en<->id`,
`en<->fr`, and `en<->it`. The route is now `full-metal-native` for the
closed-prompt local audio-locale carrier.

That scope matters. This does not claim open dictation, Unicode-script WER,
or native neural ASR/TTS. The body can now route the measured closed-prompt
audio loop natively on local Metal while those next climbs stay named.
