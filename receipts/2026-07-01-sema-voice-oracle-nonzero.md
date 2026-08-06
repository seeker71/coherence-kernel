# Receipt — Sema voice oracle is no longer zero (2026-07-01)

Problem named: `sema-live-voice` still surfaced as native/oracle zero on the
live authority path. That made the voice lane look unwitnessed even though the
local Metal teacher sample had already cleared Whisper with WER 0.

Correction:

- `learn/speech-oracle-native-backlog.fk` now records Sema live voice local
  oracle as `1/1 = 100%`, WER `0`.
- Native Sema voice authority remains held at `0/1 = 0%`, native WER `100`.
- `learn/speech-open-asr-tts-target-model.fk` now mirrors the same live voice
  split: oracle `1/1`, native `0/1`.
- The training floor is explicit: oracle `3/3`, native `2/3`; that floor does
  not promote global native Sema voice.
- `learn/speech-current-status-ledger.fk` snapshots global live authority as
  oracle `5/5`, native `0/5`.

Witnessed source:

- `presence/macos-sema-voice-teacher-carrier.fk`
- `learn/sema-voice-teacher-oracle-intake.fk`
- audio hash `497318870`
- local oracle `whisper.cpp-large-v3-turbo-metal`
- device `macos-arm64-m4-max`
- truth `Open speech flows.`
- WER `0`

Gates:

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
( cat observe/native-vs-rented.fk; printf '\n(native-vs-rented-check)\n' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk

( cat learn/speech-oracle-native-backlog.fk learn/tests/speech-oracle-native-backlog-band.fk ) > /tmp/sonb.fk
./fkwu --src /tmp/sonb.fk

( cat learn/speech-oracle-native-backlog.fk learn/speech-next-trial-scheduler.fk learn/sema-voice-trial-window.fk learn/sema-voice-trial-window-0002.fk learn/sema-voice-authority-floor.fk learn/tests/sema-voice-authority-floor-band.fk ) > /tmp/svaf.fk
./fkwu --src /tmp/svaf.fk

( cat learn/speech-open-asr-tts-target-model.fk learn/tests/speech-open-asr-tts-target-model-band.fk ) > /tmp/soatm.fk
./fkwu --src /tmp/soatm.fk

( cat learn/speech-neural-pair-coverage.fk learn/speech-pair-training-next-action.fk learn/speech-open-asr-tts-target-model.fk learn/speech-oracle-native-backlog.fk learn/speech-next-trial-scheduler.fk learn/speech-authority-learning-priority.fk learn/tests/speech-authority-learning-priority-band.fk ) > /tmp/salp.fk
./fkwu --src /tmp/salp.fk

( cat learn/speech-model-metrics-report.fk learn/speech-oracle-native-backlog.fk learn/speech-next-trial-scheduler.fk learn/speech-open-asr-trial-window.fk learn/speech-open-asr-trial-window-0002.fk learn/speech-open-asr-trial-window-0003.fk learn/sema-voice-trial-window.fk learn/sema-voice-trial-window-0002.fk learn/sema-voice-authority-floor.fk learn/speech-corpus-training-intake-0001.fk learn/speech-current-status-ledger.fk learn/tests/speech-current-status-ledger-band.fk ) > /tmp/smcl.fk
./fkwu --src /tmp/smcl.fk
```

Expected outputs: `42`, `55`, `11111`, then `32767` for each focused band.

Boundary:

This fixes the zero oracle numerator. It does not claim native Sema voice. The
next real movement is still to render another native Sema sample and make the
native voice pass the same local oracle bar.
