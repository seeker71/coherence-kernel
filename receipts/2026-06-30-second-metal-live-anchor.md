# 2026-06-30 -- second metal live pair anchor

## What Changed

Added `presence/macos-en-es-speech-roundtrip-variant.fk` and
`learn/metal-live-pair-anchors.fk`.

The base macOS carrier still owns the live local TTS -> wav -> Whisper Metal
oracle -> Form wav feature loop. The new variant loads after it and overrides
only the locale pair, prompts, and voices:

```text
pair: en <-> es
train voices: Samantha / Paulina
eval voices: Eddy English US / Eddy Spanish Spain
prompts:
  Peace for every world. <-> Paz para cada mundo.
  Truth alone wins.      <-> Solo la verdad gana.
  Everyone is happy.     <-> Todos son felices.
```

## Live Metal Witness

The live variant returned:

```text
masr-run -> 511
route-shift composition -> 1012120012010001
field-code -> 12121210001000100
```

The route-shift code carries:

```text
shifted=1
count=12
oracle_ok=12
before_native=0
after_native=12
before_rate=0
after_rate=100
```

The field code carries:

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

The live Metal anchor count is now `2/5`: `en<->de` and `en<->es`.
The route remains `metal-anchored-native-guide`. Three more live reciprocal
pair anchors are still needed before `full-metal-native` is an honest claim.
