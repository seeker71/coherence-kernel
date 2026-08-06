# Receipt -- speech native neural pair window 0029 (2026-07-02)

This patch trains the next native neural micro-pair window over the Sanskrit
baseline meaning `303`, `aham-asmi`, for `zh<->es`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- Spanish row: `yo soy` heard as `Yo soy.`, WER `0`, wav bytes `26644`, cksum `889626998`
- Total observed pair-window wav bytes: `41708`

Form movement:

- Added `learn/speech-native-neural-pair-window-0029.fk`.
- Added `learn/tests/speech-native-neural-pair-window-0029-band.fk`.
- Trained neural pairs move `28 -> 29`.
- Directed neural routes move `56 -> 58`.
- Native neural parameters move `28 -> 29`.
- Native Form NL/audio rates are `100%`.
- Local oracle passes `2/2`.

Witness:

```sh
cat observe/stt-wer.fk \
    observe/asr-prompt-id.fk \
    learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk \
    learn/speech-native-neural-bootstrap.fk \
    learn/speech-native-neural-pair-window-0029.fk \
    learn/tests/speech-native-neural-pair-window-0029-band.fk > /tmp/speech-native-neural-pair-window-0029.fk
./fkwu --src /tmp/speech-native-neural-pair-window-0029.fk
# 32767
```

Boundary:

This grows closed-pair native neural coverage. It is not global open ASR/TTS
authority and does not promote Sema live voice.
