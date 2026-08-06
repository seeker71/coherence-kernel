# Receipt -- speech native neural pair window 0022 (2026-07-01)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `es<->pt-br`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Spanish row: `yo soy` heard as `Yo soy.`, WER `0`, wav bytes `26646`
- Portuguese Brazil row: `eu sou` heard as `Eu sou.`, WER `0`, wav bytes `31774`
- Total observed pair-window wav bytes: `58420`

Form movement:

- Added `learn/speech-native-neural-pair-window-0022.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0022-band.fk`.
- Trained neural pairs move `21 -> 22`.
- Directed neural routes move `42 -> 44`.
- Native neural parameters move `21 -> 22`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0022.fk \
    learn/tests/speech-native-neural-pair-window-0022-band.fk > /tmp/speech-native-neural-pair-window-0022.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0022.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
