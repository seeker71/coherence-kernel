# Speech open-ASR trial window

This receipt executes the next trial selected by
`learn/speech-next-trial-scheduler.fk`.

The scheduler chose live open dictation because the backlog has a useful
teacher signal: local oracle `4/4 = 100%`, native `0/4 = 0%`. This new cell
records the challenger over a small consentful segmented source window backed
by the Sanskrit/locale-neutral baseline. The full segmented learner keeps its
own witness; this receipt stays compact so it can compose with the scheduler
under the current direct-source ceiling.

Witness:

```sh
cat learn/speech-oracle-native-backlog.fk \
    learn/speech-next-trial-scheduler.fk \
    learn/speech-open-asr-trial-window.fk \
    learn/tests/speech-open-asr-trial-window-band.fk > /tmp/speech-open-asr-trial-window.fk
./fkwu --src /tmp/speech-open-asr-trial-window.fk
# 32767
```

Measured trial-window result:

- Trial: `trial-open-asr-0001`.
- Challenger: `native-segmented-acoustic-learning`.
- Locale pair: `en<->de`.
- Baseline meaning: `303`, `aham asmi / i am / ich bin`.
- Local oracle: `2/2`.
- Compact native trial receipt: `2/2`.
- Trial native rate: `100`, over the `50` promotion floor.
- Action: `cut-challenger-for-trial-window`.

This is a native Form trial-window receipt, intentionally scoped. It does not
claim global live open-ASR authority has moved. The broader backlog still needs
repeated real capture receipts before the live open-ASR row can leave
oracle-guide.
