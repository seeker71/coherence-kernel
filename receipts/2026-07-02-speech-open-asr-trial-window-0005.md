# Receipt -- speech open-ASR trial window 0005 (2026-07-02)

This patch adds a fifth scoped open-ASR source window using the scheduler's
background `zh<->es` pair over Sanskrit baseline meaning `303`, `aham-asmi`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- Spanish row: `yo soy` heard as `Yo soy.`, WER `0`, wav bytes `26646`, cksum `2735625505`
- Total observed pair-window wav bytes: `41710`

Form movement:

- Added `learn/speech-open-asr-trial-window-0005.fk`.
- Added `learn/tests/speech-open-asr-trial-window-0005-band.fk`.
- Scoped open-ASR trial rows move `8 -> 10`.
- Scoped local oracle and native segmented source rows move `8/8 -> 10/10`.
- Global live open dictation remains oracle `4/4`, native `0/4`, route `oracle-guide`.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/speech-open-asr-trial-window-0004.fk \
    learn/speech-open-asr-trial-window-0005.fk \
    learn/tests/speech-open-asr-trial-window-0005-band.fk > /tmp/speech-open-asr-trial-window-0005.fk
./fkwu --src /tmp/speech-open-asr-trial-window-0005.fk
# 32767
```

Boundary:

This grows the scoped native segmented source learner. It is not global open
ASR authority and does not promote Sema live voice.
