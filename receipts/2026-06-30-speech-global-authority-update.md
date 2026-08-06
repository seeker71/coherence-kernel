# Speech global authority update

This receipt adds the route update law that consumes real live receipt rows and
decides whether global speech authority moves.

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/sema-voice-trial-window.fk \
    learn/sema-voice-trial-window-0002.fk \
    learn/speech-current-status-ledger.fk \
    learn/speech-global-promotion-readiness.fk \
    learn/speech-live-receipt-intake.fk \
    learn/speech-global-authority-update.fk \
    learn/tests/speech-global-authority-update-band.fk > /tmp/speech-global-authority-update.fk
./fkwu --src /tmp/speech-global-authority-update.fk
# 32767
```

Current input remains empty:

- Open dictation: `0/3 -> oracle-guide`.
- Sema live voice: `0/3 -> oracle-guide`.
- Missing real live receipts: `6`.

Demo clean input proves the cutover law:

- Open dictation: `3/3 -> native-open-asr-source`.
- Sema live voice: `3/3 -> native-sema-voice`.
- Missing real live receipts: `0`.

Thresholds remain WER `<= 25`, confidence `>= 80`, latency `<= 2000 ms`, and
clean `choice/cut/fail/undo/timeout`. This is still not a claim that current
global authority moved; it is the executable route update that will move it when
real carriers emit the required receipts.
