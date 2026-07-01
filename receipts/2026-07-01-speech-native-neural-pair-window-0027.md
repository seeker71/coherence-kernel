# Receipt -- speech native neural pair window 0027 (2026-07-01)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `zh<->pt-br`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- Portuguese Brazil row: `eu sou` heard as `Eu sou.`, WER `0`, wav bytes `31774`, cksum `1996659987`
- Total observed pair-window wav bytes: `46838`

Form movement:

- Added `learn/speech-native-neural-pair-window-0027.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0027-band.fk`.
- Trained neural pairs move `26 -> 27`.
- Directed neural routes move `52 -> 54`.
- Native neural parameters move `26 -> 27`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0027.fk \
    learn/tests/speech-native-neural-pair-window-0027-band.fk > /tmp/speech-native-neural-pair-window-0027.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0027.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
