# Receipt -- speech native neural pair window 0028 (2026-07-02)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `zh<->de`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- German row: `ich bin` heard as `Ich bin`, WER `0`, wav bytes `29216`, cksum `2691094203`
- Total observed pair-window wav bytes: `44280`

Form movement:

- Added `learn/speech-native-neural-pair-window-0028.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0028-band.fk`.
- Trained neural pairs move `27 -> 28`.
- Directed neural routes move `54 -> 56`.
- Native neural parameters move `27 -> 28`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0028.fk \
    learn/tests/speech-native-neural-pair-window-0028-band.fk > /tmp/speech-native-neural-pair-window-0028.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0028.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
