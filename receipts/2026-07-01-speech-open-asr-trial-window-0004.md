# Receipt -- speech open-ASR source trial window 0004 (2026-07-01)

This patch extends the scoped native segmented source-ASR lane with a real
de/pt-br loopback pair observed on the local Mac.

Observed carrier:

- Device: `macos-arm64-m4-max`
- Oracle: `whisper.cpp-large-v3-turbo-metal`
- Render path: `say -> ffmpeg -> whisper-cli`
- German: `Flo (German (Germany))`, truth `ich bin`, heard `Ich bin...`,
  wav bytes `29216`, audio hash `108967973`
- Portuguese Brazil: `Flo (Portuguese (Brazil))`, truth `eu sou`, heard
  `Eu sou.`, wav bytes `31774`, audio hash `1996659987`

Form movement:

- Added `learn/speech-open-asr-trial-window-0004.fk`.
- Added `learn/tests/speech-open-asr-trial-window-0004-band.fk`.
- Scoped open-ASR source trial moves from `6/6` to `8/8`.
- Combined scoped speech trials move from `8/8` to `10/10`.
- Live wav rows move from `213` to `215`.
- Observed wav bytes move from `7260820` to `7321810`.
- Global live open-ASR authority remains `native 0/4`; this is not a live
  microphone promotion.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/speech-open-asr-trial-window-0004.fk \
    learn/tests/speech-open-asr-trial-window-0004-band.fk > /tmp/speech-open-asr-trial-window-0004.fk
./fkwu --src /tmp/speech-open-asr-trial-window-0004.fk
# 32767
```

Boundary:

This is a real observed oracle-to-native training row for the scoped segmented
source path. It still does not satisfy the `3` clean real live native receipt
threshold required to promote global open-ASR authority.
