# Receipt -- speech native neural pair window 0023 (2026-07-01)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `fr<->pt-br`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- French row: `je suis` heard as `Je suis...`, WER `0`, wav bytes `32292`
- Portuguese Brazil row: `eu sou` heard as `Eu sou.`, WER `0`, wav bytes `31774`
- Total observed pair-window wav bytes: `64066`

Form movement:

- Added `learn/speech-native-neural-pair-window-0023.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0023-band.fk`.
- Trained neural pairs move `22 -> 23`.
- Directed neural routes move `44 -> 46`.
- Native neural parameters move `22 -> 23`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0023.fk \
    learn/tests/speech-native-neural-pair-window-0023-band.fk > /tmp/speech-native-neural-pair-window-0023.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0023.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
