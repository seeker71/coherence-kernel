# Receipt -- speech native neural pair window 0030 (2026-07-02)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `zh<->id`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- Indonesian row: `aku ada` heard as `Aku ada.`, WER `0`, wav bytes `23712`, cksum `1805192547`
- Total observed pair-window wav bytes: `38776`

Form movement:

- Added `learn/speech-native-neural-pair-window-0030.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0030-band.fk`.
- Trained neural pairs move `29 -> 30`.
- Directed neural routes move `58 -> 60`.
- Native neural parameters move `29 -> 30`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.
- Next background pair scheduler advances to `0031 en<->it`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0030.fk \
    learn/tests/speech-native-neural-pair-window-0030-band.fk > /tmp/speech-native-neural-pair-window-0030.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0030.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
