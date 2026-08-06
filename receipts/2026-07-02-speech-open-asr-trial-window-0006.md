# Receipt -- speech open-ASR trial window 0006 (2026-07-02)

This patch adds a sixth scoped open-ASR source window using the scheduler's
background `zh<->id` pair over Sanskrit baseline meaning `303`, `aham-asmi`.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- Chinese row: `我在` heard as `我在。`, WER `0`, wav bytes `15064`, cksum `3568659029`
- Indonesian row: `aku ada` heard as `Aku ada.`, WER `0`, wav bytes `23712`, cksum `1805192547`
- Total observed pair-window wav bytes: `38776`

Form movement:

- Added `learn/speech-open-asr-trial-window-0006.fk`.
- Added `learn/tests/speech-open-asr-trial-window-0006-band.fk`.
- Scoped open-ASR trial rows move `10 -> 12`.
- Scoped local oracle and native segmented source rows move `10/10 -> 12/12`.
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
    learn/tests/speech-open-asr-trial-window-0006-band.fk > /tmp/speech-open-asr-trial-window-0006.fk
./fkwu --src /tmp/speech-open-asr-trial-window-0006.fk
# 32767
```

Boundary:

This grows the scoped native segmented source learner. It is not global open
ASR authority and does not promote Sema live voice.
