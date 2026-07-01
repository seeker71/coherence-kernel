# Speech current status ledger

This receipt adds an executable ledger for the current speech model status. It
keeps two scopes separate:

- Global live authority: what can run as the broad live route today.
- Scoped trial windows: what has passed in bounded, receipt-backed trials.

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
    learn/tests/speech-current-status-ledger-band.fk > /tmp/speech-current-status-ledger.fk
./fkwu --src /tmp/speech-current-status-ledger.fk
# 32767
```

Current model size and composition:

- Native neural weight parameters admitted: `20`.
- Selected arms: `4`.
- Major Form components plus scoped trial windows: `22`.
- Native Sema voice organs/components: `6`.
- Scoped native trial windows: `4`.
- C seed growth: `0`.

Global live authority remains guarded:

- Open dictation: local oracle `4/4`, native `0/4`.
- Sema live voice: local oracle `0/1`, native `0/1`, WER `100`.
- Combined global live rows: oracle `4/5 = 80%`, native `0/5 = 0%`.

Scoped trial windows have moved:

- Open-ASR trial: oracle `6/6`, native `6/6`, native rate `100`.
- Sema voice TCAV trial: oracle `1/1`, native `1/1`, WER `0`.
- Combined scoped trials: oracle `7/7 = 100%`, native `7/7 = 100%`.

Voice quality target and scoped TCAV window agree on F0 `165`, warmth `82`,
cadence `64`, and breath `18`.
