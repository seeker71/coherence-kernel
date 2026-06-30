# 2026-06-30 -- third metal live pair anchor

## What Changed

Added `presence/macos-en-id-speech-roundtrip-variant.fk` and updated
`learn/metal-live-pair-anchors.fk`.

The base macOS carrier still owns the live local TTS -> wav -> Whisper Metal
oracle -> Form wav feature loop. The new variant loads after it and overrides
only the locale pair, prompts, and voices:

```text
pair: en <-> id
train voices: Samantha / Damayanti
eval voices: Eddy English US / Damayanti
prompts:
  Peace for every world. <-> Damai untuk setiap dunia.
  Truth alone wins.      <-> Kebenaran saja menang.
  Everyone is happy.     <-> Semua bahagia.
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

The live Metal anchor count is now `3/5`: `en<->de`, `en<->es`, and
`en<->id`. The route remains `metal-anchored-native-guide`. Two more live
reciprocal pair anchors are still needed before `full-metal-native` is an
honest claim.

The Indonesian side uses the same local macOS voice, Damayanti, for training
and evaluation on this device because no second Indonesian voice is installed.
That keeps the anchor real, but it is a weaker voice-generalization witness
than the English, German, and Spanish sides that have distinct train/eval
voices.
