# Receipt -- speech native neural pair window 0026 (2026-07-01)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `fr<->zh`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- French row: `je suis` heard as `Je suis...`, WER `0`, wav bytes `32292`, cksum `3056443609`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- Total observed pair-window wav bytes: `47356`

Form movement:

- Added `learn/speech-native-neural-pair-window-0026.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0026-band.fk`.
- Trained neural pairs move `25 -> 26`.
- Directed neural routes move `50 -> 52`.
- Native neural parameters move `25 -> 26`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0026.fk \
    learn/tests/speech-native-neural-pair-window-0026-band.fk > /tmp/speech-native-neural-pair-window-0026.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0026.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
