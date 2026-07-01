# Speech global promotion readiness

This receipt adds an executable promotion gate for moving scoped ASR/TTS trial
wins into global live authority.

Witness:

```sh
cat learn/speech-model-metrics-report.fk \
    learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/speech-open-asr-trial-window-0002.fk \
    learn/speech-open-asr-trial-window-0003.fk \
    learn/sema-voice-trial-window.fk \
    learn/speech-current-status-ledger.fk \
    learn/speech-global-promotion-readiness.fk \
    learn/tests/speech-global-promotion-readiness-band.fk > /tmp/speech-global-promotion-readiness.fk
./fkwu --src /tmp/speech-global-promotion-readiness.fk
# 32767
```

Promotion rule:

- Each live lane needs `3` real live native receipts.
- WER must be `<= 25`.
- Controls must be clean: `choice`, `cut`, `fail`, `undo`, `timeout`.
- Scoped native windows count as training evidence, not global live authority.

Current readiness:

- Open dictation: scoped native `6/6`, real live native `0/3`, missing `3`, global route `oracle-guide`.
- Sema live voice: scoped native `1/1`, real live native `0/3`, missing `3`, global route `oracle-guide`.
- Aggregate: `0` global-native-ready lanes, `2` oracle-guided lanes, `6` missing real live receipts.

This keeps trust precise: the native challengers are winning bounded trials, but
global live authority waits for repeated real metal receipts.
