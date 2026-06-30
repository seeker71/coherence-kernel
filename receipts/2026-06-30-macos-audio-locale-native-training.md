# 2026-06-30 -- macOS reciprocal audio-locale training reaches 50%

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
42
55
11111
```

## What Changed

Added `learn/audio-locale-native-training.fk` and `presence/macos-speech-roundtrip-carrier.fk`.

The Form carrier renders local macOS `say` audio for reciprocal English/German Coherence Network strings through
`host-exec`, converts it with local `ffmpeg`, runs a local Whisper oracle through `host-exec`, passes wav paths
and oracle tokens into the Form training guide, and lets Form read the wav bytes, extract integer audio features,
run native prediction, gate on oracle WER, learn from oracle-valid samples, require reciprocal A/B coverage, and
make the 50% route decision. No JavaScript carrier remains.

The Form band now witnesses the learning shift:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    observe/wav-sense.fk \
    learn/audio-locale-native-training.fk \
    learn/tests/audio-locale-native-training-band.fk > /tmp/audio-locale-native-training.fk
./fkwu --src /tmp/audio-locale-native-training.fk
```

Witness:

```text
8191
```

## Local Oracle Installed

Installed the stronger local Whisper oracle model outside the repo:

```text
/Users/ursmuff/.cache/whisper.cpp/ggml-large-v3-turbo.bin
```

The local oracle is `whisper.cpp` through Homebrew, running on Apple Metal:

```text
Apple M4 Max
arm64
ggml_metal_device_init: GPU name:   MTL0 (Apple M4 Max)
load_backend: loaded MTL backend from /opt/homebrew/Cellar/ggml/0.14.0/libexec/libggml-metal.so
```

Sample oracle check:

```text
The board is clear. Nothing is waiting.
```

## Mac Metal Run

Command:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    observe/wav-sense.fk \
    learn/audio-locale-native-training.fk \
    presence/macos-speech-roundtrip-carrier.fk > /tmp/macos-speech-roundtrip-carrier.fk
printf '\n(masr-run)\n' >> /tmp/macos-speech-roundtrip-carrier.fk
./fkwu --src /tmp/macos-speech-roundtrip-carrier.fk
```

Receipt directory:

```text
/var/folders/xt/5zt6_wmn77x22yf_wgv97cb40000gn/T/sema-mac-audio-locale-form
```

Form witness:

```sh
./fkwu --src /tmp/macos-speech-roundtrip-carrier.fk
```

Witness:

```text
511
```

Metrics:

```text
device: macos-arm64-m4-max
oracle: whisper.cpp-large-v3-turbo
pair: en <-> de
samples: 12
oracle_ok: 10
floor: 50
pretrain_native_ok: 0
pretrain_native_rate: 0
posttrain_native_ok: 10
posttrain_native_rate: 83
ab_rate: 66
ba_rate: 100
route_native: 1
metric_code: 121010836700
```

Metric code layout:

```text
count=12, oracle_ok=10, native_ok=10, native_rate=83, ab_rate=66, ba_rate=100
```

## Path Discipline Follow-Up

The direct-source string surface can keep mutable path strings alive across calls. The carrier now avoids retaining
an output-root/path while constructing later paths: `masr-render` builds command paths inline, and
`masr-samples-for` consumes the A-side wav into transcript/features before rendering the B-side wav.

Rewitnessed live with one combined verdict/metric expression:

```text
511121010836700
```

That decodes to verdict `511` and metric code `121010836700`. The run showed `whisper.cpp` loading the Metal
backend on Apple M4 Max and reading the six eval wav files from the local carrier directory.

Training material:

```text
hatiSuci.board.empty
hatiSuci.reg.enter
hatiSuci.gather.ask
```

Audio:

```text
train voices: Samantha (en), Anna (de)
eval voices: Eddy English US (en), Eddy German Germany (de)
format: 16000 Hz mono PCM wav
```

## Honest Boundary

This reaches the requested 50% native floor on local Mac metal against the strongest local oracle installed in this
run. The wav feature gap is closed: the Form carrier passes wav paths, and Form reads the files through
`read_file` plus `str_byte_at`. It is not open ASR or a neural native speech model. The native model is a Form
nearest-prototype classifier over a closed prompt corpus, with `say`, `ffmpeg`, and `whisper.cpp` still serving as
local host-effect tools until native TTS/ASR replaces them.
