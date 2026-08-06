# Receipt -- speech open-ASR trial window 0007 (2026-07-02)

This patch adds a seventh scoped open-ASR source window using the scheduler's
background `en<->it` pair over Sanskrit baseline meaning `303`, `aham-asmi`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- English row: `i am` heard as `I am.`, WER `0`, wav bytes `18170`, cksum `3569739330`
- Italian row: `io sono` heard as `Io sono`, WER `0`, wav bytes `16706`, cksum `3343634846`
- Total observed pair-window wav bytes: `34876`

Form movement:

- Added `learn/speech-open-asr-trial-window-0007.fk`.
- Added `learn/tests/speech-open-asr-trial-window-0007-band.fk`.
- Scoped open-ASR trial rows move `12 -> 14`.
- Scoped local oracle and native segmented source rows move `12/12 -> 14/14`.
- Global live open dictation remains oracle `4/4`, native `0/4`, route `oracle-guide`.
- Sema live voice remains non-empty and honest: oracle `1/1`, native `0/1`;
  the scoped voice training floor remains oracle `3/3`, native `2/3`.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/speech-open-asr-trial-window-0004.fk \
    learn/speech-open-asr-trial-window-0005.fk \
    learn/speech-open-asr-trial-window-0006.fk \
    learn/speech-open-asr-trial-window-0007.fk \
    learn/tests/speech-open-asr-trial-window-0007-band.fk > /tmp/speech-open-asr-trial-window-0007.fk
./fkwu --src /tmp/speech-open-asr-trial-window-0007.fk
# 32767
```

Boundary:

This grows the scoped native segmented source learner. It is not global open
ASR authority and does not promote Sema live voice.
