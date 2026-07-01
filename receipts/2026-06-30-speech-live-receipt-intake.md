# Speech live receipt intake

This receipt adds the live receipt intake law that the global promotion gate was
waiting for.

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/sema-voice-trial-window.fk \
    learn/speech-current-status-ledger.fk \
    learn/speech-global-promotion-readiness.fk \
    learn/speech-live-receipt-intake.fk \
    learn/tests/speech-live-receipt-intake-band.fk > /tmp/speech-live-receipt-intake.fk
./fkwu --src /tmp/speech-live-receipt-intake.fk
# 32767
```

The row admits only real live receipts with:

- local audio/oracle,
- consent,
- audio hash/sample-rate/channel evidence,
- clean `fail/timeout/undo`,
- oracle WER `<= 25`,
- native WER `<= 25`,
- confidence `>= 80`,
- latency `<= 2000 ms`.

Current real live receipt counts remain honest:

- Open dictation: `0`.
- Sema live voice: `0`.

The demo side proves the promotion law: three clean real live rows satisfy a
lane, while missing audio or fail controls earn no credit. The six missing real
live receipts are still missing; this adds the body-side intake that can count
them when carriers emit them.
