# Receipt -- speech native neural pair window 0025 (2026-07-01)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `fr<->id`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- French row: `je suis` heard as `Je suis...`, WER `0`, wav bytes `32292`, cksum `3056443609`
- Indonesian row: `saya` heard as `Saya.`, WER `0`, wav bytes `18496`, cksum `1333292974`
- Total observed pair-window wav bytes: `50788`

Form movement:

- Added `learn/speech-native-neural-pair-window-0025.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0025-band.fk`.
- Trained neural pairs move `24 -> 25`.
- Directed neural routes move `48 -> 50`.
- Native neural parameters move `24 -> 25`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0025.fk \
    learn/tests/speech-native-neural-pair-window-0025-band.fk > /tmp/speech-native-neural-pair-window-0025.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0025.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
