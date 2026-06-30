# Speech next-trial scheduler

This receipt adds the next executable step after the oracle/native backlog: a
Form-native scheduler that chooses which speech gap to exercise next and names
the evidence needed to promote the challenger.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/tests/speech-next-trial-scheduler-band.fk > /tmp/speech-next-trial-scheduler.fk
./fkwu --src /tmp/speech-next-trial-scheduler.fk
# 32767
```

Chosen next trial:

- Gap: live open dictation.
- Incumbent: `prototype-asr`.
- Challenger: `native-segmented-acoustic-learning`.
- Recipe: `segmented-open-asr-source-window-v1`.
- Source: oracle-passing live dictation window.
- Current rates: local oracle `4/4 = 100%`, native `0/4 = 0%`.
- Promotion rule: native rate at least `50`, WER at most `25`, and clean
  `choice/cut/fail/undo/timeout` controls.

Queued trial:

- Gap: Sema live voice.
- Incumbent: `sema-voice-sample-loop`.
- Challenger: `text-conditioned-acoustic-vocoder`.
- Recipe: `tcav-warm-mid-cadence-v1`.
- Current rates: local oracle `0/1 = 0%`, native `0/1 = 0%`, WER `100`.
- Promotion rule: first render a candidate that passes the local STT oracle
  with WER at most `25`, then let candidate search promote.

The model context stays honest: `0` admitted native neural weight parameters,
`2` pending neural-weight recipe lanes, `6` native Sema voice organs, `4`
selected arms, `14` major Form components, and `0` C seed growth.
