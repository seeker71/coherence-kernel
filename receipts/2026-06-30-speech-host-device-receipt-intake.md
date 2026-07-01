# Speech host/device receipt intake

Added a Form-native intake bridge from host/device observations to the existing
live receipt and global authority laws.

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/sema-voice-trial-window.fk \
    learn/speech-current-status-ledger.fk \
    learn/speech-global-promotion-readiness.fk \
    learn/speech-live-receipt-intake.fk \
    learn/speech-global-authority-update.fk \
    learn/speech-authority-model-selection.fk \
    learn/speech-host-device-receipt-intake.fk \
    learn/tests/speech-host-device-receipt-intake-band.fk > /tmp/speech-host-device-receipt-intake.fk
./fkwu --src /tmp/speech-host-device-receipt-intake.fk
```

Result:

```text
32767
```

Current observations:

- Android AAudio closed-prompt capture evidence is carried with audio hash
  `446225`, but it is training evidence only, not global open dictation.
- Current Sema live voice observation carries WER `100`, so it earns no native
  success credit.
- Android shared-device open dictation rows require `shared-safe=1`; a shared
  unsafe row is rejected from promotion.

Current global counts remain honest:

- Live open dictation: `0/3`.
- Sema live voice: `0/3`.
- Global speech-native ASR/TTS authority: `0/2`.
- Current observed global rows: open dictation `1`, Sema live voice `1`; both have `0` passing receipts, so this is not a `0/0` witness.
- Native neural parameters admitted: `0`.

The demo clean mixed Mac/Android rows prove the path without weakening it:

- `3/3` clean open dictation rows select `native-open-asr-source`.
- `3/3` clean Sema live voice rows select `native-sema-voice`.
- Model selection moves to `2/2` global speech-native authority only for those
  clean rows.

Boundary: `adb devices -l` showed no attached Android device in this shell when
this receipt was made, so this is not a fresh Android run claim. It brings the
device-metal observation membrane home so future local carrier rows can enter
the same authority path.
