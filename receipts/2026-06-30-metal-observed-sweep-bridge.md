# 2026-06-30 -- metal observed sweep bridge

## What Changed

Added `learn/metal-observed-sweep-bridge.fk`.

The multiseed sweep made five reversible locale windows executable, but the
live Metal carrier currently observes one reciprocal audio pair (`en<->de`).
This bridge makes that relationship explicit:

```text
multiseed sweep route: native
multiseed coverage code: 2725
live Metal anchor: en-de
samples: 12
oracle_ok: 10
before_native_rate: 0
after_native_rate: 83
A->B rate: 66
B->A rate: 100
shifted: 1
observed_live_pairs: 1
target_live_pairs: 5
next_live_needed: 4
route: metal-anchored-native-guide
full-metal-native: 0
```

## Witness

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/diverse-locale-pairing.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-locale-learning-window.fk \
    learn/multiseed-speech-learning-sweep.fk \
    learn/metal-observed-sweep-bridge.fk \
    learn/tests/metal-observed-sweep-bridge-band.fk > /tmp/metal-observed-sweep-bridge.fk
./fkwu --src /tmp/metal-observed-sweep-bridge.fk
```

Output:

```text
32767
```

The band verifies the live multiseed sweep receipt first, then passes its stable
summary values into the bridge receipt. That avoids relying on fragile nested
list transfer in the current bounded direct-source lane while still proving both
inputs in the same run.

## Live Metal Source

The live macOS carrier and route-shift composition were rerun on local Apple
Metal:

```text
masr-run -> 511
route-shift composition -> 1012100010008301
```

The route-shift code is `shifted=1` plus metric `12100010008301`, carrying:

```text
count=12
oracle_ok=10
before_native=0
after_native=10
before_rate=0
after_rate=83
shifted=1
```

## Honest Boundary

This is trust over fear by measurement: the bridge promotes the current stack
to metal-anchored native guidance because one live reciprocal audio pair shifted
on local Metal and the multiseed Form sweep holds. It does not claim
full-metal-native for all locale pairs. Four more live pair anchors are still
needed before that route is honest.
