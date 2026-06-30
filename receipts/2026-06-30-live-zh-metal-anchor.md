# 2026-06-30 -- live zh Metal speech anchor

## What Changed

Added `presence/macos-en-zh-speech-roundtrip-variant.fk` as the sixth live
macOS Metal reciprocal speech pair and the first live Unicode-script audio
anchor.

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
    presence/macos-en-zh-speech-roundtrip-variant.fk > /tmp/macos-en-zh-speech-roundtrip.fk
printf '\n(masr-run)\n' >> /tmp/macos-en-zh-speech-roundtrip.fk
./fkwu --src /tmp/macos-en-zh-speech-roundtrip.fk
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
    presence/macos-en-zh-speech-roundtrip-variant.fk > /tmp/macos-en-zh-speech-roundtrip-metric.fk
printf '\n(masr-run-metric-code)\n' >> /tmp/macos-en-zh-speech-roundtrip-metric.fk
./fkwu --src /tmp/macos-en-zh-speech-roundtrip-metric.fk
```

Output:

```text
121010836700
```

Decoded:

```text
samples: 12
oracle-ok: 10
native-ok: 10
posttrain-native-rate: 83
A->B rate: 66
B->A rate: 100
route: native
```

Focused direction checks:

```text
alnt-rec-ab-rate -> 66
alnt-rec-ba-rate -> 100
alnt-rec-rate    -> 83
```

The local Whisper oracle ran on Apple Metal and reported `MTL0 (Apple M4 Max)`.

## Prompt Surface

```text
Peace for every world.   -> 愿所有世界安宁。
Truth alone wins.        -> 真理终将胜利。
Everyone is happy.       -> 大家都快乐。
```

Train voices:

```text
en: Samantha
zh: Tingting
```

Eval voices:

```text
en: Eddy (English (US))
zh: Eddy (Chinese (China mainland))
```

## Anchor Ledger

`learn/metal-live-pair-anchors.fk` now counts six live reciprocal anchors:

```text
en<->de
en<->es
en<->id
en<->fr
en<->it
en<->zh
```

The anchor band returns:

```text
32767
```

## Honest Boundary

This is live Unicode-script closed-prompt audio, not open dictation. Chinese
has crossed the local Metal carrier floor; Arabic remains pending on this Mac
because the installed voice set did not expose an Arabic `say` voice in the
checked host list. Native neural ASR/TTS is still pending.
