# 2026-06-30 -- live ar Metal speech anchor

## What Changed

Added `presence/macos-en-ar-speech-roundtrip-variant.fk` as the seventh live
macOS Metal reciprocal speech pair and the second live Unicode-script audio
anchor after Chinese.

The carrier remains Form-owned: prompt rows, transcript tokenization, wav
feature extraction, receipt construction, training, and routing are Form. The
host tools are still local effects behind `host-exec`: macOS `say`, `ffmpeg`,
and `whisper-cli` using `ggml-large-v3-turbo`.

## Live Witness

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    observe/wav-sense.fk \
    learn/audio-locale-native-training.fk \
    presence/macos-speech-roundtrip-carrier.fk \
    presence/macos-en-ar-speech-roundtrip-variant.fk > /tmp/macos-en-ar-speech-roundtrip.fk
printf '\n(masr-run)\n' >> /tmp/macos-en-ar-speech-roundtrip.fk
./fkwu --src /tmp/macos-en-ar-speech-roundtrip.fk
```

Output:

```text
511
```

Metric command:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    observe/wav-sense.fk \
    learn/audio-locale-native-training.fk \
    presence/macos-speech-roundtrip-carrier.fk \
    presence/macos-en-ar-speech-roundtrip-variant.fk > /tmp/macos-en-ar-speech-roundtrip-metric.fk
printf '\n(masr-run-metric-code)\n' >> /tmp/macos-en-ar-speech-roundtrip-metric.fk
./fkwu --src /tmp/macos-en-ar-speech-roundtrip-metric.fk
```

Output:

```text
121213010100
```

The packed metric overflows two-digit fields when rates are `100`, so the
receipt fields were read directly:

```text
samples: 12
oracle-ok: 12
native-ok: 12
posttrain-native-rate: 100
A->B rate: 100
B->A rate: 100
route: native
```

The local Whisper oracle ran on Apple Metal and reported `MTL0 (Apple M4 Max)`.

## Prompt Surface

```text
Peace for every world.   -> سلام لكل العوالم.
Truth alone wins.        -> الحقيقة وحدها تنتصر.
Everyone is happy.       -> الجميع سعداء.
```

Train voices:

```text
en: Samantha
ar: Majed
```

Eval voices:

```text
en: Eddy (English (US))
ar: Majed
```

## Anchor Ledger

`learn/metal-live-pair-anchors.fk` now counts seven live reciprocal anchors:

```text
en<->de
en<->es
en<->id
en<->fr
en<->it
en<->zh
en<->ar
```

The anchor band returns:

```text
32767
```

## Honest Boundary

This is live Unicode-script closed-prompt audio, not open dictation. Chinese
and Arabic have now crossed the local Metal carrier floor. Native neural ASR/TTS
is still pending.
